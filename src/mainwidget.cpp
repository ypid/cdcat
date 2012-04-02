/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "mainwidget.h"

#include <qvariant.h>
#include <qmenubar.h>
#include <QFrame>
#include <q3header.h>
#include <q3listview.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qfontmetrics.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QList>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QAction>
#include <QPixmap>
#include <QLatin1String>

#include <iostream>

#include <QPlastiqueStyle>

#include <QDockWidget>

#include "commwidget.h"
#include "hdirview.h"
#include "dbase.h"
#include "guibase.h"
#include "config.h"
#include "icons.h"
#include "adddialog.h"

#include <QScrollArea>
#include <QStatusBar>

#include <QMessageBox>

/*
 *  Constructs a CdCatMainWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CdCatMainWidget::CdCatMainWidget ( CdCatConfig *ccp, QApplication *appp, QWidget *parent, const char *name, Qt::WFlags fl )
	: QMainWindow ( parent, name, fl ) {

	main = new QWidget ( this );
	setCentralWidget ( main );
	app = appp;

	int fontHeight = ( new QFontMetrics ( app->font() ) )->height();

	if ( !name )
		setName ( "CdCatMainWidget" );
	setIcon ( *get_p_icon_big() );

	db = NULL;
	cconfig = ccp;
	cconfig->startProgram ( &db, this );

	splitMain = new QSplitter ( this );

	Toolbar = addToolBar ( tr ( "Main toolbar" ) );
	//Toolbar->setFrameShape ( QFrame::StyledPanel );
	//Toolbar->setFrameShadow ( QFrame::Raised );
	//Toolbar->setMaximumHeight ( 36 );
	//Toolbar->setMinimumHeight ( 36 );
	
	DirView = new HDirectoryView ( &db, splitMain, "DirView" );
	DirView->header()->setLabel ( 0, tr ( "Directory Tree" ) );

	listView = new HQListView ( this, splitMain, "listView" );
	listView->header()->setLabel ( 0, tr ( "Name" ) );
	listView->header()->setLabel ( 1, tr ( "Size" ) );
	listView->clear();


	commentWidget = new CommentWidget ( cconfig, app , this, "CommentWiget" );

	QScrollArea *scrollArea = new QScrollArea ( splitMain );
	commentWidget->setScrollArea ( scrollArea );
	scrollArea->setBackgroundRole ( QPalette::Dark );
	scrollArea->setWidget ( commentWidget );
	CommentDock = new QDockWidget ( this );
	CommentDock->setWindowTitle (tr("Comment dock"));
	//scrollArea->resize ( 500, 100 );
	addDockWidget ( (Qt::DockWidgetArea)cconfig->commentDockSize_dockarea, CommentDock );
	CommentDock->setWidget ( scrollArea );
	scrollArea->setWidgetResizable ( true );
	//scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	//scrollArea->resize(commentWidget->sizeHint());
	//commentWidget->resize(scrollArea->size() );



	guis = new GuiSlave ( this );

	/*Menubar*/
	mainMenu = menuBar();
	statusBar()->show();

	QMenu *fileMenu = new QMenu ( this );
	QMenu *editMenu = new QMenu ( this );
	QMenu *findMenu = new QMenu ( this );
	QMenu *inoutMenu = new QMenu ( this );
	QMenu *othersMenu = new QMenu ( this );
	QMenu *helpMenu = new QMenu ( this );

	QAction *new_action = new QAction ( QIcon ( *get_t_new_icon() ), tr ( "&New..." ), this );
	new_action->setShortcuts ( QKeySequence::New );
	new_action->setStatusTip ( tr ( "Create a new catalog" ) );
	connect ( new_action, SIGNAL ( triggered() ), guis, SLOT ( newEvent() ) );
	fileMenu->addAction ( new_action );

	QAction *open_action = new QAction ( QIcon ( *get_t_open_icon() ), tr ( "&Open..." ), this );
	open_action->setShortcuts ( QKeySequence::Open );
	open_action->setStatusTip ( tr ( "Open a existing catalog" ) );
	connect ( open_action, SIGNAL ( triggered() ), guis, SLOT ( openEvent() ) );
	fileMenu->addAction ( open_action );

	QAction *save_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "&Save" ), this );
	save_action->setShortcuts ( QKeySequence::Save );
	save_action->setStatusTip ( tr ( "Save catalog" ) );
	connect ( save_action, SIGNAL ( triggered() ), guis, SLOT ( saveEvent() ) );
	fileMenu->addAction ( save_action );

	QAction *saveas_action = new QAction ( QIcon ( *get_t_saveas_icon() ), tr ( "&Save as..." ), this );
	saveas_action->setShortcuts ( QKeySequence::Open );
	saveas_action->setStatusTip ( tr ( "save catalog with new name" ) );
	connect ( saveas_action, SIGNAL ( triggered() ), guis, SLOT ( saveasEvent() ) );
	fileMenu->addAction ( saveas_action );

	fileMenu->insertSeparator ();
	historyMenu = new QMenu();

	//QAction *history_action = new QMenu (QIcon(*get_t_open_icon()), tr ( "Recent files..." ), this);
	//history_action->setShortcuts(QKeySequence::Open);
	//history_action->setStatusTip(tr("save catalog with new name"));

	fileMenu->addMenu ( historyMenu );
	historyMenu->setTitle ( tr ( "Recent files..." ) );
	historyMenu->setIcon ( QIcon ( *get_t_open_icon() ) );

	for ( int i = cconfig->hlist.size() - 1; i > -1; i-- ) {
		if ( !cconfig->hlist.at ( i ).isEmpty() ) {
			QAction *history_action = new QAction ( QIcon ( *get_p_icon() ), cconfig->hlist.at ( i ), this );
			//history_action->setShortcut( QKeySequence (Qt::CTRL + Qt::Key_W));
			//history_action->setStatusTip(tr("Open catalog history element"));
			connect ( history_action, SIGNAL ( activated ( int ) ), guis, SLOT ( openHistoryElementEvent ( int ) ) );
			historyMenu->addAction ( history_action );
		}
	}

	QAction *close_action = new QAction ( QIcon ( *get_t_close_icon() ), tr ( "Close" ), this );
	close_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_W ) );
	close_action->setStatusTip ( tr ( "Close catalog" ) );
	connect ( close_action, SIGNAL ( triggered() ), guis, SLOT ( closeEvent() ) );
	fileMenu->addAction ( close_action );

	fileMenu->insertSeparator();

	QAction *quit_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "&Quit" ), this );
	quit_action->setShortcuts ( QKeySequence::Close );
	quit_action->setStatusTip ( tr ( "Close program" ) );
	connect ( quit_action, SIGNAL ( triggered() ), this, SLOT ( close() ) );
	fileMenu->addAction ( quit_action );
	Toolbar->addAction ( quit_action );

	QAction *add_action = new QAction ( QIcon ( *get_t_add_icon() ), tr ( "Add media..." ), this );
	add_action->setShortcut ( QKeySequence ( Qt::Key_A ) );
	add_action->setStatusTip ( tr ( "Add new media to catalog" ) );
	connect ( add_action, SIGNAL ( triggered() ), guis, SLOT ( addEvent() ) );
	editMenu->addAction ( add_action );
	Toolbar->addAction ( add_action );

	QAction *addlink_action = new QAction ( QIcon ( *get_p_icon() ), tr ( "Add a link to a CdCat Catalog..." ), this );
	addlink_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_L ) );
	addlink_action->setStatusTip ( tr ( "Add a link to a existing cdcat catalog" ) );
	connect ( addlink_action, SIGNAL ( triggered() ), guis, SLOT ( addlnkEvent() ) );
	editMenu->addAction ( addlink_action );

	QAction *rescan_action = new QAction ( QIcon ( *get_t_rescan_icon() ), tr ( "Rescan media..." ), this );
	rescan_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_R ) );
	rescan_action->setStatusTip ( tr ( "Rescan existing media" ) );
	connect ( rescan_action, SIGNAL ( triggered() ), guis, SLOT ( rescanEvent() ) );
	editMenu->addAction ( rescan_action );

	editMenu->insertSeparator();

	QAction *insert_action = new QAction ( QIcon ( *get_m_import_icon() ), tr ( "Insert Catalog..." ), this );
	//insert_action->setShortcut(QKeySequence::Save);
	insert_action->setStatusTip ( tr ( "Insert catalog into database" ) );
	connect ( insert_action, SIGNAL ( triggered() ), guis, SLOT ( insertcEvent() ) );
	editMenu->addAction ( insert_action );

	editMenu->insertSeparator();

	QAction *rename_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Rename node..." ), this );
	rename_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_E ) );
	rename_action->setStatusTip ( tr ( "Rename node" ) );
	connect ( rename_action, SIGNAL ( triggered() ), guis, SLOT ( renameEvent() ) );
	editMenu->addAction ( rename_action );

	QAction *renumber_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Re-Number media node..." ), this );
	//renumber_action->setShortcuts(QKeySequence::Save);
	renumber_action->setStatusTip ( tr ( "Renumber node" ) );
	connect ( renumber_action, SIGNAL ( triggered() ), guis, SLOT ( renumberEvent() ) );
	editMenu->addAction ( renumber_action );

	//QAction *changemediatype_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Change media type..." ), this);
	////changemediatype_action->setShortcuts(QKeySequence::Save);
	//changemediatype_action->setStatusTip(tr("Change media type"));
	//connect(changemediatype_action, SIGNAL(triggered()), guis, SLOT(typeChangeEvent()));
	//editMenu->addAction(changemediatype_action);

	QAction *delete_action = new QAction ( QIcon ( *get_t_delete_icon() ), tr ( "Delete node" ), this );
	//delete_action->setShortcuts(QKeySequence::Delete);
	delete_action->setStatusTip ( tr ( "Delete node" ) );
	connect ( delete_action, SIGNAL ( triggered() ), guis, SLOT ( deleteEvent() ) );
	editMenu->addAction ( delete_action );

	editMenu->insertSeparator();
	QAction *sortnu_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by number" ), this );
	//sortnu_action->setShortcuts(QKeySequence::Save);
	sortnu_action->setStatusTip ( tr ( "Sort media by number" ) );
	connect ( sortnu_action, SIGNAL ( triggered() ), guis, SLOT ( sortNuEvent() ) );
	editMenu->addAction ( sortnu_action );

	QAction *sortna_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by name" ), this );
	//sortna_action->setShortcuts(QKeySequence::Save);
	sortna_action->setStatusTip ( tr ( "Sort media by name" ) );
	connect ( sortna_action, SIGNAL ( triggered() ), guis, SLOT ( sortNaEvent() ) );
	editMenu->addAction ( sortna_action );

	QAction *sortti_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by time" ), this );
	//sortti_action->setShortcuts(QKeySequence::Save);
	sortti_action->setStatusTip ( tr ( "Sort media by time" ) );
	connect ( sortti_action, SIGNAL ( triggered() ), guis, SLOT ( sortTiEvent() ) );
	editMenu->addAction ( sortti_action );

	QAction *find_action = new QAction ( QIcon ( *get_t_find_icon() ), tr ( "Seek in database..." ), this );
	find_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_F ) );
	find_action->setStatusTip ( tr ( "Seek in database for files and folders" ) );
	connect ( find_action, SIGNAL ( triggered() ), guis, SLOT ( findEvent() ) );
	findMenu->addAction ( find_action );

	QAction *findpanel_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Seek in the panel" ), this );
	findpanel_action->setShortcut ( QKeySequence ( Qt::ALT + Qt::Key_S ) );
	findpanel_action->setStatusTip ( tr ( "Seek in the panel" ) );
	connect ( findpanel_action, SIGNAL ( triggered() ), guis, SLOT ( posEvent() ) );
	findMenu->addAction ( findpanel_action );

	QAction *borrow_action = new QAction ( QIcon ( *get_m_borrow_icon() ), tr ( "Borrowing info..." ), this );
	borrow_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_B ) );
	borrow_action->setStatusTip ( tr ( "Borrowing info..." ) );
	connect ( borrow_action, SIGNAL ( triggered() ), guis, SLOT ( borrowingEvent() ) );
	othersMenu->addAction ( borrow_action );

	QAction *size_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Node size" ), this );
	size_action->setShortcut ( QKeySequence ( Qt::Key_Space ) );
	size_action->setStatusTip ( tr ( "Calculate node size" ) );
	connect ( size_action, SIGNAL ( triggered() ), guis, SLOT ( sizeEvent() ) );
	othersMenu->addAction ( size_action );

	QAction *config_action = new QAction ( QIcon ( *get_t_config_icon() ), tr ( "Configuration..." ), this );
	config_action->setShortcut ( QKeySequence ( Qt::CTRL + Qt::Key_G ) );
	config_action->setStatusTip ( tr ( "Edit cdcat configuration" ) );
	connect ( config_action, SIGNAL ( triggered() ), guis, SLOT ( configEvent() ) );
	othersMenu->addAction ( config_action );

	QAction *color_action = new QAction ( QIcon ( *get_t_colorconfig_icon() ), tr ( "Set Colors..." ), this );
	//color_action->setShortcuts(QKeySequence::Save);
	color_action->setStatusTip ( tr ( "Set the colors for display" ) );
	connect ( color_action, SIGNAL ( triggered() ), guis, SLOT ( colorEvent() ) );
	othersMenu->addAction ( color_action );

	QAction *import_action = new QAction ( QIcon ( *get_m_import_icon() ), tr ( "Import database (CSV/XML)" ), this );
	//import_action->setShortcuts(QKeySequence::Save);
	import_action->setStatusTip ( tr ( "Import database (CSV/XML) from various catalog programs" ) );
	connect ( import_action, SIGNAL ( triggered() ), guis, SLOT ( importEvent() ) );
	inoutMenu->addAction ( import_action );

	QAction *export_action = new QAction ( QIcon ( *get_m_export_icon() ), tr ( "Export database (CSV/HTML/XML)" ), this );
	//export_action->setShortcuts(QKeySequence::Save);
	export_action->setStatusTip ( tr ( "Export database (CSV/HTML/XML)" ) );
	connect ( export_action, SIGNAL ( triggered() ), guis, SLOT ( exportEvent() ) );
	inoutMenu->addAction ( export_action );

	QAction *help_action = new QAction ( QIcon ( *get_t_help_icon() ), tr ( "Help" ), this );
	help_action->setShortcut ( QKeySequence ( Qt::Key_F1 ) );
	help_action->setStatusTip ( tr ( "Help" ) );
	connect ( help_action, SIGNAL ( triggered() ), guis, SLOT ( helpEvent() ) );
	helpMenu->addAction ( help_action );

	QAction *about_action = new QAction ( QIcon ( *get_p_icon() ), tr ( "About Cdcat" ), this );
	//about_action->setShortcuts(QKeySequence::Save);
	about_action->setStatusTip ( tr ( "About Cdcat" ) );
	connect ( about_action, SIGNAL ( triggered() ), guis, SLOT ( aboutEvent() ) );
	helpMenu->addAction ( about_action );

	QAction *aboutqt_action = new QAction ( QIcon ( *get_t_qtlogo_icon() ), tr ( "About Qt" ), this );
	//aboutqt_action->setShortcuts(QKeySequence::Save);
	aboutqt_action->setStatusTip ( tr ( "About the Qt toolkit" ) );
	connect ( aboutqt_action, SIGNAL ( triggered() ), guis, SLOT ( aboutQtEvent() ) );
	helpMenu->addAction ( aboutqt_action );

	mainMenu->insertItem ( tr ( "Catalog" ), fileMenu );
	mainMenu->insertItem ( tr ( "Edit" ), editMenu );
	mainMenu->insertItem ( tr ( "Search" ), findMenu );
	mainMenu->insertItem ( tr ( "Import/Export" ), inoutMenu );
	mainMenu->insertItem ( tr ( "Others" ), othersMenu );
	mainMenu->insertItem ( tr ( "Help" ), helpMenu );
#ifdef Q_WS_MAC
	mainMenu->hide();
#endif

	// trayIcon
	trayIcon = NULL;
	if ( cconfig->showTrayIcon ) {
		createTrayIcon();
		trayIcon->show();
		trayIcon->setToolTip ( tr ( "Cdcat - idle" ) );
	}

	languageChange();

	lvMain   = new QVBoxLayout ( main );
	lvMain->insertSpacing ( 2, 4 );
	lvMain->insertWidget ( 3, splitMain, 8 );
	lvMain->insertSpacing ( 4, 4 );
	
	Toolbar->addAction ( new_action );
	Toolbar->addAction ( open_action );
	Toolbar->addAction ( save_action );
	Toolbar->addAction ( saveas_action );
	Toolbar->addAction ( close_action );
	Toolbar->addAction ( add_action );
	Toolbar->addAction ( rescan_action );
	Toolbar->addAction ( delete_action );
	Toolbar->addAction ( find_action );
	Toolbar->addAction ( config_action );
	Toolbar->addAction ( help_action );
	Toolbar->addAction ( about_action );
	Toolbar->addAction ( aboutqt_action );

	resize ( ( cconfig->windowSize ).expandedTo ( minimumSizeHint() ) );
	move ( cconfig->windowPos );

	QList<int> splitterSizesList;
	splitterSizesList.append ( cconfig->mainP1 );
	splitterSizesList.append ( cconfig->mainP2 ); // remaining space for listView
	//splitterSizesList.append ( cconfig->mainP3 );
	
	if(!cconfig->commentDockIsFloating)
		setDockSize ( CommentDock, cconfig->mainP3, 0 );
	
	CommentDock->setFloating(cconfig->commentDockIsFloating);
	if(cconfig->commentDockIsFloating) {
		CommentDock->show();
		scrollArea->show();
		commentWidget->show();
		CommentDock->resize(cconfig->commentDockSize_width, cconfig->commentDockSize_height);
		CommentDock->move(cconfig->commentDockPos_x, cconfig->commentDockPos_y);
	}
	
	splitMain->setSizes ( splitterSizesList );
	splitMain->setResizeMode ( DirView, QSplitter::Stretch );
	splitMain->setResizeMode ( listView, QSplitter::Stretch );
	splitMain->setStyle ( new QPlastiqueStyle() );

	if(!cconfig->commentDockIsFloating)
		CommentDock->resize(QSize(cconfig->mainP3, CommentDock->height()));
	
	connect ( DirView , SIGNAL ( folderSelected ( const QString & ) ), guis, SLOT ( listUpdate ( const QString & ) ) );
	connect ( DirView , SIGNAL ( hitkey ( QKeyEvent * ) ), guis, SLOT ( hotKeys ( QKeyEvent * ) ) );
	connect ( DirView, SIGNAL ( rightButtonPressed ( Q3ListViewItem *, const QPoint &, int ) ),
	          guis, SLOT ( showTreeContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );
	connect ( listView, SIGNAL ( rightButtonClicked ( Q3ListViewItem *, const QPoint &, int ) ),
	          guis, SLOT ( showListviewContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );

	connect ( listView, SIGNAL ( currentChanged ( Q3ListViewItem * ) ), guis, SLOT ( standOn ( Q3ListViewItem * ) ) );
	connect ( listView, SIGNAL ( clicked ( Q3ListViewItem * ) ), guis, SLOT ( standOn ( Q3ListViewItem * ) ) );
	connect ( listView, SIGNAL ( doubleClicked ( Q3ListViewItem * ) ), guis, SLOT ( doubleClickOn ( Q3ListViewItem * ) ) );

// 	connect ( ButtonFind  , SIGNAL ( clicked() ), guis, SLOT ( findEvent () ) );
// 	connect ( ButtonConfig, SIGNAL ( clicked() ), guis, SLOT ( configEvent() ) );

	connect ( commentWidget , SIGNAL ( touchdb() ), guis, SLOT ( cHcaption() ) );

	show();

	listView ->setFocus();

	//scrollArea->resize(QSize(splitMain->handle(2)->width(), scrollArea->height()));

	//commentWidget->resize ( QSize ( scrollArea->viewport()->width(), height() - 120 ) );
	commentWidget->updateContents();
	//CommentDock->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));


	//std::cout << "splitmain sizes: " << std::endl;
	//for (int i=0; i < splitMain->sizes().size(); i++)
	//	std::cout << splitMain->sizes().at(i) << std::endl;

	if ( cconfig->autoload )
		guis->checkversion ( this, db );
}


void CdCatMainWidget::setDockSize ( QDockWidget *dock, int setWidth, int setHeight ) {
	// http://qt-project.org/faq/answer/how_can_i_resize_a_qdockwidget_programatically
	oldMaxSize = dock->maximumSize();
	oldMinSize = dock->minimumSize();

	if ( setWidth >= 0 ) {
		if ( dock->width() < setWidth ) {
			dock->setMinimumWidth ( setWidth );
		}
		else {
			dock->setMaximumWidth ( setWidth );
		}
	}
	if ( setHeight >= 0 ) {
		if ( dock->height() < setHeight ) {
			dock->setMinimumHeight ( setHeight );
		}
		else {
			dock->setMaximumHeight ( setHeight );
		}
	}
	
	QTimer::singleShot ( 1, this, SLOT ( returnToOldMaxMinSizes() ) );
}

void CdCatMainWidget::returnToOldMaxMinSizes() {
	CommentDock->setMinimumSize ( oldMinSize );
	CommentDock->setMaximumSize ( oldMaxSize );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CdCatMainWidget::~CdCatMainWidget() {
	// no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CdCatMainWidget::languageChange() {
	setCaption ( tr ( "Hyper's CD Catalogizer" ) );
}

void CdCatMainWidget::closeEvent ( QCloseEvent *e ) {
	//if (trayIcon != NULL && trayIcon->isVisible()) {
	//	QMessageBox::information(this, tr("Systray"),
	//				tr("The program will keep running in the "
	//				"system tray. To terminate the program, "
	//				"choose <b>Quit</b> in the context menu "
	//				"of the system tray entry."));
	//	hide();
	//	e->ignore();
	//}
	//else
	{
		// An error occured during the REQUISTED saving of the database
		// (eg: access denied for writing...)
		if ( guis->closeEvent() != 0 )
			return;

		cconfig->windowSize = size();
		cconfig->windowPos  =  pos();
		cconfig->mainP1 = ( splitMain->sizes() ) [0];
		cconfig->mainP2 = ( splitMain->sizes() ) [1];
		//cconfig->mainP3 = ( splitMain->sizes() ) [2];
		
		
		cconfig->mainP3 = CommentDock->width();
		if(CommentDock->isFloating()) {
			cconfig->commentDockSize_width = CommentDock->width();
			cconfig->commentDockSize_height = CommentDock->height();
			cconfig->commentDockPos_x = CommentDock->x();
			cconfig->commentDockPos_y = CommentDock->y();
		}
		cconfig->commentDockSize_dockarea = dockWidgetArea(CommentDock);
		cconfig->commentDockIsFloating = CommentDock->isFloating();
		
		cconfig->writeConfig();
		if ( cconfig->showTrayIcon && trayIcon != NULL && trayIcon->isVisible() )
			trayIcon->hide();
		QWidget::closeEvent ( e );
	}
}

void CdCatMainWidget::pathScanned ( QString path ) {
// 	std::cerr <<"CdCatMainWidget::pathScanned: " << qPrintable(path) << std::endl;
	lastScanPath = path;
	statusBar()->showMessage ( tr ( "Scanning:" ) + " " + path );
	//if(cconfig->showTrayIcon && isHidden())
	//	trayIcon->setToolTip(tr ( "Scanning:" ) + " " + path);
}

void CdCatMainWidget::extraInfoAppend ( QString extraInfo ) {
	statusBar ()->showMessage( lastScanPath + " [" + extraInfo + "]" );
	//if(cconfig->showTrayIcon && isHidden())
	//	trayIcon->setToolTip(lastScanPath + " [" + extraInfo + "]");
}

void CdCatMainWidget::createTrayIcon() {
	if ( trayIcon == NULL ) {
		minimizeAction = new QAction ( tr ( "Mi&nimize" ), this );
		connect ( minimizeAction, SIGNAL ( triggered() ), this, SLOT ( hide() ) );

		maximizeAction = new QAction ( tr ( "Ma&ximize" ), this );
		connect ( maximizeAction, SIGNAL ( triggered() ), this, SLOT ( showMaximized() ) );

		restoreAction = new QAction ( tr ( "&Restore" ), this );
		connect ( restoreAction, SIGNAL ( triggered() ), this, SLOT ( showNormal() ) );

		quitAction = new QAction ( tr ( "&Quit" ), this );
		connect ( quitAction, SIGNAL ( triggered() ), app, SLOT ( quit() ) );

		trayIconMenu = new QMenu ( this );
		trayIconMenu->addAction ( minimizeAction );
		trayIconMenu->addAction ( maximizeAction );
		trayIconMenu->addAction ( restoreAction );
		trayIconMenu->addSeparator();
		trayIconMenu->addAction ( quitAction );

		trayIcon = new QSystemTrayIcon ( this );
		trayIcon->setContextMenu ( trayIconMenu );
		trayIcon->setIcon ( QIcon ( *get_p_icon() ) );
		anim_list = get_anim_list();
		trayIconAnimValue = 0;

		connect ( trayIcon, SIGNAL ( activated ( QSystemTrayIcon::ActivationReason ) ),
		          this, SLOT ( trayIconActivated ( QSystemTrayIcon::ActivationReason ) ) );
	}
}

void CdCatMainWidget::showTrayMessage() {

}

void CdCatMainWidget::trayIconActivated ( QSystemTrayIcon::ActivationReason reason ) {
	switch ( reason ) {
		case QSystemTrayIcon::Trigger:
			//std::cout << "systray single click " << std::endl;
			if ( isHidden() ) {
				showNormal();
				emit restoredFromTray();
			} else {
				hide();
				emit minimizedToTray();
			}
			break;
		case QSystemTrayIcon::DoubleClick:
			//std::cout << "systray double click " << std::endl;
			break;
		case QSystemTrayIcon::MiddleClick:
			//std::cout << "systray middle click " << std::endl;
			break;
		default:
			;
	}
	trayIcon->setIcon ( QIcon ( *get_p_icon() ) );
}

void CdCatMainWidget::trayIconMessageClicked() {
	//QMessageBox::information(0, tr("Systray"), tr("Sorry, I already gave what help I could.\nMaybe you should try asking a human?"));
}

void CdCatMainWidget::trayIconAnimateTimerSlot() {
	if ( cconfig->showTrayIcon && trayIcon != NULL ) {
		QPixmap pm = anim_list.at ( trayIconAnimValue );
		trayIcon->setIcon ( QIcon ( pm ) );
		if ( ++trayIconAnimValue == 5 )
			trayIconAnimValue = 0;
	}
}

void CdCatMainWidget::startTrayIconAnim() {
	connect ( &trayIconTimer, SIGNAL ( timeout() ), this, SLOT ( trayIconAnimateTimerSlot() ) );
	trayIconTimer.start ( 200 );
}

void CdCatMainWidget::stopTrayIconAnim() {
	trayIconTimer.stop();
	disconnect ( &trayIconTimer, SIGNAL ( timeout() ), this, SLOT ( trayIconAnimateTimerSlot() ) );
	trayIcon->setIcon ( QIcon ( *get_p_icon() ) );
}



// kate: indent-mode cstyle; indent-width 8; replace-tabs off; tab-width 8; 
