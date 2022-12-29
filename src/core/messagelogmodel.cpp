/***************************************************************************
  messagelogmodel.cpp - MessageLogModel

 ---------------------
 begin                : 13.7.2016
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
#include "messagelogmodel.h"

#include <QDebug>
#include <qgsapplication.h>

MessageLogModel::MessageLogModel( QObject *parent )
  : QAbstractListModel( parent )
  , mMessageLog( QgsApplication::messageLog() )
{
  connect( mMessageLog, static_cast<void ( QgsMessageLog::* )( const QString &message, const QString &tag, Qgis::MessageLevel level )>( &QgsMessageLog::messageReceived ), this, &MessageLogModel::onMessageReceived );
}

QHash<int, QByteArray> MessageLogModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[MessageRole] = "Message";
  roles[MessageTagRole] = "MessageTag";
  roles[MessageLevelRole] = "MessageLevel";
  roles[MessageDateTimeRole] = "MessageDateTime";

  return roles;
}

int MessageLogModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mMessages.size();
}

QVariant MessageLogModel::data( const QModelIndex &index, int role ) const
{
  if ( index.row() >= mMessages.size() )
    return QVariant();

  if ( role == MessageRole )
    return mMessages.at( index.row() ).message;
  else if ( role == MessageTagRole )
    return mMessages.at( index.row() ).tag;
  else if ( role == MessageLevelRole )
    return mMessages.at( index.row() ).level;
  else if ( role == MessageDateTimeRole )
    return mMessages.at( index.row() ).datetime;

  return QVariant();
}

void MessageLogModel::suppressTags( const QList<QString> &tags )
{
  for ( const QString &tag : tags )
  {
    if ( !mSuppressedTags.contains( tag ) )
      mSuppressedTags.append( tag );
  }
}

void MessageLogModel::unsuppressTags( const QList<QString> &tags )
{
  for ( const QString &tag : tags )
  {
    if ( mSuppressedTags.contains( tag ) )
      mSuppressedTags.removeAll( tag );
  }
}

void MessageLogModel::clear()
{
  beginResetModel();
  mMessages.clear();
  endResetModel();
}

void MessageLogModel::onMessageReceived( const QString &message, const QString &tag, Qgis::MessageLevel level )
{
  if ( mSuppressedTags.contains( tag ) || tag == QLatin1String( "3D" ) )
    return;

  beginInsertRows( QModelIndex(), 0, 0 );
  mMessages.prepend( LogMessage( tag, message, level ) );
  qDebug() << "Nes message " << tag << " : " << message;
  endInsertRows();
}
