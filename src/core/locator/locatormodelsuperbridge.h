/***************************************************************************
  locatormodelsuperbridge.h

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

#ifndef LOCATORMODELSUPERBRIDGE_H
#define LOCATORMODELSUPERBRIDGE_H

#include "bookmarkmodel.h"
#include "navigation.h"

#include <QStandardItemModel>
#include <qgslocatorfilter.h>
#include <qgslocatormodelbridge.h>

class QgsQuickMapSettings;
class FeatureListExtentController;
class PeliasGeocoder;
class GnssPositionInformation;
class QgsLocator;

/**
 * LocatorActionsModel is a model used to dislay
 * additional actions from the result of a filter.
 * These are displayed as contextual menu in QGIS
 * while they are shown in a row of buttons in QField.
 */
class LocatorActionsModel : public QStandardItemModel
{
    Q_OBJECT
  public:
    enum ActionRoles
    {
      IdRole = Qt::UserRole + 1,
      IconPathRole
    };
    explicit LocatorActionsModel( QObject *parent = nullptr );
    LocatorActionsModel( int rows, int columns, QObject *parent = nullptr );
    QHash<int, QByteArray> roleNames() const override;
};


/**
 * LocatorModelSuperBridge reimplements QgsLocatorModelBridge
 *  for specific needs of QField / QML implementation.
 */
class LocatorModelSuperBridge : public QgsLocatorModelBridge
{
    Q_OBJECT
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QObject *locatorHighlightGeometry READ locatorHighlightGeometry WRITE setLocatorHighlightGeometry NOTIFY locatorHighlightGeometryChanged )
    Q_PROPERTY( FeatureListExtentController *featureListController READ featureListController WRITE setFeatureListController NOTIFY featureListControllerChanged )
    Q_PROPERTY( BookmarkModel *bookmarks READ bookmarks WRITE setBookmarks NOTIFY bookmarksChanged )
    Q_PROPERTY( Navigation *navigation READ navigation WRITE setNavigation NOTIFY navigationChanged )
    Q_PROPERTY( bool keepScale READ keepScale WRITE setKeepScale NOTIFY keepScaleChanged )

  public:
    explicit LocatorModelSuperBridge( QObject *parent = nullptr );
    ~LocatorModelSuperBridge() = default;

    QgsQuickMapSettings *mapSettings() const;
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    BookmarkModel *bookmarks() const;
    void setBookmarks( BookmarkModel *bookmarks );

    Navigation *navigation() const;
    void setNavigation( Navigation *navigation );

    QObject *locatorHighlightGeometry() const;
    void setLocatorHighlightGeometry( QObject *locatorHighlightGeometry );

    FeatureListExtentController *featureListController() const;
    void setFeatureListController( FeatureListExtentController *featureListController );

    bool keepScale() const;
    void setKeepScale( bool keepScale );

    Q_INVOKABLE LocatorActionsModel *contextMenuActionsModel( const int row );

    void emitMessage( const QString &text );

  signals:
    void mapSettingsChanged();
    void bookmarksChanged();
    void navigationChanged();
    void locatorHighlightGeometryChanged();
    void featureListControllerChanged();
    void messageEmitted( const QString &text );
    void keepScaleChanged();

  public slots:
    Q_INVOKABLE void triggerResultAtRow( const int row, const int id = -1 );

  private:
    QgsQuickMapSettings *mMapSettings = nullptr;
    QObject *mLocatorHighlightGeometry = nullptr;
    FeatureListExtentController *mFeatureListController = nullptr;
    bool mKeepScale = false;

    PeliasGeocoder *mFinlandGeocoder = nullptr;
    BookmarkModel *mBookmarks = nullptr;
    Navigation *mNavigation = nullptr;
};

class LocatorFiltersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( LocatorModelSuperBridge *locatorModelSuperBridge READ locatorModelSuperBridge WRITE setLocatorModelSuperBridge NOTIFY locatorModelSuperBridgeChanged )

  public:
    //! Custom model roles
    enum Role
    {
      NameRole = Qt::UserRole + 1,
      DescriptionRole,
      PrefixRole,
      ActiveRole,
      DefaultRole,
    };
    Q_ENUM( Role )

    /**
     * Constructor for QgsLocatorFiltersModel.
     */
    LocatorFiltersModel();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;

    QgsLocatorFilter *filterForIndex( const QModelIndex &index ) const;

    LocatorModelSuperBridge *locatorModelSuperBridge() const;
    void setLocatorModelSuperBridge( LocatorModelSuperBridge *locatorModelSuperBridge );

    Q_INVOKABLE void setGeocoderLocatorFiltersDefaulByPosition( const GnssPositionInformation &position );

  signals:

    void locatorModelSuperBridgeChanged();

  private:
    LocatorModelSuperBridge *mLocatorModelSuperBridge = nullptr;
};
#endif // LOCATORMODELSUPERBRIDGE_H
