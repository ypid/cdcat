
/****************************************************************************
** Form interface generated from reading ui file 'colorsettings.ui'
**
** Created: p júl 2 19:17:12 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef COLORSETTINGS_H
#define COLORSETTINGS_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPaintEvent>

#include "config.h"

class QVBoxLayout;
class QColor;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QPaintEvent;

class ColorPreview : public QFrame {
    Q_OBJECT

public:
    QColor c;
    ColorPreview ( QWidget *parent );
    void updColor ( void );

protected:
    void paintEvent ( QPaintEvent *event );

};

class ColorSchemePreview : public QFrame {
    Q_OBJECT

public:
    QColor bg,ts,td,fr;
    ColorSchemePreview ( QWidget *parent );
    void updColor ( void );

protected:
    void paintEvent ( QPaintEvent *event );

};


class ColorSettings : public QDialog {
    Q_OBJECT

public:
    CdCatConfig *cfg;
    QColor bg,ts,td,fr;
    int current;

    ColorSettings ( CdCatConfig *cfgp, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~ColorSettings();


    ColorSchemePreview* preview;
    QComboBox* itemselector;
    QGroupBox* groupBox1;
    QLabel* textLabel1;
    QSpinBox* rspin;
    QLabel* textLabel1_2;
    QSpinBox* gspin;
    QLabel* textLabel1_3;
    QSpinBox* bspin;
    ColorPreview* colorpreview;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

protected:
    QVBoxLayout* ColorSettingsLayout;
    QHBoxLayout* layout14;
    QHBoxLayout* groupBox1Layout;
    QVBoxLayout* layout4;
    QHBoxLayout* layout1;
    QHBoxLayout* layout2;
    QHBoxLayout* layout3;
    QVBoxLayout* layout13;
    QHBoxLayout* layout11;
    QVBoxLayout* layout9;

protected slots:
    virtual void languageChange();
private:
    QPixmap image0;

public slots:
    int changedr ( int v );
    int changedg ( int v );
    int changedb ( int v );
    int changedcurr ( int v );

    int change ( void );

    int sok ( void );
    int scancel ( void );

};

#endif // COLORSETTINGS_H
