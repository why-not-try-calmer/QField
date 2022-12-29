/***************************************************************************
  androidpicturesource.h - AndroidPictureSource

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
#ifndef ANDROIDPICTURESOURCE_H
#define ANDROIDPICTURESOURCE_H

#include "picturesource.h"

#include <QAndroidActivityResultReceiver>


class AndroidPictureSource : public PictureSource, public QAndroidActivityResultReceiver
{
    Q_OBJECT

  public:
    /**
     * Construct a new Android Picture Source object
     *
     * @param prefix The project folder. Base directory path for all relative paths.
     */
    explicit AndroidPictureSource( const QString &prefix );

    //! QAndroidActivityResultReceiver
    void handleActivityResult( int receiverRequestCode, int resultCode, const QAndroidJniObject &data ) override;

  private:
    /**
     * Base directory path for all relative paths.
     */
    QString mPrefix;
};

#endif // ANDROIDPICTURESOURCE_H
