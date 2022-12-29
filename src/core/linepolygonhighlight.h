/***************************************************************************
              locatorhighlight.h
               ----------------------------------------------------
              date                 : 28.11.2018
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

#ifndef LOCATORHIGHLIGHT_H
#define LOCATORHIGHLIGHT_H

#include "qgsgeometrywrapper.h"
#include "qgsquickmapsettings.h"

#include <QtQuick/QQuickItem>

class QgsGeometry;


/**
 * LocatorHighlight allows highlighting geometries
 * on the canvas for the specific needs of the locator.
 */
class LinePolygonHighlight : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY( QColor color READ color WRITE setColor NOTIFY colorChanged )
    Q_PROPERTY( float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QgsGeometryWrapper *geometry READ geometry WRITE setGeometry NOTIFY qgsGeometryChanged )

  public:
    explicit LinePolygonHighlight( QQuickItem *parent = nullptr );

    QgsGeometryWrapper *geometry() const;
    void setGeometry( QgsGeometryWrapper *geometry );

    QgsQuickMapSettings *mapSettings() const;
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    QColor color() const;
    void setColor( const QColor &color );

    float lineWidth() const;
    void setLineWidth( float width );

  signals:
    void colorChanged();
    void lineWidthChanged();
    void mapSettingsChanged();
    void qgsGeometryChanged();
    void updated();

  private slots:
    void mapCrsChanged();
    void visibleExtentChanged();
    void makeDirty();

  private:
    virtual QSGNode *updatePaintNode( QSGNode *n, UpdatePaintNodeData * ) override;

    QColor mColor;
    float mWidth = 0;
    bool mDirty = false;
    QgsQuickMapSettings *mMapSettings = nullptr;
    QgsGeometryWrapper *mGeometry = nullptr;
};

#endif // LOCATORHIGHLIGHT_H
