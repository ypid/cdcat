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

class FileItem : public Q3ListViewItem {
public:
    FileItem ( Q3ListViewItem *parent, const QString &s1, const QString &s2 )
            : Q3ListViewItem ( parent, s1, s2 ), pix ( 0 ) {}

    const QPixmap *pixmap ( int i ) const;
    void setPixmap ( QPixmap *p );

private:
    QPixmap *pix;

};

class Directory : public Q3ListViewItem {
public:
    Directory ( Q3ListView * parent, const QString& filename );
    Directory ( Directory * parent, const QString& filename, const QString &col2 )
            : Q3ListViewItem ( parent, filename, col2 ), pix ( 0 ) {}
    Directory ( Directory * parent, const QString& filename );

    QString text ( int column ) const;

    QString fullName();

    void setOpen ( bool );
    void setup();

    const QPixmap *pixmap ( int i ) const;
    void setPixmap ( QPixmap *p );

private:
    QFile f;
    Directory * p;
    bool readable;
    QPixmap *pix;

};

class DirectoryView : public Q3ListView {
    Q_OBJECT

public:
    DirectoryView ( QWidget *parent = 0, const char *name = 0 );
    QString sDir;

public slots:
    void setDir ( const QString & );

signals:
    void folderSelected ( const QString & );

protected slots:
    void slotFolderSelected ( Q3ListViewItem * );
    void slotFolderSelectedR ( Q3ListViewItem * );
    void openFolder();

protected:
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
