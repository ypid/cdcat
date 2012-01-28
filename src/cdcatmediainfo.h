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


#ifdef MEDIAINFO_UNICODE
// workaround for mediainfo char width confusion
#ifndef UNICODE
#define UNICODE
#define UNSETUNICODE
#endif
#endif


//#define MEDIAINFO_STATIC

#ifdef MEDIAINFO_STATIC
    #include "MediaInfo/MediaInfo.h" //Staticly-loaded library (.lib or .a or .so)
    #define MediaInfoNameSpace MediaInfoLib
#else //MEDIAINFO_STATIC
    #include "MediaInfoDLL/MediaInfoDLL.h" //Dynamicly-loaded library (.dll or .so)
    #define MediaInfoNameSpace MediaInfoDLL
#endif //MEDIAINFO_STATIC

#ifdef UNSETUNICODE // from the mediainfo unicode workaround
#undef UNICODE
#undef UNSETUNICODE
#endif

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

