/***************************************************************************
  locatormodelsuperbridge.cpp

 ---------------------
 begin                : 01.12.2018
 copyright            : (C) 2018 by Denis Rouzaud
 email                : denis@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "bookmarklocatorfilter.h"
#include "featurelistextentcontroller.h"
#include "featureslocatorfilter.h"
#include "finlandlocatorfilter.h"
#include "gnsspositioninformation.h"
#include "gotolocatorfilter.h"
#include "locatormodelsuperbridge.h"
#include "peliasgeocoder.h"
#include "qgsquickmapsettings.h"

#include <QStandardItem>
#include <qgslocator.h>
#include <qgslocatormodel.h>
#include <qgssettings.h>


LocatorModelSuperBridge::LocatorModelSuperBridge( QObject *parent )
  : QgsLocatorModelBridge( parent )
{
  locator()->registerFilter( new FeaturesLocatorFilter( this ) );
  locator()->registerFilter( new GotoLocatorFilter( this ) );
  locator()->registerFilter( new BookmarkLocatorFilter( this ) );

  // Finnish's Digitransit geocoder
  mFinlandGeocoder = new PeliasGeocoder( QStringLiteral( "https://api.digitransit.fi/geocoding/v1/search" ) );
  locator()->registerFilter( new FinlandLocatorFilter( mFinlandGeocoder, this ) );
}

Navigation *LocatorModelSuperBridge::navigation() const
{
  return mNavigation;
}

void LocatorModelSuperBridge::setNavigation( Navigation *navigation )
{
  if ( navigation == mNavigation )
    return;

  mNavigation = navigation;

  emit navigationChanged();
}

BookmarkModel *LocatorModelSuperBridge::bookmarks() const
{
  return mBookmarks;
}

void LocatorModelSuperBridge::setBookmarks( BookmarkModel *bookmarks )
{
  if ( bookmarks == mBookmarks )
    return;

  mBookmarks = bookmarks;

  emit bookmarksChanged();
}

QgsQuickMapSettings *LocatorModelSuperBridge::mapSettings() const
{
  return mMapSettings;
}

void LocatorModelSuperBridge::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mapSettings == mMapSettings )
    return;

  mMapSettings = mapSettings;

  updateCanvasExtent( mMapSettings->extent() );
  updateCanvasCrs( mMapSettings->destinationCrs() );

  connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, [=]() { updateCanvasExtent( mMapSettings->visibleExtent() ); } );
  connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, [=]() { updateCanvasCrs( mMapSettings->destinationCrs() ); } );

  emit mapSettingsChanged();
}

QObject *LocatorModelSuperBridge::locatorHighlightGeometry() const
{
  return mLocatorHighlightGeometry;
}

void LocatorModelSuperBridge::setLocatorHighlightGeometry( QObject *locatorHighlightGeometry )
{
  if ( locatorHighlightGeometry == mLocatorHighlightGeometry )
    return;

  mLocatorHighlightGeometry = locatorHighlightGeometry;
  emit locatorHighlightGeometryChanged();
}

FeatureListExtentController *LocatorModelSuperBridge::featureListController() const
{
  return mFeatureListController;
}

void LocatorModelSuperBridge::setFeatureListController( FeatureListExtentController *featureListController )
{
  if ( featureListController == mFeatureListController )
    return;

  mFeatureListController = featureListController;
  emit featureListControllerChanged();
}

bool LocatorModelSuperBridge::keepScale() const
{
  return mKeepScale;
}

void LocatorModelSuperBridge::setKeepScale( bool keepScale )
{
  if ( keepScale == mKeepScale )
    return;

  mKeepScale = keepScale;
  emit keepScaleChanged();
}

LocatorActionsModel *LocatorModelSuperBridge::contextMenuActionsModel( const int row )
{
  const QModelIndex index = proxyModel()->index( row, 0 );
  if ( !index.isValid() )
    return nullptr;

  const QList<QgsLocatorResult::ResultAction> actions = proxyModel()->data( index, QgsLocatorModel::ResultActionsRole ).value<QList<QgsLocatorResult::ResultAction>>();
  int r = 0;
  LocatorActionsModel *model = new LocatorActionsModel( actions.count(), 1 );
  for ( auto action : actions )
  {
    QStandardItem *item = new QStandardItem( action.text );
    item->setData( action.id, LocatorActionsModel::IdRole );
    item->setData( action.iconPath, LocatorActionsModel::IconPathRole );
    model->setItem( r, 0, item );
    r++;
  }

  return model;
}

void LocatorModelSuperBridge::emitMessage( const QString &text )
{
  emit messageEmitted( text );
}

void LocatorModelSuperBridge::triggerResultAtRow( const int row, const int id )
{
  const QModelIndex index = proxyModel()->index( row, 0 );
  if ( index.isValid() )
    triggerResult( index, id );
}

//
// LocatorActionsModel
//

LocatorActionsModel::LocatorActionsModel( QObject *parent )
  : QStandardItemModel( parent )
{
}

LocatorActionsModel::LocatorActionsModel( int rows, int columns, QObject *parent )
  : QStandardItemModel( rows, columns, parent )
{
}

QHash<int, QByteArray> LocatorActionsModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[IconPathRole] = "iconPath";
  roles[IdRole] = "id";
  return roles;
}

//
// LocatorFiltersModel
//

LocatorFiltersModel::LocatorFiltersModel()
  : QAbstractListModel()
{
}

int LocatorFiltersModel::rowCount( const QModelIndex &parent ) const
{
  if ( !mLocatorModelSuperBridge->locator() || parent.isValid() )
    return 0;

  return mLocatorModelSuperBridge->locator()->filters().count();
}

QHash<int, QByteArray> LocatorFiltersModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();

  roles[NameRole] = "Name";
  roles[DescriptionRole] = "Description";
  roles[PrefixRole] = "Prefix";
  roles[ActiveRole] = "Active";
  roles[DefaultRole] = "Default";

  return roles;
}

QVariant LocatorFiltersModel::data( const QModelIndex &index, int role ) const
{
  const static QMap<QString, QString> sLocatorFilterDescriptions = {
    { QStringLiteral( "allfeatures" ), tr( "Returns a list of features accross all searchable layers with matching attributes" ) },
    { QStringLiteral( "goto" ), tr( "Returns a point from a pair of X and Y coordinates typed in the search bar" ) },
    { QStringLiteral( "bookmarks" ), tr( "Returns a list of bookmark with matching names" ) },
    { QStringLiteral( "pelias-finland" ), tr( "Returns a list of locations and addresses within Finland with matching terms" ) } };

  if ( !mLocatorModelSuperBridge->locator() || !index.isValid() || index.parent().isValid() || index.row() < 0 || index.row() >= rowCount( QModelIndex() ) )
    return QVariant();

  switch ( role )
  {
    case Qt::DisplayRole:
    case NameRole:
      return filterForIndex( index )->displayName();

    case DescriptionRole:
      return sLocatorFilterDescriptions.value( filterForIndex( index )->name() );

    case PrefixRole:
      return filterForIndex( index )->activePrefix();

    case ActiveRole:
      return QVariant( filterForIndex( index )->enabled() );

    case DefaultRole:
      return filterForIndex( index )->useWithoutPrefix();
  }

  return QVariant();
}

bool LocatorFiltersModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  if ( !mLocatorModelSuperBridge->locator() || !index.isValid() || index.parent().isValid() || index.row() < 0 || index.row() >= rowCount( QModelIndex() ) )
    return false;

  switch ( role )
  {
    case NameRole:
    case PrefixRole:
      return false;

    case ActiveRole:
    {
      QgsLocatorFilter *filter = filterForIndex( index );
      const bool newValue = value.toBool();
      if ( filter->enabled() != newValue )
      {
        QgsSettings settings;
        filter->setEnabled( newValue );
        settings.setValue( QStringLiteral( "locator_filters/enabled_%1" ).arg( filter->name() ), newValue, QgsSettings::Section::Gui );
        emit dataChanged( index, index, QVector<int>() << ActiveRole );
        return true;
      }
      break;
    }

    case DefaultRole:
    {
      QgsLocatorFilter *filter = filterForIndex( index );
      const bool newValue = value.toBool();
      if ( filter->useWithoutPrefix() != newValue )
      {
        QgsSettings settings;
        filter->setUseWithoutPrefix( value.toBool() );
        settings.setValue( QStringLiteral( "locator_filters/default_%1" ).arg( filter->name() ), newValue, QgsSettings::Section::Gui );
        settings.setValue( QStringLiteral( "locator_filters/default_touched_%1" ).arg( filter->name() ), true, QgsSettings::Section::Gui );
        emit dataChanged( index, index, QVector<int>() << DefaultRole );
        return true;
      }
      break;
    }
  }

  return false;
}

void LocatorFiltersModel::setGeocoderLocatorFiltersDefaulByPosition( const GnssPositionInformation &position )
{
  if ( !mLocatorModelSuperBridge->locator() )
    return;

  QgsPointXY point( position.longitude(), position.latitude() );
  int i = 0;
  for ( QgsLocatorFilter *filter : mLocatorModelSuperBridge->locator()->filters() )
  {
    FinlandLocatorFilter *f = dynamic_cast<FinlandLocatorFilter *>( filter );
    if ( f )
    {
      if ( f->boundingBox().contains( point ) )
      {
        QgsSettings settings;
        bool filterTouched = settings.value( QStringLiteral( "locator_filters/default_touched_%1" ).arg( f->name() ), false, QgsSettings::Section::Gui ).toBool();
        bool filterDefault = settings.value( QStringLiteral( "locator_filters/default_%1" ).arg( f->name() ), false, QgsSettings::Section::Gui ).toBool();
        if ( !filterTouched && !filterDefault )
        {
          f->setUseWithoutPrefix( true );
          settings.setValue( QStringLiteral( "locator_filters/default_%1" ).arg( filter->name() ), true, QgsSettings::Section::Gui );
          QModelIndex modifiedIndex = index( i, 0 );
          emit dataChanged( modifiedIndex, modifiedIndex, QVector<int>() << DefaultRole );
          emit mLocatorModelSuperBridge->emitMessage( tr( "Search filters for your locations have been activated, customize results in the settings panel" ) );
        }
      }
    }
    i++;
  }
}

QgsLocatorFilter *LocatorFiltersModel::filterForIndex( const QModelIndex &index ) const
{
  if ( !mLocatorModelSuperBridge->locator() )
    return nullptr;

  return mLocatorModelSuperBridge->locator()->filters().at( index.row() );
}

LocatorModelSuperBridge *LocatorFiltersModel::locatorModelSuperBridge() const
{
  return mLocatorModelSuperBridge;
}

void LocatorFiltersModel::setLocatorModelSuperBridge( LocatorModelSuperBridge *locatorModelSuperBridge )
{
  if ( mLocatorModelSuperBridge == locatorModelSuperBridge )
    return;

  emit beginResetModel();
  mLocatorModelSuperBridge = locatorModelSuperBridge;
  emit locatorModelSuperBridgeChanged();
  emit endResetModel();
}
