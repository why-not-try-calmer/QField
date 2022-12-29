/***************************************************************************
  androidpicturesource.cpp - AndroidPictureSource

 ---------------------
 begin                : 5.7.2016
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
#include "androidpicturesource.h"
#include "qgsapplication.h"
#include "qgsmessagelog.h"

#include <QAndroidJniEnvironment>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QtAndroid>

AndroidPictureSource::AndroidPictureSource( const QString &prefix )
  : PictureSource( nullptr, prefix )
  , QAndroidActivityResultReceiver()
  , mPrefix( prefix )
{
}

void AndroidPictureSource::handleActivityResult( int receiverRequestCode, int resultCode, const QAndroidJniObject &data )
{
  if ( receiverRequestCode == 171 )
  {
    jint RESULT_OK = QAndroidJniObject::getStaticField<jint>( "android/app/Activity", "RESULT_OK" );
    if ( resultCode == RESULT_OK )
    {
      QAndroidJniObject extras = data.callObjectMethod( "getExtras", "()Landroid/os/Bundle;" );


      QAndroidJniObject picture_image_path = QAndroidJniObject::fromString( "PICTURE_IMAGE_FILENAME" );
      picture_image_path = extras.callObjectMethod( "getString", "(Ljava/lang/String;)Ljava/lang/String;",
                                                    picture_image_path.object<jstring>() );

      QString picture_image_relative_path = picture_image_path.toString().remove( mPrefix );

      emit pictureReceived( picture_image_relative_path );
    }
    else
    {
      emit pictureReceived( QString() );
    }
  }
}
