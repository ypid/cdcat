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
#include <QTreeWidget>
#include <QHeaderView>
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
#include "cdcat.h"
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
	: QMainWindow ( parent, fl ) {

	main = new QWidget ( this );
	setCentralWidget ( main );
	app = appp;
	QFontMetrics fm = app->font();

	if ( !name )
		setObjectName ( "CdCatMainWidget" );
	setWindowIcon ( *get_p_icon_big() );

	db = NULL;
	cconfig = ccp;
	pww = new PWw(this, appp);
	pww->hide();
	cconfig->startProgram ( &db, this );

	splitMain = new QSplitter ( this );

	Toolbar = addToolBar ( tr ( "Main toolbar" ) );
	//Toolbar->setFrameShape ( QFrame::StyledPanel );
	//Toolbar->setFrameShadow ( QFrame::Raised );
	//Toolbar->setMaximumHeight ( 36 );
	//Toolbar->setMinimumHeight ( 36 );

	DirView = new HDirectoryView ( &db, splitMain, "DirView" );
	QStringList labels;
	labels.append(tr ( "Directory Tree" ) );
	DirView->setHeaderLabels(labels);
	DirView->setSortingEnabled(true);

	listView = new HQListView ( this, splitMain, "listView" );
	QStringList labels2;
	labels2.append(tr ( "Name" ) );
	labels2.append(tr ( "Size" ) );
	listView->setHeaderLabels(labels2);
	listView->clear();
	listView->setSorting(0, false);


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
	guis = new GuiSlave ( this );

	/*Menubar*/
	mainMenu = menuBar();
	mainMenu->show();
	statusBar()->show();

	fileMenu = new QMenu ( this );
	fileMenu->setTitle(tr("File"));
	editMenu = new QMenu ( this );
	editMenu->setTitle(tr("Edit"));
	viewMenu = new QMenu ( this );
	viewMenu->setTitle(tr("View"));
	findMenu = new QMenu ( this );
	findMenu->setTitle(tr("Search"));
	inoutMenu = new QMenu ( this );
	inoutMenu->setTitle(tr("Import/Export"));
	othersMenu = new QMenu ( this );
	othersMenu->setTitle(tr("Others"));
	helpMenu = new QMenu ( this );
	helpMenu->setTitle(tr("Help"));

	new_action = new QAction ( QIcon ( *get_t_new_icon() ), tr ( "&New..." ), this );
	new_action->setShortcuts ( QKeySequence::New );
	new_action->setStatusTip ( tr ( "Create a new catalog" ) );
	connect ( new_action, SIGNAL ( triggered() ), guis, SLOT ( newEvent() ) );
	fileMenu->addAction ( new_action );

	open_action = new QAction ( QIcon ( *get_t_open_icon() ), tr ( "&Open..." ), this );
	open_action->setShortcuts ( QKeySequence::Open );
	open_action->setStatusTip ( tr ( "Open a existing catalog" ) );
	connect ( open_action, SIGNAL ( triggered() ), guis, SLOT ( openEvent() ) );
	fileMenu->addAction ( open_action );

	save_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "&Save" ), this );
	save_action->setShortcuts ( QKeySequence::Save );
	save_action->setStatusTip ( tr ( "Save catalog" ) );
	connect ( save_action, SIGNAL ( triggered() ), guis, SLOT ( saveEvent() ) );
	fileMenu->addAction ( save_action );

	saveas_action = new QAction ( QIcon ( *get_t_saveas_icon() ), tr ( "&Save as..." ), this );
	saveas_action->setShortcuts ( QKeySequence::Open );
	saveas_action->setStatusTip ( tr ( "save catalog with new name" ) );
	connect ( saveas_action, SIGNAL ( triggered() ), guis, SLOT ( saveasEvent() ) );
	fileMenu->addAction ( saveas_action );

#ifdef CATALOG_ENCRYPTION
	changepass_action = new QAction ( QIcon ( *get_t_change_encryption_password_icon() ), tr ( "Change password..." ), this );
	//changepass_action->setShortcuts ( QKeySequence::Open );
	changepass_action->setStatusTip ( tr ( "Changes password for catalog encryption" ) );
	connect ( changepass_action, SIGNAL ( triggered() ), guis, SLOT ( changePassEvent() ) );
	fileMenu->addAction ( changepass_action );
	changepass_action->setEnabled(false);

	disableencryption_action = new QAction ( QIcon ( *get_t_remove_encryption_icon() ), tr ( "Disable encryption" ), this );
	//disableencryption_action->setShortcuts ( QKeySequence::Open );
	disableencryption_action->setStatusTip (  tr ( "Disables catalog encryption" ) );
	connect ( disableencryption_action, SIGNAL ( triggered() ), guis, SLOT ( disableEncryptionEvent() ) );
	fileMenu->addAction ( disableencryption_action );
	disableencryption_action->setEnabled(false);

	enableencryption_action = new QAction ( QIcon ( *get_t_add_encryption_icon() ), tr ( "Enable encryption" ), this );
// 	enableencryption_action->setShortcuts ( QKeySequence::Open );
	enableencryption_action->setStatusTip (tr ( "Enables catalog encryption" ) );
	connect ( enableencryption_action, SIGNAL ( triggered() ), guis, SLOT ( enableEncryptionEvent() ) );
	fileMenu->addAction ( enableencryption_action );
	enableencryption_action->setEnabled(false);
#endif

	fileMenu->insertSeparator (NULL);
	historyMenu = new QMenu(this);

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
			historyMenu->addAction ( history_action );
		}
	}
	connect ( historyMenu, SIGNAL ( triggered ( QAction * ) ), guis, SLOT ( openHistoryElementEvent ( QAction * ) ) );

	close_action = new QAction ( QIcon ( *get_t_close_icon() ), tr ( "Close catalog" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_closeEvent, QKeySequence ( Qt::ControlModifier + Qt::Key_W ), tr("Close catalog")));
	close_action->setShortcut ( guis->getKeyBinding(guis->key_closeEvent) );
	close_action->setStatusTip ( tr ( "Close catalog" ) );
	connect ( close_action, SIGNAL ( triggered() ), guis, SLOT ( closeEvent() ) );
	fileMenu->addAction ( close_action );

	fileMenu->insertSeparator(NULL);

	quit_action = new QAction ( QIcon ( *get_t_close_icon() ), tr ( "&Quit" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_quitEvent, QKeySequence(Qt::ControlModifier + Qt::Key_Q), tr("Close program")));
	quit_action->setShortcut ( guis->getKeyBinding(guis->key_quitEvent) );
	quit_action->setStatusTip ( tr ( "Close program" ) );
	connect ( quit_action, SIGNAL ( triggered() ), this, SLOT ( close() ) );
	fileMenu->addAction ( quit_action );
	Toolbar->addAction ( quit_action );

	add_action = new QAction ( QIcon ( *get_t_add_icon() ), tr ( "Add media..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_addEvent, QKeySequence ( Qt::Key_A ), tr("Add new media to catalog")));
	add_action->setShortcut ( guis->getKeyBinding(guis->key_addEvent) );
	add_action->setStatusTip ( tr ( "Add new media to catalog" ) );
	connect ( add_action, SIGNAL ( triggered() ), guis, SLOT ( addEvent() ) );
	editMenu->addAction ( add_action );
	Toolbar->addAction ( add_action );

	addlink_action = new QAction ( QIcon ( *get_p_icon() ), tr ( "Add a link to a CdCat Catalog..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_addlnkEvent, QKeySequence ( Qt::ControlModifier + Qt::Key_L ), tr("Add a link to a existing cdcat catalog")));
	addlink_action->setShortcut ( guis->getKeyBinding(guis->key_addlnkEvent) );
	addlink_action->setStatusTip ( tr ( "Add a link to a existing cdcat catalog" ) );
	connect ( addlink_action, SIGNAL ( triggered() ), guis, SLOT ( addlnkEvent() ) );
	editMenu->addAction ( addlink_action );

	rescan_action = new QAction ( QIcon ( *get_t_rescan_icon() ), tr ( "Rescan media..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_rescanEvent, QKeySequence ( Qt::ControlModifier + Qt::Key_R ), tr("Rescan existing media")));
	rescan_action->setShortcut ( guis->getKeyBinding(guis->key_rescanEvent) );
	rescan_action->setStatusTip ( tr ( "Rescan existing media" ) );
	connect ( rescan_action, SIGNAL ( triggered() ), guis, SLOT ( rescanEvent() ) );
	editMenu->addAction ( rescan_action );

	editMenu->insertSeparator(NULL);

	insert_action = new QAction ( QIcon ( *get_m_import_icon() ), tr ( "Insert Catalog..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_insertcEvent, QKeySequence ( Qt::Key_I), tr("Insert catalog into database")));
	insert_action->setShortcut ( guis->getKeyBinding(guis->key_insertcEvent) );
	insert_action->setStatusTip ( tr ( "Insert catalog into database" ) );
	connect ( insert_action, SIGNAL ( triggered() ), guis, SLOT ( insertcEvent() ) );
	editMenu->addAction ( insert_action );

	insert_cdcat_xml_action = new QAction ( QIcon ( *get_m_import_icon() ), tr ( "Insert Cdcat export XML..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_insertCdcatXmlEvent, QKeySequence ( Qt::Key_X), tr("Insert Cdcat exported XML into database")));
	insert_cdcat_xml_action->setShortcut ( guis->getKeyBinding(guis->key_insertCdcatXmlEvent) );
	insert_cdcat_xml_action->setStatusTip ( tr ( "Insert Cdcat exported XML into database" ) );
	connect ( insert_cdcat_xml_action, SIGNAL ( triggered() ), guis, SLOT ( insertCdcatXmlEvent()) );
	editMenu->addAction ( insert_cdcat_xml_action );


	editMenu->insertSeparator(NULL);

	rename_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Rename node..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_insertCdcatXmlEvent, QKeySequence ( Qt::ControlModifier + Qt::Key_E ), tr("Rename node")));
	rename_action->setShortcut ( guis->getKeyBinding(guis->key_insertCdcatXmlEvent) );
	rename_action->setStatusTip ( tr ( "Rename node" ) );
	connect ( rename_action, SIGNAL ( triggered() ), guis, SLOT ( renameEvent() ) );
	editMenu->addAction ( rename_action );

	renumber_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Re-Number media node..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_renumberEvent, QKeySequence ( Qt::ControlModifier + Qt::AltModifier + Qt::Key_R ), tr("Renumber node")));
	renumber_action->setShortcut ( guis->getKeyBinding(guis->key_renumberEvent) );
	renumber_action->setStatusTip ( tr ( "Renumber node" ) );
	connect ( renumber_action, SIGNAL ( triggered() ), guis, SLOT ( renumberEvent() ) );
	editMenu->addAction ( renumber_action );

	//changemediatype_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Change media type..." ), this);
	//guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_typeChangeEvent, QKeySequence ( Qt::CTRL + Qt::Key_R ), tr("Change media type")));
	//changemediatype_action->setShortcut ( guis->getKeyBinding(guis->key_typeChangeEvent) );
	//changemediatype_action->setStatusTip(tr("Change media type"));
	//connect(changemediatype_action, SIGNAL(triggered()), guis, SLOT(typeChangeEvent()));
	//editMenu->addAction(changemediatype_action);

	delete_action = new QAction ( QIcon ( *get_t_delete_icon() ), tr ( "Delete node" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_deleteEvent, QKeySequence ( Qt::Key_Delete ), tr("Delete node")));
	delete_action->setShortcut ( guis->getKeyBinding(guis->key_deleteEvent) );
	delete_action->setStatusTip ( tr ( "Delete node" ) );
	connect ( delete_action, SIGNAL ( triggered() ), guis, SLOT ( deleteEvent() ) );
	editMenu->addAction ( delete_action );

	editMenu->insertSeparator(NULL);
	sortnu_ascending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by number (ascending)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_sortNuEventAscending, QKeySequence ( Qt::ControlModifier + Qt::AltModifier + Qt::Key_Up), tr("Sort media by number (ascending)")));
	sortnu_ascending_action->setShortcut ( guis->getKeyBinding(guis->key_sortNuEventAscending) );
	sortnu_ascending_action->setStatusTip ( tr ( "Sort media by number (ascending)" ) );
	connect ( sortnu_ascending_action, SIGNAL ( triggered() ), guis, SLOT ( sortNuEventAscending()) );
	editMenu->addAction ( sortnu_ascending_action );

	sortnu_descending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by number (descending)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_sortNuEventDescending, QKeySequence ( Qt::ControlModifier + Qt::AltModifier + Qt::Key_Down ), tr("Sort media by number (descending)")));
	sortnu_descending_action->setShortcut ( guis->getKeyBinding(guis->key_sortNuEventDescending) );
	sortnu_descending_action->setStatusTip ( tr ( "Sort media by number (descending)" ) );
	connect ( sortnu_descending_action, SIGNAL ( triggered() ), guis, SLOT ( sortNuEventDescending()) );
	editMenu->addAction ( sortnu_descending_action );

	sortna_ascending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by name (ascending)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_sortNaEventAscending, QKeySequence ( Qt::ControlModifier + Qt::Key_Up), tr("Sort media by number (ascending)")));
	sortnu_ascending_action->setShortcut ( guis->getKeyBinding(guis->key_sortNaEventAscending) );
	sortnu_ascending_action->setStatusTip ( tr ( "Sort media by name" ) );
	connect ( sortna_ascending_action, SIGNAL ( triggered() ), guis, SLOT ( sortNaEventAscending()) );
	editMenu->addAction ( sortna_ascending_action );

	sortna_descending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by name (descending)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_sortNaEventDescending, QKeySequence ( Qt::ControlModifier + Qt::Key_Down), tr("Sort media by name (descending)")));
	sortna_descending_action->setShortcut ( guis->getKeyBinding(guis->key_sortNaEventDescending) );
	sortna_descending_action->setStatusTip ( tr ( "Sort media by name (descending)" ) );
	connect ( sortna_descending_action, SIGNAL ( triggered() ), guis, SLOT ( sortNaEventDescending()) );
	editMenu->addAction ( sortna_descending_action );

	sortti_ascending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by time (ascending)" ), this );
	//sortti_ascending_action->setShortcuts(QKeySequence::Save);
	sortti_ascending_action->setStatusTip ( tr ( "Sort media by time (ascending)" ) );
	connect ( sortti_ascending_action, SIGNAL ( triggered() ), guis, SLOT ( sortTiEventAscending()) );
	editMenu->addAction ( sortti_ascending_action );

	sortti_descending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by time (descending)" ), this );
	//sortti_descending_action->setShortcuts(QKeySequence::Save);
	sortti_descending_action->setStatusTip ( tr ( "Sort media by time (descending)" ) );
	connect ( sortti_descending_action, SIGNAL ( triggered() ), guis, SLOT ( sortTiEventDescending()) );
	editMenu->addAction ( sortti_descending_action );

	sortty_ascending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by type (ascending)" ), this );
	//sortty_ascending_action->setShortcuts(QKeySequence::Save);
	sortty_ascending_action->setStatusTip ( tr ( "Sort media by type (ascending)" ) );
	connect ( sortty_ascending_action, SIGNAL ( triggered() ), guis, SLOT ( sortTyEventAscending()) );
	editMenu->addAction ( sortty_ascending_action );

	sortty_descending_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Sort media by type (descending)" ), this );
	//sortty_descending_action->setShortcuts(QKeySequence::Save);
	sortty_descending_action->setStatusTip ( tr ( "Sort media by type (descending)" ) );
	connect ( sortty_descending_action, SIGNAL ( triggered() ), guis, SLOT ( sortTyEventDescending()) );
	editMenu->addAction ( sortty_descending_action );

	view_toolbar_action = new QAction ( tr ( "view tool bar" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_toolBarEvent, QKeySequence( Qt::ControlModifier + Qt::AltModifier + Qt::Key_T ), tr("View tool bar in main window")));
	view_toolbar_action->setShortcut ( guis->getKeyBinding(guis->key_toolBarEvent) );
	view_toolbar_action->setStatusTip ( tr ( "View tool bar in main window" ) );
	view_toolbar_action->setCheckable(true);
	view_toolbar_action->setChecked(cconfig->showToolBar);
	connect ( view_toolbar_action, SIGNAL ( triggered() ), this, SLOT ( toolBarToogled() ) );
	viewMenu->addAction ( view_toolbar_action );

	view_statusbar_action = new QAction ( tr ( "view status bar" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_statusBarEvent, QKeySequence( Qt::ControlModifier + Qt::AltModifier + Qt::Key_S ), tr("View status bar in main window")));
	view_statusbar_action->setShortcut ( guis->getKeyBinding(guis->key_statusBarEvent) );
	view_statusbar_action->setStatusTip ( tr ( "View status bar in main window" ) );
	view_statusbar_action->setCheckable(true);
	view_statusbar_action->setChecked(cconfig->showStatusBar);
	connect ( view_statusbar_action, SIGNAL ( triggered() ), this, SLOT ( statusBarToogled() ) );
	viewMenu->addAction ( view_statusbar_action );

	view_comment_widget_action = new QAction ( tr ( "view comment dock" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_commentDockEvent, QKeySequence( Qt::ControlModifier + Qt::Key_D ), tr("show comment dock")));
	view_comment_widget_action->setShortcut ( guis->getKeyBinding(guis->key_commentDockEvent) );
	view_comment_widget_action->setStatusTip ( tr ( "show comment dock" ) );
	view_comment_widget_action->setCheckable(true);
	view_comment_widget_action->setChecked(cconfig->showCommentDock);
	connect ( view_comment_widget_action, SIGNAL ( triggered() ), this, SLOT ( commentWidgetToogled()));
	viewMenu->addAction ( view_comment_widget_action );

	view_tray_action = new QAction ( tr ( "show systray icon" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_dockEvent, QKeySequence( Qt::ControlModifier + Qt::AltModifier + Qt::Key_D ), tr("show systray icon")));
	view_tray_action->setShortcut ( guis->getKeyBinding(guis->key_dockEvent) );
	view_tray_action->setStatusTip ( tr ( "show systray icon" ) );
	view_tray_action->setCheckable(true);
	view_tray_action->setChecked(cconfig->showTrayIcon);
	connect ( view_tray_action, SIGNAL ( triggered() ), this, SLOT ( dockToogled()));
	viewMenu->addAction ( view_tray_action );

	find_action = new QAction ( QIcon ( *get_t_find_icon() ), tr ( "Seek in database..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_findEvent, QKeySequence( Qt::ControlModifier + Qt::Key_F ), tr("Seek in database for files and folders")));
	find_action->setShortcut ( guis->getKeyBinding(guis->key_findEvent) );
	find_action->setStatusTip ( tr ( "Seek in database for files and folders" ) );
	connect ( find_action, SIGNAL ( triggered() ), guis, SLOT ( findEvent() ) );
	findMenu->addAction ( find_action );

	findpanel_action = new QAction ( QIcon ( *get_t_find_icon() ), tr ( "Seek in the panel" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_posEvent, QKeySequence( Qt::AltModifier + Qt::Key_S ), tr("Seek in the panel")));
	findpanel_action->setShortcut ( guis->getKeyBinding(guis->key_posEvent) );
	findpanel_action->setStatusTip ( tr ( "Seek in the panel" ) );
	connect ( findpanel_action, SIGNAL ( triggered() ), guis, SLOT ( posEvent() ) );
	findMenu->addAction ( findpanel_action );

	borrow_action = new QAction ( QIcon ( *get_m_borrow_icon() ), tr ( "Borrowing info..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_borrowingEvent, QKeySequence( Qt::ControlModifier + Qt::Key_B ), tr("Borrowing info")));
	borrow_action->setShortcut ( guis->getKeyBinding(guis->key_borrowingEvent) );
	borrow_action->setStatusTip ( tr ( "Borrowing info" ) );
	connect ( borrow_action, SIGNAL ( triggered() ), guis, SLOT ( borrowingEvent() ) );
	othersMenu->addAction ( borrow_action );

	size_action = new QAction ( QIcon ( *get_t_save_icon() ), tr ( "Node size" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_sizeEvent, QKeySequence( Qt::Key_Space ), tr("Calculate node size")));
	size_action->setShortcut ( guis->getKeyBinding(guis->key_sizeEvent) );
	size_action->setStatusTip ( tr ( "Calculate node size" ) );
	connect ( size_action, SIGNAL ( triggered() ), guis, SLOT ( sizeEvent() ) );
	othersMenu->addAction ( size_action );

	config_action = new QAction ( QIcon ( *get_t_config_icon() ), tr ( "Configuration..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_configEvent, QKeySequence(Qt::ControlModifier + Qt::Key_G), tr("Edit cdcat configuration")));
	config_action->setShortcut ( guis->getKeyBinding(guis->key_configEvent) );
	config_action->setStatusTip ( tr ( "Edit cdcat configuration" ) );
	connect ( config_action, SIGNAL ( triggered() ), guis, SLOT ( configEvent() ) );
	othersMenu->addAction ( config_action );

	color_action = new QAction ( QIcon ( *get_t_colorconfig_icon() ), tr ( "Set Colors..." ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_colorEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_C), tr("Set the colors for display")));
	color_action->setShortcut ( guis->getKeyBinding(guis->key_colorEvent) );
	color_action->setStatusTip ( tr ( "Set the colors for display" ) );
	connect ( color_action, SIGNAL ( triggered() ), guis, SLOT ( colorEvent() ) );
	othersMenu->addAction ( color_action );

	import_action = new QAction ( QIcon ( *get_m_import_icon() ), tr ( "Import database (CSV/XML)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_importEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_I), tr("Import database (CSV/XML) from various catalog programs")));
	import_action->setShortcut ( guis->getKeyBinding(guis->key_importEvent) );
	import_action->setStatusTip ( tr ( "Import database (CSV/XML) from various catalog programs" ) );
	connect ( import_action, SIGNAL ( triggered() ), guis, SLOT ( importEvent() ) );
	inoutMenu->addAction ( import_action );

	export_action = new QAction ( QIcon ( *get_m_export_icon() ), tr ( "Export database (CSV/HTML/XML)" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_exportEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_E), tr("Export database (CSV/HTML/XML)")));
	export_action->setShortcut ( guis->getKeyBinding(guis->key_exportEvent) );
	export_action->setStatusTip ( tr ( "Export database (CSV/HTML/XML)" ) );
	connect ( export_action, SIGNAL ( triggered() ), guis, SLOT ( exportEvent() ) );
	inoutMenu->addAction ( export_action );

	help_action = new QAction ( QIcon ( *get_t_help_icon() ), tr ( "Help" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_helpEvent, QKeySequence(Qt::Key_F1), tr("Help")));
	help_action->setShortcut ( guis->getKeyBinding(guis->key_helpEvent) );
	help_action->setStatusTip ( tr ( "Help" ) );
	connect ( help_action, SIGNAL ( triggered() ), guis, SLOT ( helpEvent() ) );
	helpMenu->addAction ( help_action );

	keybindings_action = new QAction ( QIcon ( *get_t_help_icon() ), tr ( "Key bindings" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_keybindingEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_K), tr("Key bindings")));
	keybindings_action->setShortcut ( guis->getKeyBinding(guis->key_keybindingEvent) );
	keybindings_action->setStatusTip ( tr ( "Key bindings" ) );
	connect ( keybindings_action, SIGNAL ( triggered() ), guis, SLOT ( keyBindingsEvent() ) );
	helpMenu->addAction ( keybindings_action );

	about_action = new QAction ( QIcon ( *get_p_icon() ), tr ( "About Cdcat" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_aboutEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_A), tr("About Cdcat")));
	about_action->setShortcut ( guis->getKeyBinding(guis->key_aboutEvent) );
	about_action->setStatusTip ( tr ( "About Cdcat" ) );
	connect ( about_action, SIGNAL ( triggered() ), guis, SLOT ( aboutEvent() ) );
	helpMenu->addAction ( about_action );

	aboutqt_action = new QAction ( QIcon ( *get_t_qtlogo_icon() ), tr ( "About Qt" ), this );
	guis->KeyShortCutList.append(CdcatKeyBinding(guis->key_aboutQtEvent, QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_Q), tr("About the Qt toolkit")));
	aboutqt_action->setShortcut ( guis->getKeyBinding(guis->key_aboutQtEvent) );
	aboutqt_action->setStatusTip ( tr ( "About the Qt toolkit" ) );
	connect ( aboutqt_action, SIGNAL ( triggered() ), guis, SLOT ( aboutQtEvent() ) );
	helpMenu->addAction ( aboutqt_action );

	mainMenu->addMenu ( fileMenu );
	mainMenu->addMenu ( editMenu );
	mainMenu->addMenu ( viewMenu );
	mainMenu->addMenu ( findMenu );
	mainMenu->addMenu ( inoutMenu );
	mainMenu->addMenu ( othersMenu );
	mainMenu->addMenu ( helpMenu );
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
#ifdef CATALOG_ENCRYPTION
	Toolbar->addAction ( changepass_action );
	Toolbar->addAction ( disableencryption_action );
	Toolbar->addAction ( enableencryption_action );
#endif
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
	splitMain->setStretchFactor(splitMain->indexOf(DirView), 1);
	splitMain->setStretchFactor(splitMain->indexOf(listView), 1);
	QPlastiqueStyle plastic_style;
	splitMain->setStyle ( &plastic_style );

	if(!cconfig->commentDockIsFloating)
		CommentDock->resize(QSize(cconfig->mainP3, CommentDock->height()));

	connect ( DirView , SIGNAL ( folderSelected ( const QString & ) ), guis, SLOT ( listUpdate ( const QString & ) ) );
	connect ( DirView , SIGNAL ( hitkey ( QKeyEvent * ) ), guis, SLOT ( hotKeys ( QKeyEvent * ) ) );
	connect ( DirView, SIGNAL ( customContextMenuRequested ( const QPoint  ) ),
	          guis, SLOT ( showTreeContextMenu  ( const QPoint ) ) );
	connect ( listView, SIGNAL ( customContextMenuRequested ( const QPoint  ) ),
	          guis, SLOT ( showListviewContextMenu( const QPoint ) ) );

	//connect ( listView, SIGNAL (    itemChanged  ( QTreeWidgetItem *, int ) ), guis, SLOT ( standOn ( QTreeWidgetItem *, int ) ) );
	//connect ( listView, SIGNAL (    currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), guis, SLOT ( currentItemChanged ( QTreeWidgetItem*,QTreeWidgetItem* ) ) );
	connect ( listView, SIGNAL ( itemClicked ( QTreeWidgetItem *, int ) ), guis, SLOT ( standOn ( QTreeWidgetItem *, int ) ) );
	connect ( listView, SIGNAL (   itemDoubleClicked ( QTreeWidgetItem *, int ) ), guis, SLOT ( doubleClickOn ( QTreeWidgetItem *, int ) ) );

// 	connect ( ButtonFind  , SIGNAL ( clicked() ), guis, SLOT ( findEvent () ) );
// 	connect ( ButtonConfig, SIGNAL ( clicked() ), guis, SLOT ( configEvent() ) );

	connect ( commentWidget , SIGNAL ( touchdb() ), guis, SLOT ( cHcaption() ) );

	if(cconfig->showToolBar)
		Toolbar->show();
	else
		Toolbar->hide();

	if(cconfig->showStatusBar)
		statusBar()->show();
	else
		statusBar()->hide();

	if(cconfig->showCommentDock)
		CommentDock->show();
	else
		CommentDock->hide();

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
	delete DirView;
	delete pww;
	delete guis;
	delete main;
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CdCatMainWidget::languageChange() {
	setWindowTitle ( tr ( "Hyper's CD Catalogizer" ) );
        //Toolbar->setTitle (tr ( "Main toolbar" ) );

	fileMenu->setTitle(tr("File"));
	editMenu->setTitle(tr("Edit"));
	viewMenu->setTitle(tr("View"));
	findMenu->setTitle(tr("Search"));
	inoutMenu->setTitle(tr("Import/Export"));
	othersMenu->setTitle(tr("Others"));
	helpMenu->setTitle(tr("Help"));

	QStringList labels;
	labels.append(tr ( "Directory Tree" ) );
	DirView->setHeaderLabels(labels);
	QStringList labels2;
	labels2.append(tr ( "Name" ) );
	labels2.append(tr ( "Size" ) );
	labels2.append(tr ( "Type" ) );
	listView->setHeaderLabels(labels2);
	listView->sortByColumn(0, Qt::AscendingOrder);

	new_action->setText(tr ( "&New..." ));
        new_action->setStatusTip ( tr ( "Create a new catalog" ));
        open_action->setText(tr ( "&Open..." ));
        open_action->setStatusTip ( tr ( "Open a existing catalog" ));
        save_action->setText(tr ( "&Save" ));
        save_action->setStatusTip ( tr ( "Save catalog" ));
        saveas_action->setText(tr ( "&Save as..." ));
        saveas_action->setStatusTip ( tr ( "save catalog with new name" ));
#ifdef CATALOG_ENCRYPTION
	changepass_action->setText(tr ( "Change password..." ));
	changepass_action->setStatusTip ( tr ( "Changes password for catalog encryption" ));
	disableencryption_action->setText(tr ( "Disable encryption" ));
	disableencryption_action->setStatusTip ( tr ( "Disables catalog encryption" ));
	enableencryption_action->setText(tr ( "Enable encryption" ));
	enableencryption_action->setStatusTip ( tr ( "Enables catalog encryption" ));
#endif
        //history_action->setText(tr ( "Recent files..." ), this));
        historyMenu->setTitle ( tr ( "Recent files..." ));
        close_action->setText(tr ( "Close catalog" ));
        close_action->setStatusTip ( tr ( "Close catalog" ));
        quit_action->setText(tr ( "&Quit" ));
        quit_action->setStatusTip ( tr ( "Close program" ));
        add_action->setText(tr ( "Add media..." ));
        add_action->setStatusTip ( tr ( "Add new media to catalog" ));
        addlink_action->setText(tr ( "Add a link to a CdCat Catalog..." ));
        addlink_action->setStatusTip ( tr ( "Add a link to a existing cdcat catalog" ));
        rescan_action->setText(tr ( "Rescan media..." ));
        rescan_action->setStatusTip ( tr ( "Rescan existing media" ));
        insert_action->setText(tr ( "Insert Catalog..." ));
        insert_action->setStatusTip ( tr ( "Insert catalog into database" ));
        insert_cdcat_xml_action->setText(tr ( "Insert Cdcat XML export..." ));
        insert_cdcat_xml_action->setStatusTip ( tr ( "Insert Cdcat XML export" ));
        rename_action->setText(tr ( "Rename node..." ));
        rename_action->setStatusTip ( tr ( "Rename node" ));
        renumber_action->setText(tr ( "Re-Number media node..." ));
        renumber_action->setStatusTip ( tr ( "Renumber node" ));
        //changemediatype_action->setText(tr ( "Change media type..." ), this));
        delete_action->setText(tr ( "Delete node" ));
        delete_action->setStatusTip ( tr ( "Delete node" ));
        sortnu_ascending_action->setText(tr ( "Sort media by number (ascending)" ));
        sortnu_ascending_action->setStatusTip ( tr ( "Sort media by number (ascending)" ));
        sortnu_descending_action->setText(tr ( "Sort media by number (descending)" ));
        sortnu_descending_action->setStatusTip ( tr ( "Sort media by number (descending)" ));
        sortna_ascending_action->setText(tr ( "Sort media by name (ascending)" ));
        sortna_ascending_action->setStatusTip ( tr ( "Sort media by name (ascending)" ));
	sortna_descending_action->setText(tr ( "Sort media by name (descending)" ));
        sortna_descending_action->setStatusTip ( tr ( "Sort media by name (descending)" ));
        sortti_ascending_action->setText(tr ( "Sort media by time (ascending)" ));
        sortti_ascending_action->setStatusTip ( tr ( "Sort media by time (ascending)" ));
        sortti_descending_action->setText(tr ( "Sort media by time (descending)" ));
        sortti_descending_action->setStatusTip ( tr ( "Sort media by time (descending)" ));
        sortty_ascending_action->setText(tr ( "Sort media by type (ascending)" ));
        sortty_ascending_action->setStatusTip ( tr ( "Sort media by type (ascending)" ));
        sortty_descending_action->setText(tr ( "Sort media by type (descending)" ));
        sortty_descending_action->setStatusTip ( tr ( "Sort media by type (descending)" ));
        view_toolbar_action->setText(tr ( "view tool bar" ));
        view_toolbar_action->setStatusTip ( tr ( "View tool bar in main window" ));
        view_statusbar_action->setText(tr ( "view status bar" ));
        view_statusbar_action->setStatusTip ( tr ( "View status bar in main window" ));
        view_comment_widget_action->setText(tr ( "view comment dock" ));
        view_comment_widget_action->setStatusTip ( tr ( "show comment dock" ));
        view_tray_action->setText(tr ( "show systray icon" ));
        view_tray_action->setStatusTip ( tr ( "show systray icon" ));
        find_action->setText(tr ( "Seek in database..." ));
        find_action->setStatusTip ( tr ( "Seek in database for files and folders" ));
        findpanel_action ->setText(tr ( "Seek in the panel" ));
        findpanel_action->setStatusTip ( tr ( "Seek in the panel" ));
        borrow_action->setText(tr ( "Borrowing info..." ));
        borrow_action->setStatusTip ( tr ( "Borrowing info..." ));
        size_action->setText(tr ( "Node size" ));
        size_action->setStatusTip ( tr ( "Calculate node size" ));
        config_action->setText(tr ( "Configuration..." ));
        config_action->setStatusTip ( tr ( "Edit cdcat configuration" ));
        color_action->setText(tr ( "Set Colors..." ));
        color_action->setStatusTip ( tr ( "Set the colors for display" ));
        import_action->setText(tr ( "Import database (CSV/XML)" ));
        import_action->setStatusTip ( tr ( "Import database (CSV/XML) from various catalog programs" ));
        export_action->setText(tr ( "Export database (CSV/HTML/XML)" ));
        export_action->setStatusTip ( tr ( "Export database (CSV/HTML/XML)" ));
        help_action->setText(tr ( "Help" ));
        help_action->setStatusTip ( tr ( "Help" ));
        about_action->setText(tr ( "About Cdcat" ));
        about_action->setStatusTip ( tr ( "About Cdcat" ));
        aboutqt_action->setText(tr ( "About Qt" ));
        aboutqt_action->setStatusTip ( tr ( "About the Qt toolkit" ));
	if (trayIcon != NULL) {
		trayIcon->setToolTip ( tr ( "Cdcat - idle" ));
		minimizeAction->setText(tr ( "Mi&nimize" ));
		maximizeAction->setText(tr ( "Ma&ximize" ));
		restoreAction->setText(tr ( "&Restore" ));
		quitAction->setText(tr ( "&Quit" ));
	}
	setWindowTitle ( tr ( "Hyper's CD Catalogizer" ) );
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
		// An error occurred during the REQUISTED saving of the database
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

		cconfig->showStatusBar = view_statusbar_action->isChecked();
		cconfig->showToolBar = view_toolbar_action->isChecked();
		cconfig->showCommentDock = view_comment_widget_action->isChecked();
		cconfig->showTrayIcon = view_tray_action->isChecked();

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

void CdCatMainWidget::statusBarToogled() {
	if(view_statusbar_action->isChecked())
		statusBar()->show();
	else
		statusBar()->hide();
}

void CdCatMainWidget::toolBarToogled() {
	if(view_toolbar_action->isChecked())
		Toolbar->show();
	else
		Toolbar->hide();
}

void CdCatMainWidget::commentWidgetToogled() {
	if(view_comment_widget_action->isChecked())
		CommentDock->show();
	else
		CommentDock->hide();
}

void CdCatMainWidget::dockToogled() {
	if(view_tray_action->isChecked()) {
		if( trayIcon == NULL)
			createTrayIcon();
		trayIcon->show();
		trayIcon->setToolTip(tr("Cdcat - idle"));
	}
	else {
		if( trayIcon != NULL) {
			trayIcon->hide();
		}
	}
}

void CdCatMainWidget::setTrayToolTipInfo ( QString tooltipinfo ) {
	if(trayIcon) {
		trayIcon->setToolTip(tooltipinfo);
	}
}

void CdCatMainWidget::setTrayText ( QString text, QString header ) {
	if(trayIcon) {
		trayIcon->showMessage(header , text, QSystemTrayIcon::Information, 2000);
	}
}




// kate: indent-mode cstyle; indent-width 8; replace-tabs off; tab-width 8;
