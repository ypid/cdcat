/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak, 2010 Christoph Thielecke
****************************************************************************/

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QVariant>
#include <QDialog>
#include <QObject>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QCloseEvent>
// #include <pcre.h>
#include <QRegExp>
#include <QtWidgets/QDateTimeEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QSpinBox;
class QPushButton;
class DateTimeEdit;
class CdCatMainWidget;
class QComboBox;
class findDialog;
class findDuplicatesDialog;
class Node;
class PWw;

class seekEngine : public QObject {
    Q_OBJECT
private:
    int founded;
    PWw *pww;
    // pcre *re;
    QRegExp *re;

    char *patt;
    int offsets[99];
    // pcre_extra *hints;
    const char *error;
    int errptr;

    findDialog *fd;

    int  analyzeNode( PWw *pww, Node *n, Node *pa = NULL );
    void putNodeToList( Node *n, QString comment = "" );
    int  matchIt( QString txt );
    bool matchUnsharp( char *a, char *b );

public:
    seekEngine ( findDialog *fdp, bool isFindDuplicates = false );
    ~seekEngine ( void );

    int start_seek( void );

private:
    bool dirname;
    bool filename;
    bool comment;
    bool find_category;
    bool tartist;
    bool ttitle;
    bool talbum;
    bool tcomment;
    bool content;
    bool allmedia;
    bool allowner;
    bool dateStartChecked;
    bool dateEndChecked;
    bool sizeMinChecked;
    bool sizeMaxChecked;
    bool findInArchivesChecked;
    bool searchForDuplicates;

    QString media;
    QString owner;
    QString category;

    QDateTime dateStart;
    QDateTime dateEnd;

    double size_min;
    double size_max;
};

/***********************************************************************/

class findDialog : public QDialog {
    Q_OBJECT

public:
    findDialog ( CdCatMainWidget *parent = 0, const char *name = 0, bool isFindDuplicates = false, bool modal = false, Qt::WindowFlags fl = Qt::WindowMinMaxButtonsHint );
    ~findDialog();


    QFrame *input;
    QHBoxLayout *h1, *h2, *h3, *h4;
    QVBoxLayout *v1;
    CdCatMainWidget *mainw;
    QPushButton *buttonClose;
    QPushButton *buttonExportResult;
    QPushButton *buttonPrintResult;
    QFrame *line1;
    QComboBox *cbOwner;
    QComboBox *cbSin;
    QPushButton *buttonCancel;
    QPushButton *buttonOk;
    QPushButton *buttonClearSearchResult;
    QLabel *findTextLabel;
    QLabel *textLabel1;
    QLabel *textLabel2;
    QLabel *textLabel5;
    QLabel *textLabel6;
    QLabel *textLabel7;
    QLabel *textLabel8;
    QLabel *textLabelFindInArchive;
    QFrame *line2;
    QTreeWidget *resultsl;
    QLineEdit *leText;
    QLineEdit *leCategory;
    QSpinBox *spSizeMin;
    QSpinBox *spSizeMax;
    QComboBox *cbSizeUnitMin;
    QComboBox *cbSizeUnitMax;

    QDateTimeEdit *deDateStart;
    QDateTimeEdit *deDateEnd;
    QCheckBox *cbComment;
    QCheckBox *cbCategory;
    QCheckBox *cbFilename;
    QCheckBox *cbDirname;
    QCheckBox *cbTcomm;
    QCheckBox *cbAlbum;
    QCheckBox *cbTitle;
    QCheckBox *cbArtist;
    QCheckBox *cbCasesens;
    QCheckBox *cbEasy;
    QCheckBox *cbContent;
    QCheckBox *cbDateStart;
    QCheckBox *cbDateEnd;
    QCheckBox *cbSizeMin;
    QCheckBox *cbSizeMax;
    QCheckBox *cbUnsharpSearch;
    QCheckBox *cbFindInArchive;
    QCheckBox *cbKeepSearchResult;

    bool use_unsharpsearch;             // if unsharp match should be used
    bool isFindDuplicates;

protected:
    int fillCBox( void );
    void closeEvent( QCloseEvent *ce );
    void exportResult( bool isPrint );

    QGridLayout *FindDialogBaseLayout;
    QVBoxLayout *layout40;
    QGridLayout *layout39;
    QGridLayout *layout36;
    QGridLayout *layout37;
    QVBoxLayout *layout31;
    QHBoxLayout *layout17;
    QHBoxLayout *findLineLayout;
    QGridLayout *layout15;
    QHBoxLayout *layout30;
    QGridLayout *layout16;
    QHBoxLayout *layout_size_min;
    QHBoxLayout *layout_size_max;
    QHBoxLayout *layout_find_in_archive;

    QString searchFilepath;

protected slots:
    virtual void languageChange();
    int select( QTreeWidgetItem *after, QTreeWidgetItem *before );
    int select( QTreeWidgetItem *i, int col );
    int closee( void );
    int cancele( void );
    int seeke( void );
    int saveState( void );
    void dateStartChanged( int );
    void dateEndChanged( int );
    void dateStartDoubleClicked();
    void dateEndDoubleClicked();
    void sizeMinClicked();
    void sizeMaxClicked();
    void categoryClicked();
    void printResultClicked();
    void exportResultClicked();
    void clearSearchResultClicked();
};

#endif // FINDDIALOG_H
