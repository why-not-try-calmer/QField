#ifndef QGSGNSSPOSITIONINFORMATION_H
#define QGSGNSSPOSITIONINFORMATION_H

#include <QDateTime>
#include "qgis.h"
#include <QObject>
#include <QString>

#include "qgsgpsconnection.h"
#include <QtPositioning/QGeoPositionInfoSource>

/* Statics from external/nmea/info.h:*/
#define NMEA_SIG_BAD        (0)
#define NMEA_SIG_LOW        (1)
#define NMEA_SIG_MID        (2)
#define NMEA_SIG_HIGH       (3)

#define NMEA_FIX_BAD        (1)
#define NMEA_FIX_2D         (2)
#define NMEA_FIX_3D         (3)

#define NMEA_MAXSAT         (12)
#define NMEA_SATINPACK      (4)
#define NMEA_NSATPACKS      (NMEA_MAXSAT / NMEA_SATINPACK)

#define NMEA_DEF_LAT        (5001.2621)
#define NMEA_DEF_LON        (3613.0595)

class QgsGnssPositionInformation
{
    Q_GADGET

    Q_PROPERTY( double latitude READ latitude )
    Q_PROPERTY( double longitude READ longitude )
    Q_PROPERTY( double elevation READ elevation )
    Q_PROPERTY( double speed READ speed )
    Q_PROPERTY( double direction READ direction )
    Q_PROPERTY( QgsSatelliteInfo satellitesInView READ satellitesInView )
    Q_PROPERTY( double pdop READ pdop )
    Q_PROPERTY( double hdop READ hdop )
    Q_PROPERTY( double vdop READ vdop )
    Q_PROPERTY( double hacc READ hacc )
    Q_PROPERTY( double vacc READ vacc )
    Q_PROPERTY( QDateTime utcDateTime READ utcDateTime )
    Q_PROPERTY( QChar fixMode READ fixMode )
    Q_PROPERTY( int fixType READ fixType )
    Q_PROPERTY( int quality READ quality )
    Q_PROPERTY( int satellitesUsed READ satellitesUsed )
    Q_PROPERTY( QChar status READ status )
    Q_PROPERTY( QList<int> satPrn READ satPrn )
    Q_PROPERTY( bool satInfoComplete READ satInfoComplete )
    Q_PROPERTY( bool isValid READ isValid )
    Q_PROPERTY( FixStatus fixStatus READ fixStatus )
    Q_PROPERTY( QString qualityDescription READ qualityDescription )

  public:

    /**
     * GPS fix status
     */
    enum FixStatus
    {
      NoData,
      NoFix,
      Fix2D,
      Fix3D
    };

    QgsGnssPositionInformation() {};

    //explicit QgsGnssPositionInformation( const QgsGpsInformation &gpsInformation );
    //explicit QgsGnssPositionInformation( const QGeoPositionInfo &geoPositionInfo );

    /**
     * Latitude in decimal degrees, using the WGS84 datum. A positive value indicates the Northern Hemisphere, and
     * a negative value indicates the Southern Hemisphere.
     */
    double latitude() const { return mLatitude; }

    /**
     * Longitude in decimal degrees, using the WGS84 datum. A positive value indicates the Eastern Hemisphere, and
     * a negative value indicates the Western Hemisphere.
     */
    double longitude() const { return mLongitude; }

    /**
     * Altitude (in meters) above or below the mean sea level.
     */
    double elevation() const { return mElevation; }

    /**
     * Ground speed, in km/h.
     */
    double speed() const { return mSpeed; }

    /**
     * The bearing measured in degrees clockwise from true north to the direction of travel.
     */
    double direction() const { return mDirection; }

    /**
     * Contains a list of information relating to the current satellites in view.
     */
    QgsSatelliteInfo satellitesInView() const { return mSatellitesInView; }

    /**
     * Dilution of precision.
     */
    double pdop() const { return mPdop; }

    /**
     * Horizontal dilution of precision.
     */
    double hdop() const { return mHdop; }

    /**
     * Vertical dilution of precision.
     */
    double vdop() const { return mVdop; }

    /**
     * Horizontal accuracy in meters
     */
    double hacc() const { return mHacc; }

    /**
     * Vertical accuracy in meters
     */
    double vacc() const { return mVacc; }

    /**
     * The date and time at which this position was reported, in UTC time.
     */
    QDateTime utcDateTime() const { return mUtcDateTime; }
    /**
     * Fix mode (where M = Manual, forced to operate in 2D or 3D or A = Automatic, 3D/2D)
     */
    QChar fixMode() const { return mFixMode; }

    /**
     * Contains the fix type, where 1 = no fix, 2 = 2d fix, 3 = 3d fix
     */
    int fixType() const { return mFixType; }

    /**
     * GPS quality indicator (0 = Invalid; 1 = Fix; 2 = Differential, 3 = Sensitive)
     */
    int quality() const { return mQuality; }

    /**
     * Count of satellites used in obtaining the fix.
     */
    int satellitesUsed() const { return mSatellitesUsed; }

    /**
     * Status (A = active or V = void)
     */
    QChar status() const { return mStatus; }

    /**
     * IDs of satellites used in the position fix.
     */
    QList<int> satPrn() const { return mSatPrn; }

    /**
     * TRUE if satellite information is complete.
     */
    bool satInfoComplete() const { return mSatInfoComplete; }

    /**
     * Returns whether the connection information is valid
     */
    bool isValid() const;

    /**
     * Returns the fix status
     */
    FixStatus fixStatus() const;

    /**
     * Returns a descriptive string for the signal quality.
     */
    QString qualityDescription() const;

  private:
    double mLatitude = 0;
    double mLongitude = 0;
    double mElevation = 0;
    double mSpeed = 0;
    double mDirection = std::numeric_limits< double >::quiet_NaN();
    QgsSatelliteInfo mSatellitesInView;
    double mPdop = 0;
    double mHdop = 0;
    double mVdop = 0;
    double mHacc = std::numeric_limits< double >::quiet_NaN();
    double mVacc = std::numeric_limits< double >::quiet_NaN();
    QDateTime mUtcDateTime;
    QChar mFixMode;
    int mFixType = 0;
    int mQuality = -1;
    int mSatellitesUsed = 0;
    QChar mStatus;
    QList<int> mSatPrn;
    bool mSatInfoComplete = false;

    //unused
    bool mIsValid;
    FixStatus mFixStatus;
    QString mQualityDescription;

};
Q_DECLARE_METATYPE(QgsGnssPositionInformation)

#endif // QGSGNSSPOSITIONINFORMATION_H
