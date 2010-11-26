/****************************************************************************
                         Hyper's CD Catalog
         A multiplatform qt and xml based catalog program

      Author    : Peter Deak (hyperr@freemail.hu)
      License   : GPL
      Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "selreadable.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include "config.h"
#include "dbase.h"
#include "icons.h"

SelReadable::SelReadable ( CdCatConfig *confp,QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl )

{
    conf = confp;
    if ( !name )
        setName ( "SelReadable" );
    setIcon ( *get_t_config_icon() );

    SelReadableLayout = new Q3VBoxLayout ( this, 11, 6, "SelReadableLayout" );

    setSizeGripEnabled ( TRUE );

    cbTag = new QCheckBox ( this, "cbTag" );
    SelReadableLayout->addWidget ( cbTag );

    tagselector = new Q3ButtonGroup ( 2,Qt::Horizontal,this,"TagSelectorG" );
    tagselector->setRadioButtonExclusive ( true );
    rad_v1 = new QRadioButton ( tagselector );
    rad_v2 = new QRadioButton ( tagselector );

    layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );
    layout1->addSpacing ( 25 );
    layout1->addWidget ( tagselector );
    layout1->addSpacing ( 25 );

    SelReadableLayout->addLayout ( layout1 );

    cbInfo = new QCheckBox ( this, "cbInfo" );
    SelReadableLayout->addWidget ( cbInfo );

    line0 = new Q3Frame ( this, "line0" );
    line0->setFrameShape ( Q3Frame::HLine );
    line0->setFrameShadow ( Q3Frame::Sunken );
    line0->setFrameShape ( Q3Frame::HLine );
    SelReadableLayout->addWidget ( line0 );

    cbaInfo = new QCheckBox ( this, "cbaInfo" );
    SelReadableLayout->addWidget ( cbaInfo );

    line1 = new Q3Frame ( this, "line1" );
    line1->setFrameShape ( Q3Frame::HLine );
    line1->setFrameShadow ( Q3Frame::Sunken );
    line1->setFrameShape ( Q3Frame::HLine );
    SelReadableLayout->addWidget ( line1 );

    cbCont = new QCheckBox ( this, "cbCont" );
    SelReadableLayout->addWidget ( cbCont );

    layout12 = new Q3HBoxLayout ( 0, 0, 6, "layout12" );
    QSpacerItem* spacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem ( spacer );

    layout11 = new Q3VBoxLayout ( 0, 0, 6, "layout11" );

    layout9 = new Q3HBoxLayout ( 0, 0, 6, "layout9" );

    lineFiles = new QLineEdit ( this, "lineFiles" );
    lineFiles->setMinimumWidth ( 150 );
    layout9->addWidget ( lineFiles );

    textLabel1 = new QLabel ( this, "textLabel1" );
    layout9->addWidget ( textLabel1 );
    layout11->addLayout ( layout9 );

    layout10 = new Q3HBoxLayout ( 0, 0, 6, "layout10" );

    maxSpinBox = new QSpinBox ( 1,MAX_STORED_SIZE/1024,1,this, "maxSpinBox" );
    layout10->addWidget ( maxSpinBox );

    textLabel2 = new QLabel ( this, "textLabel2" );
    layout10->addWidget ( textLabel2 );
    layout11->addLayout ( layout10 );
    layout12->addLayout ( layout11 );
    SelReadableLayout->addLayout ( layout12 );

    line2 = new Q3Frame ( this, "line2" );
    line2->setFrameShape ( Q3Frame::HLine );
    line2->setFrameShadow ( Q3Frame::Sunken );
    line2->setFrameShape ( Q3Frame::HLine );
    SelReadableLayout->addWidget ( line2 );

    layout3 = new Q3HBoxLayout ( 0, 0, 6, "layout3" );
    QSpacerItem* spacer_2 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem ( spacer_2 );

    buttonOK = new QPushButton ( this, "buttonOK" );
    buttonOK->setMinimumSize ( QSize ( 80, 0 ) );
    layout3->addWidget ( buttonOK );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setMinimumSize ( QSize ( 80, 0 ) );
    layout3->addWidget ( buttonCancel );
    QSpacerItem* spacer_3 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem ( spacer_3 );
    SelReadableLayout->addLayout ( layout3 );
    languageChange();
    resize ( QSize ( 410, 216 ).expandedTo ( minimumSizeHint() ) );

    connect ( cbTag,SIGNAL ( stateChanged ( int ) ),this,SLOT ( schanged ( int ) ) );
    connect ( cbCont,SIGNAL ( stateChanged ( int ) ),this,SLOT ( schanged ( int ) ) );
    connect ( buttonOK,SIGNAL ( clicked() ),this,SLOT ( sok() ) );
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( scan() ) );

    cbTag->setChecked ( conf->readtag );
    cbCont->setChecked ( conf->readcontent );
    lineFiles->setText ( conf->readcfiles );
    maxSpinBox->setValue ( ( int ) ( conf->readclimit / 1024 ) );
    cbInfo->setChecked ( conf->readinfo );
    cbaInfo->setChecked ( conf->readavii );
    rad_v1->setChecked ( conf->v1_over_v2 );
    rad_v2->setChecked ( !conf->v1_over_v2 );

    schanged ( 0 );
}

int SelReadable::schanged ( int state ) {
    if ( cbCont->isChecked() ) {
        lineFiles->setEnabled ( true );
        maxSpinBox->setEnabled ( true );
    } else {
        lineFiles->setEnabled ( false );
        maxSpinBox->setEnabled ( false );
    }

    if ( cbTag->isChecked() )
        tagselector->setEnabled ( true );
    else
        tagselector->setEnabled ( false );

    return 0;
}

int SelReadable::sok ( void ) {

    conf->readcontent = cbCont->isChecked();
    conf->readcfiles  = lineFiles->text();
    conf->readclimit  = maxSpinBox->value() *1024;
    conf->readinfo    = cbInfo->isChecked();
    conf->readavii    = cbaInfo->isChecked();
    conf->readtag     = cbTag->isChecked();
    conf->v1_over_v2  = rad_v1->isChecked();

    close();
    return 0;
}

int SelReadable::scan ( void ) {
    close();
    return 0;
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelReadable::~SelReadable() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelReadable::languageChange() {
    setCaption ( tr ( "Select readable items" ) );
    cbTag->setText ( tr ( "Read mp3 tags" ) );

    tagselector->setTitle ( tr ( "Default tag" ) );
    rad_v1->setText ( tr ( "version" ) + " 1" );
    rad_v2->setText ( tr ( "version" ) + " 2" );

    cbInfo->setText ( tr ( "Read mp3 technical info as comment (bitrate,freq,length...)" ) );
    cbaInfo->setText ( tr ( "Read avi technical info as comment (codecs,length,...)" ) );
    cbCont->setText ( tr ( "Store content of some files" ) );
    lineFiles->setText ( "*.nfo;*.dzi" );
    textLabel1->setText ( tr ( "; separated list of readable file patterns" ) );
    textLabel2->setText ( tr ( "content size limit in kByte" ) );
    buttonOK->setText ( tr ( "Ok" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
}

