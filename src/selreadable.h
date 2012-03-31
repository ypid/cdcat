/****************************************************************************
                         Hyper's CD Catalog
         A multiplatform qt and xml based catalog program

      Author    : Peter Deak (hyperr@freemail.hu)
      License   : GPL
      Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef SELREADABLE_H
#define SELREADABLE_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QModelIndex>
#include <QStandardItemModel>

class Q3VBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class CdCatConfig;
class QComboBox;
class QRadioButton;
class QGroupBox;
class QListView;
class QPoint;
class QModelIndex;

class SelReadable : public QDialog {
		Q_OBJECT
	
	public:
		SelReadable ( CdCatConfig *confp, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
		~SelReadable();
		void checkExcludeRules();
		
		CdCatConfig *conf;
		QCheckBox* cbTag;
		QCheckBox* cbInfo;
		QFrame* line0;
		QCheckBox* cbaInfo;
		QFrame* line1;
		QCheckBox* cbCont;
		QCheckBox* cbFileInfo;
		QCheckBox* cbThumb;
		QCheckBox* cbUseExternalContentViewer;
		QCheckBox* cbDoExcludeFiles;
		QCheckBox* cbUseWildcardInsteadRegexForExclude;
		QSpinBox *thumbWidthSpinBox;
		QSpinBox *thumbHeightSpinBox;
		QSpinBox* maxSpinBox;
#ifdef USE_LIBEXIF
		QCheckBox *cbExif;
#endif
		QLineEdit *lineFiles;
		QLineEdit *thumbLineExts;
		QLineEdit *lineeditPathExternalContentViewer;
		QListView *listviewExcludeFiles;
		QStandardItemModel *exclude_model;
		
		QFrame* line6;
		QFrame* line7;
		QPushButton* buttonOK;
		QPushButton* buttonCancel;
		QPushButton* buttonUseExternalContentViewer;
		QPushButton* buttonAddExcludeRule;
		QPushButton *labExcludeRexexInfoButton;
		QComboBox *cbDefaultMp3TagVersion;
		
		QLabel* labelContentSize;
		QLabel* labelContentSizeUnit;
		QLabel* labelContent;
		QLabel *labArchiveExtensions;
		QLabel *labArchiveExtensionsStatusIcon;
		QLabel *labFileInfoExtensions;
		QLabel *labFileInfoExtensionsStatusIcon;
		QLabel *labThumb;
		QLabel *labThumbSize;
		QLabel *labThumbXSize;
		QLabel *labThumbExts;
		QLabel *labelExternalContentViewer;
		QLabel *labelDefaultMp3TagVersion;
		QLabel *labelExcludeFiles;
		QLabel *labExcludeRexexInfo;
		
		QGroupBox *groupBoxShowArchiveFileOpts;
		QCheckBox* cbScanArchive;
		QCheckBox* cbShowProgressedArchiveFileInStatus;
		QCheckBox* cbShowArchiveFilePerms;
		QCheckBox* cbShowArchiveFileUser;
		QCheckBox* cbShowArchiveFileGroup;
		QCheckBox* cbShowArchiveFileSize;
		QCheckBox* cbShowArchiveFileDate;
		QCheckBox* cbShowArchiveFileComment;
	
	public slots:
		int schanged ( int state );
		int sok ( void );
		int scan ( void );
		int selectExternalContentViewer( void );
		void selectExternalContentViewerString( const QString& );
		void addExcludeRulesClicked();
		void editExcludeRuleClicked();
		void deleteExcludeRuleClicked();
		void excludeContextMenuRequested(const QPoint &);
		void labExcludeRexexInfoButtonClicked();
		void excludeDataChanged ( QStandardItem * item );
	
	protected:
		Q3VBoxLayout* SelReadableLayout;
		QHBoxLayout* layoutInfo;
		QHBoxLayout* layout12;
		Q3VBoxLayout* layout11;
		QHBoxLayout* layout9;
		QHBoxLayout* layoutArchiveScan;
		QHBoxLayout* layoutButtons;
		QHBoxLayout* layout1;
		QHBoxLayout* layout62;
		QHBoxLayout* layoutShowArchiveFileOptions;
		QHBoxLayout* layoutShowProgressedArchiveFileInStatus;
		QHBoxLayout* layoutThumbGeneral;
		QHBoxLayout* layoutThumbExts;
		QHBoxLayout* layoutExternalContentViewer;
		QHBoxLayout* layoutFileInfo;
		QHBoxLayout* layoutExcludeMain;
		Q3VBoxLayout* layoutExcludeLeft;
		QHBoxLayout* layoutExcludeRegexInfo;
		QGridLayout* layoutShowArchiveFileOptionsGroup;

		QString SupportedExtensions;
		QString RegexInfoHtml;
		
		QModelIndex contextmenu_modelindex;

	protected slots:
		virtual void languageChange();
};

#endif // SELREADABLE_H
// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
