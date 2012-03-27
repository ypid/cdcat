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
#include <qfiledialog.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPaintEvent>
#include <Q3GridLayout>
#include <QMouseEvent>
#include <Q3VBoxLayout>

#include <QInputDialog>

#include <iostream>

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
	dirView->resize ( 190, 210 );
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
	textLabel1->setBuddy ( leName );
	layout7->addWidget ( leName );

	layout2 = new Q3HBoxLayout ( 0, 0, 6, "layout2" );

	textLabel2 = new QLabel ( this, "textLabel2" );
	layout2->addWidget ( textLabel2 );

	sbNumber = new QSpinBox ( 0, 50000, 1, this, "sbNumber" );
	layout2->addWidget ( sbNumber );
	textLabel2->setBuddy ( sbNumber );
	layout7->addLayout ( layout2 );

	layout3 = new Q3HBoxLayout ( 0, 0, 6, "layout3" );

	textLabel4 = new QLabel ( this, "textLabel4" );
	textLabel4->setBuddy ( textLabel4 );
	layout3->addWidget ( textLabel4 );

	cbType = new QComboBox ( FALSE, this, "cbType" );
	layout3->addWidget ( cbType );
	layout7->addLayout ( layout3 );

	layout4 = new Q3HBoxLayout ( 0, 0, 6, "layout4" );

	textLabel5 = new QLabel ( this, "textLabel5" );
	layout4->addWidget ( textLabel5 );

	leOwner = new QLineEdit ( this, "leOwner" );
	layout4->addWidget ( leOwner );
	textLabel5->setBuddy ( leOwner );
	layout7->addLayout ( layout4 );

	layout11 = new Q3HBoxLayout ( 0, 0, 6, "layout11" );

	textLabelCategory = new QLabel ( this, "textLabelCategory" );
	layout11->addWidget ( textLabelCategory );

	leCategory = new QLineEdit ( this, "leCategory" );
	layout11->addWidget ( leCategory );
	textLabelCategory->setBuddy ( leCategory );
	layout7->addLayout ( layout11 );

	layout5 = new Q3GridLayout ( 0, 1, 1, 0, 6, "layout5" );

	textLabel3 = new QLabel ( this, "textLabel3" );
	textLabel3->setBuddy ( textLabel3 );

	layout5->addWidget ( textLabel3, 0, 0 );

	teComm = new Q3MultiLineEdit ( this, "teComm" );

	layout5->addMultiCellWidget ( teComm, 0, 1, 1, 1 );
	QSpacerItem* spacer = new QSpacerItem ( 20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout5->addItem ( spacer, 1, 0 );
	layout7->addLayout ( layout5 );

	buttonPli = new QPushButton ( this, "buttonReadable" );
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

	connect ( buttonOK, SIGNAL ( clicked() ), this, SLOT ( bOk() ) );
	connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( bCan() ) );
	connect ( buttonPli, SIGNAL ( clicked() ), this, SLOT ( sread() ) );
	connect ( dirView, SIGNAL ( folderSelected ( const QString & ) ), this, SLOT ( setMediaName ( const QString & ) ) );
	connect ( cbType, SIGNAL ( activated ( int ) ), this, SLOT ( cbTypeToggeled ( int ) ) );
	
#ifndef _WIN32
	connect ( cbAutoDetectAtMount, SIGNAL ( clicked() ), this, SLOT ( autoDetectAtMountToggled() ) );
	cbAutoDetectAtMount->setChecked ( true );
#endif
	
	
	for ( i = 1; !caller->isIdentical ( i ); i++ )
		{ };
	sbNumber->setValue ( i );

	//volumename = 1; //so, the next line will set up the name
	//setMediaName ( " " );

	type = caller->mainw->cconfig->lastMediaType;
	cbType->setCurrentIndex ( caller->mainw->cconfig->lastMediaType - 1 );


	buttonOK->setFocus();

	resize ( QSize ( ( ( CdCatMainWidget * ) parent )->cconfig->addWidth, ( ( CdCatMainWidget * ) parent )->cconfig->addHeight ).expandedTo ( minimumSizeHint() ) );
	move ( ( ( CdCatMainWidget * ) parent )->cconfig->addX, ( ( CdCatMainWidget * ) parent )->cconfig->addY );

	if ( cbType->currentItem() + 1 == CD || cbType->currentItem() + 1 == DVD ) {
		dirView->setDir ( ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath );
		dirView->sDir = ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath;
		//    setMediaName ( ( ( CdCatMainWidget * ) parent )->cconfig->cdrompath );
	}
	else {
		dirView->setDir ( ( ( CdCatMainWidget * ) parent )->cconfig->lastDir );
		dirView->sDir = ( ( CdCatMainWidget * ) parent )->cconfig->lastDir;
		leName->setText (dirView->currentItem()->text(0));
	}
}

int addDialog::sread() {
	SelReadable *sr = new SelReadable ( caller->mainw->cconfig, this, "pluginsconf", true );
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
	textLabel1->setText ( tr ( "Media &Name:" ) );
	textLabel2->setText ( tr ( "S&erial number of Media:" ) );
	textLabel5->setText ( tr ( "&Owner:" ) );
	textLabelCategory->setText ( tr ( "C&ategory:" ) );
	textLabel4->setText ( tr ( "&Type:" ) );
	textLabel3->setText ( tr ( "Co&mment:" ) );
	buttonCancel->setText ( tr ( "&Cancel" ) );
	buttonOK->setText ( tr ( "&Scan" ) );
	buttonPli->setText ( tr ( "Select &readable items" ) );
#ifndef _WIN32
	cbAutoDetectAtMount->setText ( tr ( "detect CDROM/DVD med&ia name after mount" ) );
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
	//std::cerr << "mediatype " << cbType->currentItem() +1 << std::endl;

#ifdef _WIN32
	QDir confdir ( ( caller->mainw->cconfig->cdrompath ).lower() );
	QDir selected ( ds.lower() );
#else
	QDir confdir ( caller->mainw->cconfig->cdrompath );
	QDir selected ( ds );
#endif
	QApplication::setOverrideCursor ( Qt::waitCursor );

	if ( cbType->currentItem() + 1 == CD  || cbType->currentItem() + 1 == DVD )  {
		//std::cerr << "setMediaName: mediatype is cd/dvd"<< std::endl;

		if ( confdir  == selected ) {
			tm = getCDName ( caller->mainw->cconfig->cdrompath );
		}
		if ( confdir  == selected && ! ( tm.isEmpty() ) ) {


#ifdef _WIN32
			if ( ( cbType->currentItem() + 1 == CD &&  cbType->currentItem() + 1 == DVD ) && ( confdir  == selected ) ) {
				if ( !caller->mainw->cconfig->cdrompath.replace ( "/", "\\" ).isEmpty() ) {
					tm = getCDName ( caller->mainw->cconfig->cdrompath.replace ( "/", "\\" ) );
				}
			}
			else {
				//QMessageBox::warning ( ( QWidget * ) this,tr ( "Warning:" ),tr ( "Trying selected drive name" ) );
				tm = getCDName ( dirView->sDir.replace ( "/", "\\" ) );
			}
#endif
			if ( !tm.isEmpty() ) {
				leName->setText ( tm );
#ifndef _WIN32

				// also set the media type to DVD if needed
				if ( diskIsDVD ( caller->mainw->cconfig->cdrompath ) )
					cbType->setCurrentItem ( DVD - 1 );
				else
					cbType->setCurrentItem ( CD - 1 );
#endif
				volumename = 1;
			}
			else {
				volumename = 0;
			}
			if ( volumename == 0 ) {
				volumename = 0;
				tm = tr ( "New Disk %1" ).arg ( sbNumber->value() );
				leName->setText ( tm );
			}
		}
	}
	else {
		//std::cerr << "setMediaName: mediatype is not cd/dvd"<< std::endl;
#ifndef _WIN32
		tm = dirView->sDir.split ( '/' ).at ( dirView->sDir.split ( '/' ).size() - 2 );
#else
		tm = dirView->sDir.split ( '/' ).at ( dirView->sDir.split ( '/' ).size() - 2 );
#endif
		leName->setText ( tm );
	}
	//std::cerr << "setMediaName: sDir: " << qPrintable(dirView->sDir)<< std::endl;
	caller->mainw->cconfig->lastDir = dirView->sDir;
	QApplication::restoreOverrideCursor();
	return 0;
}

int addDialog::bOk ( void ) {
	if ( ( leName->text() ).isEmpty() && ! ( cbType->currentItem() + 1 == CD || cbType->currentItem() + 1 == DVD ) ) {
		QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "You must be fill the \"Name\" field!" ) );
		return 0;
	}

	if ( ( leName->text() ).startsWith ( "@" ) ) {
		QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "The media name can't begin with the \"@\" character!" ) );
		return 0;
	}

	if ( !caller->isIdentical ( leName->text() ) ) {
		//QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "The Media Name must be unique! Please change it!" ) );
		bool ok=false;
		QString text = QInputDialog::getText ( 0, tr ( "Enter media name..." ), tr ( "The Media Name must be unique! Enter new media name:" ), QLineEdit::Normal, leName->text()+".1", &ok );
		if ( ok && !text.isEmpty() && caller->isIdentical ( text)) {
			leName->setText ( text );
			dName  = text;
		}
		else {
			return 0;
		}
	}
	if ( !caller->isIdentical ( sbNumber->value() ) ) {
		QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "The Value of Serial Number must be unique! Please change it!" ) );
		return 0;
	}

	if ( dirView->sDir.isEmpty() ) {
		QMessageBox::warning ( ( QWidget * ) this, tr ( "Error:" ), tr ( "You haven't selected a directory! Please select one!" ) );
		return 0;
	}

	OK = 1;
	type   = cbType->currentItem() + 1;
	serial = sbNumber->value();
	dName  = leName->text();
	dOwner = leOwner->text();
	dComm  = teComm->text();
	dCategory = leCategory->text();
	dDir   = dirView->sDir;
	if(dDir.rightRef(1).toString() == "/")
		dDir = dDir.leftRef(dDir.size()-1).toString();

	caller->mainw->cconfig->addX      = x();
	caller->mainw->cconfig->addY      = y();
	caller->mainw->cconfig->addWidth  = width();
	caller->mainw->cconfig->addHeight = height();
	close();
	return 0;
}

int addDialog::bCan ( void ) {
	OK = 0;
	caller->mainw->cconfig->addX      = x();
	caller->mainw->cconfig->addY      = y();
	caller->mainw->cconfig->addWidth  = width();
	caller->mainw->cconfig->addHeight = height();
	close();
	return 0;
}



void addDialog::autoDetectAtMountToggled() {
#ifndef _WIN32
	if ( cbAutoDetectAtMount->isChecked() )
		leName->setEnabled ( false );
	else
		leName->setEnabled ( true );
#endif
}

void addDialog::cbTypeToggeled ( int ) {
	if ( caller->mainw->cconfig->lastMediaType != cbType->currentItem() + 1 ) {
// 	std::cerr << "mediatype changed from " << caller->mainw->cconfig->lastMediaType << " to " << cbType->currentItem() +1 << std::endl;
		caller->mainw->cconfig->lastMediaType = cbType->currentItem() + 1;
		caller->mainw->cconfig->writeConfig();
	}
}


/**************************************************************************/

PWw::PWw ( QWidget *parent, QApplication *qapp, bool showProgress, long long int steps, QString progresstext, bool showCancel ) : QWidget ( parent, "PleaseWaitBox",
#ifdef QT_NO_COMPAT
	                Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WShowModal
#else
	                Qt::WType_Modal  | Qt::WStyle_Customize | Qt::WStyle_NoBorder
#endif
	                                                                                                                                          ) {
	int i;
	QFont ownf;
	refreshTime = 250;
	appl = qapp;
	cancelButton = NULL;
	this->showProgress = showProgress;
	this->showCancel = showCancel;
	doCancel = false;
	if(showCancel) {
		if (cancelButton == NULL) {
			cancelButton = new QPushButton(this);
			connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(doCancelReceived(bool)));
			cancelButton->setText( tr("&Cancel") );
		}
	}
	this->steps = steps;
	if ( !progresstext.isEmpty() )
		this->progresstext = progresstext;
	else
		this->progresstext = tr ( "Please Wait..." );
	

	baseheight = 50;
	if ( showProgress )
		baseheight += 40;

	if ( showCancel )
		baseheight += 50;

	mywidth = 160;
	myheight = baseheight;

	/* Calculate the necesary font size*/
	ownf = font();
	i = 10;
//     do {
//         i--;
//         ownf.setPointSize ( i );
//         if ( fm != NULL ) delete fm;
//         fm = new QFontMetrics ( ownf );
//         if ( i<10 ) break;
//     } while ( fm->width ( this->progresstext ) > ( width()-5 ) );

	ownf.setPointSize ( i );
	QFontMetrics fm ( ownf );
	mywidth = ( fm.width ( this->progresstext ) ) + 10;
	myheight = fm.height() + baseheight;
	fontheight = fm.height();

	if ( showCancel && cancelButton != NULL ) {
		//myheight += fm.height() + 10;
		//begincanceltext = mywidth / 2 - fm.width ( tr ( "Cancel" ) ) / 2;
		myheight += cancelButton->height()+4;
	}

//     std::cerr << "mywidth: " << mywidth << std::endl;


	setMinimumSize ( 10, myheight );
	setMaximumSize ( mywidth, myheight );

	setGeometry ( parent->x() + ( ( parent->width() - mywidth ) / 2 ),
	              parent->y() + ( ( parent->height() - myheight ) / 2 ),
	              mywidth, myheight );

	begintext = 5;

	setFont ( ownf );


	anim_list = get_anim_list();

	/*Get the firt value of the timer*/
	t = QTime::currentTime();
	s = 0;
	show();
}

PWw::~PWw() {
	if(cancelButton != NULL) {
		disconnect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(doCancelReceived(bool)));
		delete(cancelButton);
	}
}


void PWw::end ( void ) {
	close();
}

void PWw::setProgressText ( QString progresstext ) {
	int i;
	QFont ownf;
	if ( !progresstext.isEmpty() )
		this->progresstext = progresstext;

	/* Calculate the necesary font size*/
	ownf = font();
	i = 10;
//     do {
//         i--;
//         ownf.setPointSize ( i );
//         if ( fm != NULL ) delete fm;
//         fm = new QFontMetrics ( ownf );
//         if ( i<4 ) break;
//     } while ( fm->width ( this->progresstext ) > ( width()-5 ) );

	ownf.setPointSize ( i );
	QFontMetrics fm ( ownf );
	fontheight = fm.height();
	baseheight = 30;
	
	baseheight += 40;
	
	mywidth = ( fm.width ( progresstext ) ) + 10;
	myheight = fm.height() + baseheight;
	
	if ( showCancel ) {
		//myheight += fontheight+10;
		//begincanceltext = mywidth/2- fm.width ( tr("Cancel") )/2;
		if(cancelButton != NULL)
			myheight += cancelButton->height() + 4;
		//std::cerr << "setCancel!" << std::endl;
	}

	begintext = 5;

	setFont ( ownf );

	if ( width() != mywidth ) {
		hide();
		setMinimumSize ( mywidth, myheight );
		setMaximumSize ( mywidth, myheight );
		setGeometry ( x(), y(), mywidth, myheight );
		show();
	}
}

void PWw::setCancel ( bool showCancel ) {
	this->showCancel = showCancel;
	setProgressText ( this->progresstext );
	if(showCancel) {
		if (cancelButton == NULL) {
			cancelButton = new QPushButton(this);
			cancelButton->setText( tr("&Cancel") );
			connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(doCancelReceived(bool)));
		}
		cancelButton->show();
		resize(width(), height()+cancelButton->height()+10);
	}
	else {
		if (cancelButton != NULL) {
			cancelButton->hide();
			resize(width(), height()-cancelButton->height()-10);
		}
	}
}

void PWw::step ( long long int progress_step ) {
	QTime tt;
	tt = QTime::currentTime();

	if ( appl != NULL )
		if ( appl->hasPendingEvents() )
			appl->processEvents();

	if ( t.msecsTo ( tt ) < refreshTime )
		return;
	t = tt;

	if ( showProgress )
		this->progress_step = progress_step;

	//if ( !isActiveWindow() )
	//	show();
	repaint();


}

void PWw::paintEvent ( QPaintEvent * ) {
	QPainter p ( this );
	p.setClipping ( FALSE );
	int borderless_width = mywidth - 4;
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
	p.beginNativePainting();
#endif
	p.drawText ( begintext, 18, progresstext );
	p.drawRect ( 1, 1, borderless_width, myheight - 2 );

	QPixmap pm = anim_list.at ( s );
	p.drawPixmap ( ( mywidth / 2 ) - ( pm.width() / 2 ), 25, pm );
	int buttom_offset = fontheight + pm.height() + 15;
	
	if ( showProgress ) {
		p.setPen ( QPen ( QColor().black() ) );
		p.drawRect ( 1, buttom_offset - 1, mywidth - 4, 15 );
		int percent = 0;
		int steps_hundered = ( steps / 100 );
// 		std::cerr << progress_step << "/"<< steps <<  " steps/100: " << (steps/100) << std::endl;
		if ( steps_hundered != 0 )
			percent = progress_step / ( steps / 100 );
		p.setBrush ( QBrush ( Qt::blue ) );
		p.drawRect ( 2, buttom_offset, ( borderless_width - 4 ) *percent / 100, 14 );

		//         std::cerr << progress_step << "/"<< steps <<  " p: " << percent << "%" << std::endl;
	}
	if(showCancel && cancelButton != NULL) {
	//	buttom_offset += 30;
	//	p.setBrush(QBrush(Qt::NoBrush));
	//	p.drawText (begincanceltext, buttom_offset, tr("Cancel") );
		buttom_offset += 2;
		cancelButton->move(2, buttom_offset+14 + 2);
		cancelButton->resize(width()-4, cancelButton->height());
	}
	
	
	


#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
	p.endNativePainting();
#endif

	if ( ++s == 5 )
		s = 0;

}

void PWw::mousePressEvent ( QMouseEvent *me ) {
	lastx = me->x();
	lasty = me->y();
	if ( showCancel ) {
		int buttom_offset = myheight - 15;
		if ( lastx > 0 && lastx < mywidth && lasty > buttom_offset && lasty < buttom_offset + 16 ) {
// 		std::cerr << "cancel clicked!" << std::endl;
			emit cancelReceivedByUser ( true );
		}
	}
}

void PWw::mouseMoveEvent ( QMouseEvent *me ) {
	move ( me->globalX() - lastx, me->globalY() - lasty );
}

void PWw::doCancelReceived ( bool ) {
	doCancel = true;
}

void progress ( PWw *p, long long int progress_step ) {
	if ( p != NULL )
		p->step ( progress_step );
}


/**************************************************************************/

AddLnk::AddLnk ( GuiSlave *c, QWidget *parent )
	: QDialog ( parent, "addlnkdialog", true ) {

	caller = c;
	setName ( "addlnkdialog" );
	setIcon ( *get_p_icon_big() );

	setSizeGripEnabled ( TRUE );

	vbox  = new Q3VBoxLayout ( this, 5, 5 );


	hbox1 = new Q3HBoxLayout ( 0, 5, 5 );
	hbox2 = new Q3HBoxLayout ( 0, 5, 5 );

	label = new QLabel ( this, "textLabel6" );

	fname = new QLineEdit ( this, "FnameLineedit" );
	bselect = new QPushButton ( this, "SelectFname" );
	bselect->setText ( "..." );
	bselect->setMaximumWidth ( 30 );

	hbox1->addWidget ( fname );
	hbox1->addWidget ( bselect );

	buttonOk = new QPushButton ( this, "bok" );
	buttonOk->setMinimumWidth ( 80 );
	buttonCancel = new QPushButton ( this, "bcan" );
	buttonCancel->setMinimumWidth ( 80 );

	hbox2->addStretch();
	hbox2->addWidget ( buttonOk );
	hbox2->addWidget ( buttonCancel );
	hbox2->addStretch();

	vbox->addWidget ( label );
	vbox->addLayout ( hbox1 );
	vbox->addStretch();
	vbox->addLayout ( hbox2 );
	resize ( 350, 100 );
	languageChange();

	connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( sok() ) );
	connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( scancel() ) );
	connect ( bselect, SIGNAL ( clicked() ), this, SLOT ( sselect() ) );
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
	fn = QFileDialog::getOpenFileName ( 0, tr ( "Open a file..." ), caller->mainw->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ) );
	if ( fn.isEmpty() )
		return 0;

	fname->setText ( fn );
	return 0;
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
