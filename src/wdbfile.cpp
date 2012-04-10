/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <stdio.h>
#include <ctype.h>

#include <QtXml/QXmlInputSource>
#include <zlib.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qregexp.h>


#include <QStringList>
#include <QtDebug>
#include <QBuffer>
#include <QVariant>
#include <QMessageBox>
#include <qtextcodec.h>
#include "wdbfile.h"
#include "adddialog.h"
#include "dbase.h"
#include "cdcat.h"


#define BUFFSIZE	8192
//#define AUTHOR "Pe'ter Dea'k  hyperr@freemail.hu"
#define AUTHOR "Christoph Thielecke  (crissi99@gmx.de)"

#include <iostream>
using namespace std;





char encodeHex[] = "0123456789ABCDEF";
/*************************************************************************/

int getTypeFS ( const char *str ) {
	if ( str == NULL )
		return -1;
	if ( !strcmp ( "CD"          , str ) )
		return CD;
	if ( !strcmp ( "DVD"         , str ) )
		return DVD;
	if ( !strcmp ( "HardDisc"    , str ) )
		return HARDDISC;
	if ( !strcmp ( "floppy"      , str ) )
		return FLOPPY;
	if ( !strcmp ( "NetworkPlace", str ) )
		return NETPLACE;
	if ( !strcmp ( "flashdrive"  , str ) )
		return FLASHDRV;
	if ( !strcmp ( "other"       , str ) )
		return OTHERD;
	return UNKNOWN;
}

const char *getMType ( int t ) {
	switch ( t ) {
		case UNKNOWN :
			return "unknown";
		case CD      :
			return "CD";
		case DVD     :
			return "DVD";
		case HARDDISC:
			return "HardDisc";
		case FLOPPY  :
			return "floppy";
		case NETPLACE:
			return "NetworkPlace";
		case FLASHDRV:
			return "flashdrive";
		case OTHERD  :
			return "other";
	}
	return NULL;
}

double getSizeFS ( const char *str ) {
	double  r = 0;
	char   s[10];

	strcpy ( s, "" );
	if ( str == NULL )
		return -1;
	QString unit;
	QStringList l = QString ( str ).simplified().split ( ' ' );
	unit = l.at ( 1 );
	r = l.at ( 0 ).toDouble();
	if ( l.size() != 2 )
		return -1;
	return r;
}

int   getSizetFS ( const char *str ) {
//   int scancount = 0;
	double  r;

	if ( str == NULL )
		return -1;
//     cerr << "str: \"" << str << "\"" << endl;
	QString unit;
	QStringList l = QString ( str ).simplified().split ( ' ' );
	unit = l.at ( 1 );
	r = l.at ( 0 ).toDouble();
	//cerr << "l: " << qPrintable(l.join(",")) << endl;
	//cerr << "unit: " << qPrintable(unit) << endl;

	if ( unit == "byte" )
		return UNIT_BYTE;
	if ( unit == "Kb" )
		return UNIT_KBYTE;
	if ( unit == "Mb" )
		return UNIT_MBYTE;
	if ( unit == "Gb" )
		return UNIT_GBYTE;
	if ( unit == "Tb" )
		return UNIT_TBYTE;
	return -1;
}

QString getSType ( int t, bool localized ) {
	if ( localized ) {
		switch ( t ) {
			case UNIT_BYTE :
				return QString ( " " ) + QString ( "Byte" );
			case UNIT_KBYTE:
				return QString ( " " ) + QString ( "KiB" );
			case UNIT_MBYTE:
				return QString ( " " ) + QString ( "MiB" );
			case UNIT_GBYTE:
				return QString ( " " ) + QString ( "GiB" );
			case UNIT_TBYTE:
				return QString ( " " ) + QString ( "TiB" );
//
		}
	}
	else {
		switch ( t ) {
			case UNIT_BYTE :
				return QString ( " byte" );
			case UNIT_KBYTE:
				return QString ( " Kb" );
			case UNIT_MBYTE:
				return QString ( " Mb" );
			case UNIT_GBYTE:
				return QString ( " Gb" );
			case UNIT_TBYTE:
				return QString ( " Tb" );
		}
	}
	return QString ( "" );
}

/*****************************************************************************
 WRITING FILE:
*****************************************************************************/
char * FileWriter::spg ( int spn ) {
	if ( spn >= 50 )
		return spgtable[49];
	return spgtable[spn];
}

FileWriter::FileWriter ( gzFile ff, bool nicefp, QString encoding ) {
	int i;
	XML_ENCODING = encoding;
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "using encoding for writing: " << qPrintable ( XML_ENCODING ) << endl;
	if ( XML_ENCODING != "UTF-8" ) {
		converter = QTextCodec::codecForName ( XML_ENCODING.toLocal8Bit() );
	}
	nicef = nicefp;
	level = 0;
	f = ff;

	spgtable = ( char ** ) malloc ( 50 * sizeof ( char * ) );
	for ( i = 0; i < 50; i++ )
		spgtable[i] = strdup ( QString().fill ( ' ', i ).toLocal8Bit().constData() );
}

FileWriter::~FileWriter() {
	int i;
	for ( i = 0; i < 50; i++ )
		free ( spgtable[i] );
	free ( spgtable );
}

QString FileWriter::to_cutf8 ( QString s ) {
	if ( XML_ENCODING != "UTF-8" ) {
		return converter->fromUnicode ( s.replace ( QRegExp ( "&" ), "&amp;" )
		                                .replace ( QRegExp ( "<" ), "&lt;" )
		                                .replace ( QRegExp ( ">" ), "&gt;" )
		                                .replace ( QRegExp ( "\"" ), "&quot;" ) );
	}
	else {
		return  s.replace ( QRegExp ( "&" ), "&amp;" )
		        .replace ( QRegExp ( "<" ), "&lt;" )
		        .replace ( QRegExp ( ">" ), "&gt;" )
		        .replace ( QRegExp ( "\"" ), "&quot;" );
	}
}

QString FileWriter::to_dcutf8 ( QDateTime d ) {
	char *ret;
	QString o ( "" );
	QDate qdd = d.date();
	QTime qtt = d.time();

	o.sprintf ( "%d-%02d-%02d %02d:%02d:%02d",
	            qdd.year(), qdd.month(), qdd.day(),
	            qtt.hour(), qtt.minute(), qtt.second() );
	if ( XML_ENCODING != "UTF-8" ) {
		return converter->fromUnicode ( o );
	}
	else {
		return o;
	}
}

//---------------------------------

void FileWriter::commentWriter ( QString& c ) {
	QString c1;
	if ( c.isEmpty() )
		return;

	c1 = to_cutf8 ( c );

	level++;
	gzprintf ( f, "%s<comment>", spg ( level ) );
	gzprintf ( f, "%s", c1.toLocal8Bit().data() );
	gzprintf ( f, "%s</comment>\n", spg ( level ) );

	level--;
}

void FileWriter::categoryWriter ( QString& c ) {
	QString c1;
	if ( c.isEmpty() )
		return;

	c1 = to_cutf8 ( c );

	level++;
	gzprintf ( f, "%s<category>", spg ( level ) );
	gzprintf ( f, "%s", c1.toLocal8Bit().data() );
	gzprintf ( f, "%s</category>\n", spg ( level ) );

	level--;
}

void FileWriter::archivecontentWriter ( QList<ArchiveFile>& archivecontent ) {

	if ( archivecontent.size() == 0 )
		return;
	level++;
	gzprintf ( f, "%s<archivecontent>", spg ( level ) );
	QString c;
	for ( int i = 0; i < archivecontent.size(); i++ ) {
		ArchiveFile af = archivecontent.at ( i );
		c += QString ( "\n" ) + af.toDbString();
	}
	QString c1 = to_cutf8 ( c );
	gzwrite ( f, c1.toLocal8Bit().data(), strlen ( c1.toLocal8Bit().data() ) );
	gzprintf ( f, "</archivecontent>\n" );

	level--;
}

void FileWriter::fileinfoWriter ( QString& c ) {
	QString c1;
	if ( c.isEmpty() )
		return;

	c1 = to_cutf8 ( c );

	level++;
	gzprintf ( f, "%s<fileinfo>", spg ( level ) );
	gzprintf ( f, "%s", c1.toLocal8Bit().data() );
	gzprintf ( f, "%s</fileinfo>\n", spg ( level ) );

	level--;
}

int  FileWriter::writeDown ( Node *source ) {
	int i = 0;
	switch ( source->type ) {
		case HC_UNINITIALIZED:
			return 1;

		case HC_CATALOG:
			writeHeader();
			i = writeCatalog ( source );
			break;
		case HC_MEDIA:
			i = writeMedia ( source );
			break;
		case HC_DIRECTORY:
			i = writeDirectory ( source );
			break;
		case HC_FILE:
			i = writeFile ( source );
			break;
		case HC_MP3TAG:
			i = writeMp3Tag ( source );
			break;
		case HC_CONTENT:
			i = writeContent ( source );
			break;
		case HC_EXIF:
			i = writeExif ( source );
			break;
		case HC_THUMB:
			i = writeThumb ( source );
			break;
		case HC_CATLNK:
			i = writeCatLnk ( source );
			break;
	}
	return i;
}

int  FileWriter::writeHeader ( void ) {
	level = 0;
	progress ( pww );
	gzprintf ( f, "<?xml version=\"1.0\" encoding=\"%s\"?>", XML_ENCODING.toLocal8Bit().constData() );
	gzprintf ( f, "\n<!DOCTYPE HyperCDCatalogFile>\n\n" );
	gzprintf ( f, "\n<!-- CD Catalog Database file, generated by CdCat " );
	gzprintf ( f, "\n     Homepage: %s  Author: %s", HOMEPAGE, AUTHOR );
	gzprintf ( f, "\n     Program-Version: %s, Database-Version: %s  -->\n\n", VERSION, DVERS );
	return 0;
}

int  FileWriter::writeCatalog ( Node *source ) {
	QString c1, c2, c3;

	c1 = to_cutf8 ( ( ( DBCatalog * ) ( source->data ) )->name );
	c2 = to_cutf8 ( ( ( DBCatalog * ) ( source->data ) )->owner );
	c3 = to_dcutf8 ( ( ( DBCatalog * ) ( source->data ) )->modification );
	progress ( pww );
	gzprintf ( f, "<catalog name=\"%s\" owner=\"%s\" time=\"%s\">\n",
	           c1.toLocal8Bit().data(), c2.toLocal8Bit().data(), c3.toLocal8Bit().data() );

	gzprintf ( f, "<datafile version=\"%s\"/>\n", DVERS );

	commentWriter ( ( ( DBCatalog * ) ( source->data ) )->comment );
	categoryWriter ( ( ( DBCatalog * ) ( source->data ) )->category );
	level++;
	if ( source->child != NULL )
		writeDown ( source->child );
	level--;
	gzprintf ( f, "</catalog>\n" );
	if ( source->next  != NULL )
		writeDown ( source->next );

	return 0;
}

int  FileWriter::writeMedia ( Node *source ) {
	QString c1, c2, c3;

	c1 = to_cutf8 ( ( ( DBMedia * ) ( source->data ) )->name );
	c2 = to_cutf8 ( ( ( DBMedia * ) ( source->data ) )->owner );
	c3 = to_dcutf8 ( ( ( DBMedia * ) ( source->data ) )->modification );
	progress ( pww );
	gzprintf ( f, "%s<media name=\"%s\" number=\"%d\" owner=\"%s\" type=\"%s\" time=\"%s\">\n",
	           spg ( level ), c1.toLocal8Bit().data(),
	           ( ( DBMedia * ) ( source->data ) )->number,
	           c2.toLocal8Bit().data() , getMType ( ( ( DBMedia * ) ( source->data ) )->type ), c3.toLocal8Bit().data() );

	commentWriter ( ( ( DBMedia * ) ( source->data ) )->comment );
	categoryWriter ( ( ( DBMedia * ) ( source->data ) )->category );

	//write borrowing info if exists
	if ( ! ( ( ( ( DBMedia * ) ( source->data ) )->borrowing ).isEmpty() ) ) {
		QString c4 = to_cutf8 ( ( ( DBMedia * ) ( source->data ) )->borrowing );
		gzprintf ( f, "%s<borrow>%s</borrow>\n",
		           spg ( level ), c4.toLocal8Bit().data() );
	}
	level++;
	if ( source->child != NULL )
		writeDown ( source->child );
	level--;
	gzprintf ( f, "%s</media>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );

	return 0;
}

int  FileWriter::writeDirectory ( Node *source ) {
	QString c1, c2;

	c1 = to_cutf8 ( ( ( DBDirectory * ) ( source->data ) )->name );
	c2 = to_dcutf8 ( ( ( DBDirectory * ) ( source->data ) )->modification );
	progress ( pww );
	gzprintf ( f, "%s<directory name=\"%s\" time=\"%s\">\n",
	           spg ( level ), c1.toLocal8Bit().data(), c2.toLocal8Bit().data() );

	commentWriter ( ( ( DBDirectory * ) ( source->data ) )->comment );
	categoryWriter ( ( ( DBDirectory * ) ( source->data ) )->category );

	level++;
	if ( source->child != NULL )
		writeDown ( source->child );
	level--;
	gzprintf ( f, "%s</directory>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );

	return 0;
}

int  FileWriter::writeFile ( Node *source ) {
	QString c1, c2;

	c1 = to_cutf8 ( ( ( DBFile * ) ( source->data ) )->name );
	c2 = to_dcutf8 ( ( ( DBFile * ) ( source->data ) )->modification );
	gzprintf ( f, "%s<file name=\"%s\" size=\"%.2f%s\" time=\"%s\">\n",
	           spg ( level ), c1.toLocal8Bit().data(),
	           ( ( DBFile * ) ( source->data ) )->size,
	           getSType ( ( ( ( DBFile * ) ( source->data ) )->sizeType ) ).toLocal8Bit().constData(), c2.toLocal8Bit().data() );

	commentWriter ( ( ( DBFile * ) ( source->data ) )->comment );
	categoryWriter ( ( ( DBFile * ) ( source->data ) )->category );
	archivecontentWriter ( ( ( DBFile * ) ( source->data ) )->archivecontent );
	fileinfoWriter ( ( ( DBFile * ) ( source->data ) )->fileinfo );
	level++;
	if ( source->child != NULL )
		writeDown ( source->child );
	if ( ( ( DBFile * ) ( source->data ) )->prop != NULL )
		writeDown ( ( ( DBFile * ) ( source->data ) )->prop );
	level--;
	gzprintf ( f, "%s</file>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );

	return 0;
}

int  FileWriter::writeMp3Tag ( Node *source ) {
	QString  c1, c2, c3, c4, c5;
	int tnum;


	c1 = to_cutf8 ( ( ( DBMp3Tag * ) ( source->data ) )->artist );
	c2 = to_cutf8 ( ( ( DBMp3Tag * ) ( source->data ) )->title );
	c3 = to_cutf8 ( ( ( DBMp3Tag * ) ( source->data ) )->album );
	c4 = to_cutf8 ( ( ( DBMp3Tag * ) ( source->data ) )->year );
	tnum = ( ( DBMp3Tag * ) ( source->data ) )->tnumber;
	gzprintf ( f, "%s<mp3tag artist=\"%s\" title=\"%s\" album=\"%s\" year=\"%s\" tnum=\"%d\">",
	           spg ( level ), c1.toLocal8Bit().data(), c2.toLocal8Bit().data(), c3.toLocal8Bit().data(), c4.toLocal8Bit().data(), tnum );

	c5 = to_cutf8 ( ( ( DBMp3Tag * ) ( source->data ) )->comment );
	gzprintf ( f, "%s", c5.toLocal8Bit().data() );
	gzprintf ( f, "%s</mp3tag>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );

	return 0;
}

int  FileWriter::writeContent ( Node *source ) {
	unsigned long i;
	unsigned char c;
	gzprintf ( f, "%s<content>", spg ( level ) );

	for ( i = 0; i < ( ( DBContent * ) ( source->data ) )->storedSize; i++ ) {
		c = ( ( DBContent * ) ( source->data ) )->bytes[i];
		gzputc ( f, encodeHex[ ( c & 0xF0 ) >> 4] );
		gzputc ( f, encodeHex[ c & 0x0F     ] );
	}
	gzprintf ( f, "%s</content>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );
	return 0;
}

int  FileWriter::writeExif ( Node *source ) {
	unsigned long i;
	gzprintf ( f, "%s<exif>", spg ( level ) );
	QStringList ExifDataList = ( ( DBExifData * ) ( source->data ) )->ExifDataList;
	QString c;
	for ( int i = 0; i < ExifDataList.size(); i++ ) {
		c += ExifDataList.at ( i );
		c += '\n';
	}
	QString c1 = to_cutf8 ( c );
	gzwrite ( f, c1.toLocal8Bit().data(), strlen ( c1.toLocal8Bit().data() ) );
	gzprintf ( f, "%s</exif>\n", spg ( level ) );
	if ( source->next  != NULL )
		writeDown ( source->next );
	return 0;
}

int  FileWriter::writeThumb ( Node *source ) {
	unsigned long i = 0;
	unsigned long datasize = 0;
	unsigned char c;
	gzprintf ( f, "%s<thumb>", spg ( level ) );
	//std::cout << "save image size: " << ( ( DBThumb * ) ( source->data ) )->ThumbImage.width() << "x" << ( ( DBThumb * ) ( source->data ) )->ThumbImage.height() << std::endl;
	QByteArray byteArray;
	QBuffer buffer ( &byteArray );
	buffer.open ( QIODevice::WriteOnly );
	( ( DBThumb * ) ( source->data ) )->ThumbImage.save ( &buffer, "PNG" );

	//std::cerr << "qbuffer size: " << buffer.size() << std::endl;
	buffer.close();
	//std::cerr << "byteArray size: " << byteArray.size() << std::endl;

	gzprintf ( f, "<![CDATA[" );
	char *bits = byteArray.data();
	datasize = byteArray.size();
	for ( i = 0; i < datasize; i++ ) {
		c = bits[i];
//         gzputc ( f,encodeHex[ ( c & 0xF0 ) >>4] );
//         gzputc ( f,encodeHex[ c & 0x0F     ] );
	}

	QByteArray byteArray2 = byteArray.toHex();
// 	datasize = strlen(imgdata);
	gzwrite ( f,  byteArray2.constData(), byteArray2.size() );
	gzprintf ( f, "]]>" );
	gzprintf ( f, "</thumb>\n" );


// FILE *tempfile;
// tempfile = fopen("/tmp/imagedata1.txt", "w");
//     for ( i=0;i< datasize;i++ ) {
//         c = bits[i];
// 	fwrite(&encodeHex[ ( c & 0xF0 ) >>4], 1, 1, tempfile);
// 	fwrite(&encodeHex[ c & 0x0F], 1, 1, tempfile );
//     }
// fclose(tempfile);
//
// FILE *tempfile2;
// tempfile2 = fopen("/tmp/imagedata1_raw.txt", "w");
//     for ( i=0;i< datasize;i++ ) {
//         c = bits[i];
// 	fwrite(bits, 1, 1, tempfile2);
//     }
// fclose(tempfile2);

	if ( source->next  != NULL )
		writeDown ( source->next );
	return 0;
}

int  FileWriter::writeCatLnk ( Node *source ) {
	QString c1;
	QString wr = ( ( DBCatLnk * ) ( source->data ) )->name;
	progress ( pww );
	c1 = to_cutf8 ( wr.remove ( 0, 1 ) ); //to remove the leading @
	gzprintf ( f, "%s<catlnk name=\"%s\" location=\"%s\" >\n",
	           spg ( level ), c1.toLocal8Bit().data(),
	           ( ( DBCatLnk * ) ( source->data ) )->location );


	commentWriter ( ( ( DBCatLnk * ) ( source->data ) )->comment );
	categoryWriter ( ( ( DBCatLnk * ) ( source->data ) )->category );
	gzprintf ( f, "%s</catlnk>\n", spg ( level ) );

	if ( source->next  != NULL )
		writeDown ( source->next );
	return 0;
}

/*****************************************************************************
 READING FILE:
*****************************************************************************/
int clearbuffer = 1;
unsigned long buffpos = 0;


QString FileReader::get_cutf8 ( QString s ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	QString ret;

	if ( s.isEmpty() )
		return QString ( "" );
	//if(*DEBUG_INFO_ENABLED)
	//	cerr<<"Start-converting |"<<s<<"|"<<endl;
	if ( XML_ENCODING != "UTF8" ) {
		ret = converter->toUnicode ( s.toLocal8Bit() );
		return ret;
	}
	else {
		return QString ( s );
	}
	//if(*DEBUG_INFO_ENABLED)
	//	cerr<<"End-converting   |" << qPrintable ( ret ) <<"|"<<endl;
	//ret =  QString(s);


}

QDateTime FileReader::get_dcutf8 ( QString s ) {
	QDateTime r;
	int year, month, day;
	int hour, minute, second;
	char p[4];
	char p2[10];
	char p3[10];

	DEBUG_INFO_ENABLED = init_debug_info();
//     if (*DEBUG_INFO_ENABLED)
// 	std::cerr <<"FileReader::get_dcutf8 datestr:"<<  s  <<endl;

	if ( sscanf ( s.toLocal8Bit().constData(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second ) == 6 ) {
//  if (*DEBUG_INFO_ENABLED)
// 	std::cerr <<"FileReader::get_dcutf8 new format found"<<endl;
		r.setDate ( QDate ( year, month, day ) );
		r.setTime ( QTime ( hour, minute, second ) );
		return r;
	}

	/* OLD VERSION FOUND */
	if ( sscanf ( s.toLocal8Bit().constData(), "%s %02d %02d:%02d %04d", p, &day, &hour, &minute, &year ) == 5 ) {
// 	if (*DEBUG_INFO_ENABLED)
// 	std::cerr <<"FileReader::get_dcutf8 old format found"<<endl;
		if ( !strcmp ( p, "Jan" ) )
			month = 1;
		if ( !strcmp ( p, "Feb" ) )
			month = 2;
		if ( !strcmp ( p, "Mar" ) )
			month = 3;
		if ( !strcmp ( p, "Apr" ) )
			month = 4;
		if ( !strcmp ( p, "May" ) )
			month = 5;
		if ( !strcmp ( p, "Jun" ) )
			month = 6;
		if ( !strcmp ( p, "Jul" ) )
			month = 7;
		if ( !strcmp ( p, "Aug" ) )
			month = 8;
		if ( !strcmp ( p, "Sep" ) )
			month = 9;
		if ( !strcmp ( p, "Oct" ) )
			month = 10;
		if ( !strcmp ( p, "Nov" ) )
			month = 11;
		if ( !strcmp ( p, "Dec" ) )
			month = 12;
		r.setDate ( QDate ( year, month, day ) );
		r.setTime ( QTime ( hour, minute, 0 ) );
		return r;
	}

	/* date format is: "Apr.12 15:43 2001" => "MonthShort.DayNum HourNum,MinuteNum YearLong" */
	if ( sscanf ( s.toLocal8Bit().constData(), "%s %s %04d", p2, p3, &year ) == 3 ) {
// 	        if (*DEBUG_INFO_ENABLED)
// 	 	std::cerr <<"FileReader::get_dcutf8 old format2 found"<<endl;
		if ( QString ( p2 ).contains ( '.' ) ) {
			//if (*DEBUG_INFO_ENABLED)
			//std::cerr <<"FileReader::get_dcutf8 p2 0: " << qPrintable(QString(p2).split('.').at(0)) << ", p2 1: " << qPrintable(QString(p2).split('.').at(1))<<endl;
			strcpy ( p, QString ( p2 ).split ( '.' ).at ( 0 ).toLocal8Bit().data() );
			day = QString ( p2 ).split ( '.' ).at ( 1 ).toInt();
		}

		if ( QString ( p3 ).contains ( ':' ) ) {
			if ( *DEBUG_INFO_ENABLED )
				//std::cerr <<"FileReader::get_dcutf8 p3 0: " << qPrintable(QString(p3).split(':').at(0)) << ", p3 1: " << qPrintable(QString(p3).split(':').at(1))<<endl;
				hour = QString ( p3 ).split ( ':' ).at ( 0 ).toInt();
			minute = QString ( p3 ).split ( ':' ).at ( 1 ).toInt();
			second = 0;
		}


//        if (*DEBUG_INFO_ENABLED)
// 	std::cerr <<"FileReader::get_dcutf8 p: " << p  << ", p2: " << p2 << ", p3: " << p3 << ", day: " << day << ", minute: " << minute << ", hour: " << hour <<", year: " << year << endl;

		if ( !strcmp ( p, "Jan" ) )
			month = 1;
		if ( !strcmp ( p, "Feb" ) )
			month = 2;
		if ( !strcmp ( p, "Mar" ) )
			month = 3;
		if ( !strcmp ( p, "Apr" ) )
			month = 4;
		if ( !strcmp ( p, "May" ) )
			month = 5;
		if ( !strcmp ( p, "Jun" ) )
			month = 6;
		if ( !strcmp ( p, "Jul" ) )
			month = 7;
		if ( !strcmp ( p, "Aug" ) )
			month = 8;
		if ( !strcmp ( p, "Sep" ) )
			month = 9;
		if ( !strcmp ( p, "Oct" ) )
			month = 10;
		if ( !strcmp ( p, "Nov" ) )
			month = 11;
		if ( !strcmp ( p, "Dec" ) )
			month = 12;
		r.setDate ( QDate ( year, month, day ) );
		r.setTime ( QTime ( hour, minute, 0 ) );
		return r;
	}

	r.setDate ( QDate::currentDate() );
	r.setTime ( QTime::currentTime() );
	return r;

// !!! Conversion ntfrom the old versions !!!
	/*
	char *getCurrentTime(void)
	 {
	  char *m=new char[30];
	  QTime t;
	  QDate d;
	  t = QTime::currentTime();
	  d = QDate::currentDate();
	    sprintf(m,// "%s.%d %d:%d %d"
	    "%s %02d %02d:%02d %d",(const char *)
	    (shortMonthName0(d.month())),d.day(),t.hour(),t.minute(),d.year());


	  return recodeI(m,&dbb6);
	 }
	char *getTime(QDateTime dt)
	 {
	  char *m=new char[30];
	  QTime t;
	  QDate d;
	  t = dt.time();
	  d = dt.date();
	  sprintf(m,// "%s.%d %d:%d %d"
	           "%s %02d %02d:%02d %d",(const char *)
	    (shortMonthName0(d.month())),d.day(),t.hour(),t.minute(),d.year());

	  return recodeI(m,&dbb6);
	 }
	*/
}


/* try to decode a hexadecimal byte to char very fast */
unsigned char decodeHexa ( char a, char b ) {
	/* possible values a,b: "0123456789ABCDEF"  !!!*/
	unsigned char r = 0;
	if ( a >= 'A' )
		r = ( ( a - 'A' ) + 10 );
	else
		r = ( a - '0' );
	r <<= 4;
	if ( b >= 'A' )
		r += ( ( b - 'A' ) + 10 );
	else
		r += ( b - '0' );
	return r;
}


QString FileReader::getStr2 ( const QXmlAttributes &atts, char *what, char *err ) {
	int i;
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( atts.length() == 0 ) {
		//errormsg = QString ( "Line %1: %2" ).arg ( XML_GetCurrentLineNumber ( *pp ) ).arg ( err );
		errormsg = QString ( "%2" ).arg ( err );
		error_found = 1;
		cerr << "ERROR: " << qPrintable(errormsg) << endl;
		return NULL;
	}

	bool attribute_found = false;
	for ( i = 0; i < atts.length(); i++ ) {
		if ( QString ( what ) == atts.qName ( i ) )
			attribute_found = true;
		return atts.value ( QString ( what ) );
	}
	if ( !attribute_found ) {
		//errormsg = QString ( "Line %1: %2:I can't find \"%3\" attribute." )
		//	.arg ( XML_GetCurrentLineNumber ( *pp ) ).arg ( err ).arg ( what );
		errormsg = QString ( "%1:I can't find \"%2\" attribute." ).arg ( err ).arg ( what );
		cerr << "ERROR: " << qPrintable(errormsg) << endl;
		error_found  = 1;
		return   NULL;
	}


	return NULL;
}

double FileReader::getDouble2 ( const QXmlAttributes &atts, char *what, char *err ) {
	double r;
	int i;

	if ( atts.length() == 0 ) {
//         errormsg = QString ( "Line %1: %2" ).arg ( XML_GetCurrentLineNumber ( *pp ) ).arg ( err );
		errormsg = QString ( "%1" ).arg ( err );
		cerr << "ERROR: " << qPrintable(errormsg) << endl;
		error_found = 1;
		return 0;
	}
	bool attribute_found = false;
	for ( i = 0; i < atts.length(); i++ ) {
		if ( QString ( what ) == atts.qName ( i ) )
			attribute_found = true;
		return atts.value ( QString ( what ) ).toDouble();
	}
	if ( !attribute_found ) {
//             errormsg = QString ( "Line %1: %2,I can't find \"%3\" attribute" )
//                        .arg ( XML_GetCurrentLineNumber ( *pp ) ).arg ( err ).arg ( what );
		errormsg = QString ( "%1,I can't find \"%2\" attribute" )
		           .arg ( err ).arg ( what );
		cerr << "ERROR: " << qPrintable(errormsg) << endl;
		error_found  = 1;
		return   0;
	}
	if ( 1 != sscanf ( atts.value ( i ).toLocal8Bit().constData(), "%f", & r ) ) {
//                 errormsg = QString ( "Line %1: %2:I can't understanding \"%3\" attribute." )
//                            .arg ( XML_GetCurrentLineNumber ( *pp ) ).arg ( err ).arg ( what );
		errormsg = QString ( "%1:I can't understanding \"%2\" attribute." )
		           .arg ( err ).arg ( what );
		cerr << "ERROR: " << qPrintable(errormsg) << endl;
		error_found  = 1;
		return   0;
	}


	return 0;
}



int FileReader::isthere ( const char **from, char *what ) {
	int i;

	if ( from == NULL )
		return 0;
	for ( i = 0;; i += 2 ) {
		if ( from[i] == NULL )
			return 0;
		if ( !strcmp ( from[i], what ) )
			return 1;
	}
	return 0;
}

#define FREA ((FileReader *)(data))

DBMp3Tag *tmp_tagp = NULL;


void getCharDataFromXML ( void *data, const char *s, int len ) {
	int copylen = len;

	if ( FREA->error_found )
		return;
	/*That case I found an error in file -> needs exit the pasing as fast as I can.*/

	if ( clearbuffer ) {
		buffpos = 0;
		clearbuffer = 0;
	}

	if ( buffpos + len >= ( MAX_STORED_SIZE * 2 ) )
		copylen = ( MAX_STORED_SIZE * 2 ) - buffpos;

	memcpy ( ( FREA->dataBuffer ) + buffpos, s, sizeof ( char ) * copylen );
	buffpos += copylen;
	FREA->dataBuffer[buffpos] = '\0';
}


int FileReader::readFrom ( Node *source, bool skipDuplicatesOnInsert ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	char line[80]; // encoding detect
	error_found = 0;
	int done = 0;
	sp  = sb_backup  = source;
	this->skipDuplicatesOnInsert = skipDuplicatesOnInsert;

	if ( *DEBUG_INFO_ENABLED )
		cerr << "Start Cycle" << endl;

	// detect encoding
	int len = gzread ( f, line, 80 );
	//cerr <<"line: " << line <<endl;
	QString line2 ( line );
	//if (*DEBUG_INFO_ENABLED)
	//	std::cerr <<"line2: " << line2.constData() <<endl;
	QStringList encodingline_parts = line2.split ( '"' );
	XML_ENCODING = encodingline_parts.at ( 3 );
	if ( XML_ENCODING != "UTF-8" )
		converter = QTextCodec::codecForName ( XML_ENCODING.toLocal8Bit() );

	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "detected encoding: " << XML_ENCODING.toAscii().constData() << endl;
	gzrewind ( f );

	/* now read the buffer */
	len = 0;
	int readcount = 0;
	linecount = 0;
	long long int offset = 0;
	char tmpbuffer[2048];

	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "start reading file..." << endl;

	pww->showProgress = true;
	pww->steps = allocated_buffer_len;
	pww->setProgressText ( DataBase::tr ( "Reading file, please wait..." ) );
	pww->setCancel ( true );
	while ( len != allocated_buffer_len ) {
		readcount = gzread ( f, tmpbuffer, 2048 );
		len += readcount;
		//if(*DEBUG_INFO_ENABLED)
		//  cerr << "readcount: " << readcount << endl;
		for ( int i = 0; i < readcount; i++ )
			dataBuffer[i + offset] = tmpbuffer[i];
// 		strncat(dataBuffer, tmpbuffer, 2048);
		offset += readcount;
		progress ( pww, len );
		if ( pww->doCancel ) {
			errormsg = DataBase::tr ( "You have cancelled catalog reading." );
			done = 1;
		}
	}

	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "read done: " << len << " of " << allocated_buffer_len << " bytes" << endl;

	linecount += QString ( dataBuffer ).count ( '\n' );
	if ( *DEBUG_INFO_ENABLED )
		cerr << "linecount: " << linecount << endl;

	CdCatXmlHandler *handler = new CdCatXmlHandler ( this );
	pww->steps = linecount;

	handler->setPww ( pww );
	xmlReader.setContentHandler ( handler );
	xmlReader.setErrorHandler ( handler );

	QXmlInputSource mysource;
	if ( XML_ENCODING == "UTF-8" ) {
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "set data source text..." << endl;
		mysource.setData ( QByteArray ( dataBuffer ) );
	}
	else {
		pww->setProgressText ( DataBase::tr ( "Converting to unicode, please wait..." ) );
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "set data source to utf8 converted text..." << endl;
		mysource.setData ( converter->toUnicode ( QByteArray ( dataBuffer ) ) );
	}
	
	pww->setCancel ( true );
	pww->setProgressText ( DataBase::tr ( "Parsing file, please wait..." ) );
	
	parseresult = xmlReader.parse ( mysource );
	cerr << "parse result: " << parseresult << ", errormsg: " <<qPrintable(errormsg) << endl;
	
	if ( ! parseresult) {
		if ( pww->doCancel ) {
			errormsg = DataBase::tr ( "You have cancelled catalog reading." );
		}
		//else {
		//	errormsg = DataBase::tr ( "Parse error" ) + "\n";
		//}
	}
	else {
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "parsing was successful" << endl;
	}
	pww->showProgress = false;
	
	delete handler;
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "parsing done" << endl;

	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "End Cycle" << endl;
	
	return done;
}

FileReader::FileReader ( gzFile ff, char *allocated_buffer, long long int allocated_buffer_len, int ins ) {
	f = ff;
	insert = ins;
	converter = QTextCodec::codecForName ( "utf8" );
	dataBuffer = allocated_buffer;
	this->allocated_buffer_len = allocated_buffer_len;
}




/* ********************************************************************* */
QString catname = "";

QString FileReader::getCatName ( void ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	catname = "";
	
	char line[80]; // encoding detect
	error_found = 0;
	// detect encoding
	int len = gzread ( f, line, 80 );
	//cerr <<"line: " << line <<endl;
	QString line2 ( line );
	//if (*DEBUG_INFO_ENABLED)
	//	std::cerr <<"line2: " << line2.constData() <<endl;
	QStringList encodingline_parts = line2.split ( '"' );
	XML_ENCODING = encodingline_parts.at ( 3 );
	
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "deteced encoding: " << XML_ENCODING.toAscii().constData() << endl;
	gzrewind ( f );
	//XML_SetEncoding ( p, XML_ENCODING.toAscii().constData() );
	//converter = QTextCodec::codecForName ( XML_ENCODING );
	
	/* now read the buffer */
	len = 0;
	int readcount = -1;
	char tmpbuffer[1024];
	
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "start reading file..." << endl;
	while ( readcount == -1 || readcount > 0 ) {
		progress ( pww );
		readcount = gzread ( f, tmpbuffer, 1024 );
		len += readcount;
		//if(*DEBUG_INFO_ENABLED)
		//  cerr << "readcount: " << readcount << endl;
		strncat ( dataBuffer, tmpbuffer, 1024 );
	}
	
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "read done: " << len << " of " << allocated_buffer_len << " bytes" << endl;
	
	sp = NULL;
	
	CdCatXmlHandler *handler = new CdCatXmlHandler ( this, true ); // only catalog name
	
	handler->setPww ( pww );
	pww->showProgress = true;
	pww->steps = linecount;

	xmlReader.setContentHandler ( handler );
	xmlReader.setErrorHandler ( handler );
	
	QXmlInputSource mysource;
	if ( XML_ENCODING.toAscii().constData() == "UTF-8" ) {
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "set data source text..." << endl;
		mysource.setData ( QString ( dataBuffer ) );
	}
	else {
		//pww->setProgressText(DataBase::tr("Converting to unicode, please wait..."));
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "set data source to utf8  converted text..." << endl;
		mysource.setData ( converter->toUnicode ( QString ( dataBuffer ).toLocal8Bit() ) );
	}
	
	pww->setProgressText ( DataBase::tr ( "Parsing file, please wait..." ) );
	
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "starting parsing" << endl;
	
	parseresult = xmlReader.parse ( mysource );
	if ( !parseresult  ) {
		errormsg = QString ( "Parse error\n" );
	}
	else {
		if ( *DEBUG_INFO_ENABLED )
			std::cerr << "parsing was successful" << endl;
	}
	
	pww->showProgress = false;
	error_found = 0;
	delete handler;
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "parsing done" << endl;
	
	if ( error_found != 1 || catname != "" ) {
		return catname;
	}
	return "";
}

CdCatXmlHandler::CdCatXmlHandler ( FileReader *r, bool onlyCatalog ) {
	this->r = r;
	data = r;
	this->onlyCatalog = onlyCatalog;
}

CdCatXmlHandler::~CdCatXmlHandler() {

}

void CdCatXmlHandler::setPww ( PWw *pww ) {
	this->lines = lines;
	this->pww = pww;
}

bool CdCatXmlHandler::characters ( const QString & ch ) {
	if ( pww->doCancel ) {
		return false;
	}
	if ( ch.length() == 0 ) {
		return true;
	}
	//DEBUG_INFO_ENABLED = init_debug_info();
	//if(*DEBUG_INFO_ENABLED)
	//	cerr << "CdCatXmlHandler::characters: " << qPrintable(ch) << endl;
	currentText += ch;
	progress ( pww, locator->lineNumber() );
	return true;
}

bool CdCatXmlHandler::startElement ( const QString & namespaceURI, const QString & localName, const QString & el, const QXmlAttributes & attr ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	QString ts1, ts2, ts3, ts4, ts5, ts6;
	QDateTime td1;
	double tf1;
	int   ti1;
	currentText = "";
	
	if ( FREA->error_found ) {
		cerr << "current errormsg:" << qPrintable ( FREA->errormsg ) << endl;
		return false;
	}
	/*That case I found an error in file -> needs exit the pasing as fast as I can.*/
	//if(*DEBUG_INFO_ENABLED)
	//	cerr <<"Start_start:"<<qPrintable(el)<<endl;
	
	//if(*DEBUG_INFO_ENABLED)
	//cerr <<"line number:"<<locator->lineNumber()<<endl;
	progress ( pww, locator->lineNumber() );
	
	clearbuffer = 1;
	if ( el == "catalog" ) {
		if ( FREA->insert ) {
			//if(*DEBUG_INFO_ENABLED)
			//	cerr <<"insert catalog not supported" <<endl;
			//return false;
			return true;
		}
		if ( onlyCatalog ) {
			//catname = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name","Error while parsing \"catalog\" node" ) );
			catname = FREA->getStr2 ( attr, "name", "Error while parsing \"catalog\" node" );
			return true;
		}
		
		( ( DBCatalog * ) ( ( FREA->sp )->data ) ) ->name =
			//FREA->get_cutf8 ( FREA->getStr2 ( attr,"name","Error while parsing \"catalog\" node" ) );
		FREA->getStr2 ( attr, "name", "Error while parsing \"catalog\" node" );
		if ( FREA->error_found ) {
			cerr <<"Error while parsing \"catalog\" node, el: "<<qPrintable(el)<<endl;
			return false;
		}
		
		// crissi
		catname = ( ( DBCatalog * ) ( ( FREA->sp )->data ) ) ->name;
		
		( ( DBCatalog * ) ( ( FREA->sp )->data ) ) ->owner =
			//FREA->get_cutf8 ( FREA->getStr2 ( attr,"owner","Error while parsing \"catalog\" node" ));
		        FREA->getStr2 ( attr, "owner", "Error while parsing \"catalog\" node" );
		if ( FREA->error_found ) {
			cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
			return false;
		}
		
		( ( DBCatalog * ) ( ( FREA->sp )->data ) ) ->modification =
		        FREA->get_dcutf8 ( FREA->getStr2 ( attr, "time", "Error while parsing \"catalog\" node" ) );
		if ( FREA->error_found ) {
			cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
			return false;
		}
	}
	else {
		if ( el == "datafile" ) {
			Node *tmp = FREA->sp;
			if ( FREA->insert )  {
				//if(*DEBUG_INFO_ENABLED)
				//	cerr <<"insert catalog not supported" <<endl;
				//return false;
				return true;
			}
			if ( FREA->sp == NULL ) {
				cerr <<"datafile but no datafile node, el: "<<qPrintable(el)<<endl;
				return false;
			}
			
			while ( tmp->type != HC_CATALOG )
				tmp = tmp->parent;
			
			( ( DBCatalog * ) ( tmp->data ) ) ->fileversion =
				//FREA->get_cutf8 ( FREA->getStr2 ( attr,"version","Error while parsing \"datafile\" node" ) );
			FREA->getStr2 ( attr, "version", "Error while parsing \"datafile\" node" );
			if ( FREA->error_found ) {
				cerr <<"Error while parsing \"datafile\" node, el: "<<qPrintable(el)<<endl;
				return false;
			}
		}
		else {
			if ( el == "media" ) {
				Node *tt = FREA->sp->child;
				if ( tt == NULL )
					FREA->sp->child = tt = new Node ( HC_MEDIA, FREA->sp );
				else {
					while ( tt->next != NULL )
						tt = tt->next;
					tt->next =  new Node ( HC_MEDIA, FREA->sp );
					tt = tt->next;
				}
				
				if ( FREA->insert ) {
					int i, newnum, ser = 0;
					Node *ch = tt->parent->child;
					QString newname;
					
					//newname = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name"  ,"Error while parsing \"media\" node" ) );
					newname = FREA->getStr2 ( attr, "name"  , "Error while parsing \"media\" node" );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					if ( newname.startsWith ( "@" ) ) {
						FREA->errormsg = QString ("The media name begin with \"@\".\n\
It is disallowed since cdcat version 0.98 !\n\
Please change it with an older version or rewrite it in the xml file!" );
						
						FREA->error_found = 1;
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					newnum = ( int ) FREA->getDouble2 ( attr, "number", "Error while parsing \"media\" node" );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					/*make the number unique*/
					for ( i = 1; i == 1; ) {
						i = 0;
						for ( ch = tt->parent->child; ch != NULL; ch = ch->next ) {
							if ( ch->data != NULL )
								if ( ( ( DBMedia * ) ( ch->data ) )->number ==  newnum )
									i = 1;
						}
						if ( i )
							newnum++;
					}
					
					/*make the name unique*/
					for ( i = 1; i == 1; ) {
						i = 0;
						for ( ch = tt->parent->child; ch != NULL; ch = ch->next ) {
							if ( ch->data != NULL )
								if ( ( ( DBMedia * ) ( ch->data ) )->name == newname )
									i = 1;
						}
						if ( i ) {
							if ( !FREA->skipDuplicatesOnInsert ) {
								ser++;
								// old behavior
								newname.append ( "#" + QString().setNum ( ser ) );
							}
							else {
								/* FIXME: crash... */
								// dont change name
								FREA->sp =  ch;
								if ( *DEBUG_INFO_ENABLED )
									cerr << "end_start:" << qPrintable ( el ) << endl;
								return true;
							}
						}
					}
					
					/*Fill data part:*/
					//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"owner" ,"Error while parsing \"media\" node" ) );
					ts1 = FREA->getStr2 ( attr, "owner" , "Error while parsing \"media\" node" );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					td1 = FREA->get_dcutf8 ( FREA->getStr2 ( attr, "time"  , "Error while parsing \"media\" node" ) );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					ti1 = getTypeFS ( FREA->getStr2 ( attr, "type"  , "Error while parsing \"media\" node" ).toLocal8Bit().constData() );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					if ( ti1 == UNKNOWN ) {
						FREA->errormsg = QString ( "Unknown media type in the file. (\"%1\")" )
						                 .arg ( FREA->getStr2 ( attr, "type"  , "Error while parsing \"media\" node" ).toLocal8Bit().constData() );
						
						FREA->error_found = 1;
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					tt->data = ( void * ) new DBMedia ( newname, newnum, ts1, ti1, "", td1 );
				}
				else {
					/*Fill data part:*/
					//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name"  ,"Error while parsing \"media\" node" ) );
					ts1 = FREA->getStr2 ( attr, "name"  , "Error while parsing \"media\" node" );
					//ts2 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"owner" ,"Error while parsing \"media\" node" ) );
					ts2 = FREA->getStr2 ( attr, "owner" , "Error while parsing \"media\" node" );
					tf1 = FREA->getDouble2 ( attr, "number", "Error while parsing \"media\" node" );
					td1 = FREA->get_dcutf8 ( FREA->getStr2 ( attr, "time"  , "Error while parsing \"media\" node" ) );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					if ( ts2.startsWith ( "@" ) ) {
						FREA->errormsg = QString ( "The media name begin with \"@\".\nIt is disallowed since cdcat version 0.98 !\nPlease change it with an older version or rewrite it in the xml file!" ) ;
						FREA->error_found = 1;
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					ti1 = getTypeFS ( FREA->getStr2 ( attr, "type"  , "Error while parsing \"media\" node" ).toLocal8Bit().constData() );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					if ( ti1 == UNKNOWN ) {
						FREA->errormsg = QString ( "Line %1: Unknown media type in the file. (\"%1\")" )
						                 .arg ( FREA->getStr2 ( attr, "type"  , "Error while parsing \"media\" node" ).toLocal8Bit().constData() );
						FREA->error_found = 1;
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					
					tt->data = ( void * ) new DBMedia ( ts1, ( int ) tf1, ts2, ti1, "", td1 );
				}
				/*Make this node to the actual node:*/
				FREA->sp = tt;
			}
			else {
				if ( el == "directory" ) {
					Node *tt = FREA->sp->child;
					
					if ( tt == NULL )
						FREA->sp->child = tt = new Node ( HC_DIRECTORY, FREA->sp );
					else {
						while ( tt->next != NULL )
							tt = tt->next;
						tt->next =  new Node ( HC_DIRECTORY, FREA->sp );
						tt = tt->next;
					}
					/*Fill data part:*/
					//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name"  ,"Error while parsing \"directory\" node" ) );
					ts1 = FREA->getStr2 ( attr, "name"  , "Error while parsing \"directory\" node" ) ;
					td1 = FREA->get_dcutf8 ( FREA->getStr2 ( attr, "time" , "Error while parsing \"directory\" node" ) );
					if ( FREA->error_found ) {
						cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
						return false;
					}
					tt->data = ( void * ) new DBDirectory ( ts1, td1, "" );
					
					/*Make this node to the actual node:*/
					FREA->sp = tt;
				}
				else {
					if ( el == "file" ) {
						Node *tt = FREA->sp->child;
						
						if ( tt == NULL )
							FREA->sp->child = tt = new Node ( HC_FILE, FREA->sp );
						else {
							while ( tt->next != NULL )
								tt = tt->next;
							tt->next =  new Node ( HC_FILE, FREA->sp );
							tt = tt->next;
						}
						/*Fill data part:*/
						//if(*DEBUG_INFO_ENABLED)
						//	cerr <<"1"<<endl;
						//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name"  ,"Error while parsing \"file\" node" ) );
						ts1 = FREA->getStr2 ( attr, "name"  , "Error while parsing \"file\" node" );
						//if(*DEBUG_INFO_ENABLED)
						//	cerr <<"2"<<endl;
						
						if ( FREA->error_found ) {
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						
						td1 = FREA->get_dcutf8 ( FREA->getStr2 ( attr, "time"  , "Error while parsing \"file\" node" ) );
						//if(*DEBUG_INFO_ENABLED)
						//	cerr <<"3"<<endl;
						
						if ( FREA->error_found ) {
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						//if(*DEBUG_INFO_ENABLED)
						//	cerr <<"4"<<endl;
						
						tf1 = getSizeFS ( FREA->getStr2 ( attr, "size", "Error while parsing \"file\" node" ).toLocal8Bit().constData() );
						//if(*DEBUG_INFO_ENABLED)
						//	cerr <<"5"<<endl;
						
						if ( FREA->error_found ) {
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						if ( tf1 == -1 ) {
							FREA->errormsg = QString ( "Unknown size data in file node. (\"%1\")" )
							                 .arg ( FREA->getStr2 ( attr, "size", "Error while parsing \"file\" node" ) );
							FREA->error_found = 1;
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						
						ti1 = getSizetFS ( FREA->getStr2 ( attr, "size", "Error while parsing \"file\" node" ).toLocal8Bit().constData() );
						if ( FREA->error_found ) {
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						if ( ti1 == -1 ) {
							FREA->errormsg = QString ( "Unknown size type in file node. (\"%1\")" )
							                 .arg ( FREA->getStr2 ( attr, "size", "Error while parsing \"file\" node" ).toLocal8Bit().constData() );
							FREA->error_found = 1;
							cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
							return false;
						}
						
						//std::cerr << "file size: " << tf1 << " " << ti1 << std::endl;
						bool addFile = true;
						if ( FREA->insert && FREA->skipDuplicatesOnInsert ) {
							// search for file in catalog
							if ( *DEBUG_INFO_ENABLED )
								std::cerr << "testing if file already in DB: " << qPrintable ( tt->parent->getFullPath() + "/" + ts1 ) << std::endl;

							if ( isFileInDB ( FREA->sb_backup, tt->parent->getFullPath() + "/" + ts1, tf1 ) ) {
								if ( *DEBUG_INFO_ENABLED )
									std::cerr << "file already in DB (skip adding): " << qPrintable ( tt->parent->getFullPath() + "/" + ts1 ) << std::endl;
								addFile = false;
							}
						}
						if ( addFile ) {
							tt->data = ( void * ) new DBFile ( ts1, td1, "", tf1, ti1 );
						}
						
						/*Make this node to the actual node:*/
						FREA->sp = tt;
					}
					else {
						if ( el == "mp3tag" ) {
							Node *tt = ( ( DBFile * ) ( FREA->sp->data ) )->prop;
							if ( tt == NULL )
								( ( DBFile * ) ( FREA->sp->data ) )->prop = tt = new Node ( HC_MP3TAG, FREA->sp );
							else {
								while ( tt->next != NULL )
									tt = tt->next;
								tt->next =  new Node ( HC_MP3TAG, FREA->sp );
								tt = tt->next;
							}
							/*Fill data part:*/
							//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"artist"  ,"Error while parsing \"mp3tag\" node" ));
							ts1 = FREA->getStr2 ( attr, "artist"  , "Error while parsing \"mp3tag\" node" );
							//ts2 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"title"   ,"Error while parsing \"mp3tag\" node" ) );
							ts2 = FREA->getStr2 ( attr, "title"   , "Error while parsing \"mp3tag\" node" );
							//ts3 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"album"  ,"Error while parsing \"mp3tag\" node" ) );
							ts3 = FREA->getStr2 ( attr, "album"  , "Error while parsing \"mp3tag\" node" );
							//ts4 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"year"   ,"Error while parsing \"mp3tag\" node" ) );
							ts4 = FREA->getStr2 ( attr, "year"   , "Error while parsing \"mp3tag\" node" );
							if ( FREA->error_found ) {
								cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
								return false;
							}
							ti1 = 0;
							ti1 = FREA->getDouble2 ( attr, "tnum"   , "Error while parsing \"mp3tag\" node" );
							// this is NO error
							FREA->error_found = 0;
							
							tmp_tagp = new DBMp3Tag ( ts1, ts2, "no comment", ts3 , ts4, ti1 );
							
							tt->data = ( void * ) tmp_tagp;
							/*I don't make this node to the actual node because this won't be parent.*/
						}
						else {
							if ( el == "catlnk" ) {
								char *readed_loc;
								Node *tt = FREA->sp->child;
								
								if ( tt == NULL )
									FREA->sp->child = tt = new Node ( HC_CATLNK, FREA->sp );
								else {
									while ( tt->next != NULL )
										tt = tt->next;
									tt->next =  new Node ( HC_CATLNK, FREA->sp );
									tt = tt->next;
								}
								/*Fill data part:*/
								//ts1 = FREA->get_cutf8 ( FREA->getStr2 ( attr,"name"  ,"Error while parsing \"catlnk\" node" ) );
								ts1 = FREA->getStr2 ( attr, "name"  , "Error while parsing \"catlnk\" node" );
								
								readed_loc = mstr ( FREA->getStr2 ( attr, "location" , "Error while parsing \"catlnk\" node" ).toLocal8Bit().constData() );
								if ( FREA->error_found ) {
									cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
									return false;
								}
								
								tt->data = ( void * ) new DBCatLnk ( ts1.prepend ( "@" ), readed_loc, NULL );
								
								/*Make this node to the actual node:*/
								FREA->sp = tt;
							}
							else {
								if ( el == "content" ) {
									/*nothing*/
								}
								else {
									if ( el == "comment" ) {
										/*nothing*/
									}
									else {
										if ( el == "category" ) {
											/*nothing*/
										}
										else {
											if ( el == "archivecontent" ) {
												/*nothing*/
											}
											else {
												if ( el == "borrow" ) {
													/*nothing*/
												}
												else {
													if ( el == "fileinfo" ) {
														/*nothing*/
													}
													else {
														if ( el == "exif" ) {
															/*nothing*/
														}
														else {
															if ( el == "thumb" ) {
																/*nothing*/
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	//if(*DEBUG_INFO_ENABLED)
	//	cerr <<"end_start:"<<qPrintable(el)<<endl;
	return true;
}

/*********************************************************************/
bool CdCatXmlHandler::endElement ( const QString & namespaceURI, const QString & localName, const QString & el ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	void *data = r;
	
	//if ( *DEBUG_INFO_ENABLED )
	//	cerr << "Start_end:" << qPrintable ( el ) << endl;
	
	/*That case I found an error in file -> needs exit the pasing as fast as I can.*/
	if ( FREA->error_found || pww->doCancel ) {
		return false;
	}
	
	progress ( pww, locator->lineNumber() );

//     getCharDataFromXML ( r,currentText.toLocal8Bit().data(),currentText.length());
	
	if ( el == "catalog" ) {
		//End parsing !
	}
	else {
		if ( el == "datafile" ) {
			//End parsing !
		}
		else {
			if ( el == "media" ) {
				if ( FREA->sp == NULL ) {
					cerr << "Start_end: media but media node NULL, el: " << qPrintable ( el ) << endl;
					return false;
				}
				/*Restore the parent node tho the actual node:*/
				FREA->sp = FREA->sp->parent;
			}
			else {
				if ( el == "directory" ) {
					if ( FREA->sp == NULL ) {
						cerr << "Start_end: directory but directory node NULL, el: " << qPrintable ( el ) << endl;
						return false;
					}
					/*Restore the parent node tho the actual node:*/
					FREA->sp = FREA->sp->parent;
				}
				else {
					if ( el == "file" ) {
						if ( FREA->sp == NULL ) {
							cerr << "Start_end: file but file node NULL, el: " << qPrintable ( el ) << endl;
							return false;
						}
						/*Restore the parent node tho the actual node:*/
						FREA->sp = FREA->sp->parent;
					}
					else {
						if ( el == "mp3tag" ) {
							//strcpy(tmp_tagp->comment,currentText);
							//if(*DEBUG_INFO_ENABLED)
							//	cerr <<"1"<<endl;
							//if(*DEBUG_INFO_ENABLED)
							//	cerr <<"2"<<endl;
							tmp_tagp->comment = FREA->get_cutf8 ( currentText );
							//if(*DEBUG_INFO_ENABLED)
							//	cerr <<"3"<<endl;
							tmp_tagp = NULL;
							//if(*DEBUG_INFO_ENABLED)
							//	cerr <<"4"<<endl;
						}
						else {
							if ( el == "catlnk" ) {
								/*Restore the parent node tho the actual node:*/
								if ( FREA->sp == NULL ) {
									cerr << "Start_end: catlnk but catlnk node NULL, el: " << qPrintable ( el ) << endl;
									return false;
								}
								FREA->sp = FREA->sp->parent;
							}
							else {
								if ( el == "content" ) {
									unsigned char *bytes;
									unsigned long rsize, i;
									
									Node *tt = ( ( DBFile * ) ( FREA->sp->data ) )->prop;
									if ( tt == NULL )
										( ( DBFile * ) ( FREA->sp->data ) )->prop = tt = new Node ( HC_CONTENT, FREA->sp );
									else {
										while ( tt->next != NULL )
											tt = tt->next;
										tt->next =  new Node ( HC_CONTENT, FREA->sp );
										tt = tt->next;
									}
									/*Fill data part:*/
									rsize = ( strlen ( currentText.toLocal8Bit().data() ) / 2 );
									if ( *DEBUG_INFO_ENABLED )
										cerr << "Start_end: content size: " << currentText.size() << " (raw: " << rsize << ")" << endl;
									bytes = new unsigned char[ rsize + 1];
									
									char *tempbuffer = currentText.toLocal8Bit().data();
									bytes = new unsigned char[ ( rsize = ( strlen ( tempbuffer ) / 2 ) ) + 1];
									for ( i = 0; i < rsize; i++ )
										bytes[i] = decodeHexa ( tempbuffer[i * 2], tempbuffer[i * 2 + 1] );
									bytes[rsize] = '\0';
									//bytes = (unsigned char *)QByteArray::fromHex(currentText.toLocal8Bit()).constData();
									//rsize = QByteArray::fromHex(currentText.toLocal8Bit()).size();
									tt->data = ( void * ) new DBContent ( bytes, rsize );
									
								}
								else {
									if ( el == "comment" ) {
										while ( currentText.length() > 0 && currentText.at ( 0 ) == '\n' )
											currentText = currentText.right ( currentText.length() - 1 );
										if ( FREA->sp == NULL ) {
											cerr << "Start_end: comment but comment node NULL, el: " << qPrintable ( el ) << endl;
											return false;
										}
										switch ( FREA->sp->type ) {
											case HC_CATALOG  :
//             ( ( DBCatalog    * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
												( ( DBCatalog    * ) ( FREA->sp->data ) ) -> comment = currentText ;
												break;
											case HC_MEDIA    :
//             ( ( DBMedia      * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
												( ( DBMedia      * ) ( FREA->sp->data ) ) -> comment = currentText ;
												break;
											case HC_DIRECTORY:
//             ( ( DBDirectory * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
												( ( DBDirectory * ) ( FREA->sp->data ) ) -> comment = currentText;
												break;
											case HC_FILE     :
//             ( ( DBFile      * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
												( ( DBFile      * ) ( FREA->sp->data ) ) -> comment = currentText ;
												break;
											case HC_CATLNK   :
//             ( ( DBCatLnk    * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
												( ( DBCatLnk    * ) ( FREA->sp->data ) ) -> comment = currentText ;
												break;
											case HC_MP3TAG:
												FREA->errormsg = QString ( "It isnt allowed comment node in mp3tag node." );
												FREA->error_found = 1;
												cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
												break;
										}
									}
									else {
										if ( el == "category" ) {
											if ( FREA->sp == NULL ) {
												cerr << "Start_end: catagory but file catagory NULL, el: " << qPrintable ( el ) << endl;
												return false;
											}
											switch ( FREA->sp->type ) {
												case HC_CATALOG  :

//             ( ( DBCatalog    * ) ( FREA->sp->data ) ) -> category = FREA->get_cutf8 ( currentText );
													( ( DBCatalog    * ) ( FREA->sp->data ) ) -> category = currentText ;
													break;
												case HC_MEDIA    :
//             ( ( DBMedia      * ) ( FREA->sp->data ) ) -> category = FREA->get_cutf8 ( currentText );
													( ( DBMedia      * ) ( FREA->sp->data ) ) -> category = currentText ;
													break;
												case HC_DIRECTORY:
//             ( ( DBDirectory * ) ( FREA->sp->data ) ) -> category = FREA->get_cutf8 ( currentText );
													( ( DBDirectory * ) ( FREA->sp->data ) ) -> category = currentText;
													break;
												case HC_FILE     :
//             ( ( DBFile      * ) ( FREA->sp->data ) ) -> category = FREA->get_cutf8 ( currentText );
													( ( DBFile      * ) ( FREA->sp->data ) ) -> category = currentText;
													break;
												case HC_CATLNK   :
//             ( ( DBCatLnk    * ) ( FREA->sp->data ) ) -> category = FREA->get_cutf8 ( currentText );
													( ( DBCatLnk    * ) ( FREA->sp->data ) ) -> category = currentText;
													break;
												case HC_MP3TAG:
													FREA->errormsg = QString ( "It isnt allowed category node in mp3tag node." );
													FREA->error_found = 1;
													cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
													break;
											}
										}
										else {
											if ( el == "archivecontent" ) {
												if ( FREA->sp == NULL ) {
													cerr << "Start_end: archivecontent but archivecontent node NULL, el: " << qPrintable ( el ) << endl;
													return false;
												}
												switch ( FREA->sp->type ) {
													case HC_FILE:
														if ( !currentText.isEmpty() ) {
// 			std::cout << "currentText: " << qPrintable(currentText) << std::endl;
															QList<ArchiveFile> ArchiveFileList;
															ArchiveFileList.clear();
															QStringList textList = currentText.split ( '\n');
															for ( int i = 0; i < textList.size(); i++ ) {
																if ( !textList.at ( i ).isEmpty() && textList.at ( i ).trimmed().size() > 4 ) {
// 					std::cout << "line: \"" << qPrintable(textList.at(i)) << "\""<< std::endl;
																	ArchiveFile af;
																	af.setDbString ( textList.at ( i ) );
																	ArchiveFileList.append ( af );
																}
															}
															( ( DBFile      * ) ( FREA->sp->data ) ) -> archivecontent = ArchiveFileList;
														}
														break;
													default:
														;
												}
											}
											else {
												if ( el == "fileinfo" ) {
													while ( currentText.length() > 0 && currentText.at ( 0 ) == '\n' )
														currentText = currentText.right ( currentText.length() - 1 );
													if ( FREA->sp == NULL ) {
														cerr << "Start_end: fileinfo but fileinfo node NULL, el: " << qPrintable ( el ) << endl;
														return false;
													}
													switch ( FREA->sp->type ) {
														case HC_FILE     :
															//             ( ( DBFile      * ) ( FREA->sp->data ) ) -> fileinfo = FREA->get_cutf8 ( currentText );
															( ( DBFile      * ) ( FREA->sp->data ) ) -> fileinfo = currentText ;
															break;
														default:
															;
															break;
													}
												}
												else {
													if ( el == "exif" ) {
														while ( currentText.length() > 0 && currentText.at ( 0 ) == '\n' )
															currentText = currentText.right ( currentText.length() - 1 );
														if ( FREA->sp == NULL ) {
															cerr << "Start_end: exif but exif node NULL, el: " << qPrintable ( el ) << endl;
															return false;
														}
														switch ( FREA->sp->type ) {
															case HC_FILE     :
//             ( ( DBFile      * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
																Node *tt = ( ( DBFile * ) ( FREA->sp->data ) )->prop;
																if ( tt == NULL )
																	( ( DBFile * ) ( FREA->sp->data ) )->prop = tt = new Node ( HC_EXIF, FREA->sp );
																else {
																	while ( tt->next != NULL )
																		tt = tt->next;
																	tt->next =  new Node ( HC_EXIF, FREA->sp );
																	tt = tt->next;
																}
																FREA->error_found = 0;
																
																DBExifData *tmp_exifdata = new DBExifData ( currentText.split ( '\n' ) );
																tt->data = ( void * ) tmp_exifdata;
																break;
														}
													}
													else {
														if ( el == "thumb" ) {
															// while (currentText.length() > 0 && currentText.at(0) == '\n')
															// 	currentText = currentText.right(currentText.length()-1);
															if ( FREA->sp == NULL ) {
																cerr << "Start_end: thumb but thumb node NULL, el: " << qPrintable ( el ) << endl;
																return false;
															}
															switch ( FREA->sp->type ) {
																case HC_FILE     :
																	//( ( DBFile      * ) ( FREA->sp->data ) ) -> comment = FREA->get_cutf8 ( currentText );
																	unsigned char *bytes;
																	unsigned long rsize, i;
																	
																	Node *tt = ( ( DBFile * ) ( FREA->sp->data ) )->prop;
																	if ( tt == NULL )
																		( ( DBFile * ) ( FREA->sp->data ) )->prop = tt = new Node ( HC_THUMB, FREA->sp );
																	else {
																		while ( tt->next != NULL )
																			tt = tt->next;
																		tt->next =  new Node ( HC_THUMB, FREA->sp );
																		tt = tt->next;
																	}
																	/*Fill data part:*/
																	QImage tmpThumbImage;
																	tmpThumbImage.loadFromData ( QByteArray::fromHex ( currentText.toLocal8Bit() ), "PNG" );
																	if ( *DEBUG_INFO_ENABLED )
																		cout << "found thumb: (" << QByteArray::fromHex ( currentText.toLocal8Bit() ).size() << " bytes), size: " << tmpThumbImage.width() << "x" << tmpThumbImage.height() << std::endl;
																	tt->data = ( void * ) new DBThumb ( tmpThumbImage );
																	free ( bytes );
																	break;
															}
														}
														else {
															if ( el == "borrow" ) {
																if ( FREA->sp == NULL ) {
																	cerr << "Start_end: borrow but borrow node NULL, el: " << qPrintable ( el ) << endl;
																	return false;
																}
																switch ( FREA->sp->type ) {
																	case HC_CATALOG  :
																	case HC_DIRECTORY:
																	case HC_FILE     :
																	case HC_MP3TAG   :
																	case HC_CATLNK   :
																		FREA->errormsg = QString ( "The borrow node only allowed in media node." );
																		cerr << qPrintable(FREA->errormsg) << " el: "<<qPrintable(el)<<endl;
																		FREA->error_found = 1;
																		break;
																	case HC_MEDIA    :
																		//( ( DBMedia      * ) ( FREA->sp->data ) ) -> borrowing = FREA->get_cutf8 ( currentText );
																		( ( DBMedia      * ) ( FREA->sp->data ) ) -> borrowing = currentText ;
																		break;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	clearbuffer = 1;
	currentText = "";
	
	//if(*DEBUG_INFO_ENABLED)
	//	cerr <<"end_end:"<<qPrintable(el)<<endl;
	return true;
}

bool CdCatXmlHandler::fatalError ( const QXmlParseException &exception ) {
	std::cerr << "Parse error at line " << exception.lineNumber()
	          << ", " << "column " << exception.columnNumber() << ": "
	          << qPrintable ( exception.message() ) << std::endl;
	return true;
}

void CdCatXmlHandler::setDocumentLocator ( QXmlLocator * locator ) {
	this->locator = locator;
	QXmlDefaultHandler::setDocumentLocator ( locator );
}

bool CdCatXmlHandler::isFileInDB ( Node *root, QString Path, double size ) {
	testFileInDBPath = Path;
	testFileInDBSize = size;
	testFileInDBFound = false;
	analyzeNodeIsFileinDB ( FREA->sb_backup );
	return testFileInDBFound;
}

int  CdCatXmlHandler::analyzeNodeIsFileinDB ( Node *n, Node *pa ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( testFileInDBFound )
		return 0;
	switch ( n->type ) {
		case HC_CATALOG:
			analyzeNodeIsFileinDB ( n->child );
			return 0;
		case HC_MEDIA:
			progress ( pww );
			analyzeNodeIsFileinDB ( n->child );
			analyzeNodeIsFileinDB ( n->next );
			return 0;
		case HC_DIRECTORY:
			progress ( pww );
			analyzeNodeIsFileinDB ( n->child );
			analyzeNodeIsFileinDB ( n->next );
			return 0;
		case HC_CATLNK:
			analyzeNodeIsFileinDB ( n->next );
			return 0;
		case HC_FILE:
			if ( *DEBUG_INFO_ENABLED )
// 				std::cout << "testing file: " << qPrintable(n->getNameOf()) << " name: " << qPrintable(fdp->mainw->guis->standON->getNameOf()) << " <=> " << qPrintable(n->getNameOf()) << ". size: " <<  (( DBFile * ) ( fdp->mainw->guis->standON ))->size << " <=> " << ( ( DBFile * ) ( n->data ) )->size << ", size type: " <<  (( DBFile * ) ( fdp->mainw->guis->standON ) )->sizeType << " <=> " << ( ( DBFile * ) ( n->data ) )->sizeType  << std::endl;
				std::cout << "testing file: " << qPrintable ( n->getNameOf() ) << " path: " << qPrintable ( testFileInDBPath ) << " <=> " << qPrintable ( n->getFullPath() ) << std::endl;
			if ( testFileInDBPath ==  n->getFullPath() ) {
				if ( *DEBUG_INFO_ENABLED ) {
					std::cout << "filepath match!" << std::endl;
					std::cout << "size: " <<  testFileInDBSize << " <=> " << ( ( DBFile * ) ( n->data ) )->size << std::endl;
				}
				if ( testFileInDBSize == ( ( DBFile * ) ( n->data ) )->size )  {
					if ( *DEBUG_INFO_ENABLED )
						std::cout << "filesize match!" << std::endl;
					testFileInDBFound = true;
					return 0;
				}
			}
			analyzeNodeIsFileinDB ( n->next );
			return 0;
	}
	return 0;
}


// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
