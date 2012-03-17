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
#include <Q3Frame>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>
#include <QGroupBox>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QCheckBox;
class Q3Frame;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class CdCatConfig;
class QComboBox;
class QRadioButton;
class QGroupBox;

class SelReadable : public QDialog {
		Q_OBJECT
	
	public:
		SelReadable ( CdCatConfig *confp, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
		~SelReadable();
		
		CdCatConfig *conf;
		QCheckBox* cbTag;
		QCheckBox* cbInfo;
		Q3Frame* line0;
		QCheckBox* cbaInfo;
		Q3Frame* line1;
		QCheckBox* cbCont;
		QCheckBox* cbFileInfo;
		QCheckBox* cbThumb;
		QCheckBox* cbUseExternalContentViewer;
		QSpinBox *thumbWidthSpinBox;
		QSpinBox *thumbHeightSpinBox;
		QSpinBox* maxSpinBox;
#ifdef USE_LIBEXIF
		QCheckBox *cbExif;
#endif
		QLineEdit *lineFiles;
		QLineEdit *thumbLineExts;
		QLineEdit *lineeditPathExternalContentViewer;
		
		Q3Frame* line6;
		Q3Frame* line7;
		QPushButton* buttonOK;
		QPushButton* buttonCancel;
		QPushButton* buttonUseExternalContentViewer;
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
		
		QGroupBox *groupBoxShowArchiveFileOpts;
		QCheckBox* cpScanArchive;
		QCheckBox* cpShowArchiveFilePerms;
		QCheckBox* cpShowArchiveFileUser;
		QCheckBox* cpShowArchiveFileGroup;
		QCheckBox* cpShowArchiveFileSize;
		QCheckBox* cpShowArchiveFileDate;
		QCheckBox* cpShowArchiveFileComment;
	
	public slots:
		int schanged ( int state );
		int sok ( void );
		int scan ( void );
		int selectExternalContentViewer( void );
		void selectExternalContentViewerString( const QString& );
	
	protected:
		Q3VBoxLayout* SelReadableLayout;
		Q3HBoxLayout* layout12;
		Q3VBoxLayout* layout11;
		Q3HBoxLayout* layout9;
		Q3HBoxLayout* layoutArchiveScan;
		Q3HBoxLayout* layoutButtons;
		Q3HBoxLayout* layout1;
		Q3HBoxLayout* layout62;
		Q3HBoxLayout* layoutShowArchiveFileOptions;
		Q3HBoxLayout* layoutThumbExts;
		Q3HBoxLayout* layoutExternalContentViewer;
		Q3HBoxLayout* layoutFileInfo;
		Q3GridLayout* layoutShowArchiveFileOptionsGroup;

		QString SupportedExtensions;

	protected slots:
		virtual void languageChange();
};

#endif // SELREADABLE_H
// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
