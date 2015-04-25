/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QEvent>
#include <QPoint>
#include <QMessageBox>
#include <QStringList>
#include <QApplication>
#include <QObject>
#include <QMouseEvent>

#include <QHeaderView>

#include <iostream>

#include "dirview.h"
#include "icons.h"
#include "cdcat.h"

/*****************************************************************************
*
* Class Directory
*
*****************************************************************************/

Directory::Directory ( Directory *parent, const QString& filename )
    : QTreeWidgetItem( parent, QTreeWidgetItem::UserType ), f( filename ) {
    p = parent;
    readable = QDir( fullName()).isReadable();

    if (!readable) {
        setIcon( 0, QIcon( *get_v_folderlocked_icon()));
    } else {
        setIcon( 0, QIcon( *get_v_folderclosed_icon()));
    }
    setText( 0, filename );
    childsCollected = false;
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
}


Directory::Directory ( QTreeWidget *parent, const QString& filename )
    : QTreeWidgetItem( parent, QTreeWidgetItem::UserType ), f( filename ) {
    p = NULL;
    readable = QDir( fullName()).isReadable();
    setText( 0, filename );
    childsCollected = false;
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
    this->setExpanded( true );
}


void Directory::setPixmap( QPixmap * ) {
    // pix = px;
    setup();
    // widthChanged ( 0 );
    // invalidateHeight();
    // repaint();
}


const QPixmap *Directory::pixmap( int i ) const {
    if (i) {
        return 0;
    }
    return pix;
}

void Directory::setExpanded( bool o ) {
    if (o) {
        setIcon( 0, QIcon( *get_v_folderopen_icon()));
    } else {
        setIcon( 0, QIcon( *get_v_folderclosed_icon()));
    }
    if (o && !childsCollected) {
        QString s( fullName());
        QDir thisDir( s );
        if (!thisDir.isReadable()) {
            readable = false;
            // setExpandable ( false ); // FIXME
            return;
        }

        // listView()->setUpdatesEnabled ( false ); //FIXME
        QFileInfoList files( thisDir.entryInfoList());
        if (files.size() > 0) {
            for (int i = 0; i < files.size(); ++i) {
                QFileInfo fi( files.at( i ));
                if (fi.fileName() == "." || fi.fileName() == "..") {
                    ;                     // nothing
                } else if (fi.isDir()) {
                    Directory *d = new Directory( this, fi.fileName());
                    d->setExpanded( false );
                } else {
                    ;
                }
            }
            childsCollected = true;
            setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
        } else {
            setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicator );
        }
        // listView()->setUpdatesEnabled ( true ); //FIXME
    }
    QTreeWidgetItem::setExpanded( o );
}


void Directory::setup() {
    // setExpandable ( true );
    // QTreeWidgetItem::setup();
}


QString Directory::fullName() {
    QString s;

    if (p) {
        s = p->fullName();
        s.append( f.fileName());
        s.append( "/" );
    } else {
        s = f.fileName();
    }
    return s;
}


QString Directory::text( int column ) const {
    if (column == 0) {
        return f.fileName();
    } else if (readable) {
        return QObject::tr( "Directory" );
    } else {
        return QObject::tr( "Unreadable Directory" );
    }
    return "";
}

/*****************************************************************************
*
* Class DirectoryView
*
*****************************************************************************/

DirectoryView::DirectoryView ( QWidget *parent, const char * )
    : QTreeWidget( parent ), oldCurrent( 0 ), dropItem( 0 ), mousePressed( false ) {
//      autoopen_timer = new QTimer ( this );

    setSortingEnabled( true );
    sortByColumn( 0, Qt::AscendingOrder );
    setItemsExpandable( true );
    connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int )),
             this, SLOT( slotFolderSelected( QTreeWidgetItem *, int )));

    // connect ( this, SIGNAL ( itemPressed(QTreeWidgetItem*,int)),
    //         this, SLOT ( slotFolderSelectedR ( QTreeWidgetItem *, int ) ) );

    connect( this, SIGNAL( itemClicked( QTreeWidgetItem *, int )),
             this, SLOT( slotFolderSelectedR( QTreeWidgetItem *, int )));

    connect( this, SIGNAL( itemExpanded( QTreeWidgetItem * )), this, SLOT( itemExpanded( QTreeWidgetItem * )));
    connect( this, SIGNAL( itemCollapsed( QTreeWidgetItem * )), this, SLOT( itemCollapsed( QTreeWidgetItem * )));

    setAcceptDrops( true );
    viewport()->setAcceptDrops( true );

//      connect ( autoopen_timer, SIGNAL ( timeout() ),
//                this, SLOT ( openFolder() ) );

    QStringList labels;
    labels.append( tr( "Name" ));
    setHeaderLabels( labels );
    // setIndentation ( 10 );

    QFileInfoList roots( QDir::drives());

    for (int ri = 0; ri < roots.size(); ++ri) {
        QFileInfo fi( roots.at( ri ));
        // qDebug() << "DirView::DirView() root found: " << qPrintable(fi.filePath());

        Directory *root = new Directory( this, fi.filePath());
#if defined(_WIN32) || defined(_OS2)
        // Skip reading floppy drives on startup.
        if (roots.count() >= 1 && fi.filePath() != QString( "A:/" ) && fi.filePath() != QString( "B:/" )) {
#else
        if (roots.count() >= 1) {
#endif
            root->setExpanded( true );
        }
    }
#ifdef Q_OS_MAC
    // Directory * volume_dir = new Directory ( this, "/Volumes" );
    QDir volumedir( "/Volumes" );
    QFileInfoList volumes( volumedir.entryInfoList());
    for (int ri = 0; ri < volumes.size(); ++ri) {
        QFileInfo fi( volumes.at( ri ));
        if (fi.fileName() == QString( "." ) || fi.fileName() == QString( ".." )) {
            continue;
        }
        Directory *root = new Directory( this, fi.filePath());
        if (volumes.count() >= 1) {
            root->setExpanded( true );
        }
    }
    setDir( "Volumes" );
#endif

    setAllColumnsShowFocus( true );
    // repaint();
    sDir = "";
}

DirectoryView::~DirectoryView() {
    clear();
}


void DirectoryView::slotFolderSelected( QTreeWidgetItem *i, int ) {
    if (i == NULL) {
        return;
    }
    Directory *dir = (Directory *)i;
    emit folderSelected( dir->fullName());
}

void DirectoryView::slotFolderSelectedR( QTreeWidgetItem *i, int ) {
    if (i == NULL) {
        return;
    }
    Directory *dir = (Directory *)i;
    sDir = dir->fullName();
    if (dir->isExpanded()) {
        dir->setExpanded( false );
    } else {
        emit setExpandedInProgress( true );
        dir->setExpanded( true );
        emit setExpandedInProgress( false );
    }
    emit folderSelected( dir->fullName());
}

void DirectoryView::itemExpanded( QTreeWidgetItem *item ) {
    ((Directory *)item)->setExpanded( true );
}

void DirectoryView::itemCollapsed( QTreeWidgetItem *item ) {
    ((Directory *)item)->setExpanded( false );
}

// void DirectoryView::openFolder() {
//      autoopen_timer->stop();
//      if ( dropItem) {
//              if( !dropItem->isExpanded() ) {
//              dropItem->setExpanded ( true );
//              //dropItem->repaint();
//              }
//      }
//      else {
//              itemAt(0,0)->setExpanded(true);
//      }
// }

static const int autoopenTime = 750;

QString DirectoryView::fullPath( QTreeWidgetItem *item ) {
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

void DirectoryView::contentsMousePressEvent( QMouseEvent *  ) {
    // QTreeWidget::contentsMousePressEvent ( e );
//	QPoint p (  e->pos()  );
//	QTreeWidgetItem *i = itemAt ( p );
//      if ( i ) {
//              // if the user clicked into the root decoration of the item, don't try to start a drag!
//              if ( p.x() > header()->cellPos ( header()->mapToActual ( 0 ) ) +
//                              treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0 ) ) + itemMargin() ||
//                              p.x() < header()->cellPos ( header()->mapToActual ( 0 ) ) ) {
//                      presspos = e->pos();
//                      mousePressed = true;
//              }
//      } // FIXME
}


void DirectoryView::contentsMouseReleaseEvent( QMouseEvent * ) {
    mousePressed = false;
}

void DirectoryView::setDir( const QString &s ) {
    if (s.right( s.size() - 1 ) == "/") {
        s.leftRef( s.size() - 1 );
    }
    QTreeWidgetItemIterator it( this );
    while (*it) {
        (*it)->setExpanded( false );
        ++it;
    }

    QStringList lst = s.split(
#ifdef _WIN32
        "\\"
#else
        "/"
#endif
        );

    QTreeWidgetItemIterator it3( this );
    for (int i = 1; i < lst.size(); i++) {
        while ((*it3)) {
            if ((*it3)->text( 0 ) == lst.at( i )) {
                (*it3)->setExpanded( true );
                setCurrentItem((*it3));

//                              //ensureItemVisible ( item ); //FIXME
//                              QTreeWidgetItem * item2 = item->itemAbove();
//                              if ( item2 != NULL ) {
//                                      QTreeWidgetItem *item3 = item2->itemAbove();
//                                      if ( item3 != NULL ) {
//                                              QTreeWidgetItem *item4 = item2->itemAbove();
//                                              if ( item4 != NULL ) {
//                                                      //ensureItemVisible ( item4 ); //FIXME
//                                              }
//                                              else {
//                                                      //ensureItemVisible ( item3 ); //FIXME
//                                              }
//                                      }
//                                      else {
//                                              //ensureItemVisible ( item2 ); //FIXME
//                                      }
//                              }
                break;
            }
            ++it3;
        }
    }
}

void FileItem::setPixmap( QPixmap * ) {
    // pix = p;
    // setup();
    // widthChanged ( 0 );
    // invalidateHeight();
    // repaint();
}


const QPixmap *FileItem::pixmap( int i ) const {
    if (i) {
        return 0;
    }
    return pix;
}
