/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "hdirview.h"

#include "config.h"
#include "cdcat.h"
#include "dbase.h"
#include "mainwidget.h"
#include "guibase.h"
#include "icons.h"

#include <iostream>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QEvent>
#include <QPoint>
#include <QMessageBox>
#include <QStringList>
#include <QStringList>
#include <QApplication>
#include <QEvent>
#include <QHeaderView>
#include <QPainter>
#include <QPalette>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>

LNode::LNode ( LNode *parent, Node *dbnodep, const QString &col2 )
    : QTreeWidgetItem( parent, QTreeWidgetItem::UserType ), pix( 0 ) {
    node = dbnodep;
    setText( 0, col2 );
    childsCollected = false;
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
}

LNode::LNode ( QTreeWidget *parent, Node *dbnodep )
    : QTreeWidgetItem( parent, QTreeWidgetItem::UserType ),
    pix( 0 ) {
    node = dbnodep;
    p = 0;
    childsCollected = false;
    // setExpanded(true);
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
}

LNode::LNode ( LNode *parent, LNode *after, Node *dbnodep, const QString &col2 )
    : QTreeWidgetItem( parent, QTreeWidgetItem::UserType ), pix( 0 ) {
    node = dbnodep;
    setText( 0, col2 );
    childsCollected = false;
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
}





void LNode::setPixmap( QPixmap *px ) {
    pix = px;
    setup();
    // widthChanged ( 0 ); //FIXME
    // invalidateHeight(); //FIXME
    // repaint(); //FIXME
}

const QPixmap *LNode::pixmap( int i ) const {
    if (i) {
        return 0;
    }
    return pix;
}

void LNode::setExpanded( bool o ) {
    LNode *last = NULL;

    switch (node->type) {
    case HC_CATALOG:
        setIcon( 0, QIcon( *get_p_icon()));
        break;
    case HC_MEDIA:
        switch (((DBMedia *)(node->data))->type) {
        case UNKNOWN:
            setIcon( 0, QIcon( *get_m_unknown_icon()));
            break;
        case CD:
            setIcon( 0, QIcon( *get_m_cd_icon()));
            break;
        case DVD:
            setIcon( 0, QIcon( *get_m_dvd_icon()));
            break;
        case HARDDISC:
            setIcon( 0, QIcon( *get_m_hdd_icon()));
            break;
        case FLOPPY:
            setIcon( 0, QIcon( *get_m_floppy_icon()));
            break;
        case NETPLACE:
            setIcon( 0, QIcon( *get_m_net_icon()));
            break;
        case FLASHDRV:
            setIcon( 0, QIcon( *get_m_flash_icon()));
            break;
        case OTHERD:
            setIcon( 0, QIcon( *get_m_other_icon()));
            break;
        }
        break;
    case HC_DIRECTORY:
        if (o) {
            setIcon( 0, QIcon( *get_v_folderopen_icon()));
        } else {
            setIcon( 0, QIcon( *get_v_folderclosed_icon()));
        }
        break;
    }

    if (o && !childsCollected) {
        Node *tmp = node->child;
        if (tmp != NULL) {
            setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
        }
        while (tmp != NULL) {
            LNode *newnode;
            /*We don't show the elements under directory!*/
            if (tmp->type == HC_FILE || tmp->type == HC_CATLNK) {
                tmp = tmp->next;
                continue;
            }

            newnode = new LNode( this, last, tmp, tmp->getNameOf());
            last = newnode;

            switch (tmp->type) {
            case HC_CATALOG:
                newnode->setIcon( 0, QIcon( *get_p_icon()));
                newnode->setText( 1, QObject::tr( "Catalog" ));
                if (((DBCatalog *)((tmp)->data))->isEncryptedCatalog) {
                    newnode->setText( 1, QObject::tr( "Catalog (encrypted)" ));
                }
                break;
            case HC_MEDIA:
                switch (((DBMedia *)(tmp->data))->type) {
                case UNKNOWN:
                    newnode->setIcon( 0, QIcon( *get_m_unknown_icon()));
                    newnode->setText( 1, QObject::tr( "Unknown(DB)" ));
                    break;
                case CD:
                    newnode->setIcon( 0, QIcon( *get_m_cd_icon()));
                    newnode->setText( 1, QObject::tr( "CD" ));
                    break;
                case DVD:
                    newnode->setIcon( 0, QIcon( *get_m_dvd_icon()));
                    newnode->setText( 1, QObject::tr( "DVD" ));
                    break;
                case HARDDISC:
                    newnode->setIcon( 0, QIcon( *get_m_hdd_icon()));
                    newnode->setText( 1, QObject::tr( "HardDisc" ));
                    break;
                case FLOPPY:
                    newnode->setIcon( 0, QIcon( *get_m_floppy_icon()));
                    newnode->setText( 1, QObject::tr( "Floppy" ));
                    break;
                case NETPLACE:
                    newnode->setIcon( 0, QIcon( *get_m_net_icon()));
                    newnode->setText( 1, QObject::tr( "NetworkDrv" ));
                    break;
                case FLASHDRV:
                    newnode->setIcon( 0, QIcon( *get_m_flash_icon()));
                    newnode->setText( 1, QObject::tr( "FlashDrv" ));
                    break;
                case OTHERD:
                    newnode->setIcon( 0, QIcon( *get_m_other_icon()));
                    newnode->setText( 1, QObject::tr( "OtherDevice" ));
                    break;
                }
                break;
            case HC_DIRECTORY:
                newnode->setIcon( 0, QIcon( *get_v_folderclosed_icon()));
                newnode->setText( 1, QObject::tr( "Directory" ));
                break;
            }

            tmp = tmp->next;
        }
        childsCollected = true;
        treeWidget()->setUpdatesEnabled( true );          // FIXME
    }
    QTreeWidgetItem::setExpanded( o );
    treeWidget()->resizeColumnToContents( 0 );
}


void LNode::setup() {
    // setExpandable ( true ); //FIXME
    // QTreeWidgetItem::setup(); // FIXME
}


QString LNode::fullName() {
    Node *tmp = node;
    QString s( "" );

    while (tmp) {
        s.prepend( "/" );
        s.prepend( tmp->getNameOf());
        tmp = tmp->parent;
    }
    return s;
}


QString LNode::text( int column ) const {
    if (column == 0) {
        return node->getNameOf();
    } else {
        switch (node->type) {
        case HC_CATALOG:
            if (((DBCatalog *)((node)->data))->isEncryptedCatalog) {
                return QObject::tr( "Catalog (encrypted)" );
            } else {
                return QObject::tr( "Catalog" );
            }
        case HC_MEDIA:
            switch (((DBMedia *)(node->data))->type) {
            case UNKNOWN:
                return QObject::tr( "Unknown(DB)" );
            case CD:
                return QObject::tr( "CD" );
            case DVD:
                return QObject::tr( "DVD" );
            case HARDDISC:
                return QObject::tr( "HardDisc" );
            case FLOPPY:
                return QObject::tr( "Floppy" );
            case NETPLACE:
                return QObject::tr( "NetworkDrv" );
            case FLASHDRV:
                return QObject::tr( "FlashDrv" );
            case OTHERD:
                return QObject::tr( "OtherDevice" );
            }
            break;
        case HC_DIRECTORY:
            return QObject::tr( "Directory" );
        }
    }
    return "";
}

/*****************************************************************************
*
* Class HDirectoryView
*
*****************************************************************************/
void HDirectoryView::start( void ) {
    LNode *n;

    if ((*db) != NULL) {
        n = new LNode( this, (*db)->getRootNode());
        n->setPixmap( get_p_icon());
        n->setText( 0, (*db)->getRootNode()->getNameOf());
        n->setText( 1, n->text( 1 ));
        n->setup();
        switch (((DBCatalog *)n)->sortedBy) {
        case NUMBER:
            setSortingEnabled( false );
            break;
        case NAME:
            setSortingEnabled( true );
            sortByColumn( 0, Qt::AscendingOrder );
            break;
        case TYPE:
            setSortingEnabled( false );
            break;
        case TIME:
            setSortingEnabled( false );
            break;
        }
        n->setExpanded( true );
    }
}

HDirectoryView::HDirectoryView ( DataBase **dbp, QWidget *parent, const char *name )
    : QTreeWidget( parent ), oldCurrent( 0 ),
    dropItem( 0 ), mousePressed( false ) {
    db = dbp;
    autoopen_timer = new QTimer( this );

    // setShowSortIndicator ( true ); //FIXME
    connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int )), this, SLOT( slotFolderSelected( QTreeWidgetItem *, int )));
    // connect ( this, SIGNAL ( itemPressed ( QTreeWidgetItem * , int ) ),
    //          this, SLOT ( slotFolderSelectedR ( QTreeWidgetItem *, int ) ) );

    connect( this, SIGNAL( itemClicked( QTreeWidgetItem *, int )), this, SLOT( slotFolderSelectedR( QTreeWidgetItem *, int )));

    connect( this, SIGNAL( itemExpanded( QTreeWidgetItem * )), this, SLOT( itemExpanded( QTreeWidgetItem * )));
    connect( this, SIGNAL( itemCollapsed( QTreeWidgetItem * )), this, SLOT( itemCollapsed( QTreeWidgetItem * )));

    setAcceptDrops( true );
    viewport()->setAcceptDrops( true );

//      connect ( autoopen_timer, SIGNAL ( timeout() ),
//                this, SLOT ( openFolder() ) );

    // setSorting ( -1 );
    setSortingEnabled( true );
    sortByColumn( 0, Qt::AscendingOrder );
    start();

    QStringList labels;
    labels.append( tr( "Directory Tree" ));
    labels.append( tr( "Type" ));
    setHeaderLabels( labels );
    // header()->setClickEnabled ( false, header()->count() - 1 ); // FIXME
    // header()->setResizeEnabled ( false, header()->count() - 1 ); //FIXME

    // header()->setResizeMode(0,QHeaderView::Stretch);
    // header()->setResizeMode(1,QHeaderView::Stretch);
    // setSelectionMode ( Single );
    setSelectionMode( QAbstractItemView::SingleSelection );
    setItemsExpandable( true );
    setContextMenuPolicy( Qt::CustomContextMenu );
}

HDirectoryView::~HDirectoryView() {
    delete autoopen_timer;
}


void HDirectoryView::slotFolderSelected( QTreeWidgetItem *i, int col ) {
    if (!i) {
        return;
    }
    if (i == NULL) {
        return;
    }
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "HDirectoryView::slotFolderSelected item path:" << qPrintable(((LNode *)i)->fullName());
    }

    emit folderSelected(((LNode *)i)->fullName());
}

void HDirectoryView::slotFolderSelectedR( QTreeWidgetItem *i, int col ) {
    if (i == NULL) {
        return;
    }
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        qDebug() << "HDirectoryView::slotFolderSelectedR item path:" << qPrintable(((LNode *)i)->fullName());
    }

    LNode *lnode = (LNode *)i;
    if (*DEBUG_INFO_ENABLED) {
        // qDebug() << lnode->fullName();
    }
    if (lnode->isExpanded()) {
        lnode->setExpanded( false );
    } else {
        lnode->setExpanded( true );
    }
    emit folderSelected(((LNode *)i)->fullName());
    // i->setSelected(true);
}

// void HDirectoryView::openFolder() {
//      autoopen_timer->stop();
//      if ( dropItem ) {
//              if( !dropItem->isExpanded() ) {
//              dropItem->setExpanded ( true );
//              //dropItem->repaint();
//              } else {
//              dropItem->setExpanded ( false );
//              //dropItem->repaint();
//              }
//      }
//      else {
//              itemAt(0,0)->setExpanded(true);
//      }
// }

void HDirectoryView::itemExpanded( QTreeWidgetItem *item ) {
    ((LNode *)item)->setExpanded( true );
}

void HDirectoryView::itemCollapsed( QTreeWidgetItem *item ) {
    ((LNode *)item)->setExpanded( false );
}


static const int autoopenTime = 750;



QString HDirectoryView::fullPath( QTreeWidgetItem *item ) {
    QString fullpath = item->text( 0 );

    while ((item = item->parent())) {
        if (item->parent()) {
            fullpath = item->text( 0 ) + "/" + fullpath;
        } else {
            fullpath = item->text( 0 ) + fullpath;
        }
    }
    return fullpath;
}

void HDirectoryView::contentsMousePressEvent( QMouseEvent *e ) {
    // QTreeWidget::contentsMousePressEvent ( e ); // FIXME
//      QPoint p ( e->pos() ) ;
//      QTreeWidgetItem *i = itemAt ( p );
//     if ( i ) {
//         // if the user clicked into the root decoration of the item, don't try to start a drag!
//         if ( p.x() > header()->pos ( )  +
//                 indentation() * ( i->depth() + ( rootIsDecorated() ? 1 : 0 ) ) + itemMargin() ||
//                 p.x() < header()->pos ( ))  {
//             presspos = e->pos();
//             mousePressed = true;
//         }
//     } // FIXME
}


void HDirectoryView::contentsMouseReleaseEvent( QMouseEvent * ) {
    mousePressed = false;
}

void HDirectoryView::keyPressEvent( QKeyEvent *e ) {
    emit hitkey( e );
    QTreeWidget::keyPressEvent( e );
}

void HDirectoryView::setDir( Node *node ) {
    QString part, s((node->getFullPath()).remove( 0, 2 ));

    if (!s.isEmpty()) {
        /*Jump to …*/
        QStringList lst = s.split( "/" );
        QTreeWidgetItemIterator it( this );

        QStringList::Iterator it2 = lst.begin();
        for (; it2 != lst.end(); ++it2) {
            while ((*it)) {
                if ((*it)->text( 0 ) == *it2) {
                    (*it)->setExpanded( true );
                    break;
                }
                ++it;
            }
        }

        if ((*it)) {
            setCurrentItem((*it));
            setAllColumnsShowFocus( true );
            // ensureItemVisible ( (*it) ); //FIXME
        }
        /*Jump to …*/
    }
}

void HDirectoryView::closeAllBranch( void ) {
    QTreeWidgetItemIterator it( this );

    while ((*it)) {
        (*it)->setExpanded( false );
        ++it;
    }
}
