/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "showcontent.h"

#include <QVariant>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <QToolButton>
#include <QLayout>
#include <QToolTip>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <QGridLayout>
#include <QHBoxLayout>

#include "icons.h"
#include "dbase.h"
#include "wdbfile.h"

/*
 *  Constructs a ShowContent as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
ShowContent::ShowContent ( Node *node, bool isCategory, CdCatConfig *cconfig, QWidget *parent, const char *name, bool modal, Qt::WFlags fl )
    : QDialog( parent, fl ) {
    this->cconfig = cconfig;
    mynode = NULL;
    parentnode = node;
    this->isCategory = isCategory;

    if (!name) {
        setObjectName( "ShowContent" );
    }
    setModal( modal );
    setWindowIcon( *get_t_showc_icon());

    ShowContentLayout = new QGridLayout( this );

    layout1 = new QHBoxLayout( this );

    closeButton = new QPushButton( this );
    layout1->addWidget( closeButton );
    QSpacerItem *spacer1 = new QSpacerItem( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    QSpacerItem *spacer2 = new QSpacerItem( 240, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    layout1->addItem( spacer1 );

    QLabel *sizel = new QLabel( this );
    sizel->setText( tr( "%1 kByte" ).arg( 0 ));

    layout1->addWidget( sizel );
    layout1->addItem( spacer2 );

    comboFont = new QFontComboBox( this );
    layout1->addWidget( comboFont );

    deleteButton = new QToolButton( this );
    deleteButton->setIcon( QIcon( *get_t_deletec_icon()));
    deleteButton->setToolTip( tr( "Remove the file content from the database. (Warning: Unrecoverable!)" ));

    saveButton = new QToolButton( this );
    saveButton->setIcon( QIcon( *get_t_save_icon()));
    saveButton->setToolTip( tr( "Save this content to a new file" ));

    layout1->addWidget( saveButton );
    layout1->addWidget( deleteButton );

    ShowContentLayout->addLayout( layout1, 0, 0 );

    textBrowser = new QTextBrowser( this );
    if (!cconfig->ContentViewerFont.isEmpty()) {
        QFont contentFont = QFont();
        contentFont.fromString( cconfig->ContentViewerFont );
        textBrowser->setCurrentFont( contentFont );
        comboFont->setCurrentFont( contentFont );
    }

    if (isCategory) {
        if (node != NULL) {
            setWindowTitle( tr( "Category of %1" ).arg( node->getNameOf()));

            QString o;
            switch (node->type) {
            case HC_CATALOG:
                o = ((DBCatalog *)(node->data))->category;
                break;
            case HC_DIRECTORY:
                o = ((DBDirectory *)(node->data))->category;
                break;
            case HC_FILE:
                o = ((DBFile *)(node->data))->category;
                break;
            case HC_MEDIA:
                o = ((DBMedia *)(node->data))->category;
                break;
            case HC_CATLNK:
                o = ((DBCatLnk *)(node->data))->category;
                break;
            }
            textBrowser->setPlainText( o );
        }
    } else {
        if (node != NULL && node->type == HC_FILE) {
            setWindowTitle( tr( "Content of %1" ).arg( node->getNameOf()));
            mynode = ((DBFile *)(node->data))->prop;
            while (mynode != NULL) {
                if (mynode->type == HC_CONTENT) {
                    break;
                }
                mynode = mynode->next;
            }
        }
        if (mynode != NULL) {
            textBrowser->setPlainText( QString((const char *)((DBContent *)(mynode->data))->bytes ));
            QString sizestr = "";
            double size = ((DBContent *)(mynode->data))->storedSize;

            float s;
            int st = UNIT_BYTE;

            if (size >= SIZE_ONE_GBYTE * 1024) {
                s = size / SIZE_ONE_GBYTE / 1024;
                st = UNIT_TBYTE;
            } else {
                if (size >= SIZE_ONE_GBYTE && size < SIZE_ONE_GBYTE * 1024) {
                    s = size / SIZE_ONE_GBYTE;
                    st = UNIT_GBYTE;
                } else {
                    if (size >= SIZE_ONE_MBYTE && size < SIZE_ONE_GBYTE) {
                        s = size / SIZE_ONE_MBYTE;
                        st = UNIT_MBYTE;
                    } else {
                        if (size >= SIZE_ONE_KBYTE && size < SIZE_ONE_MBYTE) {
                            s = size / SIZE_ONE_KBYTE;
                            st = UNIT_KBYTE;
                        } else {
                            s = size;
                            st = UNIT_BYTE;
                        }
                    }
                }
            }
            sizestr.sprintf( "%.2f", (s));
            QString filetype = " " + getSType( st, true );
            sizestr = sizestr + filetype;
            sizel->setText( sizestr );
        }
    }
    ShowContentLayout->addWidget( textBrowser, 1, 0 );
    languageChange();
    resize( QSize( cconfig->contentWindowSize_width, cconfig->contentWindowSize_height ).expandedTo( minimumSizeHint()));
    move( cconfig->contentWindowPos_x, cconfig->contentWindowPos_y );
    connect( closeButton, SIGNAL( clicked()), this, SLOT( close()));
    if (isCategory) {
        deleteButton->hide();
        saveButton->hide();
    } else {
        connect( deleteButton, SIGNAL( clicked()), this, SLOT( deletenode()));
        connect( saveButton, SIGNAL( clicked()), this, SLOT( savenode()));
    }
    connect( comboFont, SIGNAL( currentFontChanged( const QFont & )), this, SLOT( setCurrentFont( const QFont & )));
}

/*
 *  Destroys the object and frees any allocated resources
 */
ShowContent::~ShowContent() {
    bool need_save = false;

    if (x() != cconfig->contentWindowPos_x || y() != cconfig->contentWindowPos_y || width() != cconfig->contentWindowSize_width || cconfig->contentWindowSize_height || cconfig->ContentViewerFont != textBrowser->currentFont().toString()) {
        need_save = true;
    }
    cconfig->contentWindowPos_x = x();
    cconfig->contentWindowPos_y = y();
    cconfig->contentWindowSize_width = width();
    cconfig->contentWindowSize_height = height();
    cconfig->ContentViewerFont = textBrowser->currentFont().toString();
    if (need_save) {
        cconfig->writeConfig();
    }
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ShowContent::languageChange() {
    if (isCategory) {
        setWindowTitle( tr( "Set category" ));
    } else {
        setWindowTitle( tr( "Set content" ));
    }
    comboFont->setToolTip( tr( "select font for display" ));
    closeButton->setText( tr( "Close" ));
    deleteButton->setText( QString::null );
}

int ShowContent::savenode( void ) {
    FILE *f;
    QString fname, errormsg;

    if (isCategory) {
        return 0;
    } else {
        if (parentnode != NULL && parentnode->type == HC_FILE) {
            mynode = ((DBFile *)(parentnode->data))->prop;
            while (mynode != NULL) {
                if (mynode->type == HC_CONTENT) {
                    break;
                }
                mynode = mynode->next;
            }
        } else {
            return 0;
        }
        if (mynode == NULL) {
            return 0;
        }

        fname = QFileDialog::getSaveFileName( 0, tr( "Select a filename below" ), parentnode->getNameOf(), tr( "CdCat databases (*.hcf )" ));
        if (!fname.isEmpty()) {
            f = fopen( QFile::encodeName( fname ), "w" );
            if (f == NULL) {
                errormsg = tr( "I can't rewrite the file: %1" ).arg( fname );
                QMessageBox::warning( this, tr( "Error while saving..." ), errormsg );
                return 0;
            }
            fseek( f, 0, SEEK_SET );
            fwrite(((DBContent *)(mynode->data))->bytes, sizeof(char),
                   ((DBContent *)(mynode->data))->storedSize, f );
            fclose( f );
        }
        return 0;
    }
}

int ShowContent::deletenode( void ) {
    Node *tmp = NULL, *willdelete = NULL;

    if (isCategory) {
        return 0;
    } else {
        if (QMessageBox::warning( this, tr( "Confirmation" ),
                                  // tr("Are you sure want to delete this file's content from the database?"),
                                  tr( "Sure to delete this file's content from the database?" ),
                                  tr( "Yes" ), tr( "No" )) == 0) {
            if (parentnode == NULL || parentnode->type != HC_FILE) {
                return 0;
            }
            if ((tmp = ((DBFile *)(parentnode->data))->prop) != NULL) {
                if (tmp->type == HC_CONTENT) {                   // was the first child
                    ((DBFile *)(parentnode->data))->prop = tmp->next;
                    tmp->next = NULL;
                    delete tmp;
                } else {
                    while (tmp->next != NULL) {
                        if (tmp->next->type == HC_CONTENT) {
                            willdelete = tmp->next;
                            tmp->next = willdelete->next;
                            willdelete->next = NULL;
                            delete willdelete;
                        }
                        tmp = tmp->next;
                    }
                }
            }

            textBrowser->setText( "" );
            parentnode->touchDB();
        }
        return 0;
    }
}


void ShowContent::setCurrentFont( const QFont &font ) {
    // std::cout << "setCurrentFont" << std::endl;
    QString text = textBrowser->toPlainText();

    textBrowser->setCurrentFont( font );
    textBrowser->setPlainText( text );
}
