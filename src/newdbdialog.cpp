/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "newdbdialog.h"
#include "dbase.h"
#include "cdcat.h"
#include "guibase.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QTextEdit>
#include <qlayout.h>
#include <qtooltip.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>

#include <stdio.h>
#include <string.h>

#include "icons.h"
/*
 *  Constructs a newdbdialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
newdbdialog::newdbdialog ( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl )

{
    if ( !name )
        setName ( "NewDBDialogBase" );
    setIcon ( *get_t_new_icon() );

    setSizeGripEnabled ( TRUE );
    NewDBDialogBaseLayout = new QGridLayout ( this, 1, 1, 11, 6, "NewDBDialogBaseLayout" );

    leOwner = new QLineEdit ( this, "leOwner" );
    NewDBDialogBaseLayout->addWidget ( leOwner, 1, 1 );

    leCategory = new QLineEdit ( this, "leCategory" );
    NewDBDialogBaseLayout->addWidget ( leCategory, 2, 1 );

    QSpacerItem* spacer = new QSpacerItem ( 20, 132, QSizePolicy::Minimum, QSizePolicy::Expanding );
    NewDBDialogBaseLayout->addItem ( spacer, 3, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem ( 241, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    NewDBDialogBaseLayout->addItem ( spacer_2, 4, 0 );

    teComm = new QTextEdit ( this, "teComm" );

    NewDBDialogBaseLayout->addMultiCellWidget ( teComm, 3, 3, 1, 1 );

    textLabel2 = new QLabel ( this, "textLabel2" );

    NewDBDialogBaseLayout->addWidget ( textLabel2, 1, 0 );

    leName = new QLineEdit ( this, "leName" );

    NewDBDialogBaseLayout->addWidget ( leName, 0, 1 );

    textLabel3 = new QLabel ( this, "textLabel3" );

    NewDBDialogBaseLayout->addWidget ( textLabel3, 3, 0 );


    textLabelCategory = new QLabel ( this, "textLabelCategory" );

    NewDBDialogBaseLayout->addWidget ( textLabelCategory, 2, 0 );

    layout12 = new QHBoxLayout ( 0, 0, 6, "layout12" );
    QSpacerItem* spacer_3 = new QSpacerItem ( 140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem ( spacer_3 );

    buttonOK = new QPushButton ( this, "buttonOK" );
    buttonOK->setMinimumSize ( QSize ( 100, 0 ) );
    buttonOK->setAutoDefault ( TRUE );
    buttonOK->setDefault ( TRUE );
    layout12->addWidget ( buttonOK );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setMinimumSize ( QSize ( 100, 0 ) );
    buttonCancel->setAutoDefault ( TRUE );
    layout12->addWidget ( buttonCancel );
    QSpacerItem* spacer_4 = new QSpacerItem ( 170, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem ( spacer_4 );

    NewDBDialogBaseLayout->addMultiCellLayout ( layout12, 6, 6, 0, 1 );

    textLabel1 = new QLabel ( this, "textLabel1" );

    NewDBDialogBaseLayout->addWidget ( textLabel1, 0, 0 );
    languageChange();
    resize ( QSize ( 411, 221 ).expandedTo ( minimumSizeHint() ) );

    connect ( buttonOK,SIGNAL ( clicked() ),this,SLOT ( ok() ) );
    connect ( buttonCancel,SIGNAL ( clicked() ),this,SLOT ( can() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
newdbdialog::~newdbdialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void newdbdialog::languageChange() {
    setCaption ( tr ( "DataBase's Properties" ) );
    textLabel1->setText ( tr ( "DataBase Name:" ) );
    textLabel2->setText ( tr ( "DataBase Owner:" ) );
    textLabel3->setText ( tr ( "Comment:" ) );
    textLabelCategory->setText ( tr ( "Category:" ) );
    leName->setText ( tr ( "Catalog" ) );
    leOwner->setText ( tr ( "hcat-user" ) );
    buttonOK->setText ( tr ( "OK" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
    teComm->setText ( tr ( " " ) );
}


int newdbdialog::ok ( void ) {
    OK = 1;
    nameD  = leName->text();
    ownerD = leOwner->text();
    commD  = teComm->text();
    categoryD  = leCategory->text();
    close();
    return 0;
}

int newdbdialog::can ( void ) {
    OK = 0;
    close();
    return 0;
}

/********************************************************************************************
* RENAME dialogbox
********************************************************************************************/

renamedialog::renamedialog ( QWidget *parent,Node *ppatient,GuiSlave *pslave )
        : QDialog ( parent,"renamedialog",true ) {
    patient = ppatient;
    slave   = pslave;

    setName ( "renamedialog" );

    label = new QLabel ( this, "textLabel1" );
    label->setGeometry ( QRect ( 10, 10, 250, 20 ) );

    leText = new QLineEdit ( this, "leName" );
    leText->setGeometry ( QRect ( 10, 32, 250, 20 ) );


    okB = new QPushButton ( this, "buttonOK" );
    okB->setGeometry ( QRect ( 20, 60, 98, 25 ) );

    caB = new QPushButton ( this, "buttonCancel" );
    caB->setGeometry ( QRect ( 150, 60, 98, 25 ) );

    languageChange();
    resize ( QSize ( 283, 100 ).expandedTo ( minimumSizeHint() ) );
    setMaximumSize ( 283, 100 );
    setMinimumSize ( 283, 100 );

    connect ( okB,SIGNAL ( clicked() ),this,SLOT ( ok() ) );
    connect ( caB,SIGNAL ( clicked() ),this,SLOT ( can() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
renamedialog::~renamedialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void renamedialog::languageChange() {
    setCaption ( tr ( "Rename node..." ) );
    label->setText ( tr ( "Give the new name:" ) );
    okB->setText ( tr ( "OK" ) );
    caB->setText ( tr ( "Cancel" ) );

    leText->setText ( patient->getNameOf() );
    if ( patient->type == HC_CATLNK ) {
        QString s ( patient->getNameOf() );
        leText->setText ( s.replace ( 0,1,"" ) );
    } else
        leText->setText ( patient->getNameOf() );
}


int renamedialog::ok ( void ) {
    if ( ( leText->text() ).isEmpty() ||  QString(leText->text()) == QString(patient->getNameOf())  ) {
        close();
        return 0;
    }

    if ( patient->type == HC_MEDIA ) {
        if (  !slave->isIdentical ( leText->text() ) ) {
            QMessageBox::warning ( this,tr ( "Error:" ),tr ( "The new (media) name must be unique!" ) );
            return 0;
        }

        if ( ( leText->text() ).startsWith ( "@" ) ) {
            QMessageBox::warning ( this,tr ( "Error:" ),tr ( "The new media name can't starts with \"@\" !" ) );
            return 0;
        }
    }


    switch ( patient->type ) {
    case HC_CATALOG:
        ( ( DBCatalog * ) ( patient->data ) )->name = leText->text();
        break;
    case HC_MEDIA:
        ( ( DBMedia * ) ( patient->data ) )->name = leText->text();
        break;
    case HC_DIRECTORY:
        ( ( DBDirectory * ) ( patient->data ) )->name = leText->text();
        break;
    case HC_FILE:
        ( ( DBFile * ) ( patient->data ) )->name = leText->text();
        break;
    case HC_CATLNK:
        ( ( DBCatLnk * ) ( patient->data ) )->name = ( leText->text() ).prepend ( "@" );
        break;
    default:  close(); return 0;
    }
    patient->touchDB();
    close();
    return 0;
}

int renamedialog::can ( void ) {
    close();
    return 0;
}

/********************************************************************************************
* RENUMBER (media) dialogbox
********************************************************************************************/

renumberdialog::renumberdialog ( QWidget *parent,Node *ppatient,GuiSlave *pslave )
        : QDialog ( parent,"renumberdialog",true ) {
    patient = ppatient;
    slave   = pslave;

    if ( patient->type != HC_MEDIA ) {
        QMessageBox::warning ( this,tr ( "Error:" ),tr ( "The element is NOT a Media!" ) );
        patient = NULL;
    }

    setName ( "renumberdialog" );

    label = new QLabel ( this, "textLabel1" );
    label->setGeometry ( QRect ( 10, 10, 250, 20 ) );

    leText = new QLineEdit ( this, "leName" );
    leText->setGeometry ( QRect ( 10, 32, 250, 20 ) );

    okB = new QPushButton ( this, "buttonOK" );
    okB->setGeometry ( QRect ( 20, 60, 98, 25 ) );

    caB = new QPushButton ( this, "buttonCancel" );
    caB->setGeometry ( QRect ( 150, 60, 98, 25 ) );

    languageChange();
    resize ( QSize ( 283, 100 ).expandedTo ( minimumSizeHint() ) );
    setMaximumSize ( 283, 100 );
    setMinimumSize ( 283, 100 );
    connect ( okB,SIGNAL ( clicked() ),this,SLOT ( ok() ) );
    connect ( caB,SIGNAL ( clicked() ),this,SLOT ( can() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
renumberdialog::~renumberdialog() {
    // no need to delete child widgets, Qt does it all for us
}
/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void renumberdialog::languageChange() {
    setCaption ( tr ( "Re-Number media..." ) );
    label->setText ( tr ( "Give the new serial number:" ) );
    okB->setText ( tr ( "OK" ) );
    caB->setText ( tr ( "Cancel" ) );
    if ( patient != NULL ) {
        QString num;
        num.setNum ( ( ( DBMedia * ) ( patient->data ) )->number );
        leText->setText ( num );
    }
}

int renumberdialog::ok ( void ) {
    bool ok;
    int newnumber;

    if ( patient == NULL  ||
            ( leText->text() ).isEmpty() ) {
        close();
        return 0;
    }

    newnumber = ( leText->text() ).toInt ( &ok,10 );

    if ( !ok ) {
        QMessageBox::warning ( this,tr ( "Error:" ),tr ( "Please enter number value!" ) );
        return 0;
    }

    if (( ( DBMedia * ) ( patient->data ) )->number == newnumber) {
	close();
	return 0;
    }

    if (  !slave->isIdentical ( newnumber ) ) {
        QMessageBox::warning ( this,tr ( "Error:" ),tr ( "The new media-number must be unique!" ) );
        return 0;
    }

    ( ( DBMedia * ) ( patient->data ) )->number = newnumber;
    patient->touchDB();
    close();
    return 0;
}

int renumberdialog::can ( void ) {
    close();
    return 0;
}

