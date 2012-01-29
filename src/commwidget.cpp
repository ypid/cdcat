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
#include <QLocale>
#include <QScrollArea>

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

void  HQToolButton::enterEvent ( QEvent * ) {
    QApplication::setOverrideCursor ( Qt::arrowCursor );
}

void  HQToolButton::leaveEvent ( QEvent * ) {
    QApplication::setOverrideCursor ( Qt::PointingHandCursor );
}

CommentWidget::CommentWidget ( CdCatConfig * cc,QApplication *appl,QWidget *parent,const char *name, Qt::WFlags fl)
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

//     ButtonCategory = new HQToolButton ( this );
//     ButtonCategory->setPixmap ( *get_t_showc_icon() );
//     ButtonCategory->setGeometry ( 120,height()-45,30,30 );
//     QToolTip::add ( ButtonCategory   , tr ( "Show the actual category page." ) );

    ButtonCategoryEdit = new HQToolButton ( this );
    ButtonCategoryEdit->setPixmap ( *get_t_comment_icon() );
    ButtonCategoryEdit->setGeometry ( 85,height()-45,30,30 );
    QToolTip::add ( ButtonCategoryEdit   , tr ( "Edit and refresh the actual category page." ) );

    connect ( ButtonEdit,SIGNAL ( clicked() ),this,SLOT ( editC() ) );
//     connect ( ButtonCategory,SIGNAL ( clicked() ),this,SLOT ( showCategory()) );
    connect ( ButtonCategoryEdit,SIGNAL ( clicked() ),this,SLOT ( editCategory()) );
    connect ( ButtonContent,SIGNAL ( clicked() ),this,SLOT ( showC() ) );


    setMinimumSize(QSize(200, 400));
    
}

void CommentWidget::setScrollArea(QScrollArea *sa) {
	this->sa = sa;
}

void  CommentWidget::enterEvent ( QEvent * ) {
    QApplication::setOverrideCursor ( Qt::PointingHandCursor );
}

void  CommentWidget::leaveEvent ( QEvent * ) {
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

void  CommentWidget::mouseReleaseEvent ( QMouseEvent * ) {
    ox = mx = 0;
    oy = my = 0;
    repaint();
}

void  CommentWidget::paintEvent ( QPaintEvent * ) {
//cerr <<"paintEvent"<<endl;
    QStringList textList;
    QString text;
    QDateTime mod;
    QPainter p ( this );
    int w=25;
    int fontwidth=width()+200;
    Node *tmp;
    QFontMetrics fm ( app->font() );
    int pixelsHigh = fm.height();
    int valueoffset = 0; // text: value
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
	valueoffset =  fm.boundingRect ( tr("Name:" )).size().width();
        text=act->getNameOf();
        text.prepend ( " " );
        p.setPen ( *cconfig->comm_vtext );
        p.drawText ( mx+15+valueoffset, my+w,text );
        w+=pixelsHigh+ispace;

            /*media*/
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15, my+w, tr ( "At media (number/name):" ) );
            valueoffset =  fm.boundingRect ( tr ( "At media (number/name):" )).size().width();
            tmp=act;
            while ( tmp !=NULL && tmp->type != HC_MEDIA && tmp->type != HC_CATALOG )
                tmp=tmp->parent;
            text=" ";
            if ( tmp==NULL )
                text+=tr ( "Unknown" );
            if ( tmp->type==HC_CATALOG )
                text+= ( ( DBCatalog * ) ( tmp->data ) )->name;
            if ( tmp->type==HC_MEDIA ) {
                text+= QString().setNum ( ( ( DBMedia * ) ( tmp->data ) )->number ) + " / " + ( ( DBMedia * ) ( tmp->data ) )->name;
                if ( ! ( ( ( DBMedia * ) ( tmp->data ) )->borrowing.isEmpty() ) ) {
                    p.setPen ( QPen ( QColor ( Qt::red ),2 ) );
                    p.drawLine ( width()-24,14,width()-14,24 );
                }
            }
            p.setPen ( QPen ( *cconfig->comm_vtext,1 ) );
            p.drawText ( mx+15+valueoffset, my+w, text );
            w+=pixelsHigh+ispace;

        /*type*/
        p.setPen ( *cconfig->comm_stext );
        p.drawText ( mx+15,my+w,tr ( "Type:" ) );
	valueoffset =  fm.boundingRect ( tr ( "Type:" )).size().width();
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
        p.drawText ( mx+15+valueoffset, my+w, text );
        w+=pixelsHigh+ispace;



        /*if file->size*/
        if ( act->type == HC_FILE ) {
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15, my+w, tr ( "Size:" ) );
            valueoffset =  fm.boundingRect ( tr ( "Size:" )).size().width();
            text.sprintf ( " %.2f ", ( ( DBFile * ) ( act->data ) )->size);
            text+=QString(getSType ( ( ( DBFile * ) ( act->data ) )->sizeType, true ));
            p.setPen ( *cconfig->comm_vtext );
            p.drawText ( mx+15+valueoffset, my+w, text );
            w+=pixelsHigh+ispace;
        }

        /*if catlnkfile->location*/
        if ( act->type == HC_CATLNK ) {
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Location:" ) );
            valueoffset =  fm.boundingRect ( tr ( "Location:" )).size().width();
            p.setPen ( *cconfig->comm_vtext );
            text.sprintf ( " %s", ( ( DBCatLnk * ) ( act->data ) )->location );
            p.drawText ( mx+15+valueoffset, my+w, text );
            w+=pixelsHigh+ispace;
        }

        if ( act->type != HC_CATLNK ) {
            /*mod.time*/

            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Last modification:" ) );
            valueoffset =  fm.boundingRect ( tr ( "Last modification:" )).size().width();
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
            p.drawText ( mx+15+valueoffset, my+w, text );
            w+=pixelsHigh+ispace;
            /*owner*/
            p.setPen ( *cconfig->comm_stext );
            p.drawText ( mx+15,my+w,tr ( "Owner:" ) );
            valueoffset =  fm.boundingRect ( tr ( "Owner:" )).size().width();
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
            p.drawText ( mx+15+valueoffset, my+w, text );
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
//                     p.drawText ( mx+12,my+w,tr ( "(Art/Tit/Alb/Year/Comm)" ) );
//                     w+=pixelsHigh+2;
                    p.setPen ( *cconfig->comm_vtext );
                    p.drawText ( mx+20,my+w, tr("Artist:")+" "+( ( DBMp3Tag * ) ( tmp->data ) )->artist );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, tr("Title:")+" "+( ( DBMp3Tag * ) ( tmp->data ) )->title+ " ("+tr("track:")+" "+QString().setNum((( DBMp3Tag * ) ( tmp->data ) )->tnumber)+")");
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, tr("Album:")+" "+( ( DBMp3Tag * ) ( tmp->data ) )->album );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, tr("Year:")+" "+( ( DBMp3Tag * ) ( tmp->data ) )->year );
                    w+=pixelsHigh;
                    p.drawText ( mx+20,my+w, tr("Comment:")+" "+( ( DBMp3Tag * ) ( tmp->data ) )->comment );
                    w+=pixelsHigh+2;
                }
                tmp = tmp->next;
            }
        }

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
	if (!text.isEmpty()) {
		w++;
		p.setPen ( *cconfig->comm_fr );
		p.drawLine ( 12,my+w-pixelsHigh,width()-12,my+w-pixelsHigh );
		p.setPen ( *cconfig->comm_stext );
		w++;
		p.drawText ( mx+15,my+w,tr ( "Comment:" ) );
		w+=pixelsHigh;
		p.setPen ( *cconfig->comm_vtext );
		textList = QStringList::split ( QRegExp ( "#|\n|\r\n" ),text,TRUE );
		for ( QStringList::Iterator it=textList.begin(); it != textList.end();++it ) {
			int max_comment_len =80;
			int stringlen = (*it).size();
			if(stringlen > max_comment_len) {
		// 		cerr << "oversized comment line (" << stringlen <<"): " << qPrintable(*it) << endl;
				int curlen=0;
				QStringList textList2;
				for (int curidx=0;curidx < stringlen;curidx++) {
					if(curlen == max_comment_len) {
						textList2.append((*it).mid(curidx-max_comment_len, curidx));
		// 				cerr << "added sub comment line (" << (*it).mid(curidx-max_comment_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_comment_len, curidx)) << endl;
						curlen =0;
					}
					curlen++;
				}
				for ( QStringList::Iterator it2=textList2.begin(); it2 != textList2.end();++it2 ) {
					p.drawText ( mx+20,my+w, ( *it2 ) );
					w+=pixelsHigh;
				}
			}
			else {
		// 		cerr << "undersized comment line (" << stringlen <<"): " << qPrintable(*it) << endl;
				p.drawText ( mx+20,my+w, ( *it ) );
				w+=pixelsHigh;
			}
		}
		w++;
		p.setPen ( *cconfig->comm_fr );
		p.drawLine ( 12,my+w-pixelsHigh,width()-12,my+w-pixelsHigh );
		p.setPen ( *cconfig->comm_stext );
		
	}
        
        switch ( act->type ) {
        case HC_CATALOG  :
            text= ( ( DBCatalog   * ) ( act->data ) )->category;
            break;
        case HC_DIRECTORY:
            text= ( ( DBDirectory * ) ( act->data ) )->category;
            break;
        case HC_FILE:
            text= ( ( DBFile      * ) ( act->data ) )->category;
            break;
        case HC_MEDIA:
            text= ( ( DBMedia     * ) ( act->data ) )->category;
            break;
        case HC_CATLNK:
            text= ( ( DBCatLnk    * ) ( act->data ) )->category;
            break;
        }

	if (! text.isEmpty()) {
		w+=pixelsHigh+2;
		p.setPen ( *cconfig->comm_stext );
		w++;
		p.drawText ( mx+15,my+w,tr ( "Category:" ) );
// 		w+=pixelsHigh;
		w++;
		p.setPen ( *cconfig->comm_vtext );
		textList = QStringList::split ( QRegExp ( "#|\n|\r\n" ),text,TRUE );
		for ( QStringList::Iterator it=textList.begin(); it != textList.end();++it ) {
			int max_category_len = 80;
			int stringlen = (*it).size();
			if(stringlen > max_category_len) {
		// 		cerr << "oversized category line (" << stringlen <<"): " << qPrintable(*it) << endl;
				int curlen=0;
				QStringList textList2;
				for (int curidx=0;curidx < stringlen;curidx++) {
					if(curlen == max_category_len) {
						textList2.append((*it).mid(curidx-max_category_len, curidx));
		// 				cerr << "added sub category line (" << (*it).mid(curidx-max_category_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_category_len, curidx)) << endl;
						curlen =0;
					}
					curlen++;
				}
				for ( QStringList::Iterator it2=textList2.begin(); it2 != textList2.end();++it2 ) {
					p.drawText ( mx+20,my+w, ( *it2 ) );
					w+=pixelsHigh;
				}
			}
			else {
		// 		cerr << "undersized category line (" << stringlen <<"): " << qPrintable(*it) << endl;
				p.drawText ( mx+20,my+w, ( *it ) );
				w+=pixelsHigh;
			}
		
		}
		w++;
		p.setPen ( *cconfig->comm_fr );
		p.drawLine ( 12,my+w-pixelsHigh,width()-12,my+w-pixelsHigh );
		p.setPen ( *cconfig->comm_stext );
		w++;
		
	}
	QFont oldFont = p.font();
        switch ( act->type ) {
		case HC_FILE:
			QList<ArchiveFile> ArchiveFileList = ( ( DBFile      * ) ( act->data ) )->archivecontent;
			if (ArchiveFileList.size() > 0) {
				w+=pixelsHigh+ispace;
				p.setFont(QFont("Fixed", font().pointSize()-1));
				p.setPen ( *cconfig->comm_fr );
				p.drawLine ( 12,my+w-pixelsHigh,width()-12,my+w-pixelsHigh );
				p.setPen ( *cconfig->comm_stext );
				w++;
				p.drawText ( mx+15,my+w,tr ( "Archive contents:" ) );
 				w+=pixelsHigh;
				//w++;
				QString archive_header;
				if (cconfig->show_archive_file_perms)
					archive_header += tr("Rights")+"\t";
				
				if(cconfig->show_archive_file_user)
					archive_header += tr("Owner")+"\t";
				
				if(cconfig->show_archive_file_group)
					archive_header += tr("Group")+"\t";
				
				if(cconfig->show_archive_file_size)
					archive_header += tr("Size")+"\t";
				
				if(cconfig->show_archive_file_date)
					archive_header += tr("Changed")+"\t\t";
				
				if(cconfig->show_archive_file_comment)
					archive_header += tr("Comment")+"\t";
				
				p.drawText (mx+15,my+w, archive_header );
				
				p.setPen ( *cconfig->comm_vtext );
				for ( int i=0;i<ArchiveFileList.size();i++ ) {
					ArchiveFile af = ArchiveFileList.at(i);
					QString prettyArchiveFileLine = af.toPrettyString(cconfig->show_archive_file_perms, cconfig->show_archive_file_user, cconfig->show_archive_file_group, cconfig->show_archive_file_size, cconfig->show_archive_file_date, cconfig->show_archive_file_comment);
					int max_archivecontent_len =200;
					int stringlen = (prettyArchiveFileLine).size();
					if(stringlen > max_archivecontent_len) {
				// 		cerr << "oversized archivecontent line (" << stringlen <<"): " << qPrintable(*it) << endl;
						int curlen=0;
						QStringList textList2;
						for (int curidx=0;curidx < stringlen;curidx++) {
							if(curlen == max_archivecontent_len) {
								textList2.append(prettyArchiveFileLine.mid(curidx-max_archivecontent_len, curidx));
				// 				cerr << "added sub archivecontent line (" << prettyArchiveFileLine.mid(curidx-max_archivecontent_len, curidx).length() <<"): " << qPrintable(prettyArchiveFileLine.mid(curidx-max_archivecontent_len, curidx)) << endl;
								curlen =0;
							}
							curlen++;
						}
						for ( QStringList::Iterator it2=textList2.begin(); it2 != textList2.end();++it2 ) {
							w+=pixelsHigh;
							p.drawText ( mx+15,my+w, ( *it2 ) );
							if (p.fontMetrics().boundingRect(*it2).width() > fontwidth)
								fontwidth = p.fontMetrics().boundingRect(*it2).width()+mx+20;
						}
					}
					else {
				// 		cerr << "undersized archivecontent line (" << stringlen <<"): " << qPrintable(prettyArchiveFileLine) << endl;
						w+=pixelsHigh;
						p.drawText ( mx+15,my+w, prettyArchiveFileLine );
						if (p.fontMetrics().boundingRect(prettyArchiveFileLine).width() > fontwidth)
							fontwidth = p.fontMetrics().boundingRect(prettyArchiveFileLine).width()+mx+20;
					}
				}
				w++;
				p.setPen ( *cconfig->comm_fr );
				p.drawLine ( 12,my+w,width()-12, my+w );
				p.setPen ( *cconfig->comm_stext );
				w++;
				w++;
				
			}
			break;
// 		default:
// 			;
        }
	p.setFont(oldFont);
	
        switch ( act->type ) {
		case HC_FILE :
			QString info = ( ( DBFile      * ) ( act->data ) )->fileinfo;
			if (!info.isEmpty()) {
// 				w+=pixelsHigh;
				p.setPen ( *cconfig->comm_stext );
				p.drawText ( mx+15,my+w,tr ( "File info:" ) );
				w+=pixelsHigh;
				p.setPen ( *cconfig->comm_vtext );
				textList = QStringList::split ( QRegExp ( "#|\n|\r\n" ),info,TRUE );
				for ( QStringList::Iterator it=textList.begin(); it != textList.end();++it ) {
				int max_fileinfo_len = 80;
				int stringlen = (*it).size();
				if(stringlen > max_fileinfo_len) {
			// 		cerr << "oversized fileinfo line (" << stringlen <<"): " << qPrintable(*it) << endl;
					int curlen=0;
					QStringList textList2;
					for (int curidx=0;curidx < stringlen;curidx++) {
						if(curlen == max_fileinfo_len) {
							textList2.append((*it).mid(curidx-max_fileinfo_len, curidx));
			// 				cerr << "added sub fileinfo line (" << (*it).mid(curidx-max_fileinfo_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_fileinfo_len, curidx)) << endl;
							curlen =0;
						}
						curlen++;
					}
					for ( QStringList::Iterator it2=textList2.begin(); it2 != textList2.end();++it2 ) {
						p.drawText ( mx+20,my+w, ( *it2 ) );
						w+=pixelsHigh;
					}
				}
				else {
			// 		cerr << "undersized fileinfo line (" << stringlen <<"): " << qPrintable(*it) << endl;
					p.drawText ( mx+20,my+w, ( *it ) );
					w+=pixelsHigh;
					
				}
			}
			p.setPen ( *cconfig->comm_fr );
			p.drawLine ( 12,my+w,width()-12, my+w );
			p.setPen ( *cconfig->comm_stext );
			w++;
		}
		break;
		
	}
	
	w+=pixelsHigh;
	w+=pixelsHigh;
	w+=pixelsHigh;

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

    if(w > height())
	resize(fontwidth, w);

    ButtonContent->show();
    ButtonEdit->show();
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)) // needs Qt 4.6.0 or better
    p.beginNativePainting();
#endif

    //delete text;
//cerr <<"paintEvent-end"<<endl;
}

void  CommentWidget::resizeEvent ( QResizeEvent *re ) {
    ButtonEdit   ->setGeometry ( 20, ( ( re->size() ).height() )-45,30,30 );
    ButtonContent->setGeometry ( 55, ( ( re->size() ).height() )-45,30,30 );
    ButtonCategoryEdit->setGeometry ( 85, ( ( re->size() ).height() )-45,30,30 );
//     ButtonCategory->setGeometry ( 120, ( ( re->size() ).height() )-45,30,30 );
	resize(QSize(sa->viewport()->size()));
}

void CommentWidget::showNode ( Node *node,int mod ) {
    act  = node;
    mode = mod;
//     erase();
    repaint();
}

int CommentWidget::editC ( void ) {
    editNodeComment ( act,this );
    emit touchdb();
    repaint();
    return 0;
}

int CommentWidget::showC ( void ) {
    ShowContent *sc = new ShowContent ( act, false, this,"showcw",true );
    sc->exec();
    emit touchdb();
    repaint();
    delete sc;
    return 0;
}


int CommentWidget::editCategory ( void ) {
    editNodeComment ( act,this, false );
    emit touchdb();
    repaint();
    return 0;
}

int CommentWidget::showCategory ( void ) {
    ShowContent *sc = new ShowContent ( act, true,this,"showcw",true );
    sc->exec();
    emit touchdb();
    repaint();
    delete sc;
    return 0;
}

/****************************************************************************************/

commentEdit::commentEdit ( QString cc, QWidget* parent, const char* name, bool modal, bool isCommentEdit, Qt::WFlags fl)
        : QDialog ( parent, name, modal, fl )

{
    OK=0;
    this->isCommentEdit = isCommentEdit;
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
    resize ( QSize ( 600, 199 ).expandedTo ( minimumSizeHint() ) );

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
    if(isCommentEdit)
	setCaption ( tr ( "Edit comment of" ) );
    else
	setCaption ( tr ( "Edit category of" ) );
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

int editNodeComment ( Node *node,QWidget *parent, bool isCommentEdit ) {
    commentEdit *ce;
    QString o, n, newCaption;

    if ( node == NULL ) return 0;

    ce = new commentEdit ( o,parent,"commentEdit",true, isCommentEdit );
    if(isCommentEdit) {
	switch ( node->type ) {
	case HC_CATALOG  :
		o = ( ( DBCatalog   * ) ( node->data ) )->comment;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_DIRECTORY:
		o = ( ( DBDirectory * ) ( node->data ) )->comment;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_FILE     :
		o = ( ( DBFile      * ) ( node->data ) )->comment;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_MEDIA    :
		o = ( ( DBMedia     * ) ( node->data ) )->comment;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_CATLNK   :
		o = ( ( DBCatLnk    * ) ( node->data ) )->comment;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	}
    }
    else {
	switch ( node->type ) {
	case HC_CATALOG  :
		o = ( ( DBCatalog   * ) ( node->data ) )->category;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_DIRECTORY:
		o = ( ( DBDirectory * ) ( node->data ) )->category;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_FILE     :
		o = ( ( DBFile      * ) ( node->data ) )->category;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_MEDIA    :
		o = ( ( DBMedia     * ) ( node->data ) )->category;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	case HC_CATLNK   :
		o = ( ( DBCatLnk    * ) ( node->data ) )->category;
		newCaption =  ce->caption()+" "+node->getFullPath();
		break;
	}
    }
    
    ce->setCaption(newCaption);
    ce->teComm->setText(o);
    ce->exec();
    if ( ce->OK == 0 ) return 0;
    if(isCommentEdit) {
	switch ( node->type ) {
	case HC_CATALOG  :
		( ( DBCatalog * ) ( node->data ) )->comment = ( ce->newc );
		break;

	case HC_DIRECTORY:
		( ( DBDirectory * ) ( node->data ) )->comment = ( ce->newc );
		break;

	case HC_FILE     :
		( ( DBFile * ) ( node->data ) )->comment = ( ce->newc );
		break;

	case HC_MEDIA    :
		( ( DBMedia * ) ( node->data ) )->comment = ( ce->newc );
		break;

	case HC_CATLNK   :
		( ( DBCatLnk * ) ( node->data ) )->comment  = ( ce->newc );
		break;
	}
    }
    else {
	switch ( node->type ) {
	case HC_CATALOG  :
		( ( DBCatalog * ) ( node->data ) )->category   = ( ce->newc );
		break;

	case HC_DIRECTORY:
		( ( DBDirectory * ) ( node->data ) )->category = ( ce->newc );
		break;

	case HC_FILE     :
		( ( DBFile * ) ( node->data ) )->category = ( ce->newc );
		break;

	case HC_MEDIA    :
		( ( DBMedia * ) ( node->data ) )->category = ( ce->newc );
		break;

	case HC_CATLNK   :
		( ( DBCatLnk * ) ( node->data ) )->category = ( ce->newc );
		break;
	}
    }
    node->touchDB();
    return 0;
}

