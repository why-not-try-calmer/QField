/***************************************************************************
                            featurelistmodel.cpp
                              -------------------
              begin                : 10.12.2014
              copyright            : (C) 2014 by Matthias Kuhn
              email                : matthias (at) opengis.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "multifeaturelistmodel.h"

#include <qgscoordinatereferencesystem.h>
#include <qgsexpressioncontextutils.h>
#include <qgsgeometry.h>
#include <qgsmessagelog.h>
#include <qgsproject.h>
#include <qgsrelationmanager.h>
#include <qgsvectordataprovider.h>
#include <qgsvectorlayer.h>

MultiFeatureListModel::MultiFeatureListModel( QObject *parent )
  : QSortFilterProxyModel( parent )
  , mSourceModel( new MultiFeatureListModelBase( this ) )
{
  setSourceModel( mSourceModel );
  connect( mSourceModel, &MultiFeatureListModelBase::modelReset, this, &MultiFeatureListModel::countChanged );
  connect( mSourceModel, &MultiFeatureListModelBase::countChanged, this, &MultiFeatureListModel::countChanged );
  connect( mSourceModel, &MultiFeatureListModelBase::selectedCountChanged, this, &MultiFeatureListModel::adjustFilterToSelectedCount );
}

void MultiFeatureListModel::setFeatures( const QMap<QgsVectorLayer *, QgsFeatureRequest> requests )
{
  mSourceModel->setFeatures( requests );
}

void MultiFeatureListModel::setFeatures( QgsVectorLayer *vl, const QString &filter, const QgsRectangle &extent )
{
  QgsFeatureRequest request;
  if ( !filter.isEmpty() )
  {
    request.setFilterExpression( filter );
  }
  if ( !extent.isEmpty() )
  {
    request.setFilterRect( extent );
  }
  QMap<QgsVectorLayer *, QgsFeatureRequest> requests( { { vl, request } } );
  mSourceModel->setFeatures( requests );
}

void MultiFeatureListModel::appendFeatures( const QList<IdentifyTool::IdentifyResult> &results )
{
  mSourceModel->appendFeatures( results );
}

void MultiFeatureListModel::clear( const bool keepSelected )
{
  if ( !keepSelected )
  {
    mFilterLayer = nullptr;
  }
  mSourceModel->clear( keepSelected );
}

void MultiFeatureListModel::clearSelection()
{
  mFilterLayer = nullptr;
  mSourceModel->clearSelection();
}

int MultiFeatureListModel::count() const
{
  return mSourceModel->count();
}

int MultiFeatureListModel::selectedCount() const
{
  return mSourceModel->selectedCount();
}

bool MultiFeatureListModel::canEditAttributesSelection()
{
  return mSourceModel->canEditAttributesSelection();
}

bool MultiFeatureListModel::canMergeSelection()
{
  return mSourceModel->canMergeSelection();
}

bool MultiFeatureListModel::canDeleteSelection()
{
  return mSourceModel->canDeleteSelection();
}

bool MultiFeatureListModel::canDuplicateSelection()
{
  return mSourceModel->canDuplicateSelection();
}

bool MultiFeatureListModel::canMoveSelection()
{
  return mSourceModel->canMoveSelection();
}

bool MultiFeatureListModel::mergeSelection()
{
  return mSourceModel->mergeSelection();
}

bool MultiFeatureListModel::deleteFeature( QgsVectorLayer *layer, QgsFeatureId fid )
{
  return mSourceModel->deleteFeature( layer, fid );
}

bool MultiFeatureListModel::deleteSelection()
{
  return mSourceModel->deleteSelection();
}

bool MultiFeatureListModel::duplicateFeature( QgsVectorLayer *layer, const QgsFeature &feature )
{
  return mSourceModel->duplicateFeature( layer, feature );
}

bool MultiFeatureListModel::duplicateSelection()
{
  return mSourceModel->duplicateSelection();
}

bool MultiFeatureListModel::moveSelection( const double x, const double y )
{
  return mSourceModel->moveSelection( x, y );
}

void MultiFeatureListModel::toggleSelectedItem( int item )
{
  QModelIndex sourceItem = mapToSource( index( item, 0 ) );
  mSourceModel->toggleSelectedItem( sourceItem.row() );
  if ( mSourceModel->selectedCount() > 0 && mFilterLayer == nullptr )
  {
    mFilterLayer = mSourceModel->data( sourceItem, MultiFeatureListModel::LayerRole ).value<QgsVectorLayer *>();
    emit selectedLayerChanged();
    invalidateFilter();
  }
  else if ( mSourceModel->selectedCount() == 0 && mFilterLayer != nullptr )
  {
    mFilterLayer = nullptr;
    emit selectedLayerChanged();
    invalidateFilter();
  }
}

void MultiFeatureListModel::adjustFilterToSelectedCount()
{
  if ( mSourceModel->selectedCount() > 0 && mFilterLayer == nullptr )
  {
    mFilterLayer = mSourceModel->selectedLayer();
    emit selectedLayerChanged();
    invalidateFilter();
  }
  else if ( mSourceModel->selectedCount() == 0 && mFilterLayer != nullptr )
  {
    mFilterLayer = nullptr;
    emit selectedLayerChanged();
    invalidateFilter();
  }
  emit selectedCountChanged();
}

QList<QgsFeature> MultiFeatureListModel::selectedFeatures()
{
  return mSourceModel->selectedFeatures();
}

QgsVectorLayer *MultiFeatureListModel::selectedLayer()
{
  return mFilterLayer.data();
}

bool MultiFeatureListModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( mFilterLayer != nullptr )
  {
    return mFilterLayer == mSourceModel->data( mSourceModel->index( source_row, 0, source_parent ), MultiFeatureListModel::LayerRole ).value<QgsVectorLayer *>();
  }
  return true;
}
