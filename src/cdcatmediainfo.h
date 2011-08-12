/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Christoph Thielecke (crissi99@gmx.de)
 License   : GPL
 Copyright : (C) 2011 Christoph Thielecke

 Info: read mediainfo using libmediainfo
****************************************************************************/
#ifndef CDCAT_MEDIAINFO_H
#define CDCAT_MEDIAINFO_H



//#define MEDIAINFO_LIBRARY

#ifdef MEDIAINFO_LIBRARY
    #include "MediaInfo/MediaInfo.h" //Staticly-loaded library (.lib or .a or .so)
    #define MediaInfoNameSpace MediaInfoLib;
#else //MEDIAINFO_LIBRARY
    #include "MediaInfoDLL/MediaInfoDLL.h" //Dynamicly-loaded library (.dll or .so)
    #define MediaInfoNameSpace MediaInfoDLL;
#endif //MEDIAINFO_LIBRARY


#ifdef __MINGW32__
    #ifdef _UNICODE
        #define _itot _itow
    #else //_UNICODE
        #define _itot itoa
    #endif //_UNICODE
#endif //__MINGW32

#include <string>
#include <stdio.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <iostream>
#include <iomanip>

static QStringList MediaInfoSupportedFileExtensions;
static bool mediaInfoLibInitDone = false;
static bool mediaInfoLibFound = false;
static MediaInfoDLL::MediaInfo *MediaInfoHandler;


class CdcatMediaInfo : public QObject {
public:

    CdcatMediaInfo ( void );
    CdcatMediaInfo ( QString fn );
    ~CdcatMediaInfo ( void );
    bool readCdcatMediaInfo();
    QString filename;
    QString getInfo();
    bool detectSupportedExtensions();
    QStringList getSupportedExtensions();
    bool getMediaInfoLibFound();
    QString getMediaInfoVersion();


private:
	QString InfoText;
	bool initMediaInfoLib();

};




#endif

