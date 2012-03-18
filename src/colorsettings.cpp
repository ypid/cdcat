/****************************************************************************
** Form implementation generated from reading ui file 'colorsettings.ui'
**
** Created: p júl 2 19:17:41 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.1   edited Nov 21 17:40 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "colorsettings.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <QFrame>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPaintEvent>
#include <Q3VBoxLayout>

#include "icons.h"


ColorPreview::ColorPreview ( QWidget *parent )
        : QFrame ( parent,"cpv" ) {
    c = QColor ( 0,0,0 );
}

ColorSchemePreview::ColorSchemePreview ( QWidget *parent )
        : QFrame ( parent,"cspv" ) {
    setMinimumHeight ( 75 );

    ts = QColor ( 0,0,0 );
    td = QColor ( 0,0,0 );
    bg = QColor ( 0,0,0 );
    fr = QColor ( 0,0,0 );
}

void ColorPreview::updColor ( void ) {
    repaint();
}

void ColorSchemePreview::updColor ( void ) {
    repaint();
}


void ColorPreview::paintEvent ( QPaintEvent *event ) {
    QPainter p ( this );
    p.fillRect ( 0,0,width(),height(),c );
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
    p.beginNativePainting();
#endif
}

void ColorSchemePreview::paintEvent ( QPaintEvent *event ) {
    QPainter p ( this );
    p.setPen ( fr );
    p.fillRect ( 10,10,width()-20,height()-20,bg );
    p.drawRect ( 10,10,width()-20,height()-20 );
    p.setClipRect ( 10,10,width()-20,height()-20 );

    p.setPen ( ts );
    p.drawText ( 20,30,tr ( "Title:" ) );
    p.setPen ( td );
    p.drawText ( 25,50,tr ( "Road To Hell" ) );
    p.setPen ( fr );

    p.drawLine ( 10,65,width()-11,65 );

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
    p.beginNativePainting();
#endif
}




ColorSettings::ColorSettings ( CdCatConfig *cfgp, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    if ( !name )
        setName ( "ColorSettings" );

    cfg=cfgp;
    setIcon ( *get_t_colorconfig_icon() );
    setSizeGripEnabled ( TRUE );
    ColorSettingsLayout = new Q3VBoxLayout ( this, 11, 6, "ColorSettingsLayout" );

    preview = new ColorSchemePreview ( this );
    preview->setFrameShape ( QFrame::StyledPanel );
    preview->setFrameShadow ( QFrame::Raised );
    ColorSettingsLayout->addWidget ( preview );

    itemselector = new QComboBox ( FALSE, this, "itemselector" );
    ColorSettingsLayout->addWidget ( itemselector );

    layout14 = new Q3HBoxLayout ( 0, 0, 6, "layout14" );

    groupBox1 = new Q3GroupBox ( this, "groupBox1" );
    groupBox1->setColumnLayout ( 0, Qt::Vertical );
    groupBox1->layout()->setSpacing ( 6 );
    groupBox1->layout()->setMargin ( 11 );
    groupBox1Layout = new Q3HBoxLayout ( groupBox1->layout() );
    groupBox1Layout->setAlignment ( Qt::AlignTop );

    layout4 = new Q3VBoxLayout ( 0, 0, 6, "layout4" );

    layout1 = new Q3HBoxLayout ( 0, 0, 6, "rspin" );

    textLabel1 = new QLabel ( groupBox1, "textLabel1" );
    layout1->addWidget ( textLabel1 );

    rspin = new QSpinBox ( groupBox1, "spinBox1" );
    layout1->addWidget ( rspin );
    layout4->addLayout ( layout1 );

    layout2 = new Q3HBoxLayout ( 0, 0, 6, "layout2" );

    textLabel1_2 = new QLabel ( groupBox1, "textLabel1_2" );
    layout2->addWidget ( textLabel1_2 );

    gspin = new QSpinBox ( groupBox1, "gspin" );
    layout2->addWidget ( gspin );
    layout4->addLayout ( layout2 );

    layout3 = new Q3HBoxLayout ( 0, 0, 6, "layout3" );

    textLabel1_3 = new QLabel ( groupBox1, "textLabel1_3" );
    layout3->addWidget ( textLabel1_3 );

    bspin = new QSpinBox ( groupBox1, "bspin" );
    layout3->addWidget ( bspin );
    layout4->addLayout ( layout3 );
    groupBox1Layout->addLayout ( layout4 );

    colorpreview = new ColorPreview ( groupBox1 );
    colorpreview->setMinimumSize ( QSize ( 40, 0 ) );
    colorpreview->setMaximumSize ( QSize ( 40, 32767 ) );
    colorpreview->setFrameShape ( QFrame::StyledPanel );
    colorpreview->setFrameShadow ( QFrame::Raised );
    groupBox1Layout->addWidget ( colorpreview );
    layout14->addWidget ( groupBox1 );

    layout13 = new Q3VBoxLayout ( 0, 0, 6, "layout13" );
    QSpacerItem* spacer = new QSpacerItem ( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout13->addItem ( spacer );

    layout11 = new Q3HBoxLayout ( 0, 0, 6, "layout11" );
    QSpacerItem* spacer_2 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout11->addItem ( spacer_2 );

    layout9 = new Q3VBoxLayout ( 0, 0, 6, "layout9" );

    buttonOk = new QPushButton ( this, "buttonOk" );
    buttonOk->setMinimumSize ( QSize ( 100, 0 ) );
    layout9->addWidget ( buttonOk );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setMinimumSize ( QSize ( 100, 0 ) );
    layout9->addWidget ( buttonCancel );
    layout11->addLayout ( layout9 );
    QSpacerItem* spacer_3 = new QSpacerItem ( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout11->addItem ( spacer_3 );
    layout13->addLayout ( layout11 );
    layout14->addLayout ( layout13 );
    ColorSettingsLayout->addLayout ( layout14 );
    languageChange();
    resize ( QSize ( 429, 324 ).expandedTo ( minimumSizeHint() ) );


    rspin->setMaxValue ( 255 );
    gspin->setMaxValue ( 255 );
    bspin->setMaxValue ( 255 );

    rspin->setLineStep ( 10 );
    gspin->setLineStep ( 10 );
    bspin->setLineStep ( 10 );

    itemselector->setCurrentItem ( 0 );

    bg= *cfg->comm_bg;
    ts= *cfg->comm_stext;
    td= *cfg->comm_vtext;
    fr= *cfg->comm_fr;
    changedcurr ( 3 );
    changedcurr ( 2 );
    changedcurr ( 1 );
    changedcurr ( 0 );
    change();

    connect ( rspin,SIGNAL ( valueChanged ( int ) ),this,SLOT ( changedr ( int ) ) );
    connect ( gspin,SIGNAL ( valueChanged ( int ) ),this,SLOT ( changedg ( int ) ) );
    connect ( bspin,SIGNAL ( valueChanged ( int ) ),this,SLOT ( changedb ( int ) ) );

    connect ( itemselector,SIGNAL ( activated ( int ) ),this,SLOT ( changedcurr ( int ) ) );

    connect ( buttonOk,SIGNAL ( clicked() ),this,SLOT ( sok() ) );
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( scancel() ) );
}


/*
 *  Destroys the object and frees any allocated resources
 */
ColorSettings::~ColorSettings() {
    // no need to delete child widgets, Qt does it all for us
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ColorSettings::languageChange() {
    setCaption ( tr ( "ColorSettings" ) );
    itemselector->clear();
    itemselector->insertItem ( tr ( "Comment area background and the file higlighting line" ) );
    itemselector->insertItem ( tr ( "Frame of comment area" ) );
    itemselector->insertItem ( tr ( "Comment window static text (Program text)" ) );
    itemselector->insertItem ( tr ( "Comment window variable text (Data)" ) );
    groupBox1->setTitle ( tr ( "color" ) );
    textLabel1->setText ( tr ( "Red:" ) );
    textLabel1_2->setText ( tr ( "Green:" ) );
    textLabel1_3->setText ( tr ( "Blue:" ) );
    buttonOk->setText ( tr ( "Ok" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
}

int ColorSettings::changedcurr ( int v ) {
    int r=0,g=0,b=0;
    current = v;

    switch ( current ) {
    case 0: r=bg.red(); g=bg.green(); b=bg.blue();     break;
    case 1: r=fr.red(); g=fr.green(); b=fr.blue();     break;
    case 2: r=ts.red(); g=ts.green(); b=ts.blue();     break;
    case 3: r=td.red(); g=td.green(); b=td.blue();     break;
    };

    rspin->setValue ( r );
    gspin->setValue ( g );
    bspin->setValue ( b );
    change();
    return 0;
}

int ColorSettings::changedr ( int v ) {
    int r=0,g=0,b=0;
    switch ( current ) {
    case 0: r=bg.red(); g=bg.green(); b=bg.blue();  bg.setRgb ( v,g,b );   break;
    case 1: r=fr.red(); g=fr.green(); b=fr.blue();  fr.setRgb ( v,g,b );   break;
    case 2: r=ts.red(); g=ts.green(); b=ts.blue();  ts.setRgb ( v,g,b );   break;
    case 3: r=td.red(); g=td.green(); b=td.blue();  td.setRgb ( v,g,b );   break;
    };
    change();
    return 0;
}

int ColorSettings::changedg ( int v ) {
    int r=0,g=0,b=0;
    switch ( current ) {
    case 0: r=bg.red(); g=bg.green(); b=bg.blue();  bg.setRgb ( r,v,b );   break;
    case 1: r=fr.red(); g=fr.green(); b=fr.blue();  fr.setRgb ( r,v,b );   break;
    case 2: r=ts.red(); g=ts.green(); b=ts.blue();  ts.setRgb ( r,v,b );   break;
    case 3: r=td.red(); g=td.green(); b=td.blue();  td.setRgb ( r,v,b );   break;
    };
    change();
    return 0;
}

int ColorSettings::changedb ( int v ) {
    int r=0,g=0,b=0;
    switch ( current ) {
    case 0: r=bg.red(); g=bg.green(); b=bg.blue();  bg.setRgb ( r,g,v );   break;
    case 1: r=fr.red(); g=fr.green(); b=fr.blue();  fr.setRgb ( r,g,v );   break;
    case 2: r=ts.red(); g=ts.green(); b=ts.blue();  ts.setRgb ( r,g,v );   break;
    case 3: r=td.red(); g=td.green(); b=td.blue();  td.setRgb ( r,g,v );   break;
    };
    change();
    return 0;
}


int ColorSettings::change ( void ) {
    switch ( current ) {
    case 0: colorpreview->c=bg;   break;
    case 1: colorpreview->c=fr;   break;
    case 2: colorpreview->c=ts;   break;
    case 3: colorpreview->c=td;   break;
    };

    colorpreview->updColor();

    preview->bg=bg;
    preview->ts=ts;
    preview->td=td;
    preview->fr=fr;
    preview->updColor();

    return 0;
}

int ColorSettings::sok ( void ) {
    *cfg->comm_bg    = bg;
    *cfg->comm_stext = ts;
    *cfg->comm_vtext = td;
    *cfg->comm_fr    = fr;
    cfg->writeConfig();
    close();
    return 0;
}

int ColorSettings::scancel ( void ) {
    close();
    return 0;
}


