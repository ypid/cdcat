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
#include "config.h"
#include "cdcat.h"
#include "cdcatexif.h"
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>
#include <string.h>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QPainter>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
// #include <libintl.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

QStringList getExifSupportedExtensions() {
    // FIXME: get from libexif?
    QStringList exts;

    exts.append( "jpg" );
    exts.append( "jpeg" );
    exts.append( "JPG" );
    exts.append( "JPEG" );
    exts.append( "tiff" );
    exts.append( "TIFF" );
    return exts;
}

/* from exif examples photographer.c */
/* Remove spaces on the right of the string */
void CdcatExifData::trim_spaces( char *buf ) {
    char *s = buf - 1;

    for (; *buf; ++buf)
        if (*buf != ' ') {
            s = buf;
        }
    *++s = 0;     /* nul terminate the string on the first of the final spaces */
}

/* Get the tag name and contents if the tag exists */
QString CdcatExifData::get_tag( ExifData *d, ExifIfd ifd, ExifTag tag ) {
    /* See if this tag exists */
    DEBUG_INFO_ENABLED = init_debug_info();
    QString ExifTagString = "";
    ExifEntry *entry = exif_content_get_entry( d->ifd[ifd], tag );
    if (entry) {
        char buf[1024];

        /* Get the contents of the tag in human-readable form */
        exif_entry_get_value( entry, buf, sizeof(buf));

        /* Don't bother printing it if it's entirely blank */
        trim_spaces( buf );
        if (*buf) {
            // ExifTagString.sprintf ( "%s (0x%x): %s", exif_tag_get_name_in_ifd ( tag, ifd ), tag, buf );
            ExifTagString.sprintf( "%s: %s", exif_tag_get_name_in_ifd( tag, ifd ), buf );
        }
    }
    return ExifTagString;
}

/* Show the given MakerNote tag if it exists */
QString CdcatExifData::get_mnote_tag( ExifData *d, unsigned tag ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    QString ExifTagNoteString = "";
    ExifMnoteData *mn = exif_data_get_mnote_data( d );
    if (mn) {
        int num = exif_mnote_data_count( mn );
        int i;

        /* Loop through all MakerNote tags, searching for the desired one */
        for (i = 0; i < num; ++i) {
            char buf[1024];
            if (exif_mnote_data_get_id( mn, i ) == tag) {
                if (exif_mnote_data_get_value( mn, i, buf, sizeof(buf))) {
                    /* Don't bother printing it if it's entirely blank */
                    trim_spaces( buf );
                    if (*buf) {
                        // ExifTagNoteString.sprintf ( "%s (0x%x): %s", exif_mnote_data_get_value ( mn, i, buf, sizeof ( buf )), buf);
                        ExifTagNoteString.sprintf( "%s: %s", exif_mnote_data_get_value( mn, i, buf, sizeof(buf)), buf );
                    }
                }
            }
        }
    }
    return ExifTagNoteString;
}

CdcatExifData::CdcatExifData ( void ) {
    Comment = "";
    picturefilename = "";
}

CdcatExifData::CdcatExifData ( QString filename ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        std::cerr << "CdcatExifData() filename: " << qPrintable( filename ) << std::endl;
    }
    this->picturefilename = filename;
    readCdcatExifData();
}

bool CdcatExifData::readCdcatExifData() {
    // read data
    int i;

    DEBUG_INFO_ENABLED = init_debug_info();

    if (picturefilename == "") {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "readCdcatExifData() no filename set" << std::endl;
        }
        return false;
    }

    if (*DEBUG_INFO_ENABLED) {
        std::cout << "readCdcatExifData() filename: " << qPrintable( picturefilename ) << std::endl;
    }
    ed = exif_data_new_from_file( picturefilename.toLocal8Bit().data());
    if (ed) {
        // bind_textdomain_codeset("cdcat", "UTF-8");
        // textdomain("cdcat");
        if (*DEBUG_INFO_ENABLED) {
            std::cout << "EXIF data for file " << qPrintable( picturefilename ) << std::endl;
            std::cout << "EXIF_IFD_EXIF" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_EXIF, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    std::cout << qPrintable( TagData ) << std::endl;
                }
            }

            std::cout << "EXIF_IFD_0" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_0, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    std::cout << qPrintable( TagData ) << std::endl;
                }
            }
            std::cout << "EXIF_IFD_1" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_1, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    std::cout << qPrintable( TagData ) << std::endl;
                }
            }
        }
    } else {
        if (*DEBUG_INFO_ENABLED) {
            std::cout << "File not readable or no EXIF data in file " << qPrintable( picturefilename ) << std::endl;
        }
    }
    return true;
}

QStringList CdcatExifData::getExifData() {
    QStringList ExifDataStringList;
    // read data
    int i;

    DEBUG_INFO_ENABLED = init_debug_info();

    if (picturefilename == "") {
        if (*DEBUG_INFO_ENABLED) {
            std::cout << "readCdcatExifData() no filename set" << std::endl;
        }
        return ExifDataStringList;
    }
    if (!ed) {
        if (*DEBUG_INFO_ENABLED) {
            std::cout << "readCdcatExifData() no exif data yet" << std::endl;
        }
        return ExifDataStringList;
    }

    if (*DEBUG_INFO_ENABLED) {
        std::cout << "readCdcatExifData() filename: " << qPrintable( picturefilename ) << std::endl;
    }
    ExifData *ed = exif_data_new_from_file( picturefilename.toLocal8Bit().data());
    if (ed) {
        if (*DEBUG_INFO_ENABLED) {
            std::cout << "EXIF data for file " << qPrintable( picturefilename ) << std::endl;
            std::cout << "EXIF_IFD_EXIF" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_EXIF, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    ExifDataStringList.append( TagData );
                }
            }

            std::cout << "EXIF_IFD_0" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_0, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    ExifDataStringList.append( TagData );
                }
            }
            std::cout << "EXIF_IFD_1" << endl;
            for (i = 0x0001; i != 0xffff; i++) {
                QString TagData = get_tag( ed, EXIF_IFD_1, (ExifTag)i );
                if (!TagData.isEmpty()) {
                    ExifDataStringList.append( TagData );
                }
            }
        }
    } else {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "File not readable or no EXIF data in file " << qPrintable( picturefilename ) << std::endl;
        }
    }
    return ExifDataStringList;
}

CdcatExifData::~CdcatExifData ( void ) {
}


QString CdcatExifData::comment( void ) const {
    return Comment;
}

#endif
/*end of file*/

// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
