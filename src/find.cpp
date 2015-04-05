/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak, 2010 Christoph Thielecke
****************************************************************************/

#include "find.h"

#include <QVariant>
#include <QCheckBox>
#include <qcombobox.h>
#include <QFrame>
#include <QLayout>
#include <QLabel>
#include <qlineedit.h>
#include <QHeaderView>
#include <QPushButton>
#include <QLayout>
#include <QToolTip>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QSpinBox>
#include <QTextCodec>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QDate>
#include <QCalendarWidget>
#include <QMouseEvent>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QTextCodec>
#include <QDir>
#include <QStatusBar>

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

findDialog::findDialog ( CdCatMainWidget *parent, const char *name, bool isFindDuplicates, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    if (!name) {
        if (isFindDuplicates) {
        } else {
            setObjectName( "findDialog" );
        }
    }
    setWindowIcon( *get_t_find_icon());
    setModal( modal );
    mainw = parent;
    use_unsharpsearch = false;
    this->isFindDuplicates = isFindDuplicates;

    setSizeGripEnabled( true );
    if (!isFindDuplicates) {
        FindDialogBaseLayout = new QGridLayout( this );

        //    QSpacerItem* spacer = new QSpacerItem( 210, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_2 = new QSpacerItem( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_3 = new QSpacerItem( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_4 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_5 = new QSpacerItem( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_5_1 = new QSpacerItem( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_6 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_7 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_8 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_9 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_10 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_11 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_12 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

        layout40 = new QVBoxLayout( this );
        layout39 = new QGridLayout( this );
        findLineLayout = new QHBoxLayout( this );
        layout36 = new QGridLayout( this );
        layout37 = new QGridLayout( this );
        layout31 = new QVBoxLayout( this );
        layout17 = new QHBoxLayout( this );
        layout15 = new QGridLayout( this );
        layout30 = new QHBoxLayout( this );
        layout16 = new QGridLayout( this );
        //     layout17 = new QGridLayout ( this, 1, 1, 0, 6, "layout17" );
        layout_size_min = new QHBoxLayout( this );
        layout_size_max = new QHBoxLayout( this );
        layout_find_in_archive = new QHBoxLayout( this );

        leText = new QLineEdit( this );
        leCategory = new QLineEdit( this );

        deDateStart = new QDateTimeEdit( QDateTime( QDate( 1, 1, 1900 )), this );
        deDateEnd = new QDateTimeEdit( QDateTime( QDate().currentDate()), this );

        cbCasesens = new QCheckBox( this );
        cbEasy = new QCheckBox( this );

        cbKeepSearchResult = new QCheckBox( this );

        cbFilename = new QCheckBox( this );
        cbDirname = new QCheckBox( this );
        cbComment = new QCheckBox( this );
        cbCategory = new QCheckBox( this );
        cbContent = new QCheckBox( this );

        cbArtist = new QCheckBox( this );
        cbTitle = new QCheckBox( this );
        cbAlbum = new QCheckBox( this );
        cbTcomm = new QCheckBox( this );
        cbDateStart = new QCheckBox( this );
        cbDateEnd = new QCheckBox( this );
        cbSizeMin = new QCheckBox( this );
        cbSizeMax = new QCheckBox( this );
        cbUnsharpSearch = new QCheckBox( this );
        cbFindInArchive = new QCheckBox( this );

        cbOwner = new QComboBox( this );
        cbOwner->setMinimumSize( QSize( 0, 0 ));

        cbSin = new QComboBox( this );

        cbSizeUnitMin = new QComboBox( this );
        cbSizeUnitMin->addItem( tr( "Byte" ));
        cbSizeUnitMin->addItem( tr( "KiB" ));
        cbSizeUnitMin->addItem( tr( "MiB" ));
        cbSizeUnitMin->addItem( tr( "GiB" ));
        cbSizeUnitMin->addItem( tr( "TiB" ));

        cbSizeUnitMax = new QComboBox( this );
        cbSizeUnitMax->addItem( tr( "Byte" ));
        cbSizeUnitMax->addItem( tr( "KiB" ));
        cbSizeUnitMax->addItem( tr( "MiB" ));
        cbSizeUnitMax->addItem( tr( "GiB" ));
        cbSizeUnitMax->addItem( tr( "TiB" ));

        spSizeMin = new QSpinBox( this );
        spSizeMin->setMinimum( 1 );
        spSizeMin->setMaximum( 10000000 );

        spSizeMax = new QSpinBox( this );
        spSizeMax->setMinimum( 1 );
        spSizeMax->setMaximum( 10000000 );

        buttonClearSearchResult = new QPushButton( this );

        buttonOk = new QPushButton( this );
        buttonOk->setAutoDefault( true );
        buttonOk->setDefault( true );
        buttonOk->setMinimumWidth( 80 );

        buttonCancel = new QPushButton( this );
        buttonCancel->setAutoDefault( true );
        buttonCancel->setMinimumWidth( 80 );

        resultsl = new QTreeWidget( this );
        QStringList labels;
        labels.append( tr( "Name" ));
        labels.append( tr( "Type" ));
        labels.append( tr( "Size" ));
        labels.append( tr( "Media" ));
        labels.append( tr( "Path" ));
        labels.append( tr( "Modification" ));
        labels.append( tr( "Comment" ));
        labels.append( tr( "Category" ));
        labels.append( tr( "Extension" ));
        resultsl->setHeaderLabels( labels );
        resultsl->setIndentation( 0 );       // remove the indention (|_ sign)
        resultsl->setSortingEnabled( true );

        // resultsl->setColumnAlignment ( 2, Qt::AlignRight ); // FIXME

        buttonClose = new QPushButton( this );
        buttonPrintResult = new QPushButton( this );
        buttonExportResult = new QPushButton( this );

        findTextLabel = new QLabel( this );
        textLabel1 = new QLabel( this );
        textLabel2 = new QLabel( this );
        textLabel5 = new QLabel( this );
        textLabel6 = new QLabel( this );
        textLabel7 = new QLabel( this );
        textLabelFindInArchive = new QLabel( this );

        /* saved ops: */

        cbTcomm->setChecked( mainw->cconfig->find_mco );
        cbCasesens->setChecked( mainw->cconfig->find_cs );
        cbArtist->setChecked( mainw->cconfig->find_mar );
        cbAlbum->setChecked( mainw->cconfig->find_mal );
        cbComment->setChecked( mainw->cconfig->find_co );
        cbCategory->setChecked( mainw->cconfig->find_category );
        cbTitle->setChecked( mainw->cconfig->find_mti );
        cbEasy->setChecked( mainw->cconfig->find_em );
        cbFilename->setChecked( mainw->cconfig->find_fi );
        cbDirname->setChecked( mainw->cconfig->find_di );
        cbContent->setChecked( mainw->cconfig->find_ct );
        cbDateStart->setChecked( mainw->cconfig->find_date_start );
        cbDateEnd->setChecked( mainw->cconfig->find_date_end );
        cbSizeMin->setChecked( mainw->cconfig->find_size_min );
        cbSizeMax->setChecked( mainw->cconfig->find_size_max );
        cbUnsharpSearch->setChecked( mainw->cconfig->find_unsharp_search );
        cbFindInArchive->setChecked( mainw->cconfig->find_in_archive );


        /* layouts:   */

        findLineLayout->addWidget( findTextLabel );
        findLineLayout->addWidget( leText );
        findLineLayout->addWidget( cbKeepSearchResult );
        layout36->addWidget( cbCasesens, 1, 0 );
        layout36->addWidget( cbDirname, 2, 0 );
        layout36->addWidget( cbFilename, 3, 0 );
        layout36->addWidget( cbComment, 4, 0 );
        layout36->addWidget( cbContent, 5, 0 );
        layout36->addWidget( cbEasy, 1, 1 );
        layout36->addWidget( cbArtist, 2, 1 );
        layout36->addWidget( cbTitle, 3, 1 );
        layout36->addWidget( cbAlbum, 4, 1 );
        layout36->addWidget( cbTcomm, 5, 1 );
        layout36->addWidget( cbCategory, 6, 0 );
        layout36->addWidget( leCategory, 6, 1 );
        layout37->addWidget( cbDateStart, 6, 0 );
        layout37->addWidget( cbDateEnd, 7, 0 );
        layout37->addWidget( deDateStart, 6, 1 );
        layout37->addWidget( deDateEnd, 7, 1 );
        layout37->addWidget( cbSizeMin, 8, 0 );
        layout37->addWidget( cbSizeMax, 9, 0 );
        layout37->addWidget( cbUnsharpSearch, 10, 0 );

        layout37->addWidget( cbOwner, 3, 1 );
        layout37->addWidget( cbSin, 2, 1 );
        layout37->addWidget( textLabel1, 2, 0 );
        layout37->addWidget( textLabel2, 3, 0 );
        layout37->addItem( spacer_3, 1, 1, 0, 1 );

        layout_size_min->addWidget( spSizeMin );
        layout_size_min->addWidget( cbSizeUnitMin );
        cbSizeUnitMin->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
        layout37->addItem( layout_size_min, 8, 1 );

        layout_size_max->addWidget( spSizeMax );
        layout_size_max->addWidget( cbSizeUnitMax );
        cbSizeUnitMin->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
        layout37->addItem( layout_size_max, 9, 1 );

        layout_find_in_archive->addWidget( cbFindInArchive );
        layout_find_in_archive->addWidget( textLabelFindInArchive );
        cbFindInArchive->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
        layout37->addItem( layout_find_in_archive, 10, 1 );

        layout39->addLayout( findLineLayout, 0, 0 );
        layout39->addLayout( layout36, 1, 1 );
        layout39->addItem( spacer_2, 1, 0 );
        layout39->addLayout( layout37, 1, 0 );

        layout15->addWidget( buttonOk, 0, 0 );
        layout15->addItem( spacer_5, 0, 1 );
        layout15->addWidget( buttonCancel, 0, 2 );
        layout15->addItem( spacer_5_1, 0, 3 );
        layout15->addWidget( buttonClearSearchResult, 0, 4 );

        layout17->addItem( spacer_6 );
        layout17->addLayout( layout15 );
        layout17->addItem( spacer_4 );

        layout30->addItem( spacer_7 );
        layout30->addWidget( textLabel5 );
        layout30->addItem( spacer_8 );

        layout16->addItem( spacer_9, 0, 2 );
        layout16->addWidget( buttonClose, 0, 1 );
        layout16->addWidget( buttonPrintResult, 0, 3 );
        layout16->addWidget( buttonExportResult, 0, 4 );
        layout16->addItem( spacer_10, 0, 0 );
        layout16->addItem( spacer_11, 0, 2 );
        layout16->addItem( spacer_12, 0, 5 );

        layout31->addLayout( layout17 );
        layout31->addLayout( layout30 );
        layout31->addWidget( resultsl );
        layout31->addLayout( layout16 );

        layout40->addLayout( layout39 );
        layout40->addLayout( layout31 );

        FindDialogBaseLayout->addLayout( layout40, 0, 0 );

        resize( QSize( mainw->cconfig->findWidth, mainw->cconfig->findHeight ).expandedTo( minimumSizeHint()));
        move( mainw->cconfig->findX, mainw->cconfig->findY );

        languageChange();

        fillCBox();
        connect( buttonCancel, SIGNAL( clicked()), this, SLOT( cancele()));
        connect( buttonOk, SIGNAL( clicked()), this, SLOT( seeke()));
        connect( buttonClose, SIGNAL( clicked()), this, SLOT( closee()));
        connect( buttonClearSearchResult, SIGNAL( clicked()), this, SLOT( clearSearchResultClicked()));
        connect( buttonPrintResult, SIGNAL( clicked()), this, SLOT( printResultClicked()));
        connect( buttonExportResult, SIGNAL( clicked()), this, SLOT( exportResultClicked()));
        connect( resultsl, SIGNAL(    currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * )), this, SLOT( select( QTreeWidgetItem *, QTreeWidgetItem * )));
        connect( resultsl, SIGNAL( itemClicked( QTreeWidgetItem *, int )), this, SLOT( select( QTreeWidgetItem *, int )));
        connect( cbDateStart, SIGNAL( stateChanged( int )), this, SLOT( dateStartChanged( int )));
        connect( cbDateEnd, SIGNAL( stateChanged( int )), this, SLOT( dateEndChanged( int )));
        // connect ( deDateStart, SIGNAL ( clicked()),this,SLOT ( dateStartDoubleClicked()));
        // connect ( deDateEnd, SIGNAL ( ),this,SLOT ( dateEndDoubleClicked()));
        connect( cbSizeMin, SIGNAL( clicked()), this, SLOT( sizeMinClicked()));
        connect( cbSizeMax, SIGNAL( clicked()), this, SLOT( sizeMaxClicked()));
        connect( cbCategory, SIGNAL( clicked()), this, SLOT( categoryClicked()));

        deDateStart->setEnabled( false );
        deDateEnd->setEnabled( false );
        cbSizeUnitMin->setEnabled( false );
        spSizeMin->setEnabled( false );

        cbSizeUnitMax->setEnabled( false );
        spSizeMax->setEnabled( false );

        deDateStart->setDateTime( mainw->cconfig->find_date_start_val );
        deDateEnd->setDateTime( mainw->cconfig->find_date_end_val );

        deDateStart->setDisplayFormat( "dd.MM.yyyy hh:mm" );
        deDateEnd->setDisplayFormat( "dd.MM.yyyy hh:mm" );

        cbKeepSearchResult->setChecked( mainw->cconfig->keep_search_result );
        cbSizeMin->setChecked( mainw->cconfig->find_size_min );
        cbSizeMax->setChecked( mainw->cconfig->find_size_max );
        spSizeMin->setValue( mainw->cconfig->find_size_min_val );
        spSizeMax->setValue( mainw->cconfig->find_size_max_val );

        spSizeMin->setEnabled( mainw->cconfig->find_size_min );
        spSizeMax->setEnabled( mainw->cconfig->find_size_max );

        cbSizeUnitMin->setEnabled( mainw->cconfig->find_size_min );
        cbSizeUnitMax->setEnabled( mainw->cconfig->find_size_max );

        cbSizeUnitMin->setCurrentIndex( mainw->cconfig->find_size_unit_min_val );
        cbSizeUnitMax->setCurrentIndex( mainw->cconfig->find_size_unit_max_val );


        leText->setText( mainw->cconfig->lastSearchPattern );
        leCategory->setText( mainw->cconfig->lastFindCategory );

        dateStartChanged( 0 );
        dateEndChanged( 0 );

        setTabOrder( leText, buttonOk );
        buttonPrintResult->setEnabled( false );
        buttonExportResult->setEnabled( false );
        buttonClearSearchResult->setEnabled( false );
        leText->setFocus();
    } else {
        if (!name) {
            setObjectName( "findDuplicatesDialog" );
        }
        setWindowIcon( *get_t_find_icon());

        mainw = parent;
        searchFilepath = "";
        if (mainw->guis->standON != NULL) {
            searchFilepath = mainw->guis->standON->getFullPath();
        }

        setSizeGripEnabled( true );
        FindDialogBaseLayout = new QGridLayout( this );

        QSpacerItem *spacer_4 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_5 = new QSpacerItem( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_5_1 = new QSpacerItem( 36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_6 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_7 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_8 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_9 = new QSpacerItem( 190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_10 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_11 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem *spacer_12 = new QSpacerItem( 200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

        layout40 = new QVBoxLayout( this );
        layout39 = new QGridLayout( this );
        // layout36 = new QGridLayout ( this, 1, 1, 0, 6, "layout36" );
        // layout37 = new QGridLayout ( this, 1, 1, 0, 6, "layout37" );
        layout31 = new QVBoxLayout( this );
        layout17 = new QHBoxLayout( this );
        layout15 = new QGridLayout( this );
        layout30 = new QHBoxLayout( this );
        layout16 = new QGridLayout( this );

        buttonOk = new QPushButton( this );
        buttonOk->setAutoDefault( true );
        buttonOk->setDefault( true );
        buttonOk->setMinimumWidth( 80 );

        buttonCancel = new QPushButton( this );
        buttonCancel->setAutoDefault( true );
        buttonCancel->setMinimumWidth( 80 );

        textLabel5 = new QLabel( this );

        resultsl = new QTreeWidget( this );
        QStringList labels;
        labels.append( tr( "Name" ));
        labels.append( tr( "Type" ));
        labels.append( tr( "Size" ));
        labels.append( tr( "Media" ));
        labels.append( tr( "Path" ));
        labels.append( tr( "Modification" ));
        labels.append( tr( "Comment" ));
        labels.append( tr( "Category" ));
        labels.append( tr( "Extension" ));
        resultsl->setHeaderLabels( labels );
        resultsl->setIndentation( 0 );       // remove the indention (|_ sign)
        resultsl->setSortingEnabled( true );

        // resultsl->setColumnAlignment ( 2, Qt::AlignRight ); // FIXME

        buttonClearSearchResult = new QPushButton( this );

        buttonClose = new QPushButton( this );

        layout30->addItem( spacer_7 );
        layout30->addWidget( textLabel5 );
        layout30->addItem( spacer_8 );

        layout15->addWidget( buttonOk, 0, 0 );
        layout15->addItem( spacer_5, 0, 1 );
        layout15->addWidget( buttonCancel, 0, 2 );
        layout15->addItem( spacer_5_1, 0, 3 );
        layout15->addWidget( buttonClearSearchResult, 0, 4 );

        layout16->addItem( spacer_9, 0, 2 );
        layout16->addWidget( buttonClose, 0, 1 );
        layout16->addItem( spacer_10, 0, 0 );
        layout16->addItem( spacer_11, 0, 2 );
        layout16->addItem( spacer_12, 0, 5 );

        layout17->addItem( spacer_6 );
        layout17->addLayout( layout15 );
        layout17->addItem( spacer_4 );

        layout31->addLayout( layout17 );
        layout31->addLayout( layout30 );
        layout31->addWidget( resultsl );
        layout31->addLayout( layout16 );

        layout40->addLayout( layout39 );
        layout40->addLayout( layout31 );

        FindDialogBaseLayout->addLayout( layout40, 0, 0 );

        resize( QSize( mainw->cconfig->findWidth, mainw->cconfig->findHeight ).expandedTo( minimumSizeHint()));
        move( mainw->cconfig->findX, mainw->cconfig->findY );

        languageChange();

        connect( resultsl, SIGNAL(    currentItemChanged( QTreeWidgetItem *, QTreeWidgetItem * )), this, SLOT( select( QTreeWidgetItem *, QTreeWidgetItem * )));
        connect( resultsl, SIGNAL( itemClicked( QTreeWidgetItem *, int )), this, SLOT( select( QTreeWidgetItem *, int )));
        connect( buttonCancel, SIGNAL( clicked()), this, SLOT( cancele()));
        connect( buttonOk, SIGNAL( clicked()), this, SLOT( seeke()));
        connect( buttonClose, SIGNAL( clicked()), this, SLOT( closee()));
        connect( buttonClearSearchResult, SIGNAL( clicked()), this, SLOT( clearSearchResultClicked()));
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
    if (isFindDuplicates) {
        setWindowTitle( tr( "Search for duplicates in the database..." ));
        QStringList labels;
        labels.append( tr( "Name" ));
        labels.append( tr( "Type" ));
        labels.append( tr( "Size" ));
        labels.append( tr( "Media" ));
        labels.append( tr( "Path" ));
        labels.append( tr( "Modification" ));
        labels.append( tr( "Comment" ));
        labels.append( tr( "Category" ));
        labels.append( tr( "Extension" ));
        resultsl->setHeaderLabels( labels );

        buttonOk->setText( tr( "&Start search" ));
#ifndef _WIN32
//              buttonOk->setAccel ( QKeySequence ( QString::null ) );
#endif
        buttonCancel->setText( tr( "&Cancel" ));
#ifndef _WIN32
//              buttonCancel->setAccel ( QKeySequence ( QString::null ) );
#endif
        textLabel5->setText( tr( "Duplicates for:" ) + " " + searchFilepath );
        buttonClose->setText( tr( "Close" ));
        buttonClearSearchResult->setText( tr( "Clear search results" ));

        resultsl->clear();
    } else {
        setWindowTitle( tr( "Search in the database..." ));
        textLabel1->setText( tr( "Seek in:" ));
        textLabel2->setText( tr( "Owner:" ));
        QStringList labels;
        labels.append( tr( "Name" ));
        labels.append( tr( "Type" ));
        labels.append( tr( "Size" ));
        labels.append( tr( "Media" ));
        labels.append( tr( "Path" ));
        labels.append( tr( "Modification" ));
        labels.append( tr( "Comment" ));
        labels.append( tr( "Category" ));
        labels.append( tr( "Extension" ));
        resultsl->setHeaderLabels( labels );
        findTextLabel->setText( tr( "Find:" ));
        cbKeepSearchResult->setText( tr( "Keep search result" ));
        cbFilename->setText( tr( "File name" ));
        cbAlbum->setText( tr( "mp3-tag Album" ));
        cbArtist->setText( tr( "mp3-tag Artist" ));
        cbTcomm->setText( tr( "mp3-tag Comment" ));
        cbDirname->setText( tr( "Media / Directory name" ));
        cbTitle->setText( tr( "mp3-tag Title" ));
        cbComment->setText( tr( "Comment" ));
        cbCategory->setText( tr( "Category" ));
        cbContent->setText( tr( "Content" ));
        cbDateStart->setText( tr( "Date start" ));
        cbDateEnd->setText( tr( "Date end" ));
        cbSizeMin->setText( tr( "Min size" ));
        cbSizeMax->setText( tr( "Max size" ));
        cbUnsharpSearch->setText( tr( "Unsharp search (slow)" ));
        cbFindInArchive->setText( tr( "Find in archives too" ));
        buttonClearSearchResult->setText( tr( "Clear search results" ));
        buttonOk->setText( tr( "&Start search" ));
        leCategory->setToolTip( tr( "Category for find" ));
#ifndef _WIN32
//              buttonOk->setAccel ( QKeySequence ( QString::null ) );
#endif
        buttonCancel->setText( tr( "&Cancel" ));
#ifndef _WIN32
//              buttonCancel->setAccel ( QKeySequence ( QString::null ) );
#endif
        textLabel5->setText( tr( "Results: search not started" ));
        buttonClose->setText( tr( "Close / Go to selected" ));
        buttonPrintResult->setText( tr( "Print result..." ));
        buttonExportResult->setText( tr( "Export result to HTML..." ));
        cbCasesens->setText( tr( "Case sensitive" ));
        cbEasy->setText( tr( "Use easy matching instead regex" ));
        resultsl->clear();
    }
}
/***************************************************************************/
int findDialog::saveState( void ) {
    if (!isFindDuplicates) {
        mainw->cconfig->keep_search_result = cbKeepSearchResult->isChecked();
        mainw->cconfig->find_em = cbEasy->isChecked();
        mainw->cconfig->find_cs = cbCasesens->isChecked();
        mainw->cconfig->find_di = cbDirname->isChecked();
        mainw->cconfig->find_fi = cbFilename->isChecked();
        mainw->cconfig->find_co = cbComment->isChecked();
        mainw->cconfig->find_category = cbCategory->isChecked();
        mainw->cconfig->find_ct = cbContent->isChecked();
        mainw->cconfig->find_mar = cbArtist->isChecked();
        mainw->cconfig->find_mti = cbTitle->isChecked();
        mainw->cconfig->find_mco = cbTcomm->isChecked();
        mainw->cconfig->find_mal = cbAlbum->isChecked();
        mainw->cconfig->find_date_start = cbDateStart->isChecked();
        mainw->cconfig->find_date_end = cbDateEnd->isChecked();
        mainw->cconfig->find_date_start_val = deDateStart->dateTime();
        mainw->cconfig->find_date_end_val = deDateEnd->dateTime();
        mainw->cconfig->find_size_min = cbSizeMin->isChecked();
        mainw->cconfig->find_size_max = cbSizeMax->isChecked();
        mainw->cconfig->find_size_min_val = spSizeMin->value();
        mainw->cconfig->find_size_max_val = spSizeMax->value();
        mainw->cconfig->find_size_unit_min_val = cbSizeUnitMin->currentIndex();
        mainw->cconfig->find_size_unit_max_val = cbSizeUnitMax->currentIndex();
        mainw->cconfig->find_unsharp_search = cbUnsharpSearch->isChecked();
        mainw->cconfig->find_in_archive = cbFindInArchive->isChecked();
        mainw->cconfig->findX = x();
        mainw->cconfig->findY = y();
        mainw->cconfig->findWidth = width();
        mainw->cconfig->findHeight = height();
        mainw->cconfig->lastSearchPattern = leText->text();
        mainw->cconfig->lastFindCategory = leCategory->text();
    } else {
        // mainw->cconfig->find_em    = cbEasy->isChecked();
        // mainw->cconfig->find_cs    = cbCasesens->isChecked();
        // mainw->cconfig->find_di    = cbDirname->isChecked();
        // mainw->cconfig->find_fi    = cbFilename->isChecked();
        // mainw->cconfig->find_co    = cbComment->isChecked();
        // mainw->cconfig->find_ct    = cbContent->isChecked();
        // mainw->cconfig->find_mar   = cbArtist->isChecked();
        // mainw->cconfig->find_mti   = cbTitle->isChecked();
        // mainw->cconfig->find_mco   = cbTcomm->isChecked();
        // mainw->cconfig->find_mal   = cbAlbum->isChecked();
        // mainw->cconfig->find_date_start   = cbDateStart->isChecked();
        // mainw->cconfig->find_date_end   = cbDateEnd->isChecked();
        // mainw->cconfig->find_date_start_val   = deDateStart->dateTime();
        // mainw->cconfig->find_date_end_val   = deDateEnd->dateTime();
        // mainw->cconfig->find_size_min   = cbSizeMin->isChecked();
        // mainw->cconfig->find_size_max   = cbSizeMax->isChecked();
        // mainw->cconfig->find_size_min_val = spSizeMin->value();
        // mainw->cconfig->find_size_max_val = spSizeMax->value();
        // mainw->cconfig->find_size_unit_min_val = cbSizeUnitMin->currentIndex();
        // mainw->cconfig->find_size_unit_max_val = cbSizeUnitMax->currentIndex();
        // mainw->cconfig->find_unsharp_search = cbUnsharpSearch->isChecked();
        // mainw->cconfig->find_in_archive = cbFindInArchive->isChecked();
        // mainw->cconfig->findX      = x();
        // mainw->cconfig->findY      = y();
        // mainw->cconfig->findWidth  = width();
        // mainw->cconfig->findHeight = height();
        // mainw->cconfig->lastSearchPattern = leText->text();
    }
    mainw->cconfig->writeConfig();
    return 0;
}
/***************************************************************************/
int findDialog::fillCBox( void ) {
    int i, f, c;
    Node *tmp = mainw->db->getRootNode();

    if (tmp == NULL) {
        return 0;
    }

    cbOwner->clear();
    cbOwner->insertItem( 0, tr( "All/Everybody" ));
    cbOwner->insertItem( 0, ((DBCatalog *)(tmp->data))->owner );

    cbSin->clear();
    cbSin->insertItem( 0, tr( "All media" ));

    tmp = tmp->child;     // Jump to the first media
    while (tmp != NULL) {
        if (tmp->type != HC_MEDIA) {
            tmp = tmp->next;
            continue;
        }

        cbSin->insertItem( 0, tmp->getNameOf());
        c = cbOwner->count();
        for (i = 0, f = 1; c > i; i++)
            if (((DBMedia *)(tmp->data))->owner == cbOwner->currentText()) {
                f = 0;
            }
        if (f) {
            cbOwner->insertItem( 0, ((DBMedia *)(tmp->data))->owner );
        }
        tmp = tmp->next;
    }
    return 0;
}
/***************************************************************************/

int findDialog::select( QTreeWidgetItem *i, int ) {
    QTreeWidgetItem *dummy = NULL;

    return select( i, dummy );
}


int findDialog::select( QTreeWidgetItem *i, QTreeWidgetItem * ) {
    if (i == NULL) {
        return 0;
    }

    if (i->text( 3 ).isEmpty()) {      // Not a real result ("There is no matching" label)
        return 0;
    }

    QString nodepath = i->text( 4 ).mid( 2, i->text( 4 ).length() - 1 );
    // std::cerr << "select: nodepath " << qPrintable(nodepath) << std::endl;

    mainw->guis->updateListFromNode(
        (mainw->guis->getNodeFromFullName( mainw->db->getRootNode(), nodepath )));
    QTreeWidgetItemIterator it( mainw->listView );
    while (*it) {
        if (((*it))->text( 0 ) == i->text( 0 )) {
            mainw->listView->setCurrentItem((*it));
            mainw->listView->setCurrentVisible();
            mainw->listView->repaint();
            QString dirpath = QFileInfo( i->text( 4 )).absoluteDir().absolutePath();
            // std::cout << "path: " << qPrintable(i->text(4)) << ", dirpath: " << qPrintable(dirpath) << std::endl;
            mainw->statusBar()->showMessage( tr( "Selected dir: " ) + dirpath );
        }
        ++it;
    }
    return 0;
}
/***************************************************************************/
void findDialog::closeEvent( QCloseEvent *ce ) {
    saveState();
    QWidget::closeEvent( ce );
}

/***************************************************************************/
int findDialog::closee( void ) {
    close();
    return 0;
}
/***************************************************************************/
int findDialog::cancele( void ) {
    close();
    return 0;
}

int findDialog::seeke( void ) {
    seekEngine *se;

    if (mainw == NULL || mainw->db == NULL) {
        return 0;
    }

    if (!isFindDuplicates) {
        if ((leText->text()).isEmpty() && !cbDateStart->isChecked() && !cbDateEnd->isChecked() && !cbSizeMin->isChecked() && !cbSizeMax->isChecked()) {
            return 0;
        }
    }

    se = new seekEngine( this, isFindDuplicates );
    se->start_seek();
    delete se;
    if (!isFindDuplicates) {
        if (resultsl->children().count() > 0) {
            buttonPrintResult->setEnabled( true );
            buttonExportResult->setEnabled( true );
        }
    }
    return 0;
}

void findDialog::dateStartChanged( int ) {
    if (cbDateStart->isChecked()) {
        deDateStart->setEnabled( true );
    } else {
        deDateStart->setEnabled( false );
    }
}

void findDialog::dateEndChanged( int ) {
    if (cbDateEnd->isChecked()) {
        deDateEnd->setEnabled( true );
    } else {
        deDateEnd->setEnabled( false );
    }
}

void findDialog::dateStartDoubleClicked() {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        std::cerr << "dateStartDoubleClicked" << endl;
    }
}

void findDialog::dateEndDoubleClicked() {
}

void findDialog::sizeMinClicked() {
    if (cbSizeMin->isChecked()) {
        cbSizeUnitMin->setEnabled( true );
        spSizeMin->setEnabled( true );
    } else {
        cbSizeUnitMin->setEnabled( false );
        spSizeMin->setEnabled( false );
    }
}

void findDialog::sizeMaxClicked() {
    if (cbSizeMax->isChecked()) {
        cbSizeUnitMax->setEnabled( true );
        spSizeMax->setEnabled( true );
    } else {
        cbSizeUnitMax->setEnabled( false );
        spSizeMax->setEnabled( false );
    }
}

void findDialog::categoryClicked() {
    if (cbCategory->isChecked()) {
        leCategory->setEnabled( true );
    } else {
        leCategory->setEnabled( false );
    }
}

void findDialog::exportResult( bool isPrint ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    /* get info from results listview
     *      TODO rework as model/view and implement as print view
     */

    QPrinter printer;
    QString output_filename = "cdcat_result.html";
    if (isPrint) {
        QPrintDialog *dialog = new QPrintDialog( &printer, this );
        dialog->setWindowTitle( tr( "Print cdcat result" ));
        if (dialog->exec() != QDialog::Accepted) {
            return;
        }
    } else {
        output_filename = QFileDialog::getSaveFileName( 0, tr( "Result file name" ), "cdcat_result.html", "*.html" );
        if (output_filename.isEmpty()) {
            QMessageBox::critical( 0, tr( "Filename missing" ), tr( "Please enter a filename!" ));
            return;
        }
    }
    QTextDocument mydoc;
    QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ));
    QString result_str;
    if (*DEBUG_INFO_ENABLED) {
        cerr << "result (childCount: " << resultsl->children().count() << "): " << endl;
    }
    result_str += "<html>\n";
    result_str += "<head>\n";
    result_str += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n";
    result_str += "<title>" + tr( "Cdcat search result" ) + "</title>\n";
    result_str += "</head>\n";
    result_str += "<body>\n";
    result_str += "<h1>" + tr( "Cdcat search result" ) + "</h1>\n";
    result_str += tr( "catalog" ) + ": " + mainw->db->getRootNode()->getNameOf() + ", " + tr( "created at:" ) + " " + QDateTime::currentDateTime().toLocalTime().toString() + "\n";

    // TODO: include search options in result
    result_str += "<table border=\"1\" title=\"" + tr( "used search options" ) + "\">\n";
    result_str += "<tr>";

    result_str += "<td>";
    result_str += tr( "search pattern:" ) + " " + leText->text();
    result_str += "</td>";

    result_str += "<td>";
    result_str += tr( "Use easy matching instead regex" ) + ": ";
    if (cbEasy->isChecked()) {
        result_str += tr( "on" );
    } else {
        result_str += tr( "off" );
    }
    result_str += "</td>";

    result_str += "<td>";
    result_str += tr( "Case sensitive" ) + ": ";
    if (cbCasesens->isChecked()) {
        result_str += tr( "on" );
    } else {
        result_str += tr( "off" );
    }
    result_str += "</td>";
    result_str += "</tr>";

    result_str += "<tr>";
    result_str += "<td>";
    result_str += tr( "Search in" ) + ": ";

    if (cbDirname->isChecked()) {
        result_str += " " + tr( "Media / Directory name" );
    }
    if (cbFilename->isChecked()) {
        result_str += " " + tr( "File name" );
    }
    if (cbComment->isChecked()) {
        result_str += " " + tr( "Comment" );
    }
    if (cbContent->isChecked()) {
        result_str += " " + tr( "Content" );
    }
    if (cbArtist->isChecked()) {
        result_str += " " + tr( "mp3-tag Artist" );
    }
    if (cbTitle->isChecked()) {
        result_str += " " + tr( "mp3-tag Title" );
    }
    if (cbAlbum->isChecked()) {
        result_str += " " + tr( "mp3-tag Album" );
    }
    if (cbTcomm->isChecked()) {
        result_str += " " + tr( "mp3-tag Comment" );
    }

    if (cbFindInArchive->isChecked()) {
        result_str += " " + tr( "archives" );
    }
    result_str += "</td>";

    result_str += "<td>";
    result_str += tr( "Unsharp search (slow)" ) + ": ";
    if (cbUnsharpSearch->isChecked()) {
        result_str += tr( "on" );
    } else {
        result_str += tr( "off" );
    }
    result_str += "</td>";

    if (cbDateStart->isChecked() || cbDateEnd->isChecked() || cbSizeMin->isChecked() || cbSizeMax->isChecked()) {
        result_str += "<tr>";
        result_str += "<td>";
        result_str += tr( "other options" ) + ":";
        if (cbDateStart->isChecked()) {
            result_str += " " + tr( "Date start" ) + ": " + deDateStart->dateTime().toString();
        }

        if (cbDateEnd->isChecked()) {
            result_str += " " + tr( "Date end" ) + ": " + deDateEnd->dateTime().toString();
        }

        if (cbSizeMin->isChecked()) {
            result_str += " " + tr( "Min size" ) + ": " + QString().setNum( spSizeMin->value()) + cbSizeUnitMin->currentText();
        }

        if (cbSizeMax->isChecked()) {
            result_str += " " + tr( "Max size" ) + ": " + QString().setNum( spSizeMax->value()) + cbSizeUnitMax->currentText();
        }
        result_str += "</td>";
        result_str += "</tr>\n";
    }

    result_str += "</table>\n";

    result_str += "<table border=\"1\">\n";
    result_str += "<tr>";
    result_str += "<th>#</th><th>" + tr( "Name" ) + "</th>";
    result_str += "<th>" + tr( "Type" ) + "</th>";
    result_str += "<th>" + tr( "Size" ) + "</th>";
    result_str += "<th>" + tr( "Media" ) + "</th>";
    result_str += "<th>" + tr( "Path" ) + "</th>";
    result_str += "<th>" + tr( "Modification" ) + "</th>";
    result_str += "<th>" + tr( "Comment" ) + "</th>";
    result_str += "<th>" + tr( "Category" ) + "</th>";
    result_str += "</tr>\n";
    QTreeWidgetItemIterator lastChild( resultsl );
    int i = 0;
    while (*lastChild) {
        /*
         *      listView data:
         *      0: name
         *      1: type,
         *      2: size_str,
         *      3: media,
         *      4: fullPath
         *      5: date,
         *      6: comment (e.g. file inside archive)
         */
        if (*DEBUG_INFO_ENABLED) {
            for (int j = 0; j <= 6; j++)
                cerr << "result[" << i << "][" << j << "]: " << qPrintable((*lastChild)->text( j )) << endl;
        }

        result_str += "<tr>";
        result_str += "<td align=\"right\" style=\"font-size:-2;\">" + QString().setNum( i + 1 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 0 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 1 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 2 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 3 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 4 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 5 ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 6 ).replace( "\n", "<br>" ) + "</td>";
        result_str += "<td style=\"font-size:-2;\">" + (*lastChild)->text( 7 ).replace( "\n", "<br>" ) + "</td>";
        result_str += "</tr>\n";
        if (*DEBUG_INFO_ENABLED) {
            cerr << "result_str: " << qPrintable( result_str ) << endl;
        }
        ++lastChild;
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


    mydoc.setHtml( result_str );
    if (isPrint) {
        mydoc.print( &printer );
    } else {
        QFile outfile( output_filename );
        if (outfile.open( QIODevice::WriteOnly )) {
            outfile.write( result_str.toUtf8());
            outfile.close();
        } else {
            QMessageBox::critical( 0, tr( "File cant open: " ) + output_filename, tr( "Cant open file" ));
        }
    }
}

void findDialog::printResultClicked() {
    exportResult( true );
}

void findDialog::exportResultClicked() {
    exportResult( false );
}

void findDialog::clearSearchResultClicked() {
    resultsl->clear();
    buttonClearSearchResult->setEnabled( false );
}


/***************************************************************************
*
*  Seek engine Class
*
***************************************************************************/

seekEngine::seekEngine ( findDialog *fdp, bool isFindDuplicates ) :
    founded( 0 ), pww( NULL ), error( NULL ), errptr( 0 ), dirname( false ), filename( false ), comment( false ), find_category( false ),
    tartist( false ), ttitle( false ), talbum( false ), tcomment( false ), content( false ), allmedia( false ),
    allowner( false ), dateStartChecked( false ), dateEndChecked( false ), sizeMinChecked( false ), sizeMaxChecked( false ),
    findInArchivesChecked( false ), size_min( 0 ), size_max( 0 ) {
    this->fd = fdp;
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
int seekEngine::start_seek( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    pww = new PWw( fd, fd->mainw->app );
    pww->setCancel( true );
    pww->refreshTime = 200;
    pww->setProgressText( tr( "Searching, please wait..." ));
    pww->show();

    QObject::connect( pww, SIGNAL( cancelReceivedByUser( bool )), pww, SLOT( doCancelReceived( bool )));
    progress( pww );

    founded = 0;

    if (!searchForDuplicates) {
        if (!fd->cbKeepSearchResult->isChecked()) {
            fd->resultsl->clear();
        }

        // get the pattern
        if (fd->cbEasy->isChecked()) {
            strncpy( patt, (const char *)((QTextCodec::codecForLocale())->fromUnicode( "*" + fd->leText->text() + "*" )), 2047 );
        } else {
            strncpy( patt, (const char *)((QTextCodec::codecForLocale())->fromUnicode( fd->leText->text())), 2047 );
        }

        // recode the pattern /easy/  put ^$ and ? -> .  * -> .*
        if (fd->cbEasy->isChecked()) {
            easyFormConversion( patt );
        }

        // recode the pattern /Case sens/ lok -> [l|L][o|O][k|K]
        if (!fd->cbCasesens->isChecked()) {
            caseSensConversion( patt );
        }
        if (*DEBUG_INFO_ENABLED) {
            fprintf( stderr, "The complete pattern is \"%s\"\n", patt );
        }
        //     re = pcre_compile ( patt,0,&error,&errptr,NULL );
        re->setPattern( QString( patt ));
        //     if ( !fd->cbEasy->isChecked() )
        //      re->setPatternSyntax(QRegExp::Wildcard);
        //     else
        re->setPatternSyntax( QRegExp::RegExp2 );

        //     if ( re == NULL ) {
        if (!re->isValid()) {
            QMessageBox::warning( fd, tr( "Error in the pattern:" ), QString( patt ));
            return 1;
        }

        //// this tries to opimize pattern
        //     hints = pcre_study ( re,0,&error );
        //
        //     if ( error != NULL ) {
        //         QMessageBox::warning ( fd,tr ( "Error in the pattern:" ),error );
        //         return 1;
        //     }
        fd->use_unsharpsearch = fd->cbUnsharpSearch->isChecked();

        dirname = fd->cbDirname->isChecked();
        filename = fd->cbFilename->isChecked();
        comment = fd->cbComment->isChecked();
        category = fd->leCategory->text();
        find_category = fd->cbCategory->isChecked();
        if (find_category && category.isEmpty()) {
            find_category = false;
        }

        tartist = fd->cbArtist->isChecked();
        ttitle = fd->cbTitle->isChecked();
        tcomment = fd->cbTcomm->isChecked();
        talbum = fd->cbAlbum->isChecked();
        content = fd->cbContent->isChecked();
        dateStartChecked = fd->cbDateStart->isChecked();
        dateEndChecked = fd->cbDateEnd->isChecked();
        dateStart = fd->deDateStart->dateTime();
        dateEnd = fd->deDateEnd->dateTime();
        sizeMinChecked = fd->cbSizeMin->isChecked();
        sizeMaxChecked = fd->cbSizeMax->isChecked();
        findInArchivesChecked = fd->cbFindInArchive->isChecked();


        if (sizeMinChecked) {
            if (fd->cbSizeUnitMin->currentIndex() == 0) {
                size_min = fd->spSizeMin->value();                 // Byte
            }
            if (fd->cbSizeUnitMin->currentIndex() == 1) {
                size_min = fd->spSizeMin->value() * 1024.0;                 // KByte
            }
            if (fd->cbSizeUnitMin->currentIndex() == 2) {
                size_min = fd->spSizeMin->value() * 1024.0 * 1024.0;                 // MByte
            }
            if (fd->cbSizeUnitMin->currentIndex() == 3) {
                size_min = fd->spSizeMin->value() * 1024.0 * 1024.0 * 1024.0;                 // GByte
            }
            if (fd->cbSizeUnitMin->currentIndex() == 4) {
                size_min = fd->spSizeMin->value() * 1024.0 * 1024.0 * 1024.0 * 1024.0;                 // TByte
            }
            //  std::cerr << "minsize checked, type "<< fd->cbSizeUnitMin->currentIndex() <<", min size " << size_min << endl;
        }

        if (sizeMaxChecked) {
            if (fd->cbSizeUnitMax->currentIndex() == 0) {
                size_max = fd->spSizeMax->value();                 // Byte
            }
            if (fd->cbSizeUnitMax->currentIndex() == 1) {
                size_max = fd->spSizeMax->value() * 1024.0;                 // KByte
            }
            if (fd->cbSizeUnitMax->currentIndex() == 2) {
                size_max = fd->spSizeMax->value() * 1024.0 * 1024.0;                 // MByte
            }
            if (fd->cbSizeUnitMax->currentIndex() == 3) {
                size_max = fd->spSizeMax->value() * 1024.0 * 1024.0 * 1024.0;                 // GByte
            }
            if (fd->cbSizeUnitMax->currentIndex() == 4) {
                size_max = fd->spSizeMax->value() * 1024.0 * 1024.0 * 1024.0 * 1024.0;                 // TByte
            }
            //  std::cerr << "maxsize checked, type "<< fd->cbSizeUnitMax->currentIndex() <<", max size " << size_max << endl;
        }

        allmedia = false;
        allowner = false;

        media = fd->cbSin->currentText();
        owner = fd->cbOwner->currentText();

        if (0 == fd->cbOwner->currentIndex()) {
            allowner = true;
        }
        if (0 == fd->cbSin->currentIndex()) {
            allmedia = true;
        }
    }

    progress( pww );
    fd->textLabel5->setText( tr( "Results:" ) + " " + tr( "searching is in progress" ));
    /*seek...*/
    analyzeNode( pww, fd->mainw->db->getRootNode());
    if (pww->doCancel) {
        QMessageBox::warning( 0, tr( "Search cancelled" ), tr( "You have cancelled searching." ));
    }
    fd->textLabel5->setText( tr( "Last search results:" ) + " " + QString().setNum( founded ));
    if (founded == 0) {
        QTreeWidgetItem *newitem = new QTreeWidgetItem( fd->resultsl );
        newitem->setText( 0, tr( "There is no matching." ));
        fd->buttonClearSearchResult->setEnabled( true );
        newitem->setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );
        fd->resultsl->addTopLevelItem( newitem );
    }

    QObject::disconnect( pww, SIGNAL( cancelReceivedByUser( bool )), pww, SLOT( doCancelReceived( bool )));
    progress( pww );
    pww->end();

    delete pww;

    QApplication::restoreOverrideCursor();
    return 0;
}
/***************************************************************************/
int seekEngine::analyzeNode( PWw *pww, Node *n, Node *pa ) {
    DEBUG_INFO_ENABLED = init_debug_info();

    if (fd->mainw->app->hasPendingEvents()) {
        fd->mainw->app->processEvents();
    }

    progress( pww );

    if (n == NULL || pww->doCancel) {
        return 0;
    }

    if (searchForDuplicates) {
        if (fd->mainw->guis->standON == NULL) {
            if (*DEBUG_INFO_ENABLED) {
                std::cout << "standON is NULL! " << std::endl;
            }
        }
        return 0;
    }

    if (searchForDuplicates) {
        switch (n->type) {
        case HC_CATALOG:
            analyzeNode( pww, n->child );
            return 0;
        case HC_MEDIA:
            if (find_category) {
                if (((DBMedia *)(n->data))->category.contains( category )) {
                    putNodeToList( n );
                }
            }
            progress( pww );
            analyzeNode( pww, n->child );
            analyzeNode( pww, n->next );
            return 0;
        case HC_DIRECTORY:
            progress( pww );
            if (find_category) {
                if (((DBDirectory *)(n->data))->category.contains( category )) {
                    putNodeToList( n );
                }
            }
            analyzeNode( pww, n->child );
            analyzeNode( pww, n->next );
            return 0;
        case HC_CATLNK:
            if (find_category) {
                if (((DBCatLnk *)(n->data))->category.contains( category )) {
                    putNodeToList( n );
                }
            }
            analyzeNode( pww, n->next );
            return 0;
        case HC_FILE:
            if (find_category) {
                if (((DBFile *)(n->data))->category.contains( category )) {
                    putNodeToList( n );
                }
            }
            if (*DEBUG_INFO_ENABLED) {
                // std::cout << "testing file: " << qPrintable(n->getNameOf()) << " name: " << qPrintable(fd->mainw->guis->standON->getNameOf()) << " <=> " << qPrintable(n->getNameOf()) << ". size: " <<  (( DBFile * ) ( fd->mainw->guis->standON ))->size << " <=> " << ( ( DBFile * ) ( n->data ) )->size << ", size type: " <<  (( DBFile * ) ( fd->mainw->guis->standON ) )->sizeType << " <=> " << ( ( DBFile * ) ( n->data ) )->sizeType  << std::endl;
                std::cout << "testing file: " << qPrintable( n->getNameOf()) << " name: " << qPrintable( fd->mainw->guis->standON->getNameOf()) << " <=> " << qPrintable( n->getNameOf()) << std::endl;
            }
            if (fd->mainw->guis->standON->getNameOf() == n->getNameOf() && fd->mainw->guis->standON->getFullPath() != n->getFullPath()) {
                if (*DEBUG_INFO_ENABLED) {
                    std::cout << "filename match!" << std::endl;
                    std::cout << "size: " << fd->mainw->guis->mainw->db->getSize( fd->mainw->guis->standON ) << " <=> " << ((DBFile *)(n->data))->size << std::endl;
                }
                if (fd->mainw->guis->mainw->db->getSize( fd->mainw->guis->standON ) == ((DBFile *)(n->data))->size) {
                    if (*DEBUG_INFO_ENABLED) {
                        std::cout << "filesize match!" << std::endl;
                    }
                    putNodeToList( n );
                }
            }
            analyzeNode( pww, n->next );
            return 0;
        }
    } else {
        bool isOk = false;
        QString filecomment;
        switch (n->type) {
        case HC_CATALOG:
            analyzeNode( pww, n->child );
            return 0;
        case HC_MEDIA:
            progress( pww );

            // It is necessary to analyze this media node? /Owner/Media/
            if ((allmedia || (media == n->getNameOf())) &&
                (allowner || (owner == ((DBMedia *)(n->data))->owner))) {
                if (dirname) {
                    if (!matchIt( n->getNameOf())) {
                        isOk = false;
                    }
                    if (dateStartChecked && !dateEndChecked) {
                        if (((DBMedia *)(n->data))->modification >= dateStart) {
                            isOk = true;
                        }
                    } else if (!dateStartChecked && dateEndChecked) {
                        if (((DBMedia *)(n->data))->modification <= dateEnd) {
                            isOk = true;
                        }
                    } else if (dateStartChecked && dateEndChecked) {
                        if (((DBMedia *)(n->data))->modification >= dateStart && ((DBDirectory *)(n->data))->modification <= dateEnd) {
                            isOk = true;
                        }
                    }

                    if (isOk) {
                        putNodeToList( n );
                        analyzeNode( pww, n->child );
                        analyzeNode( pww, n->next );
                        return 0;
                    }
                }

                if (comment) {
                    if (matchIt(((DBMedia *)(n->data))->comment )) {
                        putNodeToList( n );
                    }
                }

                analyzeNode( pww, n->child );
            }
            analyzeNode( pww, n->child );
            progress( pww );
            analyzeNode( pww, n->next );
            return 0;

        case HC_CATLNK:
            analyzeNode( pww, n->next );
            return 0;

        case HC_DIRECTORY:
            progress( pww );
            isOk = true;
            if (dirname) {
                if (!matchIt( n->getNameOf())) {
                    isOk = false;
                }
                if (dateStartChecked && !dateEndChecked) {
                    if (((DBDirectory *)(n->data))->modification < dateStart) {
                        isOk = false;
                    }
                } else if (!dateStartChecked && dateEndChecked) {
                    if (((DBDirectory *)(n->data))->modification > dateEnd) {
                        isOk = false;
                    }
                } else if (dateStartChecked && dateEndChecked) {
                    if (((DBDirectory *)(n->data))->modification > dateStart && ((DBDirectory *)(n->data))->modification > dateEnd) {
                        isOk = false;
                    }
                }

                if (isOk) {
                    putNodeToList( n );
                    analyzeNode( pww, n->child );
                    analyzeNode( pww, n->next );
                    return 0;
                }
            }

            if (comment) {
                if (matchIt(((DBDirectory *)(n->data))->comment )) {
                    putNodeToList( n );
                }
            }

            analyzeNode( pww, n->child );
            progress( pww );
            analyzeNode( pww, n->next );
            return 0;

        case HC_FILE:
            isOk = true;
            filecomment = "";
            if (filename) {
                if (!matchIt( n->getNameOf())) {
                    isOk = false;
                }
            }
            if (isOk && dateStartChecked && !dateEndChecked) {
                if (((DBFile *)(n->data))->modification < dateStart) {
                    isOk = false;
                }
            } else if (isOk && !dateStartChecked && dateEndChecked) {
                if (((DBFile *)(n->data))->modification > dateEnd) {
                    isOk = false;
                }
            } else if (isOk && dateStartChecked && dateEndChecked) {
                if (((DBFile *)(n->data))->modification < dateStart || ((DBFile *)(n->data))->modification > dateEnd) {
                    isOk = false;
                }
            }

            if (isOk && sizeMinChecked && !sizeMaxChecked) {
                double real_size = 0.0;
                double real_size_min = size_min;
                switch (((DBFile *)(n->data))->sizeType) {
                case 0:
                    real_size = (((DBFile *)(n->data))->size);
                    break;                                     // byte
                case 1:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0;
                    break;                                     // Kb
                case 2:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0;
                    break;                                     // Mb
                case 3:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Gb
                case 4:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Tb
                }
                //              std::cerr << "minsize checked, min size " << real_size_min << " ~ " << real_size<< endl;
                if (real_size < real_size_min) {
                    isOk = false;
                }
            }
            if (isOk && !sizeMinChecked && sizeMaxChecked) {
                double real_size = 0.0;
                double real_size_max = size_max;
                switch (((DBFile *)(n->data))->sizeType) {
                case 0:
                    real_size = (((DBFile *)(n->data))->size);
                    break;                                     // byte
                case 1:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0;
                    break;                                     // Kb
                case 2:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0;
                    break;                                     // Mb
                case 3:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Gb
                case 4:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Tb
                }
                //              std::cerr << "size type: "<<  ( ( DBFile * ) ( n->data ) )->sizeType <<", maxsize checked, max size " << real_size_max << " ~ " << real_size << endl;
                if (real_size > real_size_max) {
                    isOk = false;
                }
            }
            if (isOk && sizeMinChecked && sizeMaxChecked) {
                double real_size = 0.0;
                double real_size_min = size_min;
                double real_size_max = size_max;
                switch (((DBFile *)(n->data))->sizeType) {
                case 0:
                    real_size = (((DBFile *)(n->data))->size);
                    break;                                     // byte
                case 1:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0;
                    break;                                     // Kb
                case 2:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0;
                    break;                                     // Mb
                case 3:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Gb
                case 4:
                    real_size = (((DBFile *)(n->data))->size) * 1024.0 * 1024.0 * 1024.0 * 1024.0;
                    break;                                     // Tb
                }
                //              std::cerr << "min & maxsize checked, min size " << real_size_min << "/max size " << real_size_max << " ~ " << real_size << endl;
                if (real_size < real_size_min || real_size > real_size_max) {
                    isOk = false;
                }
            }
            if (findInArchivesChecked) {
                // FIXME maybe a flag 'is_archive' should be added
                bool archiveMatches = false;
                filecomment = "";
                QList<ArchiveFile> ArchiveFileList = ((DBFile *)(n->data))->archivecontent;
                for (int i = 0; i < ArchiveFileList.size(); i++) {
                    ArchiveFile af = ArchiveFileList.at( i );
                    QString filepath = af.path.section( '/', -1, -1 );
                    if (*DEBUG_INFO_ENABLED) {
                        std::cout << "testing file inside archive: " << qPrintable( filepath ) << std::endl;
                    }

                    for (int i = 0; i < ArchiveFileList.size(); i++) {
                        ArchiveFile af = ArchiveFileList.at( i );
                        QString filepath = af.path.section( '/', -1, -1 );
                        if (*DEBUG_INFO_ENABLED) {
                            std::cout << "testing file inside archive: " << qPrintable( filepath ) << std::endl;
                        }

                        if (filename) {
                            if (matchIt( filepath )) {
                                archiveMatches = true;
                                filecomment = tr( "File in archive: " ) + af.path;
                                // break;
                            }
                        } else if (dateStartChecked && !dateEndChecked) {
                            if (af.date > dateStart) {
                                archiveMatches = true;
                                filecomment = tr( "File in archive: " ) + af.path;
                                // break;
                            } else {
                                archiveMatches = false;
                            }
                        } else if (!dateStartChecked && dateEndChecked) {
                            if (af.date < dateEnd) {
                                archiveMatches = true;
                                filecomment = tr( "File in archive: " ) + af.path;
                                // break;
                            } else {
                                archiveMatches = false;
                            }
                        } else if (dateStartChecked && dateEndChecked) {
                            if (af.date > dateStart || af.date < dateEnd) {
                                archiveMatches = true;
                                filecomment = tr( "File in archive: " ) + af.path;
                                // break;
                            } else {
                                archiveMatches = false;
                            }
                        }
                    }
                    if (archiveMatches) {
                        isOk = true;
                    }
                }
            }
            if (isOk) {
                putNodeToList( n, filecomment );
                analyzeNode( pww, n->next );
                return 0;
            }

            if (comment) {
                if (!matchIt(((DBFile *)(n->data))->comment )) {
                    isOk = false;
                }
                if (isOk) {
                    putNodeToList( n );
                    analyzeNode( pww, n->next );
                    return 0;
                }
            }

            analyzeNode( pww, ((DBFile *)(n->data))->prop, n );
            analyzeNode( pww, n->next );
            return 0;

        case HC_MP3TAG:
            if (tartist) {
                if (matchIt(((DBMp3Tag *)(n->data))->artist )) {
                    putNodeToList( pa );
                    return 0;
                }
            }
            if (ttitle) {
                if (matchIt(((DBMp3Tag *)(n->data))->title )) {
                    putNodeToList( pa );
                    return 0;
                }
            }
            if (talbum) {
                if (matchIt(((DBMp3Tag *)(n->data))->album )) {
                    putNodeToList( pa );
                    return 0;
                }
            }
            if (tcomment) {
                if (matchIt(((DBMp3Tag *)(n->data))->comment )) {
                    putNodeToList( pa );
                    return 0;
                }
            }

            analyzeNode( pww, n->next, pa );
            return 0;
        case HC_CONTENT:
            if (content) {
                if (matchIt( QString::fromLocal8Bit((const char *)((DBContent *)(n->data))->bytes ))) {
                    putNodeToList( pa );
                    return 0;
                }
            }
            analyzeNode( pww, n->next, pa );
            return 0;
        }
        return -1;
    }
    return -1;
}


int seekEngine::matchIt( QString txt ) {
    // const char *encoded;
    int match;

    if (txt.isEmpty()) {
        return 0;
    }
    if (!fd->use_unsharpsearch) {
        // encoded = ( const char * ) ( ( QTextCodec::codecForLocale() )->fromUnicode ( txt ) );
        // match   = pcre_exec ( re,hints,encoded,strlen ( encoded ),0,0,offsets,99 );
        // match = re->exactMatch (QString( encoded));
        match = re->exactMatch( QString( txt ));
        // cerr << "matchit: " << qPrintable(txt) << " <==> " << qPrintable(re->pattern()) <<" result: " << match << endl;

        if (match == 1) {
            return 1;
        }
    } else {
        if (fd->cbCasesens->isChecked()) {
            if (matchUnsharp( fd->leText->text().toLocal8Bit().data(), txt.toLocal8Bit().data())) {
                return 1;
            }
        } else {
            if (matchUnsharp( fd->leText->text().toLower().toLocal8Bit().data(), txt.toLower().toLocal8Bit().data())) {
                return 1;
            }
        }
    }
    return 0;
}

bool seekEngine::matchUnsharp( char *matchpattern, char *str ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (matchpattern == NULL || str == NULL) {
        return false;
    }

    int match = -1;
    int matchpattern_len = strlen( matchpattern );
    MString m_matchpattern( matchpattern, matchpattern_len );
    QString res_matchpattern_str1, res_matchpattern_str2;
    m_matchpattern.DoubleMetaphone( res_matchpattern_str1, res_matchpattern_str2 );

    MString m_str( str, matchpattern_len );
    QString res_str_str1, res_str_str2;
    m_str.DoubleMetaphone( res_str_str1, res_str_str2 );

    match = res_str_str1.indexOf( res_matchpattern_str1 );

    if (*DEBUG_INFO_ENABLED) {
        cerr << "matchUnsharp: " << matchpattern << " (" << qPrintable( res_matchpattern_str1 ) << ") <=> " << str << " (" << qPrintable( res_str_str1 ) << ")  ===> " << match << endl;
    }

    if (match > -1) {
        return true;
    } else {
        return false;
    }
}

void seekEngine::putNodeToList( Node *n, QString comment ) {
    Node *tmp = NULL;
    QString type;
    QString size_str = "";
    QString media = "";
    QString tmp_category = "";
    QString extension = "";
    QDateTime mod;

    if (n == NULL) {
        return;
    }

    DEBUG_INFO_ENABLED = init_debug_info();

    switch (n->type) {
    case HC_MEDIA:
        type = tr( "media" );
        mod = ((DBMedia *)(n->data))->modification;
        comment = ((DBMedia *)(n->data))->comment;
        tmp_category = ((DBMedia *)(n->data))->category;
        break;
    case HC_DIRECTORY:
        type = tr( "dir" );
        mod = ((DBDirectory *)(n->data))->modification;
        comment = ((DBDirectory *)(n->data))->comment;
        tmp_category = ((DBDirectory *)(n->data))->category;
        break;
    case HC_FILE:
        type = tr( "file" );
        size_str = QString().sprintf( "%.1f", ((DBFile *)(n->data))->size );
        size_str += QString( getSType(((DBFile *)(n->data))->sizeType, true ));
        mod = ((DBFile *)(n->data))->modification;
        comment = ((DBFile *)(n->data))->comment;
        tmp_category = ((DBFile *)(n->data))->category;
        if (n->getFullPath().contains( '.' )) {
            extension = n->getFullPath().split( "." ).last();
        }
        break;
    default:
        type = tr( "error" );
        break;
    }
    tmp = n;
    if (*DEBUG_INFO_ENABLED) {
        std::cout << "putNodeToList: path: " << qPrintable( n->getFullPath()) << ",  type: " << qPrintable( type ) << std::endl;
    }
    while (tmp->type != HC_MEDIA) {
        tmp = tmp->parent;
        if (fd->mainw->app->hasPendingEvents()) {
            fd->mainw->app->processEvents();
        }
        progress( pww );
    }

    media = tmp->getNameOf() + "/" + QString().setNum(((DBMedia *)(tmp->data))->number );
    QTreeWidgetItem *newitem;
    newitem = new QTreeWidgetItem( fd->resultsl );
    newitem->setText( 0, n->getNameOf());
    newitem->setText( 1, type );
    newitem->setText( 2, size_str );
    newitem->setText( 3, media );
    newitem->setText( 4, n->getFullPath());
    newitem->setText( 5, date_to_str( mod ));
    newitem->setText( 6, comment );
    newitem->setText( 7, tmp_category );
    newitem->setText( 8, extension );
    newitem->setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );

    // newitem->setMultiLinesEnabled ( true ); // FIXME
    fd->resultsl->addTopLevelItem( newitem );
    fd->buttonClearSearchResult->setEnabled( true );
    progress( pww );
    founded++;
}


// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
