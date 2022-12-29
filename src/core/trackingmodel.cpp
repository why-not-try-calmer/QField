/***************************************************************************
 trackingmodel.cpp - TrackingModel

 ---------------------
 begin                : 20.02.2020
 copyright            : (C) 2020 by David Signer
 email                : david (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trackingmodel.h"

TrackingModel::TrackingModel( QObject *parent )
  : QAbstractItemModel( parent )
{
}

TrackingModel::~TrackingModel()
{
  qDeleteAll( mTrackers );
}

QHash<int, QByteArray> TrackingModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

  roles[DisplayString] = "displayString";
  roles[VectorLayer] = "vectorLayer";
  roles[TimeInterval] = "timeInterval";
  roles[MinimumDistance] = "minimumDistance";
  roles[Conjunction] = "conjunction";
  roles[Feature] = "feature";
  roles[RubberModel] = "rubberModel";
  roles[Visible] = "visible";
  roles[StartPositionTimestamp] = "startPositionTimestamp";
  roles[MeasureType] = "measureType";

  return roles;
}

QModelIndex TrackingModel::index( int row, int column, const QModelIndex &parent ) const
{
  Q_UNUSED( column )
  Q_UNUSED( parent )

  return createIndex( row, 0, 1000 );
}

QModelIndex TrackingModel::parent( const QModelIndex &index ) const
{
  Q_UNUSED( index )

  return QModelIndex();
}

int TrackingModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mTrackers.size();
}

int TrackingModel::columnCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return 1;
}

QVariant TrackingModel::data( const QModelIndex &index, int role ) const
{
  switch ( role )
  {
    case DisplayString:
      return QString( "Tracker on layer %1" ).arg( mTrackers.at( index.row() )->layer()->name() );
    case VectorLayer:
      return QVariant::fromValue<QgsVectorLayer *>( mTrackers.at( index.row() )->layer() );
    case Visible:
      return mTrackers.at( index.row() )->visible();
    case StartPositionTimestamp:
      return mTrackers.at( index.row() )->startPositionTimestamp();
    case MeasureType:
      return mTrackers.at( index.row() )->measureType();
    default:
      return QVariant();
  }
}

bool TrackingModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  Tracker *currentTracker = mTrackers[index.row()];
  switch ( role )
  {
    case TimeInterval:
      currentTracker->setTimeInterval( value.toDouble() );
      break;
    case MinimumDistance:
      currentTracker->setMinimumDistance( value.toDouble() );
      break;
    case Conjunction:
      currentTracker->setConjunction( value.toBool() );
      break;
    case Feature:
      currentTracker->setFeature( value.value<QgsFeature>() );
      break;
    case RubberModel:
      currentTracker->setModel( value.value<RubberbandModel *>() );
      break;
    case Visible:
      currentTracker->setVisible( value.toBool() );
      break;
    case MeasureType:
      currentTracker->setMeasureType( static_cast<Tracker::MeasureType>( value.toInt() ) );
    default:
      return false;
  }
  emit dataChanged( index, index, QVector<int>() << role );
  return true;
}

bool TrackingModel::featureInTracking( QgsVectorLayer *layer, const QgsFeatureId featureId )
{
  if ( trackerIterator( layer ) != mTrackers.constEnd() )
  {
    int listIndex = trackerIterator( layer ) - mTrackers.constBegin();
    if ( mTrackers[listIndex]->feature().id() == featureId )
      return true;
  }
  return false;
}

bool TrackingModel::featuresInTracking( QgsVectorLayer *layer, const QList<QgsFeature> &features )
{
  if ( trackerIterator( layer ) != mTrackers.constEnd() )
  {
    int listIndex = trackerIterator( layer ) - mTrackers.constBegin();
    QgsFeatureId fid = mTrackers[listIndex]->feature().id();
    if ( std::any_of( features.begin(), features.end(), [fid]( const QgsFeature &f ) { return f.id() == fid; } ) )
    {
      return true;
    }
  }
  return false;
}

bool TrackingModel::layerInTracking( QgsVectorLayer *layer )
{
  return trackerIterator( layer ) != mTrackers.constEnd();
}

void TrackingModel::reset()
{
  beginResetModel();
  qDeleteAll( mTrackers );
  mTrackers.clear();
  endResetModel();
}

void TrackingModel::createTracker( QgsVectorLayer *layer, bool visible )
{
  beginInsertRows( QModelIndex(), mTrackers.count(), mTrackers.count() );
  mTrackers.append( new Tracker( layer, visible ) );
  endInsertRows();
}

void TrackingModel::startTracker( QgsVectorLayer *layer )
{
  int listIndex = trackerIterator( layer ) - mTrackers.constBegin();
  mTrackers[listIndex]->start();
  emit layerInTrackingChanged( layer, true );
}

void TrackingModel::stopTracker( QgsVectorLayer *layer )
{
  int listIndex = trackerIterator( layer ) - mTrackers.constBegin();
  mTrackers[listIndex]->stop();

  beginRemoveRows( QModelIndex(), listIndex, listIndex );
  delete mTrackers.takeAt( listIndex );
  endRemoveRows();

  emit layerInTrackingChanged( layer, false );
}

void TrackingModel::setLayerVisible( QgsVectorLayer *layer, bool visible )
{
  if ( trackerIterator( layer ) != mTrackers.constEnd() )
  {
    int listIndex = trackerIterator( layer ) - mTrackers.constBegin();
    setData( index( listIndex, 0, QModelIndex() ), visible, Visible );
  }
}
