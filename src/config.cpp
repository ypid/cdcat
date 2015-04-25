/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "config.h"

#include "dbase.h"
#include "icons.h"
#include "cdcat.h"
#include "adddialog.h"
#include "mainwidget.h"
#include "selreadable.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <iostream>

#include <QDebug>
#include <QSettings>
#include <QString>
#include <QMessageBox>
#include <QWidget>
#include <QVariant>
#include <QCheckBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QToolTip>
#include <QLabel>
#include <QSpinBox>
#include <QFont>
#include <QColor>
#include <QFrame>
#include <QTextStream>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QTranslator>
#include <QTextCodec>

#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    static QTranslator *translator;
#endif

CdCatConfig::CdCatConfig ( QString arg_config_file ) {

    startpar = false;
    startfn = "";

    if (arg_config_file == NULL) {
        QSettings settings;
    } else {
        QSettings settings(arg_config_file, QSettings::NativeFormat);
        qDebug() << "config_file:" << arg_config_file;
    }
    qDebug() << "settings.fileName():" << settings.fileName();
    // QSettings settings_shortcuts(QCoreApplication::organizationName(), PROGRAM_NAME "_shortcuts");

#if defined(_WIN32) || defined(_OS2)
    cdrompath = "D:/";
    lastDir = getenv( "USER_PROFILE" );
#else
    cdrompath = "/dev/sr0";
    lastDir = getenv( "HOME" );
#endif

    hlist.clear();
}


CdCatConfig::~CdCatConfig ( void ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (DEBUG_INFO_ENABLED != NULL) {
        delete DEBUG_INFO_ENABLED;
    }
}

// FIXME change to Unicode
void CdCatConfig::setParameter( char *par ) {
    startpar = true;
    startfn = par;
}

int CdCatConfig::startProgram( DataBase **dbp, QWidget *mw ) {
    QString loadablefile;

    if (autoload) {
        loadablefile = autoloadfn;
    }
    if (startpar) {
        loadablefile = startfn;
    }

    if (autoload || startpar) {
        if ((*dbp) == NULL) {
            (*dbp) = new DataBase();
        }

        PWw *pww = new PWw( mw );
        (*dbp)->pww = pww;
        progress( pww );
        if ((*dbp)->openDB( loadablefile.toLocal8Bit().data() )) {
            if (startpar) {
                qDebug() << (*dbp)->saveAsDB(  loadablefile.toLocal8Bit().data() );
            } else {
                (*dbp) = NULL;
                QMessageBox::warning(
                    mw,
                    tr( "Error during autoload …" ),
                    startpar
                        ? tr( "I can't open the autoload catalog according the first command line parameter.\nCheck the file!\n" )
                        : tr( "I can't open the autoload catalog.\nCheck the file, or change the autoload option in the config dialog!\n" )
                );
            }
        }
        progress( pww );
        pww->end();
        delete pww;
        if ((*dbp) != NULL) {
            (*dbp)->pww = NULL;
            (*dbp)->storeMp3tags = readtag;
            (*dbp)->v1_over_v2 = v1_over_v2;
            (*dbp)->storeMp3techinfo = readinfo;
            (*dbp)->storeAvitechinfo = readavii;
            (*dbp)->storeFileInfo = usefileinfo;
            (*dbp)->storeContent = readcontent;
            (*dbp)->doScanArchive = doScanArchive;
            (*dbp)->doScanArchiveTar = doScanArchive;               // FIXME: make dynamic
            (*dbp)->doScanArchiveLib7zip = doScanArchive;           // FIXME: make dynamic
            (*dbp)->storeExifData = storeExifData;
            (*dbp)->storeThumb = storeThumb;
            (*dbp)->ThumbExtsList = ThumbExtsList;
            (*dbp)->thumbWidth = thumbWidth;
            (*dbp)->thumbHeight = thumbHeight;
            (*dbp)->storeLimit = readclimit;
            (*dbp)->storedFiles = readcfiles;
            (*dbp)->doExcludeFiles = doExcludeFiles;
            (*dbp)->ExcludeFileList = ExcludeFileList;
            (*dbp)->useWildcardInsteadRegexForExclude = useWildcardInsteadRegexForExclude;
            (*dbp)->displayCurrentScannedFileInTray = displayCurrentScannedFileInTray;
        }
    }
    return 0;
}

int CdCatConfig::readConfig() {
    int r = 0, g = 0, b = 0, size = 0;

    /* Default values are the second argument of QSettings.value.
     * If you delete the configuration file (or parts of file)
     * the values will be set back to those.
     */

    /* UI {{{ */
#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    lang = settings.value("main/lang", "eng").toString();
#endif

    fsize                             = settings.value("ui/font_size"                             , 8).toInt();
    ownfont                           = settings.value("ui/use_own_font_size"                     , false).toBool();
    ContentViewerFont                 = settings.value("ui/content_viewer_font"                   , "").toString();
    showTrayIcon                      = settings.value("ui/show_tray_icon"                        , true).toBool();
    displayCurrentScannedFileInTray   = settings.value("ui/display_current_scanned_file_in_tray"  , false).toBool();
    showStatusBar                     = settings.value("ui/show_status_bar"                       , true).toBool();
    showToolBar                       = settings.value("ui/show_tool_bar"                         , true).toBool();
    showCommentDock                   = settings.value("ui/show_comment_dock"                     , true).toBool();
    showProgressedFileInStatus        = settings.value("ui/show_progressed_file_in_status"        , true).toBool();
    showProgressedArchiveFileInStatus = settings.value("ui/show_progressed_archive_file_in_status", true).toBool();
    commentDockSize_height            = settings.value("ui/comment_dock_height"                   , 0).toInt();
    commentDockSize_width             = settings.value("ui/comment_dock_width"                    , 0).toInt();
    commentDockPos_x                  = settings.value("ui/comment_dock_position_x"               , 0).toInt();
    commentDockPos_y                  = settings.value("ui/comment_dock_position_y"               , 0).toInt();
    commentDockSize_dockarea          = settings.value("ui/comment_dock_dockarea"                 , Qt::RightDockWidgetArea).toInt();
    commentDockIsFloating             = settings.value("ui/comment_dock_is_floating"              , false).toBool();
    contentWindowSize_height          = settings.value("ui/content_window_height"                 , 0).toInt();
    contentWindowSize_width           = settings.value("ui/content_window_width"                  , 0).toInt();
    contentWindowPos_x                = settings.value("ui/content_window_position_x"             , 0).toInt();
    contentWindowPos_y                = settings.value("ui/content_window_position_y"             , 0).toInt();
    commentWindowSize_height          = settings.value("ui/comment_window_height"                 , 0).toInt();
    commentWindowSize_width           = settings.value("ui/comment_window_width"                  , 0).toInt();
    commentWindowPos_x                = settings.value("ui/comment_window_position_x"             , 0).toInt();
    commentWindowPos_y                = settings.value("ui/comment_window_position_y"             , 0).toInt();
    findWidth                         = settings.value("ui/find_width"                            , 600).toInt();
    findHeight                        = settings.value("ui/find_height"                           , 500).toInt();
    findX                             = settings.value("ui/find_position_x"                       , 10).toInt();
    findY                             = settings.value("ui/find_position_y"                       , 10).toInt();
    addX                              = settings.value("ui/add_position_x"                        , 50).toInt();
    addY                              = settings.value("ui/add_position_y"                        , 10).toInt();
    addWidth                          = settings.value("ui/add_width"                             , 600).toInt();
    addHeight                         = settings.value("ui/add_height"                            , 500).toInt();
    mainP1                            = settings.value("ui/dirview_size"                          , 200).toInt();
    mainP2                            = settings.value("ui/listview_size"                         , 270).toInt();
    mainP3                            = settings.value("ui/commentview_size"                      , 170).toInt();
    useExternalContentViewer          = settings.value("ui/use_ext_content_viewer"                , false).toBool();
    ExternalContentViewerPath         = settings.value("ui/ext_content_viewer_path"               , "").toString();
    windowPos.setX(                     settings.value("ui/window_position_x"                     , 5).toInt() );
    windowPos.setY(                     settings.value("ui/window_position_y"                     , 5).toInt() );
    windowSize.setHeight(               settings.value("ui/window_height"                         , 480).toInt() );
    windowSize.setWidth(                settings.value("ui/window_width"                          , 640).toInt() );
    /* }}} */

    /* Find dialog {{{ */
    lastFindCategory       = settings.value("find/last_category"      , "").toString();
    keep_search_result     = settings.value("find/keep_search_result" , false).toBool();
    find_in_archive        = settings.value("find/in_archive"         , false).toBool();
    find_cs                = settings.value("find/checkbox_casesens"  , false).toBool();
    find_em                = settings.value("find/checkbox_easymatch" , true).toBool();
    find_di                = settings.value("find/checkbox_directory" , true).toBool();
    find_fi                = settings.value("find/checkbox_file"      , true).toBool();
    find_co                = settings.value("find/checkbox_comment"   , true).toBool();
    find_ct                = settings.value("find/checkbox_content"   , true).toBool();
    find_mar               = settings.value("find/checkbox_mp3artist" , true).toBool();
    find_mti               = settings.value("find/checkbox_mp3title"  , true).toBool();
    find_mal               = settings.value("find/checkbox_mp3album"  , true).toBool();
    find_mco               = settings.value("find/checkbox_mp3comment", true).toBool();
    find_size_min          = settings.value("find/checkbox_size_min"  , false).toBool();
    find_size_max          = settings.value("find/checkbox_size_max"  , false).toBool();
    find_category          = settings.value("find/category"           , false).toBool();
    find_size_min_val      = settings.value("find/size_min_val"       , 1).toInt();
    find_size_max_val      = settings.value("find/size_max_val"       , 10).toInt();
    find_size_unit_min_val = settings.value("find/size_unit_min_val"  , 2).toInt();
    find_size_unit_max_val = settings.value("find/size_unit_max_val"  , 2).toInt();
    find_date_start        = settings.value("find/checkbox_date_start", false).toBool();
    find_date_end          = settings.value("find/checkbox_date_end"  , false).toBool();
    find_date_start_val    = QDateTime().fromString( settings.value("find/date_start_val", "").toString() );
    find_date_end_val      = QDateTime().fromString( settings.value("find/date_end_val"  , "").toString() );
    /* }}} */

    /* Main settings {{{ */
    debug_info_enabled = settings.value("main/debug_info_enabled" , false).toBool();
    autoload           = settings.value("main/autoload"           , false).toBool();
    autosave           = settings.value("main/autosave"           , false).toBool();
    autoloadfn         = settings.value("main/autoload_file"      , "").toString();
    lastSearchPattern  = settings.value("main/last_search_pattern", "").toString();
    lastDir            = settings.value("main/last_dir"           , lastDir).toString();
    lastMediaType      = settings.value("main/last_media_type"    , HARDDISC).toInt();
    cdrompath          = settings.value("main/cdrom_path"         , cdrompath).toString();

#ifndef _WIN32
    mounteject = settings.value("main/mounteject", false).toBool();
#endif

    /* DB file history {{{ */
    historysize = settings.value("main/history_size", 10).toInt();
    size = settings.beginReadArray("history");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        hlist.append(settings.value("path").toString());
        while ((int)hlist.count() > (int)historysize) {
            hlist.removeFirst();
        }
    }
    settings.endArray();
    /* }}} */
    /* }}} */

    /* Color settings {{{ */
    r = settings.value("ui/comment_bg_color/red"  , 255).toInt();
    g = settings.value("ui/comment_bg_color/green", 255).toInt();
    b = settings.value("ui/comment_bg_color/blue" , 0  ).toInt();
    comm_bg.setRgb( r, g, b );

    r = settings.value("ui/comment_fg_color/red"  , 0).toInt();
    g = settings.value("ui/comment_fg_color/green", 0).toInt();
    b = settings.value("ui/comment_fg_color/blue" , 0).toInt();
    comm_fr.setRgb( r, g, b );

    r = settings.value("ui/comment_ts_color/red"  , 0).toInt();
    g = settings.value("ui/comment_ts_color/green", 0).toInt();
    b = settings.value("ui/comment_ts_color/blue" , 0).toInt();
    comm_stext.setRgb( r, g, b );

    r = settings.value("ui/comment_td_color/red"  , 10 ).toInt();
    g = settings.value("ui/comment_td_color/green", 10 ).toInt();
    b = settings.value("ui/comment_td_color/blue" , 255).toInt();
    comm_vtext.setRgb( r, g, b );
    /* }}} */

    /* DB settings {{{ */
    saveAlwaysCatalogInUtf8 = settings.value("db/save_always_catalog_in_utf8", true).toBool();
    nice  = settings.value("db/nice_format", false).toBool();
    linkf = settings.value("ui/catalog_link_is_first", true).toBool();
    /* }}} */

    /* Media scanner {{{ */
    ThumbExtsList  = settings.value("media_scanner/thumb_for_file_extensions", "png;gif;jpg;jpeg;bmp" ).toString().split( ';' );
    storeThumb     = settings.value("media_scanner/store_thumb"              , true).toBool();
    thumbWidth     = settings.value("media_scanner/thumb_width"              , 150).toInt();
    thumbHeight    = settings.value("media_scanner/thumb_height"             , 150).toInt();
    readcfiles     = settings.value("media_scanner/read_content_files"       , "*.nfo;*.diz").toString();
    readclimit     = settings.value("media_scanner/read_content_limit"       , 32 * 1024).toInt();
    doScanArchive  = settings.value("media_scanner/do_scan_archive"          , true).toBool();
    readavii       = settings.value("media_scanner/read_avi_metadata"        , true).toBool();
    readtag        = settings.value("media_scanner/read_mp3tag"              , true).toBool();
    v1_over_v2     = settings.value("media_scanner/mp3tag_default_v1"        , false).toBool();
    readinfo       = settings.value("media_scanner/read_mp3_metadata"        , true).toBool();
    readcontent    = settings.value("media_scanner/read_content"             , false).toBool();
    usefileinfo    = settings.value("media_scanner/use_fileinfo"             , false).toBool();
    storeExifData  = settings.value("media_scanner/store_exif_data"          , false).toBool();

    doExcludeFiles = settings.value("media_scanner/do_exclude_files", false).toBool();
    useWildcardInsteadRegexForExclude = settings.value("media_scanner/use_wildcard_instead_regex_for_exclude", false).toBool();
    size = settings.beginReadArray("db_exclude_pattern_list");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ExcludeFileList.append(settings.value("pattern").toString());
    }
    settings.endArray();
    /* }}} */

    /* Export: HTML {{{ */
    show_archive_file_perms   = settings.value("ui/show_archive_file_perms"  , true).toBool();
    show_archive_file_user    = settings.value("ui/show_archive_file_user"   , true).toBool();
    show_archive_file_group   = settings.value("ui/show_archive_file_group"  , true).toBool();
    show_archive_file_size    = settings.value("ui/show_archive_file_size"   , true).toBool();
    show_archive_file_date    = settings.value("ui/show_archive_file_date"   , true).toBool();
    show_archive_file_comment = settings.value("ui/show_archive_file_comment", true).toBool();
    /* }}} */

    return 0;
}

int CdCatConfig::writeConfig( void ) {

    /* UI {{{ */
#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    settings.setValue("main/lang", lang);
#endif

    settings.setValue("ui/font_size"                             , fsize);
    settings.setValue("ui/use_own_font_size"                     , ownfont);
    settings.setValue("ui/content_viewer_font"                   , ContentViewerFont);
    settings.setValue("ui/show_tray_icon"                        , showTrayIcon);
    settings.setValue("ui/display_current_scanned_file_in_tray"  , displayCurrentScannedFileInTray);
    settings.setValue("ui/show_status_bar"                       , showStatusBar);
    settings.setValue("ui/show_tool_bar"                         , showToolBar);
    settings.setValue("ui/show_comment_dock"                     , showCommentDock);
    settings.setValue("ui/show_progressed_file_in_status"        , showProgressedFileInStatus);
    settings.setValue("ui/show_progressed_archive_file_in_status", showProgressedArchiveFileInStatus);
    settings.setValue("ui/comment_dock_height"                   , commentDockSize_height);
    settings.setValue("ui/comment_dock_width"                    , commentDockSize_width);
    settings.setValue("ui/comment_dock_position_x"               , commentDockPos_x);
    settings.setValue("ui/comment_dock_position_y"               , commentDockPos_y);
    settings.setValue("ui/comment_dock_dockarea"                 , commentDockSize_dockarea);
    settings.setValue("ui/comment_dock_is_floating"              , commentDockIsFloating);
    settings.setValue("ui/content_window_height"                 , contentWindowSize_height);
    settings.setValue("ui/content_window_width"                  , contentWindowSize_width);
    settings.setValue("ui/content_window_position_x"             , contentWindowPos_x);
    settings.setValue("ui/content_window_position_y"             , contentWindowPos_y);
    settings.setValue("ui/comment_window_height"                 , commentWindowSize_height);
    settings.setValue("ui/comment_window_width"                  , commentWindowSize_width);
    settings.setValue("ui/comment_window_position_x"             , commentWindowPos_x);
    settings.setValue("ui/comment_window_position_y"             , commentWindowPos_y);
    settings.setValue("ui/find_width"                            , findWidth);
    settings.setValue("ui/find_height"                           , findHeight);
    settings.setValue("ui/find_position_x"                       , findX);
    settings.setValue("ui/find_position_y"                       , findY);
    settings.setValue("ui/add_position_x"                        , addX);
    settings.setValue("ui/add_position_y"                        , addY);
    settings.setValue("ui/add_width"                             , addWidth);
    settings.setValue("ui/add_height"                            , addHeight);
    settings.setValue("ui/dirview_size"                          , mainP1);
    settings.setValue("ui/listview_size"                         , mainP2);
    settings.setValue("ui/commentview_size"                      , mainP3);
    settings.setValue("ui/use_ext_content_viewer"                , useExternalContentViewer);
    settings.setValue("ui/ext_content_viewer_path"               , ExternalContentViewerPath);
    settings.setValue("ui/window_position_x"                     , windowPos.x());
    settings.setValue("ui/window_position_y"                     , windowPos.y());
    settings.setValue("ui/window_height"                         , windowSize.height());
    settings.setValue("ui/window_width"                          , windowSize.width());
    /* }}} */

    /* Find dialog {{{ */
    settings.setValue("find/last_category"      , lastFindCategory);
    settings.setValue("find/keep_search_result" , keep_search_result);
    settings.setValue("find/in_archive"         , find_in_archive);
    settings.setValue("find/checkbox_casesens"  , find_cs);
    settings.setValue("find/checkbox_easymatch" , find_em);
    settings.setValue("find/checkbox_directory" , find_di);
    settings.setValue("find/checkbox_file"      , find_fi);
    settings.setValue("find/checkbox_comment"   , find_co);
    settings.setValue("find/checkbox_content"   , find_ct);
    settings.setValue("find/checkbox_mp3artist" , find_mar);
    settings.setValue("find/checkbox_mp3title"  , find_mti);
    settings.setValue("find/checkbox_mp3album"  , find_mal);
    settings.setValue("find/checkbox_mp3comment", find_mco);
    settings.setValue("find/checkbox_size_min"  , find_size_min);
    settings.setValue("find/checkbox_size_max"  , find_size_max);
    settings.setValue("find/category"           , find_category);
    settings.setValue("find/size_min_val"       , find_size_min_val);
    settings.setValue("find/size_max_val"       , find_size_max_val);
    settings.setValue("find/size_unit_min_val"  , find_size_unit_min_val);
    settings.setValue("find/size_unit_max_val"  , find_size_unit_max_val);
    settings.setValue("find/checkbox_date_start", find_date_start);
    settings.setValue("find/checkbox_date_end"  , find_date_end);
    settings.setValue("find/date_start_val"     , find_date_start_val.toString());
    settings.setValue("find/date_end_val"       , find_date_end_val.toString());
    /* }}} */

    /* Main settings {{{ */
    settings.setValue("main/debug_info_enabled" , debug_info_enabled);
    settings.setValue("main/autoload"           , autoload          );
    settings.setValue("main/autosave"           , autosave          );
    settings.setValue("main/autoload_file"      , autoloadfn        );
    settings.setValue("main/last_search_pattern", lastSearchPattern );
    settings.setValue("main/last_dir"           , lastDir           );
    settings.setValue("main/last_media_type"    , lastMediaType     );
    settings.setValue("main/cdrom_path"         , cdrompath         );

#ifndef _WIN32
    settings.setValue("main/mounteject", mounteject);
#endif

    /* DB file history {{{ */
    settings.setValue("main/history_size", historysize );

    settings.beginWriteArray("history");
    for (int i = 0; i < hlist.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", hlist.at(i));
    }
    settings.endArray();
    /* }}} */
    /* }}} */

    /* Color settings {{{ */
    settings.setValue("ui/comment_bg_color/red"  , comm_bg.red());
    settings.setValue("ui/comment_bg_color/green", comm_bg.green());
    settings.setValue("ui/comment_bg_color/blue" , comm_bg.blue());

    settings.setValue("ui/comment_fg_color/red"  , comm_fr.red());
    settings.setValue("ui/comment_fg_color/green", comm_fr.green());
    settings.setValue("ui/comment_fg_color/blue" , comm_fr.blue());

    settings.setValue("ui/comment_ts_color/red"  , comm_stext.red());
    settings.setValue("ui/comment_ts_color/green", comm_stext.green());
    settings.setValue("ui/comment_ts_color/blue" , comm_stext.blue());

    settings.setValue("ui/comment_td_color/red"  , comm_vtext.red());
    settings.setValue("ui/comment_td_color/green", comm_vtext.green());
    settings.setValue("ui/comment_td_color/blue" , comm_vtext.blue());
    /* }}} */

    /* DB settings {{{ */
    settings.setValue("db/save_always_catalog_in_utf8", saveAlwaysCatalogInUtf8);
    settings.setValue("db/nice_format"          , nice);
    settings.setValue("ui/catalog_link_is_first", linkf);
    /* }}} */

    /* Media scanner {{{ */
    settings.setValue("media_scanner/thumb_for_file_extensions", ThumbExtsList.join(";"));
    settings.setValue("media_scanner/store_thumb"              , storeThumb);
    settings.setValue("media_scanner/thumb_width"              , thumbWidth);
    settings.setValue("media_scanner/thumb_height"             , thumbHeight);
    settings.setValue("media_scanner/read_content_files"       , readcfiles);
    settings.setValue("media_scanner/read_content_limit"       , readclimit);
    settings.setValue("media_scanner/do_scan_archive"          , doScanArchive);
    settings.setValue("media_scanner/read_avi_metadata"        , readavii);
    settings.setValue("media_scanner/read_mp3tag"              , readtag);
    settings.setValue("media_scanner/mp3tag_default_v1"        , v1_over_v2);
    settings.setValue("media_scanner/read_mp3_metadata"        , readinfo);
    settings.setValue("media_scanner/read_content"             , readcontent);
    settings.setValue("media_scanner/use_fileinfo"             , usefileinfo);
    settings.setValue("media_scanner/store_exif_data"          , storeExifData);

    settings.setValue("media_scanner/do_exclude_files", doExcludeFiles);
    settings.setValue("media_scanner/use_wildcard_instead_regex_for_exclude", useWildcardInsteadRegexForExclude);
    settings.beginWriteArray("db_exclude_pattern_list");
    for (int i = 0; i < ExcludeFileList.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("pattern", ExcludeFileList.at(i));
    }
    settings.endArray();
    /* }}} */

    /* Export: HTML {{{ */
    settings.setValue("ui/show_archive_file_perms"  , show_archive_file_perms);
    settings.setValue("ui/show_archive_file_user"   , show_archive_file_user);
    settings.setValue("ui/show_archive_file_group"  , show_archive_file_group);
    settings.setValue("ui/show_archive_file_size"   , show_archive_file_size);
    settings.setValue("ui/show_archive_file_date"   , show_archive_file_date);
    settings.setValue("ui/show_archive_file_comment", show_archive_file_comment);
    /* }}} */

    return 0;
}

/************************************************************************************/
ConfigDialog::ConfigDialog ( CdCatMainWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {

    if (!name) {
        setObjectName( "ConfigDialog" );
    }
    setWindowIcon( *get_t_config_icon());
    setModal( modal );
    p = parent;

    setSizeGripEnabled( true );
    ConfigDialogBaseLayout = new QGridLayout( this );

    layout1 = new QHBoxLayout( this );
    cbAutoload = new QCheckBox( this );
    filename = new QLineEdit( this );
    searchButton = new QPushButton( this );
    searchButton->setText( "..." );
    searchButton->setFlat( false );

    layout1->addWidget( cbAutoload );

    layout1->addWidget( filename );
    layout1->addWidget( searchButton );
    ConfigDialogBaseLayout->addLayout( layout1, 0, 0 );

    line1 = new QFrame( this );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line1, 2, 0 );

    cbOwnFont = new QCheckBox( tr( "Use own font size" ), this );


    layout4 = new QHBoxLayout( this );
    spinFontSize = new QSpinBox( this );
    spinFontSize->setMaximumWidth( 80 );
    QSpacerItem *fontspacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    lab = new QLabel( this );
    layout4->addWidget( cbOwnFont, 0 );
    layout1->addItem( fontspacer );
    layout4->addWidget( spinFontSize );
    layout4->addWidget( lab );
    ConfigDialogBaseLayout->addLayout( layout4, 4, 0 );

    line2 = new QFrame( this );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line2, 5, 0 );

    cbAutosave = new QCheckBox( this );
    ConfigDialogBaseLayout->addWidget( cbAutosave, 6, 0 );

    line8 = new QFrame( this );
    line8->setFrameShape( QFrame::HLine );
    line8->setFrameShadow( QFrame::Sunken );
    line8->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line8, 7, 0 );

    layoutSave = new QHBoxLayout( this );
    cbNice = new QCheckBox( this );
    cbSaveCatalogAlwaysInUtf8 = new QCheckBox( this );
    layoutSave->addWidget( cbNice );
    layoutSave->addWidget( cbSaveCatalogAlwaysInUtf8 );
    ConfigDialogBaseLayout->addLayout( layoutSave, 8, 0 );

    line3 = new QFrame( this );
    line3->setFrameShape( QFrame::HLine );
    line3->setFrameShadow( QFrame::Sunken );
    line3->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line3, 9, 0 );

    layout5 = new QHBoxLayout( this );
    cdrompath_lineedit = new QLineEdit( this );
    layout5->addWidget( cdrompath_lineedit );
    cdrom_lab = new QLabel( this );
    layout5->addWidget( cdrom_lab );
    searchButton2 = new QPushButton( this );
    searchButton2->setText( "..." );
    searchButton2->setFlat( false );
    layout5->addWidget( searchButton2 );
    ConfigDialogBaseLayout->addLayout( layout5, 10, 0 );

#ifndef _WIN32
    cbMoEj = new QCheckBox( this );
    ConfigDialogBaseLayout->addWidget( cbMoEj, 12, 0 );
#endif

#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    layout9 = new QHBoxLayout( this );
    cbLang = new QComboBox( this );
    langLabel = new QLabel( this );
    layout9->insertSpacing( 0, 5 );
    layout9->insertWidget( 1, cbLang );
    layout9->insertWidget( 2, langLabel );
    layout9->insertSpacing( 3, 5 );
    ConfigDialogBaseLayout->addLayout( layout9, 13, 0 );

    /*scanning existing languages:*/
    QDir d( "./lang" );
    cbLang->insertItem( 0, QString( "eng" ));
    if (d.exists()) {
        d.setFilter( QDir::Files );
        QStringList filters;
        filters << "cdcat_??.qm";
        d.setNameFilters( filters );
        QFileInfoList list( d.entryInfoList());
        // QFileInfoListIterator it ( list. );
        // QFileInfo *fi;
        foreach( const QFileInfo &fi, list ) {
            // while ( ( fi = it.current() ) != 0 )
            cbLang->insertItem( 0, (fi.fileName()).mid( 6, 2 ));
            // ++it;
        }
        /*end scanning*/
    } else {
        cbLang->setEnabled( false );
    }
#endif

    line5 = new QFrame( this );
    line5->setFrameShape( QFrame::HLine );
    line5->setFrameShadow( QFrame::Sunken );
    line5->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line5, 14, 0 );

    layout6 = new QHBoxLayout( this );
    spinHistorySize = new QSpinBox( this );
    spinHistorySize->setMaximumWidth( 80 );
    layout6->addWidget( spinHistorySize );
    labHistorySize = new QLabel( this );
    layout6->addWidget( labHistorySize );
    ConfigDialogBaseLayout->addLayout( layout6, 15, 0 );

    layoutDock = new QHBoxLayout( this );
    cbShowTrayIcon = new QCheckBox( tr( "show systray icon" ), this );
    cbShowCurrentScannedFileInTrayIcon = new QCheckBox( tr( "display current scanned file in tray" ), this );
    // QSpacerItem* systrayspacer1 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    // layoutDock->addItem(systrayspacer1);
    layoutDock->addWidget( cbShowTrayIcon );
    layoutDock->addWidget( cbShowCurrentScannedFileInTrayIcon );


    ConfigDialogBaseLayout->addLayout( layoutDock, 16, 0 );


    line6 = new QFrame( this );
    line6->setFrameShape( QFrame::HLine );
    line6->setFrameShadow( QFrame::Sunken );
    line6->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line6, 17, 0 );

    riButton = new QPushButton( this );
    ConfigDialogBaseLayout->addWidget( riButton, 18, 0 );

    line7 = new QFrame( this );
    line7->setFrameShape( QFrame::HLine );
    line7->setFrameShadow( QFrame::Sunken );
    line7->setFrameShape( QFrame::HLine );
    ConfigDialogBaseLayout->addWidget( line7, 19, 0 );

    layoutStatus = new QHBoxLayout( this );
    cbEnableDebugInfo = new QCheckBox( this );
    cbShowProgressedFileInStatus = new QCheckBox( this );
    layoutStatus->addWidget( cbEnableDebugInfo );
    layoutStatus->addWidget( cbShowProgressedFileInStatus );
    ConfigDialogBaseLayout->addLayout( layoutStatus, 20, 0 );

    layout7 = new QHBoxLayout( this );
    QSpacerItem *spacer = new QSpacerItem( 110, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer );

    okButton = new QPushButton( this );
    okButton->setMinimumSize( QSize( 100, 0 ));
    okButton->setAutoDefault( true );
    okButton->setDefault( true );
    layout7->addWidget( okButton );

    cancelButton = new QPushButton( this );
    cancelButton->setMinimumSize( QSize( 100, 0 ));
    cancelButton->setAutoDefault( true );
    layout7->addWidget( cancelButton );
    QSpacerItem *spacer_2 = new QSpacerItem( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer_2 );

    ConfigDialogBaseLayout->addLayout( layout7, 22, 0 );

    connect( searchButton2, SIGNAL( clicked()), this, SLOT( cdrombutton()));

    languageChange();
    resize( QSize( 506, 309 ).expandedTo( minimumSizeHint()));

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked()), this, SLOT( close()));
    connect( okButton, SIGNAL( clicked()), this, SLOT( okExit()));
    connect( searchButton, SIGNAL( clicked()), this, SLOT( ffdbutton()));
    connect( cbOwnFont, SIGNAL( clicked()), this, SLOT( ownFontToggled()));
    connect( riButton, SIGNAL( clicked()), this, SLOT( runri()));
    connect( cbShowTrayIcon, SIGNAL( clicked()), this, SLOT( showDockToggled()));

    if (p->cconfig->ownfont) {
        spinFontSize->setEnabled( true );
        lab->setEnabled( true );
    } else {
        spinFontSize->setEnabled( false );
        lab->setEnabled( false );
    }

    if (p->cconfig->ownfont) {
        cbOwnFont->setChecked( true );
    } else {
        cbOwnFont->setChecked( false );
    }

    cbNice->setChecked( p->cconfig->nice );
    cbShowTrayIcon->setChecked( p->cconfig->showTrayIcon );
    cbShowCurrentScannedFileInTrayIcon->setChecked( p->cconfig->displayCurrentScannedFileInTray );
    cbAutoload->setChecked( p->cconfig->autoload );
    cbAutosave->setChecked( p->cconfig->autosave );
    if (!p->cconfig->autoloadfn.isEmpty()) {
        filename->setText( p->cconfig->autoloadfn );
    }
    spinFontSize->setValue( p->cconfig->fsize );
    spinHistorySize->setValue( p->cconfig->historysize );

    cdrompath_lineedit->setText( p->cconfig->cdrompath );
#ifndef _WIN32
    cbMoEj->setChecked( p->cconfig->mounteject );
#endif

#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    cbLang->setCurrentIndex( 0 );
    int index = cbLang->findText( p->cconfig->lang );
    if (index != -1) {
        cbLang->setCurrentIndex( index );
    }
#endif

    cbEnableDebugInfo->setChecked( p->cconfig->debug_info_enabled );
    cbSaveCatalogAlwaysInUtf8->setChecked( p->cconfig->saveAlwaysCatalogInUtf8 );
    cbShowProgressedFileInStatus->setChecked( p->cconfig->showProgressedFileInStatus );
}

/*
 *  Destroys the object and frees any allocated resources.
 */
ConfigDialog::~ConfigDialog() {
    // No need to delete child widgets, Qt does it all for us.
}

void ConfigDialog::languageChange() {
    setWindowTitle( tr( "Configure %1 …" ).arg(PROGRAM_NAME));
    cbShowTrayIcon->setText( tr( "Show systray icon" ));
    cbShowCurrentScannedFileInTrayIcon->setText( tr( "display current scanned file in tray" ));
    cbShowCurrentScannedFileInTrayIcon->setToolTip( tr( "display current scanned file in tray (mediainfo / archive scan)" ));
    cbAutoload->setText( tr( "Autoload DataBase on startup" ));
    cbAutosave->setText( tr( "Automatically save the database after every scan (for safety sake)" ));
    cbNice->setText( tr( "Save the XML-db.file nicer format(needs more space)" ));
    cancelButton->setText( tr( "Cancel" ));
    okButton->setText( tr( "Ok" ));
    riButton->setText( tr( "Select additional items to read" ));
    cbOwnFont->setText( tr( "Use own font size" ));
    lab->setText( tr( "font size" ));
    cbOwnFont->setToolTip( tr( "Application font size.(must restart cdcat!) " ));
    labHistorySize->setText( tr( "Number of history entries" ));
    cdrom_lab->setText( tr( "Path to cdrom device / mount dir" ));
    cdrompath_lineedit->setToolTip( tr( "This is the path to the cdrom/dvd device or the path where its mounted" ));

#ifndef _WIN32
    cbMoEj->setText( tr( "Scanning: mount cdrom at start / eject when finish" ));
#endif

#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    langLabel->setText( tr( "The language of %1 interface" ).arg(PROGRAM_NAME));
#endif

    cbEnableDebugInfo->setText( tr( "Display debug info on console" ));
    cbSaveCatalogAlwaysInUtf8->setText( tr( "Save catalogs always as UTF8" ));
    cbShowProgressedFileInStatus->setText( tr( "Show progressed file at scanning in status label" ));
}

void ConfigDialog::ffdbutton() {
    QString s = QFileDialog::getOpenFileName(
        0,
        tr( "Choose a file to load automatically!" ),
        p->cconfig->lastDir,
        tr( "%1 databases (*.hcf )" )
            .arg(PROGRAM_NAME)
    );

    if (!s.isEmpty()) {
        filename->setText( s );
    }
}

void ConfigDialog::cdrombutton() {
    QString s = QFileDialog::getExistingDirectory( 0, tr( "Choose path to cdrom!" ), p->cconfig->lastDir );

    if (!s.isEmpty()) {
        cdrompath_lineedit->setText( s );
    }
}

void ConfigDialog::okExit() {
    p->cconfig->nice = cbNice->isChecked();
    p->cconfig->showTrayIcon = cbShowTrayIcon->isChecked();
    p->cconfig->autoload = cbAutoload->isChecked();
    p->cconfig->autosave = cbAutosave->isChecked();
    p->cconfig->autoloadfn = filename->text();
    p->cconfig->ownfont = cbOwnFont->isChecked();
    p->cconfig->fsize = spinFontSize->value();
    p->cconfig->historysize = spinHistorySize->value();
    p->cconfig->cdrompath = cdrompath_lineedit->text();
#ifndef _WIN32
    p->cconfig->mounteject = cbMoEj->isChecked();
#endif

#if defined(_WIN32) || defined(Q_OS_MAC) || defined(_OS2)
    QString langfile( "./lang/cdcat_" );
    langfile += p->cconfig->lang;
    langfile += ".qm";


    p->app->removeTranslator( translator );
    delete(translator);

    translator = new QTranslator( 0 );
    translator->load( langfile, "." );


    // read the value
    p->cconfig->lang = cbLang->currentText();

    translator = 0;
    langfile = "./lang/cdcat_";
    langfile += p->cconfig->lang;
    langfile += ".qm";

    translator = new QTranslator( 0 );
    translator->load( langfile, "." );
    p->app->installTranslator( translator );
#endif

    p->cconfig->debug_info_enabled = cbEnableDebugInfo->isChecked();
    DEBUG_INFO_ENABLED = init_debug_info();
    *DEBUG_INFO_ENABLED = cbEnableDebugInfo->isChecked();
    p->cconfig->saveAlwaysCatalogInUtf8 = cbSaveCatalogAlwaysInUtf8->isChecked();
    p->cconfig->showProgressedFileInStatus = cbShowProgressedFileInStatus->isChecked();
    p->cconfig->displayCurrentScannedFileInTray = cbShowCurrentScannedFileInTrayIcon->isChecked();

    p->cconfig->writeConfig();

    QFont font;
    font.setPointSize( p->cconfig->fsize );
    if (p->cconfig->ownfont) {
        p->app->setFont( font );
    } else {
        p->app->setFont( p->cconfig->defaultfont );
    }

    close();
}

void ConfigDialog::ownFontToggled() {
    if (cbOwnFont->isChecked()) {
        lab->setEnabled( true );
        spinFontSize->setEnabled( true );
    } else {
        lab->setEnabled( false );
        spinFontSize->setEnabled( false );
    }
}

void ConfigDialog::showDockToggled() {
    if (cbShowTrayIcon->isChecked()) {
        cbShowCurrentScannedFileInTrayIcon->setEnabled( true );
    } else {
        cbShowCurrentScannedFileInTrayIcon->setEnabled( false );
    }
}


void ConfigDialog::runri() {
    SelReadable *sr = new SelReadable( p->cconfig, this, "runri", true );

    sr->exec();
    delete sr;
}

