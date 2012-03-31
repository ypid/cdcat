/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Christoph Thielecke (crissi99@gmx.de)
 License   : GPL
 Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#ifndef __HELPDIALOG_H__
#define __HELPDIALOG_H__

#include <qvariant.h>
#include <qdialog.h>
#include <qstring.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QGridLayout;
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class QTextBrowser;

class InfoDialog : public QDialog {
    Q_OBJECT

public:
    InfoDialog ( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~InfoDialog();

    QPushButton* closeButton;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QLabel* textLabel1;
    QWidget* tab_2;
    QTextBrowser* textBrowser1;
    QWidget* TabPage;
    QTextBrowser* textThanx;

protected:
    QGridLayout* helpDialogLayout;
    QGridLayout* tabLayout;
    QGridLayout* tabLayout_2;
    QGridLayout* TabPageLayout;
    QString infotext;
    QString contribution;
    QString license;

protected slots:
    virtual void languageChange();

};

#endif // HELPDIALOG_H
