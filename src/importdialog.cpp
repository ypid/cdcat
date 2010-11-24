/****************************************************************************
                            Hyper's CD Catalog
	A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke <crissi99@gmx.de>
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/


#include "importdialog.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qfile.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <stdio.h>
#include <stdlib.h>
#include <qvariant.h>


ImportDialog::ImportDialog ( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    if ( !name )
        setName ( "ImportDialog" );
    //	ImportDialogLayout = new QGridLayout( this, 1, 1, 11, 7, "ImportDialogLayout" );

    setSizeGripEnabled ( true );

    layout4 = new Q3GridLayout ( this, 10, 6, 10, 10, "layout4" );

    info_lab = new QLabel ( this, "info_lab" );
    info_lab->setText ( "info" );
    layout4->addMultiCellWidget ( info_lab, 0, 0, 0, 4 );

    filename_lab = new QLabel ( this, "filename_lab" );
    layout4->addWidget ( filename_lab, 1, 1 );

    filename_lineedit = new QLineEdit ( this, "filename_lineedit" );
    layout4->addMultiCellWidget ( filename_lineedit, 1, 1, 2, 4 );

    buttonGetFile = new QPushButton ( this, "buttonGetFile" );
    layout4->addWidget ( buttonGetFile, 1,5 );

    importButtonBox = new Q3ButtonGroup ( 1,Qt::Horizontal,tr ( "Type" ),this,"import_button_box" );
    importButtonBox->setRadioButtonExclusive ( true );
    importTypeCsv = new QRadioButton ( "&CSV",importButtonBox, "importTypeCsv" );
    importTypeGtktalogXml = new QRadioButton ( "Gtktalog &XML",importButtonBox ,"importTypeGtktalogXml" );
    importTypeWhereisitXml = new QRadioButton ( "&WhereIsIt XML (classic)", importButtonBox, "importTypeWhereisitXml" );

    layout4->addMultiCellWidget ( importButtonBox, 2, 5, 1, 4 );

    newdatabase = new QCheckBox ( this, "newdatabase" );
    newdatabase->setText ( tr ( "Create new Database" ) );
    layout4->addMultiCellWidget ( newdatabase, 6, 6, 1, 5 );

    separator_lab = new QLabel ( this, "separator_lab" );
    layout4->addMultiCellWidget ( separator_lab, 7, 7, 1, 3 );
    separator_lineedit = new QLineEdit ( this, "separator_lineedit" );
    separator_lineedit->setMinimumSize ( QSize ( 0, 0 ) );
    separator_lineedit->setMaximumSize ( QSize ( 20, 32767 ) );
    separator_lineedit->setMaxLength ( 1 );
    layout4->addMultiCellWidget ( separator_lineedit, 7, 7, 4, 4 );


    correctbadstyle = new QCheckBox ( this, "correctbadstyle" );
    correctbadstyle->setText ( tr ( "Correct bad style from gtktalog export" ) );
    layout4->addMultiCellWidget ( correctbadstyle, 8, 8, 1, 5 );

    buttonOK = new QPushButton ( this, "buttonOK" );
    buttonOK->setDefault ( true );
    buttonOK-> setMinimumWidth ( 100 );
    layout4->addWidget ( buttonOK, 9, 2 );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel-> setMinimumWidth ( 100 );
    layout4->addMultiCellWidget ( buttonCancel, 9, 9, 3, 3 );
    /*
    	QSpacerItem* spacer = new QSpacerItem( 181, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    	layout4->addMultiCell( spacer, 5, 5, 0, 2 );

    	QSpacerItem* spacer_2 = new QSpacerItem( 291, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    	layout4->addMultiCell( spacer_2, 3, 3, 2, 5 );
    */
    importTypeWhereisitXml->setChecked ( true );
    correctbadstyle->setEnabled ( false );
    separator_lab->setEnabled ( false );
    separator_lineedit->setEnabled ( false );


    //	ImportDialogLayout->addLayout( layout4, 0, 0 );

    languageChange();
    resize ( QSize ( 450, 350 ).expandedTo ( minimumSizeHint() ) );

    setMinimumSize ( minimumSizeHint() );

    //this->sizeHint();
    //setFixedSize( size() );

    //clearWState( WState_Polished );

    connect ( buttonOK, SIGNAL ( clicked() ), this, SLOT ( bOk() ) );
    connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( bCan() ) );
    connect ( buttonGetFile, SIGNAL ( clicked() ), this, SLOT ( getFileName() ) );
    connect ( importTypeCsv, SIGNAL ( clicked() ), this, SLOT ( typeChanged() ) );
    connect ( importTypeGtktalogXml, SIGNAL ( clicked() ), this, SLOT ( typeChanged() ) );
    connect ( importTypeWhereisitXml, SIGNAL ( clicked() ), this, SLOT ( typeChanged() ) );
    separator_lab->setEnabled ( false );
    separator_lineedit->setEnabled ( false );



    // tmp
    /*
    importTypeCsv->setChecked(true);
    separator_lab->setEnabled(true);
    separator_lineedit->setEnabled(true);
    //filename_lineedit->setText("c:\\devel\\mp3s.csv");
    filename_lineedit->setText("/data3/res3/musikvideo.csv");
    separator_lineedit->setText("*");
    correctbadstyle->setEnabled(true);
    correctbadstyle->setChecked(true);
    */
    // tmp
    //filename_lineedit->setText("/home/crissi/compile/cvs/CdCat-0.98pre_whereisit_xml_import/sample-export.xml");
    filename_lineedit->setText ( "" );

}

/*
 *  Destroys the object and frees any allocated resources
 */
ImportDialog::~ImportDialog() {
    //no need to delete child widgets, Qt does it for us!
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ImportDialog::languageChange() {
    setCaption ( tr ( "Import CSV file" ) );
    filename_lab->setText ( tr ( "File:" ) );

    separator_lineedit->setText ( ";" );
    separator_lab->setText ( tr ( "Separator:" ) );
    QToolTip::add
    ( separator_lineedit , tr ( "This is the separator in dataline\n<path>SEPARATOR<size>SEPARATOR<date><space><time>" ) );
    Q3WhatsThis::add
    ( separator_lineedit , tr ( "Separator in data line" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
    buttonOK->setText ( tr ( "Import" ) );
    buttonGetFile->setText ( tr ( "..." ) );

    importButtonBox->setTitle ( tr ( "Type" ) );
    importTypeCsv->setText ( tr ( "&Text (csv)" ) );
    importTypeGtktalogXml->setText ( tr ( "Gtktalog &XML" ) );
    importTypeWhereisitXml->setText ( tr ( "&WhereIsIt XML (classic)" ) );

    QToolTip::add
    ( importButtonBox , tr ( "Select the type of import here" ) );

    QToolTip::add
    ( importTypeCsv , tr ( "Select this for importing a text import (csv)" ) );

    QToolTip::add
    ( importTypeGtktalogXml , tr ( "Select this for importing a xml report generated frm gtktalog" ) );

    QToolTip::add
    ( buttonGetFile , tr ( "Open the file dialog for selecting file to import." ) );
    Q3WhatsThis::add
    ( buttonGetFile , tr ( "Open file dialog" ) );
    QToolTip::add
    ( correctbadstyle , tr ( "Corrects bad output style from gtktalog.\n<media>SEPARATOR/<dir>/SEPARATOR/<dir>\n will be to\n<media>/<dir>/<dir>" ) );
    Q3WhatsThis::add
    ( correctbadstyle , tr ( "Corrects bad output style (gtktalog)" ) );
    info_lab->setText ( tr ( "<strong>Please read the README_IMPORT before you import!</strong>" ) );



}

int ImportDialog::bOk ( void ) {
    if ( ( filename_lineedit->text() ).isEmpty() ) {
        QMessageBox::warning ( this, tr ( "Error:" ), tr ( "You must be fill the \"Filename\" field!" ) );
        return 0;
    }

    if ( importTypeCsv->isChecked() && separator_lineedit->text().isEmpty() ) {
        QMessageBox::warning ( this, tr ( "Error:" ), tr ( "You must be fill the \"Separator\" field!" ) );
        return 0;
    }

    filename = filename_lineedit->text();
    separator = separator_lineedit->text();
    createdatabase = newdatabase->isChecked();
    if ( importTypeCsv->isChecked() )
        type = 0;
    if ( importTypeGtktalogXml->isChecked() )
        type = 1;
    if ( importTypeWhereisitXml->isChecked() )
        type = 2;

    OK = 1;

    close();
    return 0;
}

int ImportDialog::bCan ( void ) {
    OK = 0;
    close();
    return 0;
}

void ImportDialog::getFileName() {
    QString filetypes;

    if ( importTypeCsv->isChecked() )
        filetypes = QString ( tr ( "csv files(*.csv)" ) );
    if ( importTypeGtktalogXml->isChecked() )
        filetypes = QString ( tr ( "xml files(*.xml)" ) );
    if ( importTypeWhereisitXml->isChecked() )
        filetypes = QString ( tr ( "xml files(*.xml)" ) );
    else
        filetypes = QString ( tr ( "all files(*.*)" ) );

    filename_lineedit->setText ( Q3FileDialog::getOpenFileName (
                                     getenv ( "HOME" ),
                                     //     "/data/compile/CdCat-0.91/",
                                     filetypes,
                                     0,
                                     tr ( "open file dialog" ),
                                     tr ( "Choose a file for import" ) ) );

    filename= filename_lineedit->text();
}

void ImportDialog::typeChanged() {
    if ( importTypeCsv->isChecked() ) {
        correctbadstyle->setEnabled ( true );
        separator_lab->setEnabled ( true );
        separator_lineedit->setEnabled ( true );

    } else {
        correctbadstyle->setEnabled ( false );
        separator_lab->setEnabled ( false );
        separator_lineedit->setEnabled ( false );
    }

}
