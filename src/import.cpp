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
#include <expat.h>
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
    this->medianame = newobj.medianame;
    this->path = newobj.path;
    this->filename = newobj.filename;
    this->size = newobj.size;
    this->datetime = newobj.datetime;
}

lineObject& lineObject::operator= ( const lineObject& newobj ) {
    this->medianame = newobj.medianame;
    this->path = newobj.path;
    this->filename = newobj.filename;
    this->size = newobj.size;
    this->datetime = newobj.datetime;
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

importGtktalogCsv::importGtktalogCsv ( GuiSlave * parent, QString separator, QString filename, bool createdatabase, bool correctbadstyle ) {

    this->filename = filename;
    this->correctbadstyle = correctbadstyle;
    this->createdatabase = createdatabase;
    this->separator = separator;

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
                    if ( line.contains ( '\n' ) )
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
                    float size;
                    //QDate date;
                    QString new_medianame;
                    QString datetimestring;


                    line = t.readLine();	// line of text excluding '\n'

                    if ( !line.startsWith ( "#" ) && !line.isEmpty() ) {

                        if ( correctbadstyle ) {
                            //     QMessageBox::warning (0, "wrong", line);
                            int idx = 0;
                            while ( idx != -1 ) {
                                idx = line.find ( QString ( separator + "/" ), idx );
                                if ( idx != -1 ) {
                                    line.replace ( idx, QString ( separator + "/" ).length(), "/" );
                                }
                            }
                            idx = 0;
                            while ( idx != -1 ) {
                                idx = line.find ( QString ( "/" + separator ), idx );
                                if ( idx != -1 ) {
                                    line.replace ( idx, QString ( "/" + separator ).length(), "/" );
                                }
                            }
                            //   QMessageBox::warning (0, "fine", line);
                        }

                        int mediaindex = line.find ( '/', 0 );
                        int pathindex = line.find ( separator, mediaindex + 1 );
                        fullpath = ( line.mid ( mediaindex, pathindex - mediaindex ) );
                        new_medianame = ( line.left ( mediaindex ) );


                        //   path = fullpath.mid(mediaindex + 1, medianame.length()+1);

                        int sizeindex = line.find ( separator, pathindex + 1 );
                        QString sizestring = line.mid ( pathindex + 1, sizeindex - pathindex - 1 ).stripWhiteSpace();
                        size = ( ( line.mid ( pathindex + 1, sizeindex - pathindex - 1 ) ).stripWhiteSpace() ).toFloat();

                        datetimestring = ( line.mid ( sizeindex + 1, line.length() ) ).stripWhiteSpace();

                        int dayindex = datetimestring.find ( "/" );
                        int monthindex = datetimestring.find ( "/", dayindex + 1 );
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
                        ;
                        int second = ( datetimestring.mid ( minuteindex + 1, datetimestring.length() - 1 ) ).toInt();
                        QDate date ( year, month, day );
                        QTime time ( hour, minute, second );

                        QDateTime datetime = QDateTime ( date, time );

                        int fileindex = fullpath.findRev ( '/' );

                        QString dirpath = "";
                        if ( fileindex != 0 )
                            dirpath = fullpath.mid ( 1, fileindex - 1 );
                        QString filename = fullpath.mid ( fileindex + 1, fullpath.length() - fileindex );

                        /*
                        //if (!validDate)
                        {
                        	QString msg;
                        	msg += "line: " + line + "\n";
                        	msg += "path: " + path + "\n";
                        	msg += "fullpath: " + fullpath + "\n";
                        	msg += "sizestring: "+ sizestring + "\n";
                        	msg += "sizeindex: " + QString().setNum(sizeindex) + "\n";
                        	msg += "size: " + QString().setNum(size) + "\n";
                        	msg += "day: " + QString().setNum(day) + "\n";
                        	msg += "month: " + QString().setNum(month) + "\n";
                        	msg += "hour: " + QString().setNum(hour) + "\n";
                        	msg += "minute: " + QString().setNum(minute) + "\n";
                        	msg += "second: " + QString().setNum(second) + "\n";
                        	msg += "year: " + QString().setNum(year) + "\n";
                        	msg += "dat_: " + day_ + "\n";
                        	msg += "datetimestring: " + datetimestring + "\n";
                        	msg += "new_medianame: " + new_medianame + "\n";
                        	msg+= "directory path:"+dirpath+"\n";
                        	msg+="file name: "+filename+"\n";

                        	QMessageBox::warning(0, "line", msg);
                        }
                        */
                        if ( medianame == "" )
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

int
importGtktalogCsv::addNewItem ( QString medianame, QString path,
                                QString filename, float size, QDateTime datetime ) {
    lineObject l ( medianame, path, filename, size, datetime );
    medialines->append ( l );

    // QMessageBox::critical( 0, "item", "new item!");
    return 0;

}

int importGtktalogCsv::addNewMedia ( QString new_medianame, QList < lineObject > *medialines ) {
    // QMessageBox::critical(0, "media", new_medianame);

    if ( guislave->mainw->db == NULL )
        guislave->newEvent();
    if ( guislave->mainw->db == NULL )
        return 0;

    /* create new media */
    Node *env, *curr;
    curr = db->getMediaNode ( new_medianame );
    if ( curr == NULL )
        curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, "" );

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
                    curr = db->putDirectoryNode ( env,  dir , obj->getDateTime() , "" );
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
        curr = db->getFileNode ( env, QString ( obj->getFileName() ) );
        if ( curr == NULL )
            curr = db->putFileNode ( env,  obj->getFileName() , obj->getDateTime() , "", st, s );

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
extern "C" {
    static void startElement_gtktalog_parse ( void * userData, const char * name, const char **atts ) {
        int * depthPtr = ( int * ) userData;
        QString name2 = QString::fromLatin1 ( name ).stripWhiteSpace();

        class_link->last_tag = name2;

        /*
        if (name2 == "information"){
        	// found information

        }
        */

        *depthPtr += 1;
    }

    static void endElement_gtktalog_parse ( void * userData, const char * name ) {
        DEBUG_INFO_ENABLED = init_debug_info();
        QString name2 = QString::fromLatin1 ( name ).stripWhiteSpace();

        class_link->last_tag = "";
        QString line = "";

        class_link->path = class_link->directory + "/" + class_link->filename;

        if ( name2 == "name" ) {
            line += "catalogName: " + class_link->catalogName + "\n";
        }
        if ( name2 == "report" ) {
            class_link->guislave->mainw->db->setName ( class_link->catalogName );
        }
        if ( name2 == "directory" ) {

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


        int * depthPtr = ( int * ) userData;
        *depthPtr -= 1;
    }

    static void getCdata_gtktalog_parse ( void * userData, const XML_Char * s, int len ) {

        QString tmp2 = "";
        for ( int i = 0;i < len;i++ ) {
            tmp2 += s[ i ];
        }

        QString name2 = tmp2.stripWhiteSpace();

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


    }

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

        /*

        XML_ParserCreate(ISO-8859-1);
        */

        //char buf[ BUFSIZ ];
        parser = XML_ParserCreate ( NULL );
        int done = 0;
        int depth = 0;
        XML_SetUserData ( parser, &depth );
        XML_SetElementHandler ( parser, startElement_gtktalog_parse, endElement_gtktalog_parse );
        XML_SetCharacterDataHandler ( class_link->parser, getCdata_gtktalog_parse );

        QString document_string = "";
        QByteArray array;
        if ( f.open ( QIODevice::ReadOnly ) ) {
            array = f.readAll();
        }
        int len = array.size();

        if ( len > 0 ) {
            if ( ! XML_Parse ( parser, ( char* ) array.data(), len, done ) ) {
                fprintf ( stderr,
                          "%s at line %d\n",
                          XML_ErrorString ( XML_GetErrorCode ( parser ) ),
                          XML_GetCurrentLineNumber ( parser ) );
                import_ok = false;
            }
            document_string = "";
        }


        XML_ParserFree ( parser );
        // end parse

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
        curr = db->putMediaNode ( new_medianame , mediacount, tr ( "importuser" ), CD, "" );

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

// ------------------ WhereIsIt classic ---------------------

importWhereIsItXml* class_link_whereisit;

QString tmpdirname_whereisit;
int lines;


extern "C" {
    static void startElement_whereisit_parse ( void * userData, const char * name, const char **atts ) {
        int * depthPtr = ( int * ) userData;
        QString name2 = QString::fromLocal8Bit ( name ).stripWhiteSpace();
        QString line = "";
        class_link_whereisit->last_tag = name2;


        if ( name2 == "REPORT" ) {
            if ( ( QString ( atts[ 0 ] ).stripWhiteSpace() ) == "Title" ) {
                class_link_whereisit->catalogName = ( QString ( atts[ 1 ] ).stripWhiteSpace() );
                line += "catalogName: \"" + class_link_whereisit->catalogName + "\"\n";
                class_link_whereisit->guislave->mainw->db->setName ( class_link_whereisit->catalogName );
                //		std::cout << line << endl;
            }
        }

        if ( name2 == "ITEM" ) {
            //std::cout << "Itemtype: " << QString ( atts[ 1 ] ) << endl;

            if ( ( QString ( atts[ 0 ] ).stripWhiteSpace() ) == "ItemType" ) {
                if ( ( QString ( atts[ 1 ] ).stripWhiteSpace() ) == "Disk" )
                    class_link_whereisit->last_type = media;

                if ( ( QString ( atts[ 1 ] ).stripWhiteSpace() ) == "File" )
                    class_link_whereisit->last_type = file;

                if ( ( QString ( atts[ 1 ] ).stripWhiteSpace() ) == "Folder" )
                    class_link_whereisit->last_type = folder;
            }
        }

        *depthPtr += 1;
    }

    static void endElement_whereisit_parse ( void * userData, const char * name ) {
        DEBUG_INFO_ENABLED = init_debug_info();

        QString name2 = QString::fromLocal8Bit ( name ).stripWhiteSpace();

        class_link_whereisit->last_tag = "";
        QString line = "";

        if ( name2 == "DATA" ) {
            if ( class_link_whereisit->last_type == media ) {
                QString line = "\"" + class_link_whereisit->new_medianame + "\"\n";
                //if(*DEBUG_INFO_ENABLED)
		//	std::cout << "media name found: " << line << endl;
            }

            if ( class_link_whereisit->last_type == folder ) {
                QString line = "\"" + class_link_whereisit->folder + "\"\n";
                //if(*DEBUG_INFO_ENABLED)
		//	std::cout << "folder name found: " << line << endl;
            }

            if ( class_link_whereisit->last_type == file ) {
                QString line = "\"" + class_link_whereisit->file + "\"\n";
                 //if(*DEBUG_INFO_ENABLED)
                //	std::cout << "file name found: " << line << endl;
            }
        }

        if ( name2 == "ITEM" ) {
            if ( class_link_whereisit->last_type == media ) {
                //			std::cout << "add media: " << class_link_whereisit->new_medianame << endl;
                //        QMessageBox::warning (0, "info", class_link_whereisit->medianame);
                class_link_whereisit->medianame = class_link_whereisit->new_medianame;
                //QDateTime datetime = class_link_whereisit->datetime;

                if ( class_link_whereisit->guislave->mainw->db == NULL ) {
                    class_link_whereisit->guislave->newEvent();
                    class_link_whereisit->guislave->mainw->db->setDBName ( class_link_whereisit->catalogName );
                }

                /* create new media */
                Node *env, *curr;

                env = class_link_whereisit->db->getRootNode();
                curr = class_link_whereisit->db->putMediaNode ( class_link_whereisit->new_medianame ,
                        class_link_whereisit->number, QObject::tr ( "importuser" ) , CD, class_link_whereisit->comment );

                class_link_whereisit->mediacount++;
                class_link_whereisit->linecount++;
                //class_link_whereisit->last_upper_container_node = class_link_whereisit->db->getRootNode();
                class_link_whereisit->last_type = empty;
                //			class_link_whereisit->guislave->mainw->app->processEvents();
            } // media

            if ( class_link_whereisit->last_type == folder ) {

                if ( class_link_whereisit->db != NULL ) {
                    //				std::cout << "add folder: " << class_link_whereisit->folder << endl;
                    Node * env2, *curr2, *curr3;
                    int number = class_link_whereisit->number;
                    QString folder = class_link_whereisit->folder;
                    QString path = class_link_whereisit->path;
                    QDateTime datetime = class_link_whereisit->datetime;
                    //	QString datetime = "";
                    QString comment = class_link_whereisit->comment;

                    env2 = class_link_whereisit->db->getMediaNode ( number ) ;


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
                                    curr3 = class_link_whereisit->db->getDirectoryNode ( curr3, tmp_path2 );

                                    if ( curr3 == NULL )
                                        curr3 = class_link_whereisit->db->putDirectoryNode ( curr2, tmp_path2 , datetime , class_link_whereisit->comment );
                                    class_link_whereisit->comment="";
                                }
                            }

                        }
                        curr3 = class_link_whereisit->db->putDirectoryNode ( curr3 , folder , datetime , class_link_whereisit->comment );
                        class_link_whereisit->comment="";


                    }
                }

                class_link_whereisit->dircount++;
                class_link_whereisit->linecount++;
                class_link_whereisit->last_type = empty;
                //			class_link_whereisit->guislave->mainw->app->processEvents();
            } // folder

            if ( class_link_whereisit->last_type == file ) {

                if ( class_link_whereisit->db != NULL ) {
                    //				std::cout << "add file: " << class_link_whereisit->file << endl;
                    Node * env2, *curr2, *curr3;
                    int number = class_link_whereisit->number;
                    QString file = class_link_whereisit->file;
                    QString path = class_link_whereisit->path;

                    QDateTime datetime = class_link_whereisit->datetime;
                    QString comment = class_link_whereisit->comment;

                    //QString datetime = "";


                    uint size = ( uint ) class_link_whereisit->size;
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

                    env2 = class_link_whereisit->db->getMediaNode ( number ) ;


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
                                    curr3 = class_link_whereisit->db->getDirectoryNode ( curr3,  tmp_path2 );

                                    if ( curr3 == NULL )
                                        curr3 = class_link_whereisit->db->putDirectoryNode ( curr2, tmp_path2 , datetime , comment );
                                    class_link_whereisit->comment="";
                                }
                            }

                        }

                        //			std::cout << "file: \"" << file << "\"" << endl;

                        curr3 = class_link_whereisit->db->putFileNode ( curr3 ,  file ,  datetime , class_link_whereisit->comment , st, s );
                        class_link_whereisit->comment="";


                    }
                }

                class_link_whereisit->filecount++;
                class_link_whereisit->linecount++;
                class_link_whereisit->last_type = empty;
                class_link_whereisit->setProgress();



            } // file

        } // ITEM



        int * depthPtr = ( int * ) userData;
        *depthPtr -= 1;

    }

    static void getCdata_whereisit_parse ( void * userData, const XML_Char * s, int len ) {

        //QString tmp2 = QCString( s, len + 1 );
        /*
        for ( int i = 0;i < len;i++ ) {
        	tmp2 += s[ i ];
        }
        */
        QString tmp2 = QString::fromUtf8 ( ( const char* ) ( s ), len );

        QString name2 = tmp2.stripWhiteSpace();

        if ( class_link_whereisit->last_tag == "ITEM" ) {
            // found ITEM entry: for each file there is one
            // just the start of each file
        }

        if ( class_link_whereisit->last_tag == "DISK_NUM" ) {
            class_link_whereisit->number = name2.toInt();
            //		std::cout << "number: " << QString().setNum( class_link_whereisit->number ) << endl;

        }

        if ( class_link_whereisit->last_tag == "SIZE" ) {
            if ( class_link_whereisit->last_type == file ) {
                name2 = /*name2.remove( ' ' );*/
                    name2.replace ( QRegExp ( " " ),"" );
                //				std::cout << "cleaned: " << name2 << endl;
                class_link_whereisit->size = name2.toInt();
            }
        }

        if ( class_link_whereisit->last_tag == "DATE" ) {

            name2.stripWhiteSpace();

            /*
            int dayindex = class_link_whereisit->datetimestring.find( "-" );
            int monthindex = class_link_whereisit->datetimestring.find( "-", dayindex + 1 );
            int yearindex = class_link_whereisit->datetimestring.length() - 1;
            */
            //			std::cout << "date: " << name2 << endl;
            int day = ( name2.mid ( 0, 2 ) ).toInt();
            //		std::cout << "day: " << day << endl;

            QString month_str = name2.mid ( 3, 3 ) ;
            //	std::cout << "month_str: " << month_str << endl;
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

            int year = ( name2.mid ( 7, 2 ) ).toInt();

            //		std::cout << "year : " << name2.mid( 7, 4 ) << endl;
            QDate date ( year, month, day );
            if ( !date.isValid() )
                date = QDate::currentDate ();

            class_link_whereisit->datetime.setDate ( date );
        }

        if ( class_link_whereisit->last_tag == "DISK_NAME" ) {
            if ( class_link_whereisit->last_type == media ) {}
        }

        if ( class_link_whereisit->last_tag == "DISK_TYPE" ) {
            if ( class_link_whereisit->last_type == media ) {}
        }

        if ( class_link_whereisit->last_tag == "PATH" ) {
            if ( class_link_whereisit->last_type == folder ) {
                //			std::cout << "getCdata_whereisit_parse(): PATH: \"" << name2 << "\"" << endl;
                class_link_whereisit->path = name2;
            }

            if ( class_link_whereisit->last_type == file ) {
                //			std::cout << "getCdata_whereisit_parse(): PATH: \"" << name2 << "\"" << endl;
                class_link_whereisit->path = name2;
            }

        }

        if ( class_link_whereisit->last_tag == "NAME" ) {
            if ( class_link_whereisit->last_type == media ) {
                //std::cout << "getCdata_whereisit_parse(): NAME: \"" << name2 << "\"" << endl;
                class_link_whereisit->new_medianame = name2;
            }

            if ( class_link_whereisit->last_type == folder ) {
                //			std::cout << "getCdata_whereisit_parse(): DATA: \"" << name2 << "\"" << endl;
                class_link_whereisit->folder = name2;
            }

            if ( class_link_whereisit->last_type == file ) {
                //std::cout << "getCdata_whereisit_parse(): DATA: \"" << name2 << "\"" << endl;
                class_link_whereisit->file = name2;
            }
        }

        if ( class_link_whereisit->last_tag == "TIME" ) {
            // found file time

            class_link_whereisit->datetimestring = name2.stripWhiteSpace();

            int hourindex = class_link_whereisit->datetimestring.find ( ":", 1 );
            int minuteindex = class_link_whereisit->datetimestring.find ( ":", hourindex + 1 );
            int hour = ( class_link_whereisit->datetimestring.mid ( 0, minuteindex - hourindex - 1 ) ).toInt();
            int minute = ( class_link_whereisit->datetimestring.mid ( hourindex + 1, minuteindex - 1 - hourindex ) ).toInt();
            int second = ( class_link_whereisit->datetimestring.mid ( minuteindex + 1, class_link_whereisit->datetimestring.length() - 1 ) ).toInt();


            QTime time ( hour, minute, second );


            class_link_whereisit->datetime.setTime ( time );

        }

        if ( class_link_whereisit->last_tag == "DESC" ) {
            // tmp
            name2.truncate ( 254 );

            if ( class_link_whereisit->last_type == media ) {
                class_link_whereisit->comment = name2;
            }

            if ( class_link_whereisit->last_type == folder ) {
                class_link_whereisit->comment = name2;
            }

            if ( class_link_whereisit->last_type == file ) {
                class_link_whereisit->comment = name2;
            }
        }

        class_link_whereisit->setProgress();

    } // parse

}



importWhereIsItXml::importWhereIsItXml ( GuiSlave * parent, QString filename, bool createdatabase ) {
    this->guislave = parent;
    bool import_ok = true;
    last_tag = "";
    last_type = empty;
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




        //XML_ParserCreate(ISO-8859-1);


        //char buf[ BUFSIZ ];
        parser = XML_ParserCreate ( "ISO-8859-1" );
        int done = 0;
        int depth = 0;
        XML_SetUserData ( parser, &depth );
        XML_SetElementHandler ( parser, startElement_whereisit_parse, endElement_whereisit_parse );
        XML_SetCharacterDataHandler ( class_link_whereisit->parser, getCdata_whereisit_parse );

        QString document_string = "";
        QByteArray array;
        if ( f.open ( QIODevice::ReadOnly ) ) { // old
            //	if ( f_tmp.open( IO_ReadOnly ) ) {
// 			int lines = 0;
            Q3ProgressDialog progress ( 0, "Progressdialog", true );
            progress.setLabelText ( tr ( "Importing XML..." ) );
            progress.setCancelButton ( 0 );
            progress.setTotalSteps ( all_lines );
            progress.show();
            array = f.readAll(); // old
            /*
            QTextStream t( &f );	// use a text stream
            while ( !t.eof() ) {
            document_string.append(t.readLine());
            document_string.append('\n');
            guislave->mainw->app->processEvents();
            lines++;
            progress.setProgress(lines);

            }
            */
            //	array = f_tmp.readAll();
        }
        /*std::cout << "file read." << endl;*/

        int len = array.size();

        document_string = QString ( array );
        /*std::cout << "string created." << endl;*/
        //document_string.stripWhiteSpace();
        //std::cout << "string stripped." << endl;
        for ( int i = 1;i < 32;i++ ) {
            if ( i != 10 )
                /*document_string.remove( QChar( i ) );*/
                document_string.replace ( QRegExp ( QString ( QChar ( i ) ) ),"" );
            /*std::cout << "remove char: " << QString().setNum( i ) << endl;*/
        }


        /*
        	^B ==> ACSII 02


        */
        /*
        		int fails = 0;
        		for ( int x = 1; x < 26;x++ ) {
        			if ( x != 10 ) {
        				int idx = array.find( x );
        				while ( idx > 0 ) {
        					array[ idx ] = ' ';
        					fails++;
        					idx = array.find( x, idx + 1 );
        				}
        			}
        		}
        */




        if ( len > 0 ) {
            if ( ! XML_Parse ( parser, document_string.latin1(), document_string.length(), done ) ) {
                //		if ( ! XML_Parse( parser, ( char * ) array.data(), len, done ) ) {
                fprintf ( stderr,
                          "%s at line %d\n",
                          XML_ErrorString ( XML_GetErrorCode ( parser ) ),
                          XML_GetCurrentLineNumber ( parser ) );
                import_ok = false;
            }
            //			document_string = "";
        }


        XML_ParserFree ( parser );
        // end parse


        //		f_tmp.close();
        //	f_tmp.remove();

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
    int line = XML_GetCurrentLineNumber ( parser );
    progress->setProgress ( line );
    class_link_whereisit->guislave->mainw->app->processEvents();
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
        else {
            //		cerr << "wrong type!!!" << endl;
        }
    }
}
