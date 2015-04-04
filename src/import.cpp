/****************************************************************************
Hyper's CD Catalog
A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke (crissi99@gmx.de)
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#include <qmessagebox.h>
#include <qfile.h>
#include <QTextStream>
#include <qdatetime.h>
#include <QProgressDialog>
#include <QList>
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlAttributes>
#include <iostream>
#include <stdio.h>

using namespace std;

#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qstringlist.h>
#include <QTextStream>
#include <QLineEdit>

#include "import.h"
#include "importdialog.h"
#include "mainwidget.h"
#include "dbase.h"
#include "cdcat.h"
#include "config.h"

lineObject::lineObject ( QString medianame, QString path, QString filename,
                         double size, QDateTime datetime, QString comment, QString information, QString category, QList<ArchiveFile> archivecontent, bool is_archive ) {
	this->medianame = medianame;
	this->path = path;
	this->filename = filename;
	this->size = size;
	this->datetime = datetime;
	this->comment = comment;
	this->information = information;
	this->category = category;
	for ( int i = 0; i < archivecontent.size(); i++ ) {
		this->archivecontent.append ( archivecontent.at ( i ) );
	}
	this->is_archive = is_archive;
}

lineObject::~lineObject() {
}

lineObject::lineObject ( const lineObject& newobj ) : QObject(), information("") {
	medianame = newobj.medianame;
	path = newobj.path;
	filename = newobj.filename;
	size = newobj.size;
	datetime = newobj.datetime;
	comment = newobj.comment;
	category = newobj.category;
	for ( int i = 0; i < newobj.archivecontent.size(); i++ ) {
		archivecontent.append ( newobj.archivecontent.at ( i ) );
	}
	is_archive = newobj.is_archive;
}

lineObject& lineObject::operator= ( const lineObject& newobj ) {
	medianame = newobj.medianame;
	path = newobj.path;
	filename = newobj.filename;
	size = newobj.size;
	datetime = newobj.datetime;
	comment = newobj.comment;
	category = newobj.category;
	archivecontent.clear();
	for ( int i = 0; i < newobj.archivecontent.size(); i++ ) {
		archivecontent.append ( newobj.archivecontent.at ( i ) );
	}
	is_archive = newobj.is_archive;
	return *this;
}

QString lineObject::getMediaName() {
	return medianame;
}

QString lineObject::getPath() {
	return path;
}

QString lineObject::getFileName() {
	return filename;
}

double lineObject::getSize() {
	return size;
}

QList<ArchiveFile> lineObject::getArchiveFileContent() {
	return archivecontent;
}

QDateTime lineObject::getDateTime() {
	return datetime;
}

QString lineObject::getComment() {
	return comment;
}

QString lineObject::getInformation() {
	return information;
}


QString lineObject::getCategory() {
	return category;
}

bool lineObject::isArchive() {
	return is_archive;
}

void lineObject::setArchiveFileContent ( QList< ArchiveFile > archivecontent ) {
	this->archivecontent = archivecontent;
}

void lineObject::appendArchiveFileContent ( ArchiveFile af ) {
	archivecontent.append ( af );
}


/*
 * import a file with following format:
 * <media>/<path>/..../<file><separator><size><separator><day/month/year><space><hour>:<minute>:<second>
 * the hashmark '#' can be used as comment in csv file.
 *
 */

importGtktalogCsv::importGtktalogCsv ( GuiSlave * parent, QString separator, QString filename, bool createdatabase, bool correctbadstyle, QString csvtype, QString cdcat_pattern, QString extra_media_name ) {
	this->filename = filename;
	this->correctbadstyle = correctbadstyle;
	this->createdatabase = createdatabase;
	this->separator = separator;
	DEBUG_INFO_ENABLED = init_debug_info();

	if ( filename.isEmpty() ) {
		// file dialog canceled
		return;
	}
	if ( parent->mainw->db == NULL )
		parent->newEvent();
	else {
		if ( createdatabase ) {
			while ( parent->closeEvent() != 0 )
				;
			parent->newEvent();
		}
	}

	if ( parent->mainw->db == NULL ) {
		return;
	}

	guislave = parent;
	db = parent->mainw->db;
	mediacount = 0;
	filecount = 0;
	dircount = 0;
	refreshcount = 0;
	int linecount = 0;
	int lines = 0;

	QApplication::setOverrideCursor ( Qt::WaitCursor );
	guislave->panelsOFF();

	QFile f ( filename );

	if ( f.open ( QIODevice::ReadOnly ) ) {
		QTextStream in ( &f );
		while ( !in.atEnd() ) {
			QString line = in.readLine();
			lines ++;
		}
		f.close();
		if ( *DEBUG_INFO_ENABLED )
			cerr << "importGtktalogCsv:: " << lines << " lines found." << endl;
	}

    // FIXME: Why not use else?
	if  ( !f.open ( QIODevice::ReadOnly ) ) {      // file open failed
		if ( f.error() == QFile::ReadError)
			QMessageBox::critical ( 0, tr ( "file read error" ), tr ( "Could not read file" ) );

		if ( f.error() == QFile::FatalError )
			QMessageBox::critical ( 0, tr ( "Fatal error" ), tr ( "Fatal error occurred." ) );

		if ( f.error() == QFile::OpenError )
			QMessageBox::critical ( 0, tr ( "file open error" ), tr ( "Could not open file" ) );
		return;
	}

	QProgressDialog *progress = new QProgressDialog ( tr ( "Importing CSV..." ), tr ( "Cancel" ), 1, lines );
	progress->setCancelButton ( 0 );

	QTextStream t ( &f );              // use a text stream
	QString medianame = "";
	QString comment = "";
	QString category = "";
	QDateTime datetime;
	QString import_filename = filename;

	medialines = new QList < lineObject > ();
	//medialines->setAutoDelete( true );  // the list owns the objects
	int sizefactor = 1; /* factor for file size, 1 = 1 byte, 1024 = 1 kilobyte)*/

	QRegExp regex_pattern;
	QList<pattern_entry> pattern_list;

	if ( csvtype == "generic_plaintext_regex" ) {
		QList<QList<QString>> regex_pattern_list = getImportPatternList();

		QString pattern = "";
		int index = 0;
		int last_index = 0;
		for(int i=0;i< regex_pattern_list.size(); i++) {
			index = 0;
			while (index < cdcat_pattern.size()) {
				//pattern = pattern.replace(0, regex_pattern_list.at(i)[1].size(), regex_pattern_list.at(i)[1]);
				int new_index = cdcat_pattern.indexOf(regex_pattern_list.at(i).at(0), index, Qt::CaseInsensitive);

				if (new_index > -1) {
					if ( *DEBUG_INFO_ENABLED )
						std::cout << "pattern [" << qPrintable(regex_pattern_list.at(i).at(0)) << "] lookup result: " << new_index << std::endl;

					last_index = new_index;
					index = new_index+ regex_pattern_list.at(i).at(0).size()+1;
					bool inserted = false;
					for(int j=0;j< pattern_list.size(); j++) {
						if (!inserted) {
							if (pattern_list.at(j).index > new_index) {
								pattern_list.insert(j, pattern_entry(new_index, regex_pattern_list.at(i).at(0), regex_pattern_list.at(i).at(1)));
								inserted = true;
								//std::cout << "insert pattern [" << qPrintable(regex_pattern_list.at(i).at(0)) << "] at " << j << std::endl;
							}
						}
					}
					if (!inserted) {
						pattern_list.append(pattern_entry(new_index, regex_pattern_list.at(i).at(0), regex_pattern_list.at(i).at(1)));
						//std::cout << "append pattern [" << qPrintable(regex_pattern_list.at(i).at(0)) << "] at end" << std::endl;
					}
				}
				else {
					index = cdcat_pattern.size();
				}
			}
		}

		if ( *DEBUG_INFO_ENABLED )
			std::cout << "cdcat_pattern: " << qPrintable(cdcat_pattern) << std::endl;
		for(int i=0;i< pattern_list.size(); i++) {
			if ( *DEBUG_INFO_ENABLED )
				std::cout << "regex_pattern[" << i  << "]: " << pattern_list.at(i).index << " / [" << qPrintable(pattern_list.at(i).pattern_name) << "] (" << qPrintable(pattern_list.at(i).pattern) << ")"<< std::endl;
			pattern += pattern_list.at(i).pattern;
		}
		pattern = "^" + pattern + "$";

		if ( *DEBUG_INFO_ENABLED )
			std::cout << "line pattern: " << qPrintable(pattern) << std::endl;

		regex_pattern = QRegExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp2);

	}

	while ( !t.atEnd() ) {
		QString line;
		QString fullpath;
		QString path;
		QString dirpath;
		double size = 0;
		//QDate date;

		QString new_medianame;
		QString datetimestring;
		QString pathsep = "/";
		bool line_ok = false;

		line = t.readLine();  // line of text excluding '\n'

		// check for multiple lines of a entry...
		if ( csvtype == "whereisit" ) {
			while ( line.right ( 1 ) != separator && !t.atEnd() ) {
				if ( *DEBUG_INFO_ENABLED )
					cerr << "importGtktalogCsv:: multi line entry found" << endl;
				line += '\n';
				line += t.readLine();  // line of text excluding '\n'
			}
		}

		if ( !line.startsWith ( "#" ) && !line.isEmpty() ) {
			line_ok = true;
		}

		//if(*DEBUG_INFO_ENABLED) {
		//	cerr << "line raw: " << qPrintable(line) << endl;
		//}

		if ( line_ok ) {
			if ( line.contains ( '\\' ) )
				pathsep = "\\";
			if ( correctbadstyle ) {
				//if(*DEBUG_INFO_ENABLED) {
				//	cerr << "correcting bad style"<< endl;
				//}
				//     QMessageBox::warning (0, "wrong", line);
				int idx = 0;
				while ( idx != -1 ) {
					idx = line.indexOf ( QString ( separator + pathsep ), idx );
					if ( idx != -1 ) {
						line.replace ( idx, QString ( separator + pathsep ).length(), "/" );
					}
				}
				idx = 0;
				while ( idx != -1 ) {
					idx = line.indexOf ( QString ( pathsep + separator ), idx );
					if ( idx != -1 ) {
						line.replace ( idx, QString ( pathsep + separator ).length(), "/" );
					}
				}
				//   QMessageBox::warning (0, "fine", line);
			}

			if ( csvtype == "gtktalog" ) {
				int mediaindex = line.indexOf ( pathsep, 0 );
				int pathindex = line.indexOf ( separator, mediaindex + 1 );
				fullpath = ( line.mid ( mediaindex, pathindex - mediaindex ) );

				if ( pathsep == "\\" )
					fullpath = fullpath.replace ( "\\", "/" );
				if ( fullpath.at ( 0 ) != '/' )
					fullpath = "/" + fullpath;
				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;
				new_medianame = ( line.left ( mediaindex ) );
				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

				path = fullpath.mid ( mediaindex + 1, medianame.length() + 1 );
				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

				int sizeindex = line.indexOf ( separator, pathindex + 1 );
				QString sizestring = line.mid ( pathindex + 1, sizeindex - pathindex - 1 ).trimmed();
				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv sizestring: " << qPrintable(sizestring) << endl;
				size = ( ( line.mid ( pathindex + 1, sizeindex - pathindex - 1 ) ).trimmed() ).toDouble();

				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv size: " << size << endl;

				datetimestring = ( line.mid ( sizeindex + 1, line.length() ) ).trimmed();
				if ( *DEBUG_INFO_ENABLED )
					cerr << "importGtktalogCsv datetimestring: " << qPrintable ( datetimestring ) << endl;

				// date is normally day/month/year hour:minute:second
				QString datesep = "/";

				// date in other format: day.month.year hour:minute:second
				if ( datetimestring.contains ( '.' ) )
					datesep = ".";
				int dayindex = datetimestring.indexOf ( datesep );
				int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
				int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
				int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
				int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );

				int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
				QString day_ = ( datetimestring.mid ( 0, dayindex ) );
				int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
				int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
				//bool validDate = date.setYMD( year, month, day );

				int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
				int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();

				int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
				QDate date ( year, month, day );
				QTime time ( hour, minute, second );

				datetime = QDateTime ( date, time );

				int fileindex = fullpath.lastIndexOf ( '/' );
				//if(*DEBUG_INFO_ENABLED)
				//  cerr << "importGtktalogCsv fileindex: " << fileindex << endl;

				QString dirpath = "";
				if ( fileindex != 0 )
					dirpath = fullpath.mid ( 1, fileindex - 1 );
				QString filename = fullpath.mid ( fileindex + 1, fullpath.length() - fileindex );

				if ( *DEBUG_INFO_ENABLED )
					//if (!validDate)
				{
					QString msg;
					msg += "line: " + line + "\n";
					msg += "path: " + path + "\n";
					msg += "fullpath: " + fullpath + "\n";
					msg += "sizestring: " + sizestring + "\n";
					msg += "sizeindex: " + QString().setNum ( sizeindex ) + "\n";
					msg += "size: " + QString().setNum ( size ) + "\n";
					msg += "month: " + QString().setNum ( month ) + "\n";
					msg += "hour: " + QString().setNum ( hour ) + "\n";
					msg += "minute: " + QString().setNum ( minute ) + "\n";
					msg += "second: " + QString().setNum ( second ) + "\n";
					msg += "year: " + QString().setNum ( year ) + "\n";
					msg += "day: " + QString().setNum ( day ) + "\n";
					msg += "datetimestring: " + datetimestring + "\n";
					msg += "new_medianame: " + new_medianame + "\n";
					msg += "directory path: " + dirpath + "\n";
					msg += "file name: " + filename + "\n";
					//QMessageBox::warning(0, "line", msg);
				}
			}
			else if ( csvtype == "kat-dece" ) {
					/*
						* format:
						* "Number";"Location";"CD name";"CD category";"Name";"Extension";"Length";"Date";"Path";"Comment"
						*
						* sample line:
						* "";"";"newiso";"! Alle Medien  ";"empire-efi-logo-alt256";".png";" 11,669";"2010.01.07  08:05";"Extra\Themes\Default\";""
						*/
					if ( linecount == 0 ) {
						linecount++;
						continue;
					}
					// FIXME
					separator = ";";
					QStringList csvList = line.split ( separator );
					if ( csvList.count() != 10 ) {
						if ( *DEBUG_INFO_ENABLED )
							cerr << "importGtktalogCsv invalid line (not 9 fields): " << qPrintable ( line ) << endl;
						continue;
					}
					filename = QString ( csvList.at ( 4 ) ).replace ( "\"", "" ) + QString ( csvList.at ( 5 ) ).replace ( "\"", "" );
					QString sizestring = QString ( csvList.at ( 6 ) ).replace ( "\"", "" ).replace ( ",", "." ).replace ( ".", "" ).replace ( "#", "" ).trimmed();
					// size is double and its kib!
					size = uint ( QString ( csvList.at ( 6 ) ).replace ( "\"", "" ).replace ( ",", "." ).replace ( ".", "" ).replace ( "#", "" ).trimmed().toDouble() * 1024 );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv size: " << size << endl;
					datetimestring = QString ( csvList.at ( 7 ) ).replace ( "\"", "" );
					//if (*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					QString datesep = ".";
					// date is in format: year.month.day hour:minute
					if ( datetimestring.contains ( '.' ) )
						datesep = ".";
					int yearindex = datetimestring.indexOf ( datesep, 0 );
					int monthindex = datetimestring.indexOf ( datesep, yearindex + 1 );
					int dayindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( " ", dayindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
					int year = ( datetimestring.mid ( 0, yearindex ) ).toInt();
					QString monthstring = datetimestring.mid ( yearindex + 1, monthindex - yearindex - 1 );
					int month = ( datetimestring.mid ( yearindex + 1, monthindex - yearindex - 1 ) ).toInt();
					QString daystring = datetimestring.mid ( monthindex + 1, hourindex - dayindex + 1 );
					int day = ( datetimestring.mid ( monthindex + 1, hourindex - dayindex + 1 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );
					QString hourstring = datetimestring.mid ( hourindex + 1, minuteindex - hourindex - 1 );
					int hour = ( datetimestring.mid ( hourindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( dayindex + 1, minuteindex - 1 ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );

					new_medianame = QString ( csvList.at ( 2 ) ).replace ( "\"", "" );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

					dirpath = QString ( csvList.at ( 8 ) ).replace ( "\"", "" );
					dirpath = dirpath.mid ( 0, dirpath.length() - 1 );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//	dirpath = "/"+dirpath;
					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

					fullpath = path;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					comment = QString ( csvList.at ( 9 ) ).replace ( "\"", "" );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv comment: " << qPrintable(comment) << endl;

					category = QString ( csvList.at ( 3 ) ).replace ( "\"", "" );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv category: " << qPrintable(category) << endl;
					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						msg += "monthstring: " + monthstring + "\n";
						msg += "hourstring: " + hourstring + "\n";
						msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "disclib" ) {
					/*
						* format:
						* name*"volume"*filepath*path*size*Volume serial*date*type*"category"*"comments"*"location"
						*
						* sample line:
						* acc_dis.chm*"DISK_00001_HelpCD"*DISK_00001_HelpCD\acc_dis.chm*DISK_00001_HelpCD\*20 832*6C721915*23.08.2001 12:00:00*chm*""*""*""
						*/
					// FIXME
					separator = "*";
					QStringList csvList = line.split ( separator );
					//if (csvList.count() < 10) {
					//	if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable(line) << endl;
					//	continue;
					//}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line: " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					filename = QString ( csvList.at ( 0 ) );
					QString sizestring = QString ( csvList.at ( 3 ) ) ;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
					if ( sizestring.contains ( "/" ) ) {
						// DISK_00003_Downloads\hhcode_and_exe\TestCHM\Agent\images/5 720 => 5 720
						sizestring = QString ( sizestring.split ( "/" ).at ( 1 ) ).replace ( " ", "" );
						//if(*DEBUG_INFO_ENABLED)
						//	cerr << "importGtktalogCsv sizestring raw2: " <<qPrintable( sizestring) << endl;
					}
					//QString(csvList.at(3)).stripWhiteSpace();
					size = QString ( sizestring ).toInt();

					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv size: " << size << endl;
					datetimestring = QString ( csvList.at ( 5 ) );
					//if (*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					// date is normally day/month/year hour:minute:second
					QString datesep = "/";

					// date in other format: day.month.year hour:minute:second
					if ( datetimestring.contains ( '.' ) )
						datesep = ".";
					int dayindex = datetimestring.indexOf ( datesep );
					int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
					int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );

					int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
					QString day_ = ( datetimestring.mid ( 0, dayindex ) );
					int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
					int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );

					int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );
					new_medianame = QString ( csvList.at ( 1 ) ).replace ( "\"", "" );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

					dirpath = QString ( csvList.at ( 3 ) ).replace ( "\"", "" );
					//	dirpath = dirpath.mid(0, dirpath.length()-1 );
					dirpath = dirpath.replace ( new_medianame, "" );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
				//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//	dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

					fullpath = path;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					category = QString ( csvList.at ( 7 ) ).replace ( "\"", "" );
					comment = QString ( csvList.at ( 8 ) ).replace ( "\"", "" );

					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						//msg += "monthstring: " + monthstring + "\n";
						//msg += "hourstring: " + hourstring + "\n";
						//msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";

						cerr << "msg: " << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "visualcd" ) {
					/*
						* format:
						* name;path;size;date;comment;date picture;dimensions;duration;title;artist;album
						*
						* sample line:
						* QtScriptTools4.dll;E:\Qt\4.7.1\bin\;744.448;06.11.2010 08:37:52;;;;;;;
						*/
					if ( linecount < 2 ) {
						linecount++;
						continue;
					}
					separator = ";";
					QStringList csvList = line.split ( separator );
					//if (csvList.count() < 10) {
					//	if(*DEBUG_INFO_ENABLED)
					//		cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable(line) << endl;
					//	continue;
					//}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line: " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					filename = QString ( csvList.at ( 0 ) );
					QString sizestring = QString ( csvList.at ( 2 ) ).replace ( ".", "" );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
					//QString(csvList.at(3)).stripWhiteSpace();
					size = QString ( sizestring ).toInt();
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv size: " << size << endl;
					datetimestring = QString ( csvList.at ( 3 ) );
					//if (*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					// date in format: day.month.year hour:minute:second
					QString datesep = ".";
					int dayindex = datetimestring.indexOf ( datesep );
					int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
					int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
					int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
					QString day_ = ( datetimestring.mid ( 0, dayindex ) );
					int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
					int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );
					int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );
					int dotindex = import_filename.lastIndexOf ( '.' );
					new_medianame = import_filename.mid ( 0, dotindex );

					if ( new_medianame.contains ( '\\' ) )
						new_medianame = new_medianame.replace ( '\\', "/" );
					if ( new_medianame.contains ( '/' ) && new_medianame.split ( '/' ).size() > 1 ) {
						int last_index = new_medianame.lastIndexOf ( '/' );
						int first_index = new_medianame.indexOf ( '/', last_index );
						new_medianame = new_medianame.mid ( first_index + 1, last_index );
					}
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

					dirpath = QString ( csvList.at ( 1 ) ).replace ( "\"", "" );
					dirpath = dirpath.mid ( 0, dirpath.length() - 1 );
					dirpath = dirpath.replace ( new_medianame, "" );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//        dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

					fullpath = path;
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;
					category = "";
					comment = QString ( csvList.at ( 4 ) ).replace ( "\"", "" );
					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						//msg += "monthstring: " + monthstring + "\n";
						//msg += "hourstring: " + hourstring + "\n";
						//msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";
						cerr << "msg: " << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "vvv" ) {
					/*
						* format:
						* "media","path","filename",size,"extension",date,"description"
						*
						* sample line:
						* "winxp","/I386/LANG","PINTLPAD.CH_",65841,"CH_",23.08.2001 14:00:00,""
						*/
					if ( linecount < 1 ) {
						linecount++;
						continue;
					}
					separator = ",";
					QStringList csvList = line.split ( separator );
					//if (csvList.count() < 10) {
					//	if(*DEBUG_INFO_ENABLED)
					//		cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable(line) << endl;
					//	continue;
					//}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line: " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					filename = QString ( csvList.at ( 2 ) ).replace ( "\"", "" );
					QString sizestring = QString ( csvList.at ( 3 ) ) ;
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
					//QString(csvList.at(3)).stripWhiteSpace();
					size = QString ( sizestring ).toInt();

					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv size: " << size << endl;

					datetimestring = QString ( csvList.at ( 5 ) );
					//if (*DEBUG_INFO_ENABLED)
					//cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					// date in format: day.month.year hour:minute:second
					QString datesep = ".";
					int dayindex = datetimestring.indexOf ( datesep );
					int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
					int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
					int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
					QString day_ = ( datetimestring.mid ( 0, dayindex ) );
					int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
					int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );
					int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );

					new_medianame = QString ( csvList.at ( 0 ) ).replace ( "\"", "" );
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

					dirpath = QString ( csvList.at ( 1 ) ).replace ( "\"", "" );
					//dirpath = dirpath.mid(0, dirpath.length()-1 );
					dirpath = dirpath.replace ( new_medianame, "" );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//        dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

					fullpath = path;

					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					category = "";
					comment = QString ( csvList.at ( 6 ) ).replace ( "\"", "" );

					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//        msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						//        msg += "monthstring: " + monthstring + "\n";
						//        msg += "hourstring: " + hourstring + "\n";
						//        msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";
						cerr << "msg: " << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "afo" ) {
					/*
						* format:
						* "name","size","modified","location","category","description","type"
						*
						* sample line:
						* "intro.gif","308","16.09.2004 13:13:26","10_Jahre\c-compiler\l_cc_p_9.0.021\eclipse-platform-3.0.1-linux-gtk.zip\eclipse\plugins\org.eclipse.platform_3.0.1","","","GIF-Bild"
					"introContent.xml","11479","16.09.2004
						*/
					if ( linecount < 1 ) {
						linecount++;
						continue;
					}
					separator = ",";
					QStringList csvList = line.split ( separator );
					if ( csvList.count() < 7 ) {
						if ( *DEBUG_INFO_ENABLED )
							cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable ( line ) << endl;
						continue;
					}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line: " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					QString type = QString ( csvList.at ( 6 ) ).replace ( "\"", "" );
					if ( type == "Dateiordner" || type == "Folder" || type == "CD Disk" )
						continue;

					filename = QString ( csvList.at ( 0 ) ).replace ( "\"", "" );
					QString sizestring = QString ( csvList.at ( 1 ) ).replace ( "\"", "" ) ;
					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
					//QString(csvList.at(3)).stripWhiteSpace();
					size = QString ( sizestring ).toInt();

					//if(*DEBUG_INFO_ENABLED)
					//     cerr << "importGtktalogCsv size: " << size << endl;
					datetimestring = QString ( csvList.at ( 2 ) ).replace ( "\"", "" );
					//if (*DEBUG_INFO_ENABLED)
					//cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					// date in format: day.month.year hour:minute:second
					QString datesep = ".";
					int dayindex = datetimestring.indexOf ( datesep );
					int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
					int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
					int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
					QString day_ = ( datetimestring.mid ( 0, dayindex ) );
					int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
					int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );
					int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );
					QString location = QString ( csvList.at ( 3 ) ).replace ( "\"", "" ).replace ( "\\", "/" );
					if ( *DEBUG_INFO_ENABLED )
						cerr << "importGtktalogCsv location: " << qPrintable ( location ) << endl;

					if ( location.contains ( '/' ) ) {
						new_medianame = location.split ( '/' ) [0];
						dirpath = location.replace ( new_medianame + "/", "" );
					}
					else {
						new_medianame = location;
						dirpath = "";
					}
					if ( *DEBUG_INFO_ENABLED )
						cerr << "importGtktalogCsv new_medianame: " << qPrintable ( new_medianame ) << endl;

					//dirpath = dirpath.mid(0, dirpath.length()-1 );
					dirpath = dirpath.replace ( new_medianame, "" );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//	dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					if ( *DEBUG_INFO_ENABLED )
						cerr << "importGtktalogCsv dirpath: " << qPrintable ( dirpath ) << endl;

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

					fullpath = path;

					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					category = QString ( csvList.at ( 4 ) ).replace ( "\"", "" );
					comment = QString ( csvList.at ( 5 ) ).replace ( "\"", "" );

					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						//msg += "monthstring: " + monthstring + "\n";
						//msg += "hourstring: " + hourstring + "\n";
						//msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";
						cerr << "msg: " << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "filearchivist" ) {
					/*
						* format:
						* media|filename|path|date|open with
						*
						* sample line:
						* kottan_1|next4.bmp|e:\divx player\vlc-0.7.2\skins\default\|21.05.2004 14:26|i_view32.exe
						*/
					if ( linecount < 1 ) {
						linecount++;
						continue;
					}
					separator = "|";
					QStringList csvList = line.split ( separator );
					//if (csvList.count() < 10) {
					//	if(*DEBUG_INFO_ENABLED)
					//		cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable(line) << endl;
					//	continue;
					//}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line: " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					filename = QString ( csvList.at ( 1 ) );
					//QString sizestring = QString(csvList.at(3)) ;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
					//QString(csvList.at(3)).stripWhiteSpace();
					size = 0;

					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv size: " << size << endl;
					datetimestring = QString ( csvList.at ( 3 ) );
					//if (*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

					// date in format: day.month.year hour:minute:second
					QString datesep = ".";
					int dayindex = datetimestring.indexOf ( datesep );
					int monthindex = datetimestring.indexOf ( datesep, dayindex + 1 );
					int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
					int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
					int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
					int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
					QString day_ = ( datetimestring.mid ( 0, dayindex ) );
					int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
					int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
					//bool validDate = date.setYMD( year, month, day );
					int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
					int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
					int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
					QDate date ( year, month, day );
					QTime time ( hour, minute, second );
					datetime = QDateTime ( date, time );
					new_medianame = QString ( csvList.at ( 0 ) );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;
					dirpath = QString ( csvList.at ( 2 ) );
					dirpath = dirpath.mid ( 0, dirpath.length() - 1 );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//	dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;
					fullpath = path;

					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					category = "";
					comment = QString ( csvList.at ( 4 ) );

					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						//msg += "sizestring: " + sizestring + "\n";
						msg += "month: " + QString().setNum ( month ) + "\n";
						msg += "hour: " + QString().setNum ( hour ) + "\n";
						msg += "minute: " + QString().setNum ( minute ) + "\n";
						msg += "year: " + QString().setNum ( year ) + "\n";
						msg += "day: " + QString().setNum ( day ) + "\n";
						msg += "datetimestring: " + datetimestring + "\n";
						//msg += "monthstring: " + monthstring + "\n";
						//msg += "hourstring: " + hourstring + "\n";
						//msg += "daystring: " + daystring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";
						cerr << "msg: " << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "advanceddiskcatalog" ) {
					/*
						* format:
						* "Disk", "Name", "Path", "Full path", "Size", "Date", "Time", "Category", "Level", "Comments", "Files inside", "Type"
						* or
						* "Disk", "Name", "Path", "Full path", "Size", "Date", "Time", "Category", "Level", "Comments", "Files inside"
						*
						* sample lines:
						* "biexpp1210", "GDIPLUS.MAN", "\I386\ASMS\1000\MSFT\WINDOWS\GDIPLUS", "D:\I386\ASMS\1000\MSFT\WINDOWS\GDIPLUS\GDIPLUS.MAN", "391 bytes", "14.04.2008", "12:00:00", "None", 7, "", 0, "File"
						"Vol 03", "chicagos_30.zip", "\C64\Games\c", "E:\C64\Games\c\chicagos_30.zip", 30 113, "12/23/1999", "7:14:48 AM", "None", 4, "", 0, "File"
						*/
					if ( linecount < 1 ) {
						linecount++;
						continue;
					}

					// skip description line
					if ( line == "\"Disk\", \"Name\", \"Path\", \"Full path\", \"Size\", \"Date\", \"Time\", \"Category\", \"Level\", \"Comments\", \"Files inside\"" || line == "\"Disk\", \"Name\", \"Path\", \"Full path\", \"Size\", \"Date\", \"Time\", \"Category\", \"Level\", \"Comments\", \"Files inside\", \"Type\"" ) {
						continue;
					}

					// manually split line from csv file (because there may be commas in the file name)
					QStringList csvList;
					int linelen = line.length();
					bool in_string = false;
					QString buffer = "";
					QChar ch;
					QChar comma = ',';
					QChar quote = '"';
					for ( int i = 0; i < linelen; i++ ) {
						ch = line.at ( i );
						if ( ch == comma || i == linelen - 1 ) {
							if ( in_string ) {
								buffer += ch;
							}
							else {
								csvList.append ( buffer.trimmed() );
								//if(*DEBUG_INFO_ENABLED) {
								//	cout << "appending field" << endl;
								//}
								buffer = "";
							}
						}
						else
							if ( ch == quote ) {
								if ( in_string ) {
								}
								in_string = !in_string;
							}
							else {
								buffer.append ( ch );
								//if(*DEBUG_INFO_ENABLED) {
								//	cout << "appending field content to buffer" << endl;
								//}
							}
					}
					if ( *DEBUG_INFO_ENABLED ) {
						cerr << "line (fields " << csvList.size() << "): " << qPrintable ( line ) << endl;
						for ( int i = 0; i < csvList.size(); ++i )
							cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
					}

					if ( csvList.count() != 11  && csvList.count() != 12 ) {
						if ( *DEBUG_INFO_ENABLED )
							cerr << "importGtktalogCsv invalid line (only " << csvList.count() << ") at least 11 or 12 fields): " << qPrintable ( line ) << endl;
						continue;
					}
					//if (csvList.size() == 11 )
					if ( QString ( csvList.at ( 5 ) ) == "Folder" && QString ( csvList.at ( 6 ) ) == "Folder" )
						continue;
					//if (csvList.size() > 11 )
					//	if (QString(csvList.at(11)) == "Folder")
					//		continue;
					filename = QString ( csvList.at ( 1 ) );

					// format of size string depends on ADC's Options/Preferences/Genral/Show sizes in KB
					QString sizestring = QString ( csvList.at ( 4 ) );
					if ( sizestring.right ( 1 ).at ( 0 ).isDigit() ) { //1 234 567
						size = sizestring.replace ( " ", "" ).toDouble();
                    }
					else {  // 123.456 bytes/KB/MB/GB
						QStringList sizestringlist = sizestring.split ( " " );
						QString unit = sizestringlist.last();
						sizestringlist.removeLast();
						size = sizestringlist.join ( "" ).toDouble();
						if ( unit == "KB" )
							size = size * 1024.0;
						if ( unit == "MB" )
							size = size * 1024.0 * 1024.0;
						if ( unit == "GB" )
							size = size * 1024.0 * 1024.0 * 1024.0;
						if ( unit == "TB" )
							size = size * 1024.0 * 1024.0 * 1024.0 * 1024.0;
					}

					// FIXME - date and time depend on selected locale - this section may need fixing
					QString datestring = QString ( csvList.at ( 5 ) );
					QDate date = QDate::fromString ( datestring, "d.M.yyyy" );
					if ( not date.isValid() )
						date = QDate::fromString ( datestring, "M/d/yyyy" );
					QString timestring = QString ( csvList.at ( 6 ) );
					QTime time = QTime::fromString ( timestring, "h:mm:ss AP" );
					if ( not time.isValid() )
						time = QTime::fromString ( timestring, "h:mm:ss" );
					datetime = QDateTime ( date, time );

					new_medianame = QString ( csvList.at ( 0 ) );
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

					dirpath = QString ( csvList.at ( 2 ) );
					//dirpath = dirpath.mid(0, dirpath.length()-1 );
					if ( pathsep == "\\" )
						dirpath = dirpath.replace ( "\\", "/" );
					//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
					//	dirpath = "/"+dirpath;
					if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
						dirpath = dirpath.mid ( 1, dirpath.length() - 1 );

					path = dirpath + "/" + filename;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;
					fullpath = path;
					//if(*DEBUG_INFO_ENABLED)
					//	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

					if ( csvList.size() > 6 )
						category = QString ( csvList.at ( 7 ) );
					else
						category = "";
					if ( csvList.size() > 9 )
						comment = QString ( csvList.at ( 9 ) );
					else
						comment = QString ( "" );

					if ( *DEBUG_INFO_ENABLED )
						//if (!validDate)
					{
						QString msg;
						//msg += "line: " + line + "\n";
						msg += "path: " + path + "\n";
						msg += "fullpath: " + fullpath + "\n";
						msg += "size: " + QString().setNum ( size ) + "\n";
						msg += "sizestring: " + sizestring + "\n";
						msg += "datestring: " + datestring + "\n";
						msg += "timestring: " + timestring + "\n";
						msg += "new_medianame: " + new_medianame + "\n";
						msg += "directory path: " + dirpath + "\n";
						msg += "file name: " + filename + "\n";
						msg += "comment: " + comment + "\n";
						msg += "category: " + category + "\n";
						cerr << "msg: \n" << qPrintable ( msg ) << endl;
						//QMessageBox::warning(0, "line", msg);
					}
				}
			else if ( csvtype == "whereisit" ) {
				/*
					* format:
					* File, Folder or Disk;Ext;Item Size;Item Date;Location;Media Type;Folder;Category;Description;Disk #;Item Time;CRC;Tag;Disk Location;
					*
					* sample line:
					* REBOOTST;EXE;118784;2008-04-14;Biexpp1210;CD-ROM;\DOTNETFX;;Visual Studio RebootSt (7.00.9581);2;12:00:00;0;;;
				*/
				if ( linecount < 1 || line.startsWith ( "File, Folder or Disk;Ext;Item Size;Item Da" ) ) {
					linecount++;
					continue;
				}
				QStringList csvList = line.split ( separator );
				if ( csvList.count() != 15 ) {
					if ( *DEBUG_INFO_ENABLED )
						cerr << "importGtktalogCsv invalid line (only " << csvList.count() << ") at least 15 fields): " << qPrintable ( line ) << endl;
					continue;
				}
				if ( *DEBUG_INFO_ENABLED ) {
					cerr << "line: " << qPrintable ( line ) << endl;
					for ( int i = 0; i < csvList.size(); ++i )
						cout << "csvList[" << i << "]: " << csvList.at ( i ).toLocal8Bit().constData() << endl;
				}

				new_medianame = QString ( csvList.at ( 4 ) );
				//if(*DEBUG_INFO_ENABLED)
				//	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

				filename = QString ( csvList.at ( 0 ) );
				if ( !QString ( csvList.at ( 1 ) ).isEmpty() && !QString ( csvList.at ( 0 ) ).endsWith ( "." + csvList.at ( 1 ) ) )
					filename += "." + QString ( csvList.at ( 1 ) );
				QString sizestring = QString ( csvList.at ( 2 ) );
				//if(*DEBUG_INFO_ENABLED)
				//	cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
				//QString(csvList.at(3)).stripWhiteSpace();
				size = QString ( sizestring ).toInt();

				//if(*DEBUG_INFO_ENABLED)
				//	cerr << "importGtktalogCsv size: " << size << endl;

				QString datestring = QString ( csvList.at ( 3 ) );
				QString timestring = QString ( csvList.at ( 10 ) );
				//if (*DEBUG_INFO_ENABLED)
				//	cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;
				datetimestring = datestring + " " + timestring;
				// date in format: year-month-dayhour:minute:second
				QString datesep = "-";
				int yearindex = datetimestring.indexOf ( datesep );
				int monthindex = datetimestring.indexOf ( datesep, yearindex + 1 );
				int dayindex = datetimestring.indexOf ( " ", monthindex + 1 );
				int hourindex = datetimestring.indexOf ( ":", dayindex + 1 );
				int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
				int year = ( datetimestring.mid ( 0, yearindex ) ).toInt();
				QString year_ = ( datetimestring.mid ( 0, yearindex ) );
				int month = ( datetimestring.mid ( yearindex + 1, monthindex - yearindex - 1 ) ).toInt();
				int day = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();
				//bool validDate = date.setYMD( year, month, day );
				int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
				int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
				int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
				QDate date ( year, month, day );
				QTime time ( hour, minute, second );
				datetime = QDateTime ( date, time );
				//	int fileindex = fullpath.lastIndexOf ( '/' );
				//if(*DEBUG_INFO_ENABLED)
				//      cerr << "importGtktalogCsv fileindex: " << fileindex << endl;
				dirpath = QString ( csvList.at ( 6 ) );
				//dirpath = dirpath.mid(0, dirpath.length()-1 );
				if ( pathsep == "\\" )
					dirpath = dirpath.replace ( "\\", "/" );
				//if (!dirpath.isEmpty() && dirpath.at(0) != '/')
				//	dirpath = "/"+dirpath;
				if ( !dirpath.isEmpty() && dirpath.at ( 0 ) == '/' )
					dirpath = dirpath.mid ( 1, dirpath.length() - 1 );
				path = dirpath + "/" + filename;
				//if(*DEBUG_INFO_ENABLED)
				//	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;
				fullpath = path;
				//if(*DEBUG_INFO_ENABLED)
				//     cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

				category = QString ( csvList.at ( 7 ) );
				comment = QString ( csvList.at ( 8 ) );
				QString tag = QString ( csvList.at ( 12 ) );
				if ( tag.size() > 0 )
					comment += tr ( "tag: " ) + tag;

				if ( *DEBUG_INFO_ENABLED )
					//if (!validDate)
				{
					QString msg;
					//msg += "line: " + line + "\n";
					msg += "path: " + path + "\n";
					msg += "fullpath: " + fullpath + "\n";
					msg += "size: " + QString().setNum ( size ) + "\n";
					msg += "sizestring: " + sizestring + "\n";
					msg += "month: " + QString().setNum ( month ) + "\n";
					msg += "hour: " + QString().setNum ( hour ) + "\n";
					msg += "minute: " + QString().setNum ( minute ) + "\n";
					msg += "year: " + QString().setNum ( year ) + "\n";
					msg += "day: " + QString().setNum ( day ) + "\n";
					msg += "datetimestring: " + datetimestring + "\n";
					//msg += "monthstring: " + monthstring + "\n";
					//msg += "hourstring: " + hourstring + "\n";
					//msg += "daystring: " + daystring + "\n";
					msg += "new_medianame: " + new_medianame + "\n";
					msg += "directory path: " + dirpath + "\n";
					msg += "file name: " + filename + "\n";
					msg += "comment: " + comment + "\n";
					msg += "category: " + category + "\n";
					msg += "tag: " + tag + "\n";
					cerr << "msg: " << qPrintable ( msg ) << endl;
					//QMessageBox::warning(0, "line", msg);
				}
			}
			else if ( csvtype == "generic_plaintext_regex" ) {
				/*
				* format (generic):
				*
				* du --all --human-readable --time
				* 4,0K    2012-11-04 11:00        /etc/sysconfig/lm_sensors
				* <size>[<unit>]<tab><yyyy-mm-dd><space><hh:ss><tab><path>
				*
				* dir /S /B
				* <path>
				*
				*/
				if ( linecount < 1 ) {
					linecount++;
					continue;
				}

				if ( *DEBUG_INFO_ENABLED ) {
					cerr << "line: " << qPrintable ( line ) << endl;
				}

				if (!regex_pattern.exactMatch(line)) {
					linecount++;
					if ( *DEBUG_INFO_ENABLED )
						cerr << "importGtktalogCsv:: invalid line entry found" << endl;
					continue;
				}



				int day =  0;
				int month =  0;
				int year =  0;
				int hour =  0;
				int minute = 0;
				int second = 0;
				QString sizestring;
				pathsep = QString(QDir::separator ());
				filename = "";
				dirpath = "";

				for(int i=0;i< pattern_list.size(); i++) {
					QRegExp test_pattern(pattern_list.at(i).pattern);
					int pattern_index = test_pattern.indexIn(line);
					int pattern_length = test_pattern.matchedLength();

					if (pattern_length > 0) {
						if ( *DEBUG_INFO_ENABLED )
							cerr << "pattern matched: " << qPrintable(pattern_list.at(i).pattern_name) << " (" << qPrintable(pattern_list.at(i).pattern) << " at " << pattern_index << ", length: " << pattern_length << endl;

						if (pattern_list.at(i).pattern_name == "%dirseparator%") {
						}

						else if (pattern_list.at(i).pattern_name == "%tab%") {
						}

						else if (pattern_list.at(i).pattern_name == "%space%") {
						}

						else if (pattern_list.at(i).pattern_name == "%filename%") {
							filename = line.mid(pattern_index, pattern_length);
							if ( *DEBUG_INFO_ENABLED )
								cerr << " => filename: " << qPrintable(line.mid(pattern_index, pattern_length)) << endl;
						}

						else if (pattern_list.at(i).pattern_name == "%fullpath%") {
							fullpath = line.mid(pattern_index, pattern_length);
							QStringList path_split = fullpath.split(pathsep);
							filename = path_split.last();
							path_split.removeLast();
							dirpath = path_split.join(pathsep);
							if ( *DEBUG_INFO_ENABLED ) {
								cerr << " => fullpath: " << qPrintable(line.mid(pattern_index, pattern_length)) << endl;
								cerr << " => filename: " << qPrintable(filename) << endl;
								cerr << " => dirpath: " << qPrintable(dirpath) << endl;
							}
						}
						else if (pattern_list.at(i).pattern_name == "%size_human%") {
							sizestring = line.mid(pattern_index, pattern_length);
							QRegExp unit_test_pattern("[bBkKmMgGtT]");
							int unit_pattern_index = unit_test_pattern.indexIn(sizestring.right(1));
							int unit_pattern_length = unit_test_pattern.matchedLength();
							if (unit_pattern_length > 0) {

								size = line.mid(pattern_index, pattern_length-1).replace(",", ".").toFloat();
								if ( *DEBUG_INFO_ENABLED )
									cerr << "size with unit sizestring size: " << qPrintable(line.mid(pattern_index, pattern_length-1)) << endl;
								QString unit_string = sizestring.right(1);

								if ( *DEBUG_INFO_ENABLED )
									cerr << "size with unit sizestring unit: " << qPrintable(unit_string) << endl;

								if (unit_string.toLower() == "b")
									sizefactor = 1;
								else if (unit_string.toLower() == "k")
									sizefactor = SIZE_ONE_KBYTE;
								else if (unit_string.toLower() == "M")
									sizefactor = SIZE_ONE_MBYTE;
								else if (unit_string.toLower() == "G")
									sizefactor = SIZE_ONE_GBYTE;
								else if (unit_string.toLower() == "T")
									sizefactor = SIZE_ONE_TBYTE;

								size = size * sizefactor;
							}
							else {

								size = line.mid(pattern_index, pattern_length).toInt();
								//cerr << "sizestring size: " << qPrintable(line.mid(pattern_index, pattern_length)) << endl;
							}
						}

						else if (pattern_list.at(i).pattern_name == "%sizeb%") {
							sizestring = line.mid(pattern_index, pattern_length);
							size = line.mid(pattern_index, pattern_length).toInt();
						}

						else if (pattern_list.at(i).pattern_name == "%sizekb%") {
							sizefactor = 1024;
							sizestring = line.mid(pattern_index, pattern_length);
							size = line.mid(pattern_index, pattern_length).replace(",", ".").toFloat();
							size = size * sizefactor;
						}

						else if (pattern_list.at(i).pattern_name == "%dateyear2%") {
							// 12-05-13
							QString datestring = line.mid(pattern_index, pattern_length);
							QStringList datestring_split = datestring.split("-");
							if (datestring_split.size() == 3) {
								year = ( datestring_split.at(0) ).toInt();
								month = ( datestring_split.at(1) ).toInt();
								day = ( datestring_split.at(2) ).toInt();
							}
						}

						else if (pattern_list.at(i).pattern_name == "%dateyear4%") {
							// 2012-05-13
							QString datestring = line.mid(pattern_index, pattern_length);
							QStringList datestring_split = datestring.split("-");
							if (datestring_split.size() == 3) {
								year = ( datestring_split.at(0) ).toInt();
								month = ( datestring_split.at(1) ).toInt();
								day = ( datestring_split.at(2) ).toInt();
							}
						}

						else if (pattern_list.at(i).pattern_name == "%time_full%") {
							// 23:27:13
							QString timestring = line.mid(pattern_index, pattern_length);
							QStringList timestring_split = timestring.split(":");
							if (timestring_split.size() == 3) {
								hour = ( timestring_split.at(0) ).toInt();
								minute = ( timestring_split.at(1) ).toInt();
								second = ( timestring_split.at(2) ).toInt();
							}
						}

						else if (pattern_list.at(i).pattern_name == "%time_hm%") {
							// 23:27
							QString timestring = line.mid(pattern_index, pattern_length);
							QStringList timestring_split = timestring.split(":");
							if (timestring_split.size() == 2) {
								hour = ( timestring_split.at(0) ).toInt();
								minute = ( timestring_split.at(1) ).toInt();
								second = 0;
							}
						}

					// strip already processed
						line = line.right(line.size()-pattern_length);
						//cerr << "line mod " << qPrintable( line ) << endl;
					}
				}



				//if(*DEBUG_INFO_ENABLED)
				//     cerr << "importGtktalogCsv size: " << size << endl;


				QDate date ( year, month, day );
				QTime time ( hour, minute, second );
				datetime = QDateTime ( date, time );

				if(extra_media_name.isEmpty())
					extra_media_name = tr("importedMedia_")+QDateTime::currentDateTimeUtc().toString().replace(":", "").replace(" ", "_");
				new_medianame = QString ( extra_media_name );

				//if(*DEBUG_INFO_ENABLED)
				//     cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;


				if ( pathsep == "\\" )
					dirpath = dirpath.replace ( "\\", "/" );


				//if(*DEBUG_INFO_ENABLED)
				//     cerr << "importGtktalogCsv path: " << qPrintable(fullpath) << endl;

				//if(*DEBUG_INFO_ENABLED)
				//     cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

				category = "";
				comment = "";

				if ( *DEBUG_INFO_ENABLED )
					//if (!validDate)
				{
					QString msg;
					//        msg += "line: " + line + "\n";
					msg += "path: " + path + "\n";
					msg += "fullpath: " + fullpath + "\n";
					msg += "size: " + QString().setNum ( size ) + "\n";
					msg += "sizestring: " + sizestring + "\n";
					msg += "month: " + QString().setNum ( month ) + "\n";
					msg += "hour: " + QString().setNum ( hour ) + "\n";
					msg += "minute: " + QString().setNum ( minute ) + "\n";
					msg += "year: " + QString().setNum ( year ) + "\n";
					msg += "day: " + QString().setNum ( day ) + "\n";
					msg += "datetimestring: " + datetimestring + "\n";
					//        msg += "monthstring: " + monthstring + "\n";
					//        msg += "hourstring: " + hourstring + "\n";
					//        msg += "daystring: " + daystring + "\n";
					msg += "new_medianame: " + new_medianame + "\n";
					msg += "directory path: " + dirpath + "\n";
					msg += "file name: " + filename + "\n";
					msg += "comment: " + comment + "\n";
					msg += "category: " + category + "\n";
					cerr << "msg: " << qPrintable ( msg ) << endl;
					//QMessageBox::warning(0, "line", msg);
				}
			}
			else {
				QMessageBox::warning (0, tr("unknown import type"), tr("unknown import type: %1").arg(csvtype));
				return;
			}

			if ( medianame == "" )
				medianame = new_medianame;

			if ( medianame != new_medianame ) {
				//        QMessageBox::warning (0, "info", medianame);
				addNewMedia ( medianame, datetime, comment, category, medialines );
				medialines->clear();
				medianame = new_medianame;
				addNewItem ( new_medianame, dirpath, filename, size, datetime, comment, category );
			}
			else {
				//        QMessageBox::warning (0, "info", "new item");
				addNewItem ( new_medianame, dirpath, filename, size, datetime, comment, category );
			}
		}     // valid line
		linecount++;
		progress->setValue ( linecount );
		if ( parent->mainw->app->hasPendingEvents() )
			parent->mainw->app->processEvents();
	}
	delete progress;
	f.close();
	if ( !medialines->isEmpty() ) {
		comment = tr("Imported from %1 (CSV import).").arg(import_filename);
		addNewMedia ( medianame, datetime, comment, category, medialines );
		medialines->clear();
	}

	delete medialines;
	( db->getRootNode() ) ->touchDB();
	guislave->panelsON();
	QApplication::restoreOverrideCursor();
	QString msg;
	msg += tr ( "Import was successful." );
	msg += "\n";
	if ( mediacount < 2 )
		msg += tr ( "1 media" ) + ",\n";
	else
		msg += QString().setNum ( mediacount ) + " " + QString ( tr ( "media" ) ) + ",\n";

	if ( dircount < 2 )
		msg += QString ( tr ( "1 directory,\n" ) );
	else
		msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

	if ( filecount < 2 )
		msg += tr ( "1 file" ) + ",\n";
	else
		msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

	msg += tr ( "are imported." );

	QMessageBox::information ( 0, tr ( "Import successful" ), msg );
}

importGtktalogCsv::~importGtktalogCsv() {}

int importGtktalogCsv::addNewItem ( QString medianame, QString path,
                                    QString filename, double size, QDateTime datetime, QString comment, QString category ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	lineObject l ( medianame, path, filename, size, datetime, comment, "", category );
	if ( *DEBUG_INFO_ENABLED )
		cerr << "importGtktalogCsv::addNewItem: medianame: " <<
		     qPrintable ( medianame ) << ", path: " << qPrintable ( path ) << ", filename: " <<
		     qPrintable ( filename ) << ", size: " << size << ", date: " <<
		     qPrintable ( datetime.toString() )  << ", comment: " << qPrintable ( comment ) << ", category: " << qPrintable ( category ) << endl;
	medialines->append ( l );

	// QMessageBox::critical( 0, "item", "new item!");
	return 0;

}

int importGtktalogCsv::addNewMedia ( QString new_medianame, QDateTime media_modification, QString media_comment, QString media_category, QList < lineObject > *medialines ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	//if(*DEBUG_INFO_ENABLED)
	//  cerr << "importGtktalogCsv::addNewMedia media: " <<  qPrintable(new_medianame) << endl;

	if ( guislave->mainw->db == NULL )
		guislave->newEvent();
	if ( guislave->mainw->db == NULL )
		return 0;

	/* create new media */
	Node *env, *curr;
	curr = db->getMediaNode ( new_medianame );
	if ( curr == NULL )
		curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, media_comment, media_modification, media_category );

	QString msg;
	lineObject obj ( "", "", "", 0.0, QDateTime(), "", "", "" );
	for ( int i = 0; i < medialines->size(); i++ ) {
		//if(*DEBUG_INFO_ENABLED)
		//  cerr << "importGtktalogCsv::addNewMedia medialine: " << i << endl;
		obj = medialines->at ( i );
		env = curr;

		QString path = obj.getPath();

		//        QMessageBox::warning(0, "path", obj->getPath());
		int startindex = 0;
		int dirindex = 0;


		if ( !obj.getPath().isEmpty() ) {
			path += "/";

			//if(*DEBUG_INFO_ENABLED)
			//	cerr << "importGtktalogCsv::addNewMedia medialine: " << i << endl;

			dirindex = path.indexOf ( "/", 1 );
			while ( dirindex != -1 ) {

				QString dir = path.mid ( startindex, dirindex - startindex );
				//       QMessageBox::warning(0, "new dir", dir);

				curr = db->getDirectoryNode ( env, dir );
				if ( curr == NULL ) {
					curr = db->putDirectoryNode ( env,  dir , obj.getDateTime() , obj.getComment(), obj.getCategory() );
					dircount++;

				}

				startindex = dirindex + 1;
				dirindex = path.indexOf ( "/", dirindex + 1 );

				env = curr;

			}
		}
		dirindex = -1;

		double size = obj.getSize();
		double s = size;
		int   st = UNIT_BYTE;
// 			if ( *DEBUG_INFO_ENABLED )
// 				std::cerr << "adding file size: " << s << std::endl;

		if ( size >= SIZE_ONE_GBYTE * 1024 ) {
			s  = size / SIZE_ONE_GBYTE / 1024;
			st = UNIT_TBYTE;
		}
		else {
			if ( size >= SIZE_ONE_GBYTE && size < SIZE_ONE_GBYTE * 1024 ) {
				s  = size / SIZE_ONE_GBYTE;
				st = UNIT_GBYTE;
			}
			else {
				if ( size >= SIZE_ONE_MBYTE && size < SIZE_ONE_GBYTE ) {
					s  = size / SIZE_ONE_MBYTE;
					st = UNIT_MBYTE;
				}
				else {
					if ( size >= SIZE_ONE_KBYTE && size < SIZE_ONE_MBYTE ) {
						s  = size / SIZE_ONE_KBYTE;
						st = UNIT_KBYTE;
					}
					else {
						s = size;
						st = UNIT_BYTE;
					}
				}
			}
		}

		env = curr;
		curr = db->getFileNode ( env, QString ( obj.getFileName() ) );
		if ( curr == NULL )
			curr = db->putFileNode ( env,  obj.getFileName() , obj.getDateTime() , obj.getComment(), st, s, obj.getCategory() );

		curr = db->getMediaNode ( new_medianame );

		filecount++;
	} // over all items in list

	mediacount++;
	refreshcount++;
	if ( refreshcount == 10 ) {
		guislave->mainw->app->processEvents();
		refreshcount = 0;
	}

	return 0;
}

/*
 * Import a xml report from gtktalog

 example entry:


 <directory>
  <disc_name><![CDATA[_disc_]]></disc_name>
  <directory_name><![CDATA[/_dir_/]]></directory_name>
  <file_name><![CDATA[_filename_]]></file_name>
  <file_size><![CDATA[_size_]]></file_size>
  <file_date><![CDATA[_date_]]></file_date>
  <category><![CDATA[_category_]]></category>
  <description><![CDATA[_descripion_]]></description>
  <information><![CDATA[_information_]]></information>
</directory>

 dummy values are:
 _disc_           ==> disc name
 _dir_            ==> directory
 _filename_     ==> filename
 _size_         ==> size
 _category_     ==> category
 _descripion_   ==> description
 _information_    ==> information

 */


QString tmpdirname;
bool importGtktalogXml::startDocument() {
	//cout << "startDocument: " << endl;
	return true;
}

bool importGtktalogXml::startElement ( const QString&, const QString&,
                                       const QString & tag,
                                       const QXmlAttributes& ) {
	QString line = "";
	last_tag = tag;

	DEBUG_INFO_ENABLED = init_debug_info();
	//if(*DEBUG_INFO_ENABLED)

	tag_content = "";

	return true;
}

bool importGtktalogXml::endElement ( const QString&, const QString & tag, const QString& ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( *DEBUG_INFO_ENABLED )
		cout << "importGtktalogXml::endElement: "  << "tag: " << qPrintable ( tag ) << endl;
	QString line = "";
	last_tag = tag;

	if ( tag_content.length() > 1 ) {
		if ( tag_content.at ( 0 ) == '\n' ) {
			if ( tag_content.at ( 1 ) == '\n' )
				tag_content = tag_content.mid ( 2, tag_content.length() - 1 );
			else
				tag_content = tag_content.mid ( 1, tag_content.length() - 1 );
		}
	}

	if ( *DEBUG_INFO_ENABLED )
		cout << "importGtktalogXml::endElement: "  << "tag content: \"" << qPrintable ( tag_content ) << "\"" << endl;

	if ( tag == "information" ) {
		// found information
		information = tag_content;
		comment = tag_content;
		if ( information.contains ( "archive data" ) )
			is_archive = true;
	}
	else
		if ( tag == "name" ) {
			catalogName = tag_content.replace ( '\n', "" );
			line += "catalogName: " + catalogName + "\n";
		}

		else
			if ( tag == "disc_name" ) {
				// found disc name
				new_medianame = tag_content.replace ( '\n', "" );
			}

			else
				if ( tag == "directory_name" ) {
					// found directory
					directory = tag_content.replace ( "\n", "" );
					if ( !directory.isEmpty() && directory.size() > 1 ) {
						if ( directory.at ( 0 ) == '/' ) {
							directory = directory.right ( directory.size() - 1 );
// 								if(*DEBUG_INFO_ENABLED)
// 												std::cout << "directory name (after remove start /): " <<  qPrintable(directory) << std::endl;
						}
						if ( directory.at ( directory.size() - 1 ) == '/' ) {
							directory = directory.left ( directory.size() - 1 );
// 								if(*DEBUG_INFO_ENABLED)
// 												std::cout << "directory name (after remove end /): " <<  qPrintable(directory) << std::endl;
						}
					}
					else {
						if ( directory.at ( 0 ) == '/' ) {
							directory = "";
						}
					}
				}

				else
					if ( tag == "file_name" ) {
						// found file name
						is_directory = false;
						filename = tag_content.replace ( "\n", "" );
						if ( !filename.isEmpty() && filename.size() > 1 ) {
							if ( filename.at ( 0 ) == '/' ) {
								filename = filename.right ( filename.size() - 1 );
								if ( *DEBUG_INFO_ENABLED )
									std::cout << "filename name (after remove start /): " <<  qPrintable ( filename ) << std::endl;
							}
							if ( filename.at ( filename.size() - 1 ) == '/' ) {
								//filename = filename.left(filename.size()-1 );
								//if(*DEBUG_INFO_ENABLED)
								//				std::cout << "filename name (after remove end /): " <<  qPrintable(filename) << std::endl;
								is_directory = true;
							}
						}
// 				else {
// 								if (filename.at(0) == '/') {
// 												filename="";
// 								}
// 				}
						if ( *DEBUG_INFO_ENABLED )
							std::cout << "file name: " <<  qPrintable ( filename ) << std::endl;
					}
					else
						if ( tag == "file_size" ) {
							// found file size
							size = tag_content.toLong();
							if ( size == -1 )
								size = 0;
// 				 if(*DEBUG_INFO_ENABLED)
//                 std::cout << "file size (tag_content): " <<  qPrintable(tag_content) << ", calculated: " << size << std::endl;
						}

						else
							if ( tag == "file_date" ) {
								// found file size
								datetimestring = tag_content.trimmed();

								int dayindex = datetimestring.indexOf ( "/" );
								int monthindex = datetimestring.indexOf ( "/", dayindex + 1 );
								int yearindex = datetimestring.indexOf ( " ", monthindex + 1 );
								int hourindex = datetimestring.indexOf ( ":", yearindex + 1 );
								int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );

								int day = ( datetimestring.mid ( 0, dayindex ) ).toInt();
								QString day_ = ( datetimestring.mid ( 0, dayindex ) );
								int month = ( datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
								int year = ( datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();

								int hour = ( datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
								int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();

								int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
								QDate date ( year, month, day );
								QTime time ( hour, minute, second );

								datetime = QDateTime ( date, time );

							}

							else
								if ( tag == "file_category" ) {
									// found category
									category = tag_content;
								}

								else
									if ( tag == "description" ) {
										// found description
										description = tag_content;
//         comment = tag_content;
									}

									else
										if ( tag == "report" ) {
											guislave->mainw->db->setDBName ( catalogName );
										}
										else
											if ( tag == "directory" ) {
												// found directory entry: for each file there is one
												// just the start of each file
//         if(*DEBUG_INFO_ENABLED)
//                 std::cout << "directory tag ended. " << std::endl;
												line += "new_medianame: " + new_medianame + "\n";
												line += "directory: " + directory + "\n";
												line += "filename: " + filename + "\n";
												line += "Full path: " + directory + "/" + filename + "\n";
												line += "is_directory: " + QString().setNum ( is_directory ) + "\n";
												line += "is_archive: " + QString().setNum ( is_archive ) + "\n";
												line += "size: " + QString().setNum ( size ) + "\n";
												line += "datetimestring: " + datetimestring + "\n";
												line += "category: " + category + "\n";
												line += "description: " + description + "\n";
												line += "information: " + information + "\n";


												if ( *DEBUG_INFO_ENABLED )
													cerr << "line: " << qPrintable ( line ) << endl;

												if ( !is_directory ) {
													//cerr << line << endl << endl << endl;

													if ( medianame == "" )
														medianame = new_medianame;

													if ( medianame != new_medianame ) {
														//        QMessageBox::warning (0, "info", medianame);
														addNewMedia ( medianame, datetime, comment, category, medialines );
														medialines->clear();
														medianame = new_medianame;
														addNewItem ( new_medianame, directory, filename, size, datetime, comment, information, category, QList<ArchiveFile>(), is_archive );


													}
													else {
														//        QMessageBox::warning (0, "info", "new item");
														addNewItem ( new_medianame, directory, filename, size, datetime, comment, information, category, QList<ArchiveFile>(), is_archive );
													}
												}

												new_medianame = "";
												directory = "";
												filename = "";
												size = -1;
												category = "";
												description = "";
												information = "";
												datetimestring = "";
												tmpdirname = "";
												is_directory = false;
												is_archive = false;

												linecount++;
												progress->setValue ( linecount );
											}

	return true;
}

bool importGtktalogXml::characters ( const QString & ch ) {
	tag_content += ch;
	progress->setValue ( locator->lineNumber() );
	if ( guislave->mainw->app->hasPendingEvents() )
		guislave->mainw->app->processEvents();
	return true;
}

void importGtktalogXml::setDocumentLocator ( QXmlLocator * locator ) {
	this->locator = locator;
	QXmlDefaultHandler::setDocumentLocator ( locator );
}


importGtktalogXml::importGtktalogXml ( GuiSlave * parent, QString filename, bool createdatabase ) {
	this->guislave = parent;
	bool import_ok = true;
	last_tag = "";
	if ( !filename.isEmpty() ) {

		if ( parent->mainw->db == NULL )
			parent->newEvent();
		else {
			if ( createdatabase ) {
				while ( parent->closeEvent() != 0 )
					;
				parent->newEvent();
			}
		}

		guislave = parent;
		db = parent->mainw->db;
		mediacount = 0;
		filecount = 0;
		dircount = 0;
		refreshcount = 0;
		linecount = 0;
		lines = 0;


		QApplication::setOverrideCursor ( Qt::WaitCursor );
		guislave->panelsOFF();

		QFile f ( filename );
		QTextStream t ( &f ); // use a text stream

		if ( f.open ( QIODevice::ReadOnly ) ) {
			QString tmp = "";
			while ( !t.atEnd() ) {
				tmp = t.readLine();
				if ( tmp.contains ( "<directory>" ) )
					lines++;
			}

			f.close();
		}

		progress = new QProgressDialog ( tr ( "Importing XML..." ), tr ( "Cancel" ), 1, lines );
		progress->setCancelButton ( 0 );

		medialines = new QList < lineObject > ();

		line = "";
		fullpath = "";
		path = "";
		size = 0.0;
		new_medianame = "";
		datetimestring = "";
		is_directory = false;
		is_archive = false;

		QXmlInputSource source ( &f );
		QXmlSimpleReader reader;
		reader.setContentHandler ( this );
		reader.setErrorHandler ( this );
		reader.parse ( source );
	}
	else {
		import_ok = false;
	}

	if ( import_ok ) {

		if ( !medialines->isEmpty() ) {
			addNewMedia ( medianame, QDateTime().currentDateTime(), "", "", medialines );
			medialines->clear();
		}

		delete medialines;
		( guislave->mainw->db->getRootNode() ) ->touchDB();
		guislave->panelsON();
		QApplication::restoreOverrideCursor();

		QString msg;
		msg += tr ( "XML import was successful." );
		msg += "\n";
		if ( mediacount == 1 )
			msg += tr ( "1 media" ) + ",\n";
		else
			msg += QString().setNum ( mediacount ) + " " + QString ( tr ( "media" ) ) + ",\n";

		if ( dircount == 1 )
			msg += QString ( tr ( "1 directory,\n" ) );
		else
			msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

		if ( filecount == 1 )
			msg += tr ( "1 file" ) + ",\n";
		else
			msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

		msg += tr ( "are imported." );

		QMessageBox::information ( 0, tr ( "Import successful" ), msg );
		progress->hide();

	}
	else {
		QMessageBox::critical ( 0, tr ( "parse error" ), tr ( "error during parsing" ) );
		QApplication::restoreOverrideCursor();
	} // file dialog canceled

}

importGtktalogXml::~importGtktalogXml() {
	delete ( progress );
}

int importGtktalogXml::addNewItem ( QString medianame, QString path, QString filename, long size, QDateTime datetime, QString comment, QString information,  QString category, QList<ArchiveFile> archivecontent, bool is_archive ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	lineObject l ( medianame, path, filename, size, datetime, comment, information, category, archivecontent, is_archive );
	if ( *DEBUG_INFO_ENABLED )
		cerr << "importGtktalogXml::addNewItem: medianame: " <<
		     qPrintable ( medianame ) << ", path: " << qPrintable ( path ) << ", filename: " <<
		     qPrintable ( filename ) << ", size: " << size << ", date: " <<
		     qPrintable ( datetime.toString() )  << ", comment: " << qPrintable ( comment ) << ", category: " << qPrintable ( category ) << ", is_archive: " << is_archive << endl;

	// look for archive content
	/* example:

	<directory_name><![CDATA[/My Documents/Stock info/a1.zip/]]></directory_name>
	<file_name><![CDATA[ABCG.PRN]]></file_name>
	<directory_name><![CDATA[/My Documents/Stock info/a1.zip/]]></directory_name>
	<file_name><![CDATA[ABG.PRN]]></file_name>

	ABCG.PRN and ABG.PRN are contents of a1.zip

	 */
	QString fullpath = path + "/" + filename;

	bool found = false;
	for ( int i = 0; i < medialines->size(); i++ ) {
		lineObject currObj = medialines->at ( i );
		QString currFullpath = currObj.getPath() + "/" + currObj.getFileName();
		QString subpath = fullpath.left ( currFullpath.length() );
// 				 if (*DEBUG_INFO_ENABLED)
// 										cerr << "compare: " << "currFullpath: " << qPrintable(currFullpath) << " subpath: " << qPrintable(subpath) << " <=> fullpath: " << qPrintable(fullpath) << endl;
		if ( subpath == currFullpath && currObj.isArchive() ) {
			if ( *DEBUG_INFO_ENABLED )
				cerr << "archive file " << qPrintable ( fullpath ) << " for archive " << qPrintable ( currFullpath ) << " found" << endl;
			// its archivecontent
			ArchiveFile af;
			af.fileattr = "-rwx rwx rwx";
			af.user = "importuser";
			af.group = "importgroup";
			af.size = size;
			af.date = datetime;
			af.path = fullpath.right ( fullpath.size() - currFullpath.size() - 1 );


// 							if (*DEBUG_INFO_ENABLED)
// 																cerr << "\nold archive content " << qPrintable( fullpath) << " of archive " << qPrintable(currFullpath) << ":\n" << endl;
// 							for ( int j=0;j< medialines->at(i).archivecontent.size();j++ ) {
// 											ArchiveFile af2 = medialines->at(i).archivecontent.at(j);
// 												QString prettyArchiveFileLine = af2.toPrettyString(true, true, true, true, true, true);
// 												 if (*DEBUG_INFO_ENABLED)
// 																cerr << qPrintable(prettyArchiveFileLine) << endl;
// 							}

			QList<ArchiveFile> archivecontent2;
			for ( int j = 0; j < medialines->at ( i ).archivecontent.size(); j++ ) {
				archivecontent2.append ( medialines->at ( i ).archivecontent.at ( j ) );
			}
			archivecontent2.append ( af );
			currObj.archivecontent.append ( af );
			medialines->removeAt ( i );
			medialines->insert ( i, currObj );

// 							if (*DEBUG_INFO_ENABLED)
// 																cerr << "\ncurr archive content " << qPrintable( fullpath) << " of archive " << qPrintable(currFullpath) << ":\n" << endl;
// 							for ( int j=0;j< currObj.archivecontent.size();j++ ) {
// 											ArchiveFile af2 =  currObj.archivecontent.at(j);
// 												QString prettyArchiveFileLine = af2.toPrettyString(true, true, true, true, true, true);
// 												 if (*DEBUG_INFO_ENABLED)
// 																cerr << qPrintable(prettyArchiveFileLine) << "\n" << endl;
// 							}
			if ( *DEBUG_INFO_ENABLED )
				cerr << "\n----\n" << endl;
			if ( *DEBUG_INFO_ENABLED )
				cerr << "\nnew archive content " << qPrintable ( fullpath ) << " of archive " << qPrintable ( currFullpath ) << ":\n" << endl;
			for ( int j = 0; j < medialines->last().archivecontent.size(); j++ ) {
				ArchiveFile af2 =  medialines->last().archivecontent.at ( j );
				QString prettyArchiveFileLine = af2.toPrettyString ( true, true, true, true, true, true );
				if ( *DEBUG_INFO_ENABLED )
					cerr << qPrintable ( prettyArchiveFileLine ) << endl;
			}
			if ( *DEBUG_INFO_ENABLED )
				cerr << "\n----\n" << endl;
			found = true;
			break;
		}

	}
	if ( !found )
		medialines->append ( l );
	return 0;

}

int importGtktalogXml::addNewMedia ( QString new_medianame, QDateTime media_modification, QString media_comment, QString media_category, QList < lineObject > *medialines ) {
	if ( *DEBUG_INFO_ENABLED )
		cerr << "importGtktalogXml::addNewMedia media: " <<  qPrintable ( new_medianame ) << endl;

	if ( guislave->mainw->db == NULL )
		guislave->newEvent();
	if ( guislave->mainw->db == NULL )
		return 0;

	/* create new media */
	Node *env, *curr;
	curr = db->getMediaNode ( new_medianame );
	if ( curr == NULL )
		curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, media_comment, media_modification, media_category );

	QString msg;
	lineObject obj ( "", "", "", 0.0, QDateTime(), "", "", "" );

	for ( int i = 0; i < medialines->size(); ++i ) {
		obj = medialines->at ( i );
		env = curr;

		QString path = obj.getPath();

		//        QMessageBox::warning(0, "path", obj.getPath());
		int startindex = 0;
		int dirindex = 0;


		if ( !obj.getPath().isEmpty() ) {
			path += "/";
			dirindex = path.indexOf ( "/" );
			while ( dirindex != -1 ) {

				QString dir = path.mid ( startindex, dirindex - startindex );
				//       QMessageBox::warning(0, "new dir", dir);

				curr = db->getDirectoryNode ( env, dir );
				if ( curr == NULL ) {
					curr = db->putDirectoryNode ( env, dir ,  obj.getDateTime() , obj.getComment(), obj.getCategory() );
					dircount++;

				}

				startindex = dirindex + 1;
				dirindex = path.indexOf ( "/", dirindex + 1 );

				env = curr;

			}
		}
		dirindex = -1;

		double size = obj.getSize();
		double s = size;
		int   st = UNIT_BYTE;
// 			if ( *DEBUG_INFO_ENABLED )
// 				std::cerr << "adding file size: " << s << std::endl;

		if ( size >= SIZE_ONE_GBYTE * 1024 ) {
			s  = size / SIZE_ONE_GBYTE / 1024;
			st = UNIT_TBYTE;
		}
		else {
			if ( size >= SIZE_ONE_GBYTE && size < SIZE_ONE_GBYTE * 1024 ) {
				s  = size / SIZE_ONE_GBYTE;
				st = UNIT_GBYTE;
			}
			else {
				if ( size >= SIZE_ONE_MBYTE && size < SIZE_ONE_GBYTE ) {
					s  = size / SIZE_ONE_MBYTE;
					st = UNIT_MBYTE;
				}
				else {
					if ( size >= SIZE_ONE_KBYTE && size < SIZE_ONE_MBYTE ) {
						s  = size / SIZE_ONE_KBYTE;
						st = UNIT_KBYTE;
					}
					else {
						s = size;
						st = UNIT_BYTE;
					}
				}
			}
		}

		env = curr;
		curr = db->getFileNode ( env,  obj.getFileName() );
		if ( curr == NULL ) {
			curr = db->putFileNode ( env, obj.getFileName() , obj.getDateTime() , obj.getComment() , st, s, obj.getCategory(), obj.archivecontent, obj.getInformation() );

		}

		curr = db->getMediaNode ( new_medianame );

		filecount++;
	} // over all items in list

	mediacount++;
	refreshcount++;
	if ( refreshcount == 10 ) {
		guislave->mainw->app->processEvents();
		refreshcount = 0;
	}


	return 0;
}

bool importGtktalogXml::fatalError ( const QXmlParseException &exception ) {
	std::cerr << "Parse error at line " << exception.lineNumber()
	          << ", " << "column " << exception.columnNumber() << ": "
	          << qPrintable ( exception.message() ) << std::endl;
	return true;
}

// ------------------ WhereIsIt classic ---------------------
importWhereIsItXml* class_link_whereisit;
QString tmpdirname_whereisit;
int lines;

bool importWhereIsItXml::startDocument() {

	//cout << "startDocument: " << endl;
	return true;
}

bool importWhereIsItXml::startElement ( const QString&, const QString&,
                                        const QString & name2,
                                        const QXmlAttributes & atts ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( *DEBUG_INFO_ENABLED )
		cout << "startElement: " << qPrintable ( name2 ) << endl;
	currentText = "";

	if ( atts.length() > 0 ) {
		if ( *DEBUG_INFO_ENABLED ) {
			cout << "atts: ";
			for ( int i = 0; i < atts.length(); i++ ) {
				cout << "atts[" << i << "]: "  << qPrintable ( atts.qName ( i ) ) << "=" << qPrintable ( atts.value ( i ) ) << endl;
			}
			cout  << endl;
		}
	}

	QString line = "";
	last_tag = name2;


	if ( name2 == "REPORT" ) {
		catalogName = atts.value ( "Title" ).trimmed();
		if ( *DEBUG_INFO_ENABLED ) {
			line += "catalogName: \"" + catalogName + "\"\n";
			guislave->mainw->db->setDBName ( catalogName );
			std::cout << qPrintable ( line ) << endl;
		}
	}

	if ( name2 == "ITEM" ) {
		if ( *DEBUG_INFO_ENABLED )
			std::cout << "Itemtype: " << qPrintable ( atts.value ( "ItemType" ) ) << endl;

		if ( atts.value ( "ItemType" ) == "Disk" )
			last_type = "media";

		if ( atts.value ( "ItemType" ) == "File" )
			last_type = "file";

		if ( atts.value ( "ItemType" ) == "Folder" )
			last_type = "folder";

	}


	return true;
}

bool importWhereIsItXml::endElement ( const QString&, const QString & tag, const QString& ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( *DEBUG_INFO_ENABLED )
		cout << "endElement: "  << "tag: " << qPrintable ( tag ) << endl;

	if ( *DEBUG_INFO_ENABLED )
		cout << "cdata: " << qPrintable ( currentText ) << endl;


	if ( tag == "DISK_NUM" ) {
		number = currentText.toInt();
		//    std::cout << "number: " << QString().setNum( number ) << endl;

	}

	else
		if ( tag == "SIZE" ) {
			if ( last_type == "file" ) {
				currentText = currentText.remove ( ' ' );
				currentText.replace ( QRegExp ( " " ), "" );
				//        std::cout << "cleaned: " << currentText << endl;
				size = currentText.toInt();
			}
		}

		else
			if ( tag == "DATE" ) {

				currentText = currentText.trimmed();

				int index0 = currentText.indexOf ( "-", 0 );
				int index1 = currentText.indexOf ( "-", index0 - 1 );
				int index2 = currentText.indexOf ( "-", index1 + 1 );
				int day = 1;
				int month = 1;
				int year = 2010;
				QString month_str = "";
				if ( index0 == 2 ) {
					// DD-MM-YYYY
					//std::cout << "date: " << qPrintable(currentText) << endl;
					day = ( currentText.mid ( 0, index0 ) ).toInt();
					//std::cout << "day: " << day << endl;
					month_str = currentText.mid ( index1 + 1, index2 - index1 - index0 - 3 ) ;
					//std::cout << "month_str: " << qPrintable( month_str) << endl;
					year = currentText.mid ( index2 + 1 ).toInt();
				}
				if ( index0 == 4 ) {
					// YYYY-MM-DD
					//std::cout << "date: " << qPrintable(currentText) << endl;
					year = ( currentText.mid ( 0, index0 ) ).toInt();
					//std::cout << "day: " << day << endl;
					month_str = currentText.mid ( index1 + 1, index2 - index1 - index0 - 3 ) ;
					//std::cout << "month_str: " << qPrintable( month_str) << endl;
					day = currentText.mid ( index2 + 1 ).toInt();
				}

				month = 1;
				if ( month_str == "JAN" )
					month = 1;
				else
					if ( month_str == "FEB" )
						month = 2;
					else
						if ( month_str == "MAR" || month_str == "MRZ" )
							month = 3;
						else
							if ( month_str == "APR" )
								month = 4;
							else
								if ( month_str == "MAY" || month_str == "MAI" )
									month = 5;
								else
									if ( month_str == "JUN" )
										month = 6;
									else
										if ( month_str == "JUL" )
											month = 7;
										else
											if ( month_str == "AUG" )
												month = 8;
											else
												if ( month_str == "SEP" )
													month = 9;
												else
													if ( month_str == "OCT" || month_str == "OKT" )
														month = 10;
													else
														if ( month_str == "NOV" )
															month = 11;
														else
															if ( month_str == "DEC" || month_str == "DEZ" )
																month = 12;

				//                      std::cout << "month: " << month << endl;

				QDate date ( year, month, day );
				if ( !date.isValid() )
					date = QDate::currentDate ();

				datetime.setDate ( date );
			}

			else
				if ( tag == "DISK_NAME" ) {
					if ( last_type == "media" )
						{}
				}

				else
					if ( tag == "DISK_TYPE" ) {
						if ( last_type == "media" ) {
							class_link_whereisit->last_media_type = 1;
							if ( currentText == "CD-R"  || currentText == "CD-ROM" || currentText == "CD+RW" )
								class_link_whereisit->last_media_type = 1;
							else
								if ( currentText == "DVD-R" || currentText == "DVD-ROM" || currentText == "DVD+R" || currentText == "DVD+R DL" || currentText == "DVD-R" )
									class_link_whereisit->last_media_type = 2;
							//else if (currentText == "BD-R" || currentText == "BD-RE" || currentText == "BD-ROM" || currentText == "DVD+R DL" || currentText == "DVD-R")
							//              class_link_whereisit->last_media_type = 2;
								else
									if ( currentText == "ISO Image" )
										class_link_whereisit->last_media_type = 1;
						}
					}

					else
						if ( tag == "PATH" ) {
							if ( last_type == "folder" ) {
//                      std::cout << "getCdata_whereisit_parse(): PATH: \"" << qPrintable(currentText.replace("\n", "")) << "\"" << endl;
								path = currentText.replace ( "\n", "" );
							}

							if ( last_type == "file" ) {
//                      std::cout << "getCdata_whereisit_parse(): PATH: \"" << qPrintable(currentText.replace("\n", "")) << "\"" << endl;
								path = currentText.replace ( "\n", "" );
							}

						}

						else
							if ( tag == "NAME" ) {
								if ( last_type == "media" ) {
									//std::cout << "getCdata_whereisit_parse(): NAME: \"" << currentText.replace("\n", "") << "\"" << endl;
									new_medianame = currentText.replace ( "\n", "" );
								}

								if ( last_type == "folder" ) {
									//      std::cout << "getCdata_whereisit_parse(): DATA: \"" << currentText.replace("\n", "") << "\"" << endl;
									folder = currentText.replace ( "\n", "" );
								}

								if ( last_type == "file" ) {
									//std::cout << "getCdata_whereisit_parse(): DATA: \"" << currentText.replace("\n", "") << "\"" << endl;
									file = currentText.replace ( "\n", "" );
								}
							}

							else
								if ( tag == "EXT" ) {
									if ( last_type == "file" ) {
										//std::cout << "getCdata_whereisit_parse(): EXT: \"" << currentText.replace("\n", "") << "\"" << endl;
//            std::cout << "getCdata_whereisit_parse(): EXT\""<< currentText.replace("\n", "") << "\", " << "cut: " << file.rightRef(currentText.replace("\n", "").length()+2)<< endl;
										if ( file.rightRef ( currentText.replace ( "\n", "" ).length() + 1 ) !=  QString ( "." + currentText.replace ( "\n", "" ) ) )
											file += QString ( "." + currentText.replace ( "\n", "" ) );
									}
								}

								else
									if ( tag == "TIME" ) {
										// found file time

										datetimestring = currentText.trimmed();

										int hourindex = datetimestring.indexOf ( ":", 1 );
										int minuteindex = datetimestring.indexOf ( ":", hourindex + 1 );
										int hour = ( datetimestring.mid ( 0, minuteindex - hourindex - 1 ) ).toInt();
										int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
										int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();


										QTime time ( hour, minute, second );


										datetime.setTime ( time );

									}

									else
										if ( tag == "DESC" || tag == "DESCRIPTION" ) {
											// tmp
											currentText.truncate ( 254 );

											if ( last_type == "media" ) {
												comment = currentText;
												if ( *DEBUG_INFO_ENABLED )
													std::cout << "media comment found: " << qPrintable ( currentText ) << endl;
											}

											if ( last_type == "folder" ) {
												comment = currentText;
												if ( *DEBUG_INFO_ENABLED )
													std::cout << "folder comment found: " << qPrintable ( currentText ) << endl;
											}

											if ( last_type == "file" ) {
												comment = currentText;
												if ( *DEBUG_INFO_ENABLED )
													std::cout << "file comment found: " << qPrintable ( currentText ) << endl;
											}
										}

//     if ( tag == "DATA" ) {
//         if ( last_type == "media" ) {
//             QString line = "\"" + new_medianame + "\"\n";
//             if (*DEBUG_INFO_ENABLED)
//                 std::cout << "media name found: " << qPrintable(line) << endl;
//         }
//
//         if ( last_type == "folder" ) {
//             QString line = "\"" + folder + "\"\n";
//             if (*DEBUG_INFO_ENABLED)
//                 std::cout << "folder name found: " << qPrintable(line) << endl;
//         }
//
//         if ( last_type == "file" ) {
//             QString line = "\"" + file + "\"\n";
//             if (*DEBUG_INFO_ENABLED)
//                 std::cout << "file name found: " << qPrintable(line) << endl;
//         }
//     }

										else
											if ( tag == "FLAG" ) {
												// this is tag
												//std::cout << "getCdata_whereisit_parse(): FLAG: \"" << currentText.replace("\n", "") << "\"" << endl;
//            std::cout << "getCdata_whereisit_parse(): FLAG\""<< currentText.replace("\n", "") << "\", " << "cut: " << file.rightRef(currentText.replace("\n", "").length()+2)<< endl;
												if ( currentText.size() > 0 )
													comment += tr ( "tag:" ) + " " + currentText;
											}

											else
												if ( tag == "ITEM" ) {
													if ( last_type == "media" ) {
														if ( *DEBUG_INFO_ENABLED )
															std::cout << "add media: " << qPrintable ( new_medianame ) << endl;
														//        QMessageBox::warning (0, "info", medianame);
														medianame = new_medianame;
														//QDateTime datetime = datetime;

														if ( guislave->mainw->db == NULL ) {
															guislave->newEvent();
															guislave->mainw->db->setDBName ( catalogName );
														}

														/* create new media */
														Node *env, *curr;

														env = db->getRootNode();
														curr = db->putMediaNode ( new_medianame, number, QObject::tr ( "importuser" ), class_link_whereisit->last_media_type, comment, datetime );

														mediacount++;
														linecount++;
														//last_upper_container_node = db->getRootNode();
														last_type = "empty";
														//      guislave->mainw->app->processEvents();
													} // media

													if ( last_type == "folder" ) {
														if ( db != NULL ) {

															Node * env2, *curr2, *curr3;
															env2 = db->getMediaNode ( number ) ;

															if ( env2 == NULL && !new_medianame.isEmpty() ) {
																// try name now....
																env2 = db->getMediaNode ( new_medianame ) ;
															}


															if ( env2 != NULL ) {
																curr3 = env2;
																if ( path.length() > 1 ) {
																	//        std::cout << "path: \"" << path << "\"" << endl;

																	QString tmp_path = path;
																	QString tmp_path2 = "";
																	/*              int index = 1;
																	              int index2 = 1;*/
																	QStringList fields = tmp_path.split ( '\\' );

																	for ( QStringList::Iterator point = fields.begin(); point != fields.end(); ++point ) {
																		tmp_path2 = *point;

																		//        std::cout << "subpath: \"" << tmp_path2 << "\"" << endl;

																		if ( tmp_path2 != "" ) {
																			curr2 = curr3;
																			curr3 = db->getDirectoryNode ( curr3, tmp_path2 );

																			if ( curr3 == NULL ) {
																				if ( *DEBUG_INFO_ENABLED )
																					std::cout << "add folder: " << qPrintable ( tmp_path2 ) << "/" << qPrintable ( folder ) /*<< ", comment: " << qPrintable(comment)*/ << endl;
																				curr3 = db->putDirectoryNode ( curr2, tmp_path2 , datetime , comment );
																			}
																		}
																	}

																}
																curr3 = db->putDirectoryNode ( curr3 , folder , datetime , comment );


															}
														}

														dircount++;
														linecount++;
														last_type = "empty";
														//      guislave->mainw->app->processEvents();
													} // folder

													if ( last_type == "file" ) {
														if ( db != NULL ) {
															Node * env2, *curr2, *curr3;
															double s = size;
															int   st = UNIT_BYTE;
															// 			if ( *DEBUG_INFO_ENABLED )
															// 				std::cerr << "adding file size: " << s << std::endl;

															if ( size >= SIZE_ONE_GBYTE * 1024 ) {
																s  = size / SIZE_ONE_GBYTE / 1024;
																st = UNIT_TBYTE;
															}
															else {
																if ( size >= SIZE_ONE_GBYTE && size < SIZE_ONE_GBYTE * 1024 ) {
																	s  = size / SIZE_ONE_GBYTE;
																	st = UNIT_GBYTE;
																}
																else {
																	if ( size >= SIZE_ONE_MBYTE && size < SIZE_ONE_GBYTE ) {
																		s  = size / SIZE_ONE_MBYTE;
																		st = UNIT_MBYTE;
																	}
																	else {
																		if ( size >= SIZE_ONE_KBYTE && size < SIZE_ONE_MBYTE ) {
																			s  = size / SIZE_ONE_KBYTE;
																			st = UNIT_KBYTE;
																		}
																		else {
																			s = size;
																			st = UNIT_BYTE;
																		}
																	}
																}
															}

															env2 = db->getMediaNode ( number ) ;
															if ( env2 == NULL && !new_medianame.isEmpty() ) {
																// try name now....
																env2 = db->getMediaNode ( new_medianame ) ;
															}

															if ( env2 != NULL ) {
																curr3 = env2;
																if ( path.length() > 1 ) {
																	//        std::cout << "path: \"" << path << "\"" << endl;

																	QString tmp_path = path;
																	QString tmp_path2 = "";
																	/*              int index = 1;
																	              int index2 = 1;*/
																	QStringList fields = tmp_path.split ( '\\' );

																	for ( QStringList::Iterator point = fields.begin(); point != fields.end(); ++point ) {
																		tmp_path2 = *point;

																		//        std::cout << "subpath: \"" << tmp_path2 << "\"" << endl;

																		if ( tmp_path2 != "" ) {
																			curr2 = curr3;
																			curr3 = db->getDirectoryNode ( curr3,  tmp_path2 );

																			if ( curr3 == NULL ) {
																				if ( *DEBUG_INFO_ENABLED )
																					std::cout << "add file: " << qPrintable ( tmp_path2 ) << "/" << qPrintable ( file ) << /*", comment: " << qPrintable(comment) <<*/ endl;
																				curr3 = db->putDirectoryNode ( curr2, tmp_path2 , datetime , comment );
																			}
																		}
																	}

																}

																if ( *DEBUG_INFO_ENABLED )
																	std::cout << "file: \"" << qPrintable ( file ) << "\"" << endl;

																curr3 = db->putFileNode ( curr3 ,  file ,  datetime , comment , st, s );


															}
														}

														filecount++;
														linecount++;
														last_type = "empty";



													} // file
													comment = "";
												} // ITEM
	currentText = "";
	return true;
}

bool importWhereIsItXml::characters ( const QString & ch ) {
	//xmldata = ch;

	QString name2 = ch;
	currentText += ch;
	progress->setValue ( locator->lineNumber() );
	if ( guislave->mainw->app->hasPendingEvents() )
		guislave->mainw->app->processEvents();
	return true;
}

void importWhereIsItXml::setDocumentLocator ( QXmlLocator * locator ) {
	this->locator = locator;
	QXmlDefaultHandler::setDocumentLocator ( locator );
}


importWhereIsItXml::importWhereIsItXml ( GuiSlave * parent, QString filename, bool createdatabase ) {
	this->guislave = parent;
	bool import_ok = true;
	last_tag = "";
	last_type = "empty";
	last_media_type = 0;
	if ( !filename.isEmpty() ) {

		if ( parent->mainw->db == NULL )
			parent->newEvent();
		else {
			if ( createdatabase ) {
				while ( parent->closeEvent() != 0 )
					;
				parent->newEvent();
			}
		}

		guislave = parent;
		db = parent->mainw->db;
		mediacount = 0;
		filecount = 0;
		dircount = 0;
		refreshcount = 0;
		linecount = 0;
		lines = 0;


		QApplication::setOverrideCursor ( Qt::WaitCursor );
		guislave->panelsOFF();

		QFile f ( filename );
		QTextStream t ( &f ); // use a text stream

		int all_lines = 0;

		/*std::cout << "counting lines... ";*/
		register FILE *filehdl;
		register int c;
		if ( ( filehdl = fopen ( filename.toLocal8Bit().constData(), "r" ) ) == NULL )
			std::cerr << "Can't open " << qPrintable ( filename ) << endl;
		else {
			//      file_raise( f, FALSE );
			//      statfile( f );
			while ( ( c = getc ( filehdl ) ) != EOF ) {
				if ( c == '\n' )
					all_lines++;
				//      guislave->mainw->app->processEvents();
			}

			fclose ( filehdl );
		}
		/*std::cout << "done.\nlines:  " << QString().setNum( all_lines ) << endl;*/




		/*
		    if ( f.open( IO_ReadOnly ) ) {
		      QString tmp = "";
		      while ( !t.eof() ) {
		        tmp = t.readLine();
		        if ( tmp.contains( "<ROW " ) )
		          all_lines++;
		        guislave->mainw->app->processEvents();
		      }
		      f.close();
		    }
		    */

		lines = all_lines;


		// now we need a link to itself :(
		class_link_whereisit = this;
		progress = new QProgressDialog ( tr ( "Importing XML..." ), tr ( "Cancel" ), 1, lines );
		progress->setCancelButton ( 0 );

		//  guislave->mainw->status->setText(tr("Importing xml..."));

		line = "";
		fullpath = "";
		path = "";
		size = 0.0;
		new_medianame = "";
		datetimestring = "";
		comment = "";
		folder = "";
		file = "";
		last_upper_container_node = NULL;
		number = 0;
		filenumber = 0;
		linecount = 0;

		QXmlInputSource source ( &f );
		QXmlSimpleReader reader;
		reader.setContentHandler ( this );
		reader.setErrorHandler ( this );
		reader.parse ( source );
	}
	else {
		import_ok = false;
	}
	//  guislave->mainw->status->setText(tr("Finished."));
	if ( import_ok ) {

		( guislave->mainw->db->getRootNode() ) ->touchDB();
		guislave->panelsON();
		QApplication::restoreOverrideCursor();

		QString msg;
		msg += tr ( "Importing XML from WhereIsIt was successful." );
		msg += "\n";
		if ( mediacount == 1 )
			msg += tr ( "1 media" ) + ",\n";
		else
			msg += QString().setNum ( mediacount ) + " " + QString ( tr ( "media" ) ) + ",\n";

		if ( dircount == 1 )
			msg += QString ( tr ( "1 directory,\n" ) );
		else
			msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

		if ( filecount == 1 )
			msg += tr ( "1 file" ) + ",\n";
		else
			msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

		msg += tr ( "are imported." );

		QMessageBox::information ( 0, tr ( "Import successful" ), msg );
		progress->hide();

	}
	else {
		QMessageBox::critical ( 0, tr ( "parse error" ), tr ( "error during parsing" ) );
		QApplication::restoreOverrideCursor();
	} // file dialog canceled
}

importWhereIsItXml::~importWhereIsItXml() {
	delete ( progress );
}

bool importWhereIsItXml::fatalError ( const QXmlParseException &exception ) {
	std::cerr << "Parse error at line " << exception.lineNumber()
	          << ", " << "column " << exception.columnNumber() << ": "
	          << qPrintable ( exception.message() ) << std::endl;
	return true;
}


// -------- end ----------




import::import ( GuiSlave * parent ) {
	bool createdatabase = false;
	bool correctbadstyle = false;
	QString separator = ";";
	QString filename;
	int type = -1;
	ImportDialog *d = new ImportDialog ( 0 );
	d->setLastDir ( parent->mainw->cconfig->lastDir );
	QString mypattern = "%size_human%%tab%%dateyear4%%space%%time_hm%%tab%%fullpath%";
	d->genericRegex_lineedit->setText(mypattern);
	d->exec();

	if ( d->OK == 1 ) {
		separator = d->separator;
		filename = d->filename;
		if ( !filename.isEmpty() )
			parent->mainw->cconfig->lastDir = QDir ( d->filename ).dirName();
		createdatabase = d->newdatabase;
		correctbadstyle = d->correctbadstyle;
		separator = d->separator;
		type = d->type;
		mypattern = d->genericRegex_lineedit->text();
		delete d;

		if ( type == 0 )
			importGtktalogCsv import ( parent, separator, filename, createdatabase, correctbadstyle );
		else
			if ( type == 1 )
				importGtktalogXml import ( parent, filename, createdatabase );
			else
				if ( type == 2 )
					importWhereIsItXml import ( parent, filename, createdatabase );
				else
					if ( type == 3 )
						importGtktalogCsv import ( parent, separator, filename, createdatabase, correctbadstyle, "kat-dece" );
					else
						if ( type == 4 )
							importGtktalogCsv import ( parent, separator, filename, createdatabase, correctbadstyle, "disclib" );
						else
							if ( type == 5 )
								importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "visualcd" );
							else
								if ( type == 6 )
									importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "vvv" );
								else
									if ( type == 7 )
										importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "afo" );
									else
										if ( type == 8 )
											importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "filearchivist" );
										else
											if ( type == 9 )
												importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "advanceddiskcatalog" );
											else
												if ( type == 10 )
													importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "whereisit" );
												else {
													if ( type == 11 ) {
														importGtktalogCsv import ( parent, separator, filename, createdatabase, false, "generic_plaintext_regex", mypattern);
													}
													else {
														//    cerr << "wrong type!!!" << endl;
													}
												}
	}
}
// kate: indent-mode cstyle; replace-tabs off; tab-width 8;          ;
