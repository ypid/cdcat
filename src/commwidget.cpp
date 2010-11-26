/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "commwidget.h"

#include <qimage.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtoolbutton.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <q3multilineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qstringlist.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3GridLayout>
#include <QMouseEvent>
#include <QEvent>
#include <Q3VBoxLayout>

#include "dbase.h"
#include "cdcat.h"
#include "wdbfile.h"
#include "config.h"
#include "icons.h"
#include "mainwidget.h"
#include "showcontent.h"

#include <iostream>
using namespace std;




int  cshow = 1;
char *abuff=NULL;

HQToolButton::HQToolButton ( QWidget *parent )
        : QToolButton ( parent,"Button" ) {
    setMaximumSize ( 30,30 );
    setText ( QString::null );
}

void  HQToolButton::enterEvent ( QEvent *e ) {
    QApplication::setOverrideCursor ( Qt::arrowCursor );
}

void  HQToolButton::leaveEvent ( QEvent *e ) {
    QApplication::setOverrideCursor ( Qt::PointingHandCursor );
}

CommentWidget::CommentWidget ( CdCatConfig * cc,QApplication *appl,QWidget *parent,const char *name,Qt::WFlags fl )
        : QWidget ( parent,name,fl ) {
    if ( !name )
        setName ( "CommentWidget" );

    cconfig = cc;
    app  = appl;
    ox   = mx = 0;
    ox   = my = 0;
    mode = 0;
    act  = NULL;
    setMinimumSize ( 70,70 );

    ButtonContent = new HQToolButton ( this );
    ButtonContent->setPixmap ( *get_t_showc_icon() );
    ButtonContent->setGeometry ( 55,height()-45,30,30 );
    QToolTip::add ( ButtonContent, tr ( "Shows the content of the file." ) );

    ButtonEdit = new HQToolButton ( this );
    ButtonEdit->setPixmap ( *get_t_comment_icon() );
    ButtonEdit->setGeometry ( 20,height()-45,30,30 );
    QToolTip::add ( ButtonEdit   , tr ( "Edit and refresh the actual comment page." ) );

    connect ( ButtonEdit,SIGNAL ( clicked() ),this,SLOT ( editC() ) );
    connect ( ButtonContent,SIGNAL ( clicked() ),this,SLOT ( showC() ) );
}

void  CommentWidget::enterEvent ( QEvent *e ) {
    QApplication::setOverrideCursor ( Qt::PointingHandCursor );
}

void  CommentWidget::leaveEvent ( QEvent *e ) {
    QApplication::setOverrideCursor ( Qt::arrowCursor );
}

void  CommentWidget::mouseMoveEvent ( QMouseEvent *me ) {
    mx -= ox - me->x();
    my -= oy - me->y();
    ox = me->x();
    oy = me->y();
    repaint();
}

void  CommentWidget::mousePressEvent ( QMouseEvent *me ) {
    ox = me->x();
    oy = me->y();
    mx = 0;
    my = 0;
    repaint();
}

void  CommentWidget::mouseReleaseEvent ( QMouseEvent *me ) {
    ox = mx = 0;
    oy = my = 0;
    repaint();
}

void  CommentWidget::paintEvent ( QPaintEvent *pe ) {
//cerr <<"paintEvent"<<endl;
    QStringList textList;
    QString text;
    QDateTime mod;
    QPainter p ( this );
    int w=25;
    Node *tmp;
    QFontMetrics fm ( app->font() );
    int pixelsHigh = fm.height();
    int ispace=1;
    bool need_showc_button=false;

    pixelsHigh-=1; //I think it was too big...

    p.setPen ( *cconfig->comm_fr );
    p.fillRect ( 10,10,width()-20,height()-20,*cconfig->comm_bg/*QColor(255,229,28)*/ );
    p.drawRect ( 10,10,width()-20,height()-20 );
    p.setClipRect ( 10,10,width()-20,height()-20 );

    /*the .. case */
    if ( act != NULL ) {
        ButtonEdit->setEnabled ( true );

        if ( mode ) {
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Upper container! (..)" ) );
            w+=pixelsHigh+10;
        }
        /*name*/
        p.setPen ( *cconfig->comm_stext );
        p.drawText ( mx+15,my+w,tr ( "Name:" ) );
        w+=pixelsHigh;
        text=act->getNameOf();
        text.prepend ( " " );
        p.setPen ( *cconfig->comm_vtext );
        p.drawText ( mx+15,my+w,text );
        w+=pixelsHigh+ispace;
        /*type*/
        p.setPen ( *cconfig->comm_stext );
        p.drawText ( mx+15,my+w,tr ( "Type:" ) );
        w+=pixelsHigh;
        switch ( act->type ) {
        case HC_CATALOG  :
            text=tr ( " CdCat Catalog root" );
            break;
        case HC_CATLNK   :
            text=tr ( " Link to a CdCat catalog" );
            break;
        case HC_DIRECTORY:
            text=tr ( " Directory" );
            break;
        case HC_FILE     :
            text=tr ( " File" );
            break;
        case HC_MEDIA    :
            text=QString ( " " ) + tr ( "Media" ) + QString ( "" ).sprintf ( " (%s)",getMType ( ( ( DBMedia * ) ( act->data ) )->type ) );
            break;
        }
        p.setPen ( *cconfig->comm_vtext );
        p.drawText ( mx+15,my+w,text );
        w+=pixelsHigh+ispace;
        /*if file->size*/
        if ( act->type == HC_FILE ) {
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Size:" ) );
            w+=pixelsHigh;
            text.sprintf ( " %.2f %s", ( ( DBFile * ) ( act->data ) )->size
                           ,getSType ( ( ( DBFile * ) ( act->data ) )->sizeType ) );
            p.setPen ( *cconfig->comm_vtext );
            p.drawText ( mx+15,my+w,text );
            w+=pixelsHigh+ispace;
        }

        /*if catlnkfile->location*/
        if ( act->type == HC_CATLNK ) {
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Location:" ) );
            w+=pixelsHigh;
            p.setPen ( *cconfig->comm_vtext );
            text.sprintf ( " %s", ( ( DBCatLnk * ) ( act->data ) )->location );
            p.drawText ( mx+15,my+w,text );
            w+=pixelsHigh+ispace;
        }

        if ( act->type != HC_CATLNK ) {
            /*mod.time*/

            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Last modification:" ) );
            w+=pixelsHigh;
            switch ( act->type ) {
            case HC_CATALOG  :
                mod = ( ( DBCatalog   * ) ( act->data ) )->modification;
                break;
            case HC_DIRECTORY:
                mod = ( ( DBDirectory * ) ( act->data ) )->modification;
                break;
            case HC_FILE     :
                mod = ( ( DBFile      * ) ( act->data ) )->modification;
                break;
            case HC_MEDIA    :
                mod = ( ( DBMedia     * ) ( act->data ) )->modification;
                break;
            }

            //sprintf(text," %s",((DBMedia *)(act->data))->modification);break;
            text = date_to_str ( mod );

            p.setPen ( *cconfig->comm_vtext );
            p.drawText ( mx+15,my+w,text );
            w+=pixelsHigh+ispace;
            /*owner*/
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Owner:" ) );
            w+=pixelsHigh;
            tmp=act;
            while ( tmp !=NULL && tmp->type != HC_MEDIA && tmp->type != HC_CATALOG )
                tmp=tmp->parent;
            text=" ";
            if ( tmp==NULL ) text=tr ( "Unknown" );
            if ( tmp->type==HC_CATALOG )
                text+= ( ( DBCatalog * ) ( tmp->data ) )->owner;
            if ( tmp->type==HC_MEDIA )
                text+= ( ( DBMedia * ) ( tmp->data ) )->owner;

            p.setPen ( *cconfig->comm_vtext );
            p.drawText ( mx+15,my+w,text );
            w+=pixelsHigh+ispace;


            /*media*/
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Media (number/name):" ) );
            w+=pixelsHigh;
            tmp=act;
            while ( tmp !=NULL && tmp->type != HC_MEDIA && tmp->type != HC_CATALOG )
                tmp=tmp->parent;
            text=" ";
            if ( tmp==NULL )
                text+=tr ( "Unknown" );
            if ( tmp->type==HC_CATALOG )
                text+= ( ( DBCatalog * ) ( tmp->data ) )->name;
            if ( tmp->type==HC_MEDIA )
                text+= QString().setNum ( ( ( DBMedia * ) ( tmp->data ) )->number ) + " / " + ( ( DBMedia * ) ( tmp->data ) )->name;
            if ( ! ( ( ( DBMedia * ) ( tmp->data ) )->borrowing.isEmpty() ) ) {
                p.setPen ( QPen ( QColor ( Qt::red ),2 ) );
                p.drawLine ( width()-24,14,width()-14,24 );
                p.drawLine ( width()-14,14,width()-24,24 );

            }
            p.setPen ( QPen ( *cconfig->comm_vtext,1 ) );
            p.drawText ( mx+15,my+w,text );
            w+=pixelsHigh+ispace;
        }
        /*file properties*/
        if ( act->type == HC_FILE && ( ( DBFile * ) ( act->data ) )->prop != NULL ) { //File props:
            tmp= ( ( DBFile * ) ( act->data ) )->prop;
            while ( tmp != NULL ) {
                /*mp3tag*/
                if ( tmp->type == HC_MP3TAG ) {
                    p.setPen ( *cconfig->comm_fr );
                    p.drawLine ( 12,my+w-11,width()-12,my+w-11 );
                    p.setPen ( *cconfig->comm_stext );
                    p.drawText ( mx+12,my+w,tr ( "Mp3-Tag:" ) );
                    w+=pixelsHigh;
                    p.drawText ( mx+12,my+w,tr ( "(Art/Tit/Alb/Year/Comm)" ) );
                    w+=pixelsHigh+2;
                    p.setPen ( *cconfig->comm_vtext );
                    p.drawText ( mx+20,my+w, ( ( DBMp3Tag * ) ( tmp->data ) )->artist );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, ( ( DBMp3Tag * ) ( tmp->data ) )->title );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, ( ( DBMp3Tag * ) ( tmp->data ) )->album );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, ( ( DBMp3Tag * ) ( tmp->data ) )->year );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, ( ( DBMp3Tag * ) ( tmp->data ) )->comment );
                    w+=pixelsHigh+2;
                }
                tmp = tmp->next;
            }
        }
        p.setPen ( *cconfig->comm_fr );
        p.drawLine ( 12,my+w-pixelsHigh,width()-12,my+w-pixelsHigh );
        p.setPen ( *cconfig->comm_stext );
        w++;
        p.drawText ( mx+15,my+w,tr ( "Comment:" ) );
        w+=pixelsHigh;
        switch ( act->type ) {
        case HC_CATALOG  :
            text= ( ( DBCatalog   * ) ( act->data ) )->comment;
            break;
        case HC_DIRECTORY:
            text= ( ( DBDirectory * ) ( act->data ) )->comment;
            break;
        case HC_FILE     :
            text= ( ( DBFile      * ) ( act->data ) )->comment;
            break;
        case HC_MEDIA    :
            text= ( ( DBMedia     * ) ( act->data ) )->comment;
            break;
        case HC_CATLNK   :
            text= ( ( DBCatLnk    * ) ( act->data ) )->comment;
            break;
        }
        p.setPen ( *cconfig->comm_vtext );

        textList = QStringList::split ( QRegExp ( "#|\n" ),text,TRUE );
        for ( QStringList::Iterator it=textList.begin(); it != textList.end();++it ) {
            p.drawText ( mx+20,my+w, ( *it ) );
            w+=pixelsHigh;
        }


        /*Content button stuff*/
        if ( act->type == HC_FILE && ( ( DBFile * ) ( act->data ) )->prop != NULL ) {
            tmp= ( ( DBFile * ) ( act->data ) )->prop;
            while ( tmp != NULL ) {
                if ( tmp->type == HC_CONTENT ) {
                    need_showc_button = true;
                }
                tmp = tmp->next;
            }
        }
        /*others...*/
    } else {
        ButtonEdit->setEnabled ( false );
        p.setPen ( *cconfig->comm_stext );
        p.setPen ( *cconfig->comm_vtext );
        p.drawText ( mx+15,my+25,tr ( "There is no selected element." ) );
    }


    if ( !need_showc_button ) {
        ButtonContent->setEnabled ( false );
        //disconnect
    }

    if ( need_showc_button ) {
        ButtonContent->setEnabled ( true );
        //connect
    }
    ButtonContent->show();
    ButtonEdit->show();
    p.beginNativePainting();

    //delete text;
//cerr <<"paintEvent-end"<<endl;
}

void  CommentWidget::resizeEvent ( QResizeEvent *re ) {
    ButtonEdit   ->setGeometry ( 20, ( ( re->size() ).height() )-45,30,30 );
    ButtonContent->setGeometry ( 55, ( ( re->size() ).height() )-45,30,30 );
}

void CommentWidget::showNode ( Node *node,int mod ) {
  if (node->child && node->next) {
	 act  = node;
	 mode = mod;
	 repaint();
  }
}

int CommentWidget::editC ( void ) {
    editNodeComment ( act,this );
    emit touchdb();
    repaint();
    return 0;
}

int CommentWidget::showC ( void ) {
    ShowContent *sc = new ShowContent ( act,this,"showcw",true );
    sc->exec();
    emit touchdb();
    repaint();
    delete sc;
    return 0;
}

/****************************************************************************************/

commentEdit::commentEdit ( QString cc, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl )

{
    OK=0;
    if ( !name )
        setName ( "commentEdit" );
    setIcon ( *get_t_comment_icon() );

    setBaseSize ( QSize ( 300, 150 ) );
    setSizeGripEnabled ( TRUE );
    CommentEditBaseLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "CommentEditBaseLayout" );

    layout5 = new Q3VBoxLayout ( 0, 0, 6, "layout5" );

    teComm = new Q3MultiLineEdit ( this, "teComm" );
    layout5->addWidget ( teComm );

    layout4 = new Q3HBoxLayout ( 0, 0, 6, "layout4" );
    QSpacerItem* spacer = new QSpacerItem ( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem ( spacer );

    buttonOk = new QPushButton ( this, "buttonOk" );
    buttonOk->setMinimumSize ( QSize ( 80, 0 ) );
    buttonOk->setAutoDefault ( TRUE );
    buttonOk->setDefault ( TRUE );
    layout4->addWidget ( buttonOk );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setMinimumSize ( QSize ( 80, 0 ) );
    buttonCancel->setAutoDefault ( TRUE );
    layout4->addWidget ( buttonCancel );
    QSpacerItem* spacer_2 = new QSpacerItem ( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem ( spacer_2 );
    layout5->addLayout ( layout4 );

    CommentEditBaseLayout->addLayout ( layout5, 0, 0 );
    languageChange();
    resize ( QSize ( 275, 199 ).expandedTo ( minimumSizeHint() ) );

    // signals and slots connections
    connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( pushOk() ) );
    connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( pushCancel() ) );

    teComm->setText ( cc.replace ( QRegExp ( "#" ),"\n" ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
commentEdit::~commentEdit() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void commentEdit::languageChange() {
    setCaption ( tr ( "Edit Comment" ) );
    buttonCancel->setText ( tr ( "Cancel" ) );
    buttonOk->setText ( tr ( "OK" ) );
}

int commentEdit::pushOk ( void ) {
    OK = 1;
    newc = teComm->text();
    close();
    return 0;
}

int commentEdit::pushCancel ( void ) {
    OK = 0;
    close();
    return 0;
}

int editNodeComment ( Node *node,QWidget *parent ) {
    commentEdit *ce;
    QString o;

    if ( node == NULL ) return 0;

    switch ( node->type ) {
    case HC_CATALOG  :
        o = ( ( DBCatalog   * ) ( node->data ) )->comment;
        break;
    case HC_DIRECTORY:
        o = ( ( DBDirectory * ) ( node->data ) )->comment;
        break;
    case HC_FILE     :
        o = ( ( DBFile      * ) ( node->data ) )->comment;
        break;
    case HC_MEDIA    :
        o = ( ( DBMedia     * ) ( node->data ) )->comment;
        break;
    case HC_CATLNK   :
        o = ( ( DBCatLnk    * ) ( node->data ) )->comment;
        break;
    }

    ce = new commentEdit ( o,parent,"commentEdit",true );

    ce->exec();

    if ( ce->OK == 0 ) return 0;
    switch ( node->type ) {
    case HC_CATALOG  :
        ( ( DBCatalog * ) ( node->data ) )->comment   =
            ( ce->newc ).replace ( QRegExp ( "\n" ),"#" );
        break;

    case HC_DIRECTORY:
        ( ( DBDirectory * ) ( node->data ) )->comment =
            ( ce->newc ).replace ( QRegExp ( "\n" ),"#" );
        break;

    case HC_FILE     :
        ( ( DBFile * ) ( node->data ) )->comment      =
            ( ce->newc ).replace ( QRegExp ( "\n" ),"#" );
        break;

    case HC_MEDIA    :
        ( ( DBMedia * ) ( node->data ) )->comment     =
            ( ce->newc ).replace ( QRegExp ( "\n" ),"#" );
        break;

    case HC_CATLNK   :
        ( ( DBCatLnk * ) ( node->data ) )->comment    =
            ( ce->newc ).replace ( QRegExp ( "\n" ),"#" );
        break;
    }
    node->touchDB();
    return 0;
}


