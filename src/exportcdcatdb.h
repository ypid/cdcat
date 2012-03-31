#ifndef __EXPORTCDCATDB_H__
#define __EXPORTCDCATDB_H__
/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Christoph Thielecke (crissi99@gmx.de)
 License   : GPL
 Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qdatetime.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qobject.h>
#include <q3listview.h>
#include <qfiledialog.h>
#include <qwidget.h>
#include <qdatetime.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <Q3VBoxLayout>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <qvariant.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qdialog.h>
#include <qlabel.h>
#include <stdio.h>
#include <ctype.h>
#include <qfile.h>
#include <QLayout>


#include "guibase.h"
#include "dbase.h"
#include "wdbfile.h"
#include "mainwidget.h"

class QLabel;
class QLineEdit;
class QPushButton;
class PWw;
class QApplication;

class exportCdcatDB : public QDialog {
    Q_OBJECT

public:
    exportCdcatDB ( CdCatMainWidget *mainw, QWidget* parent=0, const char* name=0, bool modal=true, Qt::WFlags fl=0 );
    ~exportCdcatDB();


    void fillMedia();
    QLabel* textLabel2;
    Q3ListBox* listAllMedia;
    QPushButton* buttonRemove;
    QPushButton* buttonAdd;
    QLabel* textLabel2_2;
    Q3ListBox* listSelectedMedia;
    QCheckBox* checkAllMedia;
    QCheckBox* checkOnlyMedia;
    QCheckBox* checkExportMediaName;
    QCheckBox* checkExportMediaNumber;
    QCheckBox* checkExportPath;
    QCheckBox* checkExportFile;
    QCheckBox* checkExportSize;
    QCheckBox* checkExportDate;
    QCheckBox* checkExportComment;
    QCheckBox* checkExportCategory;
    QCheckBox* checkExportMp3Tag;
    QCheckBox* checkExportBorrow;


    QCheckBox* checkExportTitle;
    QCheckBox* checkExportTableHeader;
    Q3ButtonGroup* buttonGroup1;
    Q3ButtonGroup* buttonGroupFields;
    QRadioButton* radioHtml;
    QRadioButton* radioCsv;
    QRadioButton* radioXml;
    QLabel* seperatorLabel;
    QLabel* textLabel3;
    QLabel* spinColsLabel;
    QLineEdit* separatorInput;
    QLineEdit* fileName;
    QPushButton* buttonFile;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QSpinBox*	spinCols;
    int OK;


protected:
    PWw *pww;
    QApplication *app;
    QGridLayout* exportCdcatDBLayout;
    Q3VBoxLayout* layout32;
    QHBoxLayout* layout21;
    Q3VBoxLayout* layout10_2;
    Q3VBoxLayout* layout19;
    QHBoxLayout* layout10;
    Q3VBoxLayout* layout10_2_2;
    Q3VBoxLayout* buttonGroup1Layout;
    QHBoxLayout* layout26;
    QHBoxLayout* layout25;
    QHBoxLayout* layout29;
    QHBoxLayout* layout4;
    QHBoxLayout* layout27;
    Q3VBoxLayout* typeLayout;
    Q3VBoxLayout* fieldsLayout;

    DataBase *p;
    QString separator;
    QFile f;
    int writeDown ( Node *source );
    int  writeCatalog ( Node *source );
    int  writeMedia ( Node *source );
    int  writeDirectory ( Node *source );
    int  writeFile ( Node *source );
    int  writeMp3Tag ( Node *source );
    int  writeCatLnk ( Node *source );
    QString getFilePath ( Node *current );

protected slots:
    virtual void languageChange();
    bool doHtmlExport();
    bool doCsvExport();
    void checkExportToggled();
    void removeMedia();
    void listAllMediaToggled();
    void exportTypeToggled();
    void checkOnlyMediaToggled();
    void browseFile();
    void addMedia();
public slots:
    void ok();
    void cancel();
private:
    QString outstring;
    QString medianame;
    QString currentpath;
    QString lastMedianame;
    QString mediaborrow;
    int medianumber;
    CdCatMainWidget *mainw;
};

#endif
