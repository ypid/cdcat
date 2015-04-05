/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke (crissi99@gmx.de)
*  License   : GPL
*  Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#ifndef __HELPDIALOG_H__
#define __HELPDIALOG_H__

#include <QVariant>
#include <qdialog.h>
#include <qstring.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "guibase.h"

class QGridLayout;
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class QTextBrowser;
class QListView;
class QTableWidget;

class InfoDialog : public QDialog {
    Q_OBJECT

public:
    InfoDialog ( QWidget *parent = 0, const char *name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~InfoDialog();

    QPushButton *closeButton;
    QTabWidget *tabWidget2;
    QWidget *tab;
    QWidget *tab_2;
    QTextBrowser *textBrowser1;
    QTextBrowser *textBrowser2;
    QWidget *TabPage;
    QTextBrowser *textThanx;

protected:
    QGridLayout *helpDialogLayout;
    QGridLayout *tabLayout;
    QGridLayout *tabLayout_2;
    QGridLayout *TabPageLayout;
    QString infotext;
    QString contribution;
    QString license;

protected slots:
    virtual void languageChange();
};

class KeyBindingDialog : public QDialog {
    Q_OBJECT

public:
    KeyBindingDialog ( GuiSlave *gs, QWidget *parent = 0, const char *name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~KeyBindingDialog();

    QGridLayout *keyBindingDialogLayout;
    QLabel *TitleLabel;
    QPushButton *closeButton;
    QTableWidget *KeyBindingsTableWidget;

protected slots:
    virtual void languageChange();
};

#endif // HELPDIALOG_H
