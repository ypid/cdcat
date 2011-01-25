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
#include <q3frame.h>
#include <q3header.h>
#include <q3listview.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
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
#include <Q3PopupMenu>
#include <QKeyEvent>
#include <Q3VBoxLayout>

#include <QPixmap>
#include <QLatin1String>

#ifdef _WIN32
#include <QPlastiqueStyle>
#endif

#include "commwidget.h"
#include "hdirview.h"
#include "dbase.h"
#include "guibase.h"
#include "config.h"
#include "icons.h"

/*
 *  Constructs a CdCatMainWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CdCatMainWidget::CdCatMainWidget ( CdCatConfig *ccp,QApplication *appp,QWidget* parent, const char* name, Qt::WFlags fl )
        : QWidget ( parent, name, fl ) {

    app = appp;

    int fontHeight = ( new QFontMetrics ( app->font() ) )->height();

    if ( !name )
        setName ( "CdCatMainWidget" );
    setIcon ( *get_p_icon() );


    db = NULL;
    cconfig = ccp;
    cconfig->startProgram ( &db,this );

    splitMain = new QSplitter ( this );

    Toolbar = new Q3Frame ( this, "Toolbar" );
    Toolbar->setFrameShape ( Q3Frame::StyledPanel );
    Toolbar->setFrameShadow ( Q3Frame::Raised );
    Toolbar->setMaximumHeight ( 36 );
    Toolbar->setMinimumHeight ( 36 );

    ButtonNew = new QToolButton ( Toolbar, "ButtonNew" );
    ButtonNew->setPixmap ( *get_t_new_icon() );
    ButtonNew->setMaximumSize ( 30,30 );

    ButtonOpen = new QToolButton ( Toolbar, "ButtonOpen" );
    ButtonOpen->setMaximumSize ( 30,30 );
    ButtonOpen->setPixmap ( *get_t_open_icon() );

    ButtonSave = new QToolButton ( Toolbar, "ButtonSave" );
    ButtonSave->setMaximumSize ( 30,30 );
    ButtonSave->setPixmap ( *get_t_save_icon() );

    ButtonSaveAs = new QToolButton ( Toolbar, "ButtonSaveAs" );
    ButtonSaveAs->setMaximumSize ( 30,30 );
    ButtonSaveAs->setPixmap ( * get_t_saveas_icon() );

    ButtonClose = new QToolButton ( Toolbar, "ButtonClose" );
    ButtonClose->setMaximumSize ( 30,30 );
    ButtonClose->setPixmap ( *get_t_close_icon() );

    ButtonAdd = new QToolButton ( Toolbar, "ButtonAdd" );
    ButtonAdd->setMaximumSize ( 30,30 );
    ButtonAdd->setPixmap ( *get_t_add_icon() );

    ButtonRescan = new QToolButton ( Toolbar, "ButtonRescan" );
    ButtonRescan->setMaximumSize ( 30,30 );
    ButtonRescan->setPixmap ( *get_t_rescan_icon() );

    ButtonDelete = new QToolButton ( Toolbar, "ButtonDelete" );
    ButtonDelete->setMaximumSize ( 30,30 );
    ButtonDelete->setPixmap ( *get_t_delete_icon() );

    ButtonConfig = new QToolButton ( Toolbar, "ButtonConfig" );
    ButtonConfig->setMaximumSize ( 30,30 );
    ButtonConfig->setPixmap ( *get_t_config_icon() );

    ButtonHelp = new QToolButton ( Toolbar, "ButtonHelp" );
    ButtonHelp->setMaximumSize ( 30,30 );
    ButtonHelp->setPixmap ( *get_t_help_icon() );


    ButtonFind = new QToolButton ( Toolbar, "ButtonFind" );
    ButtonFind->setMaximumSize ( 30,30 );
    ButtonFind->setPixmap ( *get_t_find_icon() );

    ButtonAbout = new QToolButton ( Toolbar, "ButtonAbout" );
    ButtonAbout->setMaximumSize ( 30,30 );
    ButtonAbout->setPixmap ( *get_t_about_icon() );

    ButtonAboutQt = new QToolButton ( Toolbar, "ButtonAboutQt" );
    ButtonAboutQt->setMaximumSize ( 30,30 );
    ButtonAboutQt->setPixmap ( QPixmap ( QLatin1String ( ":/trolltech/qmessagebox/images/qtlogo-64.png" ) ) );

    statusl=new QLabel ( tr ( "No item selected" ),this,"statusl" );
    statusl->setMaximumHeight ( fontHeight+2 );


    DirView = new HDirectoryView ( &db, splitMain, "DirView" );
    DirView->header()->setLabel ( 0, tr ( "Directory Tree" ) );

    listView = new HQListView ( this, splitMain, "listView" );
    listView->header()->setLabel ( 0, tr ( "Name" ) );
    listView->header()->setLabel ( 1, tr ( "Size" ) );
    listView->clear();


    commentWidget = new CommentWidget ( cconfig,app,splitMain,"CommentWiget" );

    guis=new GuiSlave ( this );


    /*Menubar*/
    QMenuBar   *mainMenu = new QMenuBar ( this,"menu" );
    mainMenu->setMaximumHeight ( fontHeight*2 );
    mainMenu->setMinimumHeight ( fontHeight*2 );
    Q3PopupMenu *fileMenu = new Q3PopupMenu();
    Q3PopupMenu *editMenu = new Q3PopupMenu();
    Q3PopupMenu *inoutMenu = new Q3PopupMenu();
    Q3PopupMenu *othersMenu = new Q3PopupMenu();
    Q3PopupMenu *helpMenu = new Q3PopupMenu();


    fileMenu->insertItem ( *get_t_new_icon(),tr ( "New..." ),guis,SLOT ( newEvent() ),Qt::CTRL+Qt::Key_N );
    fileMenu->insertItem ( *get_t_open_icon(),tr ( "Open..." ),guis,SLOT ( openEvent() ),Qt::CTRL+Qt::Key_O );
    fileMenu->insertItem ( *get_t_save_icon() ,tr ( "Save" ),guis,SLOT ( saveEvent() ),Qt::CTRL+Qt::Key_S );
    fileMenu->insertItem ( *get_t_saveas_icon() ,tr ( "Save As..." ),guis,SLOT ( saveasEvent() ) );
    fileMenu->insertSeparator ();
    historyMenu = new Q3PopupMenu();
    fileMenu->insertItem ( *get_t_open_icon(),tr ( "Recent files..." ),historyMenu );
    connect ( historyMenu, SIGNAL ( activated ( int ) ), guis, SLOT ( openHistoryElementEvent ( int ) ) );

    for ( QStringList::Iterator it = cconfig->hlist.begin(); it != cconfig->hlist.end(); ++it ) {
        if ( !QString ( *it ).isEmpty() ) {
            historyMenu->addAction ( *get_t_open_icon(),QString ( *it ) );
        }
    }
    fileMenu->insertItem ( *get_t_close_icon() ,tr ( "Close" ),guis,SLOT ( closeEvent() ),Qt::CTRL+Qt::Key_W );
    fileMenu->insertSeparator();
    fileMenu->insertItem ( tr ( "Quit" ),this,SLOT ( close() ),Qt::CTRL+Qt::Key_Q );

    editMenu->insertItem ( *get_t_add_icon() ,tr ( "Add media..." ),guis,SLOT ( addEvent() ),Qt::Key_A );
    editMenu->insertItem ( *get_p_icon(),tr ( "Add a link to a CdCat Catalog..." ),guis,SLOT ( addlnkEvent() ),Qt::CTRL+Qt::Key_L );
    editMenu->insertItem ( *get_t_rescan_icon() ,tr ( "Rescan media..." ),guis,SLOT ( rescanEvent() ),Qt::CTRL+Qt::Key_R );
    editMenu->insertSeparator();
    editMenu->insertItem ( *get_m_import_icon(),tr ( "Insert Catalog..." ),guis,SLOT ( insertcEvent() ) );
    editMenu->insertSeparator();
    editMenu->insertItem ( tr ( "Rename node..." ),guis,SLOT ( renameEvent() ),Qt::CTRL+Qt::Key_E );
    editMenu->insertItem ( tr ( "Re-Number media node..." ),guis,SLOT ( renumberEvent() ) );
    editMenu->insertItem ( *get_t_delete_icon() ,tr ( "Delete node" ),guis,SLOT ( deleteEvent() ) );
    editMenu->insertSeparator();
    editMenu->insertItem ( tr ( "Sort media by number" ),guis,SLOT ( sortNuEvent() ) );
    editMenu->insertItem ( tr ( "Sort media by name" ),guis,SLOT ( sortNaEvent() ) );
    editMenu->insertItem ( tr ( "Sort media by type" ),guis,SLOT ( sortTyEvent() ) );
    editMenu->insertItem ( tr ( "Sort media by time" ),guis,SLOT ( sortTiEvent() ) );

    othersMenu->insertItem ( *get_t_find_icon(),tr ( "Seek in database..." ),guis,SLOT ( findEvent() ),Qt::CTRL+Qt::Key_F );
    othersMenu->insertItem ( tr ( "Seek in the panel" ),guis,SLOT ( posEvent() ),Qt::ALT+Qt::Key_S );
    othersMenu->insertItem ( *get_m_borrow_icon(),tr ( "Borrowing info..." ),guis,SLOT ( borrowingEvent() ),Qt::CTRL+Qt::Key_B );
    othersMenu->insertItem ( tr ( "Node size" ),guis,SLOT ( sizeEvent() ),Qt::Key_Space );
    othersMenu->insertItem ( *get_t_config_icon() ,tr ( "Configuration..." ),guis,SLOT ( configEvent() ),Qt::CTRL+Qt::Key_G );
    othersMenu->insertItem ( *get_t_colorconfig_icon() ,tr ( "Set Colors..." ),guis,SLOT ( colorEvent() ) );

    helpMenu->insertItem ( *get_t_help_icon(),tr ( "Help" ) ,guis,SLOT ( helpEvent() ),Qt::Key_F1 );
    helpMenu->insertItem ( *get_t_about_icon(),tr ( "About" ),guis,SLOT ( aboutEvent() ) );
    helpMenu->insertItem ( QPixmap ( QLatin1String ( ":/trolltech/qmessagebox/images/qtlogo-64.png" ) ), tr ( "About Qt" ),guis,SLOT ( aboutQtEvent() ) );

    inoutMenu->insertItem ( *get_m_import_icon(),tr ( "Import database (Gtktalog/Kat-CeDe/Disclib/WhereIsIt?)" ) ,guis,SLOT ( importEvent() ) );
    inoutMenu->insertItem ( *get_m_export_icon(),tr ( "Export database" ) ,guis,SLOT ( exportEvent() ) );

    mainMenu->insertItem ( tr ( "Catalog" ),fileMenu );
    mainMenu->insertItem ( tr ( "Edit" ),editMenu );
    mainMenu->insertItem ( tr ( "Import/Export" ),inoutMenu );
    mainMenu->insertItem ( tr ( "Others" ),othersMenu );
    mainMenu->insertItem ( tr ( "Help" ),helpMenu );

    languageChange();

    lvMain   = new Q3VBoxLayout ( this );
    lToolbar = new Q3HBoxLayout ( Toolbar );

    lvMain->insertWidget ( 0,mainMenu,1 );
    lvMain->insertWidget ( 1,Toolbar,1 );
    lvMain->insertSpacing ( 2,4 );
    lvMain->insertWidget ( 3,splitMain,8 );
    lvMain->insertSpacing ( 4,4 );
    lvMain->insertWidget ( 5,statusl,1 );
    lvMain->insertSpacing ( 6,4 );

    lToolbar->insertSpacing ( 0,4 );
    lToolbar->insertWidget ( 1,ButtonNew,1 );
    lToolbar->insertSpacing ( 2,2 );
    lToolbar->insertWidget ( 3,ButtonOpen,1 );
    lToolbar->insertSpacing ( 4,2 );
    lToolbar->insertWidget ( 5,ButtonSave,1 );
    lToolbar->insertSpacing ( 6,2 );
    lToolbar->insertWidget ( 7,ButtonSaveAs,1 );
    lToolbar->insertSpacing ( 8,2 );
    lToolbar->insertWidget ( 9,ButtonClose,1 );
    lToolbar->insertSpacing ( 10,16 );
    lToolbar->insertWidget ( 11,ButtonAdd,1 );
    lToolbar->insertSpacing ( 12,2 );
    lToolbar->insertWidget ( 13,ButtonRescan,1 );
    lToolbar->insertSpacing ( 14,2 );
    lToolbar->insertWidget ( 15,ButtonDelete,1 );
    lToolbar->insertSpacing ( 16,15 );
    lToolbar->insertWidget ( 17,ButtonFind,1 );
    lToolbar->insertSpacing ( 18,15 );
    lToolbar->insertWidget ( 19,ButtonConfig,1 );
    lToolbar->insertSpacing ( 20,8 );
    lToolbar->insertStretch ( 21 );
    lToolbar->insertSpacing ( 22,8 );
    lToolbar->insertWidget ( 23,ButtonHelp,1 );
    lToolbar->insertSpacing ( 24,2 );
    lToolbar->insertWidget ( 25,ButtonAbout,1 );
    lToolbar->insertSpacing ( 26,2 );
    lToolbar->insertWidget ( 27,ButtonAboutQt,1 );
    lToolbar->insertSpacing ( 28,4 );

    resize ( ( cconfig->windowSize ).expandedTo ( minimumSizeHint() ) );

    move ( cconfig->windowPos );

    Q3ValueList<int> splitterSizesList;
    splitterSizesList.append ( cconfig->mainP1 );
    splitterSizesList.append ( cconfig->mainP2 ); // remaining space for listView
    splitterSizesList.append ( cconfig->mainP3 );

    splitMain->setSizes ( splitterSizesList );
    splitMain->setResizeMode ( DirView, QSplitter::Stretch );
    splitMain->setResizeMode ( listView, QSplitter::Stretch );
#ifdef _WIN32
	splitMain->setStyle(new QPlastiqueStyle());
#endif

    connect ( DirView ,SIGNAL ( folderSelected ( const QString& ) ),guis,SLOT ( listUpdate ( const QString& ) ) );
    connect ( DirView ,SIGNAL ( hitkey ( QKeyEvent * ) ),guis,SLOT ( hotKeys ( QKeyEvent * ) ) );
    connect ( DirView,SIGNAL ( rightButtonPressed ( Q3ListViewItem *, const QPoint&, int ) ),
              guis,SLOT ( showTreeContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );
    connect ( listView,SIGNAL ( rightButtonClicked ( Q3ListViewItem *, const QPoint &, int ) ),
              guis,SLOT ( showListviewContextMenu ( Q3ListViewItem *, const QPoint &, int ) ) );


    connect ( listView,SIGNAL ( currentChanged ( Q3ListViewItem * ) ),guis,SLOT ( standOn ( Q3ListViewItem * ) ) );
    connect ( listView,SIGNAL ( clicked ( Q3ListViewItem * ) ),guis,SLOT ( standOn ( Q3ListViewItem * ) ) );
    connect ( listView,SIGNAL ( doubleClicked ( Q3ListViewItem * ) ),guis,SLOT ( doubleClickOn ( Q3ListViewItem * ) ) );

    connect ( ButtonNew   ,SIGNAL ( clicked() ),guis,SLOT ( newEvent () ) );
    connect ( ButtonOpen  ,SIGNAL ( clicked() ),guis,SLOT ( openEvent () ) );
    connect ( ButtonSave  ,SIGNAL ( clicked() ),guis,SLOT ( saveEvent () ) );
    connect ( ButtonSaveAs,SIGNAL ( clicked() ),guis,SLOT ( saveasEvent() ) );
    connect ( ButtonClose ,SIGNAL ( clicked() ),guis,SLOT ( closeEvent () ) );
    connect ( ButtonAdd   ,SIGNAL ( clicked() ),guis,SLOT ( addEvent () ) );
    connect ( ButtonDelete,SIGNAL ( clicked() ),guis,SLOT ( deleteEvent() ) );
    connect ( ButtonRescan,SIGNAL ( clicked() ),guis,SLOT ( rescanEvent() ) );
    connect ( ButtonFind  ,SIGNAL ( clicked() ),guis,SLOT ( findEvent () ) );
    connect ( ButtonConfig,SIGNAL ( clicked() ),guis,SLOT ( configEvent() ) );
    connect ( ButtonHelp  ,SIGNAL ( clicked() ),guis,SLOT ( helpEvent () ) );
    connect ( ButtonAbout ,SIGNAL ( clicked() ),guis,SLOT ( aboutEvent () ) );
    connect ( ButtonAboutQt ,SIGNAL ( clicked() ),guis,SLOT ( aboutQtEvent () ) );

    connect ( commentWidget ,SIGNAL ( touchdb() ),guis,SLOT ( cHcaption() ) );

    listView ->setFocus();

    if ( cconfig->autoload )
        guis->checkversion ( this,db );

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
    Q3WhatsThis::add ( ButtonClose, QString::null );
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
    Q3WhatsThis::add ( ButtonFind, tr ( "Search an element." ) );
    ButtonAbout->setText ( QString::null );
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
