/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/


#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QPixmap>

class MainWidget;
class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

class FileItem : public QTreeWidgetItem {
public:
    FileItem ( QTreeWidgetItem *parent, const QString &s1, const QString &s2 )
        : QTreeWidgetItem( parent ), pix( 0 ) {
        setText( 0, s1 );
        setText( 1, s2 );
    }

    const QPixmap *pixmap( int i ) const;
    void setPixmap( QPixmap *p );

private:
    QPixmap *pix;
};

class Directory : public QTreeWidgetItem {
public:
    Directory ( QTreeWidget *parent, const QString& filename );
    Directory ( Directory *parent, const QString& filename );

    QString text( int column ) const;

    QString fullName();


    void setup();

    const QPixmap *pixmap( int i ) const;
    void setPixmap( QPixmap *p );
    void setExpanded( bool );

private:
    QFile f;
    Directory *p;
    bool readable;
    bool childsCollected;
    QPixmap *pix;
};

class DirectoryView : public QTreeWidget {
    Q_OBJECT

public:
    DirectoryView ( QWidget *parent = 0, const char *name = 0 );
    ~DirectoryView();
    QString sDir;

public slots:
    void setDir( const QString & );

signals:
    void folderSelected( const QString & );
    void setExpandedInProgress( bool );

protected slots:
    void slotFolderSelected( QTreeWidgetItem *, int );
    void slotFolderSelectedR( QTreeWidgetItem *, int );
    void itemExpanded( QTreeWidgetItem *item );
    void itemCollapsed( QTreeWidgetItem *item );
//     void openFolder();

protected:
    void contentsMousePressEvent( QMouseEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );

private:
    QString fullPath( QTreeWidgetItem *item );
    QTreeWidgetItem *oldCurrent;
    QTreeWidgetItem *dropItem;
    QTimer *autoopen_timer;
    QPoint presspos;
    bool mousePressed;
};

#endif
