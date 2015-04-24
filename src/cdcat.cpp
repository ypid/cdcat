/****************************************************************************
*                      Hyper's CD Catalog
*  A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "cdcat.h"

#include "dbase.h"
#include "mainwidget.h"
#include "icons.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QApplication>
#include <QFont>
#include <QMessageBox>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QTextCodec>
#include <QDebug>

/* Own debug output {{{ */
bool *init_debug_info() {
    if (DEBUG_INFO_ENABLED == NULL) {
        DEBUG_INFO_ENABLED = new bool();
    }
    return DEBUG_INFO_ENABLED;
}

#ifndef QT_NO_DEBUG
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}
#endif
/* }}} */

int main( int argi, char **argc ) {

#ifndef QT_NO_DEBUG
    #warning ====> installing own message handler
    qInstallMessageHandler( myMessageOutput );
#endif

    QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ));

    QApplication app( argi, argc );
    QCoreApplication::setApplicationName(PROGRAM_NAME);
    QCoreApplication::setApplicationVersion(VERSION);

    /* Declare command line parameters {{{ */
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("db_filename",
        QCoreApplication::translate("main_cli", "Optional database file, if given, it will be used as database.")
    );

    /* Export database to other format {{{ */
    QCommandLineOption optionImportFile(QStringList() << "i" << "import-file",
        QCoreApplication::translate("main_cli", "Import %1 in the format specified by the \"%2\" parameter into the database."
            " Use the \"%3\" parameter to get a list of all supported import formats.")
                .arg("<" + QCoreApplication::translate("seekEngine", "file") + ">")
                .arg("import-format")
        ,
        QCoreApplication::translate("seekEngine", "file")
    );
    Q_ASSERT(parser.addOption(optionImportFile));

    QCommandLineOption optionImportFormat(QStringList() << "f" << "import-format",
        QCoreApplication::translate("main_cli", "Format used to by the \"%1\" parameter."
            " Use the \"%2\" parameter to get a list of all supported import formats."
            " Default is \"%3\".")
                .arg("<" + QCoreApplication::translate("seekEngine", "file") + ">")
                .arg("import-list")
                .arg("Whereisit")
        ,
        QCoreApplication::translate("main_cli", "format"),
        "Whereisit"
    );
    Q_ASSERT(parser.addOption(optionImportFormat));

    QCommandLineOption optionImportList(QStringList() << "l" << "import-list",
        QCoreApplication::translate("main_cli", "Print a list of all supported import formats and exit.")
    );
    Q_ASSERT(parser.addOption(optionImportList));
    /* }}} */

    /* Export database to other format {{{ */
    QCommandLineOption optionExportFile(QStringList() << "e" << "export-file",
        QCoreApplication::translate("main_cli", "Export to the contents of the database to %1 in the format specified by the \"%2\" parameter.")
            .arg("<" + QCoreApplication::translate("seekEngine", "file") + ">")
            .arg("export-format")
        ,
        QCoreApplication::translate("seekEngine", "file")
    );
    Q_ASSERT(parser.addOption(optionExportFile));

    QCommandLineOption optionExportFormat(QStringList() << "t" << "export-format",
        QCoreApplication::translate("main_cli", "Format used to by the \"%1\" parameter."
            " Use the \"%2\" parameter to get a list of all supported export formats."
            " Default is \"%3\".")
                .arg("<" + QCoreApplication::translate("seekEngine", "file") + ">")
                .arg("export-list")
                .arg("Whereisit")
        ,
        QCoreApplication::translate("main_cli", "format"),
        "Whereisit"
    );
    Q_ASSERT(parser.addOption(optionExportFormat));
    /* }}} */

    QCommandLineOption optionConfigFile(QStringList() << "c" << "config-file",
        QCoreApplication::translate("main_cli", "Configuration file to use."),
        QCoreApplication::translate("main_cli", "file")
    );
    Q_ASSERT(parser.addOption(optionConfigFile));

    QCommandLineOption optionSwitchBatchMode(QStringList() << "b" << "batch",
        QCoreApplication::translate("main_cli", "Run in non iterative batch mode. This mode is CLI only which means no GUI is going to appear.")
    );
    Q_ASSERT(parser.addOption(optionSwitchBatchMode));

    QCommandLineOption optionExportList(QStringList() << "L" << "export-list",
        QCoreApplication::translate("main_cli", "Print a list of all supported export formats and exit.")
    );
    Q_ASSERT(parser.addOption(optionExportList));
    /* }}} */

    /* Evaluate command line parameters {{{ */
    parser.process(app);

    const QStringList args = parser.positionalArguments();

    QString address(parser.value(optionImportFile));
    // Display values of arguments and options.
    // qDebug() << "filename:  " << args.at(0);
    // qDebug() << "no list:   " << parser.isSet(noListOption);
    qDebug() << "address:   " << address;
    qDebug() << "format:    " << args.size();

    /* Evaluate command line parameters: Error checking {{{ */
    if (args.size() > 1) {
        fprintf(stderr, "%s\n", qPrintable(
            QCoreApplication::translate("main_cli", "Error: You can only specify one %1.")
                .arg("\"db_filename\"" + args.size() )
            )
        );
        parser.showHelp(1);
    }
    /* }}} */

    /* }}} */

    CdCatConfig *cconfig;
    translator = 0;
    int font_size = 8;

    if (parser.isSet(optionConfigFile)) {
        cconfig = new CdCatConfig(parser.value(optionConfigFile));
    } else {
        cconfig = new CdCatConfig(NULL);
    }

    if (args.size() == 1) {
        QByteArray filename_a = args.at(0).toUtf8();
        char* filename = filename_a.data();
        qDebug() << "filename:" << filename;
        cconfig->setParameter( filename );
    }

    if (cconfig->readConfig() == 0) {
        font_size = cconfig->fsize;
    } else {
        cconfig->writeConfig();
    }

    // FIXME remove
    DEBUG_INFO_ENABLED = init_debug_info();
    *DEBUG_INFO_ENABLED = cconfig->debug_info_enabled;
    qDebug() << "DEBUG_INFO_ENABLED:" << QString(*DEBUG_INFO_ENABLED ? "true" : "false");

    /* Internationalization {{{ */
#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    QString langpath( applicationDirPath( argc ) + "/lang/cdcat_" );
    langpath += cconfig->lang;
    langpath += ".qm";
#endif

#ifndef _WIN32
#if !defined(Q_OS_MAC) && !defined(_OS2)
    QList<QString> translation_paths;
    // translation_paths = new QList <QString> ();
    QString locale = QLocale().name();
    QString locale2 = locale.left( 2 );
    QString prefix = applicationDirPath( argc ).left( applicationDirPath( argc ).length() - 4 ) + "/";        // /usr/local/bin -> /usr/local
    translation_paths.append( QString( prefix + "share/cdcat/translations" ));
    translation_paths.append( QString( applicationDirPath( argc ) + "/lang" ));
    translation_paths.append( QString( prefix + "share/locale/" + locale + "/LC_MESSAGES" ));
    translation_paths.append( QString( prefix + "share/locale/" + locale2 + "/LC_MESSAGES" ));

    QString langpath;

    qDebug() << "locale:" << locale;
    for (int i = 0; i < translation_paths.count(); ++i) {
        QFileInfo info( translation_paths.at( i ) + "/cdcat_" + locale + ".qm" );
        if (info.exists()) {
            langpath = translation_paths.at( i ) + "/cdcat_" + locale + ".qm";
        } else {
            QFileInfo info2( translation_paths.at( i ) + "/cdcat_" + locale2 + ".qm" );
            if (info2.exists()) {
                langpath = translation_paths.at( i ) + "/cdcat_" + locale2 + ".qm";
            }
        }
    }
#endif
#endif

    if (translator) {
        app.removeTranslator( translator );
        delete translator;
    }

    translator = new QTranslator( 0 );

    if (!langpath.isEmpty()) {
        qDebug() << "load language:" << langpath;
        translator->load( langpath, "." );
        app.installTranslator( translator );
    }
    /* }}} */

    int ret_val = 2;
    if (parser.isSet(optionSwitchBatchMode)) {
    } else if (parser.isSet(optionImportList)) {
    } else if (parser.isSet(optionExportList)) {
    } else {
        /* Create GUI {{{ */
        init_icon_base();

        CdCatMainWidget mw( cconfig, &app, 0, "MainWindow" );

        cconfig->defaultfont = app.font();
        if (cconfig->ownfont) {
            QFont font;
            font.setPointSize( font_size );
            app.setFont( font );
        }

        mw.show();

        ret_val = app.exec();
        deinit_icon_base();
        /* }}} */
    }
    delete cconfig;

    return ret_val;
}

char *mstr( const char *imp ) {
    char *x;

    if (imp == NULL) {
        return NULL;
    }
    x = new char[strlen( imp ) + 1];
    strcpy( x, imp );
    return x;
}

QString applicationFilePath( char **argc ) {
#if defined(_WIN32) || defined(_OS2)
    return QDir::cleanPath( QFile::decodeName( "cdcat" ));
#else

    QString absPath;
    QString tmp( argc[0] );
    if (tmp.startsWith( "/" )) {
        /*
         * If argc[0] starts with a slash, it is already an absolute
         * file path.
         */
        absPath = tmp;
    } else if (tmp.indexOf( '/' ) != -1) {
        /*
         * If argc[0] contains one or more slashes, it is a file path
         * relative to the current directory.
         */
        absPath = QDir::current().absoluteFilePath( tmp );
    } else {
        /*
         * Otherwise, the file path has to be determined using the
         * PATH environment variable.
         */
        char *pEnv = getenv( "PATH" );
        QStringList paths = QString( pEnv ).split( QChar( ':' ));
        QStringList::const_iterator p = paths.begin();
        while (p != paths.end()) {
            QString candidate = QDir::current().absoluteFilePath( *p + "/" + tmp );
            if (QFile::exists( candidate )) {
                absPath = candidate;
                break;
            }
            ++p;
        }
    }

    absPath = QDir::cleanPath( absPath );
    if (QFile::exists( absPath )) {
        //		return resolveSymlinks( absPath );
        return absPath;
    } else {
        return QString::null;
    }
#endif
}

QString applicationDirPath( char **argc ) {
    return QFileInfo( applicationFilePath( argc )).path();
}
