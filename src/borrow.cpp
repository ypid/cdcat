/****************************************************************************
*                        Hyper's CD Catalog
*        A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <iostream>

#include "borrow.h"
#include "icons.h"
#include "dbase.h"
#include "cdcat.h"

#include <QDebug>
#include <QVariant>
#include <QLabel>
#include <QPoint>
#include <QMenu>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QToolTip>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>

char *buff = NULL;

borrowDialog::borrowDialog ( QString mname, QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {

    ok = 0;
    setModal( modal );
    setSizeGripEnabled( true );
    setWindowIcon( *get_t_sborrow_icon());

    borrowDialogLayout = new QVBoxLayout( this );

    textLabel = new QLabel( this );
    borrowDialogLayout->addWidget( textLabel );

    leWho = new QLineEdit( this );
    borrowDialogLayout->addWidget( leWho );
    QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    borrowDialogLayout->addItem( spacer );

    layout4 = new QHBoxLayout();
    QSpacerItem *spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_2 );

    buttonOk = new QPushButton( this );
    buttonOk->setMinimumSize( QSize( 80, 0 ));
    layout4->addWidget( buttonOk );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumSize( QSize( 80, 0 ));
    layout4->addWidget( buttonCancel );
    QSpacerItem *spacer_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_3 );
    borrowDialogLayout->addLayout( layout4 );
    languageChange();
    resize( QSize( 421, 113 ).expandedTo( minimumSizeHint()));


    textLabel->setText( tr( "I borrow the \"%1\" named media to:" ).arg( mname ));

    connect( buttonOk, SIGNAL( clicked()), this, SLOT( sok()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( scancel()));
}

int borrowDialog::sok( void ) {
    ok = 1;
    if ((leWho->text()).isEmpty()) {
        m = tr( "unknown" );
    } else {
        m = leWho->text();
    }
    close();
    return 0;
}


int borrowDialog::scancel( void ) {
    ok = 0;
    close();
    return 0;
}

/*
 *  Destroys the object and frees any allocated resources
 */
borrowDialog::~borrowDialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void borrowDialog::languageChange() {
    setWindowTitle( tr( "Borrowing …" ));
    textLabel->setText( tr( "I borrow the \"\" named media to:" ));
    buttonOk->setText( tr( "Ok" ));
    buttonCancel->setText( tr( "Cancel" ));
}


/****************************************************************************
** CLASS borrowingDialog
****************************************************************************/

borrowingDialog::borrowingDialog ( DataBase *dbp, QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    setModal( modal );
    last_row_clicked = -1;
    ch = 0;
    db = dbp;
    this->parent = parent;
    if (!name) {
        setWindowTitle( "borrowingDialog" );
    }
    setSizeGripEnabled( true );
    setWindowIcon( *get_m_borrow_icon());
    borrowingDialogLayout = new QVBoxLayout( this );

    layout1 = new QHBoxLayout( this );
    QSpacerItem *spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer );

    textLabel = new QLabel( this );
    layout1->addWidget( textLabel );
    QSpacerItem *spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer_2 );
    borrowingDialogLayout->addLayout( layout1 );
    QSpacerItem *spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    borrowingDialogLayout->addItem( spacer_3 );

    layout3 = new QHBoxLayout( this );

    cbOnlyBorrowed = new QCheckBox( this );
    layout3->addWidget( cbOnlyBorrowed );
    QSpacerItem *spacer_4 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer_4 );

    buttonClear = new QPushButton( this );
    layout3->addWidget( buttonClear );
    borrowingDialogLayout->addLayout( layout3 );

    table = new QTableWidget( this );

    borrowingDialogLayout->addWidget((QWidget *)table );

    layout4 = new QHBoxLayout( this );
    QSpacerItem *spacer_5 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_5 );

    buttonOk = new QPushButton( this );
    buttonOk->setMinimumSize( QSize( 80, 0 ));
    layout4->addWidget( buttonOk );

    // buttonCancel = new QPushButton( this );
    // buttonCancel->setMinimumSize( QSize( 80, 0 ) );
    // layout4->addWidget( buttonCancel );
    QSpacerItem *spacer_6 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_6 );
    borrowingDialogLayout->addLayout( layout4 );
    table->setFocusPolicy( Qt::ClickFocus );
    languageChange();

    resize( QSize( 600, 500 ).expandedTo( minimumSizeHint()));

    connect( buttonOk, SIGNAL( clicked()), this, SLOT( sok()));
    // connect(buttonCancel,SIGNAL(clicked()),this,SLOT(scancel()));
    connect( table, SIGNAL(  cellChanged( int, int )), this, SLOT( schanged( int, int )));
    connect( cbOnlyBorrowed, SIGNAL( clicked()), this, SLOT( sonlyb()));
    connect( buttonClear, SIGNAL( clicked()), this, SLOT( sclear()));
    connect( table, SIGNAL( itemClicked(  QTableWidgetItem * )), this, SLOT( setCurrentItem( QTableWidgetItem *  )));

    table->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( table, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( click( const QPoint & )));
    filltable();
}

/*
 *  Destroys the object and frees any allocated resources
 */
borrowingDialog::~borrowingDialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void borrowingDialog::languageChange() {
    setWindowTitle( tr( "Borrowing info …" ));
    textLabel->setText( tr( "Media borrowing info:" ));
    cbOnlyBorrowed->setText( tr( "Show only borrowed items" ));
    buttonClear->setText( tr( "Clear all borrowing info" ));
    QStringList headerTitleList;
    headerTitleList.append( tr( "Media" ));
    headerTitleList.append( tr( "Borrowed" ));
    headerTitleList.append( tr( "where is it now?" ));
    table->setHorizontalHeaderLabels( headerTitleList );

    buttonOk->setText( tr( "Ok" ));
    // buttonCancel->setText( tr( "Cancel" ) );
}


int borrowingDialog::sok() {
    if (ch) {
        sstore();
    }
    close();
    return 0;
}

int borrowingDialog::scancel() {
    close();
    return 0;
}

int borrowingDialog::filltable() {
    if (db == NULL) {
        return 0;
    }

    int num = 0;
    Node *tmp = NULL;

    table->setRowCount( 0 );
    table->setColumnCount( 3 );
    QStringList headerTitleList;
    headerTitleList.append( tr( "Media" ));
    headerTitleList.append( tr( "Borrowed" ));
    headerTitleList.append( tr( "where is it now?" ));
    table->setHorizontalHeaderLabels( headerTitleList );

    table->setRowCount( table->rowCount() + 1 );

    tmp = (db->getRootNode())->child;
    for (num = 0; tmp != NULL; tmp = tmp->next) {
        if (tmp->type != HC_MEDIA) {         // numRowsThis is not a media node
            continue;
        }

        if (cbOnlyBorrowed->isChecked() && ((DBMedia *)(tmp->data))->borrowing.isEmpty()) {
            continue;
        }

        table->setRowCount( num + 1 );
        // table->verticalHeaderItem(num)->setText (QString ( "%1" ).arg ( num + 1 ) );

        table->setItem( num, 0, new QTableWidgetItem( tmp->getNameOf()));
        table->item( num, 0 )->setFlags( Qt::ItemIsEditable );

        table->setItem( num, 1, new QTableWidgetItem(((DBMedia *)(tmp->data))->borrowing.isEmpty() ? tr( "No" ) : tr( "Yes" )));
        table->item( num, 0 )->setFlags( Qt::ItemIsEditable );

        if (((DBMedia *)(tmp->data))->borrowing.isEmpty()) {
            table->setItem( num, 2, new QTableWidgetItem( "" ));
            // table->item(num, 2)->setFlags(Qt::ItemIsEditable);
            Qt::ItemFlags eFlags = table->item( num, 2 )->flags();
            eFlags |= Qt::ItemIsEditable;
            table->item( num, 2 )->setFlags( eFlags );
        } else {
            table->setItem( num, 2, new QTableWidgetItem(((DBMedia *)(tmp->data))->borrowing ));
            // table->item(num, 2)->setFlags(Qt::ItemIsEditable);
            Qt::ItemFlags eFlags = table->item( num, 2 )->flags();
            eFlags |= Qt::ItemIsEditable;
            table->item( num, 2 )->setFlags( eFlags );
        }

        // table->updateCell ( num, 0 );
        // table->updateCell ( num, 1 );
        // table->updateCell ( num, 2 );

        num++;
    }

    table->setSortingEnabled( false );
    // table->setSelectionMode ( QTableWidget::NoSelection );

    return 0;
}

int borrowingDialog::schanged( int row, int col ) {
    QString newv;

    ch = 1;

    newv = table->item( row, col )->text();

    if (col == 1) {       // Yes or No
        if (newv.toLower() == tr( "Yes" ).toLower() || newv.toLower() == tr( "No" ).toLower()) {
            if (newv.toLower() == tr( "No" ).toLower()) {
                table->item( row, 1 )->setText( tr( "No" ));
                if (table->item( row, 2 ) != NULL) {
                    table->item( row, 2 )->setText( "-" );
                }
            } else {             // Yes
                table->item( row, 1 )->setText( tr( "Yes" ));
                if (table->item( row, 2 ) != NULL) {
                    if (table->item( row, 2 )->text().isEmpty()) {
                        table->item( row, 2 )->setText( tr( "unknown" ));
                    }
                }
            }
        } else {
            QMessageBox::warning( this, tr( "Error" ), tr( "Set \"Yes\" or \"No\" !" ));
            if (table->item( row, 2 ) != NULL) {
                if (table->item( row, 2 )->text().isEmpty()) {
                    table->item( row, 1 )->setText( tr( "No" ));
                } else {
                    table->item( row, 1 )->setText( tr( "Yes" ));
                }
            }
        }
    }

    if (col == 2) {       // Man name, or place
        if (newv.isEmpty()) {
            table->item( row, 1 )->setText( tr( "No" ));
        } else {
            table->item( row, 1 )->setText( tr( "Yes" ));
        }
    }

    return 0;
}

int borrowingDialog::sonlyb( void ) {
    // if (ch) {
        // if (0 == QMessageBox::warning( this, tr( "Warning" ), tr( "Do you save the changes?" ), tr( "Yes" ), tr( "No" ))) {
            sstore();
        // }
    // }

    filltable();
    return 0;
}

int borrowingDialog::sstore( void ) {
    Node *tmp = (db->getRootNode())->child;
    int i;
    int isborrowed;

    tmp->touchDB();
    for (; tmp != NULL; tmp = tmp->next) {
        if (tmp->type != HC_MEDIA) {         // This is not a media node.
            continue;
        }

        isborrowed = 0;
        for (i = 0; i < table->rowCount(); i++) {
            if (tmp->getNameOf() == table->item( i, 0 )->text() &&
                (table->item( i, 1 )->text()).toLower() == tr( "Yes" ).toLower()) {
                isborrowed = 1;
                if (!(((DBMedia *)(tmp->data))->borrowing.isEmpty())) {
                    ((DBMedia *)(tmp->data))->borrowing = "";
                }
                ((DBMedia *)(tmp->data))->borrowing = table->item( i, 2 )->text();
            }
        }
        if (!isborrowed) {
            if (!(((DBMedia *)(tmp->data))->borrowing.isEmpty())) {
                ((DBMedia *)(tmp->data))->borrowing = "";
            }
        }
    }

    ch = 0;
    return 0;
}

int borrowingDialog::sclear( void ) {
    if (0 == QMessageBox::warning( this, tr( "Warning" ), tr( "Are you sure want to clear all borrow?" ), tr( "Yes" ), tr( "No" ))) {
        Node *tmp = (db->getRootNode())->child;

        tmp->touchDB();
        for (; tmp != NULL; tmp = tmp->next) {
            if (tmp->type != HC_MEDIA) {             // This is not a media node.
                continue;
            }

            if (!(((DBMedia *)(tmp->data))->borrowing.isEmpty())) {
                ((DBMedia *)(tmp->data))->borrowing = "";
            }
        }
        filltable();
    }
    return 0;
}

int borrowingDialog::click( const QPoint &mousePos ) {
    //  int row, int col, const QPoint& mousePos
    int i, j, b = 0;
    QString s;
    QTableWidgetItem *item = table->itemAt( mousePos );
    int col = table->column( item );
    int row = table->row( item );

    if (/*button != 2 ||*/ col != 2) {
        return 0;
    }


    QMenu *pm = new QMenu( table );
    QMenu *pma = new QMenu( table );
    QAction *back_action = new QAction( this );
    back_action->setText( tr( "I got it back!" ));
    pm->addAction( back_action );
    connect( back_action, SIGNAL( triggered( bool )), this, SLOT( click_clr()));
    connect( pma, SIGNAL( triggered( bool )), pma, SLOT( open( bool )));
    pma->setTitle( tr( "<< " ));
    pm->addMenu( pma );

    for (i = 0; i < table->rowCount(); i++) {
        s = table->item( i, 2 )->text();
        b = 0;
        if (s.isEmpty()) {
            continue;
        }
        for (j = i - 1; j >= 0; j--)
            if (s == table->item( j, 2 )->text()) {
                b = 1;
            }
        if (!b) {
            qDebug() << "borrowingDialog::addtem: " << qPrintable( s );
            QAction *a = new QAction( pma );
            a->setText( s );
            pma->addAction( a );
        }
    }

    connect( pma, SIGNAL( activated( int )), this, SLOT( click_set( int )));
    last_row_clicked = row;
    pm->exec( QCursor::pos());
    return 0;
}

int borrowingDialog::click_clr( void ) {
    table->item( last_row_clicked, 2 )->setText( "" );
    table->item( last_row_clicked, 1 )->setText( tr( "No" ));
    ch = 1;
    return 0;
}

int borrowingDialog::click_set( int a ) {
    table->item( last_row_clicked, 2 )->setText( table->item( a, 2 )->text());
    table->item( last_row_clicked, 1 )->setText( tr( "Yes" ));
    ch = 1;
    return 0;
}


void borrowingDialog::setCurrentItem( QTableWidgetItem *item ) {
    int row = item->row();

    table->item( row, 0 )->setSelected( true );
    table->item( row, 1 )->setSelected( true );
    table->item( row, 2 )->setSelected( true );
}



// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
