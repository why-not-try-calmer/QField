/***************************************************************************

               ----------------------------------------------------
              date                 : 27.12.2014
              copyright            : (C) 2014 by Matthias Kuhn
              email                : matthias (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featurelistextentcontroller.h"

#include <qgsgeometry.h>
#include <qgsvectorlayer.h>

FeatureListExtentController::FeatureListExtentController( QObject *parent )
  : QObject( parent )
{
  connect( this, &FeatureListExtentController::autoZoomChanged, this, [=]() { zoomToSelected(); } );
  connect( this, &FeatureListExtentController::modelChanged, this, &FeatureListExtentController::onModelChanged );
  connect( this, &FeatureListExtentController::selectionChanged, this, &FeatureListExtentController::onModelChanged );
}

FeatureListExtentController::~FeatureListExtentController()
{
}

FeatureListModelSelection *FeatureListExtentController::selection() const
{
  return mSelection;
}

MultiFeatureListModel *FeatureListExtentController::model() const
{
  return mModel;
}

void FeatureListExtentController::requestFeatureFormState()
{
  emit featureFormStateRequested();
}

void FeatureListExtentController::zoomToSelected( bool skipIfIntersects ) const
{
  if ( mModel && mSelection && mSelection->focusedItem() > -1 && mMapSettings )
  {
    QgsFeature feat = mSelection->focusedFeature();
    QgsVectorLayer *layer = mSelection->focusedLayer();

    if ( layer && layer->geometryType() != QgsWkbTypes::UnknownGeometry && layer->geometryType() != QgsWkbTypes::NullGeometry )
    {
      QgsCoordinateTransform transf( layer->crs(), mMapSettings->destinationCrs(), mMapSettings->mapSettings().transformContext() );
      QgsGeometry geom( feat.geometry() );
      if ( !geom.isNull() )
      {
        geom.transform( transf );

        if ( geom.type() == QgsWkbTypes::PointGeometry )
        {
          if ( !skipIfIntersects || !mMapSettings->extent().intersects( geom.boundingBox() ) )
            mMapSettings->setCenter( QgsPoint( geom.asPoint() ) );
        }
        else
        {
          QgsRectangle featureExtent = geom.boundingBox();
          QgsRectangle bufferedExtent = featureExtent.buffered( std::max( featureExtent.width(), featureExtent.height() ) );

          if ( !skipIfIntersects || !mMapSettings->extent().intersects( bufferedExtent ) )
            mMapSettings->setExtent( bufferedExtent );
        }
      }
    }
  }
}

void FeatureListExtentController::onModelChanged()
{
  if ( mModel && mSelection )
  {
    connect( mSelection, &FeatureListModelSelection::focusedItemChanged, this, &FeatureListExtentController::onCurrentSelectionChanged );
  }
}

void FeatureListExtentController::onCurrentSelectionChanged()
{
  if ( mAutoZoom )
  {
    zoomToSelected();
  }
}
