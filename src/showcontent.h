/****************************************************************************
                             Hyper's CD Catalog
                A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/
#ifndef SHOWCONTENT_H
#define SHOWCONTENT_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QTextBrowser>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QPushButton;
class QTextBrowser;
class QToolButton;
class QLabel;
class Node;

class ShowContent : public QDialog {
    Q_OBJECT

public:
    ShowContent ( Node *node, bool isCategory=false, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~ShowContent();

    QPushButton* closeButton;
    QToolButton* deleteButton;
    QToolButton* saveButton;
    QTextBrowser* textBrowser;
    QLabel* sizel;

public slots:
    int deletenode ( void );
    int savenode ( void );

protected:
    Q3GridLayout* ShowContentLayout;
    Q3HBoxLayout* layout1;

protected slots:
    virtual void languageChange();

private:
    Node *parentnode,*mynode;
    bool isCategory;

};

#endif // SHOWCONTENT_H
