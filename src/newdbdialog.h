/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef NEWDBDIALOG_H
#define NEWDBDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qstring.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class Q3MultiLineEdit;
class Node;
class GuiSlave;


class newdbdialog : public QDialog {
    Q_OBJECT

public:
    int OK;
    QString nameD,ownerD,commD;

    newdbdialog ( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~newdbdialog();

    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLineEdit* leName;
    QLineEdit* leOwner;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;

    Q3MultiLineEdit *teComm;


protected:
    Q3GridLayout* NewDBDialogBaseLayout;
    Q3HBoxLayout* layout12;

protected slots:
    virtual void languageChange();
    int ok ( void );
    int can ( void );
};

class renamedialog : public QDialog {
    Q_OBJECT
public:
    GuiSlave *slave;
    Node *patient;

    renamedialog ( QWidget *parent,Node *ppatient,GuiSlave *pslave );
    ~renamedialog();

    QLabel *label;
    QLineEdit *leText;
    QPushButton *okB;
    QPushButton *caB;

protected slots:
    virtual void languageChange();
    int ok ( void );
    int can ( void );

};

class renumberdialog : public QDialog {
    Q_OBJECT
public:
    GuiSlave *slave;
    Node *patient;

    renumberdialog ( QWidget *parent,Node *ppatient,GuiSlave *pslave );
    ~renumberdialog();

    QLabel *label;
    QLineEdit *leText;
    QPushButton *okB;
    QPushButton *caB;

protected slots:
    virtual void languageChange();
    int ok ( void );
    int can ( void );

};


#endif // NEWDBDIALOG_H
