/***************************************************************************
  layertreemapcanvasbridge.h - LayerTreeMapCanvasBridge

 ---------------------
 begin                : 26.8.2016
 copyright            : (C) 2016 by Matthias Kuhn, OPENGIS.ch
 email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LAYERTREEMAPCANVASBRIDGE_H
#define LAYERTREEMAPCANVASBRIDGE_H

#include "layertreemodel.h"
#include "trackingmodel.h"

#include <QObject>
#include <QStringList>
#include <qgscoordinatereferencesystem.h>
#include <qgsmapthemecollection.h>

class QgsLayerTreeGroup;
class QgsLayerTreeNode;
class QgsQuickMapSettings;
class QgsMapCanvasLayer;
class QgsMapLayer;

/**
 * The QgsLayerTreeMapCanvasBridge class takes care of updates of layer set
 * for QgsMapCanvas from a layer tree. The class listens to the updates in the layer tree
 * and updates the list of layers for rendering whenever some layers are added, removed,
 * or their visibility changes.
 *
 * The update of layers is not done immediately - it is postponed, so a series of updates
 * to the layer tree will trigger just one update of canvas layers.
 *
 * Also allows the client to override the default order of layers. This is useful
 * in advanced cases where the grouping in layer tree should be independent from the actual
 * order in the canvas.
 *
 * Passes to the legend (LayerTreeModel) the information, if the layer is in a tracking session
 */
class LayerTreeMapCanvasBridge : public QObject
{
    Q_OBJECT
  public:
    //! Constructor: does not take ownership of the layer tree nor canvas
    LayerTreeMapCanvasBridge( FlatLayerTreeModel *model, QgsQuickMapSettings *mapSettings, TrackingModel *trackingModel, QObject *parent = nullptr );

    QgsLayerTree *rootGroup() const { return mRoot; }
    QgsQuickMapSettings *mapSettings() const { return mMapSettings; }

    //! if enabled, will automatically set full canvas extent and destination CRS + map units
    //! when first layer(s) are added
    void setAutoSetupOnFirstLayer( bool enabled ) { mAutoSetupOnFirstLayer = enabled; }
    bool autoSetupOnFirstLayer() const { return mAutoSetupOnFirstLayer; }

    //! if enabled, will automatically turn on on-the-fly reprojection of layers if a layer
    //! with different source CRS is added
    void setAutoEnableCrsTransform( bool enabled )
    {
      mAutoEnableCrsTransform = enabled;
    }
    bool autoEnableCrsTransform() const
    {
      return mAutoEnableCrsTransform;
    }

    //! force update of canvas layers from the layer tree. Normally this should not be needed to be called.
    Q_INVOKABLE void setCanvasLayers();

  private slots:
    void extentChanged();
    void nodeVisibilityChanged();
    void mapThemeChanged();
    void layerInTrackingChanged( QgsVectorLayer *layer, bool tracking );

  private:
    void setCanvasLayers( QgsLayerTreeNode *node, QList<QgsMapLayer *> &canvasLayers, QList<QgsMapLayer *> &allLayers );

    void deferredSetCanvasLayers();

    QgsLayerTree *mRoot = nullptr;
    FlatLayerTreeModel *mModel = nullptr;
    QgsQuickMapSettings *mMapSettings = nullptr;
    TrackingModel *mTrackingModel = nullptr;

    bool mPendingCanvasUpdate;

    bool mHasCustomLayerOrder;
    QStringList mCustomLayerOrder;

    bool mAutoSetupOnFirstLayer;
    bool mAutoEnableCrsTransform;

    bool mHasFirstLayer;
    bool mHasLayersLoaded;

    QgsCoordinateReferenceSystem mFirstCRS;
};

#endif // LAYERTREEMAPCANVASBRIDGE_H
