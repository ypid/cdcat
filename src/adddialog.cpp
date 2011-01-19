/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "adddialog.h"
#include "guibase.h"
#include "dbase.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <q3header.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <q3multilineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPaintEvent>
#include <Q3GridLayout>
#include <QMouseEvent>
#include <Q3VBoxLayout>

#include "dirview.h"
#include "icons.h"
#include "mainwidget.h"
#include "config.h"
#include "misc.h"
#include "selreadable.h"

addDialog::addDialog ( GuiSlave *c, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    int i;

    caller = c;
    if ( !name )
        setName ( "addDialog" );
    setIcon ( *get_t_add_icon() );

    setSizeGripEnabled ( TRUE );
    addDialogLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "addDialogLayout" );

    layout10 = new Q3VBoxLayout ( 0, 0, 6, "layout10" );

    layout9 = new Q3HBoxLayout ( 0, 0, 6, "layout9" );

    layout8 = new Q3VBoxLayout ( 0, 0, 6, "layout8" );

    textLabel6 = new QLabel ( this, "textLabel6" );
    layout8->addWidget ( textLabel6 );

    dirView = new DirectoryView ( this, "mydirview" );
    dirView->resize ( 190,210 );
    dirView->setGeometry ( QRect ( 10, 40, 190, 210 ) );
    dirView->setCaption ( tr ( "Directory Browser" ) );

    layout8->addWidget ( dirView );

    layout9->addLayout ( layout8 );

    layout7 = new Q3VBoxLayout ( 0, 0, 6, "layout7" );

    textLabel1 = new QLabel ( this, "textLabel1" );
    layout7->addWidget ( textLabel1 );

#ifndef _WIN32
    cbAutoDetectAtMount = new QCheckBox ( this, "cbAutoDetectAtMount" );
    layout7->addWidget ( cbAutoDetectAtMount );
#endif

    leName = new QLineEdit ( this, "leName" );
    layout7->addWidget ( leName );

    layout2 = new Q3HBoxLayout ( 0, 0, 6, "layout2" );

    textLabel2 = new QLabel ( this, "textLabel2" );
    layout2->addWidget ( textLabel2 );

    sbNumber = new QSpinBox ( 0,50000,1, this, "sbNumber" );
    layout2->addWidget ( sbNumber );
    layout7->addLayout ( layout2 );

    layout3 = new Q3HBoxLayout ( 0, 0, 6, "layout3" );

    textLabel4 = new QLabel ( this, "textLabel4" );
    layout3->addWidget ( textLabel4 );

    cbType = new QComboBox ( FALSE, this, "cbType" );
    layout3->addWidget ( cbType );
    layout7->addLayout ( layout3 );

    layout4 = new Q3HBoxLayout ( 0, 0, 6, "layout4" );

    textLabel5 = new QLabel ( this, "textLabel5" );
    layout4->addWidget ( textLabel5 );

    leOwner = new QLineEdit ( this, "leOwner" );
    layout4->addWidget ( leOwner );
    layout7->addLayout ( layout4 );

    layout5 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout5" );

    textLabel3 = new QLabel ( this, "textLabel3" );

    layout5->addWidget ( textLabel3, 0, 0 );

    teComm = new Q3MultiLineEdit ( this, "teComm" );

    layout5->addMultiCellWidget ( teComm, 0, 1, 1, 1 );
    QSpacerItem* spacer = new QSpacerItem ( 20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout5->addItem ( spacer, 1, 0 );
    layout7->addLayout ( layout5 );

    buttonPli = new QPushButton ( this,"buttonReadable" );
    layout7->addWidget ( buttonPli );

    layout9->addLayout ( layout7 );
    layout10->addLayout ( layout9 );

    layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );
    QSpacerItem* spacer_2 = new QSpacerItem ( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem ( spacer_2 );

    buttonOK = new QPushButton ( this, "buttonOk" );
    buttonOK->setMinimumWidth ( 100 );
    layout1->addWidget ( buttonOK );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setMinimumWidth ( 100 );
    layout1->addWidget ( buttonCancel );
    QSpacerItem* spacer_3 = new QSpacerItem ( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem ( spacer_3 );
    layout10->addLayout ( layout1 );

    addDialogLayout->addLayout ( layout10, 0, 0 );
    languageChange();
    resize ( QSize ( 454, 350 ).expandedTo ( minimumSizeHint() ) );

    connect ( buttonOK,SIGNAL ( clicked() ),this,SLOT ( bOk() ) );
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( bCan() ) );

    connect ( buttonPli,SIGNAL ( clicked() ),this,SLOT ( sread() ) );

    connect ( dirView,SIGNAL ( folderSelected ( const QString & ) ),this,SLOT ( setMediaName ( const QString & ) ) );

#ifndef _WIN32
    connect ( cbAutoDetectAtMount, SIGNAL ( clicked() ), this, SLOT ( autoDetectAtMountToggled()) );
#endif

    for ( i=1;!caller->isIdentical ( i );i++ );
    sbNumber->setValue ( i );

    volumename = 1; //so, the next line will set up the name
    setMediaName ( " " );


    if ( cbType->currentItem() +1 == CD || cbType->currentItem() +1 == DVD ) {
        dirView->setDir ( ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath );
        dirView->sDir= ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath;
        setMediaName ( ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath );
    }
    buttonOK->setFocus();
}

int addDialog::sread() {
    SelReadable *sr = new SelReadable ( caller->mainw->cconfig,this,"pluginsconf",true );
    sr->exec();
    delete sr;
    return 0;
}


/*
 *  Destroys the object and frees any allocated resources
 */
addDialog::~addDialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */

void addDialog::languageChange() {
    setCaption ( tr ( "Add Media to the Database" ) );
    textLabel6->setText ( tr ( "Choose a directory to scan:" ) );
    textLabel1->setText ( tr ( "Media Name:" ) );
    textLabel2->setText ( tr ( "Serial number of Media:" ) );
    textLabel5->setText ( tr ( "Owner:" ) );
    textLabel4->setText ( tr ( "Type:" ) );
    textLabel3->setText ( tr ( "Comment:" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
    buttonOK->setText ( tr ( "OK / Scan" ) );
    buttonPli->setText ( tr ( "Select readable items" ) );
#ifndef _WIN32
    cbAutoDetectAtMount->setText( tr("detect CDCROM/DVD media name after mount"));
#endif
    cbType->clear();
    cbType->insertItem ( *get_m_cd_icon(), tr ( "CD" ) );
    cbType->insertItem ( *get_m_dvd_icon(), tr ( "DVD" ) );
    cbType->insertItem ( *get_m_hdd_icon(), tr ( "HardDisc" ) );
    cbType->insertItem ( *get_m_floppy_icon(), tr ( "Floppy" ) );
    cbType->insertItem ( *get_m_net_icon(), tr ( "NetworkPlace" ) );
    cbType->insertItem ( *get_m_flash_icon(), tr ( "FlashDrive" ) );
    cbType->insertItem ( *get_m_other_icon(), tr ( "OtherDevice" ) );
//  cbType->setCurrentText(tr( "CD" )); // default
}

int addDialog::setMediaName ( const QString & ds ) {
    QString tm;

#ifdef _WIN32
    QDir confdir ( ( caller->mainw->cconfig->cdrompath ).lower() );
    QDir selected ( ds.lower() );
#else
    QDir confdir ( caller->mainw->cconfig->cdrompath );
    QDir selected ( ds );
#endif

    QApplication::setOverrideCursor ( Qt::waitCursor );

    if ( ( cbType->currentItem() +1 == CD ) &&
            ( confdir  == selected )         &&
            ! ( tm = getCDName ( caller->mainw->cconfig->cdrompath ) ).isEmpty() ) {
        leName->setText ( tm );

#ifndef _WIN32
        // also set the media type to DVD if needed
        if ( diskIsDVD ( caller->mainw->cconfig->cdrompath ) )
            cbType->setCurrentItem ( DVD - 1 );
        else
            cbType->setCurrentItem ( CD - 1 );
#endif
        volumename = 1;
    } else {
        if ( volumename == 1 ) {
            volumename = 0;
            tm =tr ( "New Disk %1" ).arg ( sbNumber->value() );
            leName->setText ( tm );
        }
    }
    QApplication::restoreOverrideCursor();
    return 0;
}

int addDialog::bOk ( void ) {
    if ( ( leName->text() ).isEmpty() ) {
        QMessageBox::warning ( ( QWidget * ) this,tr ( "Error:" ),tr ( "You must be fill the \"Name\" field!" ) );
        return 0;
    }

    if ( ( leName->text() ).startsWith ( "@" ) ) {
        QMessageBox::warning ( ( QWidget * ) this,tr ( "Error:" ),tr ( "The media name can't begin with the \"@\" character!" ) );
        return 0;
    }

    if ( !caller->isIdentical ( leName->text() ) ) {
        QMessageBox::warning ( ( QWidget * ) this,tr ( "Error:" ),tr ( "The Media Name must be unique! Please change it!" ) );
        return 0;
    }
    if ( !caller->isIdentical ( sbNumber->value() ) ) {
        QMessageBox::warning ( ( QWidget * ) this,tr ( "Error:" ),tr ( "The Value of Serial Number must be unique! Please change it!" ) );
        return 0;
    }

    if ( dirView->sDir.isEmpty() ) {
        QMessageBox::warning ( ( QWidget * ) this,tr ( "Error:" ),tr ( "You haven't selected a directory! Please select one!" ) );
        return 0;
    }

    OK = 1;
    type   = cbType->currentItem() +1;
    serial = sbNumber->value();
    dName  = leName->text();
    dOwner = leOwner->text();
    dComm  = teComm->text();
    dDir   = dirView->sDir;

    close();
    return 0;
}

int addDialog::bCan ( void ) {
    OK = 0;
    close();
    return 0;
}



void addDialog::autoDetectAtMountToggled() {
#ifndef _WIN32
	if (cbAutoDetectAtMount->isChecked())
		leName->setEnabled(false);
	else
		leName->setEnabled(true);
#endif
}


/**************************************************************************/

PWw::PWw ( QWidget *parent,QApplication *qapp ) : QWidget ( parent,"PleaseWaitBox",
#ifdef QT_NO_COMPAT
                Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WShowModal
#else
                Qt::WType_Modal  | Qt::WStyle_Customize | Qt::WStyle_NoBorder
#endif
                                                                  ) {
    int i;
    QFont ownf;
    refreshTime = 100;
    appl=qapp;
    setMinimumSize ( 80,68 );
    setMaximumSize ( 80,68 );

    setGeometry ( parent->x() + ( ( parent->width()-80 ) /2 ),
                  parent->y() + ( ( parent->height()-68 ) /2 ),
                  80,
                  68 );

    /* Calculate the necesary font size*/
    QFontMetrics *fm=NULL;
    ownf = font();
    i=15;
    do {
        i--;
        ownf.setPointSize ( i );
        if ( fm != NULL ) delete fm;
        fm = new QFontMetrics ( ownf );
        if ( i<4 ) break;
    } while ( fm->width ( tr ( "Please Wait..." ) ) > ( width()-5 ) );
    begintext = ( width() - fm->width ( tr ( "Please Wait..." ) ) ) / 2;

    if ( fm != NULL ) delete fm;
    setFont ( ownf );

    /*Get the firt value of the timer*/
    t=QTime::currentTime();
    s=0;
    show();
}

void PWw::end ( void ) {
    close();
}

void PWw::step ( void ) {
    QTime tt;
    tt=QTime::currentTime();
    if ( t.msecsTo ( tt ) < refreshTime ) return;
    t = tt;

    if ( !isActiveWindow() )
        show();

    repaint();

    if ( appl != NULL )
        appl->processEvents();
}

void PWw::paintEvent ( QPaintEvent *e ) {
    QPainter p ( this );

    p.setClipping ( FALSE );
    p.drawRect ( 1,1,78,66 );
    p.drawText ( begintext,18,tr ( "Please Wait..." ) );
    p.drawPixmap ( 20,25,* ( get_anim ( s ) ) );
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
    p.beginNativePainting();
#endif
    if ( ++s==5 ) s=0;
}

void PWw::mousePressEvent ( QMouseEvent *me ) {
    lastx = me->x();
    lasty = me->y();
}

void PWw::mouseMoveEvent ( QMouseEvent *me ) {
    move ( me->globalX()-lastx,me->globalY()-lasty );
}

void progress ( PWw *p ) {
    if ( p != NULL )
        p->step();
}

/**************************************************************************/

AddLnk::AddLnk ( QWidget *parent )
        : QDialog ( parent,"addlnkdialog",true ) {
    setName ( "addlnkdialog" );
    setIcon ( *get_p_icon() );

    setSizeGripEnabled ( TRUE );

    vbox  = new Q3VBoxLayout ( this,5,5 );


    hbox1 = new Q3HBoxLayout ( 0,5,5 );
    hbox2 = new Q3HBoxLayout ( 0,5,5 );

    label = new QLabel ( this, "textLabel6" );

    fname = new QLineEdit ( this, "FnameLineedit" );
    bselect = new QPushButton ( this,"SelectFname" );
    bselect->setText ( "..." );
    bselect->setMaximumWidth ( 30 );

    hbox1->addWidget ( fname );
    hbox1->addWidget ( bselect );

    buttonOk = new QPushButton ( this,"bok" );
    buttonOk->setMinimumWidth ( 80 );
    buttonCancel = new QPushButton ( this,"bcan" );
    buttonCancel->setMinimumWidth ( 80 );

    hbox2->addStretch();
    hbox2->addWidget ( buttonOk );
    hbox2->addWidget ( buttonCancel );
    hbox2->addStretch();

    vbox->addWidget ( label );
    vbox->addLayout ( hbox1 );
    vbox->addStretch();
    vbox->addLayout ( hbox2 );
    resize ( 350,100 );
    languageChange();

    connect ( buttonOk,SIGNAL ( clicked() ),this,SLOT ( sok() ) );
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( scancel() ) );
    connect ( bselect,SIGNAL ( clicked() ),this,SLOT ( sselect() ) );
    ok = false;
}


void AddLnk::languageChange() {
    setCaption ( tr ( "Add a \"Catalog Link\" to the database" ) );
    label->setText ( tr ( "Location of the .hcf file:" ) );

    buttonCancel->setText ( tr ( "Cancel" ) );
    buttonOk->setText ( tr ( "Ok" ) );
}

int AddLnk::sok ( void ) {
    ok = true;
    close();
    return 0;
}

int AddLnk::scancel ( void ) {
    ok = false;
    close();
    return 0;
}

int AddLnk::sselect ( void ) {
    QString fn;

    fn = Q3FileDialog::getOpenFileName ( NULL,tr ( "CdCat databases (*.hcf )" ),this,"openlnfiled",tr ( "Open a file..." ) );
    if ( fn.isEmpty() )  return 0;

    fname->setText ( fn );
    return 0;
}

