
/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "find.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3frame.h>
#include <q3header.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <QSpinBox>
#include <qtextcodec.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <QCloseEvent>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QDate>
#include <QCalendarWidget>
#include <QMouseEvent>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QTextCodec>

// #include <pcre.h>
#include <QRegExp>
#include <string.h>
#include <ctype.h>

#include "dbase.h"
#include "mainwidget.h"
#include "wdbfile.h"
#include "guibase.h"
#include "adddialog.h"
#include "config.h"
#include "cdcat.h"
#include "icons.h"

#include <string.h>
#include "dmetaph.h"
#include <iostream>
#include <QFileDialog>
using namespace std;

findDialog::findDialog ( CdCatMainWidget* parent, const char* name, bool isFindDuplicates, bool modal, Qt::WFlags fl )
	: QDialog ( parent, name, modal, fl )
{
	if ( !name ) {
		if (isFindDuplicates) {
		}
		else {
			setName ( "findDialog" );
		}
	}
	setIcon ( *get_t_find_icon() );
	
	mainw = parent;
	use_unsharpsearch = false;
	this->isFindDuplicates = isFindDuplicates;
	
	setSizeGripEnabled ( TRUE );
	if (!isFindDuplicates) {
		FindDialogBaseLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "FindDialogBaseLayout" );

	//    QSpacerItem* spacer = new QSpacerItem( 210, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_2 = new QSpacerItem ( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_3 = new QSpacerItem ( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_4 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_5 = new QSpacerItem ( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_6 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_7 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_8 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_9 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_10 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_11 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_12 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

		layout40 = new Q3VBoxLayout ( 0, 0, 6, "layout40" );
		layout39 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout39" );
		layout36 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout36" );
		layout37 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout37" );
		layout31 = new Q3VBoxLayout ( 0, 0, 6, "layout31" );
		layout17 = new Q3HBoxLayout ( 0, 0, 6, "layout17" );
		layout15 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout15" );
		layout30 = new Q3HBoxLayout ( 0, 0, 6, "layout30" );
		layout16 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout16" );
	//     layout17 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout17" );
		layout_size_min = new Q3HBoxLayout ( 0, 0, 2, "layout_size_min" );
		layout_size_max = new Q3HBoxLayout ( 0, 0, 2, "layout_size_max" );
		layout_find_in_archive = new Q3HBoxLayout ( 0, 0, 2, "layout_find_in_archive" );

		leText = new QLineEdit ( this, "leText" );

		deDateStart = new QDateTimeEdit ( QDateTime ( QDate ( 1, 1, 1900 ) ) , this );
		deDateEnd = new QDateTimeEdit ( QDateTime ( QDate().currentDate() ), this );

		cbCasesens = new QCheckBox ( this, "cbCasesens" );
		cbEasy = new QCheckBox ( this, "cbEasy" );

		cbFilename = new QCheckBox ( this, "cbFilename" );
		cbDirname  = new QCheckBox ( this, "cbDirname" );
		cbComment  = new QCheckBox ( this, "cbComment" );
		cbContent  = new QCheckBox ( this, "cbContent" );

		cbArtist = new QCheckBox ( this, "cbArtist" );
		cbTitle = new QCheckBox ( this, "cbTitle" );
		cbAlbum = new QCheckBox ( this, "cbAlbum" );
		cbTcomm = new QCheckBox ( this, "cbTcomm" );
		cbDateStart = new QCheckBox ( this, "cbDateStart" );
		cbDateEnd = new QCheckBox ( this, "cbDateEnd" );
		cbSizeMin = new QCheckBox ( this, "cbSizeMin" );
		cbSizeMax = new QCheckBox ( this, "cbSizeMax" );
		cbUnsharpSearch = new QCheckBox ( this, "cbUnsharpSearch" );
		cbFindInArchive = new QCheckBox ( this, "cbFindInArchive" );

		cbOwner = new QComboBox ( FALSE, this, "cbOwner" );
		cbOwner->setMinimumSize ( QSize ( 0, 0 ) );

		cbSin = new QComboBox ( FALSE, this, "cbSin" );

		cbSizeUnitMin = new QComboBox ( false, this, "cbSizeUnitMin" );
		cbSizeUnitMin->addItem ( tr ( "Byte" ) );
		cbSizeUnitMin->addItem ( tr ( "KiB" ) );
		cbSizeUnitMin->addItem ( tr ( "MiB" ) );
		cbSizeUnitMin->addItem ( tr ( "GiB" ) );

		cbSizeUnitMax = new QComboBox ( false, this, "cbSizeUnitMax" );
		cbSizeUnitMax->addItem ( tr ( "Byte" ) );
		cbSizeUnitMax->addItem ( tr ( "KiB" ) );
		cbSizeUnitMax->addItem ( tr ( "MiB" ) );
		cbSizeUnitMax->addItem ( tr ( "GiB" ) );

		spSizeMin = new QSpinBox ( this, "spSizeMin" );
		spSizeMin->setMinimum ( 1 );
		spSizeMin->setMaximum ( 10000000 );

		spSizeMax = new QSpinBox ( this, "spSizeMax" );
		spSizeMax->setMinimum ( 1 );
		spSizeMax->setMaximum ( 10000000 );

		buttonOk = new QPushButton ( this, "buttonOk" );
		buttonOk->setAutoDefault ( TRUE );
		buttonOk->setDefault ( TRUE );
		buttonOk->setMinimumWidth ( 80 );

		buttonCancel = new QPushButton ( this, "buttonCancel" );
		buttonCancel->setAutoDefault ( TRUE );
		buttonCancel->setMinimumWidth ( 80 );

		resultsl = new Q3ListView ( this, "resultsl" );
		resultsl->addColumn ( tr ( "Name" ) );
		resultsl->addColumn ( tr ( "Type" ) );
		resultsl->addColumn ( tr ( "Size" ) );
		resultsl->addColumn ( tr ( "Media" ) );
		resultsl->addColumn ( tr ( "Path" ) );
		resultsl->addColumn ( tr ( "Modification" ) );
		resultsl->addColumn ( tr ( "Comment" ) );

		resultsl->setColumnAlignment ( 2, Qt::AlignRight );
		resultsl->setColumnWidthMode ( 0, Q3ListView::Maximum );

		buttonClose = new QPushButton ( this, "buttonClose" );
		buttonPrintResult = new QPushButton ( this, "buttonPrintResult" );
		buttonExportResult = new QPushButton ( this, "buttonExportResult" );

		textLabel3 = new QLabel ( this, "textLabel3" );
		textLabel1 = new QLabel ( this, "textLabel1" );
		textLabel2 = new QLabel ( this, "textLabel2" );
		textLabel5 = new QLabel ( this, "textLabel5" );
		textLabel6 = new QLabel ( this, "textLabel6" );
		textLabel7 = new QLabel ( this, "textLabel7" );
		textLabelFindInArchive = new QLabel ( this, "textLabelFindInArchive" );

		/* saved ops: */

		cbTcomm    -> setChecked ( mainw->cconfig->find_mco );
		cbCasesens -> setChecked ( mainw->cconfig->find_cs );
		cbArtist   -> setChecked ( mainw->cconfig->find_mar );
		cbAlbum    -> setChecked ( mainw->cconfig->find_mal );
		cbComment  -> setChecked ( mainw->cconfig->find_co );
		cbTitle    -> setChecked ( mainw->cconfig->find_mti );
		cbEasy     -> setChecked ( mainw->cconfig->find_em );
		cbFilename -> setChecked ( mainw->cconfig->find_fi );
		cbDirname  -> setChecked ( mainw->cconfig->find_di );
		cbContent  -> setChecked ( mainw->cconfig->find_ct );
		cbDateStart  -> setChecked ( mainw->cconfig->find_date_start );
		cbDateEnd  -> setChecked ( mainw->cconfig->find_date_end );
		cbSizeMin -> setChecked ( mainw->cconfig->find_size_min );
		cbSizeMax -> setChecked ( mainw->cconfig->find_size_max );
		cbUnsharpSearch -> setChecked ( mainw->cconfig->find_unsharp_search );
		cbFindInArchive -> setChecked ( mainw->cconfig->find_in_archive );


		/* layouts:   */
		layout36->addWidget ( cbCasesens, 1, 0 );
		layout36->addWidget ( cbDirname , 2, 0 );
		layout36->addWidget ( cbFilename, 3, 0 );
		layout36->addWidget ( cbComment , 4, 0 );
		layout36->addWidget ( cbContent , 5, 0 );
		layout36->addWidget ( cbEasy  , 1, 1 );
		layout36->addWidget ( cbArtist, 2, 1 );
		layout36->addWidget ( cbTitle , 3, 1 );
		layout36->addWidget ( cbAlbum , 4, 1 );
		layout36->addWidget ( cbTcomm , 5, 1 );
		layout36->addMultiCellWidget ( leText, 0, 0, 0, 1 );
		layout37->addWidget ( cbDateStart , 6, 0 );
		layout37->addWidget ( cbDateEnd , 7, 0 );
		layout37->addWidget ( deDateStart, 6, 1 );
		layout37->addWidget ( deDateEnd, 7, 1 );
		layout37->addWidget ( cbSizeMin, 8, 0 );
		layout37->addWidget ( cbSizeMax, 9, 0 );
		layout37->addWidget ( cbUnsharpSearch, 10, 0 );


		layout37->addWidget ( cbOwner, 3, 1 );
		layout37->addWidget ( cbSin, 2, 1 );
		layout37->addMultiCellWidget ( textLabel3, 0, 0, 0, 1 );
		layout37->addWidget ( textLabel1, 2, 0 );
		layout37->addWidget ( textLabel2, 3, 0 );
		layout37->addMultiCell ( spacer_3, 1, 1, 0, 1 );

		layout_size_min->addWidget ( spSizeMin );
		layout_size_min->addWidget ( cbSizeUnitMin );
		cbSizeUnitMin->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
		layout37->addItem ( layout_size_min, 8, 1 );

		layout_size_max->addWidget ( spSizeMax );
		layout_size_max->addWidget ( cbSizeUnitMax );
		cbSizeUnitMin->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
		layout37->addItem ( layout_size_max, 9, 1 );

		layout_find_in_archive->addWidget ( cbFindInArchive );
		layout_find_in_archive->addWidget ( textLabelFindInArchive );
		cbFindInArchive->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
		layout37->addItem ( layout_find_in_archive, 10, 1 );

		layout39->addMultiCellLayout ( layout36, 0, 1, 1, 1 );
		layout39->addItem ( spacer_2, 1, 0 );
		layout39->addLayout ( layout37, 0, 0 );

		layout15->addWidget ( buttonOk, 0, 0 );
		layout15->addItem ( spacer_5, 0, 1 );
		layout15->addWidget ( buttonCancel, 0, 2 );

		layout17->addItem ( spacer_6 );
		layout17->addLayout ( layout15 );
		layout17->addItem ( spacer_4 );

		layout30->addItem ( spacer_7 );
		layout30->addWidget ( textLabel5 );
		layout30->addItem ( spacer_8 );

		layout16->addItem ( spacer_9, 0, 2 );
		layout16->addWidget ( buttonClose, 0, 1 );
		layout16->addWidget ( buttonPrintResult, 0,  3 );
		layout16->addWidget ( buttonExportResult, 0, 4 );
		layout16->addItem ( spacer_10, 0, 0 );
		layout16->addItem ( spacer_11, 0, 2 );
		layout16->addItem ( spacer_12, 0, 5 );

		layout31->addLayout ( layout17 );
		layout31->addLayout ( layout30 );
		layout31->addWidget ( resultsl );
		layout31->addLayout ( layout16 );

		layout40->addLayout ( layout39 );
		layout40->addLayout ( layout31 );

		FindDialogBaseLayout->addLayout ( layout40, 0, 0 );

		resize ( QSize ( mainw->cconfig->findWidth, mainw->cconfig->findHeight ).expandedTo ( minimumSizeHint() ) );
		move ( mainw->cconfig->findX, mainw->cconfig->findY );

		languageChange();

		fillCBox();
		connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( cancele() ) );
		connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( seeke() ) );
		connect ( buttonClose, SIGNAL ( clicked() ), this, SLOT ( closee() ) );
		connect ( buttonPrintResult, SIGNAL ( clicked() ), this, SLOT ( printResultClicked() ) );
		connect ( buttonExportResult, SIGNAL ( clicked() ), this, SLOT ( exportResultClicked() ) );
		connect ( resultsl, SIGNAL ( currentChanged ( Q3ListViewItem * ) ), this, SLOT ( select ( Q3ListViewItem * ) ) );
		connect ( resultsl, SIGNAL ( clicked ( Q3ListViewItem * ) ), this, SLOT ( select ( Q3ListViewItem * ) ) );
		connect ( cbDateStart, SIGNAL ( stateChanged ( int ) ), this, SLOT ( dateStartChanged ( int ) ) );
		connect ( cbDateEnd, SIGNAL ( stateChanged ( int ) ), this, SLOT ( dateEndChanged ( int ) ) );
		//connect ( deDateStart, SIGNAL ( clicked()),this,SLOT ( dateStartDoubleClicked()));
		//connect ( deDateEnd, SIGNAL ( ),this,SLOT ( dateEndDoubleClicked()));
		connect ( cbSizeMin, SIGNAL ( clicked() ), this, SLOT ( sizeMinClicked() ) );
		connect ( cbSizeMax, SIGNAL ( clicked() ), this, SLOT ( sizeMaxClicked() ) );


		deDateStart->setEnabled ( false );
		deDateEnd->setEnabled ( false );
		cbSizeUnitMin->setEnabled ( false );
		spSizeMin->setEnabled ( false );

		cbSizeUnitMax->setEnabled ( false );
		spSizeMax->setEnabled ( false );

		deDateStart->setDateTime ( mainw->cconfig->find_date_start_val );
		deDateEnd->setDateTime ( mainw->cconfig->find_date_end_val );

		deDateStart->setDisplayFormat ( "dd.MM.yyyy hh:mm" );
		deDateEnd->setDisplayFormat ( "dd.MM.yyyy hh:mm" );

		cbSizeMin->setChecked ( mainw->cconfig->find_size_min );
		cbSizeMax->setChecked ( mainw->cconfig->find_size_max );
		spSizeMin->setValue ( mainw->cconfig->find_size_min_val );
		spSizeMax->setValue ( mainw->cconfig->find_size_max_val );

		spSizeMin->setEnabled ( mainw->cconfig->find_size_min );
		spSizeMax->setEnabled ( mainw->cconfig->find_size_max );

		cbSizeUnitMin->setEnabled ( mainw->cconfig->find_size_min );
		cbSizeUnitMax->setEnabled ( mainw->cconfig->find_size_max );

		cbSizeUnitMin->setCurrentIndex ( mainw->cconfig->find_size_unit_min_val );
		cbSizeUnitMax->setCurrentIndex ( mainw->cconfig->find_size_unit_max_val );


		leText->setText ( mainw->cconfig->lastSearchPattern );

		dateStartChanged ( 0 );
		dateEndChanged ( 0 );

		setTabOrder ( leText, buttonOk );
		buttonPrintResult->setEnabled ( false );
		buttonExportResult->setEnabled ( false );
		leText->setFocus();
	}
	else {
		if ( !name )
			setName ( "findDuplicatesDialog" );
		setIcon ( *get_t_find_icon() );
		
		mainw = parent;
		searchFilepath = "";
		if ( mainw->guis->standON != NULL )
			searchFilepath = mainw->guis->standON->getFullPath();
		
		setSizeGripEnabled ( TRUE );
		FindDialogBaseLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "FindDialogBaseLayout" );
		
		QSpacerItem* spacer_4 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_5 = new QSpacerItem ( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_6 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_7 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_8 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_9 = new QSpacerItem ( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_10 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_11 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		QSpacerItem* spacer_12 = new QSpacerItem ( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		
		layout40 = new Q3VBoxLayout ( 0, 0, 6, "layout40" );
		layout39 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout39" );
		//layout36 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout36" );
		//layout37 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout37" );
		layout31 = new Q3VBoxLayout ( 0, 0, 6, "layout31" );
		layout17 = new Q3HBoxLayout ( 0, 0, 6, "layout17" );
		layout15 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout15" );
		layout30 = new Q3HBoxLayout ( 0, 0, 6, "layout30" );
		layout16 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout16" );
		
		buttonOk = new QPushButton ( this, "buttonOk" );
		buttonOk->setAutoDefault ( TRUE );
		buttonOk->setDefault ( TRUE );
		buttonOk->setMinimumWidth ( 80 );
		
		buttonCancel = new QPushButton ( this, "buttonCancel" );
		buttonCancel->setAutoDefault ( TRUE );
		buttonCancel->setMinimumWidth ( 80 );
		
		textLabel5 = new QLabel ( this, "textLabel5" );
		
		resultsl = new Q3ListView ( this, "resultsl" );
		resultsl->addColumn ( tr ( "Name" ) );
		resultsl->addColumn ( tr ( "Type" ) );
		resultsl->addColumn ( tr ( "Size" ) );
		resultsl->addColumn ( tr ( "Media" ) );
		resultsl->addColumn ( tr ( "Path" ) );
		resultsl->addColumn ( tr ( "Modification" ) );
		resultsl->addColumn ( tr ( "Comment" ) );
		
		resultsl->setColumnAlignment ( 2, Qt::AlignRight );
		resultsl->setColumnWidthMode ( 0, Q3ListView::Maximum );
		
		buttonClose = new QPushButton ( this, "buttonClose" );
		
		layout30->addItem ( spacer_7 );
		layout30->addWidget ( textLabel5 );
		layout30->addItem ( spacer_8 );
		
		layout15->addWidget ( buttonOk, 0, 0 );
		layout15->addItem ( spacer_5, 0, 1 );
		layout15->addWidget ( buttonCancel, 0, 2 );
		
		layout16->addItem ( spacer_9, 0, 2 );
		layout16->addWidget ( buttonClose, 0, 1 );
		layout16->addItem ( spacer_10, 0, 0 );
		layout16->addItem ( spacer_11, 0, 2 );
		layout16->addItem ( spacer_12, 0, 5 );
		
		layout17->addItem ( spacer_6 );
		layout17->addLayout ( layout15 );
		layout17->addItem ( spacer_4 );
		
		layout31->addLayout ( layout17 );
		layout31->addLayout ( layout30 );
		layout31->addWidget ( resultsl );
		layout31->addLayout ( layout16 );
		
		layout40->addLayout ( layout39 );
		layout40->addLayout ( layout31 );
		
		FindDialogBaseLayout->addLayout ( layout40, 0, 0 );
		
		resize ( QSize ( mainw->cconfig->findWidth, mainw->cconfig->findHeight ).expandedTo ( minimumSizeHint() ) );
		move ( mainw->cconfig->findX, mainw->cconfig->findY );
		
		languageChange();
		
		connect ( resultsl, SIGNAL ( currentChanged ( Q3ListViewItem * ) ), this, SLOT ( select ( Q3ListViewItem * ) ) );
		connect ( resultsl, SIGNAL ( clicked ( Q3ListViewItem * ) ), this, SLOT ( select ( Q3ListViewItem * ) ) );
		connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( cancele() ) );
		connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( seeke() ) );
		connect ( buttonClose, SIGNAL ( clicked() ), this, SLOT ( closee() ) );
		
	}
}

/*
 *  Destroys the object and frees any allocated resources
 */
findDialog::~findDialog() {
	// no need to delete child widgets, Qt does it all for us
}
/***************************************************************************/
void findDialog::languageChange() {
	if (isFindDuplicates ) {
		setCaption ( tr ( "Search for duplicates in the database..." ) );
		resultsl->header()->setLabel ( 0, tr ( "Name" ) );
		resultsl->header()->setLabel ( 1, tr ( "Type" ) );
		resultsl->header()->setLabel ( 2, tr ( "Size" ) );
		resultsl->header()->setLabel ( 3, tr ( "Media" ) );
		resultsl->header()->setLabel ( 4, tr ( "Path" ) );
		resultsl->header()->setLabel ( 5, tr ( "Modification" ) );
		resultsl->header()->setLabel ( 6, tr ( "Comment" ) );

		buttonOk->setText ( tr ( "&Start search" ) );
	#ifndef _WIN32
		buttonOk->setAccel ( QKeySequence ( QString::null ) );
	#endif
		buttonCancel->setText ( tr ( "&Cancel" ) );
	#ifndef _WIN32
		buttonCancel->setAccel ( QKeySequence ( QString::null ) );
	#endif
		textLabel5->setText ( tr ( "Duplicates for:" ) + " " + searchFilepath );
		buttonClose->setText ( tr ( "Close" ) );

		resultsl->clear();
	}
	else {
		setCaption ( tr ( "Search in the database..." ) );
		textLabel1->setText ( tr ( "Seek in:" ) );
		textLabel2->setText ( tr ( "Owner:" ) );
		resultsl->header()->setLabel ( 0, tr ( "Name" ) );
		resultsl->header()->setLabel ( 1, tr ( "Type" ) );
		resultsl->header()->setLabel ( 2, tr ( "Size" ) );
		resultsl->header()->setLabel ( 3, tr ( "Media" ) );
		resultsl->header()->setLabel ( 4, tr ( "Path" ) );
		resultsl->header()->setLabel ( 5, tr ( "Modification" ) );
		resultsl->header()->setLabel ( 6, tr ( "Comment" ) );
		textLabel3->setText ( tr ( "Find:" ) );
		cbFilename->setText ( tr ( "File name" ) );
		cbAlbum->setText ( tr ( "mp3-tag Album" ) );
		cbArtist->setText ( tr ( "mp3-tag Artist" ) );
		cbTcomm->setText ( tr ( "mp3-tag Comment" ) );
		cbDirname->setText ( tr ( "Media / Directory name" ) );
		cbTitle->setText ( tr ( "mp3-tag Title" ) );
		cbComment->setText ( tr ( "Comment" ) );
		cbContent->setText ( tr ( "Content" ) );
		cbDateStart->setText ( tr ( "Date start" ) );
		cbDateEnd->setText ( tr ( "Date end" ) );
		cbSizeMin->setText ( tr ( "Min size" ) );
		cbSizeMax->setText ( tr ( "Max size" ) );
		cbUnsharpSearch->setText ( tr ( "Unsharp search (slow)" ) );
		cbFindInArchive->setText ( tr ( "Find in archives too" ) );
		buttonOk->setText ( tr ( "&Start search" ) );
	#ifndef _WIN32
		buttonOk->setAccel ( QKeySequence ( QString::null ) );
	#endif
		buttonCancel->setText ( tr ( "&Cancel" ) );
	#ifndef _WIN32
		buttonCancel->setAccel ( QKeySequence ( QString::null ) );
	#endif
		textLabel5->setText ( tr ( "Results: search not started" ) );
		buttonClose->setText ( tr ( "Close / Go to selected" ) );
		buttonPrintResult->setText ( tr ( "Print result..." ) );
		buttonExportResult->setText ( tr ( "Export result to HTML..." ) );
		cbCasesens->setText ( tr ( "Case sensitive" ) );
		cbEasy->setText ( tr ( "Use easy matching instead regex" ) );
		resultsl->clear();
		
	}
	
}
/***************************************************************************/
int findDialog::saveState ( void ) {
	mainw->cconfig->find_em    = cbEasy->isChecked();
	mainw->cconfig->find_cs    = cbCasesens->isChecked();
	mainw->cconfig->find_di    = cbDirname->isChecked();
	mainw->cconfig->find_fi    = cbFilename->isChecked();
	mainw->cconfig->find_co    = cbComment->isChecked();
	mainw->cconfig->find_ct    = cbContent->isChecked();
	mainw->cconfig->find_mar   = cbArtist->isChecked();
	mainw->cconfig->find_mti   = cbTitle->isChecked();
	mainw->cconfig->find_mco   = cbTcomm->isChecked();
	mainw->cconfig->find_mal   = cbAlbum->isChecked();
	mainw->cconfig->find_date_start   = cbDateStart->isChecked();
	mainw->cconfig->find_date_end   = cbDateEnd->isChecked();
	mainw->cconfig->find_date_start_val   = deDateStart->dateTime();
	mainw->cconfig->find_date_end_val   = deDateEnd->dateTime();
	mainw->cconfig->find_size_min   = cbSizeMin->isChecked();
	mainw->cconfig->find_size_max   = cbSizeMax->isChecked();
	mainw->cconfig->find_size_min_val = spSizeMin->value();
	mainw->cconfig->find_size_max_val = spSizeMax->value();
	mainw->cconfig->find_size_unit_min_val = cbSizeUnitMin->currentIndex();
	mainw->cconfig->find_size_unit_max_val = cbSizeUnitMax->currentIndex();
	mainw->cconfig->find_unsharp_search = cbUnsharpSearch->isChecked();
	mainw->cconfig->find_in_archive = cbFindInArchive->isChecked();
	mainw->cconfig->findX      = x();
	mainw->cconfig->findY      = y();
	mainw->cconfig->findWidth  = width();
	mainw->cconfig->findHeight = height();
	mainw->cconfig->lastSearchPattern = leText->text();
	mainw->cconfig->writeConfig();
	return 0;
}
/***************************************************************************/
int findDialog::fillCBox ( void ) {
	int  i, f, c;
	Node *tmp = mainw->db->getRootNode();

	if ( tmp == NULL )
		return 0;

	cbOwner->clear();
	cbOwner->insertItem ( tr ( "All/Everybody" ), 0 );
	cbOwner->insertItem ( ( ( DBCatalog * ) ( tmp->data ) )->owner );

	cbSin  ->clear();
	cbSin  ->insertItem ( tr ( "All media" ), 0 );

	tmp = tmp->child; //Jump to the first media
	while ( tmp != NULL ) {
		if ( tmp->type != HC_MEDIA ) {
			tmp = tmp->next;
			continue;
		}

		cbSin  ->insertItem ( tmp->getNameOf() );
		c = cbOwner->count();
		for ( i = 0, f = 1; c > i; i++ )
			if ( ( ( DBMedia * ) ( tmp->data ) )->owner == cbOwner->text ( i ) )
				f = 0;
		if ( f )
			cbOwner->insertItem ( ( ( DBMedia * ) ( tmp->data ) )->owner );
		tmp = tmp->next;
	}
	return 0;
}
/***************************************************************************/

int findDialog::select ( Q3ListViewItem *i ) {
	if ( i == NULL )
		return 0;

	if ( i->text ( 3 ).isEmpty() ) //Not a real result ("There is no matching" label)
		return 0;

	QString nodepath = i->text ( 4 ).mid ( 2, i->text ( 4 ).length() - 1 );
//    std::cerr << "select: nodepath " << qPrintable(nodepath) << std::endl;

	mainw->guis->updateListFromNode (
	    ( mainw->guis->getNodeFromFullName ( mainw->db->getRootNode(), nodepath ) ) );
	for ( Q3ListViewItemIterator it = mainw->listView->firstChild(); it.current(); it++ ) {
		if ( ( it.current() )->text ( 0 ) == i->text ( 0 ) ) {
			mainw->listView->setCurrentItem ( it.current() );
			mainw->listView->curr_vis();
			mainw->listView->repaint();
		}
	}
	return 0;
}
/***************************************************************************/
void findDialog::closeEvent ( QCloseEvent *ce ) {
	saveState();
	QWidget::closeEvent ( ce );
}

/***************************************************************************/
int findDialog::closee ( void ) {
	close ();
	return 0;
}
/***************************************************************************/
int findDialog::cancele ( void ) {

	close();
	return 0;
}

int findDialog::seeke ( void ) {
	seekEngine *se;
	
	if ( mainw == NULL && mainw->db == NULL )
		return 0;
	
	if (!isFindDuplicates) {
		if ( ( leText->text() ).isEmpty() && !cbDateStart->isChecked() && !cbDateEnd->isChecked() && !cbSizeMin->isChecked() && !cbSizeMax->isChecked() )
			return 0;
	}
	
	se = new seekEngine ( this, isFindDuplicates );
	se->start_seek();
	delete se;
	if (!isFindDuplicates) {
		if ( resultsl->childCount() > 0 ) {
			buttonPrintResult->setEnabled ( true );
			buttonExportResult->setEnabled ( true );
		}
	}
	return 0;
}

void findDialog::dateStartChanged ( int ) {
	if ( cbDateStart->isChecked() )
		deDateStart->setEnabled ( true );
	else
		deDateStart->setEnabled ( false );
}

void findDialog::dateEndChanged ( int ) {
	if ( cbDateEnd->isChecked() )
		deDateEnd->setEnabled ( true );
	else
		deDateEnd->setEnabled ( false );
}

void findDialog::dateStartDoubleClicked() {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( *DEBUG_INFO_ENABLED )
		std::cerr << "dateStartDoubleClicked" << endl;
}

void findDialog::dateEndDoubleClicked() {
}

void findDialog::sizeMinClicked() {
	if ( cbSizeMin->isChecked() ) {
		cbSizeUnitMin->setEnabled ( true );
		spSizeMin->setEnabled ( true );
	}
	else {
		cbSizeUnitMin->setEnabled ( false );
		spSizeMin->setEnabled ( false );
	}
}

void findDialog::sizeMaxClicked() {
	if ( cbSizeMax->isChecked() ) {
		cbSizeUnitMax->setEnabled ( true );
		spSizeMax->setEnabled ( true );
	}
	else {
		cbSizeUnitMax->setEnabled ( false );
		spSizeMax->setEnabled ( false );
	}
}

void findDialog::exportResult ( bool isPrint ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	/* get info from results listview
		TODO rework as model/view and implement as print view
	*/

	QPrinter printer;
	QString output_filename = "cdcat_result.html";
	if ( isPrint ) {
		QPrintDialog *dialog = new QPrintDialog ( &printer, this );
		dialog->setWindowTitle ( tr ( "Print cdcat result" ) );
		if ( dialog->exec() != QDialog::Accepted )
			return;
	}
	else {
		output_filename = QFileDialog::getSaveFileName ( 0, tr ( "Result file name" ), "cdcat_result.html", "*.html" );
		if ( output_filename.isEmpty() ) {
			QMessageBox::critical ( 0, tr ( "Filename missing" ), tr ( "Please enter a filename!" ) );
			return;
		}
	}
	QTextDocument mydoc;
	QTextCodec::setCodecForCStrings ( QTextCodec::codecForName ( "UTF-8" ) );
	QTextCodec::setCodecForLocale ( QTextCodec::codecForName ( "UTF-8" ) );
	QTextCodec::setCodecForTr ( QTextCodec::codecForName ( "UTF-8" ) );
	QString result_str;
	if ( *DEBUG_INFO_ENABLED )
		cerr << "result (childCount: " << resultsl->childCount() << "): " << endl;
	result_str += "<html>\n";
	result_str += "<head>\n";
	result_str += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n";
	result_str += "<title>" + tr ( "Cdcat search result" ) + "</title>\n";
	result_str += "</head>\n";
	result_str += "<body>\n";
	result_str += "<h1>" + tr ( "Cdcat search result" ) + "</h1>\n";
	result_str += tr ( "catalog" ) + ": " + mainw->db->getRootNode()->getNameOf() + ", " + tr ( "created at:" ) + " " + QDateTime::currentDateTime().toLocalTime().toString() + "\n";

	// TODO: include search options in result
	result_str += "<table border=\"1\" title=\"" + tr ( "used search options" ) + "\">\n";
	result_str += "<tr>";

	result_str += "<td>";
	result_str +=  tr ( "search pattern:" ) + " " + leText->text();
	result_str += "</td>";

	result_str += "<td>";
	result_str +=  tr ( "Use easy matching instead regex" ) + ": ";
	if ( cbEasy->isChecked() )
		result_str += tr ( "on" );
	else
		result_str += tr ( "off" );
	result_str += "</td>";

	result_str += "<td>";
	result_str +=  tr ( "Case sensitive" ) + ": ";
	if ( cbCasesens->isChecked() )
		result_str += tr ( "on" );
	else
		result_str += tr ( "off" );
	result_str += "</td>";
	result_str += "</tr>";

	result_str += "<tr>";
	result_str += "<td>";
	result_str += tr ( "Search in" ) + ": ";

	if ( cbDirname->isChecked() )
		result_str += " " + tr ( "Media / Directory name" );
	if ( cbFilename->isChecked() )
		result_str += " " + tr ( "File name" );
	if ( cbComment->isChecked() )
		result_str += " " + tr ( "Comment" );
	if ( cbContent->isChecked() )
		result_str += " " + tr ( "Content" );
	if ( cbArtist->isChecked() )
		result_str += " " + tr ( "mp3-tag Artist" );
	if ( cbTitle->isChecked() )
		result_str += " " + tr ( "mp3-tag Title" );
	if ( cbAlbum->isChecked() )
		result_str += " " + tr ( "mp3-tag Album" );
	if ( cbTcomm->isChecked() )
		result_str += " " + tr ( "mp3-tag Comment" ) ;

	if ( cbFindInArchive->isChecked() )
		result_str += " " + tr ( "archives" );
	result_str += "</td>";

	result_str += "<td>";
	result_str +=  tr ( "Unsharp search (slow)" ) + ": ";
	if ( cbUnsharpSearch->isChecked() )
		result_str += tr ( "on" );
	else
		result_str += tr ( "off" );
	result_str += "</td>";

	if ( cbDateStart->isChecked() || cbDateEnd->isChecked() || cbSizeMin->isChecked() || cbSizeMax->isChecked() ) {
		result_str += "<tr>";
		result_str += "<td>";
		result_str += tr ( "other options" ) + ":";
		if ( cbDateStart->isChecked() ) {
			result_str += " " + tr ( "Date start" ) + ": " + deDateStart->dateTime().toString();
		}

		if ( cbDateEnd->isChecked() ) {
			result_str += " " + tr ( "Date end" ) + ": " + deDateEnd->dateTime().toString();
		}

		if ( cbSizeMin->isChecked() ) {
			result_str +=  " " + tr ( "Min size" ) + ": " + QString().setNum ( spSizeMin->value() ) + cbSizeUnitMin->currentText() ;
		}

		if ( cbSizeMax->isChecked() ) {
			result_str += " " + tr ( "Max size" ) + ": " + QString().setNum ( spSizeMax->value() ) + cbSizeUnitMax->currentText() ;
		}
		result_str += "</td>";
		result_str += "</tr>\n";
	}

	result_str += "</table>\n";

	result_str += "<table border=\"1\">\n";
	result_str += "<tr>";
	result_str += "<th>#</th><th>" + tr ( "Name" ) + "</th>";
	result_str += "<th>" + tr ( "Type" ) + "</th>";
	result_str += "<th>" + tr ( "Size" ) + "</th>";
	result_str += "<th>" + tr ( "Media" ) + "</th>";
	result_str += "<th>" + tr ( "Path" ) + "</th>";
	result_str += "<th>" + tr ( "Modification" ) + "</th>";
	result_str += "<th>" + tr ( "Comment" ) + "</th>";
	result_str += "</tr>\n";
	Q3ListViewItem *lastChild = resultsl->firstChild();
	int i = 0;
	while ( lastChild ) {
		/*
			listView data:
			0: name
			1: type,
			2: size_str,
			3: media,
			4: fullPath
			5: date,
			6: comment (e.g. file inside archive)
		*/
		if ( *DEBUG_INFO_ENABLED ) {
			for ( int j = 0; j <= 6; j++ )
				cerr << "result[" << i << "][" << j << "]: " << qPrintable ( lastChild->text ( j ) ) << endl;
		}

		result_str += "<tr>";
		result_str += "<td align=\"right\" style=\"font-size:-2;\">" + QString().setNum ( i + 1 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 0 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 1 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 2 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 3 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 4 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 5 ) + "</td>";
		result_str += "<td style=\"font-size:-2;\">" + lastChild->text ( 6 ).replace ( "\n", "<br>" ) + "</td>";
		result_str += "</tr>\n";
		if ( *DEBUG_INFO_ENABLED ) {
			cerr << "result_str: " << qPrintable ( result_str ) << endl;
		}
		lastChild = lastChild->nextSibling();
		i++;
	}
	result_str += "</table>\n";
	result_str += "</body>\n";
	result_str += "</html>\n";
//          double xscale = printer.pageRect().width()/double(myWidget->width());
//          double yscale = printer.pageRect().height()/double(myWidget->height());
//          double scale = qMin(xscale, yscale);
//          painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
//                             printer.paperRect().y() + printer.pageRect().height()/2);
//          painter.scale(scale, scale);
//          painter.translate(-width()/2, -height()/2);


	mydoc.setHtml ( result_str );
	if ( isPrint )
		mydoc.print ( &printer );
	else {
		QFile outfile ( output_filename );
		if ( outfile.open ( QIODevice::WriteOnly ) ) {
			outfile.write ( result_str.toUtf8() );
			outfile.close();
		}
		else {
			QMessageBox::critical ( 0, tr ( "File cant open: " ) + output_filename, tr ( "Cant open file" ) );
		}
	}
}

void findDialog::printResultClicked() {
	exportResult ( true );
}
void findDialog::exportResultClicked() {
	exportResult ( false );
}

/***************************************************************************

  Seek engine Class

***************************************************************************/

seekEngine::seekEngine ( findDialog *fdp, bool isFindDuplicates ) {
	this->fd   = fdp;
	patt = new char[2048];
	re = new QRegExp();
	searchForDuplicates = isFindDuplicates;
}

/***************************************************************************/
seekEngine::~seekEngine ( void ) {
	delete [] patt;
	delete re;
}
/***************************************************************************/
int seekEngine::start_seek ( void ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	pww = new PWw ( fd );
	pww->refreshTime = 200;
	pww->setCancel ( true );
	pww->setProgressText ( tr ( "Searching, please wait..." ) );
	QObject::connect ( pww, SIGNAL ( cancelReceivedByUser ( bool ) ), pww, SLOT ( doCancelReceived ( bool ) ) );
	progress ( pww );
	QApplication::setOverrideCursor ( Qt::waitCursor );
	fd->resultsl->clear();
	founded = 0;
	
	if (!searchForDuplicates ) {
		//get the pattern
		if ( fd->cbEasy->isChecked() )
			strncpy ( patt, ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( "*" + fd->leText->text() + "*" ) ), 2047 );
		else
			strncpy ( patt, ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( fd->leText->text() ) ), 2047 );
		
		//recode the pattern /easy/  put ^$ and ? -> .  * -> .*
		if ( fd->cbEasy->isChecked() )
			easyFormConversion ( patt );
		
		//recode the pattern /Case sens/ lok -> [l|L][o|O][k|K]
		if ( !fd->cbCasesens->isChecked() )
			caseSensConversion ( patt );
		if ( *DEBUG_INFO_ENABLED )
			fprintf ( stderr, "The complete pattern is \"%s\"\n", patt );
		//     re = pcre_compile ( patt,0,&error,&errptr,NULL );
		re->setPattern ( QString ( patt ) );
		//     if ( !fd->cbEasy->isChecked() )
		// 	re->setPatternSyntax(QRegExp::Wildcard);
		//     else
		re->setPatternSyntax ( QRegExp::RegExp2 );
		
		//     if ( re == NULL ) {
		if ( !re->isValid() ) {
			QMessageBox::warning ( fd, tr ( "Error in the pattern:" ), QString ( patt ) );
			return 1;
		}
		
		//// this tries to opimize pattern
		//     hints = pcre_study ( re,0,&error );

		//     if ( error != NULL ) {
		//         QMessageBox::warning ( fd,tr ( "Error in the pattern:" ),error );
		//         return 1;
		//     }
		fd->use_unsharpsearch = fd->cbUnsharpSearch->isChecked();
		
		dirname  = fd->cbDirname ->isChecked();
		filename = fd->cbFilename->isChecked();
		comment  = fd->cbComment ->isChecked();
		tartist  = fd->cbArtist  ->isChecked();
		ttitle   = fd->cbTitle   ->isChecked();
		tcomment = fd->cbTcomm   ->isChecked();
		talbum   = fd->cbAlbum   ->isChecked();
		content  = fd->cbContent ->isChecked();
		dateStartChecked  = fd->cbDateStart ->isChecked();
		dateEndChecked  = fd->cbDateEnd ->isChecked();
		dateStart = fd->deDateStart->dateTime();
		dateEnd = fd->deDateEnd->dateTime();
		sizeMinChecked = fd->cbSizeMin->isChecked();
		sizeMaxChecked = fd->cbSizeMax->isChecked();
		findInArchivesChecked = fd->cbFindInArchive->isChecked();
		
		if ( sizeMinChecked ) {
			if ( fd->cbSizeUnitMin->currentIndex() == 0 )
				size_min = fd->spSizeMin->value(); // Byte
			else
				if ( fd->cbSizeUnitMin->currentIndex() == 1 )
					size_min = fd->spSizeMin->value() * 1024; // KByte
				else
					if ( fd->cbSizeUnitMin->currentIndex() == 2 )
						size_min = fd->spSizeMin->value() * 1024 * 1024; // MByte
					else
						if ( fd->cbSizeUnitMin->currentIndex() == 3 )
							size_min = fd->spSizeMin->value() * 1024 * 1024 * 1024; // GByte
						else
							if ( fd->cbSizeUnitMin->currentIndex() == 4 )
								size_min = fd->spSizeMin->value() * 1024 * 1024 * 1024 * 1024; // TByte
			// 	std::cerr << "minsize checked, type "<< fd->cbSizeUnitMin->currentIndex() <<", min size " << size_min << endl;
		}
		
		if ( sizeMaxChecked ) {
			if ( fd->cbSizeUnitMax->currentIndex() == 0 )
				size_max = fd->spSizeMax->value(); // Byte
			else
				if ( fd->cbSizeUnitMax->currentIndex() == 1 )
					size_max = fd->spSizeMax->value() * 1024; // KByte
				else
					if ( fd->cbSizeUnitMax->currentIndex() == 2 )
						size_max = fd->spSizeMax->value() * 1024 * 1024; // MByte
					else
						if ( fd->cbSizeUnitMax->currentIndex() == 3 )
							size_max = fd->spSizeMax->value() * 1024 * 1024 * 1024; // GByte
						else
							if ( fd->cbSizeUnitMax->currentIndex() == 4 )
								size_max = fd->spSizeMax->value() * 1024 * 1024 * 1024 * 1024; // TByte
			// 	std::cerr << "maxsize checked, type "<< fd->cbSizeUnitMax->currentIndex() <<", max size " << size_max << endl;
		}
		
		allmedia = false;
		allowner = false;
		
		media = fd->cbSin->currentText();
		owner = fd->cbOwner->currentText();
		
		if ( 0 == fd->cbOwner->currentItem() )
			allowner = true;
		if ( 0 == fd->cbSin  ->currentItem() )
			allmedia = true;
	}
	
	progress ( pww );
	fd->textLabel5->setText ( tr ( "Results:" )+" "+tr("searching is in progress"));
	/*seek...*/
	analyzeNode ( pww, fd->mainw->db->getRootNode() );
	if ( pww->doCancel ) {
		QMessageBox::warning ( 0, tr ( "Search cancelled" ), tr ( "You have cancelled searching." ) );
	}
	fd->textLabel5->setText ( tr ( "Results:" )+" "+QString().setNum(founded));
	if ( founded == 0 )
		fd->resultsl->insertItem ( new Q3ListViewItem ( fd->resultsl, tr ( "There is no matching." ) ) );
	
	QObject::disconnect ( pww, SIGNAL ( cancelReceivedByUser ( bool ) ), pww, SLOT ( doCancelReceived ( bool ) ) );
	progress ( pww );
	pww->end();
	
	delete pww;
	
	QApplication::restoreOverrideCursor();
	return 0;
}
/***************************************************************************/
int seekEngine::analyzeNode ( PWw *pww,  Node *n, Node *pa ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	
	if (fd->mainw->app->hasPendingEvents())
		fd->mainw->app->processEvents();
	
	progress ( pww );
	if ( n == NULL || pww->doCancel)
		return 0;
	if ( searchForDuplicates ) {
		if ( fd->mainw->guis->standON == NULL ) {
			if ( *DEBUG_INFO_ENABLED )
				std::cout << "standON is NULL! " << std::endl;
		}
		return 0;
	}
	
	if ( searchForDuplicates ) {
		switch ( n->type ) {
			case HC_CATALOG:
				analyzeNode ( pww, n->child );
				return 0;
			case HC_MEDIA:
				progress ( pww );
				analyzeNode ( pww, n->child );
				analyzeNode ( pww, n->next );
				return 0;
			case HC_DIRECTORY:
				progress ( pww );
				analyzeNode ( pww, n->child );
				analyzeNode ( pww, n->next );
				return 0;
			case HC_CATLNK:
				analyzeNode ( pww, n->next );
				return 0;
			case HC_FILE:
				if ( *DEBUG_INFO_ENABLED )
					//std::cout << "testing file: " << qPrintable(n->getNameOf()) << " name: " << qPrintable(fd->mainw->guis->standON->getNameOf()) << " <=> " << qPrintable(n->getNameOf()) << ". size: " <<  (( DBFile * ) ( fd->mainw->guis->standON ))->size << " <=> " << ( ( DBFile * ) ( n->data ) )->size << ", size type: " <<  (( DBFile * ) ( fd->mainw->guis->standON ) )->sizeType << " <=> " << ( ( DBFile * ) ( n->data ) )->sizeType  << std::endl;
					std::cout << "testing file: " << qPrintable ( n->getNameOf() ) << " name: " << qPrintable ( fd->mainw->guis->standON->getNameOf() ) << " <=> " << qPrintable ( n->getNameOf() ) << std::endl;
				if ( fd->mainw->guis->standON->getNameOf() ==  n->getNameOf() && fd->mainw->guis->standON->getFullPath() !=  n->getFullPath() ) {
					if ( *DEBUG_INFO_ENABLED ) {
						std::cout << "filename match!" << std::endl;
						std::cout << "size: " <<  fd->mainw->guis->mainw->db->getSize ( fd->mainw->guis->standON ) << " <=> " << ( ( DBFile * ) ( n->data ) )->size << std::endl;
					}
					if ( fd->mainw->guis->mainw->db->getSize ( fd->mainw->guis->standON ) == ( ( DBFile * ) ( n->data ) )->size )  {
						if ( *DEBUG_INFO_ENABLED )
							std::cout << "filesize match!" << std::endl;
						putNodeToList ( n );
					}
				}
				analyzeNode ( pww, n->next );
				return 0;
		}
	}
	else {
		bool isOk = false;
		QString filecomment;
		switch ( n->type ) {
			case HC_CATALOG:
				analyzeNode ( pww, n->child );
				return 0;
			case HC_MEDIA:
				progress ( pww );
				
				//It is necessary to analyze this media node? /Owner/Media/
				if ( ( allmedia || ( media == n->getNameOf() ) ) &&
				        ( allowner || ( owner == ( ( DBMedia * ) ( n->data ) )->owner ) ) ) {
					if ( dirname ) {
						if ( !matchIt ( n->getNameOf() ) ) {
							isOk = false;
						}
						if ( dateStartChecked && !dateEndChecked ) {
							if ( ( ( DBMedia * ) ( n->data ) )->modification >= dateStart ) {
								isOk = true;
							}
						}
						else
							if ( !dateStartChecked && dateEndChecked ) {
								if ( ( ( DBMedia * ) ( n->data ) )->modification <= dateEnd ) {
									isOk = true;
								}
							}
							else
								if ( dateStartChecked && dateEndChecked ) {
									if ( ( ( DBMedia * ) ( n->data ) )->modification >= dateStart && ( ( DBDirectory * ) ( n->data ) )->modification <= dateEnd ) {
										isOk = true;
									}
								}
						
						if ( isOk ) {
							putNodeToList ( n );
							analyzeNode ( pww, n->child );
							analyzeNode ( pww, n->next );
							return 0;
						}
					}
					
					if ( comment )
						if ( matchIt ( ( ( DBMedia * ) ( n->data ) )->comment ) )
							putNodeToList ( n );
					
					analyzeNode ( pww, n->child );
				}
				analyzeNode ( pww, n->next );
				return 0;
			
			case HC_CATLNK:
				analyzeNode ( pww, n->next );
				return 0;
			
			case HC_DIRECTORY:
				progress ( pww );
				isOk = true;
				if ( dirname ) {
					if ( !matchIt ( n->getNameOf() ) ) {
						isOk = false;
					}
					if ( dateStartChecked && !dateEndChecked ) {
						if ( ( ( DBDirectory * ) ( n->data ) )->modification < dateStart ) {
							isOk = false;
						}
					}
					else
						if ( !dateStartChecked && dateEndChecked ) {
							if ( ( ( DBDirectory * ) ( n->data ) )->modification > dateEnd ) {
								isOk = false;
							}
						}
						else
							if ( dateStartChecked && dateEndChecked ) {
								if ( ( ( DBDirectory * ) ( n->data ) )->modification > dateStart && ( ( DBDirectory * ) ( n->data ) )->modification > dateEnd ) {
									isOk = false;
								}
							}
					
					if ( isOk ) {
						putNodeToList ( n );
						analyzeNode ( pww, n->child );
						analyzeNode ( pww, n->next );
						return 0;
					}
				}
				
				if ( comment ) {
					if ( matchIt ( ( ( DBDirectory * ) ( n->data ) )->comment ) ) {
						putNodeToList ( n );
					}
				}
				
				analyzeNode ( pww, n->child );
				progress ( pww );
				analyzeNode ( pww, n->next );
				return 0;
			
			case HC_FILE:
				isOk = true;
				filecomment = "";
				if ( filename ) {
					if ( !matchIt ( n->getNameOf() ) ) {
						isOk = false;
					}
				}
				if ( isOk && dateStartChecked && !dateEndChecked ) {
					if ( ( ( DBFile * ) ( n->data ) )->modification < dateStart ) {
						isOk = false;
					}
				}
				else
					if ( isOk &&  !dateStartChecked && dateEndChecked ) {
						if ( ( ( DBFile * ) ( n->data ) )->modification > dateEnd ) {
							isOk = false;
						}
					}
					else
						if ( isOk &&  dateStartChecked && dateEndChecked ) {
							if ( ( ( DBFile * ) ( n->data ) )->modification < dateStart || ( ( DBFile * ) ( n->data ) )->modification > dateEnd ) {
								isOk = false;
							}
						}
				
				if ( isOk && sizeMinChecked && !sizeMaxChecked ) {
					float real_size = 0.0;
					float real_size_min = 0.0;
					switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
						case 0:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0 * 1024.0 );
							real_size_min = size_min;
							break; //byte
						case 1:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / 1024.0;
							real_size_min = size_min / 1024.0;
							break; //Kb
						case 2:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size );
							real_size_min = size_min / 1024.0 / 1024.0;
							break; //Mb
						case 3:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0;
							real_size_min = size_min / 1024.0 / 1024.0 / 1024.0;
							break; //Gb
						case 4:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0 * 1024.0;
							real_size_min = size_min / 1024.0 / 1024.0 / 1024.0 / 1024.0;
							break; //Tb
					}
					// 		std::cerr << "minsize checked, min size " << real_size_min << " ~ " << real_size<< endl;
					if ( real_size < real_size_min ) {
						isOk = false;
					}
				}
				if ( isOk && !sizeMinChecked && sizeMaxChecked ) {
					float real_size = 0.0;
					float real_size_max = 0.0;
					switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
						case 0:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0 * 1024.0 );
							real_size_max = size_max;
							break; //byte
						case 1:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / 1024.0;
							real_size_max = size_max / 1024.0;
							break; //Kb
						case 2:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size );
							real_size_max = size_max / 1024.0 / 1024.0;
							break; //Mb
						case 3:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0;
							real_size_max = size_max / 1024.0 / 1024.0 / 1024.0;
							break; //Gb
						case 4:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0 * 1024.0;
							real_size_max = size_max / 1024.0 / 1024.0 / 1024.0 / 1024.0;
							break; //Tb
					}
					// 		std::cerr << "size type: "<<  ( ( DBFile * ) ( n->data ) )->sizeType <<", maxsize checked, max size " << real_size_max << " ~ " << real_size << endl;
					if ( real_size  > real_size_max ) {
						isOk = false;
					}
				}
				if ( isOk && sizeMinChecked && sizeMaxChecked ) {
					float real_size = 0.0;
					float real_size_min = 0.0;
					float real_size_max = 0.0;
					switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
						case 0:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0 * 1024.0 );
							real_size_min = size_min;
							real_size_max = size_max;
							break; //byte
						case 1:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / 1024.0;
							real_size_min = size_min / 1024.0;
							real_size_max = size_max / 1024.0;
							break; //Kb
						case 2:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size );
							real_size_min = size_min / 1024.0 / 1024.0;
							real_size_max = size_max / 1024.0 / 1024.0;
							break; //Mb
						case 3:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0;
							real_size_min = size_min / 1024.0 / 1024.0 / 1024.0;
							real_size_max = size_max / 1024.0 / 1024.0 / 1024.0;
							break; //Gb
						case 4:
							real_size = ( ( ( DBFile * ) ( n->data ) )->size ) * 1024.0 * 1024.0;
							real_size_min = size_min / 1024.0 / 1024.0 / 1024.0 / 1024.0;
							real_size_max = size_max / 1024.0 / 1024.0 / 1024.0 / 1024.0;
							break; //Tb
					}
					// 		std::cerr << "min & maxsize checked, min size " << real_size_min << "/max size " << real_size_max << " ~ " << real_size << endl;
					if ( real_size < real_size_min || real_size > real_size_max ) {
						isOk = false;
					}
				}
				if ( findInArchivesChecked ) {
					// FIXME maybe a flag 'is_archive' should be added
					bool archiveMatches = false;
					filecomment = "";
					QList<ArchiveFile> ArchiveFileList = ( ( DBFile * ) ( n->data ) )->archivecontent;
					for ( int i = 0; i < ArchiveFileList.size(); i++ ) {
						ArchiveFile af = ArchiveFileList.at ( i );
						QString filepath = af.path.section ( '/', -1, -1 );
						if ( *DEBUG_INFO_ENABLED )
							std::cout << "testing file inside archive: " << qPrintable ( filepath ) << std::endl;

						for ( int i = 0; i < ArchiveFileList.size(); i++ ) {
							ArchiveFile af = ArchiveFileList.at ( i );
							QString filepath = af.path.section ( '/', -1, -1 );
							if ( *DEBUG_INFO_ENABLED )
								std::cout << "testing file inside archive: " << qPrintable ( filepath ) << std::endl;

							if ( filename ) {
								if ( matchIt ( filepath ) ) {
									archiveMatches = true;
									filecomment = tr ( "File in archive: " ) + af.path;
									//break;
								}
							}
							else
								if ( dateStartChecked && !dateEndChecked ) {
									if ( af.date > dateStart ) {
										archiveMatches = true;
										filecomment = tr ( "File in archive: " ) + af.path;
										//break;
									}
									else {
										archiveMatches = false;
									}
								}
								else
									if ( !dateStartChecked && dateEndChecked ) {
										if ( af.date < dateEnd ) {
											archiveMatches = true;
											filecomment = tr ( "File in archive: " ) + af.path;
											//break;
										}
										else {
											archiveMatches = false;
										}
									}
									else
										if ( dateStartChecked && dateEndChecked ) {
											if ( af.date > dateStart || af.date < dateEnd ) {
												archiveMatches = true;
												filecomment = tr ( "File in archive: " ) + af.path;
												//break;
											}
											else {
												archiveMatches = false;
											}
										}
						}
						if ( archiveMatches )
							isOk = true;
					}
				}
				if ( isOk ) {
					putNodeToList ( n, filecomment );
					analyzeNode ( pww, n->next );
					return 0;
				}
				
				if ( comment ) {
					if ( !matchIt ( ( ( DBFile * ) ( n->data ) )->comment ) ) {
						isOk = false;
					}
					if ( isOk ) {
						putNodeToList ( n );
						analyzeNode ( pww,  n->next );
						return 0;
					}
				}
				
				analyzeNode ( pww, ( ( DBFile * ) ( n->data ) )->prop, n );
				analyzeNode ( pww,  n->next );
				return 0;
			
			case HC_MP3TAG:
				if ( tartist )
					if ( matchIt ( ( ( DBMp3Tag * ) ( n->data ) )->artist ) ) {
						putNodeToList ( pa );
						return 0;
					}
				if ( ttitle )
					if ( matchIt ( ( ( DBMp3Tag * ) ( n->data ) )->title ) ) {
						putNodeToList ( pa );
						return 0;
					}
				if ( talbum )
					if ( matchIt ( ( ( DBMp3Tag * ) ( n->data ) )->album ) ) {
						putNodeToList ( pa );
						return 0;
					}
				if ( tcomment )
					if ( matchIt ( ( ( DBMp3Tag * ) ( n->data ) )->comment ) ) {
						putNodeToList ( pa );
						return 0;
					}
				
				analyzeNode ( pww, n->next, pa );
				return 0;
			case HC_CONTENT:
				if ( content )
					if ( matchIt ( QString::fromLocal8Bit ( ( const char * ) ( ( DBContent * ) ( n->data ) )->bytes ) ) ) {
						putNodeToList ( pa );
						return 0;
					}
				analyzeNode ( pww,  n->next, pa );
				return 0;
		}
		return -1;
	}
	return -1;
}


int seekEngine::matchIt ( QString txt ) {
	//const char *encoded;
	int  match;
	if ( txt.isEmpty() )
		return 0;
	if ( !fd->use_unsharpsearch ) {
		//encoded = ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( txt ) );
		//match   = pcre_exec ( re,hints,encoded,strlen ( encoded ),0,0,offsets,99 );
		//match = re->exactMatch (QString( encoded));
		match = re->exactMatch ( QString ( txt ) );
		//cerr << "matchit: " << qPrintable(txt) << " <==> " << qPrintable(re->pattern()) <<" result: " << match << endl;
		
		if ( match == 1 )
			return 1;
	}
	else {
		if ( fd->cbCasesens->isChecked() ) {
			if ( matchUnsharp ( fd->leText->text().toLocal8Bit().data(), txt.toLocal8Bit().data() ) )
				return 1;
		}
		else {
			if ( matchUnsharp ( fd->leText->text().toLower().toLocal8Bit().data(), txt.toLower().toLocal8Bit().data() ) )
				return 1;
		}
	}
	return 0;
}

bool seekEngine::matchUnsharp ( char* matchpattern, char* str ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( matchpattern == NULL || str == NULL )
		return false;
	
	int match = -1;
	int matchpattern_len = strlen ( matchpattern );
	MString m_matchpattern ( matchpattern, matchpattern_len );
	QString res_matchpattern_str1, res_matchpattern_str2;
	m_matchpattern.DoubleMetaphone ( res_matchpattern_str1, res_matchpattern_str2 );

	MString m_str ( str, matchpattern_len );
	QString res_str_str1, res_str_str2;
	m_str.DoubleMetaphone ( res_str_str1, res_str_str2 );
	
	match = res_str_str1.indexOf ( res_matchpattern_str1 );
	
	if ( *DEBUG_INFO_ENABLED )
		cerr << "matchUnsharp: " << matchpattern << " (" << qPrintable ( res_matchpattern_str1 ) << ") <=> " << str << " (" << qPrintable ( res_str_str1 ) << ")  ===> " << match << endl;
	
	if ( match > -1 )
		return true;
	else
		return false;
}

void seekEngine::putNodeToList ( Node *n, QString comment ) {
	Node *tmp = NULL;
	QString   type;
	QString   size_str = "";
	QString   media = "";
	QDateTime mod;
	if ( n == NULL )
		return;
	
	DEBUG_INFO_ENABLED = init_debug_info();
	
	switch ( n->type ) {
		case HC_MEDIA:
			type = tr ( "media" );
			mod  = ( ( DBMedia * ) ( n->data ) )->modification;
			comment  = ( ( DBMedia * ) ( n->data ) )->comment;
			break;
		case HC_DIRECTORY:
			type = tr ( "dir" );
			mod  = ( ( DBDirectory * ) ( n->data ) )->modification;
			comment  = ( ( DBDirectory * ) ( n->data ) )->comment;
			break;
		case HC_FILE:
			type = tr ( "file" );
			size_str = QString().sprintf ( "%.1f", ( ( DBFile * ) ( n->data ) )->size );
			size_str += QString ( getSType ( ( ( DBFile * ) ( n->data ) )->sizeType, true ) );
			mod  = ( ( DBFile * ) ( n->data ) )->modification;
			comment  = ( ( DBFile * ) ( n->data ) )->comment;
			break;
		default:
			type = tr ( "error" );
			break;
	}
	tmp = n;
	if ( *DEBUG_INFO_ENABLED )
		std::cout << "putNodeToList: path: " << qPrintable(n->getFullPath()) << ",  type: " << qPrintable(type) << std::endl;
	while ( tmp->type != HC_MEDIA ) {
		tmp = tmp->parent;
		if (fd->mainw->app->hasPendingEvents())
				fd->mainw->app->processEvents();
		progress ( pww );
	}

	media = tmp->getNameOf() + "/" + QString().setNum ( ( ( DBMedia * ) ( tmp->data ) )->number );
	Q3ListViewItem *newitem;
	newitem = new Q3ListViewItem ( fd->resultsl, n->getNameOf(), type, size_str, media, n->getFullPath(), date_to_str ( mod ), comment );
	
	newitem->setMultiLinesEnabled ( true );
	fd->resultsl->insertItem ( newitem );
	progress ( pww );
	founded++;
}


// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
