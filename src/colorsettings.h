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
#include <q3frame.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <QPaintEvent>

#include "config.h"

class Q3VBoxLayout;
class QColor;
class Q3HBoxLayout;
class Q3GridLayout;
class QComboBox;
class Q3GroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QPaintEvent;

class ColorPreview : public Q3Frame {
    Q_OBJECT

public:
    QColor c;
    ColorPreview ( QWidget *parent );
    void updColor ( void );

protected:
    void paintEvent ( QPaintEvent *event );

};

class ColorSchemePreview : public Q3Frame {
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
    Q3GroupBox* groupBox1;
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
    Q3VBoxLayout* ColorSettingsLayout;
    Q3HBoxLayout* layout14;
    Q3HBoxLayout* groupBox1Layout;
    Q3VBoxLayout* layout4;
    Q3HBoxLayout* layout1;
    Q3HBoxLayout* layout2;
    Q3HBoxLayout* layout3;
    Q3VBoxLayout* layout13;
    Q3HBoxLayout* layout11;
    Q3VBoxLayout* layout9;

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
