/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef COMMENTWIDGET_H
#define COMMENTWIDGET_H

#include <qwidget.h>
#include <qimage.h>
#include <qvariant.h>
#include <qdialog.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <Q3GridLayout>
#include <QEvent>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QPaintEvent;
class QPixmap;
class Node;
class QPushButton;
class Q3MultiLineEdit;
class QEvent;
class CdCatConfig;

int editNodeComment ( Node *node,QWidget *parent );

class HQToolButton : public QToolButton {
public:
    HQToolButton ( QWidget *parent = 0 );
protected:
    void enterEvent ( QEvent *e );
    void leaveEvent ( QEvent *e );
};

class CommentWidget : public QWidget {
    Q_OBJECT

public:

    CommentWidget ( CdCatConfig * cc,QApplication *appl,QWidget *parent=0,const char *name=0,Qt::WFlags fl=0 );

    CdCatConfig * cconfig;
    HQToolButton* ButtonEdit;
    HQToolButton* ButtonContent;
    void showNode ( Node *node,int mod );


    Node *act;
    int  mode;
signals:
    void touchdb();
protected:
    void enterEvent ( QEvent *e );
    void leaveEvent ( QEvent *e );
    void paintEvent ( QPaintEvent *pe );
    void resizeEvent ( QResizeEvent *re );
    void mouseMoveEvent ( QMouseEvent *me );
    void mousePressEvent ( QMouseEvent *me );
    void mouseReleaseEvent ( QMouseEvent *me );

private:
    int mx,my,ox,oy;
    QPixmap image13;
    QApplication *app;

public slots:
    int editC ( void );
    int showC ( void );
};

//-------------------------------------------------------------------------------------
class commentEdit : public QDialog {
    Q_OBJECT

public:
    commentEdit ( QString cc,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~commentEdit();

    Q3MultiLineEdit* teComm;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

public slots:
    virtual int pushOk();
    virtual int pushCancel();

protected:
    Q3GridLayout* CommentEditBaseLayout;
    Q3VBoxLayout* layout5;
    Q3HBoxLayout* layout4;

public:
    QString newc;
    int OK;

protected slots:
    virtual void languageChange();
};

#endif // COMMENTEDIT_H
