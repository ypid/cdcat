/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qobject.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QCloseEvent>
// #include <pcre.h>
#include <QRegExp>
#include <QtGui/QDateTimeEdit>

class Q3VBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class Q3ListView;
class Q3ListViewItem;
class QHBoxLayout;
class Q3VBoxLayout;
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
		//pcre *re;
		QRegExp *re;
		
		char *patt;
		int  offsets[99];
		//pcre_extra *hints;
		const char *error;
		int errptr;
		
		findDialog *fd;
		
		int  analyzeNode ( PWw *pww, Node *n, Node *pa = NULL );
		void putNodeToList ( Node *n, QString comment = "" );
		int  matchIt ( QString txt );
		bool matchUnsharp ( char *a, char *b );
	
	public:
		seekEngine ( findDialog *fdp, bool isFindDuplicates = false );
		~seekEngine ( void );
		
		int start_seek ( void );
	
	private:
		bool dirname;
		bool filename;
		bool comment;
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
		
		QString  media;
		QString  owner;
		
		QDateTime dateStart;
		QDateTime dateEnd;
		
		double size_min;
		double size_max;
};

/***********************************************************************/

class findDialog : public QDialog {
		Q_OBJECT

	public:
		findDialog ( CdCatMainWidget* parent = 0, const char* name = 0, bool isFindDuplicates = false, bool modal = FALSE, Qt::WFlags fl = Qt::WStyle_MinMax );
		~findDialog();
		
		
		QFrame *input;
		QHBoxLayout *h1, *h2, *h3, *h4;
		Q3VBoxLayout *v1;
		CdCatMainWidget *mainw;
		QPushButton* buttonClose;
		QPushButton* buttonExportResult;
		QPushButton* buttonPrintResult;
		QFrame* line1;
		QComboBox* cbOwner;
		QComboBox* cbSin;
		QPushButton* buttonCancel;
		QPushButton* buttonOk;
		QPushButton* buttonClearSearchResult;
		QLabel* findTextLabel;
		QLabel* textLabel1;
		QLabel* textLabel2;
		QLabel* textLabel5;
		QLabel* textLabel6;
		QLabel* textLabel7;
		QLabel* textLabel8;
		QLabel* textLabelFindInArchive;
		QFrame* line2;
		Q3ListView* resultsl;
		QLineEdit* leText;
		QSpinBox* spSizeMin;
		QSpinBox* spSizeMax;
		QComboBox *cbSizeUnitMin;
		QComboBox *cbSizeUnitMax;
		
		QDateTimeEdit* deDateStart;
		QDateTimeEdit* deDateEnd;
		QCheckBox* cbComment;
		QCheckBox* cbFilename;
		QCheckBox* cbDirname;
		QCheckBox* cbTcomm;
		QCheckBox* cbAlbum;
		QCheckBox* cbTitle;
		QCheckBox* cbArtist;
		QCheckBox* cbCasesens;
		QCheckBox* cbEasy;
		QCheckBox* cbContent;
		QCheckBox* cbDateStart;
		QCheckBox* cbDateEnd;
		QCheckBox* cbSizeMin;
		QCheckBox* cbSizeMax;
		QCheckBox* cbUnsharpSearch;
		QCheckBox* cbFindInArchive;
		QCheckBox* cbKeepSearchResult;

		bool use_unsharpsearch; // if unsharp match should be used
		bool isFindDuplicates;
	
	protected:
		int fillCBox ( void );
		void closeEvent ( QCloseEvent *ce );
		void exportResult ( bool isPrint );
		
		QGridLayout* FindDialogBaseLayout;
		Q3VBoxLayout* layout40;
		QGridLayout* layout39;
		QGridLayout* layout36;
		QGridLayout* layout37;
		Q3VBoxLayout* layout31;
		QHBoxLayout* layout17;
		QHBoxLayout* findLineLayout;
		QGridLayout* layout15;
		QHBoxLayout* layout30;
		QGridLayout* layout16;
		QHBoxLayout* layout_size_min;
		QHBoxLayout* layout_size_max;
		QHBoxLayout* layout_find_in_archive;
		
		QString searchFilepath;
	
	protected slots:
		virtual void languageChange();
		int select ( Q3ListViewItem *i );
		int closee ( void );
		int cancele ( void );
		int seeke ( void );
		int saveState ( void );
		void dateStartChanged ( int );
		void dateEndChanged ( int );
		void dateStartDoubleClicked();
		void dateEndDoubleClicked();
		void sizeMinClicked();
		void sizeMaxClicked();
		void printResultClicked();
		void exportResultClicked();
		void clearSearchResultClicked();

};

#endif // FINDDIALOG_H
// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
