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
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class Node;
class GuiSlave;


class newdbdialog : public QDialog {
    Q_OBJECT

public:
    int OK;
    QString nameD,ownerD,commD, categoryD;

    newdbdialog ( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~newdbdialog();

    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabelCategory;
    QLineEdit* leName;
    QLineEdit* leOwner;
    QLineEdit* leCategory;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;

    QTextEdit *teComm;



protected:
    QGridLayout* NewDBDialogBaseLayout;
    QHBoxLayout* layout12;

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
