/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "newdbdialog.h"

#include "dbase.h"
#include "cdcat.h"
#include "guibase.h"
#include "icons.h"

#include <stdio.h>
#include <string.h>

#ifdef CATALOG_ENCRYPTION
    #include <crypto++/blowfish.h>
#endif

#include <QVariant>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLayout>
#include <QToolTip>
#include <QMessageBox>
#include <QGridLayout>
#include <QHBoxLayout>

/*
 *  Constructs a newdbdialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
newdbdialog::newdbdialog ( QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {

    setModal( modal );
    if (!name) {
        setObjectName( "NewDBDialogBase" );
    }
    setWindowIcon( *get_t_new_icon());

    setSizeGripEnabled( true );
    NewDBDialogBaseLayout = new QGridLayout( this );
    textLabelOwner = new QLabel( this );
    leOwner = new QLineEdit( this );
    NewDBDialogBaseLayout->addWidget( textLabelOwner, 1, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( leOwner, 1, 1, 1, 1 );

    textLabelCategory = new QLabel( this );
    leCategory = new QLineEdit( this );
    NewDBDialogBaseLayout->addWidget( textLabelCategory, 2, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( leCategory, 2, 1, 1, 1 );

    QSpacerItem *spacer = new QSpacerItem( 20, 132, QSizePolicy::Minimum, QSizePolicy::Expanding );
    NewDBDialogBaseLayout->addItem( spacer, 3, 0 );
    QSpacerItem *spacer_2 = new QSpacerItem( 241, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    NewDBDialogBaseLayout->addItem( spacer_2, 4, 0 );

    textLabelComment = new QLabel( this );
    teComm = new QTextEdit( this );
    NewDBDialogBaseLayout->addWidget( textLabelComment, 3, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( teComm, 3, 1, 1, 1 );

    textLabelDatabase = new QLabel( this );
    leName = new QLineEdit( this );
    NewDBDialogBaseLayout->addWidget( textLabelDatabase, 4, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( leName, 4, 1, 1, 1 );

#ifdef CATALOG_ENCRYPTION
    textLabelEncryption = new QLabel( this );
    cbEncryption = new QCheckBox( this );
    NewDBDialogBaseLayout->addWidget( textLabelEncryption, 5, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( cbEncryption, 5, 1, 1, 1 );

    textLabelEncryptionPassword = new QLabel( this );
    leEncryptionPassword = new QLineEdit( this );
    leEncryptionPassword->setEchoMode( QLineEdit::Password );
    NewDBDialogBaseLayout->addWidget( textLabelEncryptionPassword, 6, 0, 1, 1 );
    NewDBDialogBaseLayout->addWidget( leEncryptionPassword, 6, 1, 1, 1 );

    catalogEncrypted = false;
    encryptionPassword = "";
#endif

    layout12 = new QHBoxLayout( this );
    QSpacerItem *spacer_3 = new QSpacerItem( 140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem( spacer_3 );

    buttonOK = new QPushButton( this );
    buttonOK->setMinimumSize( QSize( 100, 0 ));
    buttonOK->setAutoDefault( true );
    buttonOK->setDefault( true );
    layout12->addWidget( buttonOK );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumSize( QSize( 100, 0 ));
    buttonCancel->setAutoDefault( true );
    layout12->addWidget( buttonCancel );
    QSpacerItem *spacer_4 = new QSpacerItem( 170, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem( spacer_4 );

    NewDBDialogBaseLayout->addLayout( layout12, 7, 0, 2, 2 );


    languageChange();
    resize( QSize( 411, 221 ).expandedTo( minimumSizeHint()));

    connect( buttonOK, SIGNAL( clicked()), this, SLOT( ok()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( can()));
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
    setWindowTitle( tr( "Database's Properties" ));
    textLabelDatabase->setText( tr( "Database Name:" ));
    textLabelOwner->setText( tr( "Database Owner:" ));
    textLabelComment->setText( tr( "Comment:" ));
    textLabelCategory->setText( tr( "Category:" ));
    leName->setText( tr( "Catalog" ));
    leOwner->setText( tr( "hcat-user" ));
    buttonOK->setText( tr( "OK" ));
    buttonCancel->setText( tr( "Cancel" ));
    teComm->setText( tr( " " ));
#ifdef CATALOG_ENCRYPTION
    textLabelEncryption->setText( tr( "encrypt catalog" ));
    textLabelEncryptionPassword->setText( tr( "password for catalog" ));
#endif
}


int newdbdialog::ok( void ) {
    OK = 1;
    nameD = leName->text();
    ownerD = leOwner->text();
    commD = teComm->toPlainText();
    categoryD = leCategory->text();
#ifdef CATALOG_ENCRYPTION
    if (cbEncryption->isChecked()) {
        if (leEncryptionPassword->text().size() < 4) {
            QMessageBox::warning( this, tr( "Password too short" ), tr( "Password length is too short, must be minimum 4 chars" ));
            return 1;
        }
        if (leEncryptionPassword->text().size() > CryptoPP::Blowfish::BLOCKSIZE) {
            QMessageBox::warning( this,
                tr( "Password too big" ),
                tr( "Password length is too big, must be maximal %1 chars" )
                    .arg( QString()
                    .setNum( CryptoPP::Blowfish::BLOCKSIZE ))
            );
            return 1;
        }
    }
    catalogEncrypted = cbEncryption->isChecked();
    encryptionPassword = leEncryptionPassword->text();
#endif
    close();
    return 0;
}

int newdbdialog::can( void ) {
    OK = 0;
    close();
    return 0;
}

/********************************************************************************************
* RENAME dialogbox
********************************************************************************************/

renamedialog::renamedialog ( QWidget *parent, Node *ppatient, GuiSlave *pslave )
    : QDialog( parent ) {
    patient = ppatient;
    slave = pslave;
    setModal( true );

    setObjectName( "renamedialog" );

    label = new QLabel( this );
    label->setGeometry( QRect( 10, 10, 250, 20 ));

    leText = new QLineEdit( this );
    leText->setGeometry( QRect( 10, 32, 250, 20 ));


    okB = new QPushButton( this );
    okB->setGeometry( QRect( 20, 60, 98, 25 ));

    caB = new QPushButton( this );
    caB->setGeometry( QRect( 150, 60, 98, 25 ));

    languageChange();
    resize( QSize( 283, 100 ).expandedTo( minimumSizeHint()));
    setMaximumSize( 283, 100 );
    setMinimumSize( 283, 100 );

    connect( okB, SIGNAL( clicked()), this, SLOT( ok()));
    connect( caB, SIGNAL( clicked()), this, SLOT( can()));
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
    setWindowTitle( tr( "Rename node …" ));
    label->setText( tr( "Give the new name:" ));
    okB->setText( tr( "OK" ));
    caB->setText( tr( "Cancel" ));

    leText->setText( patient->getNameOf());
    if (patient->type == HC_CATLNK) {
        QString s( patient->getNameOf());
        leText->setText( s.replace( 0, 1, "" ));
    } else {
        leText->setText( patient->getNameOf());
    }
}


int renamedialog::ok( void ) {
    if ((leText->text()).isEmpty() || QString( leText->text()) == QString( patient->getNameOf())) {
        close();
        return 0;
    }

    if (patient->type == HC_MEDIA) {
        if (slave->isIdentical( leText->text())) {
            QMessageBox::warning( this, tr( "Error:" ), tr( "The new (media) name must be unique!" ));
            return 0;
        }

        if ((leText->text()).startsWith( "@" )) {
            QMessageBox::warning( this, tr( "Error:" ), tr( "The new media name can't starts with \"@\" !" ));
            return 0;
        }
    }


    switch (patient->type) {
    case HC_CATALOG:
        ((DBCatalog *)(patient->data))->name = leText->text();
        break;
    case HC_MEDIA:
        ((DBMedia *)(patient->data))->name = leText->text();
        break;
    case HC_DIRECTORY:
        ((DBDirectory *)(patient->data))->name = leText->text();
        break;
    case HC_FILE:
        ((DBFile *)(patient->data))->name = leText->text();
        break;
    case HC_CATLNK:
        ((DBCatLnk *)(patient->data))->name = (leText->text()).prepend( "@" );
        break;
    default:
        close();
        return 0;
    }
    patient->touchDB();
    close();
    return 0;
}

int renamedialog::can( void ) {
    close();
    return 0;
}

/********************************************************************************************
* RENUMBER (media) dialogbox
********************************************************************************************/

renumberdialog::renumberdialog ( QWidget *parent, Node *ppatient, GuiSlave *pslave )
    : QDialog( parent ) {
    patient = ppatient;
    slave = pslave;
    setModal( true );

    if (patient->type != HC_MEDIA) {
        QMessageBox::warning( this, tr( "Error:" ), tr( "The element is NOT a Media!" ));
        patient = NULL;
    }

    setObjectName( "renumberdialog" );

    label = new QLabel( this );
    label->setGeometry( QRect( 10, 10, 250, 20 ));

    leText = new QLineEdit( this );
    leText->setGeometry( QRect( 10, 32, 250, 20 ));

    okB = new QPushButton( this );
    okB->setGeometry( QRect( 20, 60, 98, 25 ));

    caB = new QPushButton( this );
    caB->setGeometry( QRect( 150, 60, 98, 25 ));

    languageChange();
    resize( QSize( 283, 100 ).expandedTo( minimumSizeHint()));
    setMaximumSize( 283, 100 );
    setMinimumSize( 283, 100 );
    connect( okB, SIGNAL( clicked()), this, SLOT( ok()));
    connect( caB, SIGNAL( clicked()), this, SLOT( can()));
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
    setWindowTitle( tr( "Re-Number media …" ));
    label->setText( tr( "Give the new serial number:" ));
    okB->setText( tr( "OK" ));
    caB->setText( tr( "Cancel" ));
    if (patient != NULL) {
        QString num;
        num.setNum(((DBMedia *)(patient->data))->number );
        leText->setText( num );
    }
}

int renumberdialog::ok( void ) {
    bool ok;
    int newnumber;

    if (patient == NULL ||
        (leText->text()).isEmpty()) {
        close();
        return 0;
    }

    newnumber = (leText->text()).toInt( &ok, 10 );

    if (!ok) {
        QMessageBox::warning( this, tr( "Error:" ), tr( "Please enter number value!" ));
        return 0;
    }

    if (((DBMedia *)(patient->data))->number == newnumber) {
        close();
        return 0;
    }

    if (slave->isIdentical( newnumber )) {
        QMessageBox::warning( this, tr( "Error:" ), tr( "The new media-number must be unique!" ));
        return 0;
    }

    ((DBMedia *)(patient->data))->number = newnumber;
    patient->touchDB();
    close();
    return 0;
}

int renumberdialog::can( void ) {
    close();
    return 0;
}
