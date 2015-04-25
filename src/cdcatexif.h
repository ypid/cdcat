/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke (crissi99@gmx.de)
*  License   : GPL
*  Copyright : (C) 2011 Christoph Thielecke
*
*  Info: read exif data from pictures
****************************************************************************/
#ifdef USE_LIBEXIF
#include <stdio.h>
#include <QObject>
#include <QString>

#ifdef _WIN32
// #include <windows.h>
#endif

#include <libexif/exif-data.h>



#ifndef CDCAT_EXIF_H__
#define CDCAT_EXIF_H__

QStringList getExifSupportedExtensions();

class CdcatExifData : public QObject {
public:
    QString get_mnote_tag( ExifData *d, unsigned tag );
    void trim_spaces( char *buf );
    QString get_tag( ExifData *d, ExifIfd ifd, ExifTag tag );
    CdcatExifData ( void );
    CdcatExifData ( QString fn );
    ~CdcatExifData ( void );
    QString comment( void ) const;
    bool readCdcatExifData();
    QStringList getExifData();
    QString picturefilename;

private:
    QString Comment;
    ExifData *ed;
};


#endif
#endif

