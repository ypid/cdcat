/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "config.h"

#include <qstring.h>
#include <qmessagebox.h>
#include <qwidget.h>
#include <qvariant.h>
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <q3whatsthis.h>
#include <qfile.h>
#include <qfont.h>
#include <qcolor.h>
#include <q3frame.h>
#include <qtextstream.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <qtranslator.h>
#include <qtextcodec.h>

#include "dbase.h"
#include "icons.h"
#include "cdcat.h"
#include "adddialog.h"
#include "mainwidget.h"
#include "selreadable.h"

#ifdef _WIN32
#define CONFIGFILE "cdcat.cfg"
#else
#define CONFIGFILE ".cdcatconf"
#endif

using namespace std;



CdCatConfig::CdCatConfig ( void ) {

// The default values of config.
// If you delete the config file (or prats of file)
// the values will be set to these:

    startpar   = false;
    startfn    = "";

    ownfont    = false;
    fsize      = 8;
    historysize= 10;
    autoload   = false;
    autoloadfn = "";
    nice       = true;
    // set dummy
#ifdef _WIN32
    cdrompath = "D:/";
    lastDir = getenv("USER_PROFILE");
#else
    cdrompath = "/dev/sr0";
    lastDir = getenv("HOME");
#endif
    
    hlist.clear();
    autosave   = false;
    debug_info_enabled = false;
    readtag    = true;
    v1_over_v2 = false;
    readinfo   = true;
    readcontent = false;
    readcfiles = "*.nfo;*.diz";
    readclimit = 32*1024;
    lastMediaType = CD;
    saveAlwaysCatalogInUtf8 = true;
    showProgressedFileInStatus = true;

    readavii   = true;

    find_cs    = false;
    find_em    = false;
    find_di    = true;
    find_fi    = true;
    find_co    = true;
    find_ct    = true;
    find_mco   = true;
    find_mar   = true;
    find_mti   = true;
    find_mal   = true;
    findX      = 10;
    findY      = 10;
    findWidth  = 515;
    findHeight = 365;

    mainP1     = 200;
    mainP2     = 270;
    mainP3     = 170;

    windowSize.setWidth ( windowSize_width=640 );
    windowSize.setHeight ( windowSize_height=480 );
    windowPos.setX ( 5 );
    windowPos.setY ( 5 );

    comm_bg    = new QColor ( 255,225,0 );
    comm_stext = new QColor ( 0,0,0 );
    comm_vtext = new QColor ( 10,10,255 );
    comm_fr    = new QColor ( 0,0,0 );

    linkf      = true;


#ifndef _WIN32
    mounteject = false;
#endif

#ifdef _WIN32
    lang       = "eng";
#endif

#ifdef Q_WS_MAC
    lang       = "eng";
#endif
}

CdCatConfig::~CdCatConfig(void) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if (DEBUG_INFO_ENABLED != NULL)
		delete DEBUG_INFO_ENABLED;
}

void CdCatConfig::setParameter ( char *par ) {
    startpar = true;
    startfn  = par;
}

int CdCatConfig::startProgram ( DataBase **dbp,QWidget *mw ) {
    QString loadablefile;
    if ( autoload ) loadablefile = autoloadfn;
    if ( startpar ) loadablefile = startfn;

    if ( autoload || startpar ) {
        if ( ( *dbp ) == NULL )
            ( *dbp ) = new DataBase();

        PWw *pww = new PWw ( mw );
        ( *dbp )->pww = pww;
        progress ( pww );
        if ( ( *dbp )->openDB ( ( char * ) ( ( const char * ) loadablefile ) ) ) {
            ( *dbp ) = NULL;
            QMessageBox::warning ( mw,tr ( "Error during autoload..." ),
                                   startpar ? tr ( "I can't open the autoload catalog according the first command line parameter.\nCheck the file!\n" )
                                   : tr ( "I can't open the autoload catalog.\nCheck the file, or change the autoload option in the config dialog!\n" ) );
        }
        progress ( pww );
        pww->end();
        if ( ( *dbp ) != NULL )
            ( *dbp )->pww = NULL;
        delete pww;
    }
    return 0;
}



int secv ( const char *s,int i ) {
    int v1,v2,v3;

    sscanf ( s,"%d,%d,%d",&v1,&v2,&v3 );

    if ( i == 0 ) return v1;
    if ( i == 1 ) return v2;
    if ( i == 2 ) return v3;
    return 0;
}


int CdCatConfig::readConfig ( void ) {
    int error = 0;
#ifdef _WIN32
    QFile f ( CONFIGFILE );
#else

    if ( getenv ( "HOME" ) == NULL )
        return 1;
    QString tmp;
    tmp+=getenv ( "HOME" );
    tmp+="/";
    tmp+=QString ( CONFIGFILE );
    QFile f ( tmp );
#endif

    QString msg;
    if ( f.open ( QIODevice::ReadOnly ) ) {   // file opened successfully
        QTextStream t ( &f );     // use a text stream

        while ( !t.atEnd() ) {
            QString line;
            line= t.readLine();       // line of text excluding '\n'
            msg+=line;
            msg+="\n";

            if ( !line.startsWith ( "#" ) && !line.isEmpty() && line.contains ( "=" ) ) {
                int index = line.find ( '=' );
                QString var = ( line.left ( index ) ).stripWhiteSpace();
                QString val = ( line.mid ( index+1,line.length()-index ) ).stripWhiteSpace();

                //   QString info = "variable: "+var+", val: "+val;
                //   QMessageBox::information(0,"found",info);

                if ( var=="font_size" ) {
                    fsize = val.toInt();
                    continue;
                }

                else if ( var=="history_size" ) {
                    historysize=val.toInt();
                    continue;
                }

                else if ( var=="history" ) {
                    QString history=val;
                    if ( !history.isEmpty() ) {
                        int idx=0;
                        int idx2=0;
                        while ( idx2 > -1 ) {
                            idx2 =history.find ( ';',idx );
                            if ( ( int ) idx != ( int ) ( history.length()-1 ) ) {
                                if ( ! ( history.mid ( idx,idx2-idx ) ).isEmpty() )
                                    hlist.append ( history.mid ( idx,idx2-idx ) );
                            }
                            idx=idx2+1;
                        }
                        while ( ( int ) hlist.count() > ( int ) historysize )
                            hlist.remove ( hlist.begin() );
                    }
                    continue;
                } // history

                else if ( var=="autoload" ) {
                    if ( val=="true" )
                        autoload=true;
                    else
                        autoload=false;
                    continue;
                }

                else if ( var=="autosave" ) {
                    if ( val=="true" )
                        autosave=true;
                    else
                        autosave=false;
                    continue;
                }

                else if ( var=="ownfont" ) {
                    if ( val=="true" )
                        ownfont=true;
                    else
                        ownfont=false;
                    continue;
                }

                else if ( var=="autoload_file" ) {
                    autoloadfn=val;
                    continue;
                }

                else if ( var=="niceformat" ) {
                    if ( val=="true" )
                        nice=true;
                    else
                        nice=false;
                    continue;
                }

                else if ( var=="cdrompath" ) {
                    if ( !val.isEmpty() )
                        cdrompath=val;
                    continue;
                }

#ifndef _WIN32
                else if ( var=="mounteject" ) {
                    if ( val=="true" )
                        mounteject=true;
                    else
                        mounteject=false;
                    continue;
                }
#endif

#ifdef _WIN32
                else if ( var=="lang" ) {
                    lang = val;
                    continue;
                }
#endif
#ifdef Q_WS_MAC
                else if ( var=="lang" ) {
                    lang = val;
                    continue;
                }
#endif

                else if ( var=="windowSize_height" ) {
                    windowSize_height = val.toInt();
                    continue;
                }
                else if ( var=="windowSize_width" ) {
                    windowSize_width = val.toInt();
                    continue;
                }
                else if ( var=="windowPos_x" ) {
                    windowPos.setX ( val.toInt() );
                    continue;
                }
                else if ( var=="windowPos_y" ) {
                    windowPos.setY ( val.toInt() );
                    continue;
                }

                else if ( var=="dirview_size" ) {
                    mainP1 =val.toInt();
                    continue;
                }
                else if ( var=="listview_size" ) {
                    mainP2 =val.toInt();
                    continue;
                }
                else if ( var=="commentview_size" ) {
                    mainP3 =val.toInt();
                    continue;
                }

                // Read the options of find dialog
                else if ( var=="find_checkbox_casesens" ) {
                    if ( val=="true" )
                        find_cs=true;
                    else
                        find_cs=false;
                    continue;
                }
                else if ( var=="find_checkbox_easymatch" ) {
                    if ( val=="true" )
                        find_em=true;
                    else
                        find_em=false;
                    continue;
                }
                else if ( var=="find_checkbox_directory" ) {
                    if ( val=="true" )
                        find_di=true;
                    else
                        find_di=false;
                    continue;
                }
                else if ( var=="find_checkbox_file" ) {
                    if ( val=="true" )
                        find_fi=true;
                    else
                        find_fi=false;
                    continue;
                }
                else if ( var=="find_checkbox_comment" ) {
                    if ( val=="true" )
                        find_co=true;
                    else
                        find_co=false;
                    continue;
                }
                else if ( var=="find_checkbox_content" ) {
                    if ( val=="true" )
                        find_ct=true;
                    else
                        find_ct=false;
                    continue;
                }
                else if ( var=="find_checkbox_mp3artist" ) {
                    if ( val=="true" )
                        find_mar=true;
                    else
                        find_mar=false;
                    continue;
                }
                else if ( var=="find_checkbox_mp3title" ) {
                    if ( val=="true" )
                        find_mti=true;
                    else
                        find_mti=false;
                    continue;
                }
                else if ( var=="find_checkbox_mp3album" ) {
                    if ( val=="true" )
                        find_mal=true;
                    else
                        find_mal=false;
                    continue;
                }
                else if ( var=="find_checkbox_mp3comment" ) {
                    if ( val=="true" )
                        find_mco=true;
                    else
                        find_mco=false;
                    continue;
                }

                else if ( var=="find_checkbox_size_min" ) {
                    if ( val=="true" )
                        find_size_min=true;
                    else
                        find_size_min=false;
                    continue;
                }

                else if ( var=="find_checkbox_size_max" ) {
                    if ( val=="true" )
                        find_size_max=true;
                    else
                        find_size_max=false;
                    continue;
                }

                else if ( var=="findPos_x" ) {
                    findX =val.toInt();
                    continue;
                }
                else if ( var=="findPos_y" ) {
                    findY =val.toInt();
                    continue;
                }
                else if ( var=="findSize_width" ) {
                    findWidth =val.toInt();
                    continue;
                }

                else if ( var=="findSize_height" ) {
                    findHeight =val.toInt();
                    continue;
                }

                else if ( var=="read_avi_techinfo" ) {
                    if ( val=="true" )
                        readavii=true;
                    else
                        readavii=false;
                    continue;
                }

                else if ( var=="read_mp3tag" ) {
                    if ( val=="true" )
                        readtag=true;
                    else
                        readtag=false;
                    continue;
                }

                else if ( var=="mp3tag_default_v1" ) {
                    if ( val=="true" )
                        v1_over_v2=true;
                    else
                        v1_over_v2=false;
                    continue;
                }

                else if ( var=="read_mp3techinfo" ) {
                    if ( val=="true" )
                        readinfo=true;
                    else
                        readinfo=false;
                    continue;
                }

                else if ( var=="read_content" ) {
                    if ( val=="true" )
                        readcontent=true;
                    else
                        readcontent=false;
                    continue;
                }
                else if ( var=="read_content_files" ) {
                    if ( !val.isEmpty() )
                        readcfiles=val;
                    continue;
                }

                else if ( var=="read_content_limit" ) {
                    readclimit =val.toInt();
                    continue;
                }

                else if ( var=="find_date_start" ) {
                    if ( val=="true" )
                        find_date_start=true;
                    else
                        find_date_start=false;
                    continue;
                }

                else if ( var=="find_date_end" ) {
                    if ( val=="true" )
                        find_date_end=true;
                    else
                        find_date_end=false;
                    continue;
                }

                else if ( var=="find_date_start_val" ) {
                    find_date_start_val = QDateTime().fromString(val);
                    continue;
                }

                else if ( var=="find_date_end_val" ) {
                    find_date_end_val =QDateTime().fromString(val);
                    continue;
                }

                else if ( var=="find_size_min_val" ) {
                    find_size_min_val =val.toInt();
                    continue;
                }

                else if ( var=="find_size_min" ) {
                    if ( val=="true" )
                        find_size_min=true;
                    else
                        find_size_min=false;
                    continue;
                }

                else if ( var=="find_size_max" ) {
                    if ( val=="true" )
                        find_size_max=true;
                    else
                        find_size_max=false;
                    continue;
                }

                else if ( var=="find_size_max_val" ) {
                    find_size_max_val =val.toInt();
                    continue;
                }

                else if ( var=="find_size_unit_min_val" ) {
                    find_size_unit_min_val =val.toInt();
                    continue;
                }

                else if ( var=="find_size_unit_max_val" ) {
                    find_size_unit_max_val =val.toInt();
                    continue;
                }

                if ( var=="comment_bg_color" ) {
                    int r=0,g=0,b=0;

                    r=secv ( val,0 );
                    g=secv ( val,1 );
                    b=secv ( val,2 );

                    comm_bg->setRgb ( r,g,b );
                    continue;
                }

                else if ( var=="comment_fr_color" ) {
                    int r=0,g=0,b=0;

                    r=secv ( val,0 );
                    g=secv ( val,1 );
                    b=secv ( val,2 );

                    comm_fr->setRgb ( r,g,b );
                    continue;
                }

                else if ( var=="comment_ts_color" ) {
                    int r=0,g=0,b=0;

                    r=secv ( val,0 );
                    g=secv ( val,1 );
                    b=secv ( val,2 );

                    comm_stext->setRgb ( r,g,b );
                    continue;
                }

                else if ( var=="comment_td_color" ) {
                    int r=0,g=0,b=0;

                    r=secv ( val,0 );
                    g=secv ( val,1 );
                    b=secv ( val,2 );

                    comm_vtext->setRgb ( r,g,b );
                    continue;
                }

                else if ( var=="catalog_link_is_first" ) {
                    if ( val=="true" )
                        linkf=true;
                    else
                        linkf=false;
                    continue;
                }

                else if ( var=="debug_info_enabled" ) {
                    if ( val=="true" )
                        debug_info_enabled=true;
                    else
                        debug_info_enabled=false;
                    continue;
                }

                else if ( var=="last_dir" ) {
                    if ( !val.isEmpty() )
                        lastDir=val;
                    continue;
                }

                else if ( var=="saveAlwaysCatalogInUtf8" ) {
                    if ( val=="true" )
                        saveAlwaysCatalogInUtf8=true;
                    else
                        saveAlwaysCatalogInUtf8=false;
                    continue;
                }

                else if ( var=="showProgressedFileInStatus" ) {
                    if ( val=="true" )
                        showProgressedFileInStatus=true;
                    else
                        showProgressedFileInStatus=false;
                    continue;
                }

                else if ( var=="lastMediaType" ) {
                        lastMediaType=val.toInt();
                    continue;
                }

                else if ( var=="lastSearchPattern" ) {
                        lastSearchPattern=val;
                    continue;
                }


                fprintf ( stderr,"Unknown key found: %s\n", ( const char * ) var );
                error = 1;

            } // no comment
        } // while lines
        f.close();

        windowSize.setHeight ( windowSize_height );
        windowSize.setWidth ( windowSize_width );
    } // file exist
    else return 1;
    return error;
}

int CdCatConfig::writeConfig ( void ) {

#ifdef _WIN32
    QFile f ( CONFIGFILE );
#else
    if ( getenv ( "HOME" ) == NULL ) {
        QMessageBox::warning ( 0,tr ( "Error while saving config file..." ),
                               tr ( "I can't get the $HOME environment variable.\nIt is necessary because I'd like to save the config file into your home directory." ) );

        return 1;
    }
    QString filepath=QString ( getenv ( "HOME" ) ) +QString ( "/" );
    filepath += CONFIGFILE;
    QFile f ( filepath );
#endif

    if ( f.open ( QIODevice::WriteOnly ) ) {
        QTextStream str ( &f ); // we will serialize the data into file f
        str.setEncoding ( QTextStream::Latin1 );

        QString fsize_str,historysize_str,historylength_str,history_str;

        str << "# " << endl;
        str << "#  CdCatalog config file, generated by cdcat "<< HOMEPAGE << endl;
        str << "#  Do not edit directly!" << endl;
        str << "#  Version: "<<VERSION<< endl;
        str << "# "<< endl;

        if ( ownfont )
            str << "ownfont=true"  << endl;
        else
            str << "ownfont=false" << endl;

        fsize_str.setNum ( fsize );
        str << "font_size="+  fsize_str << endl;

        historysize_str.setNum ( historysize );
        str << "history_size="+  historysize_str << endl;

        for ( QStringList::Iterator it = hlist.begin(); it != hlist.end(); ++it ) {
            if ( ! ( *it ).isEmpty() ) {
                history_str+=*it;
                history_str+=";";
                //QMessageBox::information(0,"new history element",*it);
            }
        }

        str << "history="+  history_str << endl;

        if ( autoload )
            str << "autoload=true"  << endl;
        else
            str << "autoload=false" << endl;


        if ( autoloadfn.isEmpty() )
            str << "autoload_file=empty" << endl;
        else
            str << "autoload_file="+autoloadfn << endl;

        if ( autosave )
            str << "autosave=true" << endl;
        else
            str << "autosave=false" << endl;

        if ( nice )
            str << "niceformat=true" << endl;
        else
            str << "niceformat=false" << endl;
        
        str << "cdrompath="+cdrompath << endl;

#ifndef _WIN32

        if ( mounteject )
            str << "mounteject=true" << endl;
        else
            str << "mounteject=false" << endl;
#endif

#ifdef _WIN32
        str <<"lang="+lang << endl;
#endif

#ifdef Q_WS_MAC
	str <<"lang="+lang << endl;
#endif

        str << "windowSize_height=" << windowSize.height() << endl;
        str << "windowSize_width=" << windowSize.width() << endl;

        str << "windowPos_x=" << windowPos.x() << endl;
        str << "windowPos_y=" << windowPos.y() << endl;

        str << "dirview_size=" << mainP1 << endl;
        str << "listview_size=" << mainP2 << endl;
        str << "commentview_size=" << mainP3 << endl;

        /* Write the state of find dialog */
        if ( find_cs )
            str << "find_checkbox_casesens=true" << endl;
        else
            str << "find_checkbox_casesens=false" << endl;

        if ( find_em )
            str << "find_checkbox_easymatch=true" << endl;
        else
            str << "find_checkbox_easymatch=false" << endl;

        if ( find_di )
            str << "find_checkbox_directory=true" << endl;
        else
            str << "find_checkbox_directory=false" << endl;

        if ( find_fi )
            str << "find_checkbox_file=true" << endl;
        else
            str << "find_checkbox_file=false" << endl;

        if ( find_co )
            str << "find_checkbox_comment=true" << endl;
        else
            str << "find_checkbox_comment=false" << endl;

        if ( find_ct )
            str << "find_checkbox_content=true" << endl;
        else
            str << "find_checkbox_content=false" << endl;

        if ( find_mar )
            str << "find_checkbox_mp3artist=true" << endl;
        else
            str << "find_checkbox_mp3artist=false" << endl;

        if ( find_mti )
            str << "find_checkbox_mp3title=true" << endl;
        else
            str << "find_checkbox_mp3title=false" << endl;

        if ( find_mal )
            str << "find_checkbox_mp3album=true" << endl;
        else
            str << "find_checkbox_mp3album=false" << endl;

        if ( find_mco )
            str << "find_checkbox_mp3comment=true" << endl;
        else
            str << "find_checkbox_mp3comment=false" << endl;

        if ( find_date_start )
            str << "find_checkbox_date_start=true" << endl;
        else
            str << "find_checkbox_date_start=false" << endl;

        if ( find_date_end )
            str << "find_checkbox_date_end=true" << endl;
        else
            str << "find_checkbox_date_end=false" << endl;

        str << "find_date_start_val=" << find_date_start_val.toString() << endl;
        str << "find_date_end_val=" << find_date_end_val.toString() << endl;

        if ( find_size_min )
            str << "find_size_min=true" << endl;
        else
            str << "find_size_min=false" << endl;

        if ( find_size_max )
            str << "find_size_max=true" << endl;
        else
            str << "find_size_max=false" << endl;

        str << "find_size_min_val=" << find_size_min_val << endl;
        str << "find_size_max_val=" << find_size_max_val << endl;
        str << "find_size_unit_min_val=" << find_size_unit_min_val << endl;
        str << "find_size_unit_max_val=" << find_size_unit_max_val << endl;


        str << "findPos_x=" << findX << endl;
        str << "findPos_y=" << findY << endl;
        str << "findSize_width=" << findWidth << endl;
        str << "findSize_height=" << findHeight << endl;

        if ( readavii )
            str << "read_avi_techinfo=true" <<endl;
        else
            str << "read_avi_techinfo=false" <<endl;

        if ( readtag )
            str << "read_mp3tag=true" <<endl;
        else
            str << "read_mp3tag=false" <<endl;

        if ( v1_over_v2 )
            str << "mp3tag_default_v1=true" <<endl;
        else
            str << "mp3tag_default_v1=false" <<endl;

        if ( readinfo )
            str << "read_mp3techinfo=true" <<endl;
        else
            str << "read_mp3techinfo=false" <<endl;

        if ( readcontent )
            str << "read_content=true" <<endl;
        else
            str << "read_content=false" <<endl;
        str << "read_content_files=" << readcfiles << endl;
        str << "read_content_limit=" << readclimit << endl;

        str << "comment_bg_color=" << comm_bg->red() <<","<< comm_bg->green() <<","<< comm_bg->blue() <<endl;
        str << "comment_fr_color=" << comm_fr->red() <<","<< comm_fr->green() <<","<< comm_fr->blue() <<endl;
        str << "comment_ts_color=" << comm_stext->red() <<","<< comm_stext->green() <<","<< comm_stext->blue() <<endl;
        str << "comment_td_color=" << comm_vtext->red() <<","<< comm_vtext->green() <<","<< comm_vtext->blue() <<endl;

        if ( linkf )
            str << "catalog_link_is_first=true" <<endl;
        else
            str << "catalog_link_is_first=false" <<endl;

        if ( debug_info_enabled )
            str << "debug_info_enabled=true" <<endl;
        else
            str << "debug_info_enabled=false" <<endl;

        if ( saveAlwaysCatalogInUtf8 )
            str << "saveAlwaysCatalogInUtf8=true" <<endl;
        else
            str << "saveAlwaysCatalogInUtf8=false" <<endl;

        if ( showProgressedFileInStatus )
            str << "showProgressedFileInStatus=true" <<endl;
        else
            str << "showProgressedFileInStatus=false" <<endl;



        str << "last_dir="+lastDir << endl;
        str << "lastMediaType="+QString().setNum(lastMediaType) << endl;
        str << "lastSearchPattern="+lastSearchPattern << endl;

        f.close();
        return 0;
    } else {
#ifdef _WIN32
        QMessageBox::warning ( 0,tr ( "Error while saving config file..." ),tr ( "I can't create or rewrite the ./cdcat.cfg file " ) );
#else
        QMessageBox::warning ( 0,tr ( "Error while saving config file..." ),tr ( "I can't create or rewrite the $(HOME)/.cdcatconfig file " ) );
#endif
        return 1;
    }
}

/************************************************************************************/
ConfigDialog::ConfigDialog ( CdCatMainWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    if ( !name )
        setName ( "ConfigDialog" );
    setIcon ( *get_t_config_icon() );

    p = parent;

    setSizeGripEnabled ( TRUE );
    ConfigDialogBaseLayout = new Q3GridLayout ( this, 1, 1, 18, 6, "ConfigDialogBaseLayout" );

    layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );
    cbAutoload = new QCheckBox ( this, "cbAutoload" );
    layout1->addWidget ( cbAutoload );
    ConfigDialogBaseLayout->addLayout ( layout1, 0, 0 );

    layout2 = new Q3HBoxLayout ( 0, 0, 6, "layout2" );
    filename = new QLineEdit ( this, "filename" );
    layout2->addWidget ( filename );
    searchButton = new QPushButton ( this, "searchButton" );
    searchButton->setText ( "..." );
    searchButton->setFlat ( FALSE );
    layout2->addWidget ( searchButton );
    ConfigDialogBaseLayout->addLayout ( layout2, 1, 0 );

    line1 = new Q3Frame ( this, "line1" );
    line1->setFrameShape ( Q3Frame::HLine );
    line1->setFrameShadow ( Q3Frame::Sunken );
    line1->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line1, 2, 0 );

    layout3 = new Q3HBoxLayout ( 0, 0, 5, "layout3" );
    cbOwnFont = new QCheckBox ( "Use own font size",this, "ownfontcb" );
    layout3->addWidget ( cbOwnFont,0 );
    ConfigDialogBaseLayout->addLayout ( layout3, 3, 0 );

    layout4 = new Q3HBoxLayout ( 0, 0, 5, "layout4" );
    spinFontSize = new QSpinBox ( this,"spinFontSize" );
    spinFontSize->setMaximumWidth ( 80 );
    layout4->addWidget ( spinFontSize );
    lab = new QLabel ( this, "lab" );
    layout4->addWidget ( lab );
    ConfigDialogBaseLayout->addLayout ( layout4, 4, 0 );


    line2 = new Q3Frame ( this, "line2" );
    line2->setFrameShape ( Q3Frame::HLine );
    line2->setFrameShadow ( Q3Frame::Sunken );
    line2->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line2, 5, 0 );

    cbAutosave = new QCheckBox ( this, "cbAutos" );
    ConfigDialogBaseLayout->addWidget ( cbAutosave, 6, 0 );

    line8 = new Q3Frame ( this, "line8" );
    line8->setFrameShape ( Q3Frame::HLine );
    line8->setFrameShadow ( Q3Frame::Sunken );
    line8->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line8, 7, 0 );

    cbNice = new QCheckBox ( this, "cbNice" );
    ConfigDialogBaseLayout->addWidget ( cbNice, 8, 0 );

    line3 = new Q3Frame ( this, "line3" );
    line3->setFrameShape ( Q3Frame::HLine );
    line3->setFrameShadow ( Q3Frame::Sunken );
    line3->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line3, 9, 0 );


    layout5 = new Q3HBoxLayout ( 0, 0, 6, "layout5" );
    cdrompath_lineedit = new QLineEdit ( this, "cdrompath_lineedit" );
    layout5->addWidget ( cdrompath_lineedit );
    cdrom_lab = new QLabel ( this, "cdrom_lab" );
    layout5->addWidget ( cdrom_lab );
    searchButton2 = new QPushButton ( this, "cdrombutton" );
    searchButton2->setText ( "..." );
    searchButton2->setFlat ( FALSE );
    layout5->addWidget ( searchButton2 );
    ConfigDialogBaseLayout->addLayout ( layout5, 10, 0 );

    line4 = new Q3Frame ( this, "line4" );
    line4->setFrameShape ( Q3Frame::HLine );
    line4->setFrameShadow ( Q3Frame::Sunken );
    line4->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line4, 11, 0 );



#ifndef _WIN32
    cbMoEj = new QCheckBox ( this, "cbMoEj" );
    ConfigDialogBaseLayout->addWidget ( cbMoEj, 12, 0 );
#endif

#ifdef _WIN32
    layout9   = new Q3HBoxLayout ( 0,0,5,"layout9" );
    cbLang    = new QComboBox ( this,"languageselector" );
    langLabel = new QLabel ( this,"langlabel" );
    layout9->insertSpacing ( 0,5 );
    layout9->insertWidget ( 1,cbLang );
    layout9->insertWidget ( 2,langLabel );
    layout9->insertSpacing ( 3,5 );
    ConfigDialogBaseLayout->addLayout ( layout9, 13, 0 );

    /*scanning existing languages:*/
    cbLang->insertItem ( "eng",0 );
    QDir d ( "./lang" );

    if ( d.exists() ) {
        d.setFilter ( QDir::Files );
        d.setNameFilter ( "cdcat_??.qm" );
        QFileInfoList list( d.entryInfoList());
		//QFileInfoListIterator it ( list. );
        //QFileInfo *fi;
		foreach(const QFileInfo& fi , list )
        //while ( ( fi = it.current() ) != 0 ) 
		{
            cbLang->insertItem ( ( fi.fileName() ).mid ( 6,2 ) );
            //++it;
        }
        /*end scanning*/
    } else {
        cbLang->setEnabled ( false );
        cbLang->insertItem ( "eng" );
    }
#endif

#ifdef Q_WS_MAC
// 	cerr << "config: mac! " << endl;
    layout9   = new Q3HBoxLayout ( 0,0,5,"layout9" );
    cbLang    = new QComboBox ( this,"languageselector" );
    langLabel = new QLabel ( this,"langlabel" );
    layout9->insertSpacing ( 0,5 );
    layout9->insertWidget ( 1,cbLang );
    layout9->insertWidget ( 2,langLabel );
    layout9->insertSpacing ( 3,5 );
    ConfigDialogBaseLayout->addLayout ( layout9, 13, 0 );

    /*scanning existing languages:*/
    cbLang->insertItem ( "eng",0 );
    QDir d ( "./lang" );

    if ( d.exists() ) {
        d.setFilter ( QDir::Files );
        d.setNameFilter ( "cdcat_??.qm" );
        QFileInfoList list( d.entryInfoList());
		//QFileInfoListIterator it ( list. );
        //QFileInfo *fi;
		foreach(const QFileInfo& fi , list )
        //while ( ( fi = it.current() ) != 0 ) 
		{
            cbLang->insertItem ( ( fi.fileName() ).mid ( 6,2 ) );
            //++it;
        }
        /*end scanning*/
    } else {
        cbLang->setEnabled ( false );
        cbLang->insertItem ( "eng" );
    }
#endif

    line5 = new Q3Frame ( this, "line5" );
    line5->setFrameShape ( Q3Frame::HLine );
    line5->setFrameShadow ( Q3Frame::Sunken );
    line5->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line5, 14, 0 );

    layout6 = new Q3HBoxLayout ( 0, 0, 6, "layout6" );
    spinHistorySize = new QSpinBox ( this,"spinHistorySize" );
    spinHistorySize->setMaximumWidth ( 80 );
    layout6->addWidget ( spinHistorySize );
    labHistorySize = new QLabel ( this, "labHistorySize" );
    layout6->addWidget ( labHistorySize );
    ConfigDialogBaseLayout->addLayout ( layout6, 15, 0 );

    line6 = new Q3Frame ( this, "line6" );
    line6->setFrameShape ( Q3Frame::HLine );
    line6->setFrameShadow ( Q3Frame::Sunken );
    line6->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line6, 16, 0 );


    riButton = new QPushButton ( this,"ributton" );
    ConfigDialogBaseLayout->addWidget ( riButton, 17, 0 );

    line7 = new Q3Frame ( this, "line7" );
    line7->setFrameShape ( Q3Frame::HLine );
    line7->setFrameShadow ( Q3Frame::Sunken );
    line7->setFrameShape ( Q3Frame::HLine );
    ConfigDialogBaseLayout->addWidget ( line7, 18, 0 );

    layout7 = new Q3HBoxLayout ( 0, 0, 6, "layout7" );
    QSpacerItem* spacer = new QSpacerItem ( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem ( spacer );

    cbEnableDebugInfo = new QCheckBox ( this, "cbEnableDebugInfo" );
    ConfigDialogBaseLayout->addWidget ( cbEnableDebugInfo, 19, 0 );

    cbSaveCatalogAlwaysInUtf8 = new QCheckBox ( this, "cbSaveCatalogAlwaysInUtf8" );
    ConfigDialogBaseLayout->addWidget ( cbSaveCatalogAlwaysInUtf8, 20, 0 );

    cbShowProgressedFileInStatus = new QCheckBox ( this, "cbShowProgressedFileInStatus" );
    ConfigDialogBaseLayout->addWidget ( cbShowProgressedFileInStatus, 21, 0 );

    okButton = new QPushButton ( this, "okButton" );
    okButton->setMinimumSize ( QSize ( 100, 0 ) );
    okButton->setAutoDefault ( TRUE );
    okButton->setDefault ( TRUE );
    layout7->addWidget ( okButton );

    cancelButton = new QPushButton ( this, "cancelButton" );
    cancelButton->setMinimumSize ( QSize ( 100, 0 ) );
    cancelButton->setAutoDefault ( TRUE );
    layout7->addWidget ( cancelButton );
    QSpacerItem* spacer_2 = new QSpacerItem ( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem ( spacer_2 );

    ConfigDialogBaseLayout->addLayout ( layout7, 22, 0 );



    connect ( searchButton2, SIGNAL ( clicked() ), this, SLOT ( cdrombutton() ) );

    languageChange();
    resize ( QSize ( 506, 309 ).expandedTo ( minimumSizeHint() ) );


    // signals and slots connections
    connect ( cancelButton, SIGNAL ( clicked() ), this, SLOT ( close() ) );
    connect ( okButton, SIGNAL ( clicked() ), this, SLOT ( okExit() ) );
    connect ( searchButton, SIGNAL ( clicked() ), this, SLOT ( ffdbutton() ) );
    connect ( cbOwnFont, SIGNAL ( clicked() ), this, SLOT ( ownFontToggled() ) );
    connect ( riButton, SIGNAL ( clicked() ), this, SLOT ( runri() ) );

    if ( p->cconfig->ownfont ) {
        spinFontSize->setEnabled ( true );
        lab->setEnabled ( true );
    } else {
        spinFontSize->setEnabled ( false );
        lab->setEnabled ( false );
    }

    if ( p->cconfig->ownfont )
        cbOwnFont->setChecked ( true );
    else
        cbOwnFont->setChecked ( false );

    cbNice->setChecked ( p->cconfig->nice );
    cbAutoload->setChecked ( p->cconfig->autoload );
    cbAutosave->setChecked ( p->cconfig->autosave );
    if ( !p->cconfig->autoloadfn.isEmpty() )
        filename->setText ( p->cconfig->autoloadfn );
    spinFontSize->setValue ( p->cconfig->fsize );
    spinHistorySize->setValue ( p->cconfig->historysize );

    cdrompath_lineedit->setText ( p->cconfig->cdrompath );
#ifndef _WIN32
    cbMoEj ->setChecked ( p->cconfig->mounteject );
#endif

#ifdef _WIN32
    cbLang->setCurrentItem ( 0 );
    for ( int i=0;i<cbLang->count();i++ )
        if ( p->cconfig->lang == cbLang->text ( i ) )
            cbLang->setCurrentItem ( i );
#endif

#ifdef Q_WS_MAC
    cbLang->setCurrentItem ( 0 );
    for ( int i=0;i<cbLang->count();i++ )
        if ( p->cconfig->lang == cbLang->text ( i ) )
            cbLang->setCurrentItem ( i );
#endif

    cbEnableDebugInfo->setChecked(p->cconfig->debug_info_enabled);
    cbSaveCatalogAlwaysInUtf8->setChecked(p->cconfig->saveAlwaysCatalogInUtf8);
    cbShowProgressedFileInStatus->setChecked(p->cconfig->showProgressedFileInStatus);
}

/*
 *  Destroys the object and frees any allocated resources
 */
ConfigDialog::~ConfigDialog() {
    // no need to delete child widgets, Qt does it all for us
}


void ConfigDialog::languageChange() {
    setCaption ( tr ( "Configure  CdCat..." ) );
    cbAutoload->setText ( tr ( "Autoload DataBase on startup" ) );
    cbAutosave->setText ( tr ( "Automatically save the database after every scan (for safety sake)" ) );
    cbNice->setText ( tr ( "Save the XML-db.file nicer format(needs more space)" ) );
    cancelButton->setText ( tr ( "Cancel" ) );
    okButton->setText ( tr ( "Ok" ) );
    riButton->setText ( tr ( "Select additional items to read" ) );
    cbOwnFont->setText ( tr ( "Use own font size" ) );
    lab->setText ( tr ( "Application font size.(must restart cdcat!) " ) );
    labHistorySize->setText ( tr ( "Number of history entries" ) );
    cdrom_lab->setText ( tr ( "Path to cdrom device" ) );

#ifndef _WIN32
    cbMoEj->setText ( tr ( "Scanning: mount cdrom at start / eject when finish" ) );
#endif

#ifdef _WIN32
    langLabel->setText ( tr ( "The language of CdCat interface" ) );
#endif

#ifdef Q_WS_MAC
    langLabel->setText ( tr ( "The language of CdCat interface" ) );
#endif

    cbEnableDebugInfo->setText( tr("Display debug info on console") );
    cbSaveCatalogAlwaysInUtf8->setText( tr("Save catalogs always as UTF8") );
    cbShowProgressedFileInStatus->setText( tr("Show progressed file at scanning in status label") );
}


void ConfigDialog::ffdbutton() {
    QString s = QFileDialog::getOpenFileName(0, tr ( "Choose a file to load automatically!" ), p->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ));
    if ( !s.isEmpty() )
        filename->setText ( s );
}


void ConfigDialog::cdrombutton() {
    QString s = QFileDialog::getExistingDirectory(0, tr ( "Choose path to cdrom!" ), p->cconfig->lastDir );

    if ( !s.isEmpty() )
        cdrompath_lineedit->setText ( s );
}


void ConfigDialog::okExit() {
    p->cconfig->nice        = cbNice->isChecked();
    p->cconfig->autoload    = cbAutoload->isChecked();
    p->cconfig->autosave    = cbAutosave->isChecked();
    p->cconfig->autoloadfn  = filename->text();
    p->cconfig->ownfont     = cbOwnFont->isChecked();
    p->cconfig->fsize       = spinFontSize->value();
    p->cconfig->historysize = spinHistorySize->value();
    p->cconfig->cdrompath   = cdrompath_lineedit->text();
#ifndef _WIN32
    p->cconfig->mounteject  = cbMoEj->isChecked();
#endif

#ifdef _WIN32
    QTranslator *translator = 0;
    QString langfile ( "./lang/cdcat_" );
    langfile += p->cconfig->lang;
    langfile += ".qm";

    translator = new QTranslator ( 0 );
    translator->load ( langfile,"." );
    p->app->removeTranslator ( translator );

    //read the value
    p->cconfig->lang        = cbLang->currentText();

    translator = 0;
    langfile = "./lang/cdcat_";
    langfile += p->cconfig->lang;
    langfile += ".qm";

    translator = new QTranslator ( 0 );
    translator->load ( langfile,"." );
    p->app->installTranslator ( translator );
#endif

#ifdef Q_WS_MAC
    QTranslator *translator = 0;
    QString langfile ( "./lang/cdcat_" );
    langfile += p->cconfig->lang;
    langfile += ".qm";

    translator = new QTranslator ( 0 );
    translator->load ( langfile,"." );
    p->app->removeTranslator ( translator );

    //read the value
    p->cconfig->lang        = cbLang->currentText();

    translator = 0;
    langfile = "./lang/cdcat_";
    langfile += p->cconfig->lang;
    langfile += ".qm";

    translator = new QTranslator ( 0 );
    translator->load ( langfile,"." );
    p->app->installTranslator ( translator );
#endif


    p->cconfig->debug_info_enabled = cbEnableDebugInfo->isChecked();
    DEBUG_INFO_ENABLED = init_debug_info();
    *DEBUG_INFO_ENABLED = cbEnableDebugInfo->isChecked();
    p->cconfig->saveAlwaysCatalogInUtf8 = cbSaveCatalogAlwaysInUtf8->isChecked();
    p->cconfig->showProgressedFileInStatus = cbShowProgressedFileInStatus->isChecked();
    p->cconfig->writeConfig();

    QFont *font=new QFont();
    font->setPointSize ( p->cconfig->fsize );
    if ( p->cconfig->ownfont )
        p->app->setFont ( *font,TRUE );
    else
        p->app->setFont ( *p->cconfig->defaultfont,TRUE );

    close();
}

void ConfigDialog::ownFontToggled() {
    if ( cbOwnFont->isChecked() ) {
        lab->setEnabled ( true );
        spinFontSize->setEnabled ( true );
    } else {
        lab->setEnabled ( false );
        spinFontSize->setEnabled ( false );
    }
}

void ConfigDialog::runri() {
    SelReadable *sr = new SelReadable ( p->cconfig,this,"runri",true );
    sr->exec();
    delete sr;
}

