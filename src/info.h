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
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class Q3TextBrowser;

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
    Q3TextBrowser* textBrowser1;
    QWidget* TabPage;
    Q3TextBrowser* textThanx;

protected:
    Q3GridLayout* helpDialogLayout;
    Q3GridLayout* tabLayout;
    Q3GridLayout* tabLayout_2;
    Q3GridLayout* TabPageLayout;
    QString infotext;
    QString contribution;
    QString license;

protected slots:
    virtual void languageChange();

};

#endif // HELPDIALOG_H
