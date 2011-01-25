/****************************************************************************
Hyper's CD Catalog
A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke (crissi99@gmx.de)
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#include <qmessagebox.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qdatetime.h>
#include <q3progressdialog.h>
//Added by qt3to4:
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

#include "import.h"
#include "importdialog.h"
#include "mainwidget.h"
#include "dbase.h"
#include "cdcat.h"
#include "config.h"

lineObject::lineObject ( QString medianame, QString path, QString filename,
                         float size, QDateTime datetime ) {
    this->medianame = medianame;
    this->path = path;
    this->filename = filename;
    this->size = size;
    this->datetime = datetime;
}

lineObject::~lineObject() {}
lineObject::lineObject ( const lineObject& newobj ) {
    medianame = newobj.medianame;
    path = newobj.path;
    filename = newobj.filename;
    size = newobj.size;
    datetime = newobj.datetime;
}

lineObject& lineObject::operator= ( const lineObject& newobj ) {
    medianame = newobj.medianame;
    path = newobj.path;
    filename = newobj.filename;
    size = newobj.size;
    datetime = newobj.datetime;
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

float lineObject::getSize() {
    return size;
}

QDateTime lineObject::getDateTime() {
    return datetime;
}

/*
 * import a file with following format:
 * <media>/<path>/..../<file><separator><size><separator><day/month/year><space><hour>:<minute>:<second>
 * the hashmark '#' can be used as comment in csv file.
 *
 */

importGtktalogCsv::importGtktalogCsv ( GuiSlave * parent, QString separator, QString filename, bool createdatabase, bool correctbadstyle, QString csvtype ) {

    this->filename = filename;
    this->correctbadstyle = correctbadstyle;
    this->createdatabase = createdatabase;
    this->separator = separator;
    DEBUG_INFO_ENABLED = init_debug_info();

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

        if ( parent->mainw->db != NULL ) {

            guislave = parent;
            db = parent->mainw->db;
            mediacount = 0;
            filecount = 0;
            dircount = 0;
            refreshcount = 0;
            int linecount = 0;
            int lines = 0;


            QApplication::setOverrideCursor ( Qt::waitCursor );
            guislave->panelsOFF();

            QFile f ( filename );

            if ( f.open ( QIODevice::ReadOnly ) ) {
                QTextStream in ( &f );
                while ( !in.atEnd() ) {
                    QString line = in.readLine();
                    if ( line.contains ( '\n' ) or line.contains ( "\r\n" ) )
                        lines ++;
                }
                f.close();
            }

            if ( f.open ( QIODevice::ReadOnly ) ) {	     // file opened successfully

                Q3ProgressDialog progress ( 0, "progressdialog", true );
                progress.setLabelText ( tr ( "Importing CSV..." ) );
                progress.setCancelButton ( 0 );
                progress.setTotalSteps ( lines );

                Q3TextStream t ( &f );	             // use a text stream
                QString medianame = "";

                medialines = new QList < lineObject > ();
                //medialines->setAutoDelete( TRUE );	// the list owns the objects

                while ( !t.eof() ) {
                    QString line;
                    QString fullpath;
                    QString path;
                    QString dirpath;
                    float size;
                    //QDate date;
                    QDateTime datetime;
                    QString new_medianame;
                    QString datetimestring;
                    QString pathsep = "/";


                    line = t.readLine();	// line of text excluding '\n'

                    if ( !line.startsWith ( "#" ) && !line.isEmpty() ) {
                        if (line.contains('\\'))
                            pathsep = "\\";
                        if ( correctbadstyle ) {
                            //     QMessageBox::warning (0, "wrong", line);
                            int idx = 0;
                            while ( idx != -1 ) {
                                idx = line.find ( QString ( separator + pathsep ), idx );
                                if ( idx != -1 ) {
                                    line.replace ( idx, QString ( separator + pathsep ).length(), "/" );
                                }
                            }
                            idx = 0;
                            while ( idx != -1 ) {
                                idx = line.find ( QString ( pathsep+ separator ), idx );
                                if ( idx != -1 ) {
                                    line.replace ( idx, QString ( pathsep + separator ).length(), "/" );
                                }
                            }
                            //   QMessageBox::warning (0, "fine", line);
                        }

                        if (csvtype == "gtktalog") {

                                int mediaindex = line.find ( pathsep, 0 );
                                int pathindex = line.find ( separator, mediaindex + 1 );
                                fullpath = ( line.mid ( mediaindex, pathindex - mediaindex ) );

                                if (pathsep == "\\")
                                        fullpath = fullpath.replace("\\", "/");
                                if (fullpath.at(0) != '/')
                                        fullpath = "/"+fullpath;
                                //if(*DEBUG_INFO_ENABLED)
                                //	cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;
                                new_medianame = ( line.left ( mediaindex ) );
                                //if(*DEBUG_INFO_ENABLED)
                                //	cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;

                                path = fullpath.mid(mediaindex + 1, medianame.length()+1);
                                //if(*DEBUG_INFO_ENABLED)
                                //	cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;

                                int sizeindex = line.find ( separator, pathindex + 1 );
                                QString sizestring = line.mid ( pathindex + 1, sizeindex - pathindex - 1 ).stripWhiteSpace();
                                //if(*DEBUG_INFO_ENABLED)
                                //	cerr << "importGtktalogCsv sizestring: " << qPrintable(sizestring) << endl;
                                size = ( ( line.mid ( pathindex + 1, sizeindex - pathindex - 1 ) ).stripWhiteSpace() ).toFloat();

                                //if(*DEBUG_INFO_ENABLED)
                                //	cerr << "importGtktalogCsv size: " << size << endl;

                                datetimestring = ( line.mid ( sizeindex + 1, line.length() ) ).stripWhiteSpace();
                                if (*DEBUG_INFO_ENABLED)
                                cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

                                // date is normally day/month/year hour:minute:second
                                QString datesep = "/";
                                    
                                    // date in other format: day.month.year hour:minute:second
                                if (datetimestring.contains('.'))
                                        datesep = ".";
                                int dayindex = datetimestring.find ( datesep );
                                int monthindex = datetimestring.find ( datesep, dayindex + 1 );
                                int yearindex = datetimestring.find ( " ", monthindex + 1 );
                                int hourindex = datetimestring.find ( ":", yearindex + 1 );
                                int minuteindex = datetimestring.find ( ":", hourindex + 1 );

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
                                //	cerr << "importGtktalogCsv fileindex: " << fileindex << endl;

                                QString dirpath = "";
                                if ( fileindex != 0 )
                                dirpath = fullpath.mid ( 1, fileindex-1 );
                                QString filename = fullpath.mid ( fileindex + 1, fullpath.length() - fileindex );
                                
                                if (*DEBUG_INFO_ENABLED)
                                //if (!validDate)
                                {
                                        QString msg;
                                        msg += "line: " + line + "\n";
                                        msg += "path: " + path + "\n";
                                        msg += "fullpath: " + fullpath + "\n";
                                        msg += "sizestring: "+ sizestring + "\n";
                                        msg += "sizeindex: " + QString().setNum(sizeindex) + "\n";
                                        msg += "size: " + QString().setNum(size) + "\n";
                                        msg += "month: " + QString().setNum(month) + "\n";
                                        msg += "hour: " + QString().setNum(hour) + "\n";
                                        msg += "minute: " + QString().setNum(minute) + "\n";
                                        msg += "second: " + QString().setNum(second) + "\n";
                                        msg += "year: " + QString().setNum(year) + "\n";
                                        msg += "day: " + QString().setNum(day) + "\n";
                                        msg += "datetimestring: " + datetimestring + "\n";
                                        msg += "new_medianame: " + new_medianame + "\n";
                                        msg+= "directory path: "+dirpath+"\n";
                                        msg+="file name: "+filename+"\n";

                                        //QMessageBox::warning(0, "line", msg);
                                }

                        }
                        if (csvtype == "kat-dece") {
                                /*
                                 * format:
                                 * "Number";"Location";"CD name";"CD categy";"Name";"Extension";"Length";"Date";"Path";"Comment"
                                 * 
                                 * sample line:
                                 * "";"";"newiso";"! Alle Medien  ";"empire-efi-logo-alt256";".png";" 11,669";"2010.01.07  08:05";"Extra\Themes\Default\";""
                                 */
                                if (linecount == 0) {
                                        linecount++;
                                        continue;
                                }
                                // FIXME
                                separator=";";
                                QStringList csvList = line.split(separator);
                                if (csvList.count() != 10) {
                                     if(*DEBUG_INFO_ENABLED)
                                        cerr << "importGtktalogCsv invalid line (not 9 fields): " << qPrintable(line) << endl;
                                     continue;
                                }
                                        
                                filename = QString(csvList.at(4)).replace("\"","")+QString(csvList.at(5)).replace("\"","");
                                
                                QString sizestring = QString(csvList.at(6)).replace("\"","").replace(",", ".").replace(".", "").replace("#", "").stripWhiteSpace();
                                
                                // size is float and its kib!
                                size = uint(QString(csvList.at(6)).replace("\"","").replace(",", ".").replace(".", "").replace("#", "").stripWhiteSpace().toFloat()*1024);                                
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv size: " << size << endl;
                                
                                
                                datetimestring = QString(csvList.at(7)).replace("\"","");
//                                 if (*DEBUG_INFO_ENABLED)
//                                 cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

                                 QString datesep=".";
                                // date is in format: year.month.day hour:minute
                                if (datetimestring.contains('.'))
                                        datesep = ".";
                                int yearindex = datetimestring.find ( datesep, 0 );
                                int monthindex = datetimestring.find ( datesep, yearindex + 1 );
                                int dayindex = datetimestring.find ( " ", monthindex + 1 );
                                int hourindex = datetimestring.find ( " ", dayindex + 1 );
                                int minuteindex = datetimestring.find ( ":", hourindex + 1 );
                                int year = ( datetimestring.mid ( 0, yearindex ) ).toInt();
                                QString monthstring = datetimestring.mid ( yearindex+1, monthindex-yearindex -1 );
                                int month = ( datetimestring.mid ( yearindex+1, monthindex-yearindex -1 ) ).toInt();
                                QString daystring = datetimestring.mid ( monthindex + 1, hourindex-dayindex+1 );
                                int day = ( datetimestring.mid ( monthindex + 1, hourindex-dayindex+1 ) ).toInt();
                                //bool validDate = date.setYMD( year, month, day );
                                QString hourstring = datetimestring.mid ( hourindex + 1, minuteindex- hourindex-1 );
                                int hour = ( datetimestring.mid ( hourindex + 1, minuteindex- hourindex-1 ) ).toInt();
                                int minute = ( datetimestring.mid (dayindex + 1, minuteindex-1) ).toInt();
                                int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
                                QDate date ( year, month, day );
                                QTime time ( hour, minute, second );
                                datetime = QDateTime ( date, time );
                                
                                new_medianame = QString(csvList.at(2)).replace("\"","");
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;
                                
                                dirpath = QString(csvList.at(8)).replace("\"","");
                                dirpath = dirpath.mid(0, dirpath.length()-1 );
                                if (pathsep == "\\")
                                        dirpath = dirpath.replace("\\", "/");
//                                 if (!dirpath.isEmpty() && dirpath.at(0) != '/')
//                                         dirpath = "/"+dirpath;
                                
                                path = dirpath+"/"+filename;
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;
                                
                                fullpath = path;

//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;

                                if (*DEBUG_INFO_ENABLED)
                                //if (!validDate)
                                {
                                        QString msg;
                                        msg += "line: " + line + "\n";
                                        msg += "path: " + path + "\n";
                                        msg += "fullpath: " + fullpath + "\n";
                                        msg += "size: " + QString().setNum(size) + "\n";
                                        msg += "sizestring: " + sizestring + "\n";
                                        msg += "month: " + QString().setNum(month) + "\n";
                                        msg += "hour: " + QString().setNum(hour) + "\n";
                                        msg += "minute: " + QString().setNum(minute) + "\n";
                                        msg += "year: " + QString().setNum(year) + "\n";
                                        msg += "day: " + QString().setNum(day) + "\n";
                                        msg += "datetimestring: " + datetimestring + "\n";
                                        msg += "monthstring: " + monthstring + "\n";
                                        msg += "hourstring: " + hourstring + "\n";
                                        msg += "daystring: " + daystring + "\n";
                                        msg += "new_medianame: " + new_medianame + "\n";
                                        msg += "directory path: "+dirpath+"\n";
                                        msg +="file name: "+filename+"\n";

//                                         QMessageBox::warning(0, "line", msg);
                                }

                        }
                        if (csvtype == "disclib") {
                                /*
                                 * format:
                                 * name*"volume"*filepath*path*size*Volume serial*date*type*"category"*"comments"*"location" 
                                 * 
                                 * sample line:
                                 * acc_dis.chm*"DISK_00001_HelpCD"*DISK_00001_HelpCD\acc_dis.chm*DISK_00001_HelpCD\*20 832*6C721915*23.08.2001 12:00:00*chm*""*""*""
                                 */
                                // FIXME
                                separator="*";
                                QStringList csvList = line.split(separator);
//                                 if (csvList.count() < 10) {
//                                      if(*DEBUG_INFO_ENABLED)
//                                         cerr << "importGtktalogCsv invalid line (at least 10 fields): " << qPrintable(line) << endl;
//                                      continue;
//                                 }
                                if(*DEBUG_INFO_ENABLED) {
                                 cerr << "line: " << qPrintable(line) << endl;
                                  for (int i = 0; i < csvList.size(); ++i)
                                        cout << "csvList[" << i << "]: " << csvList.at(i).toLocal8Bit().constData() << endl;
                                }
                                        
                                filename = QString(csvList.at(0));
                                QString sizestring = QString(csvList.at(3)) ;
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv sizestring raw: " <<qPrintable( sizestring) << endl;
                                if (sizestring.contains("/")) {
                                   // DISK_00003_Downloads\hhcode_and_exe\TestCHM\Agent\images/5 720 => 5 720
                                   sizestring = QString(sizestring.split("/").at(1)).replace(" ","");
//                                    if(*DEBUG_INFO_ENABLED)
//                                     cerr << "importGtktalogCsv sizestring raw2: " <<qPrintable( sizestring) << endl;
                                }
                                //QString(csvList.at(3)).stripWhiteSpace();
                                size = QString(sizestring).toInt();
                                                   
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv size: " << size << endl;
                                
                                
                                datetimestring = QString(csvList.at(5));
//                                 if (*DEBUG_INFO_ENABLED)
//                                 cerr << "importGtktalogCsv datetimestring: " << qPrintable(datetimestring) << endl;

                                // date is normally day/month/year hour:minute:second
                                QString datesep = "/";
                                    
                                    // date in other format: day.month.year hour:minute:second
                                if (datetimestring.contains('.'))
                                        datesep = ".";
                                int dayindex = datetimestring.find ( datesep );
                                int monthindex = datetimestring.find ( datesep, dayindex + 1 );
                                int yearindex = datetimestring.find ( " ", monthindex + 1 );
                                int hourindex = datetimestring.find ( ":", yearindex + 1 );
                                int minuteindex = datetimestring.find ( ":", hourindex + 1 );

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
                                
                                new_medianame = QString(csvList.at(1)).replace("\"","");
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv new_medianame: " << qPrintable(new_medianame) << endl;
                                
                                dirpath = QString(csvList.at(3)).replace("\"","");
//                                 dirpath = dirpath.mid(0, dirpath.length()-1 );
                                dirpath = dirpath.replace(new_medianame, "");
                                if (pathsep == "\\")
                                        dirpath = dirpath.replace("\\", "/");
//                                 if (!dirpath.isEmpty() && dirpath.at(0) != '/')
//                                         dirpath = "/"+dirpath;
                                    if (!dirpath.isEmpty() && dirpath.at(0) == '/')
                                        dirpath = dirpath.mid(1, dirpath.length()-1 );
                                
                                path = dirpath+"/"+filename;
//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv path: " << qPrintable(path) << endl;
                                
                                fullpath = path;

//                                 if(*DEBUG_INFO_ENABLED)
//                                      cerr << "importGtktalogCsv fullpath: " << qPrintable(fullpath) << endl;
                                
                                QString comment = QString( csvList.at(8)).replace("\"","");

                                if (*DEBUG_INFO_ENABLED)
                                //if (!validDate)
                                {
                                        QString msg;
//                                         msg += "line: " + line + "\n";
                                        msg += "path: " + path + "\n";
                                        msg += "fullpath: " + fullpath + "\n";
                                        msg += "size: " + QString().setNum(size) + "\n";
                                        msg += "sizestring: " + sizestring + "\n";
                                        msg += "month: " + QString().setNum(month) + "\n";
                                        msg += "hour: " + QString().setNum(hour) + "\n";
                                        msg += "minute: " + QString().setNum(minute) + "\n";
                                        msg += "year: " + QString().setNum(year) + "\n";
                                        msg += "day: " + QString().setNum(day) + "\n";
                                        msg += "datetimestring: " + datetimestring + "\n";
//                                         msg += "monthstring: " + monthstring + "\n";
//                                         msg += "hourstring: " + hourstring + "\n";
//                                         msg += "daystring: " + daystring + "\n";
                                        msg += "new_medianame: " + new_medianame + "\n";
                                        msg += "directory path: "+dirpath+"\n";
                                        msg +="file name: "+filename+"\n";
                                        msg += "comment: " + comment + "\n";
                                        
                                        cerr << "msg: " << qPrintable(msg) << endl;

//                                         QMessageBox::warning(0, "line", msg);
                                }

                        }


                        if ( medianame == ""  )
                        medianame = new_medianame;

                        if ( medianame != new_medianame ) {
                            //        QMessageBox::warning (0, "info", medianame);
                            addNewMedia ( medianame, medialines );
                                medialines->clear();
                                medianame = new_medianame;
                                addNewItem ( new_medianame, dirpath, filename, size, datetime );


                            } else {
                                //        QMessageBox::warning (0, "info", "new item");
                                addNewItem ( new_medianame, dirpath, filename, size, datetime );
                            }

                    }			// valid line
                    linecount++;
                    progress.setProgress ( linecount );
                }
                f.close();
                if ( !medialines->isEmpty() ) {
                    addNewMedia ( medianame, medialines );
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
                    msg += QString ( tr ( "1 directory:,\n" ) );
                else
                    msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

                if ( filecount < 2 )
                    msg += tr ( "1 File" ) + ",\n";
                else
                    msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

                msg += tr ( "are imported." );

                QMessageBox::information ( 0, tr ( "Import successful" ), msg );

            } else {

                if ( f.status() == IO_ReadError )
                    QMessageBox::critical ( 0, tr ( "file read error" ), tr ( "Could not read file" ) );

                if ( f.status() == IO_FatalError )
                    QMessageBox::critical ( 0, tr ( "Fatal error" ), tr ( "Fatal error occured." ) );

                if ( f.status() == IO_OpenError )
                    QMessageBox::critical ( 0, tr ( "file open error" ), tr ( "Could not open file" ) );
            }
        } // file dialog canceled
    }

}

importGtktalogCsv::~importGtktalogCsv() {}

int importGtktalogCsv::addNewItem ( QString medianame, QString path,
                                    QString filename, float size, QDateTime datetime ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    lineObject l ( medianame, path, filename, size, datetime );
    if (*DEBUG_INFO_ENABLED)
        cerr << "importGtktalogCsv::addNewItem: medianame: " <<
             qPrintable(medianame) << ", path: " << qPrintable(path) << ", filename: " <<
             qPrintable(filename) << ", size: " << size << ", date: " <<
             qPrintable(datetime.toString()) << endl;
    medialines->append ( l );

    // QMessageBox::critical( 0, "item", "new item!");
    return 0;

}

int importGtktalogCsv::addNewMedia ( QString new_medianame, QList < lineObject > *medialines ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    //if(*DEBUG_INFO_ENABLED)
    //	cerr << "importGtktalogCsv::addNewMedia media: " <<  qPrintable(new_medianame) << endl;

    if ( guislave->mainw->db == NULL )
        guislave->newEvent();
    if ( guislave->mainw->db == NULL )
        return 0;

    /* create new media */
    Node *env, *curr;
    curr = db->getMediaNode ( new_medianame );
    if ( curr == NULL )
        curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, "", QDateTime().currentDateTime() );

    QString msg;
    lineObject obj("", "", "", 0.0, QDateTime());
    for ( int i = 0; i < medialines->size(); i++ ) {
        //if(*DEBUG_INFO_ENABLED)
        //	cerr << "importGtktalogCsv::addNewMedia medialine: " << i << endl;
        obj = medialines->at ( i );
        env = curr;

        QString path = obj.getPath();

        //        QMessageBox::warning(0, "path", obj->getPath());
        int startindex = 0;
        int dirindex = 0;


        if ( !obj.getPath().isEmpty() ) {
            path += "/";
            dirindex = path.find ( "/" );
            while ( dirindex != -1 ) {

                QString dir = path.mid ( startindex, dirindex - startindex );
                //       QMessageBox::warning(0, "new dir", dir);

                curr = db->getDirectoryNode ( env, dir );
                if ( curr == NULL ) {
                    curr = db->putDirectoryNode ( env,  dir , obj.getDateTime() , "" );
                    dircount++;

                }

                startindex = dirindex + 1;
                dirindex = path.find ( "/", dirindex + 1 );

                env = curr;

            }
        }
        dirindex = -1;

        uint size = ( uint ) obj.getSize();
        float s;
        int st;

        if ( size > ( uint ) ( 1024 * 1024 * 1024 * 2 ) ) {          // 2 << 30 or 2^31
            s = ( double ) size / ( double ) ( 1024 * 1024 * 1024 );
            st = GBYTE;
        }
        if ( size > ( uint ) ( 1024 * 1024 ) ) {
            s = ( double ) size / ( double ) ( 1024 * 1024 );
            st = MBYTE;
        }
        if ( size > ( uint ) 1024 ) {
            s = ( double ) size / ( double ) 1024;
            st = KBYTE;
        } else {
            s = size;
            st = BYTE;
        }

        env = curr;
        curr = db->getFileNode ( env, QString ( obj.getFileName() ) );
        if ( curr == NULL )
            curr = db->putFileNode ( env,  obj.getFileName() , obj.getDateTime() , "", st, s );

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
 _disc_ 					==> disc name
 _dir_						==> directory
 _filename_			==> filename
 _size_					==> size
 _category_			==> category
 _descripion_		==> description
 _information_		==> information

 */


importGtktalogXml* class_link;

QString tmpdirname;
bool importGtktalogXml::startDocument() {
    //cout << "startDocument: " << endl;
    return TRUE;
}
bool importGtktalogXml::startElement( const QString&, const QString&,
                                      const QString & name2,
                                      const QXmlAttributes& ) {
    QString line = "";
    last_tag = name2;
    return TRUE;
}
bool importGtktalogXml::endElement( const QString&, const QString & tag, const QString& ) {
    //cout << "endElement: "  << "tag: " << tag << endl;
    QString name2 = tag;
    QString line = "";

    class_link->last_tag = name2;

    if (name2 == "information") {
        // found information
    }
    if ( name2 == "name" ) {
        line += "catalogName: " + class_link->catalogName + "\n";
    }
    if ( name2 == "report" ) {
        class_link->guislave->mainw->db->setName ( class_link->catalogName );
    }
    if ( name2 == "directory" ) {
        //	std::cout << "directory tag ended. " << std::endl;
        line += "new_medianame: " + new_medianame + "\n";
        line += "directory: " + directory + "\n";
        line += "filename: " + filename + "\n";
        line += "Full path: " + path + "\n";
        line += "size: " + QString().setNum( size ) + "\n";
        line += "datetimestring: " + datetimestring + "\n";
        line += "category: " + categorie + "\n";
        line += "description: " + description + "\n";
        line += "information: " + information + "\n";

        //		cerr << line << endl << endl << endl;


        line += "new_medianame: " + class_link->new_medianame + "\n";
        line += "directory: " + class_link->directory + "\n";
        line += "filename: " + class_link->filename + "\n";
        line += "Full path: " + class_link->path + "\n";
        line += "size: " + QString().setNum ( class_link->size ) + "\n";
        line += "datetimestring: " + class_link->datetimestring + "\n";
        line += "category: " + class_link->categorie + "\n";
        line += "description: " + class_link->description + "\n";
        line += "information: " + class_link->information + "\n";

        //cerr << line << endl << endl << endl;

        if ( class_link->medianame == "" )
            class_link->medianame = class_link->new_medianame;

        if ( class_link->medianame != class_link->new_medianame ) {
            //        QMessageBox::warning (0, "info", class_link->medianame);
            class_link->addNewMedia ( class_link->medianame, class_link->medialines );
            class_link->medialines->clear();
            class_link->medianame = class_link->new_medianame;
            class_link->addNewItem ( class_link->new_medianame, class_link->directory, class_link->filename, class_link->size, class_link->datetime );
        } else {
            //        QMessageBox::warning (0, "info", "new item");
            class_link->addNewItem ( class_link->medianame, class_link->directory, class_link->filename, class_link->size, class_link->datetime );
        }

        class_link->new_medianame = "";
        class_link->directory = "";
        class_link->filename = "";
        class_link->size = 0;
        class_link->categorie = "";
        class_link->description = "";
        class_link->information = "";
        class_link->datetimestring = "";
        tmpdirname = "";

        class_link->linecount++;
        class_link->progress->setProgress ( class_link->linecount );
    }

    return TRUE;
}

bool importGtktalogXml::characters ( const QString & ch ) {

    QString name2 = ch;

    if ( class_link->last_tag == "name" ) {
        // found catalog name
        class_link->catalogName += name2;
    }


    if ( class_link->last_tag == "directory" ) {
        // found directory entry: for each file there is one
        // just the start of each file
    }

    if ( class_link->last_tag == "disc_name" ) {
        // found disc name
        class_link->new_medianame += name2;
    }

    if ( class_link->last_tag == "directory_name" ) {
        // found directory name
        tmpdirname += name2;
        class_link->directory = tmpdirname.mid ( 1, tmpdirname.length() - 2 );
    }

    if ( class_link->last_tag == "file_name" ) {
        // found file name
        class_link->filename += name2;
    }

    if ( class_link->last_tag == "file_size" ) {
        // found file size
        class_link->size = name2.toInt();
    }

    if ( class_link->last_tag == "file_date" ) {
        // found file size
        class_link->datetimestring = name2.stripWhiteSpace();

        int dayindex = class_link->datetimestring.find ( "/" );
        int monthindex = class_link->datetimestring.find ( "/", dayindex + 1 );
        int yearindex = class_link->datetimestring.find ( " ", monthindex + 1 );
        int hourindex = class_link->datetimestring.find ( ":", yearindex + 1 );
        int minuteindex = class_link->datetimestring.find ( ":", hourindex + 1 );

        int day = ( class_link->datetimestring.mid ( 0, dayindex ) ).toInt();
        QString day_ = ( class_link->datetimestring.mid ( 0, dayindex ) );
        int month = ( class_link->datetimestring.mid ( dayindex + 1, monthindex - dayindex - 1 ) ).toInt();
        int year = ( class_link->datetimestring.mid ( monthindex + 1, hourindex - monthindex - 4 ) ).toInt();

        int hour = ( class_link->datetimestring.mid ( yearindex + 1, minuteindex - hourindex - 1 ) ).toInt();
        int minute = ( class_link->datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();

        int second = ( class_link->datetimestring.mid ( minuteindex + 1, class_link->datetimestring.length() - 1 ) ).toInt();
        QDate date ( year, month, day );
        QTime time ( hour, minute, second );

        class_link->datetime = QDateTime ( date, time );

    }


    if ( class_link->last_tag == "file_category" ) {
        // found category
        class_link->categorie = name2;
    }

    if ( class_link->last_tag == "description" ) {
        // found description
        class_link->description = name2;
    }


    if ( class_link->last_tag == "information" ) {
        // found information
        class_link->information = name2;
    }

    return TRUE;
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


        QApplication::setOverrideCursor ( Qt::waitCursor );
        guislave->panelsOFF();

        QFile f ( filename );
        Q3TextStream t ( &f );	// use a text stream

        if ( f.open ( QIODevice::ReadOnly ) ) {
            QString tmp = "";
            while ( !t.eof() ) {
                tmp = t.readLine();
                if ( tmp.contains ( "<directory>" ) )
                    lines++;
            }

            f.close();
        }

        progress = new Q3ProgressDialog ( 0, "progressdialog", true );
        progress->setLabelText ( tr ( "Importing XML..." ) );
        progress->setCancelButton ( 0 );
        progress->setTotalSteps ( lines );

        medialines = new QList < lineObject > ();

        line = "";
        fullpath = "";
        path = "";
        size = 0.0;
        new_medianame = "";
        datetimestring = "";

        // now we need a link to itself :(
        class_link = this;
        QXmlInputSource source( f );
        QXmlSimpleReader reader;
        reader.setContentHandler( this );
        reader.setErrorHandler(this);
        reader.parse( source );
        progress->hide();
    } else {
        import_ok = false;
    }

    if ( import_ok ) {

        if ( !medialines->isEmpty() ) {
            addNewMedia ( medianame, medialines );
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
            msg += QString ( tr ( "1 directory:,\n" ) );
        else
            msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

        if ( filecount == 1 )
            msg += tr ( "1 File" ) + ",\n";
        else
            msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

        msg += tr ( "are imported." );

        QMessageBox::information ( 0, tr ( "Import successful" ), msg );
        progress->hide();

    } else {
        QMessageBox::critical ( 0, tr ( "parse error" ), tr ( "error during parsing" ) );
        QApplication::restoreOverrideCursor();
    } // file dialog canceled

}

importGtktalogXml::~importGtktalogXml() {
    delete ( progress );
}

int importGtktalogXml::addNewItem ( QString medianame, QString path, QString filename, float size, QDateTime datetime ) {
    medialines->append ( lineObject ( medianame, path, filename, size, datetime ) );

    // QMessageBox::critical( 0, "item", "new item!");
    return 0;

}

int importGtktalogXml::addNewMedia ( QString new_medianame, QList < lineObject > *medialines ) {
    // QMessageBox::critical(0, "media", new_medianame);

    if ( guislave->mainw->db == NULL )
        guislave->newEvent();
    if ( guislave->mainw->db == NULL )
        return 0;

    /* create new media */
    Node *env, *curr;
    curr = db->getMediaNode ( new_medianame );
    if ( curr == NULL )
        curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, "",  class_link->datetime );

    QString msg;
    lineObject *obj;

    for ( int i = 0; i < medialines->size(); ++i ) {
        *obj = medialines->at ( i );
        env = curr;

        QString path = obj->getPath();

        //        QMessageBox::warning(0, "path", obj->getPath());
        int startindex = 0;
        int dirindex = 0;


        if ( !obj->getPath().isEmpty() ) {
            path += "/";
            dirindex = path.find ( "/" );
            while ( dirindex != -1 ) {

                QString dir = path.mid ( startindex, dirindex - startindex );
                //       QMessageBox::warning(0, "new dir", dir);

                curr = db->getDirectoryNode ( env, dir );
                if ( curr == NULL ) {
                    curr = db->putDirectoryNode ( env, dir ,  obj->getDateTime() , "" );
                    dircount++;

                }

                startindex = dirindex + 1;
                dirindex = path.find ( "/", dirindex + 1 );

                env = curr;

            }
        }
        dirindex = -1;

        uint size = ( uint ) obj->getSize();
        float s;
        int st;

        if ( size > ( uint ) ( 1024 * 1024 * 1024 * 2 ) ) {
            s = ( double ) size / ( double ) ( 1024 * 1024 * 1024 );
            st = GBYTE;
        }
        if ( size > ( uint ) ( 1024 * 1024 ) ) {
            s = ( double ) size / ( double ) ( 1024 * 1024 );
            st = MBYTE;
        }
        if ( size > ( uint ) 1024 ) {
            s = ( double ) size / ( double ) 1024;
            st = KBYTE;
        } else {
            s = size;
            st = BYTE;
        }

        env = curr;
        curr = db->getFileNode ( env,  obj->getFileName() );
        if ( curr == NULL )
            curr = db->putFileNode ( env, obj->getFileName() , obj->getDateTime() , "" , st, s );

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

bool importGtktalogXml::fatalError(const QXmlParseException &exception)
{
    std::cerr << "Parse error at line " << exception.lineNumber()
              << ", " << "column " << exception.columnNumber() << ": "
              << qPrintable(exception.message()) << std::endl;
    return true;
}

// ------------------ WhereIsIt classic ---------------------
importWhereIsItXml* class_link_whereisit;
QString tmpdirname_whereisit;
int lines;

bool importWhereIsItXml::startDocument() {

    //cout << "startDocument: " << endl;
    return TRUE;
}



bool importWhereIsItXml::startElement( const QString&, const QString&,
                                       const QString & name2,
                                       const QXmlAttributes & atts ) {
    cout << "startElement: " << qPrintable(name2) << endl;
    currentText = "";

    if (atts.length() > 0) {
        cout << "atts: ";
        for (int i=0;i<atts.length();i++) {
            cout << "atts[" << i << "]: "  << qPrintable(atts.qName(i)) << "=" << qPrintable(atts.value(i)) << endl;

        }
        cout  << endl;
    }

    QString line = "";
    last_tag = name2;


    if ( name2 == "REPORT" ) {
        catalogName = atts.value( "Title").stripWhiteSpace();
        line += "catalogName: \"" + catalogName + "\"\n";
        guislave->mainw->db->setName ( catalogName );
        std::cout << qPrintable(line) << endl;
    }

    if ( name2 == "ITEM" ) {
        std::cout << "Itemtype: " << qPrintable ( atts.value("ItemType") ) << endl;

        if ( atts.value("ItemType") == "Disk" )
            last_type = "media";

        if ( atts.value("ItemType") == "File" )
            last_type = "file";

        if ( atts.value("ItemType") == "Folder" )
            last_type = "folder";

    }


    return TRUE;
}

bool importWhereIsItXml::endElement( const QString&, const QString & tag, const QString& ) {
    cout << "endElement: "  << "tag: " << qPrintable(tag) << endl;

    cout << "cdata: " << qPrintable(currentText) << endl;

    if ( last_tag == "ITEM" ) {
        // found ITEM entry: for each file there is one
        // just the start of each file
    }

    if ( last_tag == "DISK_NUM" ) {
        number = currentText.toInt();
        //		std::cout << "number: " << QString().setNum( number ) << endl;

    }

    if ( last_tag == "SIZE" ) {
        if ( last_type == "file" ) {
            currentText = currentText.remove( ' ' );
            currentText.replace ( QRegExp ( " " ),"" );
            //				std::cout << "cleaned: " << currentText << endl;
            size = currentText.toInt();
        }
    }

    if ( last_tag == "DATE" ) {

        currentText.stripWhiteSpace();

        int dayindex = currentText.find( "-",0 );
        int monthindex = currentText.find( "-", dayindex - 1 );
        int yearindex = currentText.find("-", monthindex+1);
//             std::cout << "date: " << qPrintable(currentText) << endl;

//             std::cout << "daystr: " << qPrintable(currentText.mid ( 0, dayindex ))  << ", dayindex: " << dayindex << endl;
        int day = ( currentText.mid ( 0, dayindex ) ).toInt();
//             		std::cout << "day: " << day << endl;

        QString month_str = currentText.mid ( monthindex+1, yearindex-monthindex-dayindex-3 ) ;
//             	std::cout << "month_str: " << qPrintable( month_str) << endl;
        int month=1;
        if ( month_str == "JAN" )
            month = 1;
        else if ( month_str == "FEB" )
            month = 2;
        else if ( month_str == "MAR" || month_str == "MRZ" )
            month = 3;
        else if ( month_str == "APR" )
            month = 4;
        else if ( month_str == "MAY" || month_str == "MAI" )
            month = 5;
        else if ( month_str == "JUN" )
            month = 6;
        else if ( month_str == "JUL" )
            month = 7;
        else if ( month_str == "AUG" )
            month = 8;
        else if ( month_str == "SEP" )
            month = 9;
        else if ( month_str == "OCT" || month_str == "OKT" )
            month = 10;
        else if ( month_str == "NOV" )
            month = 11;
        else if ( month_str == "DEC" || month_str == "DEZ" )
            month = 12;

        //			std::cout << "month: " << month << endl;

        int year = currentText.mid( yearindex+1).toInt();

//             std::cout << "year : " << qPrintable( currentText.mid( yearindex+1)) << endl;
        QDate date ( year, month, day );
        if ( !date.isValid() )
            date = QDate::currentDate ();

        datetime.setDate ( date );
    }

    if ( last_tag == "DISK_NAME" ) {
        if ( last_type == "media" ) {}
    }

    if ( last_tag == "DISK_TYPE" ) {
        if ( last_type == "media" ) {}
    }

    if ( last_tag == "PATH" ) {
        if ( last_type == "folder" ) {
//                 			std::cout << "getCdata_whereisit_parse(): PATH: \"" << qPrintable(currentText.replace("\n", "")) << "\"" << endl;
            path = currentText.replace("\n", "");
        }

        if ( last_type == "file" ) {
//                 			std::cout << "getCdata_whereisit_parse(): PATH: \"" << qPrintable(currentText.replace("\n", "")) << "\"" << endl;
            path = currentText.replace("\n", "");
        }

    }

    if ( last_tag == "NAME" ) {
        if ( last_type == "media" ) {
            //std::cout << "getCdata_whereisit_parse(): NAME: \"" << currentText.replace("\n", "") << "\"" << endl;
            new_medianame = currentText.replace("\n", "");
        }

        if ( last_type == "folder" ) {
            //			std::cout << "getCdata_whereisit_parse(): DATA: \"" << currentText.replace("\n", "") << "\"" << endl;
            folder = currentText.replace("\n", "");
        }

        if ( last_type == "file" ) {
            //std::cout << "getCdata_whereisit_parse(): DATA: \"" << currentText.replace("\n", "") << "\"" << endl;
            file = currentText.replace("\n", "");
        }
    }

    if ( last_tag == "TIME" ) {
        // found file time

        datetimestring = currentText.stripWhiteSpace();

        int hourindex = datetimestring.find ( ":", 1 );
        int minuteindex = datetimestring.find ( ":", hourindex + 1 );
        int hour = ( datetimestring.mid ( 0, minuteindex - hourindex - 1 ) ).toInt();
        int minute = ( datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
        int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();


        QTime time ( hour, minute, second );


        datetime.setTime ( time );

    }

    if ( last_tag == "DESC" ) {
        // tmp
        currentText.truncate ( 254 );

        if ( last_type == "media" ) {
            comment = currentText;
        }

        if ( last_type == "folder" ) {
            comment = currentText;
        }

        if ( last_type == "file" ) {
            comment = currentText;
        }
    }




    QString name2 = tag;

    last_tag = "";
    QString line = "";

    if ( name2 == "DATA" ) {
        if ( last_type == "media" ) {
            QString line = "\"" + new_medianame + "\"\n";
            if (*DEBUG_INFO_ENABLED)
                std::cout << "media name found: " << qPrintable(line) << endl;
        }

        if ( last_type == "folder" ) {
            QString line = "\"" + folder + "\"\n";
            if (*DEBUG_INFO_ENABLED)
                std::cout << "folder name found: " << qPrintable(line) << endl;
        }

        if ( last_type == "file" ) {
            QString line = "\"" + file + "\"\n";
            if (*DEBUG_INFO_ENABLED)
                std::cout << "file name found: " << qPrintable(line) << endl;
        }
    }

    if ( name2 == "ITEM" ) {
        if ( last_type == "media" ) {
            std::cout << "add media: " << qPrintable(new_medianame) << endl;
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
            curr = db->putMediaNode ( new_medianame ,
                                      number, QObject::tr ( "importuser" ) , CD, comment, datetime );

            mediacount++;
            linecount++;
            //last_upper_container_node = db->getRootNode();
            last_type = "empty";
            //			guislave->mainw->app->processEvents();
        } // media

        if ( last_type == "folder" ) {

            if ( db != NULL ) {
                std::cout << "add folder: " << qPrintable(folder) << endl;
                Node * env2, *curr2, *curr3;
                env2 = db->getMediaNode ( number ) ;


                if ( env2 != NULL ) {
                    curr3 = env2;
                    if ( path.length() > 1 ) {
                        //				std::cout << "path: \"" << path << "\"" << endl;

                        QString tmp_path = path;
                        QString tmp_path2 = "";
                        /*							int index = 1;
                        							int index2 = 1;*/
                        QStringList fields = QStringList::split ( '\\', tmp_path );

                        for ( QStringList::Iterator point = fields.begin(); point != fields.end(); ++point ) {
                            tmp_path2 = *point;

                            //				std::cout << "subpath: \"" << tmp_path2 << "\"" << endl;

                            if ( tmp_path2 != "" ) {
                                curr2 = curr3;
                                curr3 = db->getDirectoryNode ( curr3, tmp_path2 );

                                if ( curr3 == NULL )
                                    curr3 = db->putDirectoryNode ( curr2, tmp_path2 , datetime , comment );
                                comment="";
                            }
                        }

                    }
                    curr3 = db->putDirectoryNode ( curr3 , folder , datetime , comment );
                    comment="";


                }
            }

            dircount++;
            linecount++;
            last_type = "empty";
            //			guislave->mainw->app->processEvents();
        } // folder

        if ( last_type == "file" ) {

            if ( db != NULL ) {
                std::cout << "add file: " << qPrintable(file) << endl;
                Node * env2, *curr2, *curr3;
                float s;
                int st;

                if ( size > ( uint ) ( 1024 * 1024 * 1024 * 2 ) ) {
                    s = ( double ) size / ( double ) ( 1024 * 1024 * 1024 );
                    st = GBYTE;
                }
                if ( size > ( uint ) ( 1024 * 1024 ) ) {
                    s = ( double ) size / ( double ) ( 1024 * 1024 );
                    st = MBYTE;
                }
                if ( size > ( uint ) 1024 ) {
                    s = ( double ) size / ( double ) 1024;
                    st = KBYTE;
                } else {
                    s = size;
                    st = BYTE;
                }

                env2 = db->getMediaNode ( number ) ;


                if ( env2 != NULL ) {
                    curr3 = env2;
                    if ( path.length() > 1 ) {
                        //				std::cout << "path: \"" << path << "\"" << endl;

                        QString tmp_path = path;
                        QString tmp_path2 = "";
                        /*							int index = 1;
                        							int index2 = 1;*/
                        QStringList fields = QStringList::split ( '\\', tmp_path );

                        for ( QStringList::Iterator point = fields.begin(); point != fields.end(); ++point ) {
                            tmp_path2 = *point;

                            //				std::cout << "subpath: \"" << tmp_path2 << "\"" << endl;

                            if ( tmp_path2 != "" ) {
                                curr2 = curr3;
                                curr3 = db->getDirectoryNode ( curr3,  tmp_path2 );

                                if ( curr3 == NULL )
                                    curr3 = db->putDirectoryNode ( curr2, tmp_path2 , datetime , comment );
                                comment="";
                            }
                        }

                    }

                    std::cout << "file: \"" << qPrintable(file) << "\"" << endl;

                    curr3 = db->putFileNode ( curr3 ,  file ,  datetime , comment , st, s );
                    comment="";


                }
            }

            filecount++;
            linecount++;
            last_type = "empty";
            setProgress();



        } // file

    } // ITEM
    currentText = "";
    return TRUE;
}


bool importWhereIsItXml::characters ( const QString & ch ) {
    //xmldata = ch;

    QString name2 = ch;
    currentText+= ch;
    return true;
}




importWhereIsItXml::importWhereIsItXml ( GuiSlave * parent, QString filename, bool createdatabase ) {
    this->guislave = parent;
    bool import_ok = true;
    last_tag = "";
    last_type = "empty";
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


        QApplication::setOverrideCursor ( Qt::waitCursor );
        guislave->panelsOFF();

        QFile f ( filename );
        Q3TextStream t ( &f );	// use a text stream

        int all_lines = 0;

        /*std::cout << "counting lines... ";*/
        register FILE *filehdl;
        register int c;
        if ( ( filehdl = fopen ( filename, "r" ) ) == NULL )
            std::cerr << "Can't open " << qPrintable ( filename ) << endl;
        else {
            //			file_raise( f, FALSE );
            //			statfile( f );
            while ( ( c = getc ( filehdl ) ) != EOF ) {
                if ( c == '\n' )
                    all_lines++;
                //			guislave->mainw->app->processEvents();
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

        progress = new Q3ProgressDialog ( 0, "progressdialog", true );
        progress->setLabelText ( tr ( "Importing XML..." ) );
        progress->setCancelButton ( 0 );
        progress->setTotalSteps ( lines );
        progress->show();

        //	guislave->mainw->status->setText(tr("Importing xml..."));

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

        QXmlInputSource source( f );
        QXmlSimpleReader reader;
        reader.setContentHandler( this );
        reader.setErrorHandler(this);
        reader.parse( source );
        progress->hide();
    } else {
        import_ok = false;
    }
    //	guislave->mainw->status->setText(tr("Finished."));
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
            msg += QString ( tr ( "1 directory:,\n" ) );
        else
            msg += QString().setNum ( dircount ) + " " + QString ( tr ( "directories" ) ) + ",\n";

        if ( filecount == 1 )
            msg += tr ( "1 File" ) + ",\n";
        else
            msg += QString().setNum ( filecount ) + " " + QString ( tr ( "files" ) ) + "\n";

        msg += tr ( "are imported." );

        QMessageBox::information ( 0, tr ( "Import successful" ), msg );
        progress->hide();

    } else {
        QMessageBox::critical ( 0, tr ( "parse error" ), tr ( "error during parsing" ) );
        QApplication::restoreOverrideCursor();
    } // file dialog canceled
}

importWhereIsItXml::~importWhereIsItXml() {
    delete ( progress );
}

void importWhereIsItXml::setProgress() {
    //std::cout << "setProgress(): " << linecount << "\n";
//     int line = XML_GetCurrentLineNumber ( parser );
//     progress->setProgress ( line );
    guislave->mainw->app->processEvents();
}

bool importWhereIsItXml::fatalError(const QXmlParseException &exception)
{
    std::cerr << "Parse error at line " << exception.lineNumber()
              << ", " << "column " << exception.columnNumber() << ": "
              << qPrintable(exception.message()) << std::endl;
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
    d->exec();

    if ( d->OK == 1 ) {
        separator = d->separator;
        filename = d->filename;
        createdatabase = d->newdatabase;
        correctbadstyle = d->correctbadstyle;
        separator = d->separator;
        type = d->type;

        delete d;

        if ( type == 0 )
            importGtktalogCsv import0 ( parent, separator, filename, createdatabase, correctbadstyle );
        if ( type == 1 )
            importGtktalogXml import1 ( parent, filename, createdatabase );
        if ( type == 2 )
            importWhereIsItXml import2 ( parent, filename, createdatabase );
        if ( type == 3 )
            importGtktalogCsv import3 ( parent, separator, filename, createdatabase, correctbadstyle, "kat-dece" );
        if ( type == 4 )
            importGtktalogCsv import4 ( parent, separator, filename, createdatabase, correctbadstyle, "disclib" );
        else {
            //		cerr << "wrong type!!!" << endl;
        }
    }
}
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
