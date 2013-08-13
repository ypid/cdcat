/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
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

class LNode : public QTreeWidgetItem {
public:

    LNode ( QTreeWidget * parent, Node * dbnodep );
    LNode ( LNode * parent, Node* dbnodep, const QString &col2="" );
    LNode ( LNode * parent,LNode *after,Node* dbnodep, const QString &col2 );
 
    QString text ( int column ) const;

    QString fullName();

    void setExpanded ( bool );
    void setup();

    const QPixmap *pixmap ( int i ) const;
    void setPixmap ( QPixmap *p );

private:
    Node  * node;
    LNode * p;
    QPixmap *pix;
    bool childsCollected;

};

class HDirectoryView : public QTreeWidget {
    Q_OBJECT

public:
    void start ( void );
    HDirectoryView ( DataBase **dbp,QWidget *parent = 0, const char *name = 0 );
    ~HDirectoryView();
    DataBase **db;

public slots:
    void setDir ( Node *node );
    void closeAllBranch ( void );


signals:
    void folderSelected ( const QString & );
    void hitkey ( QKeyEvent * );

protected slots:
    void slotFolderSelected ( QTreeWidgetItem *item, int col );
    void slotFolderSelectedR ( QTreeWidgetItem *item, int col );
//     void openFolder();
    void itemExpanded ( QTreeWidgetItem * item );
    void itemCollapsed ( QTreeWidgetItem * item );


protected:
    void keyPressEvent ( QKeyEvent *e );
    void contentsMousePressEvent ( QMouseEvent *e );
    void contentsMouseReleaseEvent ( QMouseEvent *e );

private:
    QString fullPath ( QTreeWidgetItem* item );
    QTreeWidgetItem *oldCurrent;
    QTreeWidgetItem *dropItem;
    QTimer* autoopen_timer;
    QPoint presspos;
    bool mousePressed;

};

#endif
