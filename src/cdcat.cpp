/****************************************************************************
                       Hyper's CD Catalog
A multiplatform qt and xml based catalog program

Author    : Peter Deak (hyperr@freemail.hu)
License   : GPL
Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <qtranslator.h>
#include <qtextcodec.h>
#include <qfont.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qdir.h>
//Added by qt3to4:
#include <Q3PtrList>

#include "dbase.h"
#include "cdcat.h"
#include "mainwidget.h"
#include "icons.h"

#include <qfileinfo.h>
#include <iostream>

using namespace std;

bool*  init_debug_info() {
	if (DEBUG_INFO_ENABLED == NULL)
		DEBUG_INFO_ENABLED = new bool();
	return DEBUG_INFO_ENABLED;
}

int main ( int argi, char **argc ) {
    QApplication app ( argi, argc );
    CdCatConfig *cconfig = new CdCatConfig();
    QTranslator *translator = 0;
    int font_size = 8;

    if ( argi > 1 )
        cconfig->setParameter ( argc[1] );

    if ( cconfig->readConfig() == 0 )
        font_size = cconfig->fsize;
    else
        cconfig->writeConfig();

    DEBUG_INFO_ENABLED = init_debug_info();
    *DEBUG_INFO_ENABLED = cconfig->debug_info_enabled;
    if (*DEBUG_INFO_ENABLED)
	cerr << qPrintable(QString("DEBUG_INFO_ENABLED: true")) << endl;
    else
	cerr << qPrintable(QString("DEBUG_INFO_ENABLED: false")) << endl;

    QFont *font = new QFont();
    font->setPointSize ( font_size );


#ifdef _WIN32
    QString langpath ( applicationDirPath ( argc ) + "/lang/cdcat_" );
    langpath += cconfig->lang;
    langpath += ".qm";

#else

    Q3PtrList<QString> translation_paths;
    //translation_paths = new QPtrList <QString> ();
    translation_paths.setAutoDelete ( TRUE );		// the list owns the objects
    QString locale = QTextCodec::locale();
    QString locale2 = locale.left ( 2 );
    QString prefix = applicationDirPath ( argc ).left ( applicationDirPath ( argc ).length() - 4 ) + "/"; // /usr/local/bin -> /usr/local
    translation_paths.append ( new QString ( prefix + "share/cdcat/translations" ) );
    translation_paths.append ( new QString ( applicationDirPath ( argc ) + "/lang" ) );
    translation_paths.append ( new QString ( prefix + "share/locale/" + locale + "/LC_MESSAGES" ) );
    translation_paths.append ( new QString ( prefix + "share/locale/" + locale2 + "/LC_MESSAGES" ) );

    QString langpath;

    for ( uint i = 0; i < translation_paths.count(); ++i ) {
        //cerr <<"path: " << *translation_paths.at(i) << endl;
        QFileInfo info ( *translation_paths.at ( i ) + "/cdcat_" + locale + ".qm" );
        if ( info.exists() ) {
            // cerr << "file " + *translation_paths.at( i ) + "/cdcat_" + locale + ".qm" + " does exist!" << endl;
            langpath = *translation_paths.at ( i ) + "/cdcat_" + locale + ".qm";
        } else {
            // cerr << "file " + *translation_paths.at( i ) + "/cdcat_" + locale + ".qm" + " does NOT exist!" << endl;
            QFileInfo info2 ( *translation_paths.at ( i ) + "/cdcat_" + locale2 + ".qm" );
            if ( info2.exists() ) {
                // cerr << "file " + *translation_paths.at( i ) + "/cdcat_" + locale2 + ".qm" + " does exist!" << endl;
                langpath = *translation_paths.at ( i ) + "/cdcat_" + locale2 + ".qm";
            }
        }

    }

#endif

    if ( translator ) {
        app.removeTranslator ( translator );
        delete translator;
    }

    translator = new QTranslator ( 0 );

    if ( !langpath.isEmpty() ) {
        //cerr << "using language file " << langpath << endl;
        translator->load ( langpath, "." );
        app.installTranslator ( translator );
    }

    init_icon_base();

    CdCatMainWidget *mw = new CdCatMainWidget ( cconfig, &app, 0, "MainWindow" );

    cconfig->defaultfont = new QFont ( app.font() );
    if ( cconfig->ownfont )
        app.setFont ( *font, TRUE );

    app.setMainWidget ( mw );

    mw->show();
    return app.exec();
}

char *mstr ( const char *imp ) {
    char * x;
    if ( imp == NULL )
        return NULL;
    x = new char[ strlen ( imp ) + 1 ];
    strcpy ( x, imp );
    return x;
}

QString applicationFilePath ( char **argc ) {
#ifdef Q_WS_WIN
    return QDir::cleanDirPath ( QFile::decodeName ( "cdcat" ) );
#else

    QString absPath;
    QString tmp ( argc[ 0 ] );
    if ( tmp.startsWith ( "/" ) ) {
        /*
        If argv0 starts with a slash, it is already an absolute
        file path.
        */
        absPath = tmp;
    } else if ( tmp.find ( '/' ) != -1 ) {
        /*
        If argv0 contains one or more slashes, it is a file path
        relative to the current directory.
        */
        absPath = QDir::current().absFilePath ( tmp );
    } else {
        /*
        Otherwise, the file path has to be determined using the
        PATH environment variable.
        */
        char *pEnv = getenv ( "PATH" );
        QStringList paths ( QStringList::split ( QChar ( ':' ), pEnv ) );
        QStringList::const_iterator p = paths.begin();
        while ( p != paths.end() ) {
            QString candidate = QDir::current().absFilePath ( *p + "/" + tmp );
            if ( QFile::exists ( candidate ) ) {
                absPath = candidate;
                break;
            }
            ++p;
        }
    }

    absPath = QDir::cleanDirPath ( absPath );
    if ( QFile::exists ( absPath ) ) {
        //		return resolveSymlinks( absPath );
        return absPath;
    } else {
        return QString::null;
    }
#endif
}

QString applicationDirPath ( char **argc ) {
    return QFileInfo ( applicationFilePath ( argc ) ).dirPath();
}

