/***************************************************************************
  snappingutils.cpp

 ---------------------
 begin                : 8.10.2016
 copyright            : (C) 2016 by Matthias Kuhn
 email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsquickmapsettings.h"
#include "snappingutils.h"

#include <qgsproject.h>
#include <qgsvectorlayer.h>

SnappingUtils::SnappingUtils( QObject *parent )
  : QgsSnappingUtils( parent, false /*enableSnappingForInvisibleFeature*/ )
  , mSettings( nullptr )
{
  connect( QgsProject::instance(), static_cast<void ( QgsProject::* )( const QStringList & )>( &QgsProject::layersWillBeRemoved ), this, &SnappingUtils::removeOutdatedLocators );
}

void SnappingUtils::onMapSettingsUpdated()
{
  QgsSnappingUtils::setMapSettings( mSettings->mapSettings() );

  snap();
}

void SnappingUtils::removeOutdatedLocators()
{
  clearAllLocators();
}

QgsPoint SnappingUtils::newPoint( const QgsPoint &snappedPoint, const QgsWkbTypes::Type wkbType )
{
  QgsPoint newPoint( QgsWkbTypes::Point, snappedPoint.x(), snappedPoint.y() );

  // convert to the corresponding type for a full ZM support
  if ( QgsWkbTypes::hasZ( wkbType ) && !QgsWkbTypes::hasM( wkbType ) )
  {
    newPoint.convertTo( QgsWkbTypes::PointZ );
  }
  else if ( !QgsWkbTypes::hasZ( wkbType ) && QgsWkbTypes::hasM( wkbType ) )
  {
    newPoint.convertTo( QgsWkbTypes::PointM );
  }
  else if ( QgsWkbTypes::hasZ( wkbType ) && QgsWkbTypes::hasM( wkbType ) )
  {
    newPoint.convertTo( QgsWkbTypes::PointZM );
  }

  // set z value
  if ( QgsWkbTypes::hasZ( newPoint.wkbType() ) && QgsWkbTypes::hasZ( snappedPoint.wkbType() ) )
  {
    newPoint.setZ( snappedPoint.z() );
  }

  // set m value
  if ( QgsWkbTypes::hasM( newPoint.wkbType() ) && QgsWkbTypes::hasM( snappedPoint.wkbType() ) )
  {
    newPoint.setM( snappedPoint.m() );
  }

  return newPoint;
}

void SnappingUtils::snap()
{
  QgsPointXY point = mapSettings()->screenToCoordinate( mInputCoordinate );
  QgsPointLocator::Match match = snapToMap( point );
  mSnappingResult = SnappingResult( match );

  //set point containing ZM if we snapped to a point/vertex
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( currentLayer() );

  bool vertexIndexValid;
  switch ( match.type() )
  {
    case QgsPointLocator::Type::Area:
    case QgsPointLocator::Type::Centroid:
    case QgsPointLocator::Type::Edge:
    case QgsPointLocator::Type::Invalid:
    case QgsPointLocator::Type::MiddleOfSegment:
    case QgsPointLocator::Type::All:
      vertexIndexValid = false;
      break;

    case QgsPointLocator::Type::Vertex:
    case QgsPointLocator::Type::LineEndpoint:
      vertexIndexValid = true;
      break;
  }

  if ( vertexIndexValid
       && vlayer
       && match.layer()
       && ( QgsWkbTypes::hasZ( vlayer->wkbType() ) || QgsWkbTypes::hasM( vlayer->wkbType() ) ) )
  {
    const QgsFeature ft = match.layer()->getFeature( match.featureId() );
    QgsPoint snappedPoint = newPoint( ft.geometry().vertexAt( match.vertexIndex() ), vlayer->wkbType() );
    if ( vlayer->crs() != mapSettings()->destinationCrs() )
    {
      QgsCoordinateTransform transform( vlayer->crs(), mapSettings()->destinationCrs(), QgsProject::instance()->transformContext() );
      try
      {
        snappedPoint.transform( transform );
      }
      catch ( const QgsException &e )
      {
        Q_UNUSED( e )
        return;
      }
      catch ( ... )
      {
        // catch any other errors
        return;
      }
    }
    mSnappingResult.setPoint( snappedPoint );
  }

  emit snappingResultChanged();
}

QPointF SnappingUtils::inputCoordinate() const
{
  return mInputCoordinate;
}

void SnappingUtils::setInputCoordinate( const QPointF &inputCoordinate )
{
  if ( inputCoordinate == mInputCoordinate )
    return;

  mInputCoordinate = inputCoordinate;

  snap();

  emit inputCoordinateChanged();
}

SnappingResult SnappingUtils::snappingResult() const
{
  return mSnappingResult;
}

void SnappingUtils::prepareIndexStarting( int count )
{
  mIndexLayerCount = count;
  emit indexingStarted( count );
}

void SnappingUtils::prepareIndexProgress( int index )
{
  if ( index == mIndexLayerCount )
    emit indexingFinished();
  else
    emit indexingProgress( index );
}

QgsVectorLayer *SnappingUtils::currentLayer() const
{
  return mCurrentLayer;
}

void SnappingUtils::setCurrentLayer( QgsVectorLayer *currentLayer )
{
  if ( currentLayer == mCurrentLayer )
    return;

  mCurrentLayer = currentLayer;
  QgsSnappingUtils::setCurrentLayer( currentLayer );

  emit currentLayerChanged();
}

QgsQuickMapSettings *SnappingUtils::mapSettings() const
{
  return mSettings;
}

void SnappingUtils::setMapSettings( QgsQuickMapSettings *settings )
{
  if ( mSettings == settings )
    return;

  connect( settings, &QgsQuickMapSettings::extentChanged, this, &SnappingUtils::onMapSettingsUpdated );
  connect( settings, &QgsQuickMapSettings::destinationCrsChanged, this, &SnappingUtils::onMapSettingsUpdated );
  connect( settings, &QgsQuickMapSettings::layersChanged, this, &SnappingUtils::onMapSettingsUpdated );

  mSettings = settings;
  emit mapSettingsChanged();
}
