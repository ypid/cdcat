/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Christoph Thielecke (crissi99@gmx.de)
 License   : GPL
 Copyright : (C) 2011 Christoph Thielecke

 Info: read exif data from pictures
****************************************************************************/

#include "config.h"
#include "cdcat.h"
#include "cdcatmediainfo.h"
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#ifdef _WIN32
// #include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace MediaInfoNameSpace;
using namespace std;

static QStringList MediaInfoSupportedFileExtensions;
static bool mediaInfoLibInitDone = false;
static bool mediaInfoLibFound = false;
static MediaInfoNameSpace::MediaInfo *MediaInfoHandler=0;


/* convienent funcs for MediaInfo */
#if defined(_WIN32) || defined(MEDIAINFO_UNICODE)
QString fromMediaInfoStrtoQString(String str) {
	return QString::fromStdWString(str);
}
#else
QString fromMediaInfoStrtoQString(String str) {
	return QString::fromStdString(str);
}
#endif

#if defined(_WIN32) || defined(MEDIAINFO_UNICODE)
inline MediaInfoNameSpace::String toMediaInfoString(const QString &str) {
	return str.toStdWString();
}
#else
inline MediaInfoNameSpace::String toMediaInfoString(const QString &str) {
	return str.toStdString();
}
#endif



CdcatMediaInfo::CdcatMediaInfo ( void ) {
	filename = "";
	InfoText = "";
	if(!mediaInfoLibInitDone) {
		detectSupportedExtensions();
	}
}

CdcatMediaInfo::CdcatMediaInfo ( QString filename ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if(*DEBUG_INFO_ENABLED)
		cerr << "CdcatMediaInfo() filename: " << qPrintable(filename)<< endl;
	this->filename = filename;
	if(!mediaInfoLibInitDone) {
		detectSupportedExtensions();
	}
	if(mediaInfoLibFound)
		readCdcatMediaInfo();
}

bool CdcatMediaInfo::readCdcatMediaInfo(){
	
	// read data
	DEBUG_INFO_ENABLED = init_debug_info();
	
	if (filename == "") {
		if(*DEBUG_INFO_ENABLED)
			cout << "readCdcatMediaInfo() no filename set" << endl;
		return false;
	}
	MediaInfoNameSpace::String MediaInfoStr(toMediaInfoString(filename));
	MediaInfoHandler->Open(MediaInfoStr);
// 	MediaInfoHandler->Option("Language", "German;German" );
// 	if(*DEBUG_INFO_ENABLED)
// 		cout << "langs of mediainfo: " << MediaInfoHandler.Option("Language_Get").c_str() << endl;
	
	
// 	MediaInfoHandler.Option("Language", QString(QString("Image")+QString(";")+tr("Image")).toLocal8Bit().constData());
// 	MediaInfoHandler.Option("Language", "Image;Bild");
	String info = MediaInfoHandler->Inform();
	if(*DEBUG_INFO_ENABLED)
		cout << "mediainfo for " << filename.toLocal8Bit().constData() << ": " << fromMediaInfoStrtoQString(info).toLocal8Bit().constData() << endl;
	if(*DEBUG_INFO_ENABLED) {
	String ver = toMediaInfoString(QString("Info_Version"));
// 	cout << "mediainfo version: " << fromMediaInfoStrtoQString(MediaInfoHandler->Option(ver)).toLocal8Bit().constData() << endl;
// 	cout << "mediainfo codecs: " << MediaInfoHandler.Option("Info_Codecs").c_str() << endl;

	}
	

	InfoText = fromMediaInfoStrtoQString(info);
	
// 	size_t Stream_General_Number;
// 	size_t GeneralCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_General_Number);
// 	size_t Stream_Video_Number;
// 	size_t VideoCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_Video_Number);
// 	size_t Stream_Audio_Number;
// 	size_t AudioCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_Audio_Number);
// 	size_t Stream_Text_Number;
// 	size_t TextCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_Text_Number);
// 	size_t Stream_Chapters_Number;
// 	size_t ChaptersCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_Chapters_Number);
// 	size_t Stream_Menu_Number;
// 	size_t MenuCount =  MediaInfoHandler.Count_Get (Stream_General, Stream_Menu_Number);
// 
// 	if(*DEBUG_INFO_ENABLED)
// 		cout << "general: " << GeneralCount << ", video: " << VideoCount << ", audio: " << AudioCount << ", text: " << TextCount << ", chapters: " << ChaptersCount << ", menu: " << MenuCount << endl;
// 
// //   Stream_General    StreamKind = General. 
// //   Stream_Video    StreamKind = Video. 
// //   Stream_Audio    StreamKind = Audio. 
// //   Stream_Text    StreamKind = Text. 
// //   Stream_Chapters    StreamKind = Chapters. 
// //   Stream_Image    StreamKind = Image. 
// //   Stream_Menu

	return true;
}


CdcatMediaInfo::~CdcatMediaInfo ( void ) {
	// FIXME close & delete should be done at close cdcat
// 	MediaInfoHandler->Close();
	
}

bool CdcatMediaInfo::initMediaInfoLib() {
	DEBUG_INFO_ENABLED = init_debug_info();
#ifdef MEDIAINFO_STATIC
	MediaInfoHandler = new MediaInfo();
	if(*DEBUG_INFO_ENABLED) {
		cout << "initMediaInfoLib(): mediainfo lib version: " << fromMediaInfoStrtoQString(MediaInfoHandler->Option(toMediaInfoString(QString("Info_Version")))).split(" - ").at(1).toStdString()  << endl;
	}
#else
	int success = MediaInfoDLL_Load();
	if (MediaInfoDLL_IsLoaded()) {
		mediaInfoLibFound = true;
		if(*DEBUG_INFO_ENABLED)
			cout << "initMediaInfoLib(): loading mediainfo lib " << MEDIAINFODLL_NAME << " succeeded" << endl;
		MediaInfoHandler = new MediaInfo();
		if(*DEBUG_INFO_ENABLED) {
			cout << "initMediaInfoLib(): mediainfo lib version: " << fromMediaInfoStrtoQString(MediaInfoHandler->Option(toMediaInfoString(QString("Info_Version")))).split(" - ").at(1).toStdString()  << endl;
		}
	}
	else {
		if(*DEBUG_INFO_ENABLED)
			cout << "initMediaInfoLib(): loading mediainfo lib " << MEDIAINFODLL_NAME << " failed" << endl;
	}
#endif // MEDIAINFO_STATIC
	mediaInfoLibInitDone = true;
	return mediaInfoLibFound;
}

QString CdcatMediaInfo::getMediaInfoVersion() {
	if (mediaInfoLibFound)
		return fromMediaInfoStrtoQString(MediaInfoHandler->Option(toMediaInfoString(QString("Info_Version")))).split(" - ").at(1);
	else
		return QString("-");
}

bool CdcatMediaInfo::detectSupportedExtensions() {
	if (!mediaInfoLibInitDone) {
		// init lib
		mediaInfoLibFound = initMediaInfoLib();
	}
	
	if (mediaInfoLibFound) {
		MediaInfoSupportedFileExtensions.append("mkv");
		MediaInfoSupportedFileExtensions.append("mka");
		MediaInfoSupportedFileExtensions.append("mks");
		MediaInfoSupportedFileExtensions.append("ogg");
		MediaInfoSupportedFileExtensions.append("ogm");
		MediaInfoSupportedFileExtensions.append("avi");
		MediaInfoSupportedFileExtensions.append("wav");
		MediaInfoSupportedFileExtensions.append("mpeg");
		MediaInfoSupportedFileExtensions.append("mpg");
		MediaInfoSupportedFileExtensions.append("vob");
		MediaInfoSupportedFileExtensions.append("mp4");
		MediaInfoSupportedFileExtensions.append("mpgv");
		MediaInfoSupportedFileExtensions.append("mpv");
		MediaInfoSupportedFileExtensions.append("m1v");
		MediaInfoSupportedFileExtensions.append("m2v");
		MediaInfoSupportedFileExtensions.append("mp2");
		MediaInfoSupportedFileExtensions.append("mp3");
		MediaInfoSupportedFileExtensions.append("asf");
		MediaInfoSupportedFileExtensions.append("wma");
		MediaInfoSupportedFileExtensions.append("wmv");
		MediaInfoSupportedFileExtensions.append("qt");
		MediaInfoSupportedFileExtensions.append("mov");
		MediaInfoSupportedFileExtensions.append("rm");
		MediaInfoSupportedFileExtensions.append("rmvb");
		MediaInfoSupportedFileExtensions.append("ra");
		MediaInfoSupportedFileExtensions.append("ifo");
		MediaInfoSupportedFileExtensions.append("ac3");
		MediaInfoSupportedFileExtensions.append("dts");
		MediaInfoSupportedFileExtensions.append("aac");
		MediaInfoSupportedFileExtensions.append("ape");
		MediaInfoSupportedFileExtensions.append("mac");
		MediaInfoSupportedFileExtensions.append("flac");
		MediaInfoSupportedFileExtensions.append("dat");
		MediaInfoSupportedFileExtensions.append("aiff");
		MediaInfoSupportedFileExtensions.append("aifc");
		MediaInfoSupportedFileExtensions.append("au");
		MediaInfoSupportedFileExtensions.append("iff");
		MediaInfoSupportedFileExtensions.append("paf");
		MediaInfoSupportedFileExtensions.append("sd2");
		MediaInfoSupportedFileExtensions.append("irca");
		MediaInfoSupportedFileExtensions.append("w64");
		MediaInfoSupportedFileExtensions.append("mat");
		MediaInfoSupportedFileExtensions.append("pvf");
		MediaInfoSupportedFileExtensions.append("xi");
		MediaInfoSupportedFileExtensions.append("sds");
		MediaInfoSupportedFileExtensions.append("avr");
		MediaInfoSupportedFileExtensions.append("mkv");
		MediaInfoSupportedFileExtensions.append("ogg");
		MediaInfoSupportedFileExtensions.append("ogm");
		MediaInfoSupportedFileExtensions.append("riff");
		MediaInfoSupportedFileExtensions.append("mpeg");
		MediaInfoSupportedFileExtensions.append("m4a ");
		MediaInfoSupportedFileExtensions.append("mp2");
		MediaInfoSupportedFileExtensions.append("mp3");
		MediaInfoSupportedFileExtensions.append("wm");
		MediaInfoSupportedFileExtensions.append("qt");
		MediaInfoSupportedFileExtensions.append("real");
		MediaInfoSupportedFileExtensions.append("ifo");
		MediaInfoSupportedFileExtensions.append("ac3");
		MediaInfoSupportedFileExtensions.append("dts");
		MediaInfoSupportedFileExtensions.append("aac");
		MediaInfoSupportedFileExtensions.append("mac");
	}
	else {
		MediaInfoSupportedFileExtensions.clear();
	}

	return true;
}

QStringList CdcatMediaInfo::getSupportedExtensions() {
	return MediaInfoSupportedFileExtensions;
}

bool CdcatMediaInfo::getMediaInfoLibFound() {
	return mediaInfoLibFound;
}


QString CdcatMediaInfo::getInfo() {
	return InfoText;
}


/*end of file*/
