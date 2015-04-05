/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef COMMENTWIDGET_H
#define COMMENTWIDGET_H

#include <qwidget.h>
#include <QImage>
#include <QVariant>
#include <qdialog.h>
#include <QToolButton>
#include <QPixmap>
#include <QResizeEvent>
#include <QGridLayout>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>

#include "config.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPaintEvent;
class QPixmap;
class Node;
class QPushButton;
class QTextEdit;
class QEvent;
class CdCatConfig;
class QScrollArea;

int editNodeComment( Node *node, QWidget *parent, CdCatConfig *cconfig, bool isCommentEdit = true );

class HQToolButton : public QToolButton {
public:
    HQToolButton ( QWidget *parent = 0 );
protected:
    void enterEvent( QEvent *e );
    void leaveEvent( QEvent *e );
};

class CommentWidget : public QWidget {
    Q_OBJECT

public:

    CommentWidget ( CdCatConfig *cc, QApplication *appl, QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0 );

    CdCatConfig *cconfig;
    HQToolButton *ButtonEdit;
    HQToolButton *ButtonCategory;
    HQToolButton *ButtonCategoryEdit;
    HQToolButton *ButtonContent;
    QScrollArea *sa;
    void showNode( Node *node, int mod );
    void setScrollArea( QScrollArea *sa );
    void updateContents();

    Node *act;
    int mode;
    bool noRepaint;
signals:
    void touchdb();
protected:
    void enterEvent( QEvent *e );
    void leaveEvent( QEvent *e );
    void paintEvent( QPaintEvent *pe );
    void resizeEvent( QResizeEvent *re );
    void mouseMoveEvent( QMouseEvent *me );
    void mousePressEvent( QMouseEvent *me );
    void mouseReleaseEvent( QMouseEvent *me );

private:
    int mx, my, ox, oy;
    bool update_contents_running;
    QPixmap image13;
    QApplication *app;
    QPixmap contentsPixmap;
    QPixmap contentsPixmapTmp;
    bool paintEventRunning;

public slots:
    int editC( void );
    int showC( void );
    int editCategory( void );
    int showCategory( void );
};

// -------------------------------------------------------------------------------------
class commentEdit : public QDialog {
    Q_OBJECT

public:
    commentEdit ( QString cc, CdCatConfig *cconfig = NULL, QWidget *parent = 0, const char *name = 0, bool modal = FALSE, bool isCommentEdit = true, Qt::WFlags fl = 0 );
    ~commentEdit();

    QTextEdit *teComm;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;

public slots:
    virtual int pushOk();
    virtual int pushCancel();

protected:
    QGridLayout *CommentEditBaseLayout;
    QVBoxLayout *layout5;
    QHBoxLayout *layout4;

public:
    QString newc;
    int OK;
    bool isCommentEdit;
    CdCatConfig *cconfig;

protected slots:
    virtual void languageChange();
};

#endif // COMMENTEDIT_H
