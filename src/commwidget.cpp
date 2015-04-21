/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "commwidget.h"

#include <QDebug>
#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QToolButton>
#include <QVariant>
#include <QPushButton>
#include <QTextEdit>
#include <QLayout>
#include <QToolTip>
#include <QStringList>
#include <QRegExp>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QGridLayout>
#include <QMouseEvent>
#include <QEvent>
#include <QVBoxLayout>
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




int cshow = 1;
char *abuff = NULL;

HQToolButton::HQToolButton ( QWidget *parent )
    : QToolButton( parent ) {
    setMaximumSize( 30, 30 );
    setText( QString::null );
}

void HQToolButton::enterEvent( QEvent * ) {
    QApplication::setOverrideCursor( Qt::ArrowCursor );
}

void HQToolButton::leaveEvent( QEvent * ) {
    QApplication::setOverrideCursor( Qt::PointingHandCursor );
}

CommentWidget::CommentWidget ( CdCatConfig *cc, QApplication *appl, QWidget *parent, const char *name, Qt::WindowFlags fl )
    : QWidget( parent, fl ) {
    if (!name) {
        setObjectName( "CommentWidget" );
    }
    cconfig = cc;
    app = appl;
    ox = mx = 0;
    oy = my = 0;
    mode = 0;
    act = NULL;
    sa = NULL;
    paintEventRunning = false;
    update_contents_running = false;
    setMinimumSize( 70, 70 );

    ButtonContent = new HQToolButton( this );
    ButtonContent->setIcon( QIcon( *get_t_showc_icon()));
    ButtonContent->setGeometry( 55, height() - 45, 30, 30 );
    ButtonContent->setToolTip( tr( "Shows the content of the file." ));

    ButtonEdit = new HQToolButton( this );
    ButtonEdit->setIcon( QIcon( *get_t_comment_icon()));
    ButtonEdit->setGeometry( 20, height() - 45, 30, 30 );
    ButtonEdit->setToolTip( tr( "Edit and refresh the actual comment page." ));

//     ButtonCategory = new HQToolButton ( this );
//     ButtonCategory->setIcon ( QIcon(*get_t_showc_icon() ));
//     ButtonCategory->setGeometry ( 120,height()-45,30,30 );
//     QToolTip::add ( ButtonCategory   , tr ( "Show the actual category page." ) );

    ButtonCategoryEdit = new HQToolButton( this );
    ButtonCategoryEdit->setIcon( QIcon( *get_t_comment_icon()));
    ButtonCategoryEdit->setGeometry( 85, height() - 45, 30, 30 );
    ButtonCategoryEdit->setToolTip( tr( "Edit and refresh the actual category page." ));

    connect( ButtonEdit, SIGNAL( clicked()), this, SLOT( editC()));
//     connect ( ButtonCategory,SIGNAL ( clicked() ),this,SLOT ( showCategory()) );
    connect( ButtonCategoryEdit, SIGNAL( clicked()), this, SLOT( editCategory()));
    connect( ButtonContent, SIGNAL( clicked()), this, SLOT( showC()));


    setMinimumSize( QSize( 300, 400 ));
    updateContents();
}

void CommentWidget::setScrollArea( QScrollArea *sa ) {
    this->sa = sa;
}

void CommentWidget::enterEvent( QEvent * ) {
    QApplication::setOverrideCursor( Qt::PointingHandCursor );
}

void CommentWidget::leaveEvent( QEvent * ) {
    QApplication::setOverrideCursor( Qt::ArrowCursor );
}

void CommentWidget::mouseMoveEvent( QMouseEvent *me ) {
    mx -= ox - me->x();
    my -= oy - me->y();
    ox = me->x();
    oy = me->y();
    repaint();
}

void CommentWidget::mousePressEvent( QMouseEvent *me ) {
    ox = me->x();
    oy = me->y();
    mx = 0;
    my = 0;
    repaint();
}

void CommentWidget::mouseReleaseEvent( QMouseEvent * ) {
    ox = mx = 0;
    oy = my = 0;
    repaint();
}

void CommentWidget::updateContents() {
    // if(paintEventRunning)
    //	return;
    update_contents_running = true;
    // QApplication::setOverrideCursor ( Qt::WaitCursor );
    contentsPixmapTmp = QPixmap( width(), height());
    // contentsPixmapTmp.resize(800, 2000);
    contentsPixmapTmp.fill();
    QPainter p( &contentsPixmapTmp );
    QStringList textList;
    QString text;
    QDateTime mod;

    int w = 25;
    int fontwidth = width();     // +200;
    Node *tmp;
    QFontMetrics fm( app->font());
    int pixelsHigh = fm.height();
    int valueoffset = 0;     // text: value
    int ispace = 1;
    bool need_showc_button = false;
    bool need_editc_button = false;

    pixelsHigh -= 1;     // I think it was too big …

    p.setFont( QFont( "Fixed", cconfig->fsize - 1 ));
    p.setPen( cconfig->comm_fr );
    p.fillRect( 10, 10, width() - 20, height() - 20, cconfig->comm_bg /*QColor(255,229,28)*/ );
    p.drawRect( 10, 10, width() - 20, height() - 20 );
    p.setClipRect( 10, 10, width() - 20, height() - 20 );

    /*the .. case */
    if (act != NULL) {
        need_editc_button = true;

        if (mode) {
            p.setPen( cconfig->comm_stext );
            p.drawText( mx + 15, my + w, tr( "Upper container! (..)" ));
            valueoffset = fm.boundingRect( tr( "Upper container! (..)" )).size().width();
            if (mx + 15 + valueoffset > fontwidth) {
                fontwidth = mx + 15 + valueoffset + 20;
            }
            w += pixelsHigh + 10;
        }

        /*name*/
        p.setPen( cconfig->comm_stext );
        p.drawText( mx + 15, my + w, tr( "Name:" ));
        valueoffset = fm.boundingRect( tr( "Name:" )).size().width();
        if (mx + 15 + valueoffset > fontwidth) {
            fontwidth = mx + 15 + valueoffset + 20;
        }
        text = act->getNameOf();
        text.prepend( " " );

        p.setPen( cconfig->comm_vtext );
        p.drawText( mx + 15 + valueoffset + 5, my + w, text );
        w += pixelsHigh + ispace;

        /*media*/
        p.setPen( cconfig->comm_stext );
        p.drawText( mx + 15, my + w, tr( "At media (number/name):" ));
        valueoffset = fm.boundingRect( tr( "At media (number/name):" )).size().width();
        if (mx + 15 + valueoffset > fontwidth) {
            fontwidth = mx + 15 + valueoffset + 20;
        }
        tmp = act;
        while (tmp != NULL && tmp->type != HC_MEDIA && tmp->type != HC_CATALOG)
            tmp = tmp->parent;
        text = " ";
        if (tmp == NULL) {
            text += tr( "Unknown" );
        } else {
            if (tmp->type == HC_CATALOG) {
                text += ((DBCatalog *)(tmp->data))->name;
            }
            if (tmp->type == HC_MEDIA) {
                text += QString().setNum(((DBMedia *)(tmp->data))->number ) + " / " + ((DBMedia *)(tmp->data))->name;
                if (!(((DBMedia *)(tmp->data))->borrowing.isEmpty())) {
                    p.setPen( QPen( QColor( Qt::red ), 2 ));
                    p.drawLine( width() - 24, 14, width() - 14, 24 );
                }
            }
            p.setPen( QPen( cconfig->comm_vtext, 1 ));
            p.drawText( mx + 15 + valueoffset + 5, my + w, text );
            valueoffset = fm.boundingRect( text ).size().width();
            if (mx + 15 + valueoffset > fontwidth) {
                fontwidth = mx + 15 + valueoffset + 20;
            }
            w += pixelsHigh + ispace;

            /*type*/
            p.setPen( cconfig->comm_stext );
            p.drawText( mx + 15, my + w, tr( "Type:" ));
            valueoffset = fm.boundingRect( tr( "Type:" )).size().width();
            if (mx + 15 + valueoffset > fontwidth) {
                fontwidth = mx + 15 + valueoffset + 20;
            }
            switch (act->type) {
            case HC_CATALOG:
                text = tr( " CdCat Catalog root" );
                break;
            case HC_CATLNK:
                text = tr( " Link to a CdCat catalog" );
                break;
            case HC_DIRECTORY:
                text = tr( " Directory" );
                break;
            case HC_FILE:
                text = tr( " File" );
                break;
            case HC_MEDIA:
                text = QString( " " ) + tr( "Media" ) + QString( "" ).sprintf( " (%s)", getMType(((DBMedia *)(act->data))->type ));
                break;
            }
            p.setPen( cconfig->comm_vtext );
            p.drawText( mx + 15 + valueoffset + 5, my + w, text );
            valueoffset = fm.boundingRect( text ).size().width();
            if (mx + 15 + valueoffset > fontwidth) {
                fontwidth = mx + 15 + valueoffset + 20;
            }
            w += pixelsHigh + ispace;

            /*if file->size*/
            if (act->type == HC_FILE) {
                p.setPen( cconfig->comm_stext );
                p.drawText( mx + 15, my + w, tr( "Size:" ));
                valueoffset = fm.boundingRect( tr( "Size:" )).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                text.sprintf( " %.2f ", ((DBFile *)(act->data))->size );
                text += QString( getSType(((DBFile *)(act->data))->sizeType, true ));
                p.setPen( cconfig->comm_vtext );
                p.drawText( mx + 15 + valueoffset + 5, my + w, text );
                valueoffset = fm.boundingRect( text ).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                // w += pixelsHigh + ispace;
                w += pixelsHigh;
            }

            /*if catlnkfile->location*/
            if (act->type == HC_CATLNK) {
                p.setPen( cconfig->comm_stext );
                p.drawText( mx + 15, my + w, tr( "Location:" ));
                valueoffset = fm.boundingRect( tr( "Location:" )).size().width();
                p.setPen( cconfig->comm_vtext );
                text.sprintf( " %s", ((DBCatLnk *)(act->data))->location );
                p.drawText( mx + 15 + valueoffset + 5, my + w, text );
                valueoffset = fm.boundingRect( text ).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                w += pixelsHigh + ispace;
            }

            /*mod.time*/
            if (act->type != HC_CATLNK) {
                p.setPen( cconfig->comm_stext );
                p.drawText( mx + 15, my + w, tr( "Last modification:" ));
                valueoffset = fm.boundingRect( tr( "Last modification:" )).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                switch (act->type) {
                case HC_CATALOG:
                    mod = ((DBCatalog *)(act->data))->modification;
                    break;
                case HC_DIRECTORY:
                    mod = ((DBDirectory *)(act->data))->modification;
                    break;
                case HC_FILE:
                    mod = ((DBFile *)(act->data))->modification;
                    break;
                case HC_MEDIA:
                    mod = ((DBMedia *)(act->data))->modification;
                    break;
                }

                // sprintf(text," %s",((DBMedia *)(act->data))->modification);break;
                text = date_to_str( mod );

                p.setPen( cconfig->comm_vtext );
                p.drawText( mx + 15 + valueoffset + 5, my + w, text );
                valueoffset = fm.boundingRect( text ).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                w += pixelsHigh + ispace;

                /*owner*/
                p.setPen( cconfig->comm_stext );
                p.drawText( mx + 15, my + w, tr( "Owner:" ));
                valueoffset = fm.boundingRect( tr( "Owner:" )).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                tmp = act;
                while (tmp != NULL && tmp->type != HC_MEDIA && tmp->type != HC_CATALOG)
                    tmp = tmp->parent;
                text = " ";
                if (tmp == NULL) {
                    text = tr( "Unknown" );
                }
                if (tmp->type == HC_CATALOG) {
                    text += ((DBCatalog *)(tmp->data))->owner;
                }
                if (tmp->type == HC_MEDIA) {
                    text += ((DBMedia *)(tmp->data))->owner;
                }

                p.setPen( cconfig->comm_vtext );
                p.drawText( mx + 15 + valueoffset + 5, my + w, text );
                valueoffset = fm.boundingRect( text ).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                w += pixelsHigh + ispace;

                if (tmp->type == HC_CATALOG) {
                    QString sortedBy;
                    switch (((DBCatalog *)tmp->data)->sortedBy) {
                    case NUMBER:
                        sortedBy += tr( "Number" );
                        break;
                    case NAME:
                        sortedBy += tr( "Name" );
                        break;
                    case TYPE:
                        sortedBy += tr( "Type" );
                        break;
                    case TIME:
                        sortedBy += tr( "Time" );
                        break;
                    }
                    p.setPen( cconfig->comm_stext );
                    p.drawText( mx + 15, my + w, tr( "sorted by:" ) + " " );
                    valueoffset = fm.boundingRect( tr( "sorted by:" ) + " " ).size().width();
                    if (mx + 15 + valueoffset > fontwidth) {
                        fontwidth = mx + 15 + valueoffset + 20;
                    }
                    p.setPen( cconfig->comm_vtext );
                    p.drawText( mx + 15 + valueoffset + 5, my + w, sortedBy );
                    w += pixelsHigh + ispace;
                }
            }

            /*file properties*/
            if (act->type == HC_FILE && ((DBFile *)(act->data))->prop != NULL) {                      // File props:
                tmp = ((DBFile *)(act->data))->prop;
                while (tmp != NULL) {
                    /*mp3tag*/
                    if (tmp->type == HC_MP3TAG) {
                        p.setPen( cconfig->comm_fr );
                        p.drawLine( 12, my + w - 11, width() - 12, my + w - 11 );
                        p.setPen( cconfig->comm_stext );
                        p.drawText( mx + 12, my + w, tr( "Mp3-Tag:" ));
                        valueoffset = fm.boundingRect( tr( "Mp3-Tag:" )).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        // p.drawText ( mx+12,my+w,tr ( "(Art/Tit/Alb/Year/Comm)" ) );
                        // w+=pixelsHigh+2;
                        p.setPen( cconfig->comm_vtext );
                        text = tr( "Artist:" ) + " " + ((DBMp3Tag *)(tmp->data))->artist;
                        p.drawText( mx + 20, my + w, text );
                        valueoffset = fm.boundingRect( text ).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        text = tr( "Title:" ) + " " + ((DBMp3Tag *)(tmp->data))->title + " (" + tr( "track:" ) + " " + QString().setNum(((DBMp3Tag *)(tmp->data))->tnumber ) + ")";
                        p.drawText( mx + 20, my + w, text );
                        valueoffset = fm.boundingRect( text ).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        text = tr( "Album:" ) + " " + ((DBMp3Tag *)(tmp->data))->album;
                        p.drawText( mx + 20, my + w, text );
                        valueoffset = fm.boundingRect( text ).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        text = tr( "Year:" ) + " " + ((DBMp3Tag *)(tmp->data))->year;
                        p.drawText( mx + 20, my + w, text );
                        valueoffset = fm.boundingRect( text ).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        text = tr( "Comment:" ) + " " + ((DBMp3Tag *)(tmp->data))->comment;
                        p.drawText( mx + 20, my + w, text );
                        valueoffset = fm.boundingRect( text ).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh + 2;
                    }

                    /* exif data */
                    if (tmp->type == HC_EXIF) {
                        p.setPen( cconfig->comm_fr );
                        p.drawLine( 12, my + w - 11, width() - 12, my + w - 11 );
                        p.setPen( cconfig->comm_stext );
                        p.drawText( mx + 12, my + w, tr( "Exif data:" ));
                        valueoffset = fm.boundingRect( tr( "Exif data:" )).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        p.setPen( cconfig->comm_vtext );
                        QStringList ExifData = ((DBExifData *)(tmp->data))->ExifDataList;

                        // qDebug() << "exif data: " << qPrintable ( ExifData.join ( "#" ) );
                        for (int i = 0; i < ExifData.size(); ++i) {
                            text = ExifData.at( i );
                            p.drawText( mx + 20, my + w, text );
                            valueoffset = fm.boundingRect( text ).size().width();
                            if (mx + 15 + valueoffset > fontwidth) {
                                fontwidth = mx + 15 + valueoffset + 20;
                            }
                            w += pixelsHigh;
                        }
                        // w += pixelsHigh;
                        p.setPen( cconfig->comm_stext );
                    }

                    /* thumbnail */
                    if (tmp->type == HC_THUMB) {
                        p.setPen( cconfig->comm_fr );
                        p.drawLine( 12, my + w - 11, width() - 12, my + w - 11 );
                        p.setPen( cconfig->comm_stext );
                        p.drawText( mx + 12, my + w, tr( "Thumbnail:" ));
                        valueoffset = fm.boundingRect( tr( "Thumbnail:" )).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        w += pixelsHigh;
                        p.setPen( cconfig->comm_vtext );
                        p.drawImage( mx + 20, my + w, (((DBThumb *)(tmp->data))->ThumbImage));
                        w += ((((DBThumb *)(tmp->data))->ThumbImage)).height() + 10;
                        w += pixelsHigh;
                    }
                    tmp = tmp->next;
                }
            }

            /* comment */
            switch (act->type) {
            case HC_CATALOG:
                text = ((DBCatalog *)(act->data))->comment;
                break;
            case HC_DIRECTORY:
                text = ((DBDirectory *)(act->data))->comment;
                break;
            case HC_FILE:
                text = ((DBFile *)(act->data))->comment;
                break;
            case HC_MEDIA:
                text = ((DBMedia *)(act->data))->comment;
                break;
            case HC_CATLNK:
                text = ((DBCatLnk *)(act->data))->comment;
                break;
            }
            if (!text.isEmpty()) {
                w++;
                p.setPen( cconfig->comm_fr );
                p.drawLine( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                p.setPen( cconfig->comm_stext );
                w++;
                p.drawText( mx + 15, my + w, tr( "Comment:" ));
                valueoffset = fm.boundingRect( tr( "Comment:" )).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                w += pixelsHigh;
                p.setPen( cconfig->comm_vtext );
                textList = text.split( QRegExp( "#|\n|\r\n" ));
                for (QStringList::Iterator it = textList.begin(); it != textList.end(); ++it) {
                    int max_comment_len = 80;
                    int stringlen = (*it).size();
                    if (stringlen > max_comment_len) {
                        //              qWarning() << "oversized comment line (" << stringlen <<"): " << qPrintable(*it);
                        int curlen = 0;
                        QStringList textList2;
                        for (int curidx = 0; curidx < stringlen; curidx++) {
                            if (curlen == max_comment_len) {
                                textList2.append((*it).mid( curidx - max_comment_len, curidx ));
                                //                              qWarning() << "added sub comment line (" << (*it).mid(curidx-max_comment_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_comment_len, curidx));
                                curlen = 0;
                            }
                            curlen++;
                        }
                        for (QStringList::Iterator it2 = textList2.begin(); it2 != textList2.end(); ++it2) {
                            p.drawText( mx + 20, my + w, (*it2));
                            w += pixelsHigh;
                        }
                    } else {
                        //              qWarning() << "undersized comment line (" << stringlen <<"): " << qPrintable(*it);
                        p.drawText( mx + 20, my + w, (*it));
                        w += pixelsHigh;
                    }
                }
                w++;
                // p.setPen ( cconfig->comm_fr );
                // p.drawLine ( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                // p.setPen ( cconfig->comm_stext );
                // w++;
            }

            /* category */
            switch (act->type) {
            case HC_CATALOG:
                text = ((DBCatalog *)(act->data))->category;
                break;
            case HC_DIRECTORY:
                text = ((DBDirectory *)(act->data))->category;
                break;
            case HC_FILE:
                text = ((DBFile *)(act->data))->category;
                break;
            case HC_MEDIA:
                text = ((DBMedia *)(act->data))->category;
                break;
            case HC_CATLNK:
                text = ((DBCatLnk *)(act->data))->category;
                break;
            }
            if (!text.isEmpty()) {
                // w+=pixelsHigh+2;
                p.setPen( cconfig->comm_stext );
                w++;
                p.setPen( cconfig->comm_fr );
                p.drawLine( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                p.setPen( cconfig->comm_stext );
                w++;
                p.drawText( mx + 15, my + w, tr( "Category:" ));
                valueoffset = fm.boundingRect( tr( "Category:" )).size().width();
                if (mx + 15 + valueoffset > fontwidth) {
                    fontwidth = mx + 15 + valueoffset + 20;
                }
                w += pixelsHigh;
                w++;
                p.setPen( cconfig->comm_vtext );
                textList = text.split( QRegExp( "#|\n|\r\n" ));
                for (QStringList::Iterator it = textList.begin(); it != textList.end(); ++it) {
                    int max_category_len = 80;
                    int stringlen = (*it).size();
                    if (stringlen > max_category_len) {
                        //              qWarning() << "oversized category line (" << stringlen <<"): " << qPrintable(*it);
                        int curlen = 0;
                        QStringList textList2;
                        for (int curidx = 0; curidx < stringlen; curidx++) {
                            if (curlen == max_category_len) {
                                textList2.append((*it).mid( curidx - max_category_len, curidx ));
                                //                              qWarning() << "added sub category line (" << (*it).mid(curidx-max_category_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_category_len, curidx));
                                curlen = 0;
                            }
                            curlen++;
                        }
                        for (QStringList::Iterator it2 = textList2.begin(); it2 != textList2.end(); ++it2) {
                            p.drawText( mx + 20, my + w, (*it2));
                            w += pixelsHigh;
                        }
                    } else {
                        //              qWarning() << "undersized category line (" << stringlen <<"): " << qPrintable(*it);
                        p.drawText( mx + 20, my + w, (*it));
                        w += pixelsHigh;
                    }
                }
                w++;
                // p.setPen ( cconfig->comm_fr );
                // p.drawLine ( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                // p.setPen ( cconfig->comm_stext );
                // w++;
            }

            QFont oldFont;
            oldFont = p.font();

            /* archive content */
            switch (act->type) {
            case HC_FILE:
                QList<ArchiveFile> ArchiveFileList = ((DBFile *)(act->data))->archivecontent;
                if (ArchiveFileList.size() > 0) {
                    w += pixelsHigh + ispace;

                    p.setPen( cconfig->comm_fr );
                    p.drawLine( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                    p.setPen( cconfig->comm_stext );
                    w++;
                    p.drawText( mx + 15, my + w, tr( "Archive contents:" ));
                    valueoffset = fm.boundingRect( tr( "Archive contents:" )).size().width();
                    if (mx + 15 + valueoffset > fontwidth) {
                        fontwidth = mx + 15 + valueoffset + 20;
                    }
                    // w+=pixelsHigh;
                    // w++;
                    p.setFont( QFont( "Fixed", font().pointSize() - 1 ));

                    QTextDocument *doc = new QTextDocument( this );
                    doc->setUndoRedoEnabled( false );
                    QString html = "";
                    html += "<html>";
                    html += "<head>";
                    html += "<style type=\"text/css\">";
                    html += ".tableline{font-size:" + QString().setNum( font().pointSize() - 1 ) + "pt;}";
                    html += "</style>";
                    html += "</head>";
                    html += "<body>";
                    html += "<table colspacing=\"5\" rowspacing=\"5\">";
                    html += "<tr>";
                    if (cconfig->show_archive_file_perms) {
                        html += "<th>";
                        html += tr( "Rights" );
                        html += "</th>";
                    }
                    if (cconfig->show_archive_file_user) {
                        html += "<th>";
                        html += tr( "Owner" );
                        html += "</th>";
                    }
                    if (cconfig->show_archive_file_group) {
                        html += "<th>";
                        html += tr( "Group" );
                        html += "</th>";
                    }
                    if (cconfig->show_archive_file_size) {
                        html += "<th>";
                        html += tr( "Size" );
                        html += "</th>";
                    }
                    if (cconfig->show_archive_file_date) {
                        html += "<th>";
                        html += tr( "Changed" );
                        html += "</th>";
                    }

                    html += "<th>";
                    html += tr( "Path" );
                    html += "</th>";

                    if (cconfig->show_archive_file_comment) {
                        html += "<th>";
                        html += tr( "Comment" );
                        html += "</th>";
                    }
                    html += "</tr>";

                    p.setPen( cconfig->comm_vtext );
                    for (int i = 0; i < ArchiveFileList.size(); i++) {
                        ArchiveFile af = ArchiveFileList.at( i );
                        html += af.toPrettyString( cconfig->show_archive_file_perms, cconfig->show_archive_file_user, cconfig->show_archive_file_group, cconfig->show_archive_file_size, cconfig->show_archive_file_date, cconfig->show_archive_file_comment, true, (font().pointSize() - 1));
                    }

                    html += "</table>";
                    html += "</body>";
                    html += "</html>";

                    doc->setHtml( html );
                    // doc->setTextWidth(width());
                    // doc->setUseDesignMetrics(true);
                    // doc->setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );
                    // doc->drawContents(&p, QRectF(QRect(mx+15+valueoffset, my+w ,doc->size().height() , width())));

                    p.save();
                    p.translate( QPoint( mx + 10, my + w ));
                    doc->setDefaultFont( QFont( "Fixed", font().pointSize() - 1 ));
                    if (doc->size().width() + 30 > fontwidth) {
                        fontwidth = doc->size().width() + 30;
                    }


                    doc->drawContents( &p );
                    p.restore();
                    w += doc->size().height() + 5;
                    // if ( w + doc->size().height() > contentsPixmap.height() )
                    //	contentsPixmap.resize ( fontwidth,  w + doc->size().height() + 5 );
                    /* end archive file list */

                    // w++;
                    p.setPen( cconfig->comm_fr );
                    p.drawLine( 12, my + w, width() - 12, my + w );
                    p.setPen( cconfig->comm_stext );
                    // w++;
                    // w++;
                    // w += pixelsHigh;
                    // w += pixelsHigh;
                }


                break;
                //              default:
                //                      ;
            }
            p.setFont( oldFont );

            /* file info */
            switch (act->type) {
            case HC_FILE:
                QString info = ((DBFile *)(act->data))->fileinfo;
                if (!info.isEmpty()) {
                    w += pixelsHigh;
                    p.setPen( cconfig->comm_fr );
                    p.drawLine( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                    p.setPen( cconfig->comm_stext );
                    w++;
                    p.setPen( cconfig->comm_stext );
                    p.drawText( mx + 15, my + w, tr( "File info:" ));
                    valueoffset = fm.boundingRect( tr( "File info:" )).size().width();
                    if (mx + 15 + valueoffset > fontwidth) {
                        fontwidth = mx + 15 + valueoffset + 20;
                    }
                    w += pixelsHigh;
                    p.setPen( cconfig->comm_vtext );
                    textList = info.split( QRegExp( "#|\n|\r\n" ));
                    for (QStringList::Iterator it = textList.begin(); it != textList.end(); ++it) {
                        int max_fileinfo_len = 80;
                        int stringlen = (*it).size();
                        if (stringlen > max_fileinfo_len) {
                            //          qWarning() << "oversized fileinfo line (" << stringlen <<"): " << qPrintable(*it);
                            int curlen = 0;
                            QStringList textList2;
                            for (int curidx = 0; curidx < stringlen; curidx++) {
                                if (curlen == max_fileinfo_len) {
                                    textList2.append((*it).mid( curidx - max_fileinfo_len, curidx ));
                                    //                          qWarning() << "added sub fileinfo line (" << (*it).mid(curidx-max_fileinfo_len, curidx).length() <<"): " << qPrintable((*it).mid(curidx-max_fileinfo_len, curidx));
                                    curlen = 0;
                                }
                                curlen++;
                            }
                            for (QStringList::Iterator it2 = textList2.begin(); it2 != textList2.end(); ++it2) {
                                text = (*it2);
                                p.drawText( mx + 20, my + w, text );
                                valueoffset = fm.boundingRect( text ).size().width();
                                if (mx + 20 + valueoffset > fontwidth) {
                                    fontwidth = mx + 20 + valueoffset + 20;
                                }
                                w += pixelsHigh;
                            }
                        } else {
                            //          qWarning() << "undersized fileinfo line (" << stringlen <<"): " << qPrintable(*it);
                            text = (*it);
                            p.drawText( mx + 20, my + w, text );
                            valueoffset = fm.boundingRect( text ).size().width();
                            if (mx + 20 + valueoffset > fontwidth) {
                                fontwidth = mx + 20 + valueoffset + 20;
                            }
                            w += pixelsHigh;
                        }
                    }
                    p.setPen( cconfig->comm_fr );
                    p.drawLine( 12, my + w, width() - 12, my + w );
                    p.setPen( cconfig->comm_stext );
                    // w += pixelsHigh;
                }
                break;
            }

            /*Content button stuff*/
            if (act->type == HC_FILE && ((DBFile *)(act->data))->prop != NULL) {
                tmp = ((DBFile *)(act->data))->prop;
                while (tmp != NULL) {
                    if (tmp->type == HC_CONTENT) {
                        need_showc_button = true;
                        w++;
                        p.setPen( cconfig->comm_fr );
                        p.drawLine( 12, my + w - pixelsHigh, width() - 12, my + w - pixelsHigh );
                        p.setPen( cconfig->comm_stext );
                        w++;
                        QString sizestr = "";
                        double size = ((DBContent *)(tmp->data))->storedSize;

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
                        text = sizestr + filetype;
                        p.setPen( cconfig->comm_stext );
                        p.drawText( mx + 15, my + w, tr( "Stored size:" ));
                        valueoffset = fm.boundingRect( tr( "Stored size:" )).size().width();
                        if (mx + 15 + valueoffset > fontwidth) {
                            fontwidth = mx + 15 + valueoffset + 20;
                        }
                        p.setPen( cconfig->comm_vtext );
                        p.drawText( mx + 15 + valueoffset + 5, my + w, text );
                    }
                    tmp = tmp->next;
                }
            }
            w += pixelsHigh;
            w += pixelsHigh;
            w += pixelsHigh;
        }
        /*others …*/
    } else {
        need_editc_button = false;
        p.setPen( cconfig->comm_stext );
        // p.setPen ( cconfig->comm_stext );
        p.setPen( cconfig->comm_vtext );
        p.drawText( mx + 15, my + 25, tr( "There is no selected element." ));
        valueoffset = fm.boundingRect( tr( "There is no selected element:" )).size().width();
        if (mx + 15 + valueoffset > fontwidth) {
            fontwidth = mx + 15 + valueoffset + 20;
        }
    }

    // if ( w > height() || fontwidth > width() )
    //	resize ( fontwidth, w );
    w += 5;
    // w += pixelsHigh;

    p.end();



    if (!need_editc_button) {
        ButtonEdit->setEnabled( false );
    }

    if (!need_showc_button) {
        ButtonContent->setEnabled( false );
        // disconnect
    }

    if (need_showc_button) {
        ButtonContent->setEnabled( true );
        // connect
    }

    ButtonEdit->setGeometry( 20, w, 30, 30 );
    ButtonContent->setGeometry( 55, w, 30, 30 );
    ButtonCategoryEdit->setGeometry( 85, w, 30, 30 );


    ButtonContent->show();
    ButtonEdit->show();


    // if ( sa != NULL )
    //	sa->ensureVisible ( 0, 0 );

    w += 45;

    if (!paintEventRunning) {
        int new_height = w;
        int new_width = fontwidth;
        if (width() >= fontwidth) {
            new_width = width();
        }
        if (height() >= w) {
            new_height = height();
        }
        contentsPixmapTmp.scaled( QSize( fontwidth, w ), Qt::KeepAspectRatio );
        if (sa != NULL) {
            sa->widget()->resize( QSize( new_width, new_height ));
        }
        contentsPixmap = contentsPixmapTmp;

        update_contents_running = false;
        repaint();
    } else {
        update_contents_running = false;
    }
    // QApplication::restoreOverrideCursor();
}

void CommentWidget::paintEvent( QPaintEvent * ) {
    // qWarning();
    if (update_contents_running) {
        return;
    }
    paintEventRunning = true;
    QPainter p( this );
    // qWarning() << "end";
    p.drawPixmap( p.viewport(), contentsPixmap );
#if (QT_VERSION >= QT_VERSION_CHECK( 4, 6, 0 )) // needs Qt 4.6.0 or better
    // p.beginNativePainting();
    // p.endNativePainting();
#endif
    p.end();
    paintEventRunning = false;
}

void CommentWidget::resizeEvent( QResizeEvent * ) {
    // ButtonEdit   ->setGeometry ( 20, ( ( re->size() ).height() ) - 45, 30, 30 );
    // ButtonContent->setGeometry ( 55, ( ( re->size() ).height() ) - 45, 30, 30 );
    // ButtonCategoryEdit->setGeometry ( 85, ( ( re->size() ).height() ) - 45, 30, 30 );

//     ButtonCategory->setGeometry ( 120, ( ( re->size() ).height() )-45,30,30 );
    // resize(QSize(sa->viewport()->size().width()-45, sa->viewport()->size().height()-45));
    // if(old_x != x() || old_y != y() || old_width != width() || old_height != height())
    if (!update_contents_running) {
        // resize(re->size());
        // sa->resize(sizeHint());
        // contentsPixmap.resize(sa->viewport()->size());
        // if(!paintEventRunning)
        updateContents();
    }
}

void CommentWidget::showNode( Node *node, int mod ) {
    act = node;
    mode = mod;
//     erase();
    repaint();
}

int CommentWidget::editC( void ) {
    editNodeComment( act, this, cconfig );
    emit touchdb();
    repaint();
    return 0;
}

int CommentWidget::showC( void ) {
    ShowContent *sc = new ShowContent( act, false, cconfig, this, "showcw", true );

    sc->exec();
    emit touchdb();
    repaint();
    delete sc;
    return 0;
}


int CommentWidget::editCategory( void ) {
    editNodeComment( act, this, cconfig, false );
    emit touchdb();
    repaint();
    return 0;
}

int CommentWidget::showCategory( void ) {
    ShowContent *sc = new ShowContent( act, true, cconfig, this, "showcw", true );

    sc->exec();
    emit touchdb();
    repaint();
    delete sc;
    return 0;
}

/****************************************************************************************/

commentEdit::commentEdit ( QString cc, CdCatConfig *cconfig, QWidget *parent, const char *name, bool modal, bool isCommentEdit, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    setModal( modal );
    OK = 0;
    this->isCommentEdit = isCommentEdit;
    this->cconfig = cconfig;
    qDebug() << "commentEdit cconfig " << this->cconfig;
    if (!name) {
        setObjectName( "commentEdit" );
    }
    setWindowIcon( *get_t_comment_icon());

    setBaseSize( QSize( 300, 150 ));
    setSizeGripEnabled( true );
    CommentEditBaseLayout = new QGridLayout( this );

    layout5 = new QVBoxLayout( this );

    teComm = new QTextEdit( this );
    layout5->addWidget( teComm );

    layout4 = new QHBoxLayout( this );
    QSpacerItem *spacer = new QSpacerItem( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer );

    buttonOk = new QPushButton( this );
    buttonOk->setMinimumSize( QSize( 80, 0 ));
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
    layout4->addWidget( buttonOk );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumSize( QSize( 80, 0 ));
    buttonCancel->setAutoDefault( true );
    layout4->addWidget( buttonCancel );
    QSpacerItem *spacer_2 = new QSpacerItem( 120, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_2 );
    layout5->addLayout( layout4 );

    CommentEditBaseLayout->addLayout( layout5, 0, 0 );
    languageChange();
    resize( QSize( cconfig->commentWindowSize_width, cconfig->commentWindowSize_height ).expandedTo( minimumSizeHint()));
    move( cconfig->commentWindowPos_x, cconfig->commentWindowPos_y );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked()), this, SLOT( pushOk()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( pushCancel()));

    teComm->setText( cc.replace( QRegExp( "#" ), "\n" ));
}

/*
 *  Destroys the object and frees any allocated resources
 */
commentEdit::~commentEdit() {
    bool need_save = false;

    if (x() != cconfig->commentWindowPos_x || y() != cconfig->commentWindowPos_y || width() != cconfig->commentWindowSize_width || cconfig->commentWindowSize_height) {
        need_save = true;
    }
    cconfig->commentWindowPos_x = x();
    cconfig->commentWindowPos_y = y();
    cconfig->commentWindowSize_width = width();
    cconfig->commentWindowSize_height = height();
    if (need_save) {
        cconfig->writeConfig();
    }
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void commentEdit::languageChange() {
    if (isCommentEdit) {
        setWindowTitle( tr( "Edit comment of" ));
    } else {
        setWindowTitle( tr( "Edit category of" ));
    }
    buttonCancel->setText( tr( "Cancel" ));
    buttonOk->setText( tr( "OK" ));
}

int commentEdit::pushOk( void ) {
    OK = 1;
    newc = teComm->toPlainText();
    close();
    return 0;
}

int commentEdit::pushCancel( void ) {
    OK = 0;
    close();
    return 0;
}

int editNodeComment( Node *node, QWidget *parent, CdCatConfig *cconfig, bool isCommentEdit ) {
    QString o, n, newCaption;

    if (node == NULL) {
        return 0;
    }
    if (isCommentEdit) {
        switch (node->type) {
        case HC_CATALOG:
            o = ((DBCatalog *)(node->data))->comment;
            newCaption = node->getFullPath();
            break;
        case HC_DIRECTORY:
            o = ((DBDirectory *)(node->data))->comment;
            newCaption = node->getFullPath();
            break;
        case HC_FILE:
            o = ((DBFile *)(node->data))->comment;
            newCaption = node->getFullPath();
            break;
        case HC_MEDIA:
            o = ((DBMedia *)(node->data))->comment;
            newCaption = node->getFullPath();
            break;
        case HC_CATLNK:
            o = ((DBCatLnk *)(node->data))->comment;
            newCaption = node->getFullPath();
            break;
        }
    } else {
        switch (node->type) {
        case HC_CATALOG:
            o = ((DBCatalog *)(node->data))->category;
            newCaption = node->getFullPath();
            break;
        case HC_DIRECTORY:
            o = ((DBDirectory *)(node->data))->category;
            newCaption = node->getFullPath();
            break;
        case HC_FILE:
            o = ((DBFile *)(node->data))->category;
            newCaption = node->getFullPath();
            break;
        case HC_MEDIA:
            o = ((DBMedia *)(node->data))->category;
            newCaption = node->getFullPath();
            break;
        case HC_CATLNK:
            o = ((DBCatLnk *)(node->data))->category;
            newCaption = node->getFullPath();
            break;
        }
    }

    commentEdit ce( o, cconfig, parent, "commentEdit", true, isCommentEdit );
    ce.setWindowTitle( ce.windowTitle() + " " + newCaption );
    ce.teComm->setText( o );
    ce.exec();
    if (ce.OK == 0) {
        return 0;
    }
    if (isCommentEdit) {
        switch (node->type) {
        case HC_CATALOG:
            ((DBCatalog *)(node->data))->comment = (ce.newc);
            break;
        case HC_DIRECTORY:
            ((DBDirectory *)(node->data))->comment = (ce.newc);
            break;
        case HC_FILE:
            ((DBFile *)(node->data))->comment = (ce.newc);
            break;
        case HC_MEDIA:
            ((DBMedia *)(node->data))->comment = (ce.newc);
            break;
        case HC_CATLNK:
            ((DBCatLnk *)(node->data))->comment = (ce.newc);
            break;
        }
    } else {
        switch (node->type) {
        case HC_CATALOG:
            ((DBCatalog *)(node->data))->category = (ce.newc);
            break;
        case HC_DIRECTORY:
            ((DBDirectory *)(node->data))->category = (ce.newc);
            break;
        case HC_FILE:
            ((DBFile *)(node->data))->category = (ce.newc);
            break;
        case HC_MEDIA:
            ((DBMedia *)(node->data))->category = (ce.newc);
            break;
        case HC_CATLNK:
            ((DBCatLnk *)(node->data))->category = (ce.newc);
            break;
        }
    }
    node->touchDB();
    return 0;
}
