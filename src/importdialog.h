/****************************************************************************
                            Hyper's CD Catalog
	A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke (crissi99@gmx.de)
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qdatetime.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>



class QLabel;
class QLineEdit;
class QPushButton;

class ImportDialog : public QDialog {
    Q_OBJECT

public:
    ImportDialog ( QWidget* parent = 0, const char* name = 0, bool modal = true, Qt::WFlags fl = 0 );
    ~ImportDialog();


    QLabel* filename_lab;
    QLabel* separator_lab;
    QLabel* info_lab;

    QPushButton* buttonCancel;
    QPushButton* buttonOK;
    QPushButton* buttonGetFile;
    QLineEdit* filename_lineedit;
    QLineEdit* separator_lineedit;
    QCheckBox *newdatabase;
    QCheckBox *correctbadstyle;
    QRadioButton *importTypeCsvGtktalog;
    QRadioButton *importTypeCsvKatCeDe;
    QRadioButton *importTypeGtktalogXml;
    QRadioButton *importTypeWhereisitXml;
    Q3ButtonGroup *importButtonBox;


protected:
    Q3GridLayout* layout4;
    Q3GridLayout* ImportDialogLayout;
    Q3VBoxLayout* type_layout;
    Q3HBoxLayout* csv_layout;

protected slots:
    virtual void languageChange();
    int bOk();
    int bCan();
    void getFileName();
    void typeChanged();

private:


public:
    int OK;
    QString filename, separator;
    bool createdatabase;
    int type;

};

#endif // IMPORTDIALOG_H
