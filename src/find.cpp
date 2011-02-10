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
#include <malloc.h>
#include "dmetaph.h"
#include <iostream>
using namespace std;

findDialog::findDialog ( CdCatMainWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl )

{
    if ( !name )
        setName ( "findDialog" );
    setIcon ( *get_t_find_icon() );

    mainw = parent;
    use_unsharpsearch = false;

    setSizeGripEnabled ( TRUE );
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

    leText = new QLineEdit ( this, "leText" );
    
    deDateStart = new QDateTimeEdit ( QDateTime(QDate(1,1,1900)) ,this );
    deDateEnd = new QDateTimeEdit ( QDateTime (QDate().currentDate()), this );

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
    cbSizeMin = new QCheckBox (this, "cbSizeMin" );
    cbSizeMax = new QCheckBox (this, "cbSizeMax" );
    cbUnsharpSearch = new QCheckBox (this, "cbUnsharpSearch" );

    cbOwner = new QComboBox ( FALSE, this, "cbOwner" );
    cbOwner->setMinimumSize ( QSize ( 0, 0 ) );

    cbSin = new QComboBox ( FALSE, this, "cbSin" );

    cbSizeUnitMin = new QComboBox (false, this, "cbSizeUnitMin");
    cbSizeUnitMin->addItem(tr("Byte"));
    cbSizeUnitMin->addItem(tr("KiB"));
    cbSizeUnitMin->addItem(tr("MiB"));
    cbSizeUnitMin->addItem(tr("GiB"));

    cbSizeUnitMax = new QComboBox (false, this, "cbSizeUnitMax");
    cbSizeUnitMax->addItem(tr("Byte"));
    cbSizeUnitMax->addItem(tr("KiB"));
    cbSizeUnitMax->addItem(tr("MiB"));
    cbSizeUnitMax->addItem(tr("GiB"));

    spSizeMin = new QSpinBox( this, "spSizeMin");
    spSizeMin->setMinimum(1);
    spSizeMin->setMaximum(10000000);

    spSizeMax = new QSpinBox( this, "spSizeMax");
    spSizeMax->setMinimum(1);
    spSizeMax->setMaximum(10000000);

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

    resultsl->setColumnAlignment(2, Qt::AlignRight );
    resultsl->setColumnWidthMode(0, Q3ListView::Maximum);

    buttonClose = new QPushButton ( this, "buttonClose" );

    textLabel3 = new QLabel ( this, "textLabel3" );
    textLabel1 = new QLabel ( this, "textLabel1" );
    textLabel2 = new QLabel ( this, "textLabel2" );
    textLabel5 = new QLabel ( this, "textLabel5" );
    textLabel6 = new QLabel ( this, "textLabel6" );
    textLabel7 = new QLabel ( this, "textLabel7" );

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
    cbSizeMin -> setChecked( mainw->cconfig->find_size_min );
    cbSizeMax -> setChecked( mainw->cconfig->find_size_max );
    

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
    cbSizeUnitMin->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout37->addItem(layout_size_min, 8, 1);

    layout_size_max->addWidget ( spSizeMax );
    layout_size_max->addWidget ( cbSizeUnitMax );
    cbSizeUnitMin->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout37->addItem(layout_size_max, 9, 1);


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
    layout16->addItem ( spacer_10, 0, 0 );

    layout31->addLayout ( layout17 );
    layout31->addLayout ( layout30 );
    layout31->addWidget ( resultsl );
    layout31->addLayout ( layout16 );

    layout40->addLayout ( layout39 );
    layout40->addLayout ( layout31 );

    FindDialogBaseLayout->addLayout ( layout40, 0, 0 );

    resize ( QSize ( mainw->cconfig->findWidth,mainw->cconfig->findHeight ).expandedTo ( minimumSizeHint() ) );
    move ( mainw->cconfig->findX,mainw->cconfig->findY );

    languageChange();

    fillCBox();
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( cancele() ) );
    connect ( buttonOk,SIGNAL ( clicked() ),this,SLOT ( seeke() ) );
    connect ( buttonClose,SIGNAL ( clicked() ),this,SLOT ( closee() ) );
    connect ( resultsl,SIGNAL ( currentChanged ( Q3ListViewItem * ) ),this,SLOT ( select ( Q3ListViewItem * ) ) );
    connect ( resultsl,SIGNAL ( clicked ( Q3ListViewItem * ) ),this,SLOT ( select ( Q3ListViewItem * ) ) );
    connect ( cbDateStart, SIGNAL ( stateChanged(int)),this,SLOT ( dateStartChanged(int)));
    connect ( cbDateEnd, SIGNAL ( stateChanged(int)),this,SLOT ( dateEndChanged(int)));
    //connect ( deDateStart, SIGNAL ( clicked()),this,SLOT ( dateStartDoubleClicked()));
    //connect ( deDateEnd, SIGNAL ( ),this,SLOT ( dateEndDoubleClicked()));
    connect ( cbSizeMin, SIGNAL ( clicked()),this,SLOT ( sizeMinClicked()));
    connect ( cbSizeMax, SIGNAL ( clicked()),this,SLOT ( sizeMaxClicked()));


    deDateStart->setEnabled(false);
    deDateEnd->setEnabled(false);
    cbSizeUnitMin->setEnabled(false);
    spSizeMin->setEnabled(false);

    cbSizeUnitMax->setEnabled(false);
    spSizeMax->setEnabled(false);

    deDateStart->setDateTime(mainw->cconfig->find_date_start_val);
    deDateEnd->setDateTime(mainw->cconfig->find_date_end_val);

    deDateStart->setDisplayFormat("dd.MM.yyyy hh:mm");
    deDateEnd->setDisplayFormat("dd.MM.yyyy hh:mm");

    cbSizeMin->setChecked(mainw->cconfig->find_size_min);
    cbSizeMax->setChecked(mainw->cconfig->find_size_max);
    spSizeMin->setValue(mainw->cconfig->find_size_min_val);
    spSizeMax->setValue(mainw->cconfig->find_size_max_val);

    spSizeMin->setEnabled(mainw->cconfig->find_size_min);
    spSizeMax->setEnabled(mainw->cconfig->find_size_max);

    cbSizeUnitMin->setEnabled(mainw->cconfig->find_size_min);
    cbSizeUnitMax->setEnabled(mainw->cconfig->find_size_max);

    cbSizeUnitMin->setCurrentIndex(mainw->cconfig->find_size_unit_min_val);
    cbSizeUnitMax->setCurrentIndex(mainw->cconfig->find_size_unit_max_val);

    cbUnsharpSearch->setChecked(mainw->cconfig->find_unsharp_search);

    leText->setText(mainw->cconfig->lastSearchPattern);

    dateStartChanged(0);
    dateEndChanged(0);

    setTabOrder ( leText,buttonOk );
    leText->setFocus();
}

/*
 *  Destroys the object and frees any allocated resources
 */
findDialog::~findDialog() {
    // no need to delete child widgets, Qt does it all for us
}
/***************************************************************************/
void findDialog::languageChange() {
    setCaption ( tr ( "Search in the database..." ) );
    textLabel1->setText ( tr ( "Seek in:" ) );
    textLabel2->setText ( tr ( "Owner:" ) );
    resultsl->header()->setLabel ( 0, tr ( "Name" ) );
    resultsl->header()->setLabel ( 1, tr ( "Type" ) );
    resultsl->header()->setLabel ( 2, tr ( "Size" ) );
    resultsl->header()->setLabel ( 3, tr ( "Media" ) );
    resultsl->header()->setLabel ( 4, tr ( "Path" ) );
    resultsl->header()->setLabel ( 5, tr ( "Modification" ) );
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
    cbSizeMin->setText( tr("Min size") );
    cbSizeMax->setText( tr("Max size") );
    cbUnsharpSearch->setText( tr("Unsharp search (slow)") );
    buttonOk->setText ( tr ( "&Start search" ) );
#ifndef _WIN32
    buttonOk->setAccel ( QKeySequence ( QString::null ) );
#endif
    buttonCancel->setText ( tr ( "&Cancel" ) );
#ifndef _WIN32
    buttonCancel->setAccel ( QKeySequence ( QString::null ) );
#endif
    textLabel5->setText ( tr ( "Results" ) );
    buttonClose->setText ( tr ( "Close / Go to selected" ) );
    cbCasesens->setText ( tr ( "Case sensitive" ) );
    cbEasy->setText ( tr ( "Use easy matching instead regex" ) );
    resultsl->clear();
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
    mainw->cconfig->find_size_unit_min_val= cbSizeUnitMin->currentIndex();
    mainw->cconfig->find_size_unit_max_val = cbSizeUnitMax->currentIndex();
    mainw->cconfig->find_unsharp_search = cbUnsharpSearch->isChecked();
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
    int  i,f,c;
    Node *tmp=mainw->db->getRootNode();

    if ( tmp == NULL ) return 0;

    cbOwner->clear();
    cbOwner->insertItem ( tr ( "All/Everybody" ),0 );
    cbOwner->insertItem ( ( ( DBCatalog * ) ( tmp->data ) )->owner );

    cbSin  ->clear();
    cbSin  ->insertItem ( tr ( "All media" ),0 );

    tmp=tmp->child; //Jump to the first media
    while ( tmp != NULL ) {
        if ( tmp->type != HC_MEDIA ) {
            tmp=tmp->next;
            continue;
        }

        cbSin  ->insertItem ( tmp->getNameOf() );
        c = cbOwner->count();
        for ( i=0,f=1;c>i;i++ )
            if ( ( ( DBMedia * ) ( tmp->data ) )->owner == cbOwner->text ( i ) )
                f=0;
        if ( f )
            cbOwner->insertItem ( ( ( DBMedia * ) ( tmp->data ) )->owner );
        tmp=tmp->next;
    }
    return 0;
}
/***************************************************************************/

int findDialog::select ( Q3ListViewItem *i ) {
    if ( i == NULL )
        return 0;

    if ( i->text ( 3 ).isEmpty() ) //Not a real result ("There is no matching" label)
        return 0;

   QString nodepath = i->text ( 4 ).mid(2,i->text ( 4 ).length()-1);
//    std::cerr << "select: nodepath " << qPrintable(nodepath) << std::endl; 

    mainw->guis->updateListFromNode (
        ( mainw->guis->getNodeFromFullName (mainw->db->getRootNode(), nodepath ) ) );
    for ( Q3ListViewItemIterator it=mainw->listView->firstChild();it.current();it++ ) {
        if ( ( it.current() )->text ( 0 ) == i->text ( 0 )) {
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

    if ( ( leText->text() ).isEmpty() && !cbDateStart->isChecked() && !cbDateEnd->isChecked() && !cbSizeMin->isChecked() && !cbSizeMax->isChecked() )
        return 0;

    se = new seekEngine ( this );
    se->start_seek();
    delete se;
    return 0;
}

void findDialog::dateStartChanged(int) {
	if (cbDateStart->isChecked())
		deDateStart->setEnabled(true);
	else
		deDateStart->setEnabled(false);
}

void findDialog::dateEndChanged(int) {
	if (cbDateEnd->isChecked())
		deDateEnd->setEnabled(true);
	else
		deDateEnd->setEnabled(false);
}

void findDialog::dateStartDoubleClicked() {
	DEBUG_INFO_ENABLED = init_debug_info();
	if (*DEBUG_INFO_ENABLED)
		std::cerr <<"dateStartDoubleClicked" <<endl;
}

void findDialog::dateEndDoubleClicked() {
}

void findDialog::sizeMinClicked() {
	if(cbSizeMin->isChecked()) {
		cbSizeUnitMin->setEnabled(true);
		spSizeMin->setEnabled(true);
	}
	else {
		cbSizeUnitMin->setEnabled(false);
		spSizeMin->setEnabled(false);
	}
}

void findDialog::sizeMaxClicked() {
	if(cbSizeMax->isChecked()) {
		cbSizeUnitMax->setEnabled(true);
		spSizeMax->setEnabled(true);
	}
	else {
		cbSizeUnitMax->setEnabled(false);
		spSizeMax->setEnabled(false);
	}
}

/***************************************************************************

  Seek engine Class

***************************************************************************/

seekEngine::seekEngine ( findDialog *fdp ) {
    fd   = fdp;
    patt = new char[2048];
    re = new QRegExp();
}
/***************************************************************************/
seekEngine::~seekEngine ( void ) {
    delete [] patt;
    delete re;
}
/***************************************************************************/
int seekEngine::start_seek ( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    //get the pattern
    if ( fd->cbEasy->isChecked() )
	strncpy ( patt, ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( "*"+fd->leText->text()+"*" ) ), 2047 );
    else
        strncpy ( patt, ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( fd->leText->text() ) ), 2047 );

    //recode the pattern /easy/  put ^$ and ? -> .  * -> .*
    if ( fd->cbEasy->isChecked() )
        easyFormConversion ( patt );

    //recode the pattern /Case sens/ lok -> [l|L][o|O][k|K]
    if ( !fd->cbCasesens->isChecked() )
        caseSensConversion ( patt );
    if(*DEBUG_INFO_ENABLED)
	fprintf(stderr,"The complete pattern is \"%s\"\n",patt);
//     re = pcre_compile ( patt,0,&error,&errptr,NULL );
    re->setPattern(QString( patt));
//     if ( !fd->cbEasy->isChecked() )
// 	re->setPatternSyntax(QRegExp::Wildcard);
//     else
	re->setPatternSyntax(QRegExp::RegExp2);

//     if ( re == NULL ) {
       if(!re->isValid()) {
        QMessageBox::warning ( fd,tr ( "Error in the pattern:" ),QString(patt) );
        return 1;
    }

    //// this tries to opimize pattern
//     hints = pcre_study ( re,0,&error );

//     if ( error != NULL ) {
//         QMessageBox::warning ( fd,tr ( "Error in the pattern:" ),error );
//         return 1;
//     }
    fd->use_unsharpsearch = fd->cbUnsharpSearch->isChecked();

    pww=new PWw ( fd );
    pww->refreshTime=200;
    pww->setCancel(true);
    pww->setProgressText(tr("Searching, please wait..."));
    QObject::connect (pww, SIGNAL(cancelReceivedByUser(bool)), pww, SLOT(doCancelReceived(bool)));
    progress ( pww );
    QApplication::setOverrideCursor ( Qt::waitCursor );

    fd->resultsl->clear();
    founded = 0;

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

    if (sizeMinChecked) {
	if(fd->cbSizeUnitMin->currentIndex() == 0)
		size_min = fd->spSizeMin->value(); // Byte
	else if(fd->cbSizeUnitMin->currentIndex() == 1)
		size_min = fd->spSizeMin->value() * 1024; // KByte
	else if(fd->cbSizeUnitMin->currentIndex() == 2)
		size_min = fd->spSizeMin->value() * 1024 * 1024; // MByte
	else if(fd->cbSizeUnitMin->currentIndex() == 3)
		size_min = fd->spSizeMin->value() * 1024 * 1024 * 1024; // GByte
// 	std::cerr << "minsize checked, type "<< fd->cbSizeUnitMin->currentIndex() <<", min size " << size_min << endl;
    }
    
    if (sizeMaxChecked) {
	if(fd->cbSizeUnitMax->currentIndex() == 0)
		size_max = fd->spSizeMax->value(); // Byte
	else if(fd->cbSizeUnitMax->currentIndex() == 1)
		size_max = fd->spSizeMax->value() * 1024; // KByte
	else if(fd->cbSizeUnitMax->currentIndex() == 2)
		size_max = fd->spSizeMax->value() * 1024 * 1024; // MByte
	else if(fd->cbSizeUnitMax->currentIndex() == 3)
		size_max = fd->spSizeMax->value() * 1024 * 1024 * 1024; // GByte
// 	std::cerr << "maxsize checked, type "<< fd->cbSizeUnitMax->currentIndex() <<", max size " << size_max << endl;
    }

    allmedia = false;
    allowner = false;

    media=fd->cbSin->currentText();
    owner=fd->cbOwner->currentText();

    if ( 0 == fd->cbOwner->currentItem() )   allowner = true;
    if ( 0 == fd->cbSin  ->currentItem() )   allmedia = true;

    progress ( pww );
    /*seek...*/
    analyzeNode ( pww, fd->mainw->db->getRootNode() );
    if(pww->doCancel) {
	QMessageBox::warning ( 0,tr ( "Search cancelled" ), tr ( "You have cancelled searching." ));
    }

    if ( founded == 0 )
        fd->resultsl->insertItem ( new Q3ListViewItem ( fd->resultsl,tr ( "There is no matching." ) ) );

    QObject::disconnect (pww, SIGNAL(cancelReceivedByUser(bool)), pww, SLOT(doCancelReceived(bool)));
    progress ( pww );
    pww->end();

    delete pww;
    QApplication::restoreOverrideCursor();
    return 0;
}
/***************************************************************************/
int seekEngine::analyzeNode (PWw *pww,  Node *n,Node *pa ) {

    progress(pww);
    if ( n == NULL || pww->doCancel ) return 0;

    bool isOk=false;
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
		if ( dateStartChecked && !dateEndChecked) {
			if (  (( DBMedia * )(n->data  ))->modification >= dateStart  ) {
				isOk = true;
			}
		}
		else if ( !dateStartChecked && dateEndChecked) {
			if (  (( DBMedia * )(n->data  ))->modification <= dateEnd ) {
				isOk = true;
			}
		}
		else if ( dateStartChecked && dateEndChecked) {
			if (  (( DBMedia * )(n->data  ))->modification >= dateStart && (( DBDirectory * )(n->data  ))->modification <= dateEnd ) {
				isOk = true;
			}
		}

		if (isOk) {
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
	    if ( dateStartChecked && !dateEndChecked) {
		if (  (( DBDirectory * )(n->data  ))->modification < dateStart  ) {
				isOk = false;
		}
	    }
	    else if ( !dateStartChecked && dateEndChecked) {
		if (  (( DBDirectory * )(n->data  ))->modification > dateEnd ) {
			isOk = false;
		}
	    }
	    else if ( dateStartChecked && dateEndChecked) {
		if (  (( DBDirectory * )(n->data  ))->modification > dateStart && (( DBDirectory * )(n->data  ))->modification > dateEnd ) {
			isOk = false;
		}
	    }

	   if (isOk) {
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
	progress(pww);
        analyzeNode ( pww, n->next );
        return 0;

    case HC_FILE:
        isOk = true;
        if ( filename ) {
            if ( !matchIt ( n->getNameOf() ) ) {
			isOk = false;
	     }
	}
	if ( isOk && dateStartChecked && !dateEndChecked) {
		if (  (( DBFile * )(n->data  ))->modification < dateStart  ) {
			isOk = false;
		}
	}
	else if (isOk &&  !dateStartChecked && dateEndChecked) {
		if (  (( DBFile * )(n->data  ))->modification > dateEnd ) {
			isOk = false;
		}
	}
	else if ( isOk &&  dateStartChecked && dateEndChecked) {
		if (  (( DBFile * )(n->data  ))->modification < dateStart || (( DBFile * )(n->data  ))->modification > dateEnd ) {
			isOk = false;
		}
	}
	
	if ( isOk && sizeMinChecked && !sizeMaxChecked) {
		float real_size = 0.0;
		float real_size_min = 0.0;
		switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
			case 0: 
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0*1024.0 );
				real_size_min = size_min;
				break; //byte
			case 1:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) /1024.0;
				real_size_min = size_min / 1024.0;
				break; //Kb
			case 2:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size );
				real_size_min = size_min / 1024.0 / 1024.0;
				break; //Mb
			case 3:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) *1024.0;
				real_size_min = size_min / 1024.0 / 1024.0/1024.0;
				break; //Gb
		}
// 		std::cerr << "minsize checked, min size " << real_size_min << " ~ " << real_size<< endl;
		if (  real_size < real_size_min ) {
			isOk = false;
		}
	}
	if ( isOk && !sizeMinChecked && sizeMaxChecked) {
		float real_size = 0.0;
		float real_size_max = 0.0;
		switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
			case 0: 
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0*1024.0 );
				real_size_max = size_max;
				break; //byte
			case 1:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) /1024.0;
				real_size_max = size_max / 1024.0;
				break; //Kb
			case 2:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size );
				real_size_max = size_max / 1024.0 / 1024.0;
				break; //Mb
			case 3:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) *1024.0;
				real_size_max = size_max / 1024.0 / 1024.0/1024.0;
				break; //Gb
		}
// 		std::cerr << "size type: "<<  ( ( DBFile * ) ( n->data ) )->sizeType <<", maxsize checked, max size " << real_size_max << " ~ " << real_size << endl;
		if ( real_size  > real_size_max ) {
			isOk = false;
		}
	}
	if ( isOk && sizeMinChecked && sizeMaxChecked) {
		float real_size = 0.0;
		float real_size_min = 0.0;
		float real_size_max = 0.0;
		switch ( ( ( DBFile * ) ( n->data ) )->sizeType ) {
			case 0: 
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) / ( 1024.0*1024.0 );
				real_size_min = size_min;
				real_size_max = size_max;
				break; //byte
			case 1:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) /1024.0;
				real_size_min = size_min / 1024.0;
				real_size_max = size_max / 1024.0;
				break; //Kb
			case 2:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size );
				real_size_min = size_min / 1024.0 / 1024.0;
				real_size_max = size_max / 1024.0 / 1024.0;
				break; //Mb
			case 3:
				real_size = ( ( ( DBFile * ) ( n->data ) )->size ) *1024.0;
				real_size_min = size_min / 1024.0 / 1024.0/1024.0;
				real_size_max = size_max / 1024.0 / 1024.0/1024.0;
				break; //Gb
		}
// 		std::cerr << "min & maxsize checked, min size " << real_size_min << "/max size " << real_size_max << " ~ " << real_size << endl;
		if ( real_size < real_size_min || real_size > real_size_max ) {
			isOk = false;
		}
	}
	if(isOk){
		putNodeToList ( n );
		analyzeNode ( pww, n->next );
		return 0;
	}
	
        if ( comment ) {
            if ( !matchIt ( ( ( DBFile * ) ( n->data ) )->comment ) ) {
			isOk = false;
	    }
	    if (isOk) {
		putNodeToList ( n );
		analyzeNode (pww,  n->next );
		return 0;
	    }
	  }
	
        analyzeNode ( pww, ( ( DBFile * ) ( n->data ) )->prop,n );
        analyzeNode (pww,  n->next );
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

        analyzeNode ( pww, n->next,pa );
        return 0;
    case HC_CONTENT:
        if ( content )
            if ( matchIt ( QString::fromLocal8Bit ( ( const char * ) ( ( DBContent * ) ( n->data ) )->bytes ) ) ) {
                putNodeToList ( pa );
                return 0;
            }
        analyzeNode (pww,  n->next,pa );
        return 0;

    }
    return -1;
}

int seekEngine::matchIt ( QString txt ) {
	//const char *encoded;
	int  match;
	if ( txt.isEmpty() )
		return 0;
	if(!fd->use_unsharpsearch) {
		//encoded = ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( txt ) );
		//match   = pcre_exec ( re,hints,encoded,strlen ( encoded ),0,0,offsets,99 );
		//match = re->exactMatch (QString( encoded));
		match = re->exactMatch (QString( txt));
		//cerr << "matchit: " << qPrintable(txt) << " <==> " << qPrintable(re->pattern()) <<" result: " << match << endl;
		
		if ( match == 1 )
			return 1;
	}
	else {
		if (fd->cbCasesens->isChecked()) {
			if (matchUnsharp(fd->leText->text().toLocal8Bit().data(), txt.toLocal8Bit().data()))
				return 1;
		}
		else {
			if (matchUnsharp(fd->leText->text().toLower().toLocal8Bit().data(), txt.toLower().toLocal8Bit().data()))
				return 1;
		}
	}
	return 0;
}

bool seekEngine::matchUnsharp(char* matchpattern, char* str){
	DEBUG_INFO_ENABLED = init_debug_info();
	if(matchpattern == NULL || str == NULL)
		return false;
	
	int match = -1;
	int matchpattern_len=strlen(matchpattern);
	MString m_matchpattern (matchpattern, matchpattern_len);
	QString res_matchpattern_str1, res_matchpattern_str2;
	m_matchpattern.DoubleMetaphone (res_matchpattern_str1, res_matchpattern_str2);
	
	MString m_str (str, matchpattern_len);
	QString res_str_str1, res_str_str2;
	m_str.DoubleMetaphone (res_str_str1, res_str_str2);
	
	match = res_str_str1.indexOf (res_matchpattern_str1);
	
	if (*DEBUG_INFO_ENABLED)
		cerr << "matchUnsharp: " << matchpattern << " (" << qPrintable(res_matchpattern_str1) << ") <=> " << str << " (" << qPrintable(res_str_str1) << ")  ===> " << match << endl;
	
	if(match > -1)
		return true;
	else
		return false;
}

void seekEngine::putNodeToList ( Node *n ) {

    Node *tmp;
    QString   type;
    QString   size_str;
    QString   media;
    QDateTime mod;

    if ( n == NULL ) return;

    switch ( n->type ) {
    case HC_MEDIA:
	type = tr ( "media" );
        mod  = ( ( DBMedia * ) ( n->data ) )->modification;
        break;

    case HC_DIRECTORY:
	type = tr ( "dir" );
        mod  = ( ( DBDirectory * ) ( n->data ) )->modification;
        break;
    case HC_FILE:
	type = tr ( "file" );
	size_str = QString().sprintf ( "%.1f",( ( DBFile * ) ( n->data ) )->size);
	size_str += QString(getSType ( ( ( DBFile * ) ( n->data ) )->sizeType, true ));
        mod  = ( ( DBFile * ) ( n->data ) )->modification;
        break;
    default:           type = tr ( "error" );
        break;
    }
    tmp=n;
    while ( tmp->type != HC_MEDIA ){
        tmp=tmp->parent;
        progress(pww);
    }

    media = tmp->getNameOf() + "/" + QString().setNum ( ( ( DBMedia * ) ( tmp->data ) )->number );



    fd->resultsl->insertItem ( new Q3ListViewItem ( fd->resultsl,
                               n->getNameOf(),
                               type,
                               size_str,
                               media,
                               n->getFullPath(),
                               date_to_str ( mod )
                                                  ) );
    progress ( pww );
    founded++;
}


