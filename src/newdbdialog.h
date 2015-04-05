/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef NEWDBDIALOG_H
#define NEWDBDIALOG_H

#include <QVariant>
#include <qdialog.h>
#include <qstring.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#ifdef CATALOG_ENCRYPTION
#include <QCheckBox>
#endif

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
#ifdef CATALOG_ENCRYPTION
class QCheckBox;
#endif
class Node;
class GuiSlave;


class newdbdialog : public QDialog {
    Q_OBJECT

public:
    int OK;
    QString nameD, ownerD, commD, categoryD;

#ifdef CATALOG_ENCRYPTION
    bool catalogEncrypted;
    QString encryptionPassword;
#endif

    newdbdialog ( QWidget *parent = 0, const char *name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~newdbdialog();

    QLabel *textLabelDatabase;
    QLabel *textLabelOwner;
    QLabel *textLabelComment;
    QLabel *textLabelCategory;
#ifdef CATALOG_ENCRYPTION
    QLabel *textLabelEncryption;
    QLabel *textLabelEncryptionPassword;
#endif
    QLineEdit *leName;
    QLineEdit *leOwner;
    QLineEdit *leCategory;
    QPushButton *buttonOK;
    QPushButton *buttonCancel;
#ifdef CATALOG_ENCRYPTION
    QCheckBox *cbEncryption;
    QLineEdit *leEncryptionPassword;
#endif

    QTextEdit *teComm;



protected:
    QGridLayout *NewDBDialogBaseLayout;
    QHBoxLayout *layout12;

protected slots:
    virtual void languageChange();
    int ok( void );
    int can( void );
};

class renamedialog : public QDialog {
    Q_OBJECT
public:
    GuiSlave * slave;
    Node *patient;

    renamedialog ( QWidget *parent, Node *ppatient, GuiSlave *pslave );
    ~renamedialog();

    QLabel *label;
    QLineEdit *leText;
    QPushButton *okB;
    QPushButton *caB;

protected slots:
    virtual void languageChange();
    int ok( void );
    int can( void );
};

class renumberdialog : public QDialog {
    Q_OBJECT
public:
    GuiSlave * slave;
    Node *patient;

    renumberdialog ( QWidget *parent, Node *ppatient, GuiSlave *pslave );
    ~renumberdialog();

    QLabel *label;
    QLineEdit *leText;
    QPushButton *okB;
    QPushButton *caB;

protected slots:
    virtual void languageChange();
    int ok( void );
    int can( void );
};


#endif // NEWDBDIALOG_H
