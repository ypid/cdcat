/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "adddialog.h"
#include "guibase.h"
#include "dbase.h"

#include <QVariant>
#include <qcombobox.h>
#include <QLabel>
#include <qlineedit.h>
#include <QPushButton>
#include <qspinbox.h>
#include <QCheckBox>
#include <QTextEdit>
#include <QLayout>
#include <QToolTip>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QDateTime>
#include <QApplication>
#include <QFontMetrics>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QGridLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include <QInputDialog>

#include <iostream>

#include "dirview.h"
#include "icons.h"
#include "mainwidget.h"
#include "config.h"
#include "misc.h"
#include "selreadable.h"

addDialog::addDialog ( GuiSlave *c, QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    int i;

    setModal( modal );

    caller = c;
    if (!name) {
        setObjectName( "addDialog" );
    }
    setWindowIcon( *get_t_add_icon());

    setSizeGripEnabled( true );
    addDialogLayout = new QGridLayout( this );

    layout10 = new QVBoxLayout( this );

    layout9 = new QHBoxLayout( this );

    layout8 = new QVBoxLayout( this );

    textLabel6 = new QLabel( this );
    layout8->addWidget( textLabel6 );

    dirView = new DirectoryView( this );
    dirView->resize( 190, 210 );
    dirView->setGeometry( QRect( 10, 40, 190, 210 ));
    dirView->setWindowTitle( tr( "Directory Browser" ));

    layout8->addWidget( dirView );

    layout9->addLayout( layout8 );

    layout7 = new QVBoxLayout( this );

    textLabel1 = new QLabel( this );
    layout7->addWidget( textLabel1 );

#if !defined(_WIN32) && !defined(_OS2)
    cbAutoDetectAtMount = new QCheckBox( this );
    layout7->addWidget( cbAutoDetectAtMount );
#endif

    leName = new QLineEdit( this );
    textLabel1->setBuddy( leName );
    layout7->addWidget( leName );

    layout2 = new QHBoxLayout( this );

    textLabel2 = new QLabel( this );
    layout2->addWidget( textLabel2 );

    sbNumber = new QSpinBox( this );
    sbNumber->setMinimum( 0 );
    sbNumber->setMaximum( 50000 );
    sbNumber->setSingleStep( 1 );
    layout2->addWidget( sbNumber );
    textLabel2->setBuddy( sbNumber );
    layout7->addLayout( layout2 );

    layout3 = new QHBoxLayout( this );

    textLabel4 = new QLabel( this );
    textLabel4->setBuddy( textLabel4 );
    layout3->addWidget( textLabel4 );

    cbType = new QComboBox( this );
    layout3->addWidget( cbType );
    layout7->addLayout( layout3 );

    layout4 = new QHBoxLayout( this );

    textLabel5 = new QLabel( this );
    layout4->addWidget( textLabel5 );

    leOwner = new QLineEdit( this );
    layout4->addWidget( leOwner );
    textLabel5->setBuddy( leOwner );
    layout7->addLayout( layout4 );

    layout11 = new QHBoxLayout( this );

    textLabelCategory = new QLabel( this );
    layout11->addWidget( textLabelCategory );

    leCategory = new QLineEdit( this );
    layout11->addWidget( leCategory );
    textLabelCategory->setBuddy( leCategory );
    layout7->addLayout( layout11 );

    layout5 = new QGridLayout( this );

    textLabel3 = new QLabel( this );
    textLabel3->setBuddy( textLabel3 );

    layout5->addWidget( textLabel3, 0, 0 );

    teComm = new QTextEdit( this );

    layout5->addWidget( teComm, 0, 1, 1, 1 );
    QSpacerItem *spacer = new QSpacerItem( 20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout5->addItem( spacer, 1, 0 );
    layout7->addLayout( layout5 );

    buttonPli = new QPushButton( this );
    layout7->addWidget( buttonPli );

    layout9->addLayout( layout7 );
    layout10->addLayout( layout9 );

    layout1 = new QHBoxLayout( this );
    QSpacerItem *spacer_2 = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer_2 );

    buttonOK = new QPushButton( this );
    buttonOK->setMinimumWidth( 100 );
    layout1->addWidget( buttonOK );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumWidth( 100 );
    layout1->addWidget( buttonCancel );
    QSpacerItem *spacer_3 = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer_3 );
    layout10->addLayout( layout1 );

    addDialogLayout->addLayout( layout10, 0, 0 );
    languageChange();
    resize( QSize( 454, 350 ).expandedTo( minimumSizeHint()));

    connect( buttonOK, SIGNAL( clicked()), this, SLOT( bOk()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( bCan()));
    connect( buttonPli, SIGNAL( clicked()), this, SLOT( sread()));
    connect( dirView, SIGNAL( folderSelected( const QString & )), this, SLOT( setMediaName( const QString & )));
    connect( dirView, SIGNAL( setExpandedInProgress( bool )), this, SLOT( setBusyState( bool )));
    connect( cbType, SIGNAL( activated( int )), this, SLOT( cbTypeToggeled( int )));



#if !defined(_WIN32) && !defined(_OS2)
    connect( cbAutoDetectAtMount, SIGNAL( clicked()), this, SLOT( autoDetectAtMountToggled()));
    cbAutoDetectAtMount->setChecked( true );
#endif


    for (i = 1; caller->isIdentical( i ); i++) {
    }
    ;
    sbNumber->setValue( i );

    // volumename = 1; //so, the next line will set up the name
    // setMediaName ( " " );

    type = caller->mainw->cconfig->lastMediaType;
    cbType->setCurrentIndex( caller->mainw->cconfig->lastMediaType );


    buttonOK->setFocus();

    resize( QSize(((CdCatMainWidget *)parent)->cconfig->addWidth, ((CdCatMainWidget *)parent)->cconfig->addHeight ).expandedTo( minimumSizeHint()));
    move(((CdCatMainWidget *)parent)->cconfig->addX, ((CdCatMainWidget *)parent)->cconfig->addY );

    if (cbType->currentIndex() == CD || cbType->currentIndex() == DVD) {
        dirView->setDir(((CdCatMainWidget *)parent)->cconfig->cdrompath );
        dirView->sDir = ((CdCatMainWidget *)parent)->cconfig->cdrompath;
        //    setMediaName ( ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath );
    } else {
        dirView->setDir(((CdCatMainWidget *)parent)->cconfig->lastDir );
        dirView->sDir = ((CdCatMainWidget *)parent)->cconfig->lastDir;
        if (dirView->currentItem() != NULL) {
            leName->setText( dirView->currentItem()->text( 0 ));
        }
    }
}

int addDialog::sread() {
    SelReadable *sr = new SelReadable( caller->mainw->cconfig, this, "pluginsconf", true );

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
    buttonOK->setText( tr( "Add Media to the Database" ));
    textLabel6->setText( tr( "Choose a directory to scan:" ));
    textLabel1->setText( tr( "Media &Name:" ));
    textLabel2->setText( tr( "S&erial number of Media:" ));
    textLabel5->setText( tr( "&Owner:" ));
    textLabelCategory->setText( tr( "C&ategory:" ));
    textLabel4->setText( tr( "&Type:" ));
    textLabel3->setText( tr( "Co&mment:" ));
    buttonCancel->setText( tr( "&Cancel" ));
    buttonOK->setText( tr( "&Scan" ));
    buttonPli->setText( tr( "Select &readable items" ));
#if !defined(_WIN32) && !defined(_OS2)
    cbAutoDetectAtMount->setText( tr( "detect CDROM/DVD med&ia name after mount" ));
#endif
    cbType->clear();
    cbType->insertItem( 0, QIcon( *get_m_other_icon()), tr( "unknown" ));
    cbType->insertItem( 1, QIcon( *get_m_cd_icon()), tr( "CD" ));
    cbType->insertItem( 2, QIcon( *get_m_dvd_icon()), tr( "DVD" ));
    cbType->insertItem( 3, QIcon( *get_m_hdd_icon()), tr( "HardDisc" ));
    cbType->insertItem( 4, QIcon( *get_m_floppy_icon()), tr( "Floppy" ));
    cbType->insertItem( 5, QIcon( *get_m_net_icon()), tr( "NetworkPlace" ));
    cbType->insertItem( 6, QIcon( *get_m_flash_icon()), tr( "FlashDrive" ));
    cbType->insertItem( 7, QIcon( *get_m_other_icon()), tr( "OtherDevice" ));
//  cbType->setCurrentText(tr( "CD" )); // default
}

int addDialog::setMediaName( const QString & ds ) {
    // std::cerr << "setMediaName(): ds: " << qPrintable(ds) << std::endl;
    QString tm;

    // std::cerr << "mediatype " << cbType->currentIndex() << std::endl;

#if defined(_WIN32) || defined(_OS2)
    QDir selected( ds.toLower());
    QString ds2 = ds;
#else
    QDir selected( ds );
#endif
    QApplication::setOverrideCursor( Qt::WaitCursor );

    if (cbType->currentIndex() == CD || cbType->currentIndex() == DVD) {
        // std::cerr << "setMediaName(): mediatype is cd/dvd"<< std::endl;
        if (cbType->currentIndex() == CD && cbType->currentIndex() == DVD) {
#if defined(_WIN32) || defined(_OS2)

            if (!ds2.replace( "/", "\\" ).isEmpty()) {
                tm = getCDName( ds2.replace( "/", "\\" ).toUtf8().constData());
            }
#else
            if (!ds.isEmpty()) {
                tm = getCDName( ds.toUtf8().constData());
            }
#endif
        } else {
            // QMessageBox::warning ( ( QWidget * ) this,tr ( "Warning:" ),tr ( "Trying selected drive name" ) );
#if defined(_WIN32) || defined(_OS2)
            tm = getCDName( ds2.replace( "/", "\\" ).toUtf8().constData());
#else
            tm = getCDName( ds.toUtf8().constData());
#endif
        }
        // std::cerr << "setMediaName(): tm: " << qPrintable(tm) << std::endl;
        if (!tm.isEmpty()) {
#if !defined(_WIN32) && !defined(_OS2)
            // also set the media type to DVD if needed
            if (diskIsDVD( ds.toUtf8().constData())) {
                cbType->setCurrentIndex( DVD );
            } else {
                cbType->setCurrentIndex( CD );
            }
#endif
            volumename = 1;
            leName->setText( tm );
        } else {
            volumename = 0;
        }
        if (volumename == 0) {
            volumename = 0;
            tm = tr( "New Disk %1" ).arg( sbNumber->value());
            leName->setText( tm );
        }
    } else {
        // std::cerr << "setMediaName: mediatype is not cd/dvd"<< std::endl;
#if !defined(_WIN32) && !defined(_OS2)
        tm = dirView->sDir.split( '/' ).at( dirView->sDir.split( '/' ).size() - 2 );
#else
        tm = dirView->sDir.split( '/' ).at( dirView->sDir.split( '/' ).size() - 2 );
#endif
        leName->setText( tm );

        if (dirView->sDir == "/" && leName->text().isEmpty()) {
            leName->setText( "root" );
            dName = "root";
        }
    }

    bool ok = false;
    QString medianame_tmp = tm;
    medianame_tmp += ".1";
    QString text = tm;
    while (text.isEmpty() || (!text.isEmpty() && caller->isIdentical( text ))) {
        text = QInputDialog::getText( 0, tr( "Enter media name..." ), tr( "The Media Name must be unique! Enter new media name:" ), QLineEdit::Normal, medianame_tmp, &ok );
        if (!ok) {
            return 0;
        }
        if (!text.isEmpty() && !caller->isIdentical( text )) {
            leName->setText( text );
            dName = text;
            break;
        }
    }


    // std::cerr << "setMediaName: sDir: " << qPrintable(dirView->sDir)<< std::endl;
    caller->mainw->cconfig->lastDir = dirView->sDir;

    QApplication::restoreOverrideCursor();
    return 0;
}

int addDialog::bOk( void ) {
    if ((leName->text()).isEmpty() && !(cbType->currentIndex() == CD || cbType->currentIndex() == DVD)) {
        QMessageBox::warning((QWidget *)this, tr( "Error:" ), tr( "You must be fill the \"Name\" field!" ));
        return 0;
    }

    if ((leName->text()).startsWith( "@" )) {
        QMessageBox::warning((QWidget *)this, tr( "Error:" ), tr( "The media name can't begin with the \"@\" character!" ));
        return 0;
    }

    if (caller->isIdentical( leName->text())) {
        // QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "The Media Name must be unique! Please change it!" ) );
        bool ok = false;
        QString medianame_tmp = leName->text();
        medianame_tmp += ".1";
        QString text = "";
        while (text.isEmpty() || (!text.isEmpty() && caller->isIdentical( text ))) {
            text = QInputDialog::getText( 0, tr( "Enter media name..." ), tr( "The Media Name must be unique! Enter new media name:" ), QLineEdit::Normal, medianame_tmp, &ok );
            if (!ok) {
                return 0;
            }
            if (!text.isEmpty() && !caller->isIdentical( text )) {
                leName->setText( text );
                break;
            }
        }
    }
    if (caller->isIdentical( sbNumber->value())) {
        QMessageBox::warning((QWidget *)this, tr( "Error:" ), tr( "The Value of Serial Number must be unique! Please change it!" ));
        return 0;
    }

    if (dirView->sDir.isEmpty()) {
        QMessageBox::warning((QWidget *)this, tr( "Error:" ), tr( "You haven't selected a directory! Please select one!" ));
        return 0;
    }

    OK = 1;
    type = cbType->currentIndex();
    serial = sbNumber->value();
    dName = leName->text();
    dOwner = leOwner->text();
    dComm = teComm->toPlainText();
    dCategory = leCategory->text();
    dDir = dirView->sDir;

    std::cout << "dDir: " << qPrintable( dDir ) << std::endl;
    caller->mainw->cconfig->addX = x();
    caller->mainw->cconfig->addY = y();
    caller->mainw->cconfig->addWidth = width();
    caller->mainw->cconfig->addHeight = height();
    close();
    return 0;
}

int addDialog::bCan( void ) {
    OK = 0;
    caller->mainw->cconfig->addX = x();
    caller->mainw->cconfig->addY = y();
    caller->mainw->cconfig->addWidth = width();
    caller->mainw->cconfig->addHeight = height();
    close();
    return 0;
}



void addDialog::autoDetectAtMountToggled() {
#if !defined(_WIN32) && !defined(_OS2)
    if (cbAutoDetectAtMount->isChecked()) {
        leName->setEnabled( false );
    } else {
        leName->setEnabled( true );
    }
#endif
}

void addDialog::cbTypeToggeled( int ) {
    if (caller->mainw->cconfig->lastMediaType != cbType->currentIndex()) {
//      std::cerr << "mediatype changed from " << caller->mainw->cconfig->lastMediaType << " to " << cbType->currentItem() +1 << std::endl;
        caller->mainw->cconfig->lastMediaType = cbType->currentIndex();
        caller->mainw->cconfig->writeConfig();
    }
}

void addDialog::setBusyState( bool state ) {
    if (state) {
        QApplication::setOverrideCursor( Qt::WaitCursor );
    } else {
        QApplication::restoreOverrideCursor();
    }
}


/**************************************************************************/

PWw::PWw ( QWidget *parent, QApplication *qapp, bool showProgress, long long int steps, QString progresstext, bool showCancel ) : QWidget( parent
#ifdef QT_NO_COMPAT
                                                                                                                                           , Qt::WType_Dialog /*| Qt::FramelessWindowHint*/
#else
                                                                                                                                                              /*, Qt::FramelessWindowHint*/
#endif

                                                                                                                                           ) {
    int i = 0;
    QFont ownf;

    refreshTime = 250;
    appl = qapp;
    cancelButton = NULL;
    this->showProgress = showProgress;
    this->showCancel = showCancel;
    doCancel = false;
    if (showCancel) {
        if (cancelButton == NULL) {
            cancelButton = new QPushButton( this );
            connect( cancelButton, SIGNAL( clicked( bool )), this, SLOT( doCancelReceived( bool )));
            cancelButton->setText( tr( "&Cancel" ));
        }
    }
    this->steps = steps;
    if (!progresstext.isEmpty()) {
        this->progresstext = progresstext;
    } else {
        this->progresstext = tr( "Please Wait..." );
    }

    baseheight = 120;
    if (showProgress) {
        baseheight += 40;
    }

    if (showCancel) {
        baseheight += 50;
    }

    mywidth = 160;
    myheight = baseheight;

    /* Calculate the necesary font size*/
    ownf = font();

    ownf.setPointSize( i );
    QFontMetrics fm( ownf );
    mywidth = (fm.width( this->progresstext )) + 10;
    if (mywidth < 160) {
        mywidth = 160;
    }
    myheight += fm.height();
    fontheight = fm.height();

    if (showCancel && cancelButton != NULL) {
        // myheight += fm.height() + 10;
        // begincanceltext = mywidth / 2 - fm.width ( tr ( "Cancel" ) ) / 2;
        myheight += cancelButton->height() + 4;
    }

//     std::cerr << "mywidth: " << mywidth << std::endl;


    setMinimumSize( 10, myheight );
    setMaximumSize( mywidth, myheight );
    QPoint p( parent->x(), parent->y());
    p = mapFromGlobal( p );
    setGeometry( p.x() + ((parent->width() - mywidth) / 2),
                 p.y() + ((parent->height() - myheight) / 2),
                 mywidth, myheight );

    begintext = 5;

    setFont( ownf );


    anim_list = get_anim_list();

    /*Get the firt value of the timer*/
    t = QTime::currentTime();
    s = 0;
}

PWw::~PWw() {
    if (cancelButton != NULL) {
        disconnect( cancelButton, SIGNAL( clicked( bool )), this, SLOT( doCancelReceived( bool )));
        delete(cancelButton);
    }
}


void PWw::end( void ) {
    close();
}

void PWw::setProgressText( QString progresstext ) {
    int i;
    QFont ownf;

    this->progresstext = progresstext;

    /* Calculate the necesary font size*/
    ownf = font();
    i = 10;

    ownf.setPointSize( i );
    QFontMetrics fm( ownf );

    mywidth = (fm.width( this->progresstext )) + 10;
    begintext = 5;

    setFont( ownf );

    if (width() != mywidth) {
        hide();
        setMinimumSize( mywidth, myheight );
        setMaximumSize( mywidth, myheight );
        setGeometry( x(), y(), mywidth, myheight );
        show();
    }
}

void PWw::setCancel( bool showCancel ) {
    bool cancel_was_set_before = false;

    if (this->showCancel) {
        cancel_was_set_before = true;
    }

    this->showCancel = showCancel;
    setProgressText( this->progresstext );
    if (showCancel) {
        if (cancelButton == NULL) {
            cancelButton = new QPushButton( this );
            cancelButton->setText( tr( "&Cancel" ));
            connect( cancelButton, SIGNAL( clicked( bool )), this, SLOT( doCancelReceived( bool )));
        }
        cancelButton->show();
        if (!cancel_was_set_before) {
            resize( width(), height() + cancelButton->height() + 10 );
        }
    } else {
        if (cancelButton != NULL) {
            cancelButton->hide();
            if (cancel_was_set_before) {
                resize( width(), height() - cancelButton->height() - 10 );
            }
        }
    }
}

void PWw::step( long long int progress_step ) {
    if (appl != NULL) {
        if (appl->hasPendingEvents()) {
            appl->processEvents();
        }
    } else {
        return;
    }

    QTime tt = QTime::currentTime();

    if (t.msecsTo( tt ) < refreshTime) {
        return;
    }

    t = tt;

    if (showProgress) {
        this->progress_step = progress_step;
    }

    // if ( !isActiveWindow() )
    //	show();
    repaint();
}

void PWw::paintEvent( QPaintEvent * ) {
    QPainter p( this );

    p.setClipping( true );
    p.fillRect( QRect( 0, 0, width(), height()), palette().background());

    int borderless_width = mywidth - 4;
#if (QT_VERSION >= QT_VERSION_CHECK( 4, 6, 0 )) // needs Qt 4.6.0 or better
    p.beginNativePainting();
#endif
    p.drawText( begintext, 18, progresstext );
    p.drawRect( 1, 1, borderless_width, myheight - 2 );

    QPixmap pm = anim_list.at( s );
    p.drawPixmap((mywidth / 2) - (pm.width() / 2), 25, pm );
    int buttom_offset = fontheight + pm.height() + 25;

    if (showProgress) {
        p.setPen( QPen( QColor().black()));
        p.drawRect( 1, buttom_offset - 1, mywidth - 4, 15 );
        int percent = 0;
        int steps_hundered = (steps / 100);
//              std::cerr << progress_step << "/"<< steps <<  " steps/100: " << (steps/100) << std::endl;
        if (steps_hundered != 0) {
            percent = progress_step / (steps / 100);
        }
        p.setBrush( QBrush( Qt::blue ));
        p.drawRect( 2, buttom_offset, (borderless_width - 4) * percent / 100, 14 );

        //         std::cerr << progress_step << "/"<< steps <<  " p: " << percent << "%" << std::endl;
    }
    if (showCancel && cancelButton != NULL) {
        //	buttom_offset += 30;
        //	p.setBrush(QBrush(Qt::NoBrush));
        //	p.drawText (begincanceltext, buttom_offset, tr("Cancel") );
        buttom_offset += 2;
        cancelButton->move( 10, buttom_offset + 14 + 2 );
        cancelButton->resize( width() - 20, cancelButton->height());
    }





#if (QT_VERSION >= QT_VERSION_CHECK( 4, 6, 0 )) // needs Qt 4.6.0 or better
    p.endNativePainting();
#endif

    if (++s == 5) {
        s = 0;
    }
}

void PWw::mousePressEvent( QMouseEvent *me ) {
    lastx = me->x();
    lasty = me->y();
    if (showCancel) {
        int buttom_offset = myheight - 15;
        if (lastx > 0 && lastx < mywidth && lasty > buttom_offset && lasty < buttom_offset + 16) {
//              std::cerr << "cancel clicked!" << std::endl;
            emit cancelReceivedByUser( true );
        }
    }
}

void PWw::mouseMoveEvent( QMouseEvent *me ) {
    move( me->globalX() - lastx, me->globalY() - lasty );
}

void PWw::doCancelReceived( bool ) {
    doCancel = true;
}

void progress( PWw *p, long long int progress_step ) {
    if (p != NULL) {
        p->step( progress_step );
    }
}


/**************************************************************************/

AddLnk::AddLnk ( GuiSlave *c, QWidget *parent )
    : QDialog( parent ) {
    setModal( true );

    caller = c;
    setObjectName( "addlnkdialog" );
    setWindowIcon( *get_p_icon_big());

    setSizeGripEnabled( true );

    vbox = new QVBoxLayout( this );


    hbox1 = new QHBoxLayout( this );
    hbox2 = new QHBoxLayout( this );

    label = new QLabel( this );

    fname = new QLineEdit( this );
    bselect = new QPushButton( this );
    bselect->setText( "..." );
    bselect->setMaximumWidth( 30 );

    hbox1->addWidget( fname );
    hbox1->addWidget( bselect );

    buttonOk = new QPushButton( this );
    buttonOk->setMinimumWidth( 80 );
    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumWidth( 80 );

    hbox2->addStretch();
    hbox2->addWidget( buttonOk );
    hbox2->addWidget( buttonCancel );
    hbox2->addStretch();

    vbox->addWidget( label );
    vbox->addLayout( hbox1 );
    vbox->addStretch();
    vbox->addLayout( hbox2 );
    resize( 350, 100 );
    languageChange();

    connect( buttonOk, SIGNAL( clicked()), this, SLOT( sok()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( scancel()));
    connect( bselect, SIGNAL( clicked()), this, SLOT( sselect()));
    ok = false;
}


void AddLnk::languageChange() {
    setWindowTitle( tr( "Add a \"Catalog Link\" to the database" ));
    label->setText( tr( "Location of the .hcf file:" ));

    buttonCancel->setText( tr( "Cancel" ));
    buttonOk->setText( tr( "Ok" ));
}

int AddLnk::sok( void ) {
    ok = true;
    close();
    return 0;
}

int AddLnk::scancel( void ) {
    ok = false;
    close();
    return 0;
}

int AddLnk::sselect( void ) {
    QString fn;

    fn = QFileDialog::getOpenFileName( 0, tr( "Open a file..." ), caller->mainw->cconfig->lastDir, tr( "CdCat databases (*.hcf )" ));
    if (fn.isEmpty()) {
        return 0;
    }

    fname->setText( fn );
    return 0;
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
