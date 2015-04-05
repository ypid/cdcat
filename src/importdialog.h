/****************************************************************************
*                           Hyper's CD Catalog
*       A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke (crissi99@gmx.de)
*  License   : GPL
*  Copyright : (C) 2003 Christoph Thielecke
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
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>



class QLabel;
class QLineEdit;
class QPushButton;

QList<QList<QString> > getImportPatternList();

class ImportDialog : public QDialog {
    Q_OBJECT

public:
    ImportDialog ( QWidget *parent = 0, const char *name = 0, bool modal = true, Qt::WFlags fl = 0 );
    ~ImportDialog();


    QLabel *filename_lab;
    QLabel *separator_lab;
    QLabel *info_lab;
    QLabel *genericRegexPattern_lab;
    QLabel *genericRegexInfo;

    QPushButton *buttonCancel;
    QPushButton *buttonOK;
    QPushButton *buttonGetFile;
    QLineEdit *filename_lineedit;
    QLineEdit *separator_lineedit;
    QLineEdit *genericRegex_lineedit;
    QCheckBox *newdatabase;
    QCheckBox *correctbadstyle;
    QRadioButton *importTypeCsvGtktalog;
    QRadioButton *importTypeCsvKatCeDe;
    QRadioButton *importTypeCsvDisclib;
    QRadioButton *importTypeCsvVisualcd;
    QRadioButton *importTypeCsvVvv;
    QRadioButton *importTypeCsvAdvancedFileOrganizer;
    QRadioButton *importTypeCsvFileArchivist;
    QRadioButton *importTypeCsvAdvancedDiskCatalog;
    QRadioButton *importTypeCsvWhereisit;
    QRadioButton *importTypeGtktalogXml;
    QRadioButton *importTypeWhereisitXml;
    QRadioButton *importTypeGenericRegex;
    QGroupBox *importButtonBox;


protected:
    QGridLayout *layout4;
    QGridLayout *ImportDialogLayout;
    QVBoxLayout *type_layout;
    QHBoxLayout *csv_layout;
    QVBoxLayout *layoutGroupBox;
    QHBoxLayout *layoutGenericRegex;

protected slots:
    virtual void languageChange();
    int bOk();
    int bCan();
    void getFileName();
    void typeChanged();


private:
    QString lastDir;

public:
    int OK;
    QString filename, separator;
    bool createdatabase;
    int type;
    void setLastDir( QString lastDir );
    QString getLastDir();
};

#endif // IMPORTDIALOG_H
