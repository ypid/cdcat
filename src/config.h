/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>
#include <qdialog.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QLabel>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QCheckBox;
class QLineEdit;
class QPushButton;
class DataBase;
class QLabel;
class QWidget;
class CdCatMainWidget;
class QSpinBox;
class Q3Frame;
class QFont;
class QComboBox;
class QColor;

class CdCatConfig : public QObject {
    Q_OBJECT
public:
    char errormsg[512];

    CdCatConfig ( void );
    ~CdCatConfig ( void );

    int startProgram ( DataBase **dbp,QWidget *mw );
    int writeConfig();
    int readConfig ();
    void setParameter ( char *par );

public:

    bool    startpar;
    QString startfn;

    //Configureable items:
    int     fsize;
    int     historysize;
    bool    autoload;
    QString autoloadfn;
    bool    nice;
    bool    autosave;
    bool    ownfont;
    bool    readtag;
    bool    v1_over_v2;
    bool    readinfo;
    bool    readavii;
    bool    readcontent;
    bool	debug_info_enabled;
    QString readcfiles;
    unsigned long readclimit;

    QStringList hlist;
    QSize   windowSize;
    QPoint  windowPos;
    int windowSize_height;
    int windowSize_width;
    QString cdrompath;
    QString lastDir;
    int  mainP1;
    int  mainP2;
    int  mainP3;

    bool find_cs;
    bool find_em;
    bool find_di;
    bool find_fi;
    bool find_co;
    bool find_ct;
    bool find_mco;
    bool find_mar;
    bool find_mti;
    bool find_mal;
    int  findX;
    int  findY;
    int  findWidth;
    int  findHeight;

    QColor *comm_bg,*comm_stext,*comm_vtext,*comm_fr;
    QFont *defaultfont;
#ifndef _WIN32
    bool    mounteject;
#else
    QString lang;
#endif

    bool linkf;
    

    /*end configureable items*/

};

/*****************************************************************************/

class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    ConfigDialog ( CdCatMainWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~ConfigDialog();

    CdCatMainWidget *p;
    QLineEdit* filename;
    QPushButton* searchButton;
    Q3Frame* line1;
    QSpinBox *spinFontSize;
    QSpinBox *spinHistorySize;
    QLabel* lab;
    QLabel *labHistorySize;
    QCheckBox* cbNice;
    QCheckBox* cbEnableDebugInfo;
    QLineEdit* cdrompath_lineedit;
    QLabel* cdrom_lab;
    Q3Frame* line2;
    Q3Frame* line3;
    Q3Frame* line4;
    Q3Frame* line5;
    Q3Frame* line6;
    Q3Frame* line7;
    Q3Frame* line8;
    QPushButton* riButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QCheckBox* cbAutoload;
    QCheckBox* cbAutosave;
    QCheckBox* cbOwnFont;


protected:
    Q3GridLayout* ConfigDialogBaseLayout;
    Q3HBoxLayout* layout1;
    Q3HBoxLayout* layout2;
    Q3HBoxLayout* layout3;
    Q3HBoxLayout* layout4;
    Q3HBoxLayout* layout5;
    Q3HBoxLayout* layout6;
    Q3HBoxLayout* layout7;
    Q3HBoxLayout* layout8;


#ifndef _WIN32
    QCheckBox* cbMoEj;
#else
    Q3HBoxLayout* layout9;
    QComboBox  * cbLang;
    QLabel     * langLabel;
#endif

    QPushButton* searchButton2; // maybe problem for moc

public slots:
    virtual void okExit();
    virtual void ffdbutton();
    virtual void runri();


protected slots:
    virtual void languageChange();
    void ownFontToggled();
    virtual void cdrombutton();
};

#endif // CONFIGDIALOG_H

#ifndef __DEBUG_INFO__
#define __DEBUG_INFO__
static bool *DEBUG_INFO_ENABLED;
#endif

