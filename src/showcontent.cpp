/****************************************************************************
                             Hyper's CD Catalog
                A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "showcontent.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <q3textbrowser.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>

#include "icons.h"
#include "dbase.h"

/*
 *  Constructs a ShowContent as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ShowContent::ShowContent ( Node *node,QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    mynode = NULL;
    parentnode = node;

    if ( !name )
        setName ( "ShowContent" );
    setIcon ( *get_t_showc_icon() );

    ShowContentLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "ShowContentLayout" );

    layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );

    closeButton = new QPushButton ( this, "closeButton" );
    layout1->addWidget ( closeButton );
    QSpacerItem* spacer1 = new QSpacerItem ( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QSpacerItem* spacer2 = new QSpacerItem ( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    layout1->addItem ( spacer1 );

    QLabel *sizel = new QLabel ( this,"sizel" );
    sizel->setText ( tr ( "%1 kByte" ).arg ( 0 ) );

    layout1->addWidget ( sizel );
    layout1->addItem ( spacer2 );

    deleteButton = new QToolButton ( this, "deleteButton" );
    deleteButton->setPixmap ( *get_t_deletec_icon() );
    QToolTip::add ( deleteButton , tr ( "Remove the file content from the database. (Warning: Unrecoverable!)" ) );

    saveButton = new QToolButton ( this, "saveButton" );
    saveButton->setPixmap ( *get_t_save_icon() );
    QToolTip::add ( saveButton , tr ( "Save this content to a new file" ) );

    layout1->addWidget ( saveButton );
    layout1->addWidget ( deleteButton );

    ShowContentLayout->addLayout ( layout1, 0, 0 );

    textBrowser = new Q3TextBrowser ( this, "textBrowser" );

    if ( node != NULL && node->type == HC_FILE ) {
        setCaption ( tr ( "Content of %1" ).arg ( node->getNameOf() ) );
        mynode = ( ( DBFile * ) ( node->data ) )->prop;
        while ( mynode != NULL ) {
            if ( mynode->type == HC_CONTENT )
                break;
            mynode = mynode->next;
        }
    }
    if ( mynode != NULL ) {
        textBrowser->setText ( QString ( ( const char * ) ( ( DBContent * ) ( mynode->data ) )->bytes ) );
        sizel->setText ( tr ( "%1 kByte" ).arg ( ( ( DBContent * ) ( mynode->data ) )->storedSize / 1024.0 ) );
    }

    ShowContentLayout->addWidget ( textBrowser, 1, 0 );
    languageChange();
    resize ( QSize ( 413, 296 ).expandedTo ( minimumSizeHint() ) );
    connect ( closeButton,SIGNAL ( clicked() ),this,SLOT ( close() ) );
    connect ( deleteButton,SIGNAL ( clicked() ),this,SLOT ( deletenode() ) );
    connect ( saveButton,SIGNAL ( clicked() ),this,SLOT ( savenode() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ShowContent::~ShowContent() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ShowContent::languageChange() {

    closeButton->setText ( tr ( "Close" ) );
    deleteButton->setText ( QString::null );
}

int ShowContent::savenode ( void ) {
    FILE *f;
    QString fname,errormsg;

    if ( parentnode != NULL && parentnode->type == HC_FILE ) {
        mynode = ( ( DBFile * ) ( parentnode->data ) )->prop;
        while ( mynode != NULL ) {
            if ( mynode->type == HC_CONTENT )
                break;
            mynode = mynode->next;
        }
    } else return 0;
    if ( mynode == NULL ) return 0;

    fname =
        Q3FileDialog::getSaveFileName ( parentnode->getNameOf(),NULL,this,
                                        tr ( "Save content..." ),tr ( "Select a filename below" ) );
    if ( !fname.isEmpty() ) {
        f = fopen ( QFile::encodeName ( fname ),"w" );
        if ( f == NULL ) {
            errormsg = tr ( "I can't rewrite the file: %1" ).arg ( fname );
            QMessageBox::warning ( this,tr ( "Error while saving..." ),errormsg );
            return 0;
        }
        fseek ( f,0,SEEK_SET );
        fwrite ( ( ( DBContent * ) ( mynode->data ) )->bytes,sizeof ( char ),
                 ( ( DBContent * ) ( mynode->data ) )->storedSize,f );
        fclose ( f );
    }
    return 0;
}

int ShowContent::deletenode ( void ) {
    Node *tmp = NULL,*willdelete = NULL;

    if ( QMessageBox::warning ( this,tr ( "Confirmation" ),
                                //tr("Are you sure want to delete this file's content from the database?"),
                                tr ( "Sure to delete this file's content from the database?" ),
                                tr ( "Yes" ),tr ( "No" ) ) == 0 ) {
        if ( parentnode == NULL || parentnode->type != HC_FILE ) return 0;
        if ( ( tmp = ( ( DBFile * ) ( parentnode->data ) )->prop ) != NULL ) {
            if ( tmp->type == HC_CONTENT ) { //was the first child
                ( ( DBFile * ) ( parentnode->data ) )->prop = tmp->next;
                tmp->next = NULL;
                delete tmp;
            } else {
                while ( tmp->next != NULL ) {
                    if ( tmp->next->type == HC_CONTENT ) {
                        willdelete = tmp->next;
                        tmp->next = willdelete->next;
                        willdelete->next = NULL;
                        delete willdelete;
                    }
                    tmp = tmp->next;
                }
            }
        }

        textBrowser->setText ( "" );
        parentnode->touchDB();
    }
    return 0;
}



