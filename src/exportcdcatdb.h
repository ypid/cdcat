#ifndef __EXPORTCDCATDB_H__
#define __EXPORTCDCATDB_H__
/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke (crissi99@gmx.de)
*  License   : GPL
*  Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#include "guibase.h"
#include "dbase.h"
#include "wdbfile.h"
#include "mainwidget.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <QCheckBox>
#include <QDateTime>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class PWw;
class QApplication;

class exportCdcatDB : public QDialog {
    Q_OBJECT

public:
    exportCdcatDB ( CdCatMainWidget *mainw, QWidget *parent = 0, const char *name = 0, bool modal = true, Qt::WindowFlags fl = 0 );
    ~exportCdcatDB();


    void fillMedia();
    QLabel *textLabel2;
    QListWidget *listAllMedia;
    QPushButton *buttonRemove;
    QPushButton *buttonAdd;
    QLabel *textLabel2_2;
    QListWidget *listSelectedMedia;
    QCheckBox *checkAllMedia;
    QCheckBox *checkOnlyMedia;
    QCheckBox *checkExportMediaName;
    QCheckBox *checkExportMediaNumber;
    QCheckBox *checkExportPath;
    QCheckBox *checkExportFile;
    QCheckBox *checkExportSize;
    QCheckBox *checkExportDate;
    QCheckBox *checkExportComment;
    QCheckBox *checkExportCategory;
    QCheckBox *checkExportMp3Tag;
    QCheckBox *checkExportBorrow;


    QCheckBox *checkExportTitle;
    QCheckBox *checkExportTableHeader;
    QGroupBox *buttonGroup1;
    QGroupBox *buttonGroupFields;
    QGridLayout *layoutGroup1;
    QGridLayout *layoutGroupFields;
    QRadioButton *radioHtml;
    QRadioButton *radioCsv;
    QRadioButton *radioXml;
    QLabel *seperatorLabel;
    QLabel *textLabel3;
    QLabel *spinColsLabel;
    QLineEdit *separatorInput;
    QLineEdit *fileName;
    QPushButton *buttonFile;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
    QSpinBox *spinCols;
    int OK;


protected:
    PWw *pww;
    QApplication *app;
    QGridLayout *exportCdcatDBLayout;
    QHBoxLayout *layoutAddRemove;
    QVBoxLayout *layoutAddRemoveCheckAll;
    QVBoxLayout *layout32;
    QHBoxLayout *layout21;
    QVBoxLayout *layout10_2;
    QVBoxLayout *layout19;
    QHBoxLayout *layout10;
    QVBoxLayout *layout10_2_2;
    QVBoxLayout *buttonGroup1Layout;
    QHBoxLayout *layout26;
    QHBoxLayout *layout25;
    QHBoxLayout *layout29;
    QHBoxLayout *layout4;
    QHBoxLayout *layout27;
    QVBoxLayout *typeLayout;
    QVBoxLayout *fieldsLayout;

    DataBase *p;
    QString separator;
    QFile f;
    gzFile ff;
    FileWriter *fw;
    int writeDown( Node *source );
    int  writeCatalog( Node *source );
    int  writeMedia( Node *source );
    int  writeDirectory( Node *source );
    int  writeFile( Node *source );
    int  writeMp3Tag( Node *source );
    int  writeCatLnk( Node *source );
    QString getFilePath( Node *current );

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
