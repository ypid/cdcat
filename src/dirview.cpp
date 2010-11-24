/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <q3dragobject.h>
#include <qmime.h>
#include <q3strlist.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <q3header.h>
#include <qobject.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "dirview.h"
#include "icons.h"

/*****************************************************************************
 *
 * Class Directory
 *
 *****************************************************************************/

Directory::Directory ( Directory * parent, const QString& filename )
        : Q3ListViewItem ( parent ), f ( filename ),
        pix ( 0 ) {
    p = parent;
    readable = QDir ( fullName() ).isReadable();

    if ( !readable )
        setPixmap ( get_v_folderlocked_icon() );
    else
        setPixmap ( get_v_folderclosed_icon() );
}


Directory::Directory ( Q3ListView * parent, const QString& filename )
        : Q3ListViewItem ( parent ), f ( filename ),
        pix ( 0 ) {
    p = 0;
    readable = QDir ( fullName() ).isReadable();
}


void Directory::setPixmap ( QPixmap *px ) {
    pix = px;
    setup();
    widthChanged ( 0 );
    invalidateHeight();
    repaint();
}


const QPixmap *Directory::pixmap ( int i ) const {
    if ( i )
        return 0;
    return pix;
}

void Directory::setOpen ( bool o ) {
    if ( o )
        setPixmap ( get_v_folderopen_icon() );
    else
        setPixmap ( get_v_folderclosed_icon() );

    if ( o && !childCount() ) {
        QString s ( fullName() );
        QDir thisDir ( s );
        if ( !thisDir.isReadable() ) {
            readable = FALSE;
            setExpandable ( FALSE );
            return;
        }

        listView()->setUpdatesEnabled ( FALSE );
        QFileInfoList *files = new QFileInfoList ( thisDir.entryInfoList() );
        if ( files ) {
            for ( int i = 0; i < files->size(); ++i ) {
                QFileInfo *fi = new QFileInfo ( files->at ( i ) );
                if ( fi->fileName() == "." || fi->fileName() == ".." ) {
                    ; // nothing
                } else if ( fi->isDir() ) {
                    ( void ) new Directory ( this, fi->fileName() );
                } else {
                    ;
                }
            }
        }
        listView()->setUpdatesEnabled ( TRUE );
    }
    Q3ListViewItem::setOpen ( o );
}


void Directory::setup() {
    setExpandable ( TRUE );
    Q3ListViewItem::setup();
}


QString Directory::fullName() {
    QString s;
    if ( p ) {
        s = p->fullName();
        s.append ( f.name() );
        s.append ( "/" );
    } else {
        s = f.name();
    }
    return s;
}


QString Directory::text ( int column ) const {
    if ( column == 0 )
        return f.name();
    else if ( readable )
        return QObject::tr ( "Directory" );
    else
        return QObject::tr ( "Unreadable Directory" );
    return "";
}

/*****************************************************************************
 *
 * Class DirectoryView
 *
 *****************************************************************************/

DirectoryView::DirectoryView ( QWidget *parent, const char *name )
        : Q3ListView ( parent, name ),  oldCurrent ( 0 ),dropItem ( 0 ), mousePressed ( FALSE ) {

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

    addColumn ( tr ( "Name" ) );
    setTreeStepSize ( 10 );

    QFileInfoList *roots=new QFileInfoList ( QDir::drives() );
#ifdef _WIN32
    ++i; //Skip reading floppy drive on startup.
#endif

    for ( int ri = 0; ri < roots->size(); ++ri ) {
        QFileInfo *fi = new QFileInfo ( roots->at ( ri ) );
        Directory * root = new Directory ( this, fi->filePath() );
        if ( roots->count() >= 1 )
            root->setOpen ( TRUE );
    }

    setAllColumnsShowFocus ( TRUE );
    repaint();
    sDir = "";

}

void DirectoryView::slotFolderSelected ( Q3ListViewItem *i ) {
    if ( i==NULL ) return;
    Directory *dir = ( Directory* ) i;
    emit folderSelected ( dir->fullName() );
}

void DirectoryView::slotFolderSelectedR ( Q3ListViewItem *i ) {
    if ( i==NULL ) return;
    Directory *dir = ( Directory* ) i;
    sDir = dir->fullName();
    emit folderSelected ( dir->fullName() );
}


void DirectoryView::openFolder() {
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
        dropItem->setOpen ( TRUE );
        dropItem->repaint();
    }
}

static const int autoopenTime = 750;

QString DirectoryView::fullPath ( Q3ListViewItem* item ) {
    QString fullpath = item->text ( 0 );
    while ( ( item=item->parent() ) ) {
        if ( item->parent() )
            fullpath = item->text ( 0 ) + "/" + fullpath;
        else
            fullpath = item->text ( 0 ) + fullpath;
    }
    return fullpath;
}

void DirectoryView::contentsMousePressEvent ( QMouseEvent* e ) {
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


void DirectoryView::contentsMouseReleaseEvent ( QMouseEvent * ) {
    mousePressed = FALSE;
}

void DirectoryView::setDir ( const QString &s ) {
    Q3ListViewItemIterator it ( this );
    ++it;
    for ( ; it.current(); ++it ) {
        it.current()->setOpen ( FALSE );
    }

    QStringList lst ( QStringList::split (
#ifdef _WIN32
                          "\\"
#else
                          "/"
#endif
                          , s ) );

    Q3ListViewItem *item = firstChild();
    QStringList::Iterator it2 = lst.begin();
    for ( ; it2 != lst.end(); ++it2 ) {
        while ( item ) {
            if ( item->text ( 0 ) == *it2 ) {
                item->setOpen ( TRUE );
                break;
            }
            item = item->itemBelow();
        }
    }

    if ( item )
        setCurrentItem ( item );
}

void FileItem::setPixmap ( QPixmap *p ) {
    pix = p;
    setup();
    widthChanged ( 0 );
    invalidateHeight();
    repaint();
}


const QPixmap *FileItem::pixmap ( int i ) const {
    if ( i )
        return 0;
    return pix;
}
