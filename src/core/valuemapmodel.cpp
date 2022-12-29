/***************************************************************************
                            valuemapmodel.cpp

                              -------------------
              begin                : March 2019
              copyright            : (C) 2019 by Matthias Kuhn
              email                : matthias@opengis.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsvaluemapfieldformatter.h"
#include "valuemapmodel.h"

#include <QDebug>

ValueMapModel::ValueMapModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

QVariant ValueMapModel::map() const
{
  return mConfiguredMap;
}

void ValueMapModel::setMap( const QVariant &map )
{
  mMap.clear();
  // QGIS 3
  const QVariantList list = map.toList();
  if ( !list.empty() )
  {
    beginInsertRows( QModelIndex(), 0, list.size() );

    for ( const QVariant &item : list )
    {
      const QVariantMap mapItem = item.toMap();

      const QString key = mapItem.firstKey();
      const QVariant value = mapItem.value( key );

      mMap.append( qMakePair( value, key ) );
    }
    endInsertRows();
  }
  else // QGIS 2 compat
  {
    const QVariantMap valueMap = map.toMap();
    if ( !valueMap.empty() )
    {
      beginInsertRows( QModelIndex(), 0, valueMap.size() );

      QMapIterator<QString, QVariant> i( valueMap );
      while ( i.hasNext() )
      {
        i.next();
        const QString key = i.key();
        const QVariant value = i.value();

        mMap.append( qMakePair( value, key ) );
      }
      endInsertRows();
    }
  }

  mConfiguredMap = map;
  emit mapChanged();
}

int ValueMapModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mMap.size();
}

QVariant ValueMapModel::data( const QModelIndex &index, int role ) const
{
  if ( role == KeyRole )
    return mMap.at( index.row() ).first;
  else
    return mMap.at( index.row() ).second;
}

QHash<int, QByteArray> ValueMapModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

  roles[KeyRole] = "key";
  roles[ValueRole] = "value";

  return roles;
}

int ValueMapModel::keyToIndex( const QVariant &key ) const
{
  int i = 0;
  for ( const auto &item : mMap )
  {
    if ( item.first.toString() == key.toString() )
    {
      return i;
    }
    ++i;
  }
  return -1;
}

QVariant ValueMapModel::keyForValue( const QString &value ) const
{
  QVariant result;

  auto match = std::find_if( mMap.begin(), mMap.end(), [&value]( const QPair<QVariant, QString> &x ) { return x.second == value; } );

  if ( match != mMap.end() )
    result = match->first;

  if ( result == QgsValueMapFieldFormatter::NULL_VALUE )
    result = QVariant();

  return result;
}
