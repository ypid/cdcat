/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
#include <qdialog.h>
#include <qstringlist.h>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QLineEdit;
class QPushButton;
class DataBase;
class QLabel;
class QWidget;
class CdCatMainWidget;
class QSpinBox;
class QFrame;
class QFont;
class QComboBox;
class QGroupBox;
class QColor;

class CdCatConfig : public QObject {
    Q_OBJECT
public:
    CdCatConfig ( void );
    ~CdCatConfig ( void );

    int startProgram( DataBase **dbp, QWidget *mw );
    int writeConfig();
    int readConfig();
    void setParameter( char *par );

public:
    bool startpar;
    QString startfn;

    // Configureable items:
    int fsize;
    int historysize;
    bool showTrayIcon;
    bool commentDockIsFloating;
    bool showStatusBar;
    bool showToolBar;
    bool showCommentDock;
    bool autoload;
    QString autoloadfn;
    bool nice;
    bool autosave;
    bool ownfont;
    bool readtag;
    bool v1_over_v2;
    bool readinfo;
    bool readavii;
    bool readcontent;
    bool usefileinfo;
    bool debug_info_enabled;
    bool saveAlwaysCatalogInUtf8;
    bool showProgressedFileInStatus;
    bool doScanArchive;
    bool showProgressedArchiveFileInStatus;
    bool doExcludeFiles;
    bool useWildcardInsteadRegexForExclude;
    bool useExternalContentViewer;
    bool displayCurrentScannedFileInTray;


    // bool doScanArchiveTar;
    // bool doScanArchiveLib7zip;
    bool storeExifData;
    bool storeThumb;

    unsigned long readclimit;

    QStringList hlist;
    QStringList ThumbExtsList;
    QStringList ExcludeFileList;
    QSize windowSize;
    QPoint windowPos;
    int windowSize_height;
    int windowSize_width;
    int commentDockPos_x;
    int commentDockPos_y;
    int commentDockSize_width;
    int commentDockSize_height;
    int commentDockSize_dockarea;
    int contentWindowPos_x;
    int contentWindowPos_y;
    int contentWindowSize_height;
    int contentWindowSize_width;
    int commentWindowPos_x;
    int commentWindowPos_y;
    int commentWindowSize_height;
    int commentWindowSize_width;
    QString cdrompath;
    QString lastDir;
    QString lastSearchPattern;
    QString readcfiles;
    QString lastFindCategory;
    QString ExternalContentViewerPath;
    QString ContentViewerFont;
    int mainP1;
    int mainP2;
    int mainP3;
    int lastMediaType;

    int find_size_unit_min_val;
    int find_size_unit_max_val;

    int find_size_min_val;
    int find_size_max_val;

    int thumbWidth;
    int thumbHeight;

    bool find_cs;
    bool find_em;
    bool find_di;
    bool find_fi;
    bool find_co;
    bool find_category;
    bool find_ct;
    bool find_mco;
    bool find_mar;
    bool find_mti;
    bool find_mal;
    bool find_date_start;
    bool find_date_end;
    bool find_size_min;
    bool find_size_max;
    bool find_unsharp_search;
    bool find_in_archive;
    bool show_archive_file_perms;
    bool show_archive_file_user;
    bool show_archive_file_group;
    bool show_archive_file_size;
    bool show_archive_file_date;
    bool show_archive_file_comment;
    bool keep_search_result;

    int findX;
    int findY;
    int findWidth;
    int findHeight;
    int addX;
    int addY;
    int addWidth;
    int addHeight;

    QDateTime find_date_start_val;
    QDateTime find_date_end_val;

    QColor comm_bg, comm_stext, comm_vtext, comm_fr;
    QFont defaultfont;
#ifndef _WIN32
    bool mounteject;
#endif

#if defined(_WIN32) || defined(_OS2)
    QString lang;
#endif
#ifdef Q_OS_MAC
    QString lang;
#endif

    bool linkf;


    /*end configureable items*/
};

/*****************************************************************************/

class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    ConfigDialog ( CdCatMainWidget *parent = 0, const char *name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~ConfigDialog();

    CdCatMainWidget *p;
    QLineEdit *filename;
    QPushButton *searchButton;
    QFrame *line1;
    QSpinBox *spinFontSize;
    QSpinBox *spinHistorySize;
    QLabel *lab;
    QLabel *labHistorySize;
    QCheckBox *cbNice;
    QCheckBox *cbEnableDebugInfo;
    QCheckBox *cbSaveCatalogAlwaysInUtf8;
    QCheckBox *cbShowProgressedFileInStatus;
    QLineEdit *cdrompath_lineedit;
    QLabel *cdrom_lab;
    QFrame *line2;
    QFrame *line3;
    QFrame *line4;
    QFrame *line5;
    QFrame *line6;
    QFrame *line61;
    QFrame *line7;
    QFrame *line8;
    QPushButton *riButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QCheckBox *cbShowTrayIcon;
    QCheckBox *cbShowCurrentScannedFileInTrayIcon;
    QCheckBox *cbAutoload;
    QCheckBox *cbAutosave;
    QCheckBox *cbOwnFont;

protected:
    QGridLayout *ConfigDialogBaseLayout;
    QHBoxLayout *layout1;
    QHBoxLayout *layout2;
    QHBoxLayout *layout3;
    QHBoxLayout *layout4;
    QHBoxLayout *layout5;
    QHBoxLayout *layout6;
    QHBoxLayout *layout7;
    QHBoxLayout *layout8;
    QHBoxLayout *layoutSave;
    QHBoxLayout *layoutStatus;
    QHBoxLayout *layoutDock;

#ifndef _WIN32
    QCheckBox *cbMoEj;
#endif

#if defined(_WIN32) || defined(_OS2)
    QHBoxLayout *layout9;
    QComboBox *cbLang;
    QLabel *langLabel;
#endif

#ifdef Q_OS_MAC
    QHBoxLayout *layout9;
    QComboBox *cbLang;
    QLabel *langLabel;
#endif

    QPushButton *searchButton2;             // maybe problem for moc

public slots:
    virtual void okExit();
    virtual void ffdbutton();
    virtual void runri();

protected slots:
    virtual void languageChange();
    void ownFontToggled();
    virtual void cdrombutton();
    void showDockToggled();
};

#endif // CONFIGDIALOG_H

#ifndef __DEBUG_INFO__
#define __DEBUG_INFO__
static bool *DEBUG_INFO_ENABLED;
#endif

// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
