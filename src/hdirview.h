/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <q3listview.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDragLeaveEvent>


class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QKeyEvent;
class DataBase;
class Node;
class QPainter;
class QColorGroup;

class LNode : public Q3ListViewItem {
public:

    LNode ( Q3ListView * parent, Node * dbnodep );

    LNode ( LNode * parent, Node* dbnodep, const QString &col2 )
            : Q3ListViewItem ( parent, col2 ), pix ( 0 ) { node=dbnodep; }

    LNode ( LNode * parent,LNode *after,Node* dbnodep, const QString &col2 )
            : Q3ListViewItem ( parent,after, col2 ), pix ( 0 ) { node=dbnodep; }

    LNode ( LNode * parent, Node*dbnodep );

    QString text ( int column ) const;

    QString fullName();

    void setOpen ( bool );
    void setup();

    const QPixmap *pixmap ( int i ) const;
    void setPixmap ( QPixmap *p );

    void paintCell ( QPainter *p,const QColorGroup & cg,int column,int width,int align );

private:
    Node  * node;
    LNode * p;
    QPixmap *pix;

};

class HDirectoryView : public Q3ListView {
    Q_OBJECT

public:
    void start ( void );
    HDirectoryView ( DataBase **dbp,QWidget *parent = 0, const char *name = 0 );

    DataBase **db;

public slots:
    void setDir ( Node *node );
    void closeAllBranch ( void );


signals:
    void folderSelected ( const QString & );
    void hitkey ( QKeyEvent * );

protected slots:
    void slotFolderSelected ( Q3ListViewItem * );
    void slotFolderSelectedR ( Q3ListViewItem * );
    void openFolder();

protected:
    void keyPressEvent ( QKeyEvent *e );
    void contentsDragEnterEvent ( QDragEnterEvent *e );
    void contentsDragMoveEvent ( QDragMoveEvent *e );
    void contentsDragLeaveEvent ( QDragLeaveEvent *e );
    void contentsDropEvent ( QDropEvent *e );
    void contentsMouseMoveEvent ( QMouseEvent *e );
    void contentsMousePressEvent ( QMouseEvent *e );
    void contentsMouseReleaseEvent ( QMouseEvent *e );

private:
    QString fullPath ( Q3ListViewItem* item );
    Q3ListViewItem *oldCurrent;
    Q3ListViewItem *dropItem;
    QTimer* autoopen_timer;
    QPoint presspos;
    bool mousePressed;

};

#endif
