/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "hdirview.h"

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <q3dragobject.h>
#include <qmime.h>
#include <QStringList>
#include <qstringlist.h>
#include <qapplication.h>
#include <qevent.h>
#include <q3header.h>
#include <qpainter.h>
#include <qpalette.h>
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>

#include <iostream>
using namespace std;

#include "config.h"
#include "cdcat.h"
#include "dbase.h"
#include "mainwidget.h"
#include "guibase.h"
#include "icons.h"



LNode::LNode ( LNode * parent, Node *dbnodep )
        : Q3ListViewItem ( parent ),
        pix ( 0 ) {
    p = parent;
    node=dbnodep;

    setPixmap ( get_v_folderclosed_icon() );
}


LNode::LNode ( Q3ListView * parent, Node *dbnodep )
        : Q3ListViewItem ( parent ),
        pix ( 0 ) {
    node = dbnodep;
    p = 0;
}

void LNode::paintCell ( QPainter *p,const QColorGroup & cg,int column,int width,int align ) {
    QColorGroup ocg ( cg );
    ocg.setColor ( QColorGroup::Highlight,QColor ( 225,225,225 ) );
    ocg.setColor ( QColorGroup::HighlightedText,QColor ( Qt::black ) );
    Q3ListViewItem::paintCell ( p,ocg,column,width,align );
}

void LNode::setPixmap ( QPixmap *px ) {
    pix = px;
    setup();
    widthChanged ( 0 );
    invalidateHeight();
    repaint();
}

const QPixmap *LNode::pixmap ( int i ) const {
    if ( i )
        return 0;
    return pix;
}

void LNode::setOpen ( bool o ) {
    LNode  *last=NULL;
    switch ( node->type ) {
    case HC_CATALOG : setPixmap ( get_p_icon() ); break;
    case HC_MEDIA:
        switch ( ( ( DBMedia* ) ( node->data ) )->type ) {
        case UNKNOWN : setPixmap ( get_m_unknown_icon() ); break;
        case CD      : setPixmap ( get_m_cd_icon() ); break;
        case DVD     : setPixmap ( get_m_dvd_icon() ); break;
        case HARDDISC: setPixmap ( get_m_hdd_icon() ); break;
        case FLOPPY  : setPixmap ( get_m_floppy_icon() ); break;
        case NETPLACE: setPixmap ( get_m_net_icon() ); break;
        case FLASHDRV: setPixmap ( get_m_flash_icon() ); break;
        case OTHERD  : setPixmap ( get_m_other_icon() ); break;
        }
        break;
    case HC_DIRECTORY:
        if ( o )
            setPixmap ( get_v_folderopen_icon() );
        else
            setPixmap ( get_v_folderclosed_icon() );
        break;
    }


    if ( o && !childCount() ) {

        Node *tmp=node->child;

        while ( tmp != NULL ) {
            LNode *newnode;
            /*We don't show the elements under directory!*/
            if ( tmp->type == HC_FILE || tmp->type == HC_CATLNK ) {
                tmp=tmp->next;
                continue;
            }

            newnode = new LNode ( this,last,tmp,tmp->getNameOf() );
            last    = newnode;


            switch ( tmp->type ) {
            case HC_CATALOG : newnode->setPixmap ( get_p_icon() ); break;
            case HC_MEDIA :
                switch ( ( ( DBMedia* ) ( tmp->data ) )->type ) {
                case UNKNOWN : newnode->setPixmap ( get_m_unknown_icon() ); break;
                case CD      : newnode->setPixmap ( get_m_cd_icon() ); break;
                case DVD     : newnode->setPixmap ( get_m_dvd_icon() ); break;
                case HARDDISC: newnode->setPixmap ( get_m_hdd_icon() ); break;
                case FLOPPY  : newnode->setPixmap ( get_m_floppy_icon() ); break;
                case NETPLACE: newnode->setPixmap ( get_m_net_icon() ); break;
                case FLASHDRV: newnode->setPixmap ( get_m_flash_icon() ); break;
                case OTHERD  : newnode->setPixmap ( get_m_other_icon() ); break;
                }
                break;
            case HC_DIRECTORY:
                newnode->setPixmap ( get_v_folderclosed_icon() );
                break;
            }
            tmp=tmp->next;
        }
        listView()->setUpdatesEnabled ( TRUE );
    }
    Q3ListViewItem::setOpen ( o );
}


void LNode::setup() {
    setExpandable ( TRUE );
    Q3ListViewItem::setup();
}


QString LNode::fullName() {
    Node *tmp=node;
    QString s ( "" );
    while ( tmp ) {
        s.prepend ( "/" );
        s.prepend ( tmp->getNameOf() );
        tmp=tmp->parent;
    }
    return s;
}


QString LNode::text ( int column ) const {
    if ( column == 0 )
        return node->getNameOf();
    else {
        switch ( node->type ) {
        case HC_CATALOG : return QObject::tr ( "Catalog" );
        case HC_MEDIA   :
            switch ( ( ( DBMedia* ) ( node->data ) )->type ) {
            case UNKNOWN : return QObject::tr ( "Unknown(DB)" );
            case CD      : return QObject::tr ( "CD" );
            case DVD     : return QObject::tr ( "DVD" );
            case HARDDISC: return QObject::tr ( "HardDisc" );
            case FLOPPY  : return QObject::tr ( "Floppy" );
            case NETPLACE: return QObject::tr ( "NetworkDrv" );
            case FLASHDRV: return QObject::tr ( "FlashDrv" );
            case OTHERD  : return QObject::tr ( "OtherDevice" );
            }
            break;
        case HC_DIRECTORY: return QObject::tr ( "Directory" );
        }
    }
    return "";
}

/*****************************************************************************
 *
 * Class HDirectoryView
 *
 *****************************************************************************/

void HDirectoryView::start ( void ) {
    LNode *n;

    if ( ( *db ) != NULL ) {
        n=new LNode ( this, ( *db )->getRootNode() );

        n->setPixmap ( get_p_icon() );
        n->setup();
        n->setOpen ( true );
    }
}

HDirectoryView::HDirectoryView ( DataBase** dbp, QWidget *parent, const char *name )
        : Q3ListView ( parent, name ), oldCurrent ( 0 ),
        dropItem ( 0 ), mousePressed ( FALSE ) {
    db = dbp;
    autoopen_timer = new QTimer ( this );

    setShowSortIndicator ( true );
    connect ( this, SIGNAL ( doubleClicked ( Q3ListViewItem * ) ),
              this, SLOT ( slotFolderSelected ( Q3ListViewItem * ) ) );

    connect ( this, SIGNAL ( pressed ( Q3ListViewItem * ) ),
              this, SLOT ( slotFolderSelectedR ( Q3ListViewItem * ) ) );

    connect ( this, SIGNAL ( returnPressed ( Q3ListViewItem * ) ),
              this, SLOT ( slotFolderSelectedR ( Q3ListViewItem * ) ) );

    setAcceptDrops ( TRUE );
    viewport()->setAcceptDrops ( TRUE );

    connect ( autoopen_timer, SIGNAL ( timeout() ),
              this, SLOT ( openFolder() ) );

    //setSorting ( -1 );
    start();

    addColumn ( tr ( "Directory Tree" ) );
    addColumn ( tr ( "Type" ) );
    header()->setClickEnabled ( FALSE, header()->count() - 1 );
    header()->setResizeEnabled ( FALSE, header()->count() - 1 );

    setSelectionMode ( Single );
}

void HDirectoryView::slotFolderSelected ( Q3ListViewItem *i ) {
    if ( !i )	return;
    if ( i==NULL ) return;
    DEBUG_INFO_ENABLED = init_debug_info();
    LNode *lnode = ( LNode* ) i;
    if(*DEBUG_INFO_ENABLED)
	cerr << "HDirectoryView::slotFolderSelected node: " << qPrintable ( lnode->fullName() ) << endl;
    emit folderSelected ( lnode->fullName() );
}



void HDirectoryView::slotFolderSelectedR ( Q3ListViewItem *i ) {
    if ( i==NULL ) return;
    DEBUG_INFO_ENABLED = init_debug_info();
    LNode *lnode = ( LNode* ) i;
    if(*DEBUG_INFO_ENABLED) {
	//cerr << lnode->fullName()<<endl;
	cerr << "HDirectoryView::slotFolderSelectedR node: " << qPrintable ( lnode->fullName() ) << endl;
    }
    emit folderSelected ( lnode->fullName() );
    //i->setSelected(true);
}

void HDirectoryView::openFolder() {
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
        dropItem->setOpen ( TRUE );
        dropItem->repaint();
    } else {
        dropItem->setOpen ( FALSE );
        dropItem->repaint();
    }
}

static const int autoopenTime = 750;


void HDirectoryView::contentsDragEnterEvent ( QDragEnterEvent *e ) {
    if ( !Q3UriDrag::canDecode ( e ) ) {
        e->ignore();
        return;
    }

    oldCurrent = currentItem();

    Q3ListViewItem *i = itemAt ( contentsToViewport ( e->pos() ) );
    if ( i ) {
        dropItem = i;
        autoopen_timer->start ( autoopenTime );
    }
}


void HDirectoryView::contentsDragMoveEvent ( QDragMoveEvent *e ) {
    if ( !Q3UriDrag::canDecode ( e ) ) {
        e->ignore();
        return;
    }

    QPoint vp = contentsToViewport ( ( ( QDragMoveEvent* ) e )->pos() );
    Q3ListViewItem *i = itemAt ( vp );
    if ( i ) {
        setSelected ( i, TRUE );
        e->accept();
        if ( i != dropItem ) {
            autoopen_timer->stop();
            dropItem = i;
            autoopen_timer->start ( autoopenTime );
        }
        switch ( e->action() ) {
        case QDropEvent::Copy:
            break;
        case QDropEvent::Move:
            e->acceptAction();
            break;
        case QDropEvent::Link:
            e->acceptAction();
            break;
        default:
            ;
        }
    } else {
        e->ignore();
        autoopen_timer->stop();
        dropItem = 0;
    }
}

void HDirectoryView::contentsDragLeaveEvent ( QDragLeaveEvent * ) {
    autoopen_timer->stop();
    dropItem = 0;

    setCurrentItem ( oldCurrent );
    setSelected ( oldCurrent, TRUE );
}

void HDirectoryView::contentsDropEvent ( QDropEvent *e ) {
    autoopen_timer->stop();

    if ( !Q3UriDrag::canDecode ( e ) ) {
        e->ignore();
        return;
    }

    Q3ListViewItem *item = itemAt ( contentsToViewport ( e->pos() ) );
    if ( item ) {

        QStringList lst;

        Q3UriDrag::decodeToUnicodeUris ( e, lst );

        QString str;

        switch ( e->action() ) {
        case QDropEvent::Copy:
            str = tr ( "Copy" );
            break;
        case QDropEvent::Move:
            str = tr ( "Move" );
            e->acceptAction();
            break;
        case QDropEvent::Link:
            str = tr ( "Link" );
            e->acceptAction();
            break;
        default:
            str = tr ( "Unknown" );
        }

        str += "\n\n";

        e->accept();

        for ( uint i = 0; i < lst.count(); ++i ) {
            QString filename = lst.at ( i );
            str += filename + "\n";
        }
        str += QString ( "\nTo\n\n   %1" )
               .arg ( fullPath ( item ) );

        QMessageBox::information ( this, tr ( "Drop target" ), str, tr ( "Not implemented" ) );
    } else
        e->ignore();

}


QString HDirectoryView::fullPath ( Q3ListViewItem* item ) {
    QString fullpath = item->text ( 0 );
    while ( ( item=item->parent() ) ) {
        if ( item->parent() )
            fullpath = item->text ( 0 ) + "/" + fullpath;
        else
            fullpath = item->text ( 0 ) + fullpath;
    }
    return fullpath;
}

void HDirectoryView::contentsMousePressEvent ( QMouseEvent* e ) {
    Q3ListView::contentsMousePressEvent ( e );
    QPoint p ( contentsToViewport ( e->pos() ) );
    Q3ListViewItem *i = itemAt ( p );
    if ( i ) {
        // if the user clicked into the root decoration of the item, don't try to start a drag!
        if ( p.x() > header()->cellPos ( header()->mapToActual ( 0 ) ) +
                treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0 ) ) + itemMargin() ||
                p.x() < header()->cellPos ( header()->mapToActual ( 0 ) ) ) {
            presspos = e->pos();
            mousePressed = TRUE;
        }
    }
}

void HDirectoryView::contentsMouseMoveEvent ( QMouseEvent* e ) {
    if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) {
        mousePressed = FALSE;
        Q3ListViewItem *item = itemAt ( contentsToViewport ( presspos ) );
        if ( item ) {
            QString source = fullPath ( item );
            if ( QFile::exists ( source ) ) {
                Q3UriDrag* ud = new Q3UriDrag ( viewport() );
                ud->setUnicodeUris ( QStringList ( source ) );
                if ( ud->drag() )
                    QMessageBox::information ( this, tr ( "Drag source" ),
                                               QString ( tr ( "Delete " ) ) +source, tr ( "Not implemented" ) );
            }
        }
    }
}

void HDirectoryView::contentsMouseReleaseEvent ( QMouseEvent * ) {
    mousePressed = FALSE;
}

void HDirectoryView::keyPressEvent ( QKeyEvent *e ) {
    emit hitkey ( e );
    Q3ListView::keyPressEvent ( e );
}

void HDirectoryView::setDir ( Node * node ) {
    QString part,s ( ( node->getFullPath() ).remove ( 0,2 ) );

    if ( !s.isEmpty() ) {
        /*Jump to...*/
        QStringList lst ( QStringList::split ( "/", s ) );
        Q3ListViewItem *item = firstChild();
        QStringList::Iterator it2 = lst.begin();
        for ( ;it2 != lst.end(); ++it2 ) {
            while ( item ) {
                if ( item->text ( 0 ) == *it2 ) {
                    item->setOpen ( TRUE );
                    break;
                }
                item = item->itemBelow();
            }
        }
        if ( item ) {
            setCurrentItem ( item );
            setAllColumnsShowFocus ( true );
            ensureItemVisible ( item );
        }
        /*Jump to...*/
    }
}

void HDirectoryView::closeAllBranch ( void ) {
    Q3ListViewItemIterator it ( this );
    ++it;
    for ( ; it.current(); ++it ) {
        it.current()->setOpen ( FALSE );
    }
}
