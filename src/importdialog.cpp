/****************************************************************************
*                           Hyper's CD Catalog
*       A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke <crissi99@gmx.de>
*  License   : GPL
*  Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/


#include "importdialog.h"

#include <QLabel>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QToolTip>
#include <QFile>
#include <qfiledialog.h>
#include <QGridLayout>
#include <stdio.h>
#include <stdlib.h>
#include <QVariant>
#include <QLocale>
#include "icons.h"

QList<QList<QString> > getImportPatternList() {
    /* extract special cdcat pattern
     *      defined patterns:
     *      %dirseparator%		directory separtor (unix: /, windows: \)
     *      %filename%		filename
     *      %fullpath%		full path (<dirpath><divider><file>)
     *      %sizehuman%		size (1234,56[<unit>])
     *                                      unit: ((empty) => B, k => kB, K => KiB, m => mb, M => MiB, g = gb, G => GB, t = tb, T => TiB)
     *      %sizeb%			size in bytes (123456)
     *      %sizekb%			size in kilobytes (123456)
     *      %YYMMDD%		date (2 letter year)
     *      %YYYYMMDD%	date (4 letter year)
     *
     *
     *      list elems: <pattern label><matching regex pattern><description>
     */
    QList<QList<QString> > regex_pattern_list;
    QList<QString> tmplist;
    tmplist.append( QString( "%dirseparator%" ));
    tmplist.append( QString( QDir::separator()));
    tmplist.append( QObject::tr( "directory separator" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%tab%" ));
    tmplist.append( QString( "	"));
    tmplist.append( QObject::tr( "tabulator" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%space%" ));
    tmplist.append( QString( "\\ " ));
    tmplist.append( QObject::tr( "space char" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%filename%" ));
    tmplist.append( QString( "[0-9a-zA-Z\\_\\-\\.\\+\\=\ \\~]+" ));
    tmplist.append( QObject::tr( "file name" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%fullpath%" ));
    tmplist.append( QString( "(([\\/\\" ) + QDir::separator() + "]|)([0-9a-zA-Z\\_\\-\\.\\+\\=\ \\~]+))+" );
    tmplist.append( QObject::tr( "full path (including directory name and file name)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%size_human%" ));
    tmplist.append( QString( "[0-9]+([\\,\\.0-9])*([kKmMgGtT]|)" ));
    tmplist.append( QObject::tr( "File size with human unit (e.g. G for Gigabyte)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%sizeb%" ));
    tmplist.append( QString( "([0-9])+" ));
    tmplist.append( QObject::tr( "File size (number only, in bytes)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%sizekb%" ));
    tmplist.append( QString( "([0-9])+" ));
    tmplist.append( QObject::tr( "File size (number only, in kilobytes)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%dateyear2%" ));
    tmplist.append( QString( "[0-9][0-9]\-([0-9]|[0-9][0-9])\\-([0-9]|[0-9][0-9])" ));
    tmplist.append( QObject::tr( "date with 2 year digit (e.g. 25th. may 2012: 12-05-25)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%dateyear4%" ));
    tmplist.append( QString( "[0-9][0-9][0-9][0-9]\\-([0-9]|[0-9][0-9])\\-([0-9]|[0-9][0-9])" ));
    tmplist.append( QObject::tr( "date with 4 year digit (e.g. 25th. may 2012: 2012-05-25)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%time_full%" ));
    tmplist.append( QString( "([0-9]|[0-9][0-9])\\:([0-9]|[0-9][0-9])\\:([0-9]|[0-9][0-9])" ));
    tmplist.append( QObject::tr( "time in full format (e.g. 07:28:13)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();

    tmplist.append( QString( "%time_hm%" ));
    tmplist.append( QString( "([0-9]|[0-9][0-9])\\:([0-9]|[0-9][0-9])" ));
    tmplist.append( QObject::tr( "time with hour and minute only (e.g. 07:28)" ));
    regex_pattern_list.append( tmplist );
    tmplist.clear();
    return regex_pattern_list;
}

ImportDialog::ImportDialog ( QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    if (!name) {
        setObjectName( "ImportDialog" );
    }
    setModal( modal );
    //	ImportDialogLayout = new QGridLayout( this, 1, 1, 11, 7, "ImportDialogLayout" );

    setSizeGripEnabled( true );

    layout4 = new QGridLayout( this );

    filename_lab = new QLabel( this );
    layout4->addWidget( filename_lab, 2, 0, 1, 1 );

    filename_lineedit = new QLineEdit( this );
    layout4->addWidget( filename_lineedit, 2, 1, 1, 4 );

    buttonGetFile = new QPushButton( this );
    layout4->addWidget( buttonGetFile, 2, 5, 1, 2 );

    info_lab = new QLabel( this );
    info_lab->setText( "info" );
    layout4->addWidget( info_lab, 0, 0, 1, 5 );

    importButtonBox = new QGroupBox( tr( "Type" ), this );
    layoutGroupBox = new QVBoxLayout( this );
    importButtonBox->setLayout( layoutGroupBox );
    importTypeCsvGtktalog = new QRadioButton( tr( "&Gtktalog CSV" ), importButtonBox );
    importTypeCsvKatCeDe = new QRadioButton( tr( "&Kat-DeCe CSV" ), importButtonBox );
    importTypeCsvDisclib = new QRadioButton( tr( "&Disclib CSV" ), importButtonBox );
    importTypeCsvVisualcd = new QRadioButton( tr( "&VisualCD CSV" ), importButtonBox );
    importTypeCsvVvv = new QRadioButton( tr( "&VVV CSV" ), importButtonBox );
    importTypeCsvAdvancedFileOrganizer = new QRadioButton( tr( "&Advanced file organizer CSV" ), importButtonBox );
    importTypeCsvFileArchivist = new QRadioButton( tr( "&File Archivist CSV" ), importButtonBox );
    importTypeCsvAdvancedDiskCatalog = new QRadioButton( tr( "&Advanced Disk Catalog CSV" ), importButtonBox );
    importTypeCsvWhereisit = new QRadioButton( tr( "&Advanced Disk Catalog CSV" ), importButtonBox );
    importTypeGtktalogXml = new QRadioButton( tr( "Gtktalog &XML" ), importButtonBox );
    importTypeWhereisitXml = new QRadioButton( tr( "&WhereIsIt XML (classic)" ), importButtonBox );
    importTypeGenericRegex = new QRadioButton( tr( "&" ), importButtonBox );

    layoutGroupBox->addWidget( importTypeCsvGtktalog );
    layoutGroupBox->addWidget( importTypeCsvKatCeDe );
    layoutGroupBox->addWidget( importTypeCsvDisclib );
    layoutGroupBox->addWidget( importTypeCsvVisualcd );
    layoutGroupBox->addWidget( importTypeCsvVvv );
    layoutGroupBox->addWidget( importTypeCsvAdvancedFileOrganizer );
    layoutGroupBox->addWidget( importTypeCsvFileArchivist );
    layoutGroupBox->addWidget( importTypeCsvAdvancedDiskCatalog );
    layoutGroupBox->addWidget( importTypeCsvWhereisit );
    layoutGroupBox->addWidget( importTypeGtktalogXml );
    layoutGroupBox->addWidget( importTypeWhereisitXml );
    layoutGroupBox->addWidget( importTypeGenericRegex );

    layout4->addWidget( importButtonBox, 3, 0, 1, 4 );

    newdatabase = new QCheckBox( this );
    newdatabase->setText( tr( "Create new Database" ));
    layout4->addWidget( newdatabase, 4, 0, 1, 2 );




    correctbadstyle = new QCheckBox( this );
    correctbadstyle->setText( tr( "Correct bad style from gtktalog export" ));
    layout4->addWidget( correctbadstyle, 4, 2, 1, 2 );


    separator_lab = new QLabel( this );
    layout4->addWidget( separator_lab, 5, 0, 1, 2 );
    separator_lineedit = new QLineEdit( this );
    separator_lineedit->setMinimumSize( QSize( 0, 0 ));
    separator_lineedit->setMaximumSize( QSize( 20, 32767 ));
    separator_lineedit->setMaxLength( 1 );
    layout4->addWidget( separator_lineedit, 5, 2, 1, 4 );

    layoutGenericRegex = new QHBoxLayout( this );
    genericRegexPattern_lab = new QLabel( this );
    genericRegex_lineedit = new QLineEdit( this );
    genericRegexInfo = new QLabel( this );
    genericRegexInfo->setPixmap( *get_t_info_icon());
    layoutGenericRegex->addWidget( genericRegexPattern_lab );
    layoutGenericRegex->addWidget( genericRegex_lineedit );
    layoutGenericRegex->addWidget( genericRegexInfo );

    layout4->addLayout( layoutGenericRegex, 6, 0, 1, 4 );

    buttonOK = new QPushButton( this );
    buttonOK->setDefault( true );
    buttonOK->setMinimumWidth( 100 );
    layout4->addWidget( buttonOK, 8, 1, 1, 1 );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumWidth( 100 );
    layout4->addWidget( buttonCancel, 8, 3, 1, 1 );
    /*
     *      QSpacerItem* spacer = new QSpacerItem( 181, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
     *      layout4->addMultiCell( spacer, 5, 5, 0, 2 );
     *
     *      QSpacerItem* spacer_2 = new QSpacerItem( 291, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
     *      layout4->addMultiCell( spacer_2, 3, 3, 2, 5 );
     */
    // importTypeWhereisitXml->setChecked ( true );
    importTypeGenericRegex->setChecked( true );


    correctbadstyle->setEnabled( false );
    separator_lab->setEnabled( false );
    separator_lineedit->setEnabled( false );


    //	ImportDialogLayout->addLayout( layout4, 0, 0 );
    languageChange();
    resize( QSize( 500, 350 ).expandedTo( minimumSizeHint()));

    setMinimumSize( minimumSizeHint());

    // this->sizeHint();
    // setFixedSize( size() );

    // clearWState( WState_Polished );

    connect( buttonOK, SIGNAL( clicked()), this, SLOT( bOk()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( bCan()));
    connect( buttonGetFile, SIGNAL( clicked()), this, SLOT( getFileName()));
    connect( importTypeCsvGtktalog, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvKatCeDe, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvDisclib, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvVisualcd, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvVvv, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvAdvancedFileOrganizer, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvFileArchivist, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvAdvancedDiskCatalog, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeCsvWhereisit, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeGtktalogXml, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeWhereisitXml, SIGNAL( clicked()), this, SLOT( typeChanged()));
    connect( importTypeGenericRegex, SIGNAL( clicked()), this, SLOT( typeChanged()));
    separator_lab->setEnabled( false );
    separator_lineedit->setEnabled( false );



    // tmp
    /*
     * importTypeCsv->setChecked(true);
     * separator_lab->setEnabled(true);
     * separator_lineedit->setEnabled(true);
     * //filename_lineedit->setText("c:\\devel\\mp3s.csv");
     * filename_lineedit->setText("/data3/res3/musikvideo.csv");
     * separator_lineedit->setText("*");
     * correctbadstyle->setEnabled(true);
     * correctbadstyle->setChecked(true);
     */
    // tmp
    // filename_lineedit->setText("/home/crissi/compile/cvs/CdCat-0.98pre_whereisit_xml_import/sample-export.xml");
    filename_lineedit->setText( "" );
    // filename_lineedit->setText("/home/crissi/work/cdcat/features/import/generic_regex_du_human.txt");
    // filename_lineedit->setText("/home/crissi/work/cdcat/features/import/generic_du_all_cdcat_dir.txt");
}

/*
 *  Destroys the object and frees any allocated resources
 */
ImportDialog::~ImportDialog() {
    // no need to delete child widgets, Qt does it for us!
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ImportDialog::languageChange() {
    setWindowTitle( tr( "Import CSV file" ));
    filename_lab->setText( tr( "File:" ));

    separator_lineedit->setText( ";" );
    separator_lab->setText( tr( "Separator:" ));
    separator_lineedit->setToolTip( tr( "This is the separator in dataline\n<path>SEPARATOR<size>SEPARATOR<date><space><time>" ));
    buttonCancel->setText( tr( "Cancel" ));
    buttonOK->setText( tr( "Import" ));
    buttonGetFile->setText( tr( "..." ));

    importButtonBox->setTitle( tr( "Type" ));
    importTypeCsvGtktalog->setText( tr( "&Gtktalog (csv)" ));
    importTypeCsvKatCeDe->setText( tr( "&Kat-CeDe (csv)" ));
    importTypeCsvDisclib->setText( tr( "&Disclib (csv)" ));
    importTypeCsvVisualcd->setText( tr( "&VisualCD (csv)" ));
    importTypeCsvVvv->setText( tr( "&VVV (csv)" ));
    importTypeCsvAdvancedFileOrganizer->setText( tr( "&Advanced File Organizer (csv)" ));
    importTypeCsvFileArchivist->setText( tr( "&File Archivist" ));
    importTypeCsvAdvancedDiskCatalog->setText( tr( "&Advanced Disk Catalog (csv)" ));
    importTypeCsvWhereisit->setText( tr( "W&hereIsIt (csv)" ));
    importTypeGtktalogXml->setText( tr( "Gtktalog &XML" ));
    importTypeWhereisitXml->setText( tr( "&WhereIsIt XML (classic)" ));
    importTypeGenericRegex->setText( tr( "Generic &pattern" ));

    importButtonBox->setToolTip( tr( "Select the type of import here" ));

    importTypeCsvGtktalog->setToolTip( tr( "Select this for importing a text import (csv) generated from Gtktalog" ));

    importTypeCsvKatCeDe->setToolTip( tr( "Select this for importing a text import (csv) generated from Kat-CeDe." ));

    importTypeCsvDisclib->setToolTip( tr( "Select this for importing a text import (csv) generated from Disclib." ));

    importTypeCsvVisualcd->setToolTip( tr( "Select this for importing a text import (csv) generated from VisualCD." ));

    importTypeCsvVvv->setToolTip( tr( "Select this for importing a text import (csv) generated from VVV." ));

    importTypeCsvAdvancedFileOrganizer->setToolTip( tr( "Select this for importing a text import (csv) generated from Advanced File Organizer." ));

    importTypeCsvFileArchivist->setToolTip( tr( "Select this for importing a File Archivist catalog." ));

    importTypeCsvAdvancedDiskCatalog->setToolTip( tr( "Select this for importing a text import (csv) generated from Advanced Disk Catalog." ));

    importTypeCsvWhereisit->setToolTip( tr( "Select this for importing a text import (csv) generated from WhereIsIt." ));

    importTypeGtktalogXml->setToolTip( tr( "Select this for importing a xml report generated from gtktalog" ));

    importTypeWhereisitXml->setToolTip( tr( "Select this for importing a xml report generated from WhereIsIt?" ));

    importTypeGenericRegex->setToolTip( tr( "Select this for importing a generic report (e.g. generated by <ul><li>Linux: <i>du --all --human-readable --time</i></li><li>rhash <i>rhash -r</i></li><li>Windows: <i>dir /S /B command output</i></li>" ));

    buttonGetFile->setToolTip( tr( "Open the file dialog for selecting file to import." ));

    correctbadstyle->setToolTip( tr( "Corrects bad output style from gtktalog.\n<media>SEPARATOR/<dir>/SEPARATOR/<dir>\n will be to\n<media>/<dir>/<dir>" ));
    info_lab->setText( tr( "<strong>Please read the README_IMPORT before you import!</strong>" ));

    genericRegexPattern_lab->setText( tr( "Pattern:" ));
    genericRegex_lineedit->setToolTip( tr( "You can use pattern for data fields, hold mouse cursor over info symbol for info" ));
    genericRegexPattern_lab->setToolTip( tr( "Pattern for data fields, hold mouse cursor over info symbol for info" ));
    QString genericRegexTipText = "<h1>" + tr( "Pattern for data fields" ) + "</h1>";
    genericRegexTipText += tr( "Valid pattern for data fields are:" );
    genericRegexTipText += "<table>";
    genericRegexTipText += "<tr><th>" + tr( "Pattern" ) + "</th><th>" + tr( "Description" ) + "</th></tr>";
    QList<QList<QString> > regex_pattern_list = getImportPatternList();
    for (int i = 0; i < regex_pattern_list.size(); ++i)
        genericRegexTipText += "<tr><td>" + regex_pattern_list.at( i ).at( 0 ) + "</td><td>" + regex_pattern_list.at( i ).at( 2 ) + "</td></tr>";
    genericRegexTipText += "<tr><th></th><th></th></tr>";

    genericRegexTipText += "</table>";
    genericRegexTipText += tr( "Example:" );
    genericRegexTipText += "<br><i><b>du --all --human-readable --time<b/>: %size_human%%tab%%dateyear4%%space%%time_hm%%tab%%fullpath%<i>";
    genericRegexInfo->setToolTip( genericRegexTipText );
}

int ImportDialog::bOk( void ) {
    if ((filename_lineedit->text()).isEmpty()) {
        QMessageBox::warning( this, tr( "Error:" ), tr( "You must be fill the \"Filename\" field!" ));
        return 0;
    }

    if (importTypeCsvGtktalog->isChecked() && separator_lineedit->text().isEmpty()) {
        QMessageBox::warning( this, tr( "Error:" ), tr( "You must be fill the \"Separator\" field!" ));
        return 0;
    }

    filename = filename_lineedit->text();
    separator = separator_lineedit->text();
    createdatabase = newdatabase->isChecked();
    if (importTypeCsvGtktalog->isChecked()) {
        type = 0;
    } else if (importTypeCsvKatCeDe->isChecked()) {
        type = 3;
    } else if (importTypeCsvDisclib->isChecked()) {
        type = 4;
    } else if (importTypeCsvVisualcd->isChecked()) {
        type = 5;
    } else if (importTypeCsvVvv->isChecked()) {
        type = 6;
    } else if (importTypeCsvAdvancedFileOrganizer->isChecked()) {
        type = 7;
    } else if (importTypeCsvFileArchivist->isChecked()) {
        type = 8;
    } else if (importTypeCsvAdvancedDiskCatalog->isChecked()) {
        type = 9;
    } else if (importTypeCsvWhereisit->isChecked()) {
        type = 10;
    } else if (importTypeGtktalogXml->isChecked()) {
        type = 1;
    } else if (importTypeWhereisitXml->isChecked()) {
        type = 2;
    } else if (importTypeWhereisitXml->isChecked()) {
        type = 2;
    } else if (importTypeGenericRegex->isChecked()) {
        type = 11;
    }

    OK = 1;

    close();
    return 0;
}

int ImportDialog::bCan( void ) {
    OK = 0;
    close();
    return 0;
}

void ImportDialog::getFileName() {
    QString filetypes = "";

    if (importTypeCsvGtktalog->isChecked() || importTypeCsvKatCeDe->isChecked() || importTypeCsvDisclib->isChecked()
        || importTypeCsvVisualcd->isChecked() || importTypeCsvVvv->isChecked() || importTypeCsvAdvancedFileOrganizer->isChecked()
        || importTypeCsvAdvancedDiskCatalog->isChecked() || importTypeCsvWhereisit->isChecked()) {
        filetypes = QString( tr( "csv files(*.csv)" ));
    } else if (importTypeGtktalogXml->isChecked()) {
        filetypes = QString( tr( "xml files(*.xml)" ));
    } else if (importTypeWhereisitXml->isChecked()) {
        filetypes = QString( tr( "xml files(*.xml)" ));
    } else if (importTypeCsvFileArchivist->isChecked()) {
        filetypes = QString( tr( "File Archivist files(*.arch)" ));
    } else {
        filetypes = QString( tr( "all files(*.*)" ));
    }

    if (lastDir.isEmpty()) {
        QString homedir;
#ifndef _WIN32
        homedir = getenv( "HOME" );
#else
        homedir = getenv( "USER_PROFILE" );
#endif
        lastDir = homedir;
    }

    filename_lineedit->setText( QFileDialog::getOpenFileName( this, tr( "Choose a file for import" ), lastDir, filetypes ));
    filename = filename_lineedit->text();
}

void ImportDialog::typeChanged() {
    if (importTypeCsvGtktalog->isChecked() || importTypeCsvKatCeDe->isChecked() || importTypeCsvKatCeDe->isChecked()) {
        if (importTypeCsvGtktalog->isChecked() || importTypeCsvDisclib->isChecked()) {
            correctbadstyle->setEnabled( true );
            separator_lab->setEnabled( true );
            separator_lineedit->setEnabled( true );
        } else {
            correctbadstyle->setEnabled( false );
            separator_lab->setEnabled( false );
            separator_lineedit->setEnabled( false );
        }
        if (importTypeCsvWhereisit->isChecked()) {
            separator_lab->setEnabled( true );
            separator_lineedit->setEnabled( true );
        }
        if (importTypeCsvGtktalog->isChecked() || importTypeCsvWhereisit->isChecked()) {
            separator_lineedit->setText( ";" );
        } else if (importTypeCsvDisclib->isChecked()) {
            separator_lineedit->setText( "*" );
        } else if (importTypeCsvAdvancedFileOrganizer->isChecked()) {
            separator_lineedit->setText( "," );
        } else if (importTypeCsvFileArchivist->isChecked()) {
            separator_lineedit->setText( "|" );
        } else if (importTypeCsvFileArchivist->isChecked()) {
            separator_lineedit->setText( ", " );
        }
    } else {
        correctbadstyle->setEnabled( false );
        separator_lab->setEnabled( false );
        separator_lineedit->setEnabled( false );
    }
    if (importTypeCsvVisualcd->isChecked()) {
        correctbadstyle->setEnabled( false );
        correctbadstyle->setChecked( false );
        separator_lineedit->setText( ";" );
    }
    if (importTypeGenericRegex->isChecked()) {
        genericRegex_lineedit->setEnabled( true );
        genericRegexInfo->setEnabled( true );
    } else {
        genericRegex_lineedit->setEnabled( false );
        genericRegexInfo->setEnabled( false );
    }
}

QString ImportDialog::getLastDir() {
    return lastDir;
}

void ImportDialog::setLastDir( QString lastDir ) {
    this->lastDir = lastDir;
}
