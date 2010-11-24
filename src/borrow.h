/****************************************************************************
                         Hyper's CD Catalog
             A multiplatform qt and xml based catalog program

  Author    : Peter Deak (hyperr@freemail.hu)
  License   : GPL
  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef BORROW_H
#define BORROW_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>


class QCheckBox;
class Q3Table;
class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class DataBase;
class QPoint;

class borrowDialog : public QDialog {
    Q_OBJECT

public:
    QString m;
    int ok;
    borrowDialog ( QString mname,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~borrowDialog();

    QLabel* textLabel;
    QLineEdit* leWho;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;


protected:
    Q3VBoxLayout* borrowDialogLayout;
    Q3HBoxLayout* layout4;

protected slots:
    int sok ( void );
    int scancel ( void );

    virtual void languageChange();

};

//===============================================================

class borrowingDialog : public QDialog {
    Q_OBJECT

public:
    int ch;
    borrowingDialog ( DataBase *dbp, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~borrowingDialog();

    DataBase *db;
    QLabel* textLabel;
    QCheckBox* cbOnlyBorrowed;
    QPushButton* buttonClear;
    Q3Table* table;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    int last_row_clicked;

public slots:

    int sok ( void );
    int scancel ( void );
    int sclear ( void );
    int filltable ( void );
    int schanged ( int row,int col );
    int sonlyb();
    int sstore();
    int click ( int row,int col,const QPoint & mousePos );
    int click_set ( int a );
    int click_clr ( void );


protected:
    Q3VBoxLayout* borrowingDialogLayout;
    Q3HBoxLayout* layout1;
    Q3HBoxLayout* layout3;
    Q3HBoxLayout* layout4;

protected slots:
    virtual void languageChange();
};

#endif // BORROW_H
