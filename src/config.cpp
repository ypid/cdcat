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
#include <qfile.h>
#include <qfont.h>
#include <qcolor.h>
#include <QFrame>
#include <qtextstream.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <QHBoxLayout>
#include <QGridLayout>


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

#if defined(_WIN32) || defined(_OS2)
#define CONFIGFILE "cdcat.cfg"
#else
#define CONFIGFILE ".cdcatconf"
#endif

using namespace std;

CdCatConfig::CdCatConfig ( void ) {
	
	/* The default values of config.
	 * If you delete the config file (or prats of file)
	 * the values will be set to these:
	*/
	
	startpar   = false;
	startfn    = "";
	
	ownfont    = false;
	fsize      = 8;
	historysize = 10;
	showTrayIcon = true;
	commentDockIsFloating = false;
	showStatusBar = true;
	showToolBar = true;
	showCommentDock = true;
	autoload   = false;
	autoloadfn = "";
	nice       = true;
	// set dummy
#if defined(_WIN32) || defined(_OS2)
	cdrompath = "D:/";
	lastDir = getenv ( "USER_PROFILE" );
#else
	cdrompath = "/dev/sr0";
	lastDir = getenv ( "HOME" );
#endif

	hlist.clear();
	autosave   = false;
	debug_info_enabled = false;
	readtag    = true;
	v1_over_v2 = false;
	readinfo   = true;
	readcontent = false;
	readcfiles = "*.nfo;*.diz";
	ContentViewerFont = "";
	ThumbExtsList.clear();
	ThumbExtsList.append("png");
	ThumbExtsList.append("gif");
	ThumbExtsList.append("jpg");
	ThumbExtsList.append("jpeg");
	ThumbExtsList.append("bmp");
	readclimit = 32 * 1024;
	lastMediaType = CD;
	saveAlwaysCatalogInUtf8 = true;
	showProgressedFileInStatus = true;
	doScanArchive = true;
	showProgressedArchiveFileInStatus = true;
	displayCurrentScannedFileInTray = false;
	doExcludeFiles = false;
	useWildcardInsteadRegexForExclude = false;
	storeThumb = true;
	keep_search_result = false;
	readavii   = true;
	
	find_cs    = false;
	find_em    = true;
	find_di    = true;
	find_fi    = true;
	find_co    = true;
	find_category = false;
	find_ct    = true;
	find_mco   = true;
	find_mar   = true;
	find_mti   = true;
	find_mal   = true;
	find_size_min = false;
	find_size_max = false;
	find_date_start = false;
	find_date_end = false;
	find_unsharp_search = false;
	find_in_archive = false;
	show_archive_file_perms = true;
	show_archive_file_user = true;
	show_archive_file_group = true;
	show_archive_file_size = true;
	show_archive_file_date = true;
	show_archive_file_comment = true;
	find_size_unit_min_val = 2;
	find_size_unit_max_val = 2;
	find_size_min_val = 1;
	find_size_max_val = 10;
	findX      = 10;
	findY      = 10;
	findWidth  = 600;
	findHeight = 500;
	addX      = 50;
	addY      = 10;
	addWidth  = 600;
	addHeight = 500;
	thumbHeight = 150;
	thumbWidth = 150;
	
	mainP1     = 200;
	mainP2     = 270;
	mainP3     = 170;
	
	windowSize.setWidth ( windowSize_width = 640 );
	windowSize.setHeight ( windowSize_height = 480 );
	windowPos.setX ( 5 );
	windowPos.setY ( 5 );
	
	commentDockSize_dockarea = Qt::RightDockWidgetArea;
	
	comm_bg    = new QColor ( 255, 225, 0 );
	comm_stext = new QColor ( 0, 0, 0 );
	comm_vtext = new QColor ( 10, 10, 255 );
	comm_fr    = new QColor ( 0, 0, 0 );
	
	linkf      = true;
	
#ifndef _WIN32
	mounteject = false;
#endif
	
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
	lang       = "eng";
#endif
}



CdCatConfig::~CdCatConfig ( void ) {
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( DEBUG_INFO_ENABLED != NULL )
		delete DEBUG_INFO_ENABLED;
}

void CdCatConfig::setParameter ( char *par ) {
	startpar = true;
	startfn  = par;
}

int CdCatConfig::startProgram ( DataBase **dbp, QWidget *mw ) {
	QString loadablefile;
	if ( autoload )
		loadablefile = autoloadfn;
	if ( startpar )
		loadablefile = startfn;
	
	if ( autoload || startpar ) {
		if ( ( *dbp ) == NULL )
			( *dbp ) = new DataBase();
		
		PWw *pww = new PWw ( mw );
		( *dbp )->pww = pww;
		progress ( pww );
		if ( ( *dbp )->openDB ( loadablefile.toLocal8Bit().data() ) ) {
			( *dbp ) = NULL;
			QMessageBox::warning ( mw, tr ( "Error during autoload..." ),
			                       startpar ? tr ( "I can't open the autoload catalog according the first command line parameter.\nCheck the file!\n" )
			                       : tr ( "I can't open the autoload catalog.\nCheck the file, or change the autoload option in the config dialog!\n" ) );
		}
		progress ( pww );
		pww->end();
		if ( ( *dbp ) != NULL ) {
			( *dbp )->pww = NULL;
			( *dbp )->storeMp3tags     = readtag;
			( *dbp )->v1_over_v2       = v1_over_v2;
			( *dbp )->storeMp3techinfo = readinfo;
			( *dbp )->storeAvitechinfo = readavii;
			( *dbp )->storeFileInfo = usefileinfo;
			( *dbp )->storeContent = readcontent;
			( *dbp )->doScanArchive = doScanArchive;
			( *dbp )->doScanArchiveTar = doScanArchive; // FIXME: make dynamiclly
			( *dbp )->doScanArchiveLib7zip = doScanArchive;  // FIXME: make dynamiclly
			( *dbp )->storeExifData = storeExifData;
			( *dbp )->storeThumb = storeThumb;
			( *dbp )->ThumbExtsList = ThumbExtsList;
			( *dbp )->thumbWidth = thumbWidth;
			( *dbp )->thumbHeight = thumbHeight;
			( *dbp )->storeLimit      = readclimit;
			( *dbp )->storedFiles = readcfiles;
			( *dbp )->doExcludeFiles = doExcludeFiles;
			( *dbp )->ExcludeFileList = ExcludeFileList;
			( *dbp )->useWildcardInsteadRegexForExclude = useWildcardInsteadRegexForExclude;
			( *dbp )->displayCurrentScannedFileInTray = displayCurrentScannedFileInTray;
		}
		delete pww;
	}
	return 0;
}

int secv ( const char *s, int i ) {
	int v1, v2, v3;
	
	sscanf ( s, "%d,%d,%d", &v1, &v2, &v3 );
	
	if ( i == 0 )
		return v1;
	if ( i == 1 )
		return v2;
	if ( i == 2 )
		return v3;
	return 0;
}

int CdCatConfig::readConfig ( void ) {
	int error = 0;
#if defined(_WIN32) || defined(_OS2)
	QFile f ( CONFIGFILE );
#else
	
	if ( getenv ( "HOME" ) == NULL )
		return 1;
	QString tmp;
	tmp += getenv ( "HOME" );
	tmp += "/";
	tmp += QString ( CONFIGFILE );
	QFile f ( tmp );
#endif
	
	QString msg;
	if ( f.open ( QIODevice::ReadOnly ) ) {   // file opened successfully
		QTextStream t ( &f );     // use a text stream
		
		while ( !t.atEnd() ) {
			QString line;
			line = t.readLine();      // line of text excluding '\n'
			msg += line;
			msg += "\n";
			
			if ( !line.startsWith ( "#" ) && !line.isEmpty() && line.contains ( "=" ) ) {
				int index = line.indexOf ( '=' );
				QString var = ( line.left ( index ) ).simplified();
				QString val = ( line.mid ( index + 1, line.length() - index ) ).simplified();
				
				//   QString info = "variable: "+var+", val: "+val;
				//   QMessageBox::information(0,"found",info);
				
				if ( var == "font_size" ) {
					fsize = val.toInt();
					continue;
				}
				if ( var == "history_size" ) {
					historysize = val.toInt();
					continue;
				}
				if ( var == "history" ) {
					QString history = val;
					if ( !history.isEmpty() ) {
						int idx = 0;
						int idx2 = 0;
						while ( idx2 > -1 ) {
							idx2 = history.indexOf ( ';', idx );
							if ( ( int ) idx != ( int ) ( history.length() - 1 ) ) {
								if ( ! ( history.mid ( idx, idx2 - idx ) ).isEmpty() )
									hlist.append ( history.mid ( idx, idx2 - idx ) );
							}
							idx = idx2 + 1;
						}
						while ( ( int ) hlist.count() > ( int ) historysize )
							hlist.removeFirst ();
					}
					continue;
				} // history
				
				
				if ( var == "showTrayIcon" ) {
					if ( val == "true" )
						showTrayIcon = true;
					else
						showTrayIcon = false;
					continue;
				}
				if ( var == "showStatusBar" ) {
					if ( val == "true" )
						showStatusBar = true;
					else
						showStatusBar = false;
					continue;
				}
				if ( var == "showToolBar" ) {
					if ( val == "true" )
						showToolBar = true;
					else
						showToolBar = false;
					continue;
				}
				if ( var == "showCommentDock" ) {
					if ( val == "true" )
						showCommentDock = true;
					else
						showCommentDock = false;
					continue;
				}
				
				if ( var == "autoload" ) {
					if ( val == "true" )
						autoload = true;
					else
						autoload = false;
					continue;
				}
				
				if ( var == "autosave" ) {
					if ( val == "true" )
						autosave = true;
					else
						autosave = false;
					continue;
				}
				if ( var == "ownfont" ) {
					if ( val == "true" )
						ownfont = true;
					else
						ownfont = false;
					continue;
				}
				if ( var == "autoload_file" ) {
					autoloadfn = val;
					continue;
				}
				if ( var == "niceformat" ) {
					if ( val == "true" )
						nice = true;
					else
						nice = false;
					continue;
				}
				if ( var == "cdrompath" ) {
					if ( !val.isEmpty() )
						cdrompath = val;
					continue;
				}
#ifndef _WIN32
				if ( var == "mounteject" ) {
					if ( val == "true" )
						mounteject = true;
					else
						mounteject = false;
					continue;
				}
#endif
				
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
				if ( var == "lang" ) {
					lang = val;
					continue;
				}
#endif
				if ( var == "windowSize_height" ) {
					windowSize_height = val.toInt();
					continue;
				}
				if ( var == "windowSize_width" ) {
					windowSize_width = val.toInt();
					continue;
				}
				if ( var == "windowPos_x" ) {
					windowPos.setX ( val.toInt() );
					continue;
				}
				if ( var == "windowPos_y" ) {
					windowPos.setY ( val.toInt() );
					continue;
				}
				if ( var == "commentDockSize_width" ) {
					commentDockSize_width = val.toInt();
					continue;
				}
				if ( var == "commentDockSize_height" ) {
					commentDockSize_height = val.toInt();
					continue;
				}
				if ( var == "commentDockPos_x" ) {
					commentDockPos_x = val.toInt();
					continue;
				}
				if ( var == "commentDockPos_y" ) {
					commentDockPos_y = val.toInt();
					continue;
				}
				if ( var == "commentDockSize_dockarea" ) {
					commentDockSize_dockarea = val.toInt();
					continue;
				}
				if ( var == "commentDockIsFloating" ) {
					if(val == "true")
						commentDockIsFloating = true;
					else
						commentDockIsFloating = false;
					continue;
				}
				if ( var == "contentWindowSize_height" ) {
					contentWindowSize_height = val.toInt();
					continue;
				}
				if ( var == "contentWindowSize_width" ) {
					contentWindowSize_width = val.toInt();
					continue;
				}
				if ( var == "contentWindowPos_x" ) {
					contentWindowPos_x = val.toInt();
					continue;
				}
				if ( var == "contentWindowPos_y" ) {
					contentWindowPos_y = val.toInt();
					continue;
				}
				if ( var == "commentWindowSize_height" ) {
					commentWindowSize_height = val.toInt();
					continue;
				}
				if ( var == "commentWindowSize_width" ) {
					commentWindowSize_width = val.toInt();
					continue;
				}
				if ( var == "commentWindowPos_x" ) {
					commentWindowPos_x = val.toInt();
					continue;
				}
				if ( var == "commentWindowPos_y" ) {
					commentWindowPos_y = val.toInt();
					continue;
				}
				if ( var == "dirview_size" ) {
					mainP1 = val.toInt();
					continue;
				}
				if ( var == "listview_size" ) {
					mainP2 = val.toInt();
					continue;
				}
				if ( var == "commentview_size" ) {
					mainP3 = val.toInt();
					continue;
				}
				
				// Read the options of find dialog
				if ( var == "find_checkbox_casesens" ) {
					if ( val == "true" )
						find_cs = true;
					else
						find_cs = false;
					continue;
				}
				if ( var == "find_checkbox_easymatch" ) {
					if ( val == "true" )
						find_em = true;
					else
						find_em = false;
					continue;
				}
				if ( var == "find_checkbox_directory" ) {
					if ( val == "true" )
						find_di = true;
					else
						find_di = false;
					continue;
				}
				if ( var == "find_checkbox_file" ) {
						if ( val == "true" )
							find_fi = true;
						else
							find_fi = false;
						continue;
				}
				if ( var == "find_checkbox_comment" ) {
					if ( val == "true" )
						find_co = true;
					else
						find_co = false;
					continue;
				}
				if ( var == "find_checkbox_comment" ) {
					if ( val == "true" )
						find_co = true;
					else
						find_co = false;
					continue;
				}
				if ( var == "find_category" ) {
					if ( val == "true" )
						find_category = true;
					else
						find_category = false;
					continue;
				}
				if ( var == "lastFindCategory" ) {
					lastFindCategory = val;
					continue;
				}
				
				if ( var == "find_checkbox_content" ) {
					if ( val == "true" )
						find_ct = true;
					else
						find_ct = false;
					continue;
				}
				if ( var == "find_checkbox_mp3artist" ) {
					if ( val == "true" )
						find_mar = true;
					else
						find_mar = false;
					continue;
				}
				if ( var == "find_checkbox_mp3title" ) {
					if ( val == "true" )
						find_mti = true;
					else
						find_mti = false;
					continue;
				}
				if ( var == "find_checkbox_mp3album" ) {
					if ( val == "true" )
						find_mal = true;
					else
						find_mal = false;
					continue;
				}
				if ( var == "find_checkbox_mp3comment" ) {
					if ( val == "true" )
					find_mco = true;
					else
					find_mco = false;
					continue;
				}
				if ( var == "find_checkbox_size_min" ) {
					if ( val == "true" )
						find_size_min = true;
					else
						find_size_min = false;
					continue;
				}
				if ( var == "find_checkbox_size_max" ) {
					if ( val == "true" )
						find_size_max = true;
					else
						find_size_max = false;
					continue;
				}
				if ( var == "findPos_x" ) {
					findX = val.toInt();
					continue;
				}
				if ( var == "findPos_y" ) {
					findY = val.toInt();
					continue;
				}
				if ( var == "findSize_width" ) {
					findWidth = val.toInt();
					continue;
				}
				if ( var == "findSize_height" ) {
					findHeight = val.toInt();
					continue;
				}
				if ( var == "addPos_x" ) {
					addX = val.toInt();
					continue;
				}
				if ( var == "addPos_y" ) {
					addY = val.toInt();
					continue;
				}
				if ( var == "addSize_width" ) {
					addWidth = val.toInt();
					continue;
				}
				if ( var == "addSize_height" ) {
					addHeight = val.toInt();
					continue;
				}
				if ( var == "read_avi_techinfo" ) {
					if ( val == "true" )
						readavii = true;
					else
						readavii = false;
						continue;
					}
				if ( var == "read_mp3tag" ) {
					if ( val == "true" )
						readtag = true;
					else
						readtag = false;
					continue;
				}
				if ( var == "mp3tag_default_v1" ) {
					if ( val == "true" )
						v1_over_v2 = true;
					else
						v1_over_v2 = false;
					continue;
				}
				if ( var == "read_mp3techinfo" ) {
					if ( val == "true" )
						readinfo = true;
					else
						readinfo = false;
					continue;
				}
				if ( var == "read_content" ) {
					if ( val == "true" )
						readcontent = true;
					else
						readcontent = false;
					continue;
				}
				if ( var == "use_fileinfo" ) {
					if ( val == "true" )
						usefileinfo = true;
					else
						usefileinfo = false;
					continue;
				}
				if ( var == "read_content_files" ) {
					if ( !val.isEmpty() )
						readcfiles = val;
						continue;
				}
				if ( var == "use_ext_content_viewer" ) {
					if ( val == "true" )
						useExternalContentViewer = true;
					else
						useExternalContentViewer = false;
					continue;
				}
				if ( var == "ext_content_viewer_path" ) {
					if ( !val.isEmpty() )
						ExternalContentViewerPath = val;
					continue;
				}
				if ( var == "ContentViewerFont" ) {
					if ( !val.isEmpty() )
						ContentViewerFont = val;
					continue;
				}
				if ( var == "read_content_limit" ) {
					readclimit = val.toInt();
					continue;
				}
				if ( var == "find_checkbox_date_start" ) {
					if ( val == "true" )
						find_date_start = true;
					else
						find_date_start = false;
					continue;
				}
				if ( var == "find_checkbox_date_end" ) {
					if ( val == "true" )
						find_date_end = true;
					else
						find_date_end = false;
					continue;
				}
				if ( var == "find_date_start_val" ) {
					find_date_start_val = QDateTime().fromString ( val );
					continue;
				}
				if ( var == "find_date_end_val" ) {
					find_date_end_val = QDateTime().fromString ( val );
					continue;
				}
				if ( var == "find_size_min_val" ) {
					find_size_min_val = val.toInt();
					continue;
				}
				if ( var == "find_size_min" ) {
					if ( val == "true" )
						find_size_min = true;
					else
						find_size_min = false;
					continue;
				}
				if ( var == "find_size_max" ) {
					if ( val == "true" )
						find_size_max = true;
					else
						find_size_max = false;
					continue;
				}
				if ( var == "find_size_max_val" ) {
					find_size_max_val = val.toInt();
					continue;
				}																																		if ( var == "find_size_unit_min_val" ) {
					find_size_unit_min_val = val.toInt();
					continue;
				}
				if ( var == "find_size_unit_max_val" ) {
					find_size_unit_max_val = val.toInt();
					continue;
				}
				if ( var == "comment_bg_color" ) {
					int r = 0, g = 0, b = 0;
					r = secv ( val.toLocal8Bit().data(), 0 );
					g = secv ( val.toLocal8Bit().data(), 1 );
					b = secv ( val.toLocal8Bit().data(), 2 );
					comm_bg->setRgb ( r, g, b );
					continue;
				}
				if ( var == "comment_fr_color" ) {
					int r = 0, g = 0, b = 0;
					r = secv ( val.toLocal8Bit().data(), 0 );
					g = secv ( val.toLocal8Bit().data(), 1 );
					b = secv ( val.toLocal8Bit().data(), 2 );
					comm_fr->setRgb ( r, g, b );
					continue;
				}
				if ( var == "comment_ts_color" ) {
					int r = 0, g = 0, b = 0;
					r = secv ( val.toLocal8Bit().data(), 0 );
					g = secv ( val.toLocal8Bit().data(), 1 );
					b = secv ( val.toLocal8Bit().data(), 2 );
					comm_stext->setRgb ( r, g, b );
					continue;
				}
				if ( var == "comment_td_color" ) {
					int r = 0, g = 0, b = 0;
					r = secv ( val.toLocal8Bit().data(), 0 );
					g = secv ( val.toLocal8Bit().data(), 1 );
					b = secv ( val.toLocal8Bit().data(), 2 );
					comm_vtext->setRgb ( r, g, b );
					continue;
				}
				if ( var == "catalog_link_is_first" ) {
					if ( val == "true" )
						linkf = true;
					else
						linkf = false;
					continue;
				}
				if ( var == "debug_info_enabled" ) {
					if ( val == "true" )
						debug_info_enabled = true;
					else
						debug_info_enabled = false;
					continue;
				}
				if ( var == "last_dir" ) {
					if ( !val.isEmpty() )
						lastDir = val;
					continue;
				}
				if ( var == "saveAlwaysCatalogInUtf8" ) {
					if ( val == "true" )
						saveAlwaysCatalogInUtf8 = true;
					else
						saveAlwaysCatalogInUtf8 = false;
					continue;
				}
				if ( var == "showProgressedFileInStatus" ) {
					if ( val == "true" )
						showProgressedFileInStatus = true;
					else
						showProgressedFileInStatus = false;
					continue;
				}
				if ( var == "lastMediaType" ) {
					lastMediaType = val.toInt();
					continue;
				}
				if ( var == "lastSearchPattern" ) {
					lastSearchPattern = val;
					continue;
				}
				if ( var == "find_unsharp_search" ) {
					if ( val == "true" )
						find_unsharp_search = true;
					else
						find_unsharp_search = false;
					continue;
				}
				if ( var == "do_scan_archive" ) {
					if ( val == "true" )
						doScanArchive = true;
					else
						doScanArchive = false;
					continue;
				}
				if ( var == "showProgressedArchiveFileInStatus" ) {
					if ( val == "true" )
						showProgressedArchiveFileInStatus = true;
					else
						showProgressedArchiveFileInStatus = false;
					continue;
				}
				if ( var == "displayCurrentScannedFileInTray" ) {
					if ( val == "true" )
						displayCurrentScannedFileInTray = true;
					else
						displayCurrentScannedFileInTray = false;
					continue;
				}
				
				if ( var == "find_in_archive" ) {
					if ( val == "true" )
						find_in_archive = true;
					else
						find_in_archive = false;
					continue;
				}
				if ( var == "show_archive_file_perms" ) {
					if ( val == "true" )
						show_archive_file_perms = true;
					else
						show_archive_file_perms = false;
					continue;
				}
				if ( var == "show_archive_file_user" ) {
					if ( val == "true" )
						show_archive_file_user = true;
					else
						show_archive_file_user = false;
					continue;
				}
				if ( var == "show_archive_file_group" ) {
					if ( val == "true" )
						show_archive_file_group = true;
					else
						show_archive_file_group = false;
					continue;
				}
				if ( var == "show_archive_file_size" ) {
					if ( val == "true" )
						show_archive_file_size = true;
					else
						show_archive_file_size = false;
					continue;
				}
				if ( var == "show_archive_file_date" ) {
					if ( val == "true" )
						show_archive_file_date = true;
					else
						show_archive_file_date = false;
					continue;
				}
				if ( var == "show_archive_file_comment" ) {
					if ( val == "true" )
						show_archive_file_comment = true;
					else
						show_archive_file_comment = false;
					continue;
				}
				if ( var == "store_thumb" ) {
					if ( val == "true" )
						storeThumb = true;
					else
						storeThumb = false;
					continue;
				}
				if ( var == "thumb_width" ) {
					thumbWidth = val.toInt();
					continue;
				}
				if ( var == "thumb_height" ) {
					thumbHeight = val.toInt();
					continue;
				}
				if ( var == "thumb_exts" ) {
					ThumbExtsList= val.split(';');
					continue;
				}
				if ( var == "store_exif_data" ) {
					if ( val == "true" )
						storeExifData = true;
					else
						storeExifData = false;
					continue;
				}
				if ( var == "do_exclude_files" ) {
					if ( val == "true" )
						doExcludeFiles = true;
					else
						doExcludeFiles = false;
					continue;
				}
				if ( var == "exclude_file_list" ) {
					ExcludeFileList= val.split(';');
					continue;
				}
				if ( var == "useWildcardInsteadRegexForExclude" ) {
					if ( val == "true" )
						useWildcardInsteadRegexForExclude = true;
					else
						useWildcardInsteadRegexForExclude = false;
					continue;
				}
				if ( var == "keep_search_result" ) {
					if ( val == "true" )
						keep_search_result = true;
					else
						keep_search_result = false;
					continue;
				}
				
				
				std::cerr << "Unknown key found: " << qPrintable(var) << std::endl;
				error = 1;
			} // no comment
		} // while lines
		f.close();
		windowSize.setHeight ( windowSize_height );
		windowSize.setWidth ( windowSize_width );
	} // file exist
	else
		return 1;
	return error;
}

int CdCatConfig::writeConfig ( void ) {

#if defined(_WIN32) || defined(_OS2)
	QFile f ( CONFIGFILE );
#else
	if ( getenv ( "HOME" ) == NULL ) {
		QMessageBox::warning ( 0, tr ( "Error while saving config file..." ),
		                       tr ( "I can't get the $HOME environment variable.\nIt is necessary because I'd like to save the config file into your home directory." ) );

		return 1;
	}
	QString filepath = QString ( getenv ( "HOME" ) ) + QString ( "/" );
	filepath += CONFIGFILE;
	QFile f ( filepath );
#endif
	
	if ( f.open ( QIODevice::WriteOnly ) ) {
		QTextStream str ( &f ); // we will serialize the data into file f
		str.setCodec(QTextCodec::codecForName("ISO-8859-1"));

		QString fsize_str, historysize_str, historylength_str, history_str;

		str << "# " << endl;
		str << "#  CdCatalog config file, generated by cdcat " << HOMEPAGE << endl;
		str << "#  Do not edit directly!" << endl;
		str << "#  Version: " << VERSION << endl;
		str << "# " << endl;
		
		if ( ownfont )
			str << "ownfont=true"  << endl;
		else
			str << "ownfont=false" << endl;
		
		fsize_str.setNum ( fsize );
		str << "font_size=" +  fsize_str << endl;
		
		historysize_str.setNum ( historysize );
		str << "history_size=" +  historysize_str << endl;
		
		for ( QStringList::Iterator it = hlist.begin(); it != hlist.end(); ++it ) {
			if ( ! ( *it ).isEmpty() ) {
				history_str += *it;
				history_str += ";";
				//QMessageBox::information(0,"new history element",*it);
			}
		}
		
		str << "history=" +  history_str << endl;
		
		if ( showTrayIcon )
			str << "showTrayIcon=true"  << endl;
		else
			str << "showTrayIcon=false" << endl;
		if ( showStatusBar )
			str << "showStatusBar=true"  << endl;
		else
			str << "showStatusBar=false" << endl;
		if ( showToolBar )
			str << "showToolBar=true"  << endl;
		else
			str << "showToolBar=false" << endl;
		if ( showCommentDock )
			str << "showCommentDock=true"  << endl;
		else
			str << "showCommentDock=false" << endl;
		if ( autoload )
			str << "autoload=true"  << endl;
		else
			str << "autoload=false" << endl;
		
		if ( autoloadfn.isEmpty() )
			str << "autoload_file=empty" << endl;
		else
			str << "autoload_file=" + autoloadfn << endl;
		
		if ( autosave )
			str << "autosave=true" << endl;
		else
			str << "autosave=false" << endl;
		
		if ( nice )
			str << "niceformat=true" << endl;
		else
			str << "niceformat=false" << endl;

		str << "cdrompath=" + cdrompath << endl;
		
#ifndef _WIN32
		
		if ( mounteject )
			str << "mounteject=true" << endl;
		else
			str << "mounteject=false" << endl;
#endif
		
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
		str << "lang=" + lang << endl;
#endif

		str << "windowSize_height=" << windowSize.height() << endl;
		str << "windowSize_width=" << windowSize.width() << endl;
		
		str << "windowPos_x=" << windowPos.x() << endl;
		str << "windowPos_y=" << windowPos.y() << endl;
		
		str << "commentDockSize_width=" << commentDockSize_width << endl;
		str << "commentDockSize_height=" << commentDockSize_height << endl;
		str << "commentDockPos_x=" << commentDockPos_x << endl;
		str << "commentDockPos_y=" << commentDockPos_y << endl;
		str << "commentDockSize_dockarea=" << commentDockSize_dockarea << endl;
		if(commentDockIsFloating)
			str << "commentDockIsFloating=true" << endl;
		else
			str << "commentDockIsFloating=false" << endl;
		
		str << "contentWindowSize_height=" << contentWindowSize_height << endl;
		str << "contentWindowSize_width=" << contentWindowSize_width << endl;
		
		str << "contentWindowPos_x=" << contentWindowPos_x << endl;
		str << "contentWindowPos_y=" << contentWindowPos_y << endl;
		
		str << "commentWindowSize_height=" << commentWindowSize_height << endl;
		str << "commentWindowSize_width=" << commentWindowSize_width << endl;
		
		str << "commentWindowPos_x=" << commentWindowPos_x << endl;
		str << "commentWindowPos_y=" << commentWindowPos_y << endl;
		
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
		
		if ( find_category )
			str << "find_category=true" << endl;
		else
			str << "find_category=false" << endl;
		
		str << "lastFindCategory=" << lastFindCategory << endl;
		
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
		
		str << "addPos_x=" << addX << endl;
		str << "addPos_y=" << addY << endl;
		str << "addSize_width=" << addWidth << endl;
		str << "addSize_height=" << addHeight << endl;
		
		if ( readavii )
			str << "read_avi_techinfo=true" << endl;
		else
			str << "read_avi_techinfo=false" << endl;
		
		if ( readtag )
			str << "read_mp3tag=true" << endl;
		else
			str << "read_mp3tag=false" << endl;
		
		if ( v1_over_v2 )
			str << "mp3tag_default_v1=true" << endl;
		else
			str << "mp3tag_default_v1=false" << endl;
		
		if ( readinfo )
			str << "read_mp3techinfo=true" << endl;
		else
			str << "read_mp3techinfo=false" << endl;
		
		if ( readcontent )
			str << "read_content=true" << endl;
		else
			str << "read_content=false" << endl;
		
		if ( useExternalContentViewer )
			str << "use_ext_content_viewer=true" << endl;
		else
			str << "use_ext_content_viewer=false" << endl;
		
		str << "ext_content_viewer_path=" << ExternalContentViewerPath << endl;
		str << "ContentViewerFont=" << ContentViewerFont << endl;
		
		if ( usefileinfo )
			str << "use_fileinfo=true" << endl;
		else
			str << "use_fileinfo=false" << endl;
		
		str << "read_content_files=" << readcfiles << endl;
		str << "read_content_limit=" << readclimit << endl;
		
		str << "comment_bg_color=" << comm_bg->red() << "," << comm_bg->green() << "," << comm_bg->blue() << endl;
		str << "comment_fr_color=" << comm_fr->red() << "," << comm_fr->green() << "," << comm_fr->blue() << endl;
		str << "comment_ts_color=" << comm_stext->red() << "," << comm_stext->green() << "," << comm_stext->blue() << endl;
		str << "comment_td_color=" << comm_vtext->red() << "," << comm_vtext->green() << "," << comm_vtext->blue() << endl;
		
		if ( linkf )
			str << "catalog_link_is_first=true" << endl;
		else
			str << "catalog_link_is_first=false" << endl;
		
		if ( debug_info_enabled )
			str << "debug_info_enabled=true" << endl;
		else
			str << "debug_info_enabled=false" << endl;
		
		if ( saveAlwaysCatalogInUtf8 )
			str << "saveAlwaysCatalogInUtf8=true" << endl;
		else
			str << "saveAlwaysCatalogInUtf8=false" << endl;
		
		if ( showProgressedFileInStatus )
			str << "showProgressedFileInStatus=true" << endl;
		else
			str << "showProgressedFileInStatus=false" << endl;
		
		if ( find_unsharp_search )
			str << "find_unsharp_search=true" << endl;
		else
			str << "find_unsharp_search=false" << endl;
		
		str << "last_dir=" + lastDir << endl;
		str << "lastMediaType=" + QString().setNum ( lastMediaType ) << endl;
		str << "lastSearchPattern=" + lastSearchPattern << endl;
		
		if ( doScanArchive )
			str << "do_scan_archive=true" << endl;
		else
			str << "do_scan_archive=false" << endl;
		
		if(showProgressedArchiveFileInStatus)
			str << "showProgressedArchiveFileInStatus=true" << endl;
		else
			str << "showProgressedArchiveFileInStatus=false" << endl;
		
		if(displayCurrentScannedFileInTray)
			str << "displayCurrentScannedFileInTray=true" << endl;
		else
			str << "displayCurrentScannedFileInTray=false" << endl;
		
		if ( find_in_archive )
			str << "find_in_archive=true" << endl;
		else
			str << "find_in_archive=false" << endl;
		
		if ( show_archive_file_perms )
			str << "show_archive_file_perms=true" << endl;
		else
			str << "show_archive_file_perms=false" << endl;
		if ( show_archive_file_user )
			str << "show_archive_file_user=true" << endl;
		else
			str << "show_archive_file_user=false" << endl;
		if ( show_archive_file_group )
			str << "show_archive_file_group=true" << endl;
		else
			str << "show_archive_file_group=false" << endl;
		if ( show_archive_file_size )
			str << "show_archive_file_size=true" << endl;
		else
			str << "show_archive_file_size=false" << endl;
		if ( show_archive_file_date )
			str << "show_archive_file_date=true" << endl;
		else
			str << "show_archive_file_date=false" << endl;
		if ( show_archive_file_comment )
			str << "show_archive_file_comment=true" << endl;
		else
			str << "show_archive_file_comment=false" << endl;
		if ( storeThumb )
			str << "store_thumb=true" << endl;
		else
			str << "store_thumb=false" << endl;
		
		str << "thumb_width=" << thumbWidth << endl;
		str << "thumb_height=" << thumbHeight << endl;
		
		str << "thumb_exts=" << ThumbExtsList.join(";") << endl;
		
		if ( storeExifData )
			str << "store_exif_data=true" << endl;
		else
			str << "store_exif_data=false" << endl;
		
		if (doExcludeFiles )
			str << "do_exclude_files=true" << endl;
		else
			str << "do_exclude_files=false" << endl;
		
		str << "exclude_file_list=" << ExcludeFileList.join(";") << endl;
		
		if(useWildcardInsteadRegexForExclude)
			str << "useWildcardInsteadRegexForExclude=true" << endl;
		else
			str << "useWildcardInsteadRegexForExclude=false" << endl;
		if(keep_search_result)
			str << "keep_search_result=true" << endl;
		else
			str << "keep_search_result=false" << endl;
		
		f.close();
		return 0;
	}
	else {
#if defined(_WIN32) || defined(_OS2)
		QMessageBox::warning ( 0, tr ( "Error while saving config file..." ), tr ( "I can't create or rewrite the ./cdcat.cfg file " ) );
#else
		QMessageBox::warning ( 0, tr ( "Error while saving config file..." ), tr ( "I can't create or rewrite the $(HOME)/.cdcatconfig file " ) );
#endif
		return 1;
	}
}

/************************************************************************************/
ConfigDialog::ConfigDialog ( CdCatMainWidget* parent, const char* name, bool modal, Qt::WFlags fl )
	: QDialog ( parent, fl ) {
	if ( !name )
		setObjectName ( "ConfigDialog" );
	setWindowIcon ( *get_t_config_icon() );
	setModal(modal);
	p = parent;
	
	setSizeGripEnabled ( TRUE );
	ConfigDialogBaseLayout = new QGridLayout ( this );
	
	layout1 = new QHBoxLayout ( this );
	cbAutoload = new QCheckBox ( this );
	filename = new QLineEdit ( this );
	searchButton = new QPushButton ( this );
	searchButton->setText ( "..." );
	searchButton->setFlat ( FALSE );
	
	layout1->addWidget ( cbAutoload );
	
	layout1->addWidget ( filename );
	layout1->addWidget ( searchButton );
	ConfigDialogBaseLayout->addLayout ( layout1, 0, 0 );
	
	line1 = new QFrame ( this );
	line1->setFrameShape ( QFrame::HLine );
	line1->setFrameShadow ( QFrame::Sunken );
	line1->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line1, 2, 0 );
	
	cbOwnFont = new QCheckBox ( tr("Use own font size"), this );
	
	
	layout4 = new QHBoxLayout ( this );
	spinFontSize = new QSpinBox ( this );
	spinFontSize->setMaximumWidth ( 80 );
	QSpacerItem* fontspacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	
	lab = new QLabel ( this );
	layout4->addWidget ( cbOwnFont, 0 );
	layout1->addItem(fontspacer);
	layout4->addWidget ( spinFontSize );
	layout4->addWidget ( lab );
	ConfigDialogBaseLayout->addLayout ( layout4, 4, 0 );
	
	line2 = new QFrame ( this );
	line2->setFrameShape ( QFrame::HLine );
	line2->setFrameShadow ( QFrame::Sunken );
	line2->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line2, 5, 0 );
	
	cbAutosave = new QCheckBox ( this );
	ConfigDialogBaseLayout->addWidget ( cbAutosave, 6, 0 );
	
	line8 = new QFrame ( this );
	line8->setFrameShape ( QFrame::HLine );
	line8->setFrameShadow ( QFrame::Sunken );
	line8->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line8, 7, 0 );
	
	layoutSave = new QHBoxLayout ( this );
	cbNice = new QCheckBox ( this );
	cbSaveCatalogAlwaysInUtf8 = new QCheckBox ( this );
	layoutSave->addWidget ( cbNice );
	layoutSave->addWidget ( cbSaveCatalogAlwaysInUtf8 );
	ConfigDialogBaseLayout->addLayout ( layoutSave, 8, 0 );
	
	line3 = new QFrame ( this );
	line3->setFrameShape ( QFrame::HLine );
	line3->setFrameShadow ( QFrame::Sunken );
	line3->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line3, 9, 0 );
	
	layout5 = new QHBoxLayout ( this );
	cdrompath_lineedit = new QLineEdit ( this );
	layout5->addWidget ( cdrompath_lineedit );
	cdrom_lab = new QLabel ( this );
	layout5->addWidget ( cdrom_lab );
	searchButton2 = new QPushButton ( this );
	searchButton2->setText ( "..." );
	searchButton2->setFlat ( FALSE );
	layout5->addWidget ( searchButton2 );
	ConfigDialogBaseLayout->addLayout ( layout5, 10, 0 );
	
#ifndef _WIN32
	cbMoEj = new QCheckBox ( this );
	ConfigDialogBaseLayout->addWidget ( cbMoEj, 12, 0 );
#endif
	
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
	layout9   = new QHBoxLayout ( this );
	cbLang    = new QComboBox ( this );
	langLabel = new QLabel ( this );
	layout9->insertSpacing ( 0, 5 );
	layout9->insertWidget ( 1, cbLang );
	layout9->insertWidget ( 2, langLabel );
	layout9->insertSpacing ( 3, 5 );
	ConfigDialogBaseLayout->addLayout ( layout9, 13, 0 );
	
	/*scanning existing languages:*/
	QDir d ( "./lang" );
	cbLang->insertItem ( 0, QString("eng") );
	if ( d.exists() ) {
		d.setFilter ( QDir::Files );
		QStringList filters;
		filters << "cdcat_??.qm";
		d.setNameFilters(filters);
		QFileInfoList list ( d.entryInfoList() );
		//QFileInfoListIterator it ( list. );
		//QFileInfo *fi;
		foreach ( const QFileInfo & fi , list )
		//while ( ( fi = it.current() ) != 0 )
		{
			cbLang->insertItem ( 0, ( fi.fileName() ).mid ( 6, 2 ) );
			//++it;
		}
		/*end scanning*/
	}
	else {
		cbLang->setEnabled ( false );
	}
#endif
	
	line5 = new QFrame ( this );
	line5->setFrameShape ( QFrame::HLine );
	line5->setFrameShadow ( QFrame::Sunken );
	line5->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line5, 14, 0 );
	
	layout6 = new QHBoxLayout ( this );
	spinHistorySize = new QSpinBox ( this );
	spinHistorySize->setMaximumWidth ( 80 );
	layout6->addWidget ( spinHistorySize );
	labHistorySize = new QLabel ( this );
	layout6->addWidget ( labHistorySize );
	ConfigDialogBaseLayout->addLayout ( layout6, 15, 0 );
	
	layoutDock = new QHBoxLayout ( this );
	cbShowTrayIcon = new QCheckBox(tr( "show systray icon" ), this );
	cbShowCurrentScannedFileInTrayIcon = new QCheckBox(tr( "display current scanned file in tray" ), this );
	//QSpacerItem* systrayspacer1 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	//layoutDock->addItem(systrayspacer1);
	layoutDock->addWidget(cbShowTrayIcon);
	layoutDock->addWidget(cbShowCurrentScannedFileInTrayIcon);
	
	
	ConfigDialogBaseLayout->addLayout ( layoutDock, 16, 0 );
	
	
	line6 = new QFrame ( this );
	line6->setFrameShape ( QFrame::HLine );
	line6->setFrameShadow ( QFrame::Sunken );
	line6->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line6, 17, 0 );
	
	riButton = new QPushButton ( this );
	ConfigDialogBaseLayout->addWidget ( riButton, 18, 0 );
	
	line7 = new QFrame ( this );
	line7->setFrameShape ( QFrame::HLine );
	line7->setFrameShadow ( QFrame::Sunken );
	line7->setFrameShape ( QFrame::HLine );
	ConfigDialogBaseLayout->addWidget ( line7, 19, 0 );
	
	layoutStatus = new QHBoxLayout (this );
	cbEnableDebugInfo = new QCheckBox ( this );
	cbShowProgressedFileInStatus = new QCheckBox ( this );
	layoutStatus->addWidget ( cbEnableDebugInfo );
	layoutStatus->addWidget ( cbShowProgressedFileInStatus );
	ConfigDialogBaseLayout->addLayout ( layoutStatus, 20, 0 );
	
	layout7 = new QHBoxLayout ( this );
	QSpacerItem* spacer = new QSpacerItem ( 110, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout7->addItem ( spacer );
	
	okButton = new QPushButton ( this );
	okButton->setMinimumSize ( QSize ( 100, 0 ) );
	okButton->setAutoDefault ( TRUE );
	okButton->setDefault ( TRUE );
	layout7->addWidget ( okButton );
	
	cancelButton = new QPushButton ( this );
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
	connect ( cbShowTrayIcon, SIGNAL ( clicked() ), this, SLOT ( showDockToggled()) );
	
	if ( p->cconfig->ownfont ) {
		spinFontSize->setEnabled ( true );
		lab->setEnabled ( true );
	}
	else {
		spinFontSize->setEnabled ( false );
		lab->setEnabled ( false );
	}
	
	if ( p->cconfig->ownfont )
		cbOwnFont->setChecked ( true );
	else
		cbOwnFont->setChecked ( false );
	
	cbNice->setChecked ( p->cconfig->nice );
	cbShowTrayIcon->setChecked( p->cconfig->showTrayIcon );
	cbShowCurrentScannedFileInTrayIcon->setChecked( p->cconfig->displayCurrentScannedFileInTray );
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
	
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
	cbLang->setCurrentIndex ( 0 );
	int index = cbLang->findText( p->cconfig->lang );
	if(index != -1) {
		cbLang->setCurrentIndex ( index );
	}
#endif
	
	cbEnableDebugInfo->setChecked ( p->cconfig->debug_info_enabled );
	cbSaveCatalogAlwaysInUtf8->setChecked ( p->cconfig->saveAlwaysCatalogInUtf8 );
	cbShowProgressedFileInStatus->setChecked ( p->cconfig->showProgressedFileInStatus );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ConfigDialog::~ConfigDialog() {
	// no need to delete child widgets, Qt does it all for us
}

void ConfigDialog::languageChange() {
	setWindowTitle ( tr ( "Configure  CdCat..." ) );
	cbShowTrayIcon->setText( tr( "Show systray icon" ));
	cbShowCurrentScannedFileInTrayIcon->setText( tr( "display current scanned file in tray" ) );
	cbShowCurrentScannedFileInTrayIcon->setToolTip( tr( "display current scanned file in tray (mediainfo / archive scan)" ) );
	cbAutoload->setText ( tr ( "Autoload DataBase on startup" ) );
	cbAutosave->setText ( tr ( "Automatically save the database after every scan (for safety sake)" ) );
	cbNice->setText ( tr ( "Save the XML-db.file nicer format(needs more space)" ) );
	cancelButton->setText ( tr ( "Cancel" ) );
	okButton->setText ( tr ( "Ok" ) );
	riButton->setText ( tr ( "Select additional items to read" ) );
	cbOwnFont->setText ( tr ( "Use own font size" ) );
	lab->setText ( tr ( "font size" ) );
	cbOwnFont->setToolTip ( tr ( "Application font size.(must restart cdcat!) " ) );
	labHistorySize->setText ( tr ( "Number of history entries" ) );
	cdrom_lab->setText ( tr ( "Path to cdrom device / mount dir" ) );
	cdrompath_lineedit->setToolTip( tr("This is the path to the cdrom/dvd device or the path where its mounted"));
	
#ifndef _WIN32
	cbMoEj->setText ( tr ( "Scanning: mount cdrom at start / eject when finish" ) );
#endif
	
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
	langLabel->setText ( tr ( "The language of CdCat interface" ) );
#endif
	
	cbEnableDebugInfo->setText ( tr ( "Display debug info on console" ) );
	cbSaveCatalogAlwaysInUtf8->setText ( tr ( "Save catalogs always as UTF8" ) );
	cbShowProgressedFileInStatus->setText ( tr ( "Show progressed file at scanning in status label" ) );
}

void ConfigDialog::ffdbutton() {
	QString s = QFileDialog::getOpenFileName ( 0, tr ( "Choose a file to load automatically!" ), p->cconfig->lastDir, tr ( "CdCat databases (*.hcf )" ) );
	if ( !s.isEmpty() )
		filename->setText ( s );
}

void ConfigDialog::cdrombutton() {
	QString s = QFileDialog::getExistingDirectory ( 0, tr ( "Choose path to cdrom!" ), p->cconfig->lastDir );
	
	if ( !s.isEmpty() )
		cdrompath_lineedit->setText ( s );
}

void ConfigDialog::okExit() {
	p->cconfig->nice        = cbNice->isChecked();
	p->cconfig->showTrayIcon = cbShowTrayIcon->isChecked();
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
	
#if defined(_WIN32) || defined(Q_WS_MAC) || defined(_OS2)
	QString langfile ( "./lang/cdcat_" );
	langfile += p->cconfig->lang;
	langfile += ".qm";
	
	
	p->app->removeTranslator ( translator );
	delete(translator);
	
	translator = new QTranslator ( 0 );
	translator->load ( langfile, "." );
	
	
	//read the value
	p->cconfig->lang        = cbLang->currentText();
	
	translator = 0;
	langfile = "./lang/cdcat_";
	langfile += p->cconfig->lang;
	langfile += ".qm";
	
	translator = new QTranslator ( 0 );
	translator->load ( langfile, "." );
	p->app->installTranslator ( translator );
#endif
	
	p->cconfig->debug_info_enabled = cbEnableDebugInfo->isChecked();
	DEBUG_INFO_ENABLED = init_debug_info();
	*DEBUG_INFO_ENABLED = cbEnableDebugInfo->isChecked();
	p->cconfig->saveAlwaysCatalogInUtf8 = cbSaveCatalogAlwaysInUtf8->isChecked();
	p->cconfig->showProgressedFileInStatus = cbShowProgressedFileInStatus->isChecked();
	p->cconfig->displayCurrentScannedFileInTray = cbShowCurrentScannedFileInTrayIcon->isChecked();
	
	p->cconfig->writeConfig();
	
	QFont *font = new QFont();
	font->setPointSize ( p->cconfig->fsize );
	if ( p->cconfig->ownfont )
		p->app->setFont ( *font );
	else
		p->app->setFont ( *p->cconfig->defaultfont );
	
	close();
}

void ConfigDialog::ownFontToggled() {
	if ( cbOwnFont->isChecked() ) {
		lab->setEnabled ( true );
		spinFontSize->setEnabled ( true );
	}
	else {
		lab->setEnabled ( false );
		spinFontSize->setEnabled ( false );
	}
}

void ConfigDialog::showDockToggled() {
	if (cbShowTrayIcon->isChecked()) {
		cbShowCurrentScannedFileInTrayIcon->setEnabled(true);
	}
	else {
		cbShowCurrentScannedFileInTrayIcon->setEnabled(false);
	}
}


void ConfigDialog::runri() {
	SelReadable *sr = new SelReadable ( p->cconfig, this, "runri", true );
	sr->exec();
	delete sr;
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
