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
#include <Q3HBoxLayout>
#include <QCloseEvent>
#include <Q3ValueList>
#include <QKeyEvent>
#include <Q3VBoxLayout>
#include <QAction>
#include <QPixmap>
#include <QLatin1String>

#include <iostream>

#include <QPlastiqueStyle>

#include "commwidget.h"
#include "hdirview.h"
#include "dbase.h"
#include "guibase.h"
#include "config.h"
#include "icons.h"

#include <QScrollArea>

/*
 *  Constructs a CdCatMainWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CdCatMainWidget::CdCatMainWidget ( CdCatConfig *ccp, QApplication *appp, QWidget* parent, const char* name, Qt::WFlags fl )
	: QWidget ( parent, name, fl ) {
	
	app = appp;
	
	int fontHeight = ( new QFontMetrics ( app->font() ) )->height();
	
	if ( !name )
		setName ( "CdCatMainWidget" );
	setIcon ( *get_p_icon() );
	
	db = NULL;
	cconfig = ccp;
	cconfig->startProgram ( &db, this );
	
	splitMain = new QSplitter ( this );
	
	Toolbar = new QFrame ( this, "Toolbar" );
	Toolbar->setFrameShape ( QFrame::StyledPanel );
	Toolbar->setFrameShadow ( QFrame::Raised );
	Toolbar->setMaximumHeight ( 36 );
	Toolbar->setMinimumHeight ( 36 );
	
	ButtonNew = new QToolButton ( Toolbar, "ButtonNew" );
	ButtonNew->setPixmap ( *get_t_new_icon() );
	ButtonNew->setMaximumSize ( 30, 30 );
	
	ButtonOpen = new QToolButton ( Toolbar, "ButtonOpen" );
	ButtonOpen->setMaximumSize ( 30, 30 );
	ButtonOpen->setPixmap ( *get_t_open_icon() );
	
	ButtonSave = new QToolButton ( Toolbar, "ButtonSave" );
	ButtonSave->setMaximumSize ( 30, 30 );
	ButtonSave->setPixmap ( *get_t_save_icon() );
	
	ButtonSaveAs = new QToolButton ( Toolbar, "ButtonSaveAs" );
	ButtonSaveAs->setMaximumSize ( 30, 30 );
	ButtonSaveAs->setPixmap ( * get_t_saveas_icon() );
	
	ButtonClose = new QToolButton ( Toolbar, "ButtonClose" );
	ButtonClose->setMaximumSize ( 30, 30 );
	ButtonClose->setPixmap ( *get_t_close_icon() );
	
	ButtonAdd = new QToolButton ( Toolbar, "ButtonAdd" );
	ButtonAdd->setMaximumSize ( 30, 30 );
	ButtonAdd->setPixmap ( *get_t_add_icon() );
	
	ButtonRescan = new QToolButton ( Toolbar, "ButtonRescan" );
	ButtonRescan->setMaximumSize ( 30, 30 );
	ButtonRescan->setPixmap ( *get_t_rescan_icon() );
	
	ButtonDelete = new QToolButton ( Toolbar, "ButtonDelete" );
	ButtonDelete->setMaximumSize ( 30, 30 );
	ButtonDelete->setPixmap ( *get_t_delete_icon() );
	
	ButtonConfig = new QToolButton ( Toolbar, "ButtonConfig" );
	ButtonConfig->setMaximumSize ( 30, 30 );
	ButtonConfig->setPixmap ( *get_t_config_icon() );
	
	ButtonHelp = new QToolButton ( Toolbar, "ButtonHelp" );
	ButtonHelp->setMaximumSize ( 30, 30 );
	ButtonHelp->setPixmap ( *get_t_help_icon() );
	
	ButtonFind = new QToolButton ( Toolbar, "ButtonFind" );
	ButtonFind->setMaximumSize ( 30, 30 );
	ButtonFind->setPixmap ( *get_t_find_icon() );
	
	ButtonAbout = new QToolButton ( Toolbar, "ButtonAbout" );
	ButtonAbout->setMaximumSize ( 30, 30 );
	ButtonAbout->setPixmap ( *get_t_about_icon() );
	
	ButtonAboutQt = new QToolButton ( Toolbar, "ButtonAboutQt" );
	ButtonAboutQt->setMaximumSize ( 30, 30 );
	ButtonAboutQt->setPixmap ( * ( get_t_qtlogo_icon() ) );
	
	statusl = new QLabel ( tr ( "No item selected" ), this, "statusl" );
	statusl->setMaximumHeight ( fontHeight + 2 );
	
	DirView = new HDirectoryView ( &db, splitMain, "DirView" );
	DirView->header()->setLabel ( 0, tr ( "Directory Tree" ) );
	
	listView = new HQListView ( this, splitMain, "listView" );
	listView->header()->setLabel ( 0, tr ( "Name" ) );
	listView->header()->setLabel ( 1, tr ( "Size" ) );
	listView->clear();
	
	commentWidget = new CommentWidget ( cconfig, app , 0, "CommentWiget" );
	
	QScrollArea *scrollArea = new QScrollArea ( splitMain );
	scrollArea->setBackgroundRole ( QPalette::Dark );
	scrollArea->setWidget ( commentWidget );
	//commentWidget->resize(scrollArea->size() );
	
	commentWidget->setScrollArea ( scrollArea );
	
	guis = new GuiSlave ( this );
	
	/*Menubar*/
	mainMenu = new QMenuBar ( this, "menu" );
	mainMenu->setMaximumHeight ( fontHeight * 2 );
	mainMenu->setMinimumHeight ( fontHeight * 2 );
	QMenu *fileMenu = new QMenu();
	QMenu *editMenu = new QMenu();
	QMenu *findMenu = new QMenu();
	QMenu *inoutMenu = new QMenu();
	QMenu *othersMenu = new QMenu();
	QMenu *helpMenu = new QMenu();
	
	QAction *new_action = new QAction (QIcon(*get_t_new_icon()), tr ( "&New..." ), this);
	new_action->setShortcuts(QKeySequence::New);
	new_action->setStatusTip(tr("Create a new catalog"));
	connect(new_action, SIGNAL(triggered()), guis, SLOT(newEvent()));
	fileMenu->addAction(new_action);
	
	QAction *open_action = new QAction (QIcon(*get_t_open_icon()), tr ( "&Open..." ), this);
	open_action->setShortcuts(QKeySequence::Open);
	open_action->setStatusTip(tr("Open a existing catalog"));
	connect(open_action, SIGNAL(triggered()), guis, SLOT(openEvent()));
	fileMenu->addAction(open_action);
	
	QAction *save_action = new QAction (QIcon(*get_t_save_icon()), tr ( "&Save" ), this);
	save_action->setShortcuts(QKeySequence::Save);
	save_action->setStatusTip(tr("Save catalog"));
	connect(save_action, SIGNAL(triggered()), guis, SLOT(saveEvent()));
	fileMenu->addAction(save_action);
	
	QAction *saveas_action = new QAction (QIcon(*get_t_saveas_icon()), tr ( "&Save as..." ), this);
	saveas_action->setShortcuts(QKeySequence::Open);
	saveas_action->setStatusTip(tr("save catalog with new name"));
	connect(saveas_action, SIGNAL(triggered()), guis, SLOT(saveasEvent()));
	fileMenu->addAction(saveas_action);
	
	fileMenu->insertSeparator ();
	historyMenu = new QMenu();
	
	//QAction *history_action = new QMenu (QIcon(*get_t_open_icon()), tr ( "Recent files..." ), this);
	//history_action->setShortcuts(QKeySequence::Open);
	//history_action->setStatusTip(tr("save catalog with new name"));
	
	fileMenu->addMenu(historyMenu);
	historyMenu->setTitle(tr("Recent files..."));
	historyMenu->setIcon(QIcon(*get_t_open_icon()));
	
	for ( int i = cconfig->hlist.size() - 1; i > -1; i-- ) {
		if ( !cconfig->hlist.at ( i ).isEmpty() ) {
			QAction *history_action = new QAction (QIcon(*get_p_icon()), cconfig->hlist.at ( i ), this);
			//history_action->setShortcut( QKeySequence (Qt::CTRL + Qt::Key_W));
			//history_action->setStatusTip(tr("Open catalog history element"));
			connect(history_action, SIGNAL ( activated ( int ) ), guis, SLOT ( openHistoryElementEvent ( int ) ));
			historyMenu->addAction ( history_action );
		}
	}
	
	QAction *close_action = new QAction (QIcon(*get_t_close_icon()), tr ( "Close" ), this);
	close_action->setShortcut( QKeySequence (Qt::CTRL + Qt::Key_W));
	close_action->setStatusTip(tr("Close catalog"));
	connect(close_action, SIGNAL(triggered()), guis, SLOT(closeEvent()));
	fileMenu->addAction(close_action);
	
	fileMenu->insertSeparator();
	
	QAction *quit_action = new QAction (QIcon(*get_t_save_icon()), tr ( "&Quit" ), this);
	quit_action->setShortcuts(QKeySequence::Close);
	quit_action->setStatusTip(tr("Close program"));
	connect(quit_action, SIGNAL(triggered()), this, SLOT(close()));
	fileMenu->addAction(quit_action);
	
	QAction *add_action = new QAction (QIcon(*get_t_add_icon()), tr ( "Add media..." ), this);
	add_action->setShortcut(QKeySequence (Qt::Key_A ));
	add_action->setStatusTip(tr("Add new media to catalog"));
	connect(add_action, SIGNAL(triggered()), guis, SLOT(addEvent()));
	editMenu->addAction(add_action);
	
	QAction *addlink_action = new QAction (QIcon(*get_p_icon()), tr ( "Add a link to a CdCat Catalog..." ), this);
	addlink_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_L));
	addlink_action->setStatusTip(tr("Add a link to a existing cdcat catalog"));
	connect(addlink_action, SIGNAL(triggered()), guis, SLOT(addlnkEvent()));
	editMenu->addAction(addlink_action);
	
	QAction *rescan_action = new QAction (QIcon(*get_t_rescan_icon()), tr ( "Rescan media..." ), this);
	rescan_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_R));
	rescan_action->setStatusTip(tr("Rescan existing media"));
	connect(rescan_action, SIGNAL(triggered()), guis, SLOT(rescanEvent()));
	editMenu->addAction(rescan_action);
	
	editMenu->insertSeparator();
	
	QAction *insert_action = new QAction (QIcon(*get_m_import_icon()), tr ( "Insert Catalog..." ), this);
	//insert_action->setShortcut(QKeySequence::Save);
	insert_action->setStatusTip(tr("Insert catalog into database"));
	connect(insert_action, SIGNAL(triggered()), guis, SLOT(insertcEvent()));
	editMenu->addAction(insert_action);
	
	editMenu->insertSeparator();
	
	QAction *rename_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Rename node..." ), this);
	rename_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_E));
	rename_action->setStatusTip(tr("Rename node"));
	connect(rename_action, SIGNAL(triggered()), guis, SLOT(renameEvent()));
	editMenu->addAction(rename_action);
	
	QAction *renumber_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Re-Number media node..." ), this);
	//renumber_action->setShortcuts(QKeySequence::Save);
	renumber_action->setStatusTip(tr("Renumber node"));
	connect(renumber_action, SIGNAL(triggered()), guis, SLOT(renumberEvent()));
	editMenu->addAction(renumber_action);
	
	//QAction *changemediatype_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Change media type..." ), this);
	////changemediatype_action->setShortcuts(QKeySequence::Save);
	//changemediatype_action->setStatusTip(tr("Change media type"));
	//connect(changemediatype_action, SIGNAL(triggered()), guis, SLOT(typeChangeEvent()));
	//editMenu->addAction(changemediatype_action);
	
	QAction *delete_action = new QAction (QIcon(*get_t_delete_icon()), tr ( "Delete node" ), this);
	//delete_action->setShortcuts(QKeySequence::Delete);
	delete_action->setStatusTip(tr("Delete node"));
	connect(delete_action, SIGNAL(triggered()), guis, SLOT(deleteEvent()));
	editMenu->addAction(delete_action);
	
	editMenu->insertSeparator();
	QAction *sortnu_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Sort media by number" ), this);
	//sortnu_action->setShortcuts(QKeySequence::Save);
	sortnu_action->setStatusTip(tr("Sort media by number"));
	connect(sortnu_action, SIGNAL(triggered()), guis, SLOT(sortNuEvent()));
	editMenu->addAction(sortnu_action);
	
	QAction *sortna_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Sort media by name" ), this);
	//sortna_action->setShortcuts(QKeySequence::Save);
	sortna_action->setStatusTip(tr("Sort media by name"));
	connect(sortna_action, SIGNAL(triggered()), guis, SLOT(sortNaEvent()));
	editMenu->addAction(sortna_action);
	
	QAction *sortti_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Sort media by time" ), this);
	//sortti_action->setShortcuts(QKeySequence::Save);
	sortti_action->setStatusTip(tr("Sort media by time"));
	connect(sortti_action, SIGNAL(triggered()), guis, SLOT(sortTiEvent()));
	editMenu->addAction(sortti_action);
	
	QAction *find_action = new QAction (QIcon(*get_t_find_icon()), tr ( "Seek in database..." ), this);
	find_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_F));
	find_action->setStatusTip(tr("Seek in database for files and folders"));
	connect(find_action, SIGNAL(triggered()), guis, SLOT(findEvent()));
	findMenu->addAction(find_action);
	
	QAction *findpanel_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Seek in the panel" ), this);
	findpanel_action->setShortcut(QKeySequence (Qt::ALT + Qt::Key_S));
	findpanel_action->setStatusTip(tr("Seek in the panel"));
	connect(findpanel_action, SIGNAL(triggered()), guis, SLOT(posEvent()));
	findMenu->addAction(findpanel_action);
	
	QAction *borrow_action = new QAction (QIcon(*get_m_borrow_icon()), tr ( "Borrowing info..." ), this);
	borrow_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_B));
	borrow_action->setStatusTip(tr("Borrowing info..."));
	connect(borrow_action, SIGNAL(triggered()), guis, SLOT(borrowingEvent()));
	othersMenu->addAction(borrow_action);
	
	QAction *size_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Node size" ), this);
	size_action->setShortcut(QKeySequence (Qt::Key_Space));
	size_action->setStatusTip(tr("Calculate node size"));
	connect(size_action, SIGNAL(triggered()), guis, SLOT(sizeEvent()));
	othersMenu->addAction(size_action);
	
	QAction *config_action = new QAction (QIcon(*get_t_config_icon()), tr ( "Configuration..." ), this);
	config_action->setShortcut(QKeySequence (Qt::CTRL + Qt::Key_G));
	config_action->setStatusTip(tr("Edit cdcat configuration"));
	connect(config_action, SIGNAL(triggered()), guis, SLOT(configEvent()));
	othersMenu->addAction(config_action);
	
	QAction *color_action = new QAction (QIcon(*get_t_colorconfig_icon()), tr ( "Set Colors..." ), this);
	//color_action->setShortcuts(QKeySequence::Save);
	color_action->setStatusTip(tr("Set the colors for display"));
	connect(color_action, SIGNAL(triggered()), guis, SLOT(colorEvent()));
	othersMenu->addAction(color_action);
	
	QAction *import_action = new QAction (QIcon(*get_m_import_icon()), tr ( "Import database (CSV/XML)" ), this);
	//import_action->setShortcuts(QKeySequence::Save);
	import_action->setStatusTip(tr("Import database (CSV/XML) from various catalog programs"));
	connect(import_action, SIGNAL(triggered()), guis, SLOT(importEvent()));
	inoutMenu->addAction(import_action);
	
	QAction *export_action = new QAction (QIcon(*get_m_export_icon()), tr ( "Export database (CSV/HTML/XML)" ), this);
	//export_action->setShortcuts(QKeySequence::Save);
	export_action->setStatusTip(tr("Export database (CSV/HTML/XML)"));
	connect(export_action, SIGNAL(triggered()), guis, SLOT(exportEvent()));
	inoutMenu->addAction(export_action);
	
	QAction *help_action = new QAction (QIcon(*get_t_save_icon()), tr ( "Help" ), this);
	help_action->setShortcut(QKeySequence (Qt::Key_F1));
	help_action->setStatusTip(tr("Help"));
	connect(help_action, SIGNAL(triggered()), guis, SLOT(helpEvent()));
	helpMenu->addAction(help_action);
	
	QAction *about_action = new QAction (QIcon(*get_p_icon()), tr ( "About Cdcat" ), this);
	//about_action->setShortcuts(QKeySequence::Save);
	about_action->setStatusTip(tr("About Cdcat"));
	connect(about_action, SIGNAL(triggered()), guis, SLOT(aboutEvent()));
	helpMenu->addAction(about_action);
	
	QAction *aboutqt_action = new QAction (QIcon(*get_t_qtlogo_icon()), tr ( "About Qt" ), this);
	//aboutqt_action->setShortcuts(QKeySequence::Save);
	aboutqt_action->setStatusTip(tr("About the Qt toolkit"));
	connect(aboutqt_action, SIGNAL(triggered()), guis, SLOT(aboutQtEvent()));
	helpMenu->addAction(aboutqt_action);
	
	mainMenu->insertItem ( tr ( "Catalog" ), fileMenu );
	mainMenu->insertItem ( tr ( "Edit" ), editMenu );
	mainMenu->insertItem ( tr ( "Search" ), findMenu );
	mainMenu->insertItem ( tr ( "Import/Export" ), inoutMenu );
	mainMenu->insertItem ( tr ( "Others" ), othersMenu );
	mainMenu->insertItem ( tr ( "Help" ), helpMenu );
#ifdef Q_WS_MAC
	mainMenu->hide();
#endif
	
	languageChange();
	
	lvMain   = new Q3VBoxLayout ( this );
	lToolbar = new Q3HBoxLayout ( Toolbar );
	
	lvMain->insertWidget ( 0, mainMenu, 1 );
	lvMain->insertWidget ( 1, Toolbar, 1 );
	lvMain->insertSpacing ( 2, 4 );
	lvMain->insertWidget ( 3, splitMain, 8 );
	lvMain->insertSpacing ( 4, 4 );
	lvMain->insertWidget ( 5, statusl, 1 );
	lvMain->insertSpacing ( 6, 4 );
	
	lToolbar->insertSpacing ( 0, 4 );
	lToolbar->insertWidget ( 1, ButtonNew, 1 );
	lToolbar->insertSpacing ( 2, 2 );
	lToolbar->insertWidget ( 3, ButtonOpen, 1 );
	lToolbar->insertSpacing ( 4, 2 );
	lToolbar->insertWidget ( 5, ButtonSave, 1 );
	lToolbar->insertSpacing ( 6, 2 );
	lToolbar->insertWidget ( 7, ButtonSaveAs, 1 );
	lToolbar->insertSpacing ( 8, 2 );
	lToolbar->insertWidget ( 9, ButtonClose, 1 );
	lToolbar->insertSpacing ( 10, 16 );
	lToolbar->insertWidget ( 11, ButtonAdd, 1 );
	lToolbar->insertSpacing ( 12, 2 );
	lToolbar->insertWidget ( 13, ButtonRescan, 1 );
	lToolbar->insertSpacing ( 14, 2 );
	lToolbar->insertWidget ( 15, ButtonDelete, 1 );
	lToolbar->insertSpacing ( 16, 15 );
	lToolbar->insertWidget ( 17, ButtonFind, 1 );
	lToolbar->insertSpacing ( 18, 15 );
	lToolbar->insertWidget ( 19, ButtonConfig, 1 );
	lToolbar->insertSpacing ( 20, 8 );
	lToolbar->insertStretch ( 21 );
	lToolbar->insertSpacing ( 22, 8 );
	lToolbar->insertWidget ( 23, ButtonHelp, 1 );
	lToolbar->insertSpacing ( 24, 2 );
	lToolbar->insertWidget ( 25, ButtonAbout, 1 );
	lToolbar->insertSpacing ( 26, 2 );
	lToolbar->insertWidget ( 27, ButtonAboutQt, 1 );
	lToolbar->insertSpacing ( 28, 4 );
	
	resize ( ( cconfig->windowSize ).expandedTo ( minimumSizeHint() ) );
	move ( cconfig->windowPos );
	
	Q3ValueList<int> splitterSizesList;
	splitterSizesList.append ( cconfig->mainP1 );
	splitterSizesList.append ( cconfig->mainP2 ); // remaining space for listView
	splitterSizesList.append ( cconfig->mainP3 );
	
	splitMain->setSizes ( splitterSizesList );
	splitMain->setResizeMode ( DirView, QSplitter::Stretch );
	splitMain->setResizeMode ( listView, QSplitter::Stretch );
	splitMain->setStyle ( new QPlastiqueStyle() );

	connect ( DirView , SIGNAL ( folderSelected ( const QString& ) ), guis, SLOT ( listUpdate ( const QString& ) ) );
	connect ( DirView , SIGNAL ( hitkey ( QKeyEvent * ) ), guis, SLOT ( hotKeys ( QKeyEvent * ) ) );
	connect ( DirView, SIGNAL ( rightButtonPressed ( Q3ListViewItem *, const QPoint&, int ) ),
	          guis, SLOT ( showTreeContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );
	connect ( listView, SIGNAL ( rightButtonClicked ( Q3ListViewItem *, const QPoint &, int ) ),
	          guis, SLOT ( showListviewContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );
	
	connect ( listView, SIGNAL ( currentChanged ( Q3ListViewItem * ) ), guis, SLOT ( standOn ( Q3ListViewItem * ) ) );
	connect ( listView, SIGNAL ( clicked ( Q3ListViewItem * ) ), guis, SLOT ( standOn ( Q3ListViewItem * ) ) );
	connect ( listView, SIGNAL ( doubleClicked ( Q3ListViewItem * ) ), guis, SLOT ( doubleClickOn ( Q3ListViewItem * ) ) );
	
	connect ( ButtonNew   , SIGNAL ( clicked() ), guis, SLOT ( newEvent () ) );
	connect ( ButtonOpen  , SIGNAL ( clicked() ), guis, SLOT ( openEvent () ) );
	connect ( ButtonSave  , SIGNAL ( clicked() ), guis, SLOT ( saveEvent () ) );
	connect ( ButtonSaveAs, SIGNAL ( clicked() ), guis, SLOT ( saveasEvent() ) );
	connect ( ButtonClose , SIGNAL ( clicked() ), guis, SLOT ( closeEvent () ) );
	connect ( ButtonAdd   , SIGNAL ( clicked() ), guis, SLOT ( addEvent () ) );
	connect ( ButtonDelete, SIGNAL ( clicked() ), guis, SLOT ( deleteEvent() ) );
	connect ( ButtonRescan, SIGNAL ( clicked() ), guis, SLOT ( rescanEvent() ) );
	connect ( ButtonFind  , SIGNAL ( clicked() ), guis, SLOT ( findEvent () ) );
	connect ( ButtonConfig, SIGNAL ( clicked() ), guis, SLOT ( configEvent() ) );
	connect ( ButtonHelp  , SIGNAL ( clicked() ), guis, SLOT ( helpEvent () ) );
	connect ( ButtonAbout , SIGNAL ( clicked() ), guis, SLOT ( aboutEvent () ) );
	connect ( ButtonAboutQt , SIGNAL ( clicked() ), guis, SLOT ( aboutQtEvent () ) );
	
	connect ( commentWidget , SIGNAL ( touchdb() ), guis, SLOT ( cHcaption() ) );
	
	show();

	listView ->setFocus();
	
	//scrollArea->resize(QSize(splitMain->handle(2)->width(), scrollArea->height()));
	commentWidget->resize ( QSize ( scrollArea->viewport()->width(), height() - 120 ) );
	commentWidget->updateContents();
	
	//std::cout << "splitmain sizes: " << std::endl;
	//for (int i=0; i < splitMain->sizes().size(); i++)
	//	std::cout << splitMain->sizes().at(i) << std::endl;
	
	if ( cconfig->autoload )
		guis->checkversion ( this, db );
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
	ButtonOpen->setText ( QString::null );
	QToolTip::add ( ButtonOpen, tr ( "Open a catalog from a file." ) );
	ButtonSave->setText ( QString::null );
	QToolTip::add ( ButtonSave, tr ( "Save all modifications to the disc." ) );
	ButtonSaveAs->setText ( QString::null );
	QToolTip::add ( ButtonSaveAs, tr ( "Save the catalog to a new file." ) );
	ButtonClose->setText ( QString::null );
	QToolTip::add ( ButtonClose, tr ( "Close the actual catalog." ) );
	ButtonAdd->setText ( QString::null );
	QToolTip::add ( ButtonAdd, tr ( "Add a new media to the catalog." ) );
	ButtonRescan->setText ( QString::null );
	QToolTip::add ( ButtonRescan, tr ( "Rescan the selected media." ) );
	ButtonDelete->setText ( QString::null );
	QToolTip::add ( ButtonDelete, tr ( "Delete selected node." ) );
	ButtonConfig->setText ( QString::null );
	QToolTip::add ( ButtonConfig, tr ( "Configuration of the program." ) );
	ButtonHelp->setText ( QString::null );
	QToolTip::add ( ButtonHelp, tr ( "Help" ) );
	ButtonNew->setText ( QString::null );
	QToolTip::add ( ButtonNew, tr ( "Create a new, empty catalog and close the previous." ) );
	ButtonFind->setText ( QString::null );
	QToolTip::add ( ButtonFind, tr ( "Search an element in the database(catalog).You can search filenames, directory names, name parts or mp3 tags etc..." ) );
	QToolTip::add ( ButtonAbout, tr ( "About" ) );
	ButtonAboutQt->setText ( QString::null );
	QToolTip::add ( ButtonAboutQt, tr ( "About Qt" ) );

}

void CdCatMainWidget::closeEvent ( QCloseEvent *e ) {

// An error occured during the REQUISTED saving of the database
// (eg: access denied for writing...)
	if ( guis->closeEvent() != 0 )
		return;

	cconfig->windowSize = size();
	cconfig->windowPos  =  pos();
	cconfig->mainP1 = ( splitMain->sizes() ) [0];
	cconfig->mainP2 = ( splitMain->sizes() ) [1];
	cconfig->mainP3 = ( splitMain->sizes() ) [2];

	cconfig->writeConfig();
	QWidget::closeEvent ( e );
}

void CdCatMainWidget::pathScanned ( QString path ) {
// 	std::cerr <<"CdCatMainWidget::pathScanned: " << qPrintable(path) << std::endl;
	statusl->setText ( tr ( "Scanning:" ) + " " + path );
}
// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
