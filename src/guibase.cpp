/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QRegExp>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QApplication>
#include <QSplitter>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QTemporaryFile>
#include <QProcess>
#include <QStatusBar>
#include <QHeaderView>
#include <QKeySequence>
#include <QDebug>

#include <string.h>
#ifndef _WIN32
    #include <unistd.h>
    #include <sys/wait.h>
#endif

#include "cdcat.h"
#include "guibase.h"
#include "borrow.h"
#include "mainwidget.h"
#include "colorsettings.h"
#include "dbase.h"
#include "cdcat.h"
#include "wdbfile.h"
#include "commwidget.h"
#include "hdirview.h"
#include "newdbdialog.h"
#include "adddialog.h"
#include "find.h"
#include "config.h"
#include "ui_help.h"
#include "import.h"
#include "exportcdcatdb.h"
#include "info.h"
#include "showcontent.h"

#include "icons.h"

#include "misc.h"

#ifndef NO_MEDIAINFO
    #include "cdcatmediainfo.h"
#endif

#ifdef USE_LIBEXIF
    #include "cdcatexif.h"
#endif


#include <iostream>
using namespace std;


char *rbuff = NULL;

CdCatConfig *glob_conf;

/*****************************************************************************
*
*  class HQListView
*
*****************************************************************************/

HQListViewItem::HQListViewItem ( QTreeWidget *parent )
    : QTreeWidgetItem( parent ), etype( HC_UNINITIALIZED ) {
    setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );
}

HQListViewItem::HQListViewItem ( QTreeWidget *parent, QString label1, QString label2, QString label3 )
    : QTreeWidgetItem( parent ), etype( HC_UNINITIALIZED ) {
    setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );
    setText( 0, label1 );
    setText( 1, label2 );
    setText( 2, label3 );
}

bool HQListViewItem::operator <( const QTreeWidgetItem &other ) const {
    // int col = treeWidget()->sortColumn();
    // qDebug() << "HQListViewItem::operator < col: " << col << ", text(0): " << qPrintable(text(0)) << ", other.text(0): " << qPrintable(other.text(0));
    // this is the item to compare
    if (text( 0 ) == "..") {
        return false;
    } else {
        if (etype == HC_DIRECTORY) {
            if (((HQListViewItem &)other).etype == HC_FILE) {
                return true;
            } else {
                return text( 0 ) < other.text( 0 );
            }
        }
        if (etype == HC_FILE) {
            if (((HQListViewItem &)other).etype == HC_DIRECTORY) {
                return false;
            } else {
                return text( 0 ) < other.text( 0 );
            }
        }

        return text( 0 ) < other.text( 0 );
    }
}

/*
 * HQListViewItem::HQListViewItem ( QTreeWidget *parent, QTreeWidgetItem *after, QString label1, QString label2, QString label3 )
 *      : QTreeWidgetItem ( parent ) {
 *      setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
 *      setText(0, label1);
 *      setText(1, label2);
 *      setText(2, label3);
 * }
 */
QString HQListViewItem::key( int column, bool ascending ) const {
    double value = 0;
    int mod = ascending ? etype : 5 - etype;

    switch (column) {
    // NAME
    case 0:
        return text( column ).prepend( '1' + mod );
    // SIZE
    case 1:
        // ret = (QListViewItem::key(1,ascending)).append('0'+mod);
        if (etype == HC_FILE && !text( 1 ).isEmpty()) {
            value = getSizeFS( text( 1 ).toUtf8().constData());
            switch (getSizetFS( text( 1 ).toUtf8().constData())) {
            case UNIT_KBYTE:
                value *= SIZE_ONE_KBYTE;
                break;
            case UNIT_MBYTE:
                value *= SIZE_ONE_MBYTE;
                break;
            case UNIT_GBYTE:
                value *= SIZE_ONE_GBYTE;
                break;
            case UNIT_TBYTE:
                value *= SIZE_ONE_GBYTE * 1024;
                break;
            default:
                break;
            }
            return ((QString().setNum((double)value ))
                    .rightJustified( 10, '0' ))
                   .prepend( '1' + mod );
        }
        if (etype == HC_MEDIA) {                   // HC_MEDIA
            return (QString().setNum( text( 1 ).toInt()))
                   .rightJustified( 10, '0' )
                   .prepend( '1' + mod );
        }
        return text( 0 ).prepend( '1' + mod );
    // TYPE
    case 2:
        return (text( 2 ).append( text( 0 ))).prepend( '1' + mod );
    }
    return "";
}

void HQListView::changed( void ) {
    // emit currentChanged ( currentItem() ); // FIXME: maybe obsolete
}

void HQListView::start( void ) {
    ;
}

HQListView::HQListView ( CdCatMainWidget *mw, QWidget *parent, const char *, Qt::WindowFlags )
    : QTreeWidget( parent ) {
    mainw = mw;
    setColumnCount( 3 );
    QStringList labels;
    labels.append( tr( "Name" ));
    labels.append( tr( "Size" ));
    labels.append( tr( "Type" ));
    setHeaderLabels( labels );
    setIndentation( 0 );   // remove the indention (|_ sign)
    setSortingEnabled( true );
    setContextMenuPolicy( Qt::CustomContextMenu );
    setSorting( 0, true );
    sortByColumn( 0, Qt::AscendingOrder );

    /*
     * setSelectionMode ( Single );
     * setAllColumnsShowFocus ( true );
     */
}

void HQListView::setCurrentVisible( void ) {
/*	QTreeWidgetItem * i;
 *      if ( ( i = currentItem() ) != 0 )
 *              ensureItemVisible ( i );
 */ // FIXME: maybe obsolete
}

void HQListView::setSorting( int column, bool increasing ) {
    scol = column;
    sasc = increasing;

    if (scol < 0 || scol > 3) {
        scol = 0;
    }
    if (increasing) {
        sortByColumn( scol, Qt::AscendingOrder );
    } else {
        sortByColumn( scol, Qt::DescendingOrder );
    }
}

void HQListView::keyPressEvent( QKeyEvent *ke ) {
    Node *tmp;
    QTreeWidgetItem *it;


    if (mainw->guis->hotKeys( ke )) {
        return;         // It was a hotkey and it's handled yet
    }
    if (mainw->db == NULL) {
        return;
    }

    if (ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Backspace) {
        if (mainw->guis->NodePwd->parent != NULL) {
            mainw->guis->tmpParent = mainw->guis->NodePwd;
            mainw->guis->NodePwd = mainw->guis->NodePwd->parent;
        }

        mainw->guis->updateListFromNode();
    }

    if (ke->key() == Qt::Key_Right || ke->key() == Qt::Key_Return) {
        it = currentItem();
        if (!strcmp( it->text( 0 ).toUtf8().constData(), ".." )) {
            if (mainw->guis->NodePwd->parent != NULL) {
                mainw->guis->tmpParent = mainw->guis->NodePwd;
                mainw->guis->NodePwd = mainw->guis->NodePwd->parent;
            }
        } else {         // step down
            tmp = mainw->guis->NodePwd->child;
            while (strcmp( tmp->getNameOf().toUtf8().constData(), it->text( 0 ).toUtf8().constData())) {
                tmp = tmp->next;
                if (tmp == NULL) {
                    return;
                }
            }
            if (tmp->type == HC_FILE) {
                mainw->guis->showContent();
                return;
            }
            if (tmp->type == HC_CATLNK) {
                mainw->guis->followLnk();
                return;
            }

            mainw->guis->NodePwd = tmp;
        }

        mainw->guis->updateListFromNode();
    }

    setCurrentVisible();
    if (ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right) {
        return;
    }

    QTreeWidget::keyPressEvent( ke );
}

CdcatKeyBinding::CdcatKeyBinding ( int eventDefine, QKeySequence eventSequence, QString description ) {
    this->eventDefine = eventDefine;
    this->eventSequence = eventSequence;
    this->description = description;
}

GuiSlave::GuiSlave ( CdCatMainWidget *p ) {
    mainw = p;

    standON = NULL;
    tmpParent = NULL;
    if (mainw->db != NULL) {
        updateListFromNode( mainw->db->getRootNode());
        standOn( mainw->listView->currentItem(), 0 );
    }
    glob_conf = p->cconfig;
}

GuiSlave::~GuiSlave() {
}

void GuiSlave::checkversion( QWidget *p, DataBase *db ) {
    if (db == NULL) {
        return;
    }
    if ((db->getRootNode()) == NULL) {
        return;
    }
    if (((db->getRootNode())->data) == NULL) {
        return;
    }

    QString fv = ((DBCatalog *)((db->getRootNode())->data))->fileversion;

    if (fv.isEmpty()) {     // vers < 1.2
        return;
    }

    if (fv == "1.2") {
        return;
    }
    if (fv == "1.3") {
        return;
    }
    if (fv == "1.4") {
        return;
    }
    if (fv == "2.0") {
        return;
    }
    if (fv == "2.1") {
        return;
    }
    if (fv == "2.2") {
        return;
    }

    QMessageBox::warning( p, tr( "Warning..." ),
                          tr(
                              "The database file has newer version than this version of cdcat can work with:\n\
I understand maximum %1 datafile version but readed %2\n\n\
Strongly recommended to upgrade your cdcat!!!\n\
Homepage: %3" ).arg( DVERS ).arg( fv ).arg( HOMEPAGE ));
}

void GuiSlave::updateStatusl( Node *n ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }

    if (n == NULL) {
        if (mainw->db == NULL) {
            mainw->statusBar()->showMessage( tr( "No database opened." ));
        } else {
            mainw->statusBar()->showMessage( tr( "No item selected." ));
        }
        return;
    }
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << n->getFullPath();
    }
    mainw->statusBar()->showMessage( n->getFullPath());
}

bool GuiSlave::isIdentical( int i ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }

    Node *t;

    if (mainw->db == NULL) {
        return false;
    }
    t = mainw->db->getRootNode();
    if (t->child == NULL) {
        return false;
    }
    t = t->child;
    while (t != NULL) {
        if (t->type == HC_MEDIA) {
            if (((DBMedia *)(t->data))->number == i) {
                return true;
            }
        }

        t = t->next;
    }
    return false;
}

bool GuiSlave::haveContent( Node *node ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    bool need_showc = false;
    Node *tmp = NULL;

    if (node == NULL) {
        return false;
    }

    if (node->type == HC_FILE && ((DBFile *)(node->data))->prop != NULL) {
        tmp = ((DBFile *)(node->data))->prop;
        while (tmp != NULL) {
            if (tmp->type == HC_CONTENT) {
                need_showc = true;
            }
            tmp = tmp->next;
        }
    }
    return need_showc;
}

bool GuiSlave::isIdentical( QString q ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    Node *t;

    if (mainw->db == NULL) {
        return false;
    }
    t = mainw->db->getRootNode();
    if (t->child == NULL) {
        return false;
    }
    t = t->child;
    while (t != NULL) {
        if (t->type == HC_MEDIA) {
            if (((DBMedia *)(t->data))->name == q) {
                return true;
            }
        }

        t = t->next;
    }
    return false;
}

Node *GuiSlave::getNodeFromFullName( Node *root, const QString& newloc ) {
    QString name;

    DEBUG_INFO_ENABLED = init_debug_info();
    /*------------*/
    Node *tmp = NULL;
    Node *step = root;
    int down = 0;
    QStringList strl = newloc.split( '/' );
    int index = 1;

    while (index != strl.size()) {
//     qDebug() << "index: " << index;
        name = strl.at( index - 1 );
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "New name: " << qPrintable( name );
        }

        if (down == 0) {
            // qDebug() << "step->getNameOf(): " << qPrintable(step->getNameOf());
            if (QString( name ) != step->getNameOf()) {
                return NULL;                 /* error Not matching catalog name! */
            }
            down++;
        } else {
            tmp = step->child;
            if (tmp == NULL) {
                return NULL;                  /* I couldn't find the requested element! */
            }
            // qDebug() << "1 tmp->getNameOf(): " << qPrintable(tmp->getNameOf());
            while (tmp->getNameOf() != QString( name )) {
                tmp = tmp->next;
                //	qDebug() << "2 tmp->getNameOf(): " << qPrintable(tmp->getNameOf());
                if (tmp == NULL) {
                    return NULL;                      /* I couldn't find the requested element! */
                }
                down++;
            }
            step = tmp;
        }
        index++;
    }
    return step;
}


/*
 * int i=0,out=0;
 * char *org;
 * char *re_name;
 * char name[256];
 * //------------//
 * Node *tmp=NULL;
 * Node *step =  root;
 * int   down = 0;
 * org=re_name=mstr((const char *)newloc);
 *
 * while(!out)
 * {
 *  for(i=0;re_name[i]!='/' && re_name[i]!='\0';i++ )
 *      name[i]=re_name[i];
 *  if(re_name[i]=='\0')
 *   {
 *     out=1;
 *     continue;
 *   }
 *  name[i]='\0';       // <-
 *
 *  if(down == 0 )
 *   {
 *    if(strcmp(name,step->getNameOf()))
 *       return NULL; //error Not matching catalog name!
 *    down++;
 *   }
 *  else
 *   {
 *    tmp=step->child;
 *    if(tmp==NULL)
 *        return NULL;  //I couldn't find the requested element!
 *
 *    while(strcmp(tmp->getNameOf(),name))
 *     {
 *      tmp=tmp->next;
 *      if(tmp==NULL)
 *          return NULL;  //I couldn't find the requested element!
 *      down++;
 *     }
 *    step=tmp;
 *   }
 *  re_name+=(strlen(name)+1);
 * }
 * delete [] org;
 * return step;
 * }
 */
int GuiSlave::listUpdate( const QString& newloc ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "F-listUpdate 1 newloc: " << qPrintable( newloc );
    }
    if (mainw->db == NULL) {
        return 0;
    }
    Node *pdir = getNodeFromFullName( mainw->db->getRootNode(), newloc );
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "2 newloc: " << qPrintable( newloc );
    }
    if (pdir) {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "pdir: " << qPrintable( pdir->getNameOf());
        }

        // standON = pdir;

        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "====> we are now on " << qPrintable( pdir->getNameOf());
        }
    } else {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "pdir: null";
        }
    }
    updateStatusl( pdir );
    mainw->commentWidget->showNode( pdir, 0 );
    mainw->commentWidget->updateContents();
    standON = NULL;
    return updateListFromNode( pdir );
}

int GuiSlave::updateListFromNode( Node *pdir ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    int fflag = 0;
    Node *tmp;
    HQListViewItem *lvi = NULL;
    QString qstr1;
    QString qstr2;

    if (pdir == NULL) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
        pdir = NodePwd;
    }

    if (pdir == NULL) {
        mainw->listView->clear();
        return 0;
    }
    mainw->listView->clear();
    switch (((DBCatalog *)mainw->db->getRootNode()->data)->sortedBy) {
    case NUMBER:
        mainw->listView->setSortingEnabled( false );
        break;
    case NAME:
        mainw->listView->setSortingEnabled( true );
        break;
    case TYPE:
        mainw->listView->setSortingEnabled( false );
        break;
    case TIME:
        mainw->listView->setSortingEnabled( false );
        break;
    }
    // mainw->listView->setSorting ( mainw->listView->scol, mainw->listView->sasc );

    // Set column text:
    if (pdir != NULL && pdir->type == HC_CATALOG) {
        mainw->listView->model()->setHeaderData( 1, Qt::Horizontal, tr( "Number" ));
    } else {
        mainw->listView->model()->setHeaderData( 1, Qt::Horizontal, tr( "Size" ));
    }

    if (pdir->parent != NULL) {
        lvi = new HQListViewItem( mainw->listView, "..", "", tr( "Directory" ));
        lvi->setIcon( 0, QIcon( *get_v_back_icon()));
        lvi->etype = HC_DIRECTORY;
    }

    NodePwd = pdir;
    tmp = pdir->child;

    /*List everything*/

    // step 1: list dirs
    while (tmp != NULL) {
        if (tmp->type == HC_MP3TAG) {
            return 1;             // Error
        }
        if (tmp->type == HC_DIRECTORY) {
            // 2.(size) Column name:
            qstr1 = " ";

            // 3. Column name:
            qstr2 = tr( "Directory" );

            //	qDebug() <<"GETNAMEOF-----------"<<qPrintable ( tmp->getNameOf() ) <<endl;
            QString valami;
            valami = tmp->getNameOf();

            // !!!
            // valami.append("---1");
            //	qDebug() <<"GETNAMEOF-----------"<<qPrintable ( valami ) <<endl;

            lvi = new HQListViewItem( mainw->listView, valami, qstr1, qstr2 );
            lvi->etype = HC_DIRECTORY;
            lvi->setIcon( 0, QIcon( *get_v_folderclosed_icon()));

            if (tmpParent != NULL) {               /*Return to previous parent*/
                if (tmp->getNameOf() == tmpParent->getNameOf()) {
                    mainw->listView->setCurrentItem( lvi );
                    if (lvi->isExpanded()) {
                        lvi->setExpanded( false );
                    } else {
                        lvi->setExpanded( true );
                    }
                    tmpParent = NULL;
                    fflag = 1;
                }
            }
        }
        tmp = tmp->next;
    }

    tmp = pdir->child;
    // step 2: list other
    while (tmp != NULL) {
        if (tmp->type == HC_MP3TAG) {
            return 1;             // Error
        }
        if (tmp->type != HC_DIRECTORY) {
            // 2.(size) Column name:

            if (tmp->type == HC_FILE) {
                QString filetype = QString( " " ) + tr( getSType(((DBFile *)(tmp->data))->sizeType, true ).toUtf8().constData());
                // qDebug() << "file type " << qPrintable(filetype);
                if (filetype == " " || filetype.isEmpty()) {
                    filetype = " " + getSType(((DBFile *)(tmp->data))->sizeType, false );
                }
                qstr1.sprintf( "%.2f", ((DBFile *)(tmp->data))->size );
                qstr1 = qstr1 + filetype;
            } else {
                if (tmp->type == HC_MEDIA) {
                    qstr1.setNum(((DBMedia *)(tmp->data))->number );
                } else {
                    qstr1 = " ";
                }
            }
            // 3. Column name:
            switch (tmp->type) {
            case HC_CATALOG:
                qstr2 = tr( "Catalog" );
                break;
            case HC_MEDIA:
                switch (((DBMedia *)(tmp->data))->type) {
                case UNKNOWN:
                    qstr2 = tr( "Unknown(DB)" );
                    break;
                case CD:
                    qstr2 = tr( "CD" );
                    break;
                case DVD:
                    qstr2 = tr( "DVD" );
                    break;
                case HARDDISC:
                    qstr2 = tr( "HardDisc" );
                    break;
                case FLOPPY:
                    qstr2 = tr( "Floppy" );
                    break;
                case NETPLACE:
                    qstr2 = tr( "NetworkDrv" );
                    break;
                case FLASHDRV:
                    qstr2 = tr( "FlashDrv" );
                    break;
                case OTHERD:
                    qstr2 = tr( "OtherDevice" );
                    break;
                }
                break;
            case HC_FILE:
                qstr2 = tr( "File" );
                break;
            case HC_CATLNK:
                qstr2 = tr( "Catalog Link" );
                break;
            }

            //	qDebug() <<"GETNAMEOF-----------"<<qPrintable ( tmp->getNameOf() ) <<endl;
            QString valami;
            valami = tmp->getNameOf();

            // !!!
            // valami.append("---1");
            //	qDebug() <<"GETNAMEOF-----------"<<qPrintable ( valami ) <<endl;

            lvi = new HQListViewItem( mainw->listView, valami, qstr1, qstr2 );
            switch (tmp->type) {
            case HC_CATALOG:
                lvi->etype = HC_CATALOG;
                break;
            case HC_MEDIA:
                lvi->etype = HC_MEDIA;
                switch (((DBMedia *)(tmp->data))->type) {
                case UNKNOWN:
                    lvi->setIcon( 0, QIcon( *get_m_unknown_icon()));
                    break;
                case CD:
                    lvi->setIcon( 0, QIcon( *get_m_cd_icon()));
                    break;
                case DVD:
                    lvi->setIcon( 0, QIcon( *get_m_dvd_icon()));
                    break;
                case HARDDISC:
                    lvi->setIcon( 0, QIcon( *get_m_hdd_icon()));
                    break;
                case FLOPPY:
                    lvi->setIcon( 0, QIcon( *get_m_floppy_icon()));
                    break;
                case NETPLACE:
                    lvi->setIcon( 0, QIcon( *get_m_net_icon()));
                    break;
                case FLASHDRV:
                    lvi->setIcon( 0, QIcon( *get_m_flash_icon()));
                    break;
                case OTHERD:
                    lvi->setIcon( 0, QIcon( *get_m_other_icon()));
                    break;
                }
                break;
            case HC_FILE:
                lvi->etype = HC_FILE;
                lvi->setIcon( 0, QIcon( *get_v_file_icon()));
                break;
            case HC_CATLNK:
                lvi->etype = (mainw->cconfig->linkf ? 0 : HC_CATLNK);
                lvi->setIcon( 0, QIcon( *get_p_icon()));
                break;
            }

            if (tmpParent != NULL) {               /*Return to previous parent*/
                if (tmp->getNameOf() == tmpParent->getNameOf()) {
                    mainw->listView->setCurrentItem( lvi );
                    tmpParent = NULL;
                    fflag = 1;
                }
            }
        }
        tmp = tmp->next;
    }

    // mainw->listView->setSorting ( mainw->listView->scol, mainw->listView->sasc );

    if (!fflag) {
        mainw->listView->setCurrentItem( mainw->listView->topLevelItem( 0 ));
    }

    // qDebug() << "current elem is \"..\", parent node path: " << qPrintable(pdir->getFullPath());
    standON = pdir;

    mainw->listView->resizeColumnToContents( 0 );

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "BEACON-1";
    }
    mainw->listView->changed();
    // mainw->DirView->setDir ( NodePwd );
    return 0;
}

int GuiSlave::standOn( QTreeWidgetItem *on, int ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    Node *tmp;

    if (on == NULL || on == 0) {
        return 0;
    }
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "1";
    }
    mainw->listView->setItemSelected( on, true );

    if (on->text( 0 ) == "..") {
        mainw->commentWidget->showNode( NodePwd, 1 );
        mainw->commentWidget->updateContents();
        updateStatusl( NodePwd->parent );
        // qDebug() << "current elem is \"..\", parent node path: " << qPrintable(NodePwd->getFullPath());
        standON = NodePwd;
        return 0;
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "2";
    }

    tmp = NodePwd->child;
    while (tmp->getNameOf() != on->text( 0 )) {
        tmp = tmp->next;
        if (tmp == NULL) {
            return 0;
        }
    }
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "3";
    }
    standON = tmp;
    last_dirview_item = on;

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "4";
    }
    mainw->commentWidget->showNode( tmp, 0 );
    mainw->commentWidget->updateContents();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "5";
    }
    updateStatusl( tmp );
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "6";
    }

    return 0;
}

int GuiSlave::doubleClickOn( QTreeWidgetItem *on, int ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    Node *tmp;
    if (on->text( 0 ) == "..") {
        if (NodePwd->parent != NULL) {
            tmpParent = NodePwd;
            NodePwd = NodePwd->parent;
        }
    } else {     // step down
        tmp = NodePwd->child;
        while (tmp->getNameOf() != on->text( 0 )) {
            tmp = tmp->next;
            if (tmp == NULL) {
                return 0;
            }
        }
        if (tmp->type == HC_FILE) {
            showContent();
            return 0;
        }
        if (tmp->type == HC_CATLNK) {
            followLnk();
            return 0;
        }
        NodePwd = tmp;
    }
    updateListFromNode();
    return 0;
}

void GuiSlave::currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *before ) {
//      if (current != NULL) {
//              current->setBackground(0, *mainw->cconfig->comm_bg);
//              current->setTextColor(0, Qt::black);
//              current->setBackground(1, *mainw->cconfig->comm_bg);
//              current->setTextColor(1, Qt::black);
//              current->setBackground(2, *mainw->cconfig->comm_bg);
//              current->setTextColor(2, Qt::black);
//      }
//      if (before != NULL) {
//              QColor bgColor (Qt::white);
//              QColor textcolor (Qt::black);
//              //QColor color = tmp_item.textColor(0);
//              before->setForeground(0, bgColor);
//              before->setTextColor(0, textcolor);
//              before->setForeground(1, bgColor);
//              before->setTextColor(1, textcolor);
//              before->setForeground(2, bgColor);
//              before->setTextColor(2, textcolor);
//      }
}

void GuiSlave::panelsOFF( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    mainw->DirView->clear();
    mainw->listView->clear();
    if (mainw->db != NULL) {
        tmpParent = NodePwd = mainw->db->getRootNode();
    } else {
        tmpParent = NodePwd = NULL;
    }
    mainw->commentWidget->act = NULL;
}

int GuiSlave::cHcaption( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    if ((mainw->db != NULL) && (((DBCatalog *)(mainw->db->getRootNode()->data))->writed == 0)) {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "case:1";
        }
        mainw->setWindowTitle( PROGRAM_NAME + tr( "modified" ));
    } else {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "case:2";
        }

        mainw->setWindowTitle( PROGRAM_NAME );
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "F-done";
    }
    return 0;
}

void GuiSlave::panelsON( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    panelsOFF();     // That case you forget the OFF before...

    mainw->DirView->start();
    mainw->listView->start();
    updateListFromNode();
    mainw->commentWidget->act = NULL;
    // mainw->commentWidget->repaint();
    updateStatusl( standON );
    standOn( mainw->listView->currentItem(), 0 );
    cHcaption();
}

void GuiSlave::setGuiMenuAndToolBarEnabled( bool enable ) {
    if (enable) {
        mainw->Toolbar->setEnabled( true );
        mainw->mainMenu->setEnabled( true );
    } else {
        mainw->Toolbar->setEnabled( false );
        mainw->mainMenu->setEnabled( false );
    }
}

void GuiSlave::showListviewContextMenu( QPoint p ) {
    mPopup = new QMenu( mainw );
    QTreeWidgetItem *on = mainw->listView->itemAt( p );
    if (on != last_dirview_item) {
        standOn( on, 0 );
    }

    if (standON != NULL) {
        mPopup->addAction( QIcon( *get_t_comment_icon()), tr( "View/Edit Comment..." ), this, SLOT( editComment()));
        mPopup->addAction( QIcon( *get_t_comment_icon()), tr( "View/Edit Category..." ), this, SLOT( editCategory()));
        mPopup->insertSeparator( NULL );
        mPopup->addAction( tr( "Node size" ), this, SLOT( sizeEvent()));
        mPopup->insertSeparator( NULL );

        if (haveContent( standON )) {
            if (mainw->cconfig->useExternalContentViewer && QFileInfo( mainw->cconfig->ExternalContentViewerPath ).exists()) {
                mPopup->addAction( QIcon( *get_t_showc_icon()), tr( "Show content..." ), this, SLOT( showContent()));
            } else {
                mPopup->addAction( QIcon( *get_t_showc_icon()), tr( "Show/Remove content..." ), this, SLOT( showContent()));
            }
        }

        if (standON->type == HC_CATLNK) {
            mPopup->addAction( QIcon( *get_p_icon()), tr( "Follow the link (Open it) !" ), this, SLOT( followLnk()));
        }

        mPopup->addAction( tr( "Rename node..." ), this, SLOT( renameEvent()));
        mPopup->addAction( QIcon( *get_t_delete_icon()), tr( "Delete node" ), this, SLOT( deleteEvent()));

        if (standON->type == HC_MEDIA) {
            mPopup->insertSeparator( NULL );
            if (((DBMedia *)(standON->data))->borrowing == "") {
                mPopup->addAction( QIcon( *get_t_sborrow_icon()), tr( "Borrow this media to..." ), this, SLOT( sborrowEvent()));
            } else {
                mPopup->addAction( QIcon( *get_t_cborrow_icon()), tr( "I got it back! (clear borrowing mark)" ), this, SLOT( cborrowEvent()));
            }

            mPopup->insertSeparator( NULL );
            mPopup->addAction( QIcon( *get_t_rescan_icon()), tr( "Rescan media..." ), this, SLOT( rescanEvent()));
            mPopup->addAction( tr( "Re-Number media..." ), this, SLOT( renumberEvent()));
        }
        if (standON->type == HC_FILE) {
            mPopup->insertSeparator( NULL );
            mPopup->addAction( tr( "search for duplicates..." ), this, SLOT( searchDuplicatesEvent()));
        }
    }
    mPopup->insertSeparator( NULL );
    mPopup->addAction( QIcon( *get_t_add_icon()), tr( "Add media..." ), this, SLOT( addEvent()));
    mPopup->addAction( QIcon( *get_p_icon()), tr( "Add a link to a CdCat Catalog..." ), this, SLOT( addlnkEvent()));
    mPopup->addAction( tr( "Insert Catalog..." ), this, SLOT( insertcEvent()));
    mPopup->exec( mainw->listView->viewport()->mapToGlobal( p ));
    delete mPopup;
    mPopup = NULL;
}

void GuiSlave::showTreeContextMenu( const QPoint p2 ) {
    Node *on, *save;

    if (mainw->db == NULL) {
        on = NULL;
    } else {
        on = getNodeFromFullName(
            mainw->db->getRootNode(),
            ((LNode *)mainw->DirView->currentItem())->fullName()
            );
    }

    mPopup = new QMenu( mainw );
    if (on != NULL) {
        mPopup->addAction( QIcon( *get_t_comment_icon()), tr( "View/Edit Comment..." ), this, SLOT( editComment()));
        mPopup->addAction( tr( "View/Edit Category..." ), this, SLOT( editCategory()));
        mPopup->insertSeparator( NULL );
        mPopup->addAction( tr( "Node size" ), this, SLOT( sizeEvent()));
        mPopup->insertSeparator( NULL );
    }
    if (on != NULL && on->type != HC_CATALOG) {
        mPopup->addAction( tr( "Rename node..." ), this, SLOT( renameEvent()));
        mPopup->addAction( QIcon( *get_t_delete_icon()), tr( "Delete node" ), this, SLOT( deleteEvent()));
        mPopup->insertSeparator( NULL );
        if (on->type == HC_MEDIA) {
            if (((DBMedia *)(on->data))->borrowing == "") {
                mPopup->addAction( QIcon( *get_t_sborrow_icon()), tr( "Borrow this media to..." ), this, SLOT( sborrowEvent()));
            } else {
                mPopup->addAction( QIcon( *get_t_cborrow_icon()), tr( "I got it back! (clear borrowing mark)" ), this, SLOT( cborrowEvent()));
            }

            mPopup->addAction( QIcon( *get_t_rescan_icon()), tr( "Rescan media..." ), this, SLOT( rescanEvent()));
            mPopup->addAction( tr( "Re-Number media..." ), this, SLOT( renumberEvent()));

            mPopup->insertSeparator( NULL );
            context_item = mainw->DirView->itemAt( p2 );
            mPopup->addAction( QIcon( *get_t_add_icon()), tr( "Change media type..." ), this, SLOT( typeChangeEvent()));
        }
    }
#ifdef CATALOG_ENCRYPTION
    if (on != NULL && on->type == HC_CATALOG) {
        bool isEncryptedCatalog = ((DBCatalog *)((on)->data))->isEncryptedCatalog;
        if (isEncryptedCatalog) {
            mPopup->insertSeparator( NULL );
            mPopup->addAction( QIcon( *get_t_change_encryption_password_icon()), tr( "Change password..." ), this, SLOT( changePassEvent()));
            mPopup->addAction( QIcon( *get_t_remove_encryption_icon()), tr( "Disable encryption" ), this, SLOT( disableEncryptionEvent()));
            mPopup->insertSeparator( NULL );
        } else {
            mPopup->addAction( QIcon( *get_t_add_encryption_icon()), tr( "Enable encryption" ), this, SLOT( Event()));
        }
    }
#endif

    mPopup->addAction( QIcon( *get_t_add_icon()), tr( "Add media..." ), this, SLOT( addEvent()));
    mPopup->addAction( QIcon( *get_p_icon()), tr( "Add a link to a Cdcat catalog..." ), this, SLOT( addlnkEvent()));
    mPopup->addAction( tr( "Insert Catalog..." ), this, SLOT( insertcEvent()));
    // mPopup->addAction ( tr ( "Insert Catalog (no duplicates)..." ), this, SLOT(insertcEventNoDup()) );
    if (on != NULL) {
        mPopup->insertSeparator( NULL );
        mPopup->addAction( tr( "Close all branch" ), this, SLOT( closeBranch()));
    }

    save = standON;
    standON = on;
    mPopup->exec( mainw->DirView->viewport()->mapToGlobal( p2 ));
    standON = save;
    delete mPopup;
    mPopup = NULL;
    context_item = NULL;
}

int GuiSlave::hotKeys( QKeyEvent *ke ) {
    switch (ke->key()) {
    /*  Handled from mainwidget.cpp
     *
     *  case (Qt::Key_O) :      openEvent();    return 1;
     *  case (Qt::Key_N) :      newEvent();     return 1;
     *  case (Qt::Key_C) :      closeEvent();   return 1;
     *  case (Qt::Key_A) :      addEvent();     return 1;
     *  case (Qt::Key_R) :      rescanEvent();  return 1;
     *  case (Qt::Key_G) :      configEvent();  return 1;
     *  case (Qt::Key_F) :      findEvent();    return 1;
     *  case (Qt::Key_H) :      helpEvent();    return 1;
     *  case (Qt::Key_Q) :      mainw->close(); return 1;
     *  case (Qt::Key_Space) :  sizeEvent();    return 1;
     */
    case (Qt::Key_Escape):
        mainw->close();
        return 1;

    case (Qt::Key_S):
        if (ke->modifiers() == Qt::ControlModifier) {
            posEvent();
        }
        return 1;
    }
    return 0;
}

QKeySequence GuiSlave::getKeyBinding( int eventDefine ) {
    for (int i = 0; i < KeyShortCutList.size(); ++i)
        if (KeyShortCutList.at( i ).eventDefine == eventDefine) {
            return KeyShortCutList.at( i ).eventSequence;
        }
    return QKeySequence::UnknownKey;
}

/* Mainwindow buttons ***********************************/
int GuiSlave::newEvent( void ) {
    newdbdialog *d = new newdbdialog( mainw, "new_db_dialog", true );

    while (closeEvent() != 0) { }

    d->exec();
    if (d->OK == 1) {
        panelsOFF();
        if (mainw->db == NULL) {
            mainw->db = new DataBase();
        }
        mainw->db->setDBName( d->nameD );
        mainw->db->setDBOwner( d->ownerD );
        mainw->db->setComment( d->commD );
        mainw->db->setCategory( d->categoryD );
        mainw->db->storeMp3tags = mainw->cconfig->readtag;
        mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
        mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
        mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
        mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
        mainw->db->storeContent = mainw->cconfig->readcontent;
        mainw->db->storeExifData = mainw->cconfig->storeExifData;
        mainw->db->storeThumb = mainw->cconfig->storeThumb;
        mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
        mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
        mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
        mainw->db->storeLimit = mainw->cconfig->readclimit;
        mainw->db->storedFiles = mainw->cconfig->readcfiles;
        mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
        mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
        mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;

#ifdef CATALOG_ENCRYPTION
        ((DBCatalog *)((mainw->db->getRootNode())->data))->isEncryptedCatalog = d->catalogEncrypted;
        generate_cryptokey( d->encryptionPassword );
#endif
        panelsON();
    }
    return 0;
}

int GuiSlave::openEvent( void ) {
    char fnc[256];
    QString fn;
    int ret_val = 0;

    DEBUG_INFO_ENABLED = init_debug_info();

    fn = QFileDialog::getOpenFileName( 0, tr( "Open a file..." ), mainw->cconfig->lastDir, tr( "CdCat databases (*.hcf )" ));
    if (fn.isEmpty()) {
        return 0;
    }
    QApplication::setOverrideCursor( Qt::WaitCursor );
    mainw->cconfig->lastDir = QFileInfo( fn ).absoluteDir().absolutePath();

    strcpy( fnc, (const char *)(QFile::encodeName( fn )));
    while (closeEvent() != 0) {
    }
    ;
    panelsOFF();

    if (mainw->db == NULL) {
        mainw->db = new DataBase();
    }
    mainw->db->setDoArchiveScan( mainw->cconfig->doScanArchive );
    mainw->db->storeMp3tags = mainw->cconfig->readtag;
    mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
    mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
    mainw->db->storeContent = mainw->cconfig->readcontent;
    mainw->db->storeExifData = mainw->cconfig->storeExifData;
    mainw->db->storeThumb = mainw->cconfig->storeThumb;
    mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
    mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
    mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
    mainw->db->storeLimit = mainw->cconfig->readclimit;
    mainw->db->storedFiles = mainw->cconfig->readcfiles;
    mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
    mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
    mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    progress( pww );


    if ((ret_val = mainw->db->openDB( fnc )) != 0) {          // An error occurred
        QMessageBox::warning( mainw, tr( "Error while opening..." ),
                              mainw->db->errormsg );
        mainw->db = NULL;
        standON = NodePwd = NULL;
        panelsOFF();
        QApplication::restoreOverrideCursor();
    }

    checkversion( mainw, mainw->db );

    progress( pww );
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "FLAG-1";
    }
    panelsON();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "FLAG-2";
    }

    progress( pww );

    // Save the opened file to the history
    // QMessageBox::information(0,"new history element",fn);
    if (ret_val == 0 && !fn.isEmpty()) {
        if (*DEBUG_INFO_ENABLED) { qDebug() << "LIST:" << qPrintable( mainw->cconfig->hlist.join( "|" ));
        }

        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "0-1";
        }
//         if ( mainw->cconfig->hlist.isEmpty() ) qDebug() <<"emptlyysdsaf?ashfk"<<endl;
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "0-2";
        }
        // mainw->cconfig->hlist.grep ( "AAAA" );
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "0-3";
        }


        if (mainw->cconfig->hlist.isEmpty() ||
            mainw->cconfig->hlist.filter( QString( fn )).isEmpty()) {
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "1";
            }
            mainw->cconfig->hlist.insert( 0, QString( fn ));
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "2";
            }
            QAction *newaction = new QAction( *get_p_icon(), fn, 0 );
            mainw->historyMenu->addAction( newaction );
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "3";
            }
            if ((int)mainw->cconfig->hlist.count() > (int)mainw->cconfig->historysize) {
                if (*DEBUG_INFO_ENABLED) {
                    qDebug() << "4";
                }
                (mainw->cconfig->hlist).removeLast();
                if (*DEBUG_INFO_ENABLED) {
                    qDebug() << "5";
                }
                QAction *last_action = mainw->historyMenu->actions().last();
                mainw->historyMenu->removeAction( last_action );
                delete last_action;
            }
        }
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "FLAG-3";
    }

    progress( pww );
    pww->end();
    if (mainw->db != NULL) {
        mainw->db->pww = NULL;
    }
    delete pww;

#ifdef CATALOG_ENCRYPTION
    if (mainw->db != NULL) {
        Node *root = mainw->db->getRootNode();
        bool isEncryptedCatalog = ((DBCatalog *)((root)->data))->isEncryptedCatalog;
        if (isEncryptedCatalog) {
            mainw->disableencryption_action->setEnabled( true );
            mainw->enableencryption_action->setEnabled( false );
        } else {
            mainw->disableencryption_action->setEnabled( false );
            mainw->enableencryption_action->setEnabled( true );
        }
    }
#endif

    QApplication::restoreOverrideCursor();
    return 0;
}

int GuiSlave::saveEvent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    pww->setProgressText( tr( "Saving catalog, please wait..." ));
    pww->show();
    progress( pww );

    QApplication::setOverrideCursor( Qt::WaitCursor );
    mainw->db->setNice( mainw->cconfig->nice );
    if (mainw->cconfig->saveAlwaysCatalogInUtf8) {
        mainw->db->XML_ENCODING = "UTF-8";
    }
    if (mainw->db->saveDB() != 0) {
        progress( pww );
        pww->end();
        mainw->db->pww = NULL;
        delete pww;

        saveasEvent();
        QApplication::restoreOverrideCursor();
        return 0;
    }

    panelsOFF();
    panelsON();

    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    QApplication::restoreOverrideCursor();
    return 0;
}

int GuiSlave::saveasEvent( void ) {
    int retv = 0;
    char fnc[256];
    QString fn;

    if (mainw->db == NULL) {
        return 0;
    }
    fn = QFileDialog::getSaveFileName( 0, tr( "Save to file..." ), mainw->cconfig->lastDir, tr( "CdCat databases (*.hcf )" ));
    if (fn.isEmpty()) {
        return 0;
    }


    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    pww->setProgressText( tr( "Saving catalog, please wait..." ));
    pww->show();

    progress( pww );
    QApplication::setOverrideCursor( Qt::WaitCursor );

    strcpy( fnc, (const char *)QFile::encodeName( fn ));

    // extension correction if necessary
    if (strlen( fnc ) < 5 || strcmp((fnc + (strlen( fnc ) - 4)), ".hcf" ) != 0) {
        sprintf( fnc, "%s.hcf", fn.toUtf8().constData());
    }

    progress( pww );

    mainw->db->setNice( mainw->cconfig->nice );
    if (mainw->cconfig->saveAlwaysCatalogInUtf8) {
        mainw->db->XML_ENCODING = "UTF-8";
    }
    if (mainw->db->saveAsDB( fnc ) != 0) {        // An error occurred
        QMessageBox::warning( mainw, tr( "Error while saving..." ), mainw->db->errormsg );
        retv = 2;
        // The file cannot be writed
        // (It is the only reason the saveAsSB return nonzero)
    } else {
        // add history item
        if (mainw->cconfig->hlist.isEmpty() || mainw->cconfig->hlist.filter( QString( fnc )).isEmpty()) {
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "1";
            }
            mainw->cconfig->hlist.insert( 0, QString( fnc ));
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "2";
            }
            QAction *newaction = new QAction( *get_p_icon(), fnc, 0 );
            mainw->historyMenu->addAction( newaction );
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "3";
            }
            if ((int)mainw->cconfig->hlist.count() > (int)mainw->cconfig->historysize) {
                if (*DEBUG_INFO_ENABLED) {
                    qDebug() << "4";
                }
                (mainw->cconfig->hlist).removeLast();
                if (*DEBUG_INFO_ENABLED) {
                    qDebug() << "5";
                }
                QAction *last_action = mainw->historyMenu->actions().last();
                mainw->historyMenu->removeAction( last_action );
                delete last_action;
            }
        }
    }
    panelsOFF();
    progress( pww );
    panelsON();

    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    QApplication::restoreOverrideCursor();
    return retv;
}

int GuiSlave::closeEvent( void ) {
    int userrq = 0;
    int retv = 0;

    if (mainw->db == NULL) {
        return 0;
    }
    if (((DBCatalog *)(mainw->db->getRootNode()->data))->writed == 0) {
        userrq = QMessageBox::warning( mainw, tr( "Closing previous database..." ), tr( "Do you want to save the changes?" ),
                                       tr( "Yes" ), tr( "No" ));

        if (userrq == -1) {
            retv = -1;
        }

        if (userrq == 0) {
            if ((retv = mainw->db->saveDB()) != 0) {
                if (retv == 1) {                 // The databas had no name.
                    retv = saveasEvent();
                }
                // retv can be nonrezo reasoned by saveDB or saveAsEvent too.
            }
        }
    }
    panelsOFF();

    mainw->db->doWork = false;

    /* Freeing database object */
    if (retv == 0) {
        delete mainw->db;
        mainw->db = NULL;
        standON = NodePwd = NULL;
    }
    return retv;
}

int GuiSlave::deleteEvent( void ) {
    QString o;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected item in the middle list box!" ));
        return 0;
    }

    o = tr( "Do you really want to delete\n\"%1\"?" ).arg( standON->getFullPath());
    if (QMessageBox::warning( mainw, tr( "Confirmation" ), o,
                              tr( "Yes" ), tr( "No" )) == 0) {
        panelsOFF();
        mainw->db->deleteNode( standON );
        standON = NULL;
        panelsON();
    }
    return 0;
}

int GuiSlave::addEvent( void ) {
    int i;

#if !defined(_WIN32) && !defined(_OS2)
    bool mount_successful = false;
#endif

    QString cdrom_mountpath = mainw->cconfig->cdrompath;
    DEBUG_INFO_ENABLED = init_debug_info();
    if (mainw->db == NULL) {
        newEvent();
    }
    if (mainw->db == NULL) {
        return 0;
    }

    addDialog *d = new addDialog( this, mainw, "adddialog", true );
    d->exec();

    mainw->db->storeMp3tags = mainw->cconfig->readtag;
    mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
    mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
    mainw->db->storeContent = mainw->cconfig->readcontent;
    mainw->db->doScanArchive = mainw->cconfig->doScanArchive;
    mainw->db->doScanArchiveTar = mainw->cconfig->doScanArchive;     // FIXME: make dynamiclly
    mainw->db->doScanArchiveLib7zip = mainw->cconfig->doScanArchive; // FIXME: make dynamiclly
    mainw->db->storeExifData = mainw->cconfig->storeExifData;
    mainw->db->storeThumb = mainw->cconfig->storeThumb;
    mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
    mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
    mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
    mainw->db->storeLimit = mainw->cconfig->readclimit;
    mainw->db->storedFiles = mainw->cconfig->readcfiles;
    mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
    mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
    mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;
    mainw->db->displayCurrentScannedFileInTray = mainw->cconfig->displayCurrentScannedFileInTray;

    panelsOFF();

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "ADDEVENT-1";
    }
    PWw *pww = mainw->pww;
    pww->showProgress = false;
    pww->setProgressText( tr( "Scanning directory tree, please wait..." ));
    pww->setCancel( true );
    mainw->db->pww = pww;
    QApplication::setOverrideCursor( Qt::WaitCursor );
    d->type = mainw->cconfig->lastMediaType;


#if !defined(_WIN32) && !defined(_OS2)
    if ((d->type == CD || d->type == DVD)) {
        d->dDir = mainw->cconfig->cdrompath;
        int pid;
        char **arg = new char *[3];
        char **env = new char *[2];

        if (mainw->cconfig->mounteject) {
            // search the mount program:
            if (QFile( "/usr/local/bin/mount" ).exists()) {
                arg[0] = mstr( "/usr/local/bin/mount" );
            } else {
                if (QFile( "/usr/local/sbin/mount" ).exists()) {
                    arg[0] = mstr( "/usr/local/sbin/mount" );
                } else {
                    if (QFile( "/usr/bin/mount" ).exists()) {
                        arg[0] = mstr( "/usr/bin/mount" );
                    } else {
                        if (QFile( "/usr/sbin/mount" ).exists()) {
                            arg[0] = mstr( "/usr/sbin/mount" );
                        } else {
                            if (QFile( "/bin/mount" ).exists()) {
                                arg[0] = mstr( "/bin/mount" );
                            } else {
                                if (QFile( "/sbin/mount" ).exists()) {
                                    arg[0] = mstr( "/sbin/mount" );
                                } else {
                                    QMessageBox::warning
                                        ( d, tr( "Cannot mount CD" ), tr( "I can't find the \"mount\" program" ));
                                    arg[0] = mstr( "mount" );
                                }
                            }
                        }
                    }
                }
            }
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "mount program found at: " << arg[0];
            }
        }
        if (QFile( "/etc/mtab" ).exists()) {
            // check if already mounted
            QFile file( "/etc/mtab" );
            if (file.open( QIODevice::ReadOnly | QIODevice::Text )) {
                //	qDebug() << "/etc/mtab opened";

                QString line = file.readLine();
                while (line.length() > 0) {
                    line = file.readLine();
                    //	qDebug() << "mtab line: " << qPrintable(line);
                    QStringList mtablist = line.split( ' ' );
                    if (mtablist.size() > 3) {
                        QString mtab_devicepath = mtablist.at( 0 );
                        QString mtab_mountpath = mtablist.at( 1 );
                        //	qDebug() << "mtab device path: " << qPrintable(mtab_mountpath)<< " <=> " << qPrintable(mainw->cconfig->cdrompath);
                        if (mtab_mountpath == mainw->cconfig->cdrompath) {
                            if (*DEBUG_INFO_ENABLED) {
                                qDebug() << "mount path " << qPrintable( mtab_mountpath ) << " (" << qPrintable( mtab_devicepath ) << ") is already mounted, skipping mount";
                            }
                            mount_successful = true;
                            break;
                        }
                        if (mtab_devicepath == mainw->cconfig->cdrompath) {
                            if (*DEBUG_INFO_ENABLED) {
                                qDebug() << "device path " << qPrintable( mtab_devicepath ) << " is already mounted (" << qPrintable( mtab_mountpath ) << "), skipping mount";
                            }
                            mount_successful = true;
                            cdrom_mountpath = mtab_mountpath;
                            d->dDir = mtab_mountpath;
                            break;
                        }
                    } else {
                        //	qWarning() << "invalid mtab line: " << qPrintable(line) << ", skipping";
                    }
                }
                file.close();
                //	qDebug() << "/etc/mtab closed";
            } else {
                if (*DEBUG_INFO_ENABLED) {
                    qWarning() << "mtab could not openend";
                }
            }
        } else {
            if (*DEBUG_INFO_ENABLED) {
                qWarning() << "mtab could not found";
            }
        }
        if (mainw->cconfig->mounteject && !mount_successful) {
            if (*DEBUG_INFO_ENABLED) {
                qWarning("Call: %s %s...", arg[0], (const char *)cdrom_mountpath.toUtf8().constData());
            }
            arg[1] = mstr( cdrom_mountpath.toUtf8().constData());
            arg[2] = 0;
            env[0] = mstr( "PATH=/usr/local/bin:/usr/bin:/bin" );
            env[1] = 0;

            pid = fork();
            if (pid == 0) {               // mount process
                if (execve( arg[0], (char *const *)arg, (char *const *)env ) == -1) {
                    exit( -1 );
                } else {
                    exit( 0 );        // That's never happend.
                }
            } else {                  // cdcat process
                int v;
                pww->refreshTime = 200;
                while (0 == waitpid( pid, &v, WNOHANG )) {
                    progress( pww );
                    usleep( 500 );
                }
                pww->refreshTime = 100;
                qDebug() << "done";
                if (WEXITSTATUS( v ) != 0) {
                    QMessageBox::warning( 0, tr( "Cannot mount CD!" ), tr( "Cannot mount CD!" ));
                } else {
                    mount_successful = true;
                    if (d->cbAutoDetectAtMount->isChecked()) {
                        // mount succeded, read media name
                        QString new_medianame = getCDName( cdrom_mountpath.toUtf8().constData());
                        if (!new_medianame.isEmpty()) {
                            if (*DEBUG_INFO_ENABLED) {
                                qDebug() << "new_medianame after mount: " << qPrintable( new_medianame );
                            }
                            d->dName = new_medianame;
                        }
                    }
                }
            }
        }
        delete []env;
        delete []arg;

        if (QFile( "/etc/mtab" ).exists()) {
            // check if already mounted
            QFile file( "/etc/mtab" );
            if (file.open( QIODevice::ReadOnly | QIODevice::Text )) {
                //	qDebug() << "/etc/mtab opened";

                QString line = file.readLine();
                while (line.length() > 0) {
                    line = file.readLine();
                    //	qDebug() << "mtab line: " << qPrintable(line);
                    QStringList mtablist = line.split( ' ' );
                    if (mtablist.size() > 3) {
                        QString mtab_devicepath = mtablist.at( 0 );
                        QString mtab_mountpath = mtablist.at( 1 );
                        //	qDebug() << "mtab device path: " << qPrintable(mtab_mountpath)<< " <=> " << qPrintable(mainw->cconfig->cdrompath);
                        if (mtab_mountpath == mainw->cconfig->cdrompath) {
                            if (*DEBUG_INFO_ENABLED) {
                                qDebug() << "mount path " << qPrintable( mtab_mountpath ) << " (" << qPrintable( mtab_devicepath ) << ") is already mounted, skipping mount";
                            }
                            break;
                        }
                        if (mtab_devicepath == mainw->cconfig->cdrompath) {
                            if (*DEBUG_INFO_ENABLED) {
                                qDebug() << "device path " << qPrintable( mtab_devicepath ) << " is already mounted (" << qPrintable( mtab_mountpath ) << "), skipping mount";
                            }
                            cdrom_mountpath = mtab_mountpath;
                            d->dDir = mtab_mountpath;
                            break;
                        }
                    } else {
                        //	qWarning() << "invalid mtab line: " << qPrintable(line) << ", skipping";
                    }
                }
                file.close();
                //	qDebug() << "/etc/mtab closed";
            } else {
                if (*DEBUG_INFO_ENABLED) {
                    qWarning() << "mtab could not openend";
                }
            }
        } else {
            if (*DEBUG_INFO_ENABLED) {
                qWarning() << "mtab could not found";
            }
        }
    } else {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "mount not needed";
        }
    }
#endif

#if !defined(_WIN32) && !defined(_OS2)
    if (((d->type == CD || d->type == DVD) && ((mainw->cconfig->mounteject && mount_successful) || (!mainw->cconfig->mounteject))) || (d->type != CD && d->type != DVD)) {
#endif
//              qDebug()<< "media type: " << d->type <<endl;
    if (d->type == CD || d->type == DVD) {
        if (mainw->cconfig->cdrompath != cdrom_mountpath) {
            d->setMediaName( mainw->cconfig->cdrompath );
        } else {
            d->setMediaName( cdrom_mountpath );
        }
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "media name for cd: " << qPrintable( d->leName->text());
        }
        if (d->leName->text().isEmpty()) {
            bool ok;
            QString text = QInputDialog::getText( 0, tr( "Enter media name..." ),
                                                  tr( "Media name:" ), QLineEdit::Normal,
                                                  tr( "Media" ), &ok );
            if (ok && !text.isEmpty()) {
                d->leName->setText( text );
                d->dName = text;
            } else {
                d->OK = 0;
                if (*DEBUG_INFO_ENABLED) {
                    qWarning() << "media name for cd empty";
                }
                }
            }
        }
        d->dName = d->leName->text();
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "ADDEVENT-2";
    }
    progress( pww );

    if (d->OK == 1) {
        panelsOFF();
        if (mainw->cconfig->showProgressedFileInStatus) {
            mainw->db->setShowProgressedFileInStatus( mainw->cconfig->showProgressedFileInStatus );
            mainw->db->setShowProgressedArchiveFileInStatus( mainw->cconfig->showProgressedArchiveFileInStatus );
            connect( mainw->db, SIGNAL( pathScanned( QString )), mainw, SLOT( pathScanned( QString )));
            connect( mainw->db, SIGNAL( pathExtraInfoAppend( QString )), mainw, SLOT( extraInfoAppend( QString )));
        }
        if (mainw->cconfig->displayCurrentScannedFileInTray) {
            connect( mainw->db, SIGNAL( fileScanned( QString )), mainw, SLOT( setTrayToolTipInfo( QString )));
            // connect ( mainw->db, SIGNAL ( fileScanned(QString)), mainw, SLOT ( setTrayText ( QString ) ) );
        }
        if (mainw->cconfig->showTrayIcon) {
            connect( mainw, SIGNAL( minimizedToTray()), pww, SLOT( hide()));
            connect( mainw, SIGNAL( restoredFromTray()), pww, SLOT( show()));
            mainw->trayIcon->setToolTip( tr( "Scanning directory tree, please wait..." ));
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 1 );
            mainw->startTrayIconAnim();
            mainw->trayIcon->showMessage( tr( "Scan started" ), tr( "Scanning %1 into %2 has been started" ).arg( d->dDir, d->dName ), icon, 3 * 1000 );
        }

        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "Scanning " << qPrintable( d->dDir ) << " into " << qPrintable( d->dName );
        }

        i = mainw->db->addMedia( d->dDir, d->dName, d->serial, d->type,
                                 (d->dOwner.isEmpty() ? QString( "" ) : d->dOwner), (d->dCategory.isEmpty() ? QString( "" ) : d->dCategory));

        if (*DEBUG_INFO_ENABLED) {
            qWarning() << "ret addMedia: " << i;
        }
        if (i == 2) {
            QMessageBox::warning( mainw,
                tr( "Warning..." ), tr( "You have cancelled catalog scanning,\nthe DataBase may be incomplete" ));
        }

        if (mainw->cconfig->showTrayIcon) {
            disconnect( mainw, SIGNAL( minimizedToTray()), pww, SLOT( hide()));
            disconnect( mainw, SIGNAL( restoredFromTray()), pww, SLOT( show()));
            if (i == 0) {
                QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 1 );
                mainw->trayIcon->showMessage( tr( "Scan finished" ), tr( "Scanning %1 into %2 has been finished" ).arg( d->dDir, d->dName ), icon, 3 * 1000 );
            } else {
                QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 2 );
                mainw->trayIcon->showMessage( tr( "Scan finished" ), tr( "Scanning %1 into %2 has been finished (NOT complete)" ).arg( d->dDir, d->dName ), icon, 3 * 1000 );
            }
            mainw->stopTrayIconAnim();
            mainw->trayIcon->setToolTip( tr( "Cdcat - idle" ));
        }

        if (mainw->cconfig->showProgressedFileInStatus) {
            disconnect( mainw->db, SIGNAL( pathScanned( QString )), mainw, SLOT( pathScanned( QString )));
            disconnect( mainw->db, SIGNAL( pathExtraInfoAppend( QString )), mainw, SLOT( extraInfoAppend( QString )));
        }

        if (mainw->cconfig->displayCurrentScannedFileInTray) {
            // disconnect ( mainw->db, SIGNAL ( fileScanned(QString)), mainw, SLOT ( setTrayText ( QString ) ) );
            disconnect( mainw->db, SIGNAL( fileScanned( QString )), mainw, SLOT( setTrayToolTipInfo( QString )));
        }

        // Do autosave if the user ask it in the config
        if (mainw->cconfig->autosave) {
            int retv = 0;
            if ((retv = mainw->db->saveDB()) != 0) {
                if (retv == 1) {                         // The database had no name. need a save as...
                    retv = saveasEvent();
                }
            }
        }
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "ADDEVENT-3";
        }
        panelsON();
        if (i != 0 && !pww->doCancel) {
            QMessageBox::warning( mainw,
                                  tr( "Warning..." ), tr( "An error occurred while scanning,\nthe DataBase may be incomplete" ));
        }

        if (!(d->dComm).isEmpty()) {
            Node *tn = (mainw->db->getRootNode())->child;
            while (tn->getNameOf() != d->dName)
                tn = tn->next;
            ((DBMedia *)(tn->data))->comment = d->dComm;
        }
    }
    progress( pww );
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "ADDEVENT-4";
    }

#if !defined(_WIN32) && !defined(_OS2)
    if (mainw->cconfig->mounteject) {
        if ((d->type == CD || d->type == DVD)) {
            d->dDir = mainw->cconfig->cdrompath;
            int pid;

            char **arg = new char *[3];
            char **env = new char *[2];

            // search the eject program:
            if (QFile( "/usr/local/bin/eject" ).exists()) {
                arg[0] = mstr( "/usr/local/bin/eject" );
            } else {
                if (QFile( "/usr/local/sbin/eject" ).exists()) {
                    arg[0] = mstr( "/usr/local/sbin/eject" );
                } else {
                    if (QFile( "/usr/bin/eject" ).exists()) {
                        arg[0] = mstr( "/usr/bin/eject" );
                    } else {
                        if (QFile( "/usr/sbin/eject" ).exists()) {
                            arg[0] = mstr( "/usr/sbin/eject" );
                        } else {
                            if (QFile( "/bin/eject" ).exists()) {
                                arg[0] = mstr( "/bin/eject" );
                            } else {
                                if (QFile( "/sbin/eject" ).exists()) {
                                    arg[0] = mstr( "/sbin/eject" );
                                } else {
                                    QMessageBox::warning
                                        ( d, tr( "Cannot eject CD" ), tr( "I can't find the \"eject\" program" ));
                                    arg[0] = mstr( "eject" );
                                }
                            }
                        }
                    }
                }
            }

            if (*DEBUG_INFO_ENABLED) {
                qDebug("Call: %s %s...", arg[0], (const char *)mainw->cconfig->cdrompath.toUtf8().constData());
            }
            arg[1] = mstr( cdrom_mountpath.toUtf8().constData());
            arg[2] = 0;
            env[0] = mstr( "PATH=/usr/local/bin:/usr/bin:/bin" );
            env[1] = 0;

            pid = fork();
            if (pid == 0) {                       // eject process
                if (execve( arg[0], (char *const *)arg, (char *const *)env ) == -1) {
                    exit( -1 );
                } else {
                    exit( 0 );                // That's never happend.
                }
            } else {                          // cdcat process
                int v;
                pww->refreshTime = 200;
                while (0 == waitpid( pid, &v, WNOHANG )) {
                    progress( pww );
                    usleep( 500 );
                }
                pww->refreshTime = 100;
                if (*DEBUG_INFO_ENABLED) {
                    qDebug() << "done";
                }
                if (WEXITSTATUS( v ) != 0) {
                    QMessageBox::warning( 0, tr( "Cannot eject CD!" ), tr( "Cannot eject CD!" ));
                }
            }
            delete []env;
            delete []arg;
        }
    } else {
        if (*DEBUG_INFO_ENABLED) {
            qDebug() << "umount not needed";
        }
    }
    // mount_successful
// }

    if ((d->type == CD || d->type == DVD) && mainw->cconfig->mounteject && !mount_successful) {
        if (*DEBUG_INFO_ENABLED) {
            qWarning() << "mount failed";
        }
        QMessageBox::warning( 0, tr( "Cannot mount CD!" ), tr( "Cannot mount CD!" ));
    }
#endif


    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "ADDEVENT-5";
    }
    pww->end();
    // qDebug() << "ADDEVENT-6";
    mainw->db->pww = NULL;
    // qDebug() << "ADDEVENT-7";
    pww->hide();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "ADDEVENT-8";
    }
    panelsON();
    QApplication::restoreOverrideCursor();
    return 0;
}

int GuiSlave::rescanEvent( void ) {
    QString o;
    QString rfd;

    if (mainw->db == NULL) {
        return 0;
    }

    mainw->db->setDoArchiveScan( mainw->cconfig->doScanArchive );
    mainw->db->storeMp3tags = mainw->cconfig->readtag;
    mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
    mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
    mainw->db->storeContent = mainw->cconfig->readcontent;
    mainw->db->storeExifData = mainw->cconfig->storeExifData;
    mainw->db->storeThumb = mainw->cconfig->storeThumb;
    mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
    mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
    mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
    mainw->db->storeLimit = mainw->cconfig->readclimit;
    mainw->db->storedFiles = mainw->cconfig->readcfiles;
    mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
    mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
    mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;
    mainw->db->displayCurrentScannedFileInTray = mainw->cconfig->displayCurrentScannedFileInTray;

    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected Media in the middle list box!" ));
        return 0;
    }
    if (standON->type != HC_MEDIA) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "You can refresh ONLY a MEDIA node!" ));
        return 0;
    }

    o = tr( "Rescan %1" ).arg( standON->getNameOf());
    rfd = QFileDialog::getExistingDirectory( 0, tr( "Select directory" ), mainw->cconfig->lastDir );
    if (rfd.isEmpty()) {
        return 0;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    PWw *pww = new PWw( mainw, mainw->app, false, 0, tr( "Scanning directory tree, please wait..." ), true );
    mainw->db->pww = pww;
    pww->setProgressText( tr( "Scanning directory tree, please wait..." ));
    pww->setCancel( true );
    progress( pww );

    panelsOFF();

    if (mainw->cconfig->showProgressedFileInStatus) {
        mainw->db->setShowProgressedFileInStatus( mainw->cconfig->showProgressedFileInStatus );
        mainw->db->setShowProgressedArchiveFileInStatus( mainw->cconfig->showProgressedArchiveFileInStatus );
        connect( mainw->db, SIGNAL( pathScanned( QString )), mainw, SLOT( pathScanned( QString )));
        connect( mainw->db, SIGNAL( pathExtraInfoAppend( QString )), mainw, SLOT( extraInfoAppend( QString )));
    }
    if (mainw->cconfig->showTrayIcon) {
        connect( mainw, SIGNAL( minimizedToTray()), pww, SLOT( hide()));
        connect( mainw, SIGNAL( restoredFromTray()), pww, SLOT( show()));
        mainw->trayIcon->setToolTip( tr( "Scanning directory tree, please wait..." ));
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 1 );
        mainw->startTrayIconAnim();
        mainw->trayIcon->showMessage( tr( "Scan started" ), tr( "Scanning %1 into %2 has been started" ).arg( rfd, "__rescanned__" ), icon, 3 * 1000 );
    }

    if (mainw->cconfig->displayCurrentScannedFileInTray) {
        // connect ( mainw->db, SIGNAL ( fileScanned(QString)), mainw, SLOT ( setTrayText ( QString ) ) );
        connect( mainw->db, SIGNAL( fileScanned( QString )), mainw, SLOT( setTrayToolTipInfo( QString )));
    }

    int rescan_ret = mainw->db->addMedia( rfd, "__rescanned__", -1, UNKNOWN, "system" );

    if (mainw->cconfig->showTrayIcon) {
        disconnect( mainw, SIGNAL( minimizedToTray()), pww, SLOT( hide()));
        disconnect( mainw, SIGNAL( restoredFromTray()), pww, SLOT( show()));
        if (rescan_ret == 0) {
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 1 );
            mainw->trayIcon->showMessage( tr( "Scan finished" ), tr( "Scanning %1 into %2 has been finished" ).arg( rfd, "__rescanned__" ), icon, 3 * 1000 );
        } else {
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon( 2 );
            mainw->trayIcon->showMessage( tr( "Scan finished" ), tr( "Scanning %1 into %2 has been finished (NOT complete)" ).arg( rfd, "__rescanned__" ), icon, 3 * 1000 );
        }
        mainw->stopTrayIconAnim();
        mainw->trayIcon->setToolTip( tr( "Cdcat - idle" ));
    }

    if (mainw->cconfig->showProgressedFileInStatus) {
        disconnect( mainw->db, SIGNAL( pathScanned( QString )), mainw, SLOT( pathScanned( QString )));
        disconnect( mainw->db, SIGNAL( pathExtraInfoAppend( QString )), mainw, SLOT( extraInfoAppend( QString )));
    }

    if (mainw->cconfig->displayCurrentScannedFileInTray) {
        // disconnect ( mainw->db, SIGNAL ( fileScanned(QString)), mainw, SLOT ( setTrayText ( QString ) ) );
        disconnect( mainw->db, SIGNAL( fileScanned( QString )), mainw, SLOT( setTrayToolTipInfo( QString )));
    }


    if (rescan_ret != 0) {
        Node *d;
        o = tr( "An error occurred while scanning, the rescan operation was cancelled: \n%1" )
            .arg( mainw->db->errormsg );

        d = (mainw->db->getRootNode())->child;
        while (d->getNameOf() != "__rescanned__")
            d = d->next;
        mainw->db->deleteNode( d );
        QMessageBox::warning( mainw, tr( "Error:" ), o );
    } else {
        Node *d;
        d = (mainw->db->getRootNode())->child;
        while (d->getNameOf() != "__rescanned__")
            d = d->next;

        ((DBMedia *)(d->data))->name = ((DBMedia *)(standON->data))->name;

        ((DBMedia *)(d->data))->comment = ((DBMedia *)(standON->data))->comment;

        ((DBMedia *)(d->data))->category = ((DBMedia *)(standON->data))->category;

        ((DBMedia *)(d->data))->owner = ((DBMedia *)(standON->data))->owner;

        ((DBMedia *)(d->data))->type = ((DBMedia *)(standON->data))->type;

        ((DBMedia *)(d->data))->number = ((DBMedia *)(standON->data))->number;

        mainw->db->deleteNode( standON );
    }


    standON = NULL;
    QApplication::restoreOverrideCursor();
    // Do autosave if the user ask it in the config !
    if (mainw->cconfig->autosave) {
        int retv = 0;
        if ((retv = mainw->db->saveDB()) != 0) {
            if (retv == 1) {             // The databas had no name. need a save as...
                retv = saveasEvent();
            }
        }
    }

    panelsON();

    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;

    return 0;
}

int GuiSlave::findEvent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    setGuiMenuAndToolBarEnabled( false );
    findDialog *d = new findDialog( mainw, "finddialog", false, false );
    d->exec();
    setGuiMenuAndToolBarEnabled();
    return 0;
}

int GuiSlave::configEvent( void ) {
    ConfigDialog *d = new ConfigDialog( mainw, "configdialog", true );

    d->exec();
    if (mainw->cconfig->showTrayIcon) {
        if (mainw->trayIcon == NULL) {
            mainw->createTrayIcon();
        }
        mainw->trayIcon->show();
        mainw->trayIcon->setToolTip( tr( "Cdcat - idle" ));
    } else {
        if (mainw->trayIcon != NULL) {
            mainw->trayIcon->hide();
        }
    }
    return 0;
}

int GuiSlave::aboutEvent( void ) {
    InfoDialog *di = new InfoDialog( mainw, "infodialog", true );

    di->exec();
    return 0;
}

int GuiSlave::keyBindingsEvent( void ) {
    KeyBindingDialog kb( this, mainw, "Key bindings", true );

    kb.exec();
    return 0;
}

int GuiSlave::aboutQtEvent( void ) {
    QApplication::aboutQt();

    return 0;
}

int GuiSlave::insertcEvent( void ) {
    char fnc[256];
    QString fn;

    if (mainw->db == NULL) {
        newEvent();
    }
    if (mainw->db == NULL) {
        return 0;
    }
    fn = QFileDialog::getOpenFileName( 0, tr( "Insert a database file..." ), mainw->cconfig->lastDir, tr( "CdCat databases (*.hcf )" ));
    if (fn.isEmpty()) {
        return 0;
    }

    strcpy( fnc, (const char *)QFile::encodeName( fn ));

    panelsOFF();

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    progress( pww );

    if (mainw->db->insertDB( fnc, false ) != 0) {        // An error occurred
        QMessageBox::warning( mainw, tr( "Error while opening..." ),
                              mainw->db->errormsg );
    }

    progress( pww );
    panelsON();
    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::insertCdcatXmlEvent( void ) {
    char fnc[256];
    QString fn;

    if (mainw->db == NULL) {
        newEvent();
    }
    if (mainw->db == NULL) {
        return 0;
    }
    fn = QFileDialog::getOpenFileName( 0, tr( "Insert a cdcat exported xml file..." ), mainw->cconfig->lastDir, tr( "CdCat xml export (*.xml )" ));
    if (fn.isEmpty()) {
        return 0;
    }

    strcpy( fnc, (const char *)QFile::encodeName( fn ));

    panelsOFF();

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    progress( pww );

    if (mainw->db->insertDB( fnc, false, true ) != 0) {        // An error occurred
        QMessageBox::warning( mainw, tr( "Error while opening..." ),
                              mainw->db->errormsg );
    }

    progress( pww );
    panelsON();
    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::insertcEventNoDup( void ) {
    char fnc[256];
    QString fn;

    if (mainw->db == NULL) {
        newEvent();
    }
    if (mainw->db == NULL) {
        return 0;
    }
    fn = QFileDialog::getOpenFileName( 0, tr( "Insert a database file..." ), mainw->cconfig->lastDir, tr( "CdCat databases (*.hcf )" ));
    if (fn.isEmpty()) {
        return 0;
    }

    strcpy( fnc, (const char *)QFile::encodeName( fn ));

    panelsOFF();

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    progress( pww );

    if (mainw->db->insertDB( fnc, true ) != 0) {        // An error occurred
        QMessageBox::warning( mainw, tr( "Error while opening..." ),
                              mainw->db->errormsg );
    }

    progress( pww );
    panelsON();
    progress( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::renameEvent( void ) {
    renamedialog *rd;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected item in the middle list box!" ));
        return 0;
    }
    rd = new renamedialog( mainw, standON, this );
    rd->exec();
    cHcaption();
    updateListFromNode();
    delete rd;
    return 0;
}

int GuiSlave::typeChangeEvent() {
    if (mainw->db == NULL) {
        return 0;
    }

    if (standON == NULL) {
        // QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected item in the middle list box!" ) );
        return 0;
    }

    if (context_item == NULL) {
        return 0;
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "standON: " << qPrintable( standON->getFullPath()) << ", nodetype: " << standON->type;
    }
    if (standON->type != HC_MEDIA) {
        return 0;
    }

    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "mediatype: " << ((DBMedia *)(standON->data))->type;
    }
    CatalogTypeEditDialog te( mainw, standON );
    te.exec();
    if (te.changeOk) {
        // ok

        // 1- name
        context_item->setText( 0, standON->getNameOf());

        // 2.(size) Column name:
        context_item->setText( 0, QString().setNum(((DBMedia *)(standON->data))->number ));

        // 3. Column name:
        switch (((DBMedia *)(standON->data))->type) {
        case UNKNOWN:
            context_item->setText( 1, tr( "Unknown(DB)" ));
            context_item->setIcon( 0, QIcon( *get_m_unknown_icon()));
            break;
        case CD:
            context_item->setText( 1, tr( "CD" ));
            context_item->setIcon( 0, QIcon( *get_m_cd_icon()));
            break;
        case DVD:
            context_item->setText( 1, tr( "DVD" ));
            context_item->setIcon( 0, QIcon( *get_m_dvd_icon()));
            break;
        case HARDDISC:
            context_item->setText( 1, tr( "HardDisc" ));
            context_item->setIcon( 0, QIcon( *get_m_hdd_icon()));
            break;
        case FLOPPY:
            context_item->setText( 1, tr( "Floppy" ));
            context_item->setIcon( 0, QIcon( *get_m_floppy_icon()));
            break;
        case NETPLACE:
            context_item->setText( 1, tr( "NetworkDrv" ));
            context_item->setIcon( 0, QIcon( *get_m_net_icon()));
            break;
        case FLASHDRV:
            context_item->setText( 1, tr( "FlashDrv" ));
            context_item->setIcon( 0, QIcon( *get_m_flash_icon()));
            break;
        case OTHERD:
            context_item->setText( 1, tr( "OtherDevice" ));
            context_item->setIcon( 0, QIcon( *get_m_other_icon()));
            break;
        }
        saveEvent();
    }

    return 0;
}

int GuiSlave::renumberEvent( void ) {
    renumberdialog *rd;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected item in the middle list box!" ));
        return 0;
    }

    if (standON->type != HC_MEDIA) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "It is NOT a media node!" ));
        return 0;
    }
    rd = new renumberdialog( mainw, standON, this );
    rd->exec();
    cHcaption();
    updateListFromNode();
    return 0;
}

int GuiSlave::sizeEvent( void ) {
    char text[52];
    QString qtext;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected item in the middle list box!" ));
        return 0;
    }


    double size = mainw->db->getSize( standON );
    double s = size;
    int st = UNIT_BYTE;


    if (size >= (double)SIZE_ONE_GBYTE * 1024.0) {
        s = size / SIZE_ONE_GBYTE / 1024.0;
        st = UNIT_TBYTE;
    } else {
        if (size >= (double)SIZE_ONE_GBYTE && size < (double)SIZE_ONE_GBYTE * 1024.0) {
            s = size / SIZE_ONE_GBYTE;
            st = UNIT_GBYTE;
        } else {
            if (size >= (double)SIZE_ONE_MBYTE && size < (double)SIZE_ONE_GBYTE) {
                s = size / SIZE_ONE_MBYTE;
                st = UNIT_MBYTE;
            } else {
                if (size >= (double)SIZE_ONE_KBYTE && size < (double)SIZE_ONE_MBYTE) {
                    s = size / SIZE_ONE_KBYTE;
                    st = UNIT_KBYTE;
                } else {
                    s = size;
                    st = UNIT_BYTE;
                }
            }
        }
    }
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "node " << qPrintable( standON->getFullPath()) << ", size raw: " << size << ", size: " << s << ", size type: " << st;
    }

    sprintf( text, "%.2f", s );
    qtext = tr( "The size of \"%1\" : \n %2 \n %3 file /%4 directory" )
            .arg( standON->getFullPath())
            .arg( QString( text ) + " " + getSType( st, true ))
            .arg( mainw->db->getCountFiles( standON ))
            .arg( mainw->db->getCountDirs( standON ));

    QMessageBox::information( mainw, tr( "The result:" ), qtext );
    return 0;
}

int GuiSlave::sortNuEventAscending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->DirView->setSortingEnabled( false );
    mainw->listView->setSortingEnabled( false );
    // mainw->DirView->sortByColumn(0, Qt::AscendingOrder);
    mainw->db->sortM( NUMBER, true );
    mainw->DirView->clear();
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortNuEventDescending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->DirView->setSortingEnabled( false );
    mainw->listView->setSortingEnabled( false );
    // mainw->DirView->sortByColumn(0, Qt::DescendingOrder);
    mainw->db->sortM( NUMBER, false );
    mainw->DirView->clear();
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortNaEventAscending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( NAME, true );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( true );
    mainw->listView->setSortingEnabled( true );
    mainw->DirView->sortByColumn( 0, Qt::AscendingOrder );
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortNaEventDescending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( NAME, false );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( true );
    mainw->listView->setSortingEnabled( true );
    mainw->DirView->sortByColumn( 0, Qt::DescendingOrder );
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortTyEventAscending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( TYPE, true );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( true );
    mainw->listView->setSortingEnabled( true );
    mainw->DirView->sortByColumn( 1, Qt::AscendingOrder );
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortTyEventDescending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( TYPE, false );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( true );
    mainw->listView->setSortingEnabled( true );
    mainw->DirView->sortByColumn( 1, Qt::DescendingOrder );
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortTiEventAscending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( TIME, true );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( false );
    mainw->listView->setSortingEnabled( false );
    // mainw->DirView->sortByColumn(0, Qt::AscendingOrder);
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::sortTiEventDescending( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    panelsOFF();
    mainw->db->sortM( TIME, false );
    mainw->DirView->clear();
    mainw->DirView->setSortingEnabled( false );
    mainw->listView->setSortingEnabled( false );
    // mainw->DirView->sortByColumn(0, Qt::DescendingOrder);
    mainw->DirView->start();
    panelsON();
    return 0;
}

int GuiSlave::helpEvent( void ) {
    QDialog dh;
    Ui_helpDialog *ui_dh = new  Ui_helpDialog();

    ui_dh->setupUi((&dh));

    dh.resize( 750, 550 );

    QString helptext = "";
    helptext += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
    helptext += "<html><head><meta name=\"qrichtext\" content=\"1\" />";
    helptext += "<style type=\"text/css\">; p, li { white-space: pre-wrap; }</style>";
    helptext += "</head>";
    helptext += "<body style=\" font-family:'DejaVu Sans'; font-size:9pt; font-weight:400; font-style:normal;\">";

    helptext += "<h1>" + tr( "Help" ) + "</h1>";

    helptext += "<h2>" + tr( "Whats this?" ) + "</h2>";
    helptext += " <p>";

    helptext += tr( "The cdcat is graphical (QT based) multiplatform (Linux/Windows) catalog program which scans the directories/drives you want and memorize the filesystem including the tags of mp3's and other data and store it in a small file. The database is stored in a gzipped XML format, so you can hack it, or use it if necessary :-).)" );

    helptext += tr( "The program can store the content of some specified files up to a limit size if you want. (for example: *.nfo)" );

    helptext += " </p>";
    helptext += "<h2>" + tr( "Usage:" ) + "</h2>";
    helptext += " <p>";

    helptext += tr( "Before the scanning select the necessary readable components in the config dialog, which can be mp3 tags content of some files or etc." );

    helptext += "</p>";

    helptext += "<h2>" + tr( "Create a new catalog" ) + "</h2>";
    helptext += "<p>";

    helptext += tr( "Run the %1 command in the catalog menu. You have to type the name of the new catalog. You can specify the default username of the media(which you scan later), and add a comment to the catalog." ).arg( "<i>" + tr( "New" ) + "</i>" );

    helptext += "</p>";

    helptext += "<h2>" + tr( "Add media" ) + "</h2>";

    helptext += "<p>";

    helptext += tr( "Run the %1 command in the edit menu. In that dialog you have to specify the directory/or drive the media you want to add. It's recommended to specify the name and the number of the media which has to be unique. (The program always generate one identical name and number). You can label the media to a owner, if necessary." ).arg( +"<i>" + tr( "Add media..." ) + "</i>" );


    helptext += " <br />";

    helptext += tr( "If you scanned your media, you will be able to browse in it with the browser window (like mc) , or search in it. You can save the catalog with %1 command in the file menu." ).arg( " <i>" + tr( "save as" ) + "</i>" );

    helptext += "</p>";

    helptext += "<h2>" + tr( "Open an existing catalog:" ) + "</h2>";

    helptext += tr( "Run the %1 command in the file menu, and choice the file of the catalog. (*.hcf). After the opening you will be able browse the catalog or search in it." ).arg( "<i>" + tr( "open" ) + "</i>" );

    helptext += "</p>";

    helptext += "<h2>" + tr( "Author:" ) + "</h2>";
    helptext += "<p>";

    helptext += tr( "The program was written by Peter Deak (hungary)" );

    helptext += "<br />";
    helptext += tr( "The current maintainer is %1." ).arg( MAINTAINER_NAME );

    helptext += "</p>";

    helptext += "<h2>" + tr( "License:" ) + "</h2>";

    helptext += "<p>";

    helptext += tr( "General Public License (GPL)" );

    helptext += "</p>";

    helptext += "<h2>" + tr( "Homepage:" ) + "</h2>";

    helptext += "<p>";

    helptext += tr( "You can read about the program and get new versions, sources etc, from the hompage of %1:" ).arg(PROGRAM_NAME) + " ";

    helptext += HOMEPAGE;
    helptext += "</p></body></html>";


    ui_dh->textBrowser1->setHtml( helptext );
    dh.exec();
    delete ui_dh;

//      getCDSerial("/dev/sr0");
    return 0;
}

int GuiSlave::exportEvent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    exportCdcatDB i( mainw );
    i.exec();
    return 0;
}

int GuiSlave::importEvent( void ) {
    import i( this );

    panelsON();
    return 0;
}

int GuiSlave::openHistoryElementEvent( QAction *action ) {
    char fnc[256];
    QString fn = action->text();

    strcpy( fnc, QFile::encodeName( fn ).constData());
    while (closeEvent() != 0) {
    }
    ;
    QApplication::setOverrideCursor( Qt::WaitCursor );
    panelsOFF();

    if (mainw->db == NULL) {
        mainw->db = new DataBase();
    }

    mainw->db->setDoArchiveScan( mainw->cconfig->doScanArchive );
    mainw->db->storeMp3tags = mainw->cconfig->readtag;
    mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
    mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
    mainw->db->storeContent = mainw->cconfig->readcontent;
    mainw->db->storeExifData = mainw->cconfig->storeExifData;
    mainw->db->storeThumb = mainw->cconfig->storeThumb;
    mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
    mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
    mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
    mainw->db->storeLimit = mainw->cconfig->readclimit;
    mainw->db->storedFiles = mainw->cconfig->readcfiles;
    mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
    mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
    mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;

    PWw *pww = new PWw( mainw, mainw->app );
    mainw->db->pww = pww;
    progress( pww );

    if (mainw->db->openDB( fnc ) != 0) {        // An error occurred
        QMessageBox::warning( mainw, tr( "Error while opening..." ),
                              mainw->db->errormsg );
        mainw->db = NULL;
        standON = NodePwd = NULL;;
        panelsOFF();
    }
    progress( pww );
    panelsON();
    progress( pww );

    // QMessageBox::information(0,"new history element",fn);
    if ((mainw->cconfig->hlist.filter( fn )).isEmpty()) {
        mainw->cconfig->hlist.append( fn );
        mainw->historyMenu->addAction( *get_p_icon(), fn );
    }

    if ((int)mainw->cconfig->hlist.count() > (int)mainw->cconfig->historysize) {
        mainw->cconfig->hlist.removeLast();
        QAction *last_action = mainw->historyMenu->actions().last();
        mainw->historyMenu->removeAction( last_action );
        delete last_action;
    }

    progress( pww );
    pww->end();
    if (mainw->db != NULL) {
        mainw->db->pww = NULL;
    }
    delete pww;
#ifdef CATALOG_ENCRYPTION
    if (mainw->db != NULL) {
        Node *root = mainw->db->getRootNode();
        bool isEncryptedCatalog = ((DBCatalog *)((root)->data))->isEncryptedCatalog;
        if (isEncryptedCatalog) {
            mainw->disableencryption_action->setEnabled( true );
            mainw->enableencryption_action->setEnabled( false );
        } else {
            mainw->disableencryption_action->setEnabled( false );
            mainw->enableencryption_action->setEnabled( true );
        }
    }
#endif
    QApplication::restoreOverrideCursor();
    return 0;
}

int GuiSlave::closeBranch( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    mainw->DirView->closeAllBranch();
    return 0;
}

int GuiSlave::editComment( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        return 0;
    }

    editNodeComment( standON, mainw, mainw->cconfig  );
    cHcaption();
    mainw->commentWidget->updateContents();
    return 0;
}

int GuiSlave::editCategory( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        return 0;
    }

    editNodeComment( standON, mainw, mainw->cconfig, false );
    cHcaption();
    mainw->commentWidget->updateContents();
    return 0;
}

int GuiSlave::showContent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    if (haveContent( standON )) {
        if (mainw->cconfig->useExternalContentViewer && QFileInfo( mainw->cconfig->ExternalContentViewerPath ).exists()) {
            QTemporaryFile tmpContentTempFile;
            tmpContentTempFile.setAutoRemove( false );
            if (!tmpContentTempFile.open()) {
                qWarning() << "Cant write temp file: " << qPrintable( tmpContentTempFile.fileName());
                return 1;
            }
            QString tmpFileName = tmpContentTempFile.fileName();
            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "GuiSlave::showContent tmpContentTempFile: " << qPrintable( tmpFileName );
            }
            Node *mynode = NULL;
            if (standON != NULL && standON->type == HC_FILE) {
                mynode = ((DBFile *)(standON->data))->prop;
                while (mynode != NULL) {
                    if (mynode->type == HC_CONTENT) {
                        break;
                    }
                    mynode = mynode->next;
                }
            }
            if (mynode != NULL) {
                tmpContentTempFile.write( QByteArray((const char *)((DBContent *)(mynode->data))->bytes ));
                tmpContentTempFile.close();
            } else {
                qWarning() << "Cant find node content: " << qPrintable( standON->getFullPath());
                return 1;
            }
            QProcess FileContentProcess;

            if (*DEBUG_INFO_ENABLED) {
                qDebug() << "GuiSlave::showContent args: " << qPrintable( mainw->cconfig->ExternalContentViewerPath ) << " " << qPrintable( tmpFileName );
            }
            FileContentProcess.startDetached( mainw->cconfig->ExternalContentViewerPath + " " + tmpFileName );
            if (!FileContentProcess.waitForStarted()) {
                return 1;
            }
            while (!FileContentProcess.waitForFinished() || FileContentProcess.state() == QProcess::Running) {
                if (mainw->app->hasPendingEvents()) {
                    mainw->app->processEvents();
                }
                usleep( 250 );
            }
            QByteArray result = FileContentProcess.readAll();
            // delete (( ( DBContent * ) ( mynode->data ) )->bytes);
            // QByteArray filedata(QFile(tmpFileName).readAll());
            // mynode->data = ( void * ) new DBContent ( (unsigned char*)filedata.constData(), filedata.length() );
            tmpContentTempFile.remove();
        } else {
            ShowContent *sc = new ShowContent( standON, false, mainw->cconfig, mainw, "showcw" );
            sc->exec();
            delete sc;
        }
    }
    cHcaption();
    return 0;
}

int GuiSlave::addlnkEvent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    PWw *pww = NULL;
    AddLnk al( this, mainw );
    al.exec();
    if (al.ok) {
        pww = new PWw( mainw, mainw->app );
        mainw->db->pww = pww;
        panelsOFF();
        mainw->db->addLnk((const char *)QFile::encodeName( al.fname->text()));
        panelsON();
    }
    delete pww;
    cHcaption();
    return 0;
}

int GuiSlave::followLnk( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    if (standON->type == HC_CATLNK) {
        char fnc[256];

        if (((DBCatLnk *)(standON->data))->location == NULL ||
            !strcmp(((DBCatLnk *)(standON->data))->location, "" )) {
            QMessageBox::warning( mainw, tr( "Error while opening..." ),
                                  tr( "The link is empty." ));

            return 0;
        }


        int ret_val = 0;


        strcpy( fnc, (const char *)((DBCatLnk *)(standON->data))->location );
        while (closeEvent() != 0) {
        }
        ;
        panelsOFF();

        if (mainw->db == NULL) {
            mainw->db = new DataBase();
        }
        mainw->db->setDoArchiveScan( mainw->cconfig->doScanArchive );
        mainw->db->storeMp3tags = mainw->cconfig->readtag;
        mainw->db->v1_over_v2 = mainw->cconfig->v1_over_v2;
        mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
        mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
        mainw->db->storeFileInfo = mainw->cconfig->usefileinfo;
        mainw->db->storeContent = mainw->cconfig->readcontent;
        mainw->db->storeExifData = mainw->cconfig->storeExifData;
        mainw->db->storeThumb = mainw->cconfig->storeThumb;
        mainw->db->ThumbExtsList = mainw->cconfig->ThumbExtsList;
        mainw->db->thumbWidth = mainw->cconfig->thumbWidth;
        mainw->db->thumbHeight = mainw->cconfig->thumbHeight;
        mainw->db->storeLimit = mainw->cconfig->readclimit;
        mainw->db->storedFiles = mainw->cconfig->readcfiles;
        mainw->db->doExcludeFiles = mainw->cconfig->doExcludeFiles;
        mainw->db->ExcludeFileList = mainw->cconfig->ExcludeFileList;
        mainw->db->useWildcardInsteadRegexForExclude = mainw->cconfig->useWildcardInsteadRegexForExclude;

        PWw *pww = new PWw( mainw, mainw->app );
        mainw->db->pww = pww;
        progress( pww );

        if ((ret_val = mainw->db->openDB( fnc )) != 0) {              // An error occurred
            QMessageBox::warning( mainw, tr( "Error while opening..." ),
                                  mainw->db->errormsg );
            mainw->db = NULL;
            standON = NodePwd = NULL;
            panelsOFF();
        }

        checkversion( mainw, mainw->db );

        progress( pww );
        panelsON();
        progress( pww );

        progress( pww );
        pww->end();
        if (mainw->db != NULL) {
            mainw->db->pww = NULL;
        }
        delete pww;
    }

    return 0;
}

int GuiSlave::borrowingEvent( void ) {
    borrowingDialog *d;

    if (mainw->db == NULL) {
        return 0;
    }

    d = new borrowingDialog( mainw->db, mainw, "borrowingdialog", true );
    d->exec();
    mainw->commentWidget->updateContents();
    cHcaption();
    return 0;
}

int GuiSlave::sborrowEvent( void ) {
    borrowDialog *d;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        return 0;
    }

    d = new borrowDialog( standON->getNameOf(), mainw, "borrowingdialog", true );
    d->exec();

    if (d->ok == 1) {
        if (!(((DBMedia *)(standON->data))->borrowing.isEmpty())) {
            ((DBMedia *)(standON->data))->borrowing = "";
        }
        ((DBMedia *)(standON->data))->borrowing = d->m;
        standON->touchDB();
    }
    mainw->commentWidget->updateContents();
    cHcaption();
    return 0;
}

int GuiSlave::cborrowEvent( void ) {
    int i;

    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        return 0;
    }

    i = QMessageBox::warning( mainw, tr( "Confirmation" ),
                              tr( "Do you want to clear the borrowing mark from media \"%1\"?\n(Say yes if you got it back.)" ).arg( standON->getNameOf())
                              , tr( "Yes" ), tr( "No" ));

    if (i != 0) {
        return 0;
    }
    if (!(((DBMedia *)(standON->data))->borrowing.isEmpty())) {
        ((DBMedia *)(standON->data))->borrowing = "";
    }
    standON->touchDB();
    mainw->commentWidget->updateContents();
    cHcaption();
    return 0;
}

int GuiSlave::searchDuplicatesEvent( void ) {
    if (mainw->db == NULL) {
        return 0;
    }
    if (standON == NULL) {
        QMessageBox::warning( mainw, tr( "Error:" ), tr( "There is no selected item in the middle list box!" ));
        return 0;
    }
    findDialog *finddupd = new findDialog( mainw, "finddialog", true, true );
    finddupd->exec();
    cHcaption();
    updateListFromNode();
    delete finddupd;
    return 0;
}

#ifdef CATALOG_ENCRYPTION
int GuiSlave::changePassEvent( void ) {
    bool ok = false;
    bool ok_again = false;
    QString Password = QInputDialog::getText( 0, QObject::tr( "Enter password..." ), QObject::tr( "Enter password for catalog:" ), QLineEdit::Password, "", &ok );
    QString PasswordAgain = QInputDialog::getText( 0, QObject::tr( "Enter password..." ), QObject::tr( "Enter password for catalog (again):" ), QLineEdit::Password, "", &ok_again );

    if (ok && ok_again) {
        if (Password == PasswordAgain) {
            if (Password.size() >= 4) {
                if (Password.size() > CryptoPP::Blowfish::BLOCKSIZE) {
                    QMessageBox::critical( 0, tr( "Password too big" ), tr( "Password length is too big, must be maximal %1 chars" ).arg( QString().setNum( CryptoPP::Blowfish::BLOCKSIZE )));
                } else {
                    generate_cryptokey( Password );
                    QMessageBox::information( 0, tr( "Password changed" ), tr( "Password has been successfully changed" ));
                    saveEvent();
                    return 0;
                }
            } else {
                QMessageBox::critical( 0, tr( "Password too short" ), tr( "Password length is too short, must be minimum 4 chars" ));
            }
        } else {
            QMessageBox::critical( 0, tr( "Passwords not match" ), tr( "Passwords does not match" ));
        }
    }
    return 1;
}

int GuiSlave::enableEncryptionEvent( void ) {
    Node *root = mainw->db->getRootNode();
    bool ok = false;
    bool ok_again = false;
    QString Password = QInputDialog::getText( 0, QObject::tr( "Enter password..." ), QObject::tr( "Enter password for catalog:" ), QLineEdit::Password, "", &ok );
    QString PasswordAgain = QInputDialog::getText( 0, QObject::tr( "Enter password..." ), QObject::tr( "Enter password for catalog (again):" ), QLineEdit::Password, "", &ok_again );

    if (ok && ok_again) {
        if (Password == PasswordAgain) {
            if (Password.size() >= 4) {
                if (Password.size() > CryptoPP::Blowfish::BLOCKSIZE) {
                    QMessageBox::critical( 0, tr( "Password too big" ), tr( "Password length is too big, must be maximal %1 chars" ).arg( QString().setNum( CryptoPP::Blowfish::BLOCKSIZE )));
                } else {
                    ((DBCatalog *)((root)->data))->isEncryptedCatalog = true;
                    generate_cryptokey( Password );
                    QMessageBox::information( 0, tr( "Password changed" ), tr( "Encryption has been successfully enabled" ));
                    saveEvent();
                    mainw->disableencryption_action->setEnabled( true );
                    mainw->enableencryption_action->setEnabled( false );
                    return 0;
                }
            } else {
                QMessageBox::critical( 0, tr( "Password too short" ), tr( "Password length is too short, must be minimum 4 chars" ));
            }
        } else {
            QMessageBox::critical( 0, tr( "Passwords not match" ), tr( "Passwords does not match" ));
        }
    }
    return 1;
}

int GuiSlave::disableEncryptionEvent( void ) {
    Node *root = mainw->db->getRootNode();

    ((DBCatalog *)((root)->data))->isEncryptedCatalog = false;
    QMessageBox::information( 0, tr( "Password changed" ), tr( "Encryption has been successfully disabled" ));
    saveEvent();
    mainw->disableencryption_action->setEnabled( false );
    mainw->enableencryption_action->setEnabled( true );
    return 1;
}

#endif

// *****************************************************************************
// Positioning dialog
// *****************************************************************************

QPosDialog::QPosDialog ( CdCatMainWidget *parent )
    : QDialog( parent, Qt::FramelessWindowHint ) {
    QPoint point;
    QHBoxLayout *l1;
    QVBoxLayout *l2;

    setModal( true );
    p = parent;
    le = new QLineEdit( this );

    l2 = new QVBoxLayout();
    l2->addSpacing( 2 );
    l2->addWidget( le );
    l2->addSpacing( 2 );

    l1 = new QHBoxLayout( this );
    l1->addSpacing( 2 );
    l1->addLayout( l2 );
    l1->addSpacing( 2 );

    point = p->splitMain->mapToGlobal(
        QPoint( p->listView->x() + 5, p->listView->y() + p->listView->height() - 40 ));

    move( point );

    maxp = point.y() + 40;
    minp = point.y() - 100;

    connect( le, SIGNAL( returnPressed()), this, SLOT( close()));
    connect( le, SIGNAL( textChanged( const QString & )), this, SLOT( pos( const QString & )));
    le->setFocus();
}

int QPosDialog::pos( const QString & str ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug();
    }
    QTreeWidgetItemIterator it( p->listView );

    while (*it) {
        if ((*it)->text( 0 ) == str) {
            p->listView->setCurrentItem((*it));
            p->listView->setCurrentVisible();
            p->guis->standOn((*it), 0 );
            return 0;
        }
        ++it;
    }
    return 0;
}

void QPosDialog::keyPressEvent( QKeyEvent *ke ) {
    int yp = y();

    if (ke->key() == Qt::Key_Escape ||
        ke->key() == Qt::Key_Return) {
        close();
    }

    if (ke->key() == Qt::Key_Up) {
        yp -= 5;
    }
    if (ke->key() == Qt::Key_Down) {
        yp += 5;
    }

    if (yp < minp) {
        yp = minp;
    }
    if (yp > maxp) {
        yp = maxp;
    }

    move( x(), yp );

    QDialog::keyPressEvent( ke );
}

int GuiSlave::posEvent( void ) {
    QPosDialog *d;

    d = new QPosDialog( mainw );
    d->exec();
    return 0;
}

int GuiSlave::colorEvent( void ) {
    ColorSettings *d;

    d = new ColorSettings( mainw->cconfig, mainw, "cs", true );
    d->exec();
    mainw->repaint();
    mainw->commentWidget->updateContents();
    return 0;
}

CatalogTypeEditDialog::CatalogTypeEditDialog ( CdCatMainWidget *parent, Node *n ) : QDialog( parent ) {
    this->n = n;
    changeOk = false;

    setSizeGripEnabled( true );
    layout1 = new QVBoxLayout( this );
    TextLabel = new QLabel( this );
    layout1->addWidget( TextLabel );
    cbType = new QComboBox( this );
    layout1->addWidget( cbType );

    buttonOK = new QPushButton( this );
    buttonOK->setMinimumWidth( 100 );
    layout1->addWidget( buttonOK );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumWidth( 100 );
    layout1->addWidget( buttonCancel );

    connect( buttonOK, SIGNAL( clicked()), this, SLOT( okExit()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( cancel()));
    connect( cbType, SIGNAL( activated( int )), this, SLOT( cbTypeToggeled( int )));
    languageChange();
    resize( QSize( 200, 250 ).expandedTo( minimumSizeHint()));
    cbType->setCurrentIndex(((DBMedia *)(n->data))->type - 1 );
}

void CatalogTypeEditDialog::languageChange() {
    setWindowTitle( tr( "Change media type" ));
    TextLabel->setText( tr( "Change type of media" ) + " " + this->n->getFullPath());
    cbType->clear();
    cbType->insertItem( CD, *get_m_cd_icon(), tr( "CD" ));
    cbType->insertItem( DVD, *get_m_dvd_icon(), tr( "DVD" ));
    cbType->insertItem( HARDDISC, *get_m_hdd_icon(), tr( "HardDisc" ));
    cbType->insertItem( FLOPPY, *get_m_floppy_icon(), tr( "Floppy" ));
    cbType->insertItem( NETPLACE, *get_m_net_icon(), tr( "NetworkPlace" ));
    cbType->insertItem( FLASHDRV, *get_m_flash_icon(), tr( "FlashDrive" ));
    cbType->insertItem( OTHERD, *get_m_other_icon(), tr( "OtherDevice" ));

    // cbType->setCurrentText(tr( "CD" )); // default
    buttonCancel->setText( tr( "Cancel" ));
    buttonOK->setText( tr( "OK" ));
}

void CatalogTypeEditDialog::okExit() {
    changeOk = true;
    qWarning() << "mediatype changed from " << ((DBMedia *)(n->data))->type << " to " << cbType->currentIndex() + 1;
    ((DBMedia *)(n->data))->type = cbType->currentIndex() + 1;
    qWarning() << "mediatype new: " << ((DBMedia *)(n->data))->type + 1;
    close();
}

void CatalogTypeEditDialog::cancel() {
    close();
}

void CatalogTypeEditDialog::cbTypeToggeled( int ) {
    qWarning() << "mediatype changed to " << cbType->currentIndex();
}




// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
