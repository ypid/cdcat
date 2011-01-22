/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <string.h>

#include <q3listview.h>
#include <qimage.h>
#include <qlabel.h>
#include <q3header.h>
#include <qpainter.h>

#include <qregexp.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <QFileDialog>
#include <QFileInfo>
#include <qapplication.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QKeyEvent>
#include <Q3PopupMenu>
#include <Q3VBoxLayout>

#ifndef _WIN32

#include <unistd.h>
#include <sys/wait.h>

#endif

#include "cdcat.h"
#include "guibase.h"
#include "borrow.h"
#include "mainwidget.h"
#include "colorsettings.h"
#include "dbase.h"
#include "cdcat.h"
#include "wdbfile.h"
#include "commwidget.h"
#include "hdirview.h"
#include "newdbdialog.h"
#include "adddialog.h"
#include "find.h"
#include "config.h"
#include "ui_help.h"
#include "import.h"
#include "exportcdcatdb.h"
#include "info.h"
#include "showcontent.h"

#include "icons.h"

#include "misc.h"



#include <iostream>
using namespace std;


char *rbuff=NULL;

CdCatConfig *glob_conf;

void GuiSlave::checkversion ( QWidget *p,DataBase *db ) {
    if ( db == NULL ) return;
    if ( ( db->getRootNode() ) == NULL ) return;
    if ( ( ( db->getRootNode() )->data ) == NULL ) return;

    QString fv= ( ( DBCatalog * ) ( ( db->getRootNode() )->data ) )->fileversion;

    if ( fv.isEmpty() ) //vers < 1.2
        return;

    if ( fv == "1.2" ) return;
    if ( fv == "1.3" ) return;
    if ( fv == "1.4" ) return;
    if ( fv == "2.0" ) return;

    QMessageBox::warning ( p,tr ( "Warning..." ),
                           tr (
                               "The database file has newer version than this version of cdcat can work with:\n\
I understand maximum %1 datafile version but readed %2\n\n\
Strongly recommended to upgrade your cdcat!!!\n\
Homepage: %3" ).arg ( DVERS ).arg ( fv ).arg ( HOMEPAGE ) );

}

GuiSlave::GuiSlave ( CdCatMainWidget *p ) {
    mainw=p;

    standON = NULL;
    tmpParent = NULL;
    if ( mainw->db != NULL ) {
        updateListFromNode ( mainw->db->getRootNode() );
        standOn ( mainw->listView->currentItem() );
    }
    glob_conf = p->cconfig;
}

void GuiSlave::updateStatusl ( Node *n ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	std::cerr <<"F-updateStatusl" << endl;
   
    QString o ( "" );
    if ( n == NULL ) {
        if ( mainw->db == NULL )
            mainw->statusl->setText ( tr ( "No database opened." ) );
        else
            mainw->statusl->setText ( tr ( "No item selected." ) );
        return;
    }
    if(*DEBUG_INFO_ENABLED)
	cerr<<"F-updateStatusl: " << qPrintable(n->getFullPath())<<endl;
    mainw->statusl->setText ( n->getFullPath() );
}

int GuiSlave::isIdentical ( int i ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
         std::cerr <<"F-isIdentical" << endl;
    Node *t;

    if ( mainw->db == NULL ) return 1;
    t=mainw->db->getRootNode();
    if ( t->child == NULL ) return 1;
    t = t->child;
    while ( t != NULL ) {
        if ( t->type == HC_MEDIA )
            if ( ( ( DBMedia * ) ( t->data ) )->number == i ) return 0;

        t = t->next;
    }
    return 1;
}

bool GuiSlave::haveContent ( Node *node ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"F-haveContent"<<endl;
    bool need_showc = false;
    Node *tmp = NULL;

    if ( node == NULL )
        return false;

    if ( node->type == HC_FILE && ( ( DBFile * ) ( node->data ) )->prop != NULL ) {
        tmp= ( ( DBFile * ) ( node->data ) )->prop;
        while ( tmp != NULL ) {
            if ( tmp->type == HC_CONTENT ) {
                need_showc = true;
            }
            tmp = tmp->next;
        }
    }
    return need_showc;
}

int GuiSlave::isIdentical ( QString q ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-isIdentical"<<endl;
    Node *t;

    if ( mainw->db == NULL ) return 1;
    t=mainw->db->getRootNode();
    if ( t->child == NULL ) return 1;
    t = t->child;
    while ( t != NULL ) {
        if ( t->type == HC_MEDIA )
            if ( ( ( DBMedia * ) ( t->data ) )->name == q ) return 0;

        t = t->next;
    }
    return 1;
}

Node *GuiSlave::getNodeFromFullName ( Node *root,const QString& newloc ) {
    QString name;
    DEBUG_INFO_ENABLED = init_debug_info();
    /*------------*/
    Node *tmp=NULL;
    Node *step =  root;
    int   down = 0;
    QStringList strl = newloc.split ( '/' );
    int index=1;

    while ( index != strl.size() ) {
//     cerr << "index: " << index << endl;
        name= strl.at ( index-1 );
       if(*DEBUG_INFO_ENABLED)
	cerr << "new name: " << qPrintable(name) << endl;

        if ( down == 0 ) {
		//if(*DEBUG_INFO_ENABLED)
		//	cerr << "step->getNameOf(): " << qPrintable(step->getNameOf()) << endl;
            if ( QString ( name ) != step->getNameOf() )
                return NULL; /* error Not matching catalog name! */
            down++;
        } else {
            tmp=step->child;
            if ( tmp==NULL )
                return NULL;  /* I couldn't find the requested element! */

		//if(*DEBUG_INFO_ENABLED)
		//	cerr << "1 tmp->getNameOf(): " << qPrintable(tmp->getNameOf()) << endl;
            while ( tmp->getNameOf() != QString ( name ) ) {
                tmp=tmp->next;
		//if(*DEBUG_INFO_ENABLED)
		//	cerr << "2 tmp->getNameOf(): " << qPrintable(tmp->getNameOf()) << endl;
                if ( tmp==NULL )
                    return NULL;  /* I couldn't find the requested element! */
                down++;
            }
            step=tmp;
        }
        index++;
    }
    return step;
}


/*
  int i=0,out=0;
  char *org;
  char *re_name;
  char name[256];
  //------------//
  Node *tmp=NULL;
  Node *step =  root;
  int   down = 0;
  org=re_name=mstr((const char *)newloc);

  while(!out)
   {
    for(i=0;re_name[i]!='/' && re_name[i]!='\0';i++ )
        name[i]=re_name[i];
    if(re_name[i]=='\0')
     {
       out=1;
       continue;
     }
    name[i]='\0';       // <-

    if(down == 0 )
     {
      if(strcmp(name,step->getNameOf()))
         return NULL; //error Not matching catalog name!
      down++;
     }
    else
     {
      tmp=step->child;
      if(tmp==NULL)
	  return NULL;  //I couldn't find the requested element!

      while(strcmp(tmp->getNameOf(),name))
       {
        tmp=tmp->next;
        if(tmp==NULL)
  	    return NULL;  //I couldn't find the requested element!
	down++;
       }
      step=tmp;
     }
    re_name+=(strlen(name)+1);
   }
  delete [] org;
  return step;
 }
 */

int GuiSlave::listUpdate ( const QString& newloc ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-listUpdate 1 newloc: " << qPrintable(newloc)<<endl;
    if ( mainw->db == NULL ) {
        return 0;
    }
    Node *pdir = getNodeFromFullName ( mainw->db->getRootNode(), newloc );
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-listUpdate: 2 newloc: " << qPrintable ( newloc ) <<endl;
    if ( pdir ) {
        if(*DEBUG_INFO_ENABLED)
		cerr<<"F-listUpdate: pdir: " << qPrintable ( pdir->getNameOf() ) <<endl;
    }
    else {
        if(*DEBUG_INFO_ENABLED)
		cerr<<"F-listUpdate: pdir: null" <<endl;
    }
    updateStatusl ( pdir );
    mainw->commentWidget->showNode ( pdir, 0 );
    standON = NULL;
    return updateListFromNode ( pdir );
}

int GuiSlave::updateListFromNode ( Node *pdir ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"F-updateListFromNode"<<endl;
    int fflag=0;
    Node *tmp;
    HQListViewItem *lvi=NULL;
    QString qstr1;
    QString qstr2;

    if ( pdir == NULL )  {
        if(*DEBUG_INFO_ENABLED)
		cerr<<"F-updateListFromNode: pdir is null"<<endl;
        pdir = NodePwd;
    }

    if ( pdir == NULL ) {
        mainw->listView->clear();
        return 0;
    }

    mainw->listView->clear();
    mainw->listView->setSorting ( mainw->listView->scol,mainw->listView->sasc );

    //Set column text:
    if ( pdir != NULL && pdir->type == HC_CATALOG ) {
        mainw->listView->setColumnText ( 1,tr ( "Number" ) );
    } else {
        mainw->listView->setColumnText ( 1,tr ( "Size" ) );
    }


    if ( pdir->parent != NULL ) {
        lvi = new HQListViewItem ( mainw->listView,"..","",tr ( "Directory" ) );
        lvi->setPixmap ( 0,*get_v_back_icon() );
        lvi->etype = 0;
    }

    NodePwd = pdir;
    tmp=pdir->child;

    /*List everything*/

    while ( tmp != NULL ) {
        if ( tmp->type == HC_MP3TAG )
            return 1; //Error

        // 2.(size) Column name:

        if ( tmp->type == HC_FILE )
            qstr1.sprintf ( "%.2f %s",
                            ( ( DBFile * ) ( tmp->data ) )->size,
                            getSType ( ( ( DBFile * ) ( tmp->data ) )->sizeType ) );
        else if ( tmp->type == HC_MEDIA )
            qstr1.setNum ( ( ( DBMedia * ) ( tmp->data ) )->number );
        else
            qstr1 = " ";


        // 3. Column name:
        switch ( tmp->type ) {
        case HC_CATALOG : qstr2 =tr ( "Catalog" ); break;
        case HC_MEDIA   :
            switch ( ( ( DBMedia* ) ( tmp->data ) )->type ) {
            case UNKNOWN : qstr2=tr ( "Unknown(DB)" );  break;
            case CD      : qstr2=tr ( "CD" );           break;
            case DVD     : qstr2=tr ( "DVD" );          break;
            case HARDDISC: qstr2=tr ( "HardDisc" );     break;
            case FLOPPY  : qstr2=tr ( "Floppy" );       break;
            case NETPLACE: qstr2=tr ( "NetworkDrv" );   break;
            case FLASHDRV: qstr2=tr ( "FlashDrv" );     break;
            case OTHERD  : qstr2=tr ( "OtherDevice" );  break;
            }
            break;
        case HC_DIRECTORY:   qstr2=tr ( "Directory" );       break;
        case HC_FILE:        qstr2=tr ( "File" );            break;
        case HC_CATLNK:      qstr2=tr ( "Catalog Link" );    break;
        }

        //if(*DEBUG_INFO_ENABLED)
	//	cerr <<"GETNAMEOF-----------"<<qPrintable ( tmp->getNameOf() ) <<endl;
        QString valami;
        valami=tmp->getNameOf();
//!!!
//valami.append("---1");
        //if(*DEBUG_INFO_ENABLED)
	//	cerr <<"GETNAMEOF-----------"<<qPrintable ( valami ) <<endl;

        lvi = new HQListViewItem ( mainw->listView,valami,qstr1,qstr2 );

        switch ( tmp->type ) {
        case HC_CATALOG  : lvi->etype = 0;  break;
        case HC_MEDIA    : lvi->etype = 3;  break;
        case HC_DIRECTORY: lvi->etype = 1;  break;
        case HC_FILE     : lvi->etype = 2;  break;
        case HC_CATLNK   : lvi->etype = ( mainw->cconfig->linkf ? 0 : 4 );  break;
        }

        switch ( tmp->type ) {
        case HC_CATALOG : break;
        case HC_MEDIA:
            switch ( ( ( DBMedia* ) ( tmp->data ) )->type ) {
            case UNKNOWN : lvi->setPixmap ( 0,*get_m_unknown_icon() ); break;
            case CD      : lvi->setPixmap ( 0,*get_m_cd_icon() ); break;
            case DVD     : lvi->setPixmap ( 0,*get_m_dvd_icon() ); break;
            case HARDDISC: lvi->setPixmap ( 0,*get_m_hdd_icon() ); break;
            case FLOPPY  : lvi->setPixmap ( 0,*get_m_floppy_icon() ); break;
            case NETPLACE: lvi->setPixmap ( 0,*get_m_net_icon() ); break;
            case FLASHDRV: lvi->setPixmap ( 0,*get_m_flash_icon() ); break;
            case OTHERD  : lvi->setPixmap ( 0,*get_m_other_icon() ); break;
            }
            break;
        case HC_DIRECTORY: lvi->setPixmap ( 0,*get_v_folderclosed_icon() );  break;
        case HC_FILE:      lvi->setPixmap ( 0,*get_v_file_icon() );         break;
        case HC_CATLNK:    lvi->setPixmap ( 0,*get_p_icon() );         break;
        }

        if ( tmpParent != NULL ) { /*Return to previous parent*/
            if ( tmp->getNameOf() == tmpParent->getNameOf() ) {
                mainw->listView->setCurrentItem ( lvi );
                tmpParent = NULL;
                fflag = 1;
            }
        }
        tmp=tmp->next;
    }

    mainw->listView->setSorting ( mainw->listView->scol,mainw->listView->sasc );

    if ( !fflag )
        mainw->listView->setCurrentItem ( mainw->listView->firstChild() );

    if(*DEBUG_INFO_ENABLED)
	cerr <<"BEACON-1"<<endl;
    mainw->listView->changed();
    if(*DEBUG_INFO_ENABLED)
	cerr <<"BEACON-2"<<endl;
    mainw->DirView->setDir ( NodePwd );
    if(*DEBUG_INFO_ENABLED)
	cerr <<"BEACON-3"<<endl;
    return 0;
}

int GuiSlave::standOn ( Q3ListViewItem *on ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-standOn"<<endl;
    Node *tmp;

    if ( on == NULL || on == 0 ) return 0;
    if(*DEBUG_INFO_ENABLED)
	cerr<<"1"<<endl;
    mainw->listView->setSelected ( on,true );
    if ( on->text ( 0 ) == ".." ) {
        mainw->commentWidget->showNode ( NodePwd,1 );
        updateStatusl ( NodePwd->parent );
        standON = NULL;
        return 0;
    }
    if (*DEBUG_INFO_ENABLED)
	cerr<<"2"<<endl;

    tmp=NodePwd->child;
    while ( tmp->getNameOf() != on->text ( 0 ) ) {
        tmp=tmp->next;
        if ( tmp == NULL )
            return 0;
    }
    if(*DEBUG_INFO_ENABLED)
	cerr<<"3"<<endl;
    standON = tmp;
    if(*DEBUG_INFO_ENABLED)
	cerr<<"4"<<endl;
    mainw->commentWidget->showNode ( tmp,0 );
    if(*DEBUG_INFO_ENABLED)
	cerr<<"5"<<endl;
    updateStatusl ( tmp );
    if(*DEBUG_INFO_ENABLED)
	cerr<<"6"<<endl;
    return 0;
}

int GuiSlave::doubleClickOn ( Q3ListViewItem *on ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"F-doubleClickOn"<<endl;
    Node *tmp;
    if ( on->text ( 0 ) == ".." ) {
        if ( NodePwd->parent != NULL ) {
            tmpParent =  NodePwd;
            NodePwd   =  NodePwd->parent;
        }
    } else { //step down
        tmp=NodePwd->child;
        while ( tmp->getNameOf() != on->text ( 0 ) ) {
            tmp=tmp->next;
            if ( tmp == NULL ) return 0;
        }
        if ( tmp->type == HC_FILE ) {
            showContent();
            return 0;
        }
        if ( tmp->type == HC_CATLNK ) {
            followLnk();
            return 0;
        }
        NodePwd = tmp;
    }
    updateListFromNode();
    return 0;
}

void GuiSlave::panelsOFF ( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"F-panelsOFF"<<endl;
    mainw->DirView->clear();
    mainw->listView->clear();
    if ( mainw->db != NULL )
        tmpParent=NodePwd=mainw->db->getRootNode();
    else
        tmpParent=NodePwd=NULL;
    mainw->commentWidget->act = NULL;
}

int  GuiSlave::cHcaption ( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-cHcaption"<<endl;
    if ( ( mainw->db != NULL ) && ( ( ( DBCatalog * ) ( mainw->db->getRootNode()->data ) )->writed == 0 ) ) {
        if (*DEBUG_INFO_ENABLED)
		cerr<<"case:1"<<endl;
        mainw->setCaption ( tr ( "Hyper's CD Catalogizer (modified)" ) );
    } else {
        if (*DEBUG_INFO_ENABLED)
		cerr<<"case:2"<<endl;

        mainw->setCaption ( tr ( "Hyper's CD Catalogizer" ) );
    }

    if (*DEBUG_INFO_ENABLED)
	cerr<<"....F-done."<<endl;
    return 0;
}

void GuiSlave::panelsON ( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED)
	cerr<<"F-panelsON"<<endl;
    panelsOFF(); //That case you forget the OFF before...

    mainw->DirView->start();
    mainw->listView->start();
    updateListFromNode();
    mainw->commentWidget->act = NULL;
    mainw->commentWidget->repaint();
    updateStatusl ( standON );
    standOn ( mainw->listView->currentItem() );
    cHcaption();
}


void GuiSlave::showListviewContextMenu ( Q3ListViewItem *, const QPoint &p, int ) {
    mPopup = new Q3PopupMenu ( 0 );
    mPopup->setCheckable ( true );

    if ( standON != NULL ) {
        mPopup->insertItem ( *get_t_comment_icon(),tr ( "View/Edit Comment..." ),this,SLOT ( editComment() ) );
        mPopup->insertSeparator();
        mPopup->insertItem ( tr ( "Node size" ),this,SLOT ( sizeEvent() ) );
        mPopup->insertSeparator();

        if ( haveContent ( standON ) )
            mPopup->insertItem ( *get_t_showc_icon(),tr ( "Show/Remove Content..." ),this,SLOT ( showContent() ) );

        if ( standON->type == HC_CATLNK )
            mPopup->insertItem ( *get_p_icon(),tr ( "Follow the link (Open it) !" ),this,SLOT ( followLnk() ) );

        mPopup->insertItem ( tr ( "Rename node..." ),this,SLOT ( renameEvent() ) );
        mPopup->insertItem ( *get_t_delete_icon() ,tr ( "Delete node" ),this,SLOT ( deleteEvent() ) );


        if ( standON->type == HC_MEDIA ) {
            mPopup->insertSeparator();
            if ( ( ( DBMedia * ) ( standON->data ) )->borrowing == "" )
                mPopup->insertItem ( *get_t_sborrow_icon() ,tr ( "Borrow this media to..." ),this,SLOT ( sborrowEvent() ) );
            else
                mPopup->insertItem ( *get_t_cborrow_icon() ,tr ( "I got it back! (clear borrowing mark)" ),this,SLOT ( cborrowEvent() ) );

            mPopup->insertSeparator();
            mPopup->insertItem ( *get_t_rescan_icon() ,tr ( "Rescan media..." ),this,SLOT ( rescanEvent() ) );
            mPopup->insertItem ( tr ( "Re-Number media..." ),this,SLOT ( renumberEvent() ) );
        }
    }
    mPopup->insertSeparator();
    mPopup->insertItem ( *get_t_add_icon() ,tr ( "Add media..." ),this,SLOT ( addEvent() ) );
    mPopup->insertItem ( *get_p_icon() ,tr ( "Add a link to a CdCAt Catalog..." ),this,SLOT ( addlnkEvent() ) );
    mPopup->insertItem ( tr ( "Insert Catalog..." ),this,SLOT ( insertcEvent() ) );
    mPopup->exec ( p );
    delete mPopup;
    mPopup = NULL;
}

void GuiSlave::showTreeContextMenu ( Q3ListViewItem *, const QPoint &p2, int ) {
    Node *on,*save;
    if ( mainw->db == NULL ) on = NULL;
    else {
        on= getNodeFromFullName (
                mainw->db->getRootNode(),
                ( ( LNode * ) mainw->DirView->currentItem() )->fullName()
            );
    }

    mPopup = new Q3PopupMenu ( 0 );
    mPopup->setCheckable ( true );
    if ( on != NULL ) {
        mPopup->insertItem ( *get_t_comment_icon(),tr ( "View/Edit Comment..." ),this,SLOT ( editComment() ) );
        mPopup->insertSeparator();
        mPopup->insertItem ( tr ( "Node size" ),this,SLOT ( sizeEvent() ) );
        mPopup->insertSeparator();

    }
    if ( on != NULL && on->type != HC_CATALOG ) {
        mPopup->insertItem ( tr ( "Rename node..." ),this,SLOT ( renameEvent() ) );
        mPopup->insertItem ( *get_t_delete_icon() ,tr ( "Delete node" ),this,SLOT ( deleteEvent() ) );
        mPopup->insertSeparator();
        if ( on->type == HC_MEDIA ) {

            if ( ( ( DBMedia * ) ( on->data ) )->borrowing == "" )
                mPopup->insertItem ( *get_t_sborrow_icon() ,tr ( "Borrow this media to..." ),this,SLOT ( sborrowEvent() ) );
            else
                mPopup->insertItem ( *get_t_cborrow_icon() ,tr ( "I got it back! (clear borrowing mark)" ),this,SLOT ( cborrowEvent() ) );

            mPopup->insertItem ( *get_t_rescan_icon() ,tr ( "Rescan media..." ),this,SLOT ( rescanEvent() ) );
            mPopup->insertItem ( tr ( "Re-Number media..." ),this,SLOT ( renumberEvent() ) );
            mPopup->insertSeparator();
        }
    }
    mPopup->insertItem ( *get_t_add_icon() ,tr ( "Add media..." ),this,SLOT ( addEvent() ) );
    mPopup->insertItem ( *get_p_icon() ,tr ( "Add a link to a CdCAt Catalog..." ),this,SLOT ( addlnkEvent() ) );
    mPopup->insertItem ( tr ( "Insert Catalog..." ),this,SLOT ( insertcEvent() ) );
    if ( on != NULL ) {
        mPopup->insertSeparator();
        mPopup->insertItem ( tr ( "Close all branch" ),this,SLOT ( closeBranch() ) );
    }

    save = standON;
    standON = on;
    mPopup->exec ( p2 );
    standON = save;
    delete mPopup;
    mPopup = NULL;
}


/*****************************************************************************
 *
 *  class HQListView
 *
*****************************************************************************/

void HQListViewItem::paintCell ( QPainter *p,const QColorGroup & cg,int column,int width,int align ) {
    QColorGroup ocg ( cg );
    ocg.setColor ( QColorGroup::Highlight,*glob_conf->comm_bg );
    ocg.setColor ( QColorGroup::HighlightedText,QColor ( Qt::black ) );
    Q3ListViewItem::paintCell ( p,ocg,column,width,align );
}

HQListViewItem::HQListViewItem ( Q3ListView *parent )
        :Q3ListViewItem ( parent ) {
    setExpandable ( FALSE );
}

HQListViewItem::HQListViewItem ( Q3ListView *parent,QString label1,QString label2,QString label3 )
        :Q3ListViewItem ( parent,label1,label2,label3 ) {
    setExpandable ( FALSE );
}

HQListViewItem::HQListViewItem ( Q3ListView *parent,Q3ListViewItem *after,QString label1,QString label2,QString label3 )
        :Q3ListViewItem ( parent,after,label1,label2,label3 ) {
    setExpandable ( FALSE );
}

QString HQListViewItem::key ( int column,bool ascending ) const {
    double value = 0;
    int    mod   = ascending ? etype : 5 - etype ;

    switch ( column ) {
        //NAME
    case 0: return text ( column ).prepend ( '1'+mod );
        //SIZE
    case 1:
        //ret = (QListViewItem::key(1,ascending)).append('0'+mod);
        if ( etype == 2 && !text ( 1 ).isEmpty() ) {
            value = getSizeFS ( text ( 1 ) );
            switch ( getSizetFS ( text ( 1 ) ) ) {
            case KBYTE: value *= ( 1024.0 );               break;
            case MBYTE: value *= ( 1024.0*1024.0 );        break;
            case GBYTE: value *= ( 1024.0*1024.0*1024.0 ); break;
            default: break;
            }
            return ( ( QString().setNum ( ( long int ) value ) )
                     .rightJustify ( 10,'0' ) )
                   .prepend ( '1'+mod );
        }
        if ( etype == 3 ) { //HC_MEDIA
            return ( QString().setNum ( text ( 1 ).toInt() ) )
                   .rightJustify ( 10,'0' )
                   .prepend ( '1'+mod );
        }
        return text ( 0 ).prepend ( '1'+mod );
        //TYPE
    case 2:  return ( text ( 2 ).append ( text ( 0 ) ) ).prepend ( '1'+mod );

    }
    return "";
}

void HQListView::changed ( void ) {
    emit currentChanged ( currentItem() );
}

void HQListView::start ( void ) {
    ;
}

HQListView::HQListView ( CdCatMainWidget *mw,QWidget *parent,const char *name,Qt::WFlags f )
        :Q3ListView ( parent,name,f ) {
    mainw = mw;
    setSelectionMode ( Single );
    addColumn ( tr ( "Name" ) );
    addColumn ( tr ( "Size" ) );
    addColumn ( tr ( "Type" ) );
    setAllColumnsShowFocus ( true );
    setShowSortIndicator ( true );
    setSorting ( -1 );
}

void HQListView::curr_vis ( void ) {
    Q3ListViewItem * i;
    if ( ( i=currentItem() ) != 0 )
        ensureItemVisible ( i );
}

void HQListView::setSorting ( int column, bool increasing ) {
    scol = column;
    sasc = increasing;

    if ( scol<0 || scol>3 )
        scol = 0;

    Q3ListView::setSorting ( scol,increasing );
}

int GuiSlave::hotKeys ( QKeyEvent *ke ) {
    switch ( ke->key() ) {
        /*  Handled from mainwidget.cpp

            case (Qt::Key_O) :     openEvent();    return 1;
            case (Qt::Key_N) :     newEvent();     return 1;
            case (Qt::Key_C) :     closeEvent();   return 1;
            case (Qt::Key_A) :     addEvent();     return 1;
            case (Qt::Key_R) :     rescanEvent();  return 1;
            case (Qt::Key_G) :     configEvent();  return 1;
            case (Qt::Key_F) :     findEvent();    return 1;
            case (Qt::Key_H) :     helpEvent();    return 1;
            case (Qt::Key_Q) :     mainw->close(); return 1;
            case (Qt::Key_Space) :  sizeEvent();    return 1;
        */
    case ( Qt::Key_Escape ) : mainw->close(); return 1;

    case ( Qt::Key_S ) :
        if ( ke->state() == Qt::ControlModifier )
            posEvent();
        return 1;
    }
    return 0;
}

void HQListView::keyPressEvent ( QKeyEvent *ke ) {
    Node *tmp;
    Q3ListViewItem *it;


    if ( mainw->guis->hotKeys ( ke ) ) return; //It was a hotkey and it's handled yet

    if ( mainw->db == NULL ) return;

    if ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Backspace ) {
        if ( mainw->guis->NodePwd->parent != NULL ) {
            mainw->guis->tmpParent =  mainw->guis->NodePwd;
            mainw->guis->NodePwd   = mainw->guis->NodePwd->parent;
        }

        mainw->guis->updateListFromNode();
    }

    if ( ke->key() == Qt::Key_Right || ke->key() == Qt::Key_Return ) {
        it = currentItem();
        if ( !strcmp ( it->text ( 0 ),".." ) ) {

            if ( mainw->guis->NodePwd->parent != NULL ) {
                mainw->guis->tmpParent =  mainw->guis->NodePwd;
                mainw->guis->NodePwd   = mainw->guis->NodePwd->parent;
            }
        } else { //step down
            tmp=mainw->guis->NodePwd->child;
            while ( strcmp ( tmp->getNameOf(),it->text ( 0 ) ) ) {
                tmp=tmp->next;
                if ( tmp == NULL ) return;
            }
            if ( tmp->type == HC_FILE ) {
                mainw->guis->showContent();
                return;
            }
            if ( tmp->type == HC_CATLNK ) {
                mainw->guis->followLnk();
                return;
            }

            mainw->guis->NodePwd = tmp;
        }

        mainw->guis->updateListFromNode();
    }

    curr_vis();
    if ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right )
        return;

    Q3ListView::keyPressEvent ( ke );
}

/* Mainwindow buttons ***********************************/
int GuiSlave::newEvent ( void ) {
    newdbdialog *d = new newdbdialog ( mainw,"new_db_dialog",true );

    while ( closeEvent() != 0 );
    d->exec();
    if ( d->OK == 1 ) {
        panelsOFF();
        if ( mainw->db == NULL )
            mainw->db = new DataBase();
        mainw->db->setDBName ( d->nameD );
        mainw->db->setDBOwner ( d->ownerD );
        mainw->db->setComment ( d->commD );
        panelsON();
    }
    return 0;
}

int GuiSlave::openEvent ( void ) {
    char fnc[256];
    QString fn;
    int ret_val=0;
    DEBUG_INFO_ENABLED = init_debug_info();
    
    fn = QFileDialog::getOpenFileName(0, tr ( "Open a file..." ), mainw->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ));
    if ( fn.isEmpty() )
         return 0;

    mainw->cconfig->lastDir = QFileInfo( fn ).absoluteDir().absolutePath();

    strcpy ( fnc, ( const char * ) ( QFile::encodeName ( fn ) ) );
    while ( closeEvent() !=0 );
    panelsOFF();

    if ( mainw->db == NULL )
        mainw->db = new DataBase();

    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );

    if ( ( ret_val=mainw->db->openDB ( fnc ) ) != 0 ) { // An error occured
        QMessageBox::warning ( mainw,tr ( "Error while opening..." ),
                               mainw->db->errormsg );
        mainw->db = NULL;
        standON = NodePwd = NULL;
        panelsOFF();
    }

    checkversion ( mainw,mainw->db );

    progress ( pww );
    if(*DEBUG_INFO_ENABLED)
	cerr<<"FLAG-1"<<endl;
    panelsON();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"FLAG-2"<<endl;

    progress ( pww );

    //Save the opened file to the history
    //QMessageBox::information(0,"new history element",fn);
    if ( ret_val == 0 && !fn.isEmpty() ) {

       if(*DEBUG_INFO_ENABLED)
		cerr <<"LIST:"<<qPrintable(mainw->cconfig->hlist.join("|"))<<endl;

        if(*DEBUG_INFO_ENABLED)
		cerr<<"0-1"<<endl;
        if ( mainw->cconfig->hlist.isEmpty() ) cerr <<"emptlyysdsaféashfk"<<endl;
        if(*DEBUG_INFO_ENABLED)
		cerr<<"0-2"<<endl;
        mainw->cconfig->hlist.grep ( "AAAA" );
        if(*DEBUG_INFO_ENABLED)
		cerr<<"0-3"<<endl;


        if ( mainw->cconfig->hlist.isEmpty() ||
                mainw->cconfig->hlist.grep ( "^"+QString ( fn ) +"$" ).isEmpty() ) {
            if(*DEBUG_INFO_ENABLED)
		cerr<<"1"<<endl;
            mainw->cconfig->hlist.append ( QString ( fn ) );
            if(*DEBUG_INFO_ENABLED)
		cerr<<"2"<<endl;
            mainw->historyMenu->addAction ( *get_t_open_icon(),fn );
            if(*DEBUG_INFO_ENABLED)
		cerr<<"3"<<endl;
            if ( ( int ) mainw->cconfig->hlist.count() > ( int ) mainw->cconfig->historysize ) {
                if(*DEBUG_INFO_ENABLED)
			cerr<<"4"<<endl;
                ( mainw->cconfig->hlist ).remove ( mainw->cconfig->hlist.begin() );
                if(*DEBUG_INFO_ENABLED)
			cerr<<"5"<<endl;
                mainw->historyMenu->removeItemAt ( 0 );
            }
        }
    }

    if(*DEBUG_INFO_ENABLED)
	cerr<<"FLAG-3"<<endl;

    progress ( pww );
    pww->end();
    if ( mainw->db != NULL )
        mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::saveEvent ( void ) {
    if ( mainw->db == NULL ) return 0;

    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );

    mainw->db->setNice ( mainw->cconfig->nice );
    if ( mainw->db->saveDB() != 0 ) {
        progress ( pww );
        pww->end();
        mainw->db->pww = NULL;
        delete pww;

        saveasEvent();
        return 0;
    }

    panelsOFF();
    panelsON();

    progress ( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;

    return 0;
}

int GuiSlave::saveasEvent ( void ) {
    int retv=0;
    char fnc[256];
    QString fn;

    if ( mainw->db == NULL ) return 0;
    fn = QFileDialog::getSaveFileName(0, tr ( "Save to file..." ), mainw->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ));
    if ( fn.isEmpty() )  return 0;


    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );


    strcpy ( fnc, ( const char * ) QFile::encodeName ( fn ) );

    //extension correction if necessary
    if ( strlen ( fnc ) <5 || strcmp ( ( fnc+ ( strlen ( fnc )-4 ) ),".hcf" ) != 0 )
        sprintf ( fnc,"%s.hcf", ( const char * ) fn );

    progress ( pww );

    mainw->db->setNice ( mainw->cconfig->nice );
    if ( mainw->db->saveAsDB ( fnc ) != 0 ) { // An error occured
        QMessageBox::warning ( mainw,tr ( "Error while saving..." ),mainw->db->errormsg );
        retv=2;
        //The file cannot be writed
        //(It is the only reason the saveAsSB return nonzero)
    }
    panelsOFF();
    progress ( pww );
    panelsON();

    progress ( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    return retv;
}

int GuiSlave::closeEvent ( void ) {
    int userrq=0;
    int retv=0;
    if ( mainw->db == NULL ) return 0;
    if ( ( ( DBCatalog * ) ( mainw->db->getRootNode()->data ) )->writed == 0 ) {
        userrq = QMessageBox::warning ( mainw,tr ( "Closing previous database..." ),tr ( "Do you want to save the changes?" ),
                                        tr ( "Yes" ),tr ( "No" ) );

        if ( userrq == -1 ) retv = -1;

        if ( userrq == 0 )
            if ( ( retv=mainw->db->saveDB() ) != 0 ) {
                if ( retv==1 ) //The databas had no name.
                    retv=saveasEvent();
                // retv can be nonrezo reasoned by saveDB or saveAsEvent too.
            }
    }
    panelsOFF();

    /* Freeing database object */
    if ( retv == 0 ) {
        delete mainw->db;
        mainw->db = NULL;
        standON = NodePwd = NULL;
    }

    panelsON();
    return retv;
}

int GuiSlave::deleteEvent ( void ) {
    QString o;
    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected item in the middle list box!" ) );
        return 0;
    }

    o = tr ( "Do you really want to delete this?: \n%1" ).arg ( standON->getNameOf() );
    if ( QMessageBox::warning ( mainw,tr ( "Confirmation" ),o,
                                tr ( "Yes" ),tr ( "No" ) ) == 0 ) {
        panelsOFF();
        mainw->db->deleteNode ( standON );
        standON = NULL;
        panelsON();
    }
    return 0;
}

int GuiSlave::addEvent ( void ) {
    int i;
#ifndef _WIN32
    bool mount_successful = false;
#endif
    DEBUG_INFO_ENABLED = init_debug_info();
    if ( mainw->db == NULL )
        newEvent();
    if ( mainw->db == NULL )
        return 0;

    addDialog *d=new addDialog ( this,mainw,"adddialog",true );
    d->exec();

    mainw->db->storeMp3tags     = mainw->cconfig->readtag;
    mainw->db->v1_over_v2       = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;


    mainw->db->storeContent = mainw->cconfig->readcontent;
    mainw->db->storeLimit      = mainw->cconfig->readclimit;
    mainw->db->storedFiles = mainw->cconfig->readcfiles;
    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-1"<<endl;
    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;

#ifndef _WIN32
    if ( mainw->cconfig->mounteject )
        if ( ( d->type == CD || d->type == DVD ) &&
                d->dDir == mainw->cconfig->cdrompath ) {
            int pid;

            char **arg=new char*[3];
            char **env=new char*[2];

            //search the mount program:
            if ( QFile ( "/usr/local/bin/mount" ).exists() )	  arg[0] = mstr ( "/usr/local/bin/mount" );
            else if ( QFile ( "/usr/local/sbin/mount" ).exists() )  arg[0] = mstr ( "/usr/local/sbin/mount" );
            else if ( QFile ( "/usr/bin/mount" ).exists() )   	  arg[0] = mstr ( "/usr/bin/mount" );
            else if ( QFile ( "/usr/sbin/mount" ).exists() )   	  arg[0] = mstr ( "/usr/sbin/mount" );
            else if ( QFile ( "/bin/mount" ).exists() ) 	 	  arg[0] = mstr ( "/bin/mount" );
            else if ( QFile ( "/sbin/mount" ).exists() )		  arg[0] = mstr ( "/sbin/mount" );
            else {
                QMessageBox::warning
                ( d,tr ( "Cannot mount CD" ),tr ( "I can't find the \"mount\" program" ) );
                arg[0] = mstr ( "mount" );
            }
            if(*DEBUG_INFO_ENABLED)
		fprintf ( stderr,"Call:%s %s...",arg[0], ( const char * ) mainw->cconfig->cdrompath );
            arg[1] = mstr ( mainw->cconfig->cdrompath );
            arg[2] = 0;
            env[0] = mstr ( "PATH=/usr/local/bin:/usr/bin:/bin" );
            env[1] = 0;

            pid = fork();
            if ( pid == 0 ) { //mount process
                if ( execve ( arg[0], ( char *const* ) arg, ( char *const* ) env ) == -1 )
                    exit ( -1 );
                else exit ( 0 ); //That's never happend.
            } else {      //cdcat process
                int v;
                pww->refreshTime = 200;
                while ( 0 == waitpid ( pid,&v,WNOHANG ) ) {
                    progress ( pww );
                    usleep ( 500 );
                }
                pww->refreshTime = 100;
                fprintf ( stderr,"done.\n" );
                if ( WEXITSTATUS ( v ) != 0 )
                    QMessageBox::warning ( 0,tr ( "Cannot mount CD!" ),tr ( "Cannot mount CD!" ) );
                else {
                   mount_successful = true;
                   if(d->cbAutoDetectAtMount->isChecked()) {
			// mount succeded, read media name
			QString new_medianame = getCDName ( mainw->cconfig->cdrompath );
			if(! new_medianame.isEmpty() ) {
				if(*DEBUG_INFO_ENABLED)
					cerr<<"new_medianame after mount: "  << qPrintable(new_medianame) <<endl;
			d->dName = new_medianame;
			}
                   }
                }
            }
            delete []env;
            delete []arg;
        }
#endif

#ifndef _WIN32
    if (mount_successful) {
#endif

    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-2"<<endl;
    progress ( pww );

    if ( d->OK == 1 ) {
        panelsOFF();
        i=  mainw->db->addMedia ( d->dDir,d->dName,d->serial,d->type,
                                  ( d->dOwner.isEmpty() ? QString ( "" ) : d->dOwner ) );

        //Do autosave if the user ask it in the config
        if ( mainw->cconfig->autosave ) {
            int retv=0;
            if ( ( retv=mainw->db->saveDB() ) != 0 ) {
                if ( retv==1 ) //The databas had no name. need a save as...
                    retv=saveasEvent();
            }
        }
        if(*DEBUG_INFO_ENABLED)
		cerr<<"ADDEVENT-3"<<endl;
        panelsON();
        if ( i!=0 ) {
            QMessageBox::warning ( mainw,
                                   tr ( "Warning..." ),tr ( "An error occured while scanning,\nthe DataBase may be incomplete" ) );
        }

        if ( ! ( d->dComm ).isEmpty() ) {
            Node *tn= ( mainw->db->getRootNode() )->child;
            while ( tn->getNameOf() != d->dName )
                tn=tn->next;
            ( ( DBMedia * ) ( tn->data ) )->comment = d->dComm;
        }
    }
    progress ( pww );
    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-4"<<endl;
#ifndef _WIN32
    if ( mainw->cconfig->mounteject )
        if ( ( d->type == CD || d->type == DVD ) &&
                d->dDir == mainw->cconfig->cdrompath ) {
            int pid;

            char **arg=new char*[3];
            char **env=new char*[2];

            //search the eject program:
            if ( QFile ( "/usr/local/bin/eject" ).exists() )	  arg[0] = mstr ( "/usr/local/bin/eject" );
            else if ( QFile ( "/usr/local/sbin/eject" ).exists() )  arg[0] = mstr ( "/usr/local/sbin/eject" );
            else if ( QFile ( "/usr/bin/eject" ).exists() )   	  arg[0] = mstr ( "/usr/bin/eject" );
            else if ( QFile ( "/usr/sbin/eject" ).exists() )   	  arg[0] = mstr ( "/usr/sbin/eject" );
            else if ( QFile ( "/bin/eject" ).exists() ) 	 	  arg[0] = mstr ( "/bin/eject" );
            else if ( QFile ( "/sbin/eject" ).exists() )		  arg[0] = mstr ( "/sbin/eject" );
            else {
                QMessageBox::warning
                ( d,tr ( "Cannot eject CD" ),tr ( "I can't find the \"eject\" program" ) );
                arg[0] = mstr ( "eject" );
            }

            if(*DEBUG_INFO_ENABLED)
		fprintf ( stderr,"Call:%s %s...",arg[0], ( const char * ) mainw->cconfig->cdrompath );
            arg[1] = mstr ( mainw->cconfig->cdrompath );
            arg[2] = 0;
            env[0] = mstr ( "PATH=/usr/local/bin:/usr/bin:/bin" );
            env[1] = 0;

            pid = fork();
            if ( pid == 0 ) { // eject process
                if ( execve ( arg[0], ( char *const* ) arg, ( char *const* ) env ) == -1 )
                    exit ( -1 );
                else exit ( 0 ); //That's never happend.
            } else {      //cdcat process
                int v;
                pww->refreshTime = 200;
                while ( 0 == waitpid ( pid,&v,WNOHANG ) ) {
                    progress ( pww );
                    usleep ( 500 );
                }
                pww->refreshTime = 100;
                if(*DEBUG_INFO_ENABLED)
                fprintf ( stderr,"done.\n" );
                if ( WEXITSTATUS ( v ) != 0 )
                    QMessageBox::warning ( 0,tr ( "Cannot eject CD!" ),tr ( "Cannot eject CD!" ) );
            }
            delete []env;
            delete []arg;
        }
#endif

#ifndef _WIN32
    // mount_successful
    }
#endif


    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-5"<<endl;
    pww->end();
    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-6"<<endl;
    mainw->db->pww = NULL;
    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-7"<<endl;
    delete pww;
    if(*DEBUG_INFO_ENABLED)
	cerr<<"ADDEVENT-8"<<endl;
    return 0;
}

int GuiSlave::rescanEvent ( void ) {
    QString o;
    QString rfd;

    if ( mainw->db == NULL ) return 0;

    mainw->db->storeMp3tags     = mainw->cconfig->readtag;
    mainw->db->v1_over_v2       = mainw->cconfig->v1_over_v2;
    mainw->db->storeMp3techinfo = mainw->cconfig->readinfo;
    mainw->db->storeAvitechinfo = mainw->cconfig->readavii;
    mainw->db->storeContent     = mainw->cconfig->readcontent;
    mainw->db->storeLimit       = mainw->cconfig->readclimit;
    mainw->db->storedFiles      = mainw->cconfig->readcfiles;

    if ( standON == NULL ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected Media in the middle list box!" ) );
        return 0;
    }
    if ( standON->type != HC_MEDIA ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "You can refresh ONLY a MEDIA node!" ) );
        return 0;
    }

    o=tr ( "Rescan %1" ).arg ( standON->getNameOf() );
    rfd = QFileDialog::getExistingDirectory(0, tr ( "Select directory" ), o);
    if ( rfd.isEmpty() ) return 0;

    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );

    panelsOFF();
    if ( 0 != mainw->db->addMedia ( rfd,"__rescanned__",-1,UNKNOWN,"system" ) ) {
        Node *d;
        o =tr ( "An error occured while scanning, the rescan operation was cancelled: \n%1" )
           .arg ( mainw->db->errormsg );

        d= ( mainw->db->getRootNode() )->child;
        while ( d->getNameOf() != "__rescanned__" )
            d=d->next;
        mainw->db->deleteNode ( d );
        QMessageBox::warning ( mainw,tr ( "Error:" ),o );
    } else {
        Node *d;
        d= ( mainw->db->getRootNode() )->child;
        while ( d->getNameOf() != "__rescanned__" )
            d=d->next;

        ( ( DBMedia * ) ( d->data ) )->name = ( ( DBMedia * ) ( standON->data ) )->name;

        ( ( DBMedia * ) ( d->data ) )->comment = ( ( DBMedia * ) ( standON->data ) )->comment;

        ( ( DBMedia * ) ( d->data ) )->owner = ( ( DBMedia * ) ( standON->data ) )->owner;

        ( ( DBMedia * ) ( d->data ) )->type   = ( ( DBMedia * ) ( standON->data ) )->type ;

        ( ( DBMedia * ) ( d->data ) )->number = ( ( DBMedia * ) ( standON->data ) )->number;

        mainw->db->deleteNode ( standON );
    }
    standON=NULL;

    //Do autosave if the user ask it in the config !
    if ( mainw->cconfig->autosave ) {
        int retv=0;
        if ( ( retv=mainw->db->saveDB() ) != 0 ) {
            if ( retv==1 ) //The databas had no name. need a save as...
                retv=saveasEvent();
        }
    }

    panelsON();

    progress ( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;

    return 0;
}

int GuiSlave::findEvent ( void ) {
    if ( mainw->db == NULL ) return 0;

    findDialog *d=new findDialog ( mainw,"adddialog",true );
    d->exec();
    return 0;
}

int GuiSlave::configEvent ( void ) {
    ConfigDialog *d=new ConfigDialog ( mainw,"configdialog",true );
    d->exec();
    return 0;
}

int GuiSlave::aboutEvent ( void ) {
    InfoDialog *di=new InfoDialog ( mainw,"infodialog",true );
    di->exec();
    return 0;
}

int GuiSlave::aboutQtEvent ( void ) {
    QApplication::aboutQt();
    return 0;
}


int GuiSlave::insertcEvent ( void ) {
    char fnc[256];
    QString fn;

    if ( mainw->db == NULL )
        newEvent();
    if ( mainw->db == NULL )
        return 0;
    fn = QFileDialog::getOpenFileName(0, tr ( "Insert a database file..." ), mainw->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ));
    if ( fn.isEmpty() )  return 0;

    strcpy ( fnc, ( const char * ) QFile::encodeName ( fn ) );

    panelsOFF();

    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );

    if ( mainw->db->insertDB ( fnc ) != 0 ) { // An error occured
        QMessageBox::warning ( mainw,tr ( "Error while opening..." ),
                               mainw->db->errormsg );
    }

    progress ( pww );
    panelsON();
    progress ( pww );
    pww->end();
    mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::renameEvent ( void ) {
    renamedialog *rd;
    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected item in the middle list box!" ) );
        return 0;
    }
    rd = new renamedialog ( mainw,standON,this );
    rd->exec();
    cHcaption();
    updateListFromNode();
    delete rd;
    return 0;
}

int GuiSlave::renumberEvent ( void ) {
    renumberdialog *rd;
    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected item in the middle list box!" ) );
        return 0;
    }

    if ( standON->type != HC_MEDIA ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "It is NOT a media node!" ) );
        return 0;
    }
    rd = new renumberdialog ( mainw,standON,this );
    rd->exec();
    cHcaption();
    updateListFromNode();
    return 0;
}

int GuiSlave::sizeEvent ( void ) {
    char text[52];
    QString qtext;
    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL ) {
        QMessageBox::warning ( mainw,tr ( "Error:" ),tr ( "There is no selected item in the middle list box!" ) );
        return 0;
    }

    sprintf ( text,"%.2f Mb",mainw->db->getSize ( standON ) );
    qtext = tr ( "The size of \"%1\" : \n %2 \n %3 file /%4 directory" )
            .arg ( standON->getNameOf() )
            .arg ( text )
            .arg ( mainw->db->getCountFiles ( standON ) )
            .arg ( mainw->db->getCountDirs ( standON ) );

    QMessageBox::information ( mainw,tr ( "The resoult:" ),qtext );
    return 0;
}

int GuiSlave::sortNuEvent ( void ) {
    if ( mainw->db == NULL ) return 0;
    panelsOFF();
    mainw->db->sortM ( NUMBER );
    panelsON();
    return 0;
}

int GuiSlave::sortNaEvent ( void ) {
    if ( mainw->db == NULL ) return 0;
    panelsOFF();
    mainw->db->sortM ( NAME );
    panelsON();
    return 0;
}

int GuiSlave::sortTyEvent ( void ) {
    if ( mainw->db == NULL ) return 0;
    panelsOFF();
    mainw->db->sortM ( TYPE );
    panelsON();
    return 0;
}

int GuiSlave::sortTiEvent ( void ) {
    if ( mainw->db == NULL ) return 0;
    panelsOFF();
    mainw->db->sortM ( TIME );
    panelsON();
    return 0;
}

int GuiSlave::helpEvent ( void ) {
	 QDialog dh;
     Ui_helpDialog *ui_dh=new  Ui_helpDialog();
	 ui_dh->setupUi((QDialog *)(&dh));
	 dh.exec();
	 delete ui_dh;
    return 0;
}

int GuiSlave::exportEvent ( void ) {
    exportCdcatDB i ( mainw->app,mainw->db );
    i.exec();
    return 0;
}

int GuiSlave::importEvent ( void ) {
    import i ( this );
    return 0;
}

int GuiSlave::openHistoryElementEvent ( int id ) {
    char fnc[256];
    QString fn=mainw->historyMenu->text ( id );

    strcpy ( fnc, ( const char * ) QFile::encodeName ( fn ) );
    while ( closeEvent() != 0 );
    panelsOFF();

    if ( mainw->db == NULL )
        mainw->db = new DataBase();

    PWw *pww = new PWw ( mainw,mainw->app );
    mainw->db->pww = pww;
    progress ( pww );

    if ( mainw->db->openDB ( fnc ) != 0 ) { // An error occured
        QMessageBox::warning ( mainw,tr ( "Error while opening..." ),
                               mainw->db->errormsg );
        mainw->db = NULL;
        standON = NodePwd = NULL;;
        panelsOFF();
    }
    progress ( pww );
    panelsON();
    progress ( pww );

    //QMessageBox::information(0,"new history element",fn);
    if ( ( mainw->cconfig->hlist.grep ( fn ) ).isEmpty() ) {
        mainw->cconfig->hlist.append ( fn );
        mainw->historyMenu->addAction ( *get_t_open_icon(),fn );
    }

    if ( ( int ) mainw->cconfig->hlist.count() > ( int ) mainw->cconfig->historysize ) {
        mainw->cconfig->hlist.remove ( mainw->cconfig->hlist.begin() );
        mainw->historyMenu->removeItemAt ( 0 );
    }

    progress ( pww );
    pww->end();
    if ( mainw->db != NULL )
        mainw->db->pww = NULL;
    delete pww;
    return 0;
}

int GuiSlave::closeBranch ( void ) {
    if ( mainw->db == NULL ) return 0;
    mainw->DirView->closeAllBranch();
    return 0;
}

int GuiSlave::editComment ( void ) {
    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL )   return 0;

    editNodeComment ( standON,mainw );
    cHcaption();
    mainw->commentWidget->repaint();
    return 0;
}

int GuiSlave::showContent ( void ) {
    if ( mainw->db == NULL ) return 0;
    if ( haveContent ( standON ) ) {
        ShowContent *sc = new ShowContent ( standON,mainw,"showcw",true );
        sc->exec();
        delete sc;
    }
    cHcaption();
    return 0;
}

int GuiSlave::addlnkEvent ( void ) {
    if ( mainw->db == NULL ) return 0;

    AddLnk *al = new AddLnk ( this, mainw );
    al->exec();
    if ( al->ok ) {
        panelsOFF();
        mainw->db->addLnk ( ( const char * ) QFile::encodeName ( al->fname->text() ) );
        panelsON();
    }
    delete al;
    cHcaption();
    return 0;
}

int GuiSlave::followLnk ( void ) {
    if ( mainw->db == NULL ) return 0;
    if ( standON->type == HC_CATLNK ) {
        char fnc[256];

        if ( ( ( DBCatLnk * ) ( standON->data ) )->location == NULL ||
                !strcmp ( ( ( DBCatLnk * ) ( standON->data ) )->location,"" ) ) {
            QMessageBox::warning ( mainw,tr ( "Error while opening..." ),
                                   tr ( "The link is empty." ) );

            return 0;
        }


        int ret_val=0;


        strcpy ( fnc, ( const char * ) ( ( DBCatLnk * ) ( standON->data ) )->location );
        while ( closeEvent() !=0 );
        panelsOFF();

        if ( mainw->db == NULL )
            mainw->db = new DataBase();

        PWw *pww = new PWw ( mainw,mainw->app );
        mainw->db->pww = pww;
        progress ( pww );

        if ( ( ret_val=mainw->db->openDB ( fnc ) ) != 0 ) { // An error occured
            QMessageBox::warning ( mainw,tr ( "Error while opening..." ),
                                   mainw->db->errormsg );
            mainw->db = NULL;
            standON = NodePwd = NULL;
            panelsOFF();
        }

        checkversion ( mainw,mainw->db );

        progress ( pww );
        panelsON();
        progress ( pww );

        progress ( pww );
        pww->end();
        if ( mainw->db != NULL )
            mainw->db->pww = NULL;
        delete pww;
    }

    return 0;
}

int GuiSlave::borrowingEvent ( void ) {
    borrowingDialog *d;

    if ( mainw->db == NULL ) return 0;

    d = new borrowingDialog ( mainw->db,mainw,"borrowingdialog",true );
    d->exec();
    mainw->commentWidget->repaint();
    cHcaption();
    return 0;
}

int GuiSlave::sborrowEvent ( void ) {
    borrowDialog *d;

    if ( mainw->db == NULL ) return 0;
    if ( standON == NULL )   return 0;

    d = new borrowDialog ( standON->getNameOf(),mainw,"borrowingdialog",true );
    d->exec();

    if ( d->ok == 1 ) {
        if ( ! ( ( ( DBMedia * ) ( standON->data ) )->borrowing.isEmpty() ) )
            ( ( DBMedia * ) ( standON->data ) )->borrowing = "";
        ( ( DBMedia * ) ( standON->data ) )->borrowing = d->m;
        standON->touchDB();
    }
    mainw->commentWidget->repaint();
    cHcaption();
    return 0;
}

int GuiSlave::cborrowEvent ( void ) {
    int i;
    if ( mainw->db == NULL ) return 0;
    if ( standON   == NULL ) return 0;

    i=QMessageBox::warning ( mainw,tr ( "Confirmation" ),
                             tr ( "Do you want to clear the borrowing mark from media \"%1\"?\n(Say yes if you got it back.)" ).arg ( standON->getNameOf() )
                             ,tr ( "Yes" ),tr ( "No" ) );

    if ( i != 0 ) return 0;
    if ( ! ( ( ( DBMedia * ) ( standON->data ) )->borrowing.isEmpty() ) )
        ( ( DBMedia * ) ( standON->data ) )->borrowing = "";
    standON->touchDB();
    mainw->commentWidget->repaint();
    cHcaption();
    return 0;
}

//*****************************************************************************
// Positioning dialog
//*****************************************************************************

QPosDialog::QPosDialog ( CdCatMainWidget *parent )
        :QDialog ( parent,"searchdd",true,Qt::WStyle_Customize | Qt::WStyle_NoBorder ) {
    QPoint point;
    Q3HBoxLayout *l1;
    Q3VBoxLayout *l2;

    p = parent;
    le = new QLineEdit ( this,"leedit" );

    setBackgroundMode ( Qt::PaletteDark );

    l2 = new Q3VBoxLayout();
    l2->addSpacing ( 2 );
    l2->addWidget ( le );
    l2->addSpacing ( 2 );

    l1 = new Q3HBoxLayout ( this );
    l1->addSpacing ( 2 );
    l1->addLayout ( l2 );
    l1->addSpacing ( 2 );

    point = p->splitMain->mapToGlobal (
                QPoint ( p->listView->x() +5,p->listView->y() +p->listView->height()-40 ) );

    move ( point );

    maxp = point.y() + 40;
    minp = point.y()-100;

    connect ( le,SIGNAL ( returnPressed() ),this,SLOT ( close() ) );
    connect ( le,SIGNAL ( textChanged ( const QString & ) ),this,SLOT ( pos ( const QString & ) ) );
    le->setFocus();
}

int QPosDialog::pos ( const QString & str ) {
    int len;
    char pattit[256];
    DEBUG_INFO_ENABLED = init_debug_info();
    if(*DEBUG_INFO_ENABLED)
	cerr << "QPosDialog::pos() str: " << qPrintable ( str ) << endl;

    strcpy ( pattit, ( const char * ) str );
    len = strlen ( pattit );

    Q3ListViewItemIterator it ( p->listView );

    for ( ; ( it.current() ) != NULL;it++ ) {
        if ( 0 == strncmp ( ( const char * ) ( ( it.current() )->text ( 0 ) ),pattit,len ) ) {
            p->listView->setCurrentItem ( it.current() );
            p->listView->curr_vis();
            p->guis->standOn ( it.current() );
            return 0;
        }
    }
    return 0;
}

void QPosDialog::keyPressEvent ( QKeyEvent *ke ) {
    int yp = y();
    if ( ke->key() == Qt::Key_Escape ||
            ke->key() == Qt::Key_Return ) close();

    if ( ke->key() == Qt::Key_Up )
        yp-=5;
    if ( ke->key() == Qt::Key_Down )
        yp+=5;

    if ( yp < minp ) yp = minp;
    if ( yp > maxp ) yp = maxp;

    move ( x(),yp );

    QDialog::keyPressEvent ( ke );
}

int GuiSlave::posEvent ( void ) {
    QPosDialog *d;

    d=new QPosDialog ( mainw );
    d->exec();
    return 0;
}

int GuiSlave::colorEvent ( void ) {
    ColorSettings *d;

    d=new ColorSettings ( mainw->cconfig,mainw,"cs",true );
    d->exec();
    mainw->repaint();
    mainw->commentWidget->repaint();
    return 0;
}


