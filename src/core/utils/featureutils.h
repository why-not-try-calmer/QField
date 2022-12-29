/***************************************************************************
  featureutils.h - FeatureUtils

 ---------------------
 begin                : 05.03.2020
 copyright            : (C) 2020 by Denis Rouzaud
 email                : denis@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef FEATUREUTILS_H
#define FEATUREUTILS_H

#include "qfield_core_export.h"

#include <QObject>
#include <qgsfeature.h>
#include <qgsgeometry.h>

class QgsVectorLayer;

class QFIELD_CORE_EXPORT FeatureUtils : public QObject
{
    Q_OBJECT
  public:
    explicit FeatureUtils( QObject *parent = nullptr );

    static Q_INVOKABLE QgsFeature initFeature( QgsVectorLayer *layer, QgsGeometry geometry = QgsGeometry() );

    /**
    * Returns the display name of a given feature.
    * \param layer the vector layer containing the feature
    * \param feature the feature to be named
    */
    static Q_INVOKABLE QString displayName( QgsVectorLayer *layer, const QgsFeature &feature );
};

#endif // FEATUREUTILS_H
