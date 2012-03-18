/****************************************************************************
                         Hyper's CD Catalog
         A multiplatform qt and xml based catalog program

      Author    : Peter Deak (hyperr@freemail.hu)
      License   : GPL
      Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "selreadable.h"

#include <QVariant>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLayout>
#include <QToolTip>
#include <QFileDialog>
#include <QFileInfo>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#ifdef USE_LIB7ZIP
#include <lib7zip.h>
#endif

#include "config.h"
#include "icons.h"
#include "dbase.h"
#include "cdcat.h"
#include "cdcatmediainfo.h"

SelReadable::SelReadable ( CdCatConfig *confp, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
	: QDialog ( parent, name, modal, fl )

{
	conf = confp;
	DEBUG_INFO_ENABLED = init_debug_info();
	if ( !name )
		setName ( "SelReadable" );
	setIcon ( *get_t_config_icon() );
	
	SelReadableLayout = new Q3VBoxLayout ( this, 15, 6, "SelReadableLayout" );
	setSizeGripEnabled ( TRUE );
	
	layoutArchiveScan= new Q3HBoxLayout ( 0, 0, 6, "layoutArchiveScan" );
	cpScanArchive = new QCheckBox ( this, "cpScanArchive" );
//     cpScanArchive->setMaximumWidth ( 80 );
	
	QSpacerItem* archiveInfospacer1 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	labArchiveExtensions = new QLabel ( this, "labArchiveExtensions" );
	labArchiveExtensionsStatusIcon = new QLabel ( this, "labArchiveExtensionsStatusIcon" );
	labArchiveExtensionsStatusIcon->setPixmap(*get_t_info_icon());
	QSpacerItem* archiveInfospacer2 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layoutArchiveScan->addWidget ( cpScanArchive );
	layoutArchiveScan->addItem ( archiveInfospacer1 );
	layoutArchiveScan->addWidget ( labArchiveExtensions );
	layoutArchiveScan->addWidget ( labArchiveExtensionsStatusIcon );
	layoutArchiveScan->addItem ( archiveInfospacer2 );
	
	SelReadableLayout->addLayout(layoutArchiveScan);
	
	layoutShowArchiveFileOptions = new Q3HBoxLayout ( 0, 0, 6, "layoutShowArchiveFileOptions" );
	layoutShowArchiveFileOptions->addSpacing ( 25 );
	groupBoxShowArchiveFileOpts = new QGroupBox ( this, "groupBoxShowArchiveFileOpts" );
	layoutShowArchiveFileOptions->addWidget ( groupBoxShowArchiveFileOpts );
	layoutShowArchiveFileOptionsGroup = new Q3GridLayout ( this, 1, 1, 5, 5, "layoutShowArchiveFileOptionsGroup" );
	groupBoxShowArchiveFileOpts->setLayout ( layoutShowArchiveFileOptionsGroup );
	cpShowArchiveFilePerms = new QCheckBox ( this, "cpShowArchiveFilePerms" );
	cpShowArchiveFileUser = new QCheckBox ( this, "cpShowArchiveFileUser" );
	cpShowArchiveFileGroup = new QCheckBox ( this, "cpShowArchiveFileGroup" );
	cpShowArchiveFileSize = new QCheckBox ( this, "cpShowArchiveFileSize" );
	cpShowArchiveFileDate = new QCheckBox ( this, "cpShowArchiveFileDate" );
	cpShowArchiveFileComment = new QCheckBox ( this, "cpShowArchiveFileComment" );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFilePerms, 0, 0 );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFileUser, 0, 1 );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFileGroup, 0, 2 );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFileSize, 1, 0 );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFileDate, 1, 1 );
	layoutShowArchiveFileOptionsGroup->addWidget ( cpShowArchiveFileComment, 1, 2 );
	
	SelReadableLayout->addLayout ( layoutShowArchiveFileOptions );
	
	line6 = new QFrame ( this, "line1" );
	line6->setFrameShape ( QFrame::HLine );
	line6->setFrameShadow ( QFrame::Sunken );
	line6->setFrameShape ( QFrame::HLine );
	SelReadableLayout->addWidget ( line6 );

	cbTag = new QCheckBox ( this, "cbTag" );
	cbDefaultMp3TagVersion = new QComboBox(this, "cbDefaultMp3TagVersion");
	cbDefaultMp3TagVersion->addItem(tr ( "version" ) + " 1");
	cbDefaultMp3TagVersion->addItem(tr ( "version" ) + " 2");
	labelDefaultMp3TagVersion = new QLabel(this, "labelDefaultMp3TagVersion");
	QSpacerItem* mp3tagspacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	
	layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );
	layout1->addSpacing ( 25 );
	layout1->addWidget ( cbTag );
	layout1->addItem(mp3tagspacer);
	layout1->addWidget ( labelDefaultMp3TagVersion );
	layout1->addWidget ( cbDefaultMp3TagVersion );
	layout1->addSpacing ( 25 );
	
	SelReadableLayout->addLayout ( layout1 );
	
	layoutInfo = new Q3HBoxLayout ( 0, 0, 6, "layoutInfo" );
	cbInfo = new QCheckBox ( this, "cbInfo" );
	cbaInfo = new QCheckBox ( this, "cbaInfo" );
	layoutInfo->addWidget ( cbaInfo );
	layoutInfo->addWidget ( cbInfo );
	SelReadableLayout->addLayout ( layoutInfo );

	line1 = new QFrame ( this, "line1" );
	line1->setFrameShape ( QFrame::HLine );
	line1->setFrameShadow ( QFrame::Sunken );
	line1->setFrameShape ( QFrame::HLine );
	SelReadableLayout->addWidget ( line1 );

	cbCont = new QCheckBox ( this, "cbCont" );
	SelReadableLayout->addWidget ( cbCont );

	layout12 = new Q3HBoxLayout ( 0, 0, 6, "layout12" );
	layout12->addSpacing ( 25 );
//     QSpacerItem* spacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//     layout12->addItem ( spacer );

	layout11 = new Q3VBoxLayout ( 0, 0, 6, "layout11" );
	
	layout9 = new Q3HBoxLayout ( 0, 0, 6, "layout9" );
	
	labelContent = new QLabel ( this, "labelContent" );
	lineFiles = new QLineEdit ( this, "lineFiles" );
	lineFiles->setMinimumWidth ( 150 );
	
	
	QSpacerItem* contentspacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout9->addWidget ( labelContent );
	layout9->addWidget ( lineFiles );
	layout9->addItem (contentspacer);
	
	maxSpinBox = new QSpinBox ( 1, MAX_STORED_SIZE / 1024, 1, this, "maxSpinBox" );
	labelContentSize = new QLabel ( this, "labelContentSize" );
	labelContentSizeUnit = new QLabel ( this, "labelContentSizeUnit" );
	labelContentSizeUnit->setText(tr("KiB"));
	QSpacerItem* contentSizeSpacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	
	layout9->addItem(contentSizeSpacer);
	layout9->addWidget ( labelContentSize );
	layout9->addWidget ( maxSpinBox );
	layout9->addWidget ( labelContentSizeUnit );
	
	layout11->addLayout ( layout9 );
	layout12->addLayout ( layout11 );
	SelReadableLayout->addLayout ( layout12 );
	
	layoutExternalContentViewer = new Q3HBoxLayout ( 0, 0, 7, "layoutExternalContentViewer" );
	cbUseExternalContentViewer = new QCheckBox ( this, "cbUseExternalContentViewer" );
// 	labelExternalContentViewer = new QLabel ( this, "labelExternalContentViewer" );
	lineeditPathExternalContentViewer = new QLineEdit ( this, "lineeditPathExternalContentViewer" );
	buttonUseExternalContentViewer = new QPushButton ( this, "buttonUseExternalContentViewer" );
	layoutExternalContentViewer->addSpacing ( 25 );
	layoutExternalContentViewer->addWidget( cbUseExternalContentViewer );
// 	layoutExternalContentViewer->addWidget( labelExternalContentViewer );
	layoutExternalContentViewer->addWidget( lineeditPathExternalContentViewer );
	layoutExternalContentViewer->addWidget( buttonUseExternalContentViewer );
	SelReadableLayout->addLayout ( layoutExternalContentViewer );
	
	cbFileInfo = new QCheckBox ( this, "cbFileInfo" );
	SelReadableLayout->addWidget ( cbFileInfo );
	
	layoutFileInfo = new Q3HBoxLayout ( 0, 0, 6, "layoutFileInfo" );
	labFileInfoExtensions = new QLabel ( this, "labFileInfoExtensions" );
	labFileInfoExtensionsStatusIcon = new QLabel ( this, "labFileInfoExtensionsStatusIcon" );
	labFileInfoExtensionsStatusIcon->setPixmap(*get_t_info_icon());
	QSpacerItem* fileInfospacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layoutFileInfo->addSpacing ( 25 );
	layoutFileInfo->addWidget ( labFileInfoExtensions );
	layoutFileInfo->addWidget ( labFileInfoExtensionsStatusIcon );
	layoutFileInfo->addItem ( fileInfospacer );
	SelReadableLayout->addLayout ( layoutFileInfo );
	
	cbThumb = new QCheckBox ( this, "cbThumb" );
	SelReadableLayout->addWidget ( cbThumb );
	
	layoutThumbExts = new Q3HBoxLayout ( 0, 0, 6, "layoutThumbExts" );
	labThumbExts = new QLabel ( this, "labThumbExts" );
	thumbLineExts = new QLineEdit ( this, "thumbLineExts" );
	thumbLineExts->setMinimumWidth ( 150 );
	QSpacerItem* thumpspacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	thumbWidthSpinBox = new QSpinBox ( 20, 400, 150, this, "thumbWidthSpinBox" );
	labThumbXSize = new QLabel ( this, "labThumbXSize" );
	thumbHeightSpinBox = new QSpinBox ( 20, 400, 150, this, "thumbHeightSpinBox" );
	labThumbSize = new QLabel ( this, "labThumbSize" );
	
	layoutThumbExts->addSpacing ( 25 );
	layoutThumbExts->addWidget ( labThumbExts );
	layoutThumbExts->addWidget ( thumbLineExts );
	layoutThumbExts->addItem(thumpspacer);
	layoutThumbExts->addWidget(labThumbSize);
	layoutThumbExts->addWidget(thumbWidthSpinBox);
	layoutThumbExts->addWidget(labThumbXSize);
	layoutThumbExts->addWidget(thumbHeightSpinBox);
	SelReadableLayout->addLayout ( layoutThumbExts );
	
#ifdef USE_LIBEXIF
	cbExif = new QCheckBox ( this, "cbExif" );
	SelReadableLayout->addWidget ( cbExif );
#endif
	
	layoutExcludeFiles = new Q3HBoxLayout ( 0, 0, 6, "layoutExcludeFiles" );
	cbDoExcludeFiles = new QCheckBox ( this, "cbDoExcludeFiles" );
	labelExcludeFiles = new QLabel ( this, "labelExcludeFiles" );
	lineExcludeFiles = new QLineEdit (this, "lineExcludeFiles");
	layoutExcludeFiles->addWidget ( cbDoExcludeFiles );
	layoutExcludeFiles->addWidget (labelExcludeFiles);
	layoutExcludeFiles->addWidget (lineExcludeFiles);
	
	SelReadableLayout->addLayout ( layoutExcludeFiles );
	
	layoutButtons = new Q3HBoxLayout ( 0, 0, 6, "layoutButtons" );
	QSpacerItem* spacer_2 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layoutButtons->addItem ( spacer_2 );
	
	buttonOK = new QPushButton ( this, "buttonOK" );
	buttonOK->setMinimumSize ( QSize ( 80, 0 ) );
	layoutButtons->addWidget ( buttonOK );
	
	buttonCancel = new QPushButton ( this, "buttonCancel" );
	buttonCancel->setMinimumSize ( QSize ( 80, 0 ) );
	layoutButtons->addWidget ( buttonCancel );
	QSpacerItem* spacer_3 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layoutButtons->addItem ( spacer_3 );
	SelReadableLayout->addLayout ( layoutButtons );
	languageChange();
	resize ( QSize ( 350, 216 ).expandedTo ( minimumSizeHint() ) );
	
	connect ( cbTag, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged ( int ) ) );
	connect ( cbCont, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged ( int ) ) );
	connect ( cbThumb, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged ( int ) ) );
	connect ( cpScanArchive, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged(int)) );
	connect ( cbUseExternalContentViewer, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged(int)) );
	connect ( cbDoExcludeFiles , SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged(int)) );
	connect ( buttonOK, SIGNAL ( clicked() ), this, SLOT ( sok() ) );
	connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( scan() ) );
	connect ( buttonUseExternalContentViewer, SIGNAL ( clicked() ), this, SLOT ( selectExternalContentViewer() ) );
	connect ( lineeditPathExternalContentViewer, SIGNAL ( textEdited ( const QString & ) ), this, SLOT ( selectExternalContentViewerString( const QString & ) ) );
	
	
	cpScanArchive->setChecked ( conf->doScanArchive );
	cpShowArchiveFilePerms->setChecked ( conf->show_archive_file_perms );
	cpShowArchiveFileUser->setChecked ( conf->show_archive_file_user );
	cpShowArchiveFileGroup->setChecked ( conf->show_archive_file_group );
	cpShowArchiveFileSize->setChecked ( conf->show_archive_file_size );
	cpShowArchiveFileDate->setChecked ( conf->show_archive_file_date );
	cpShowArchiveFileComment->setChecked ( conf->show_archive_file_comment );
	cbTag->setChecked ( conf->readtag );
	cbCont->setChecked ( conf->readcontent );
	cbUseExternalContentViewer->setChecked ( conf->useExternalContentViewer );
	lineeditPathExternalContentViewer->setText( conf->ExternalContentViewerPath );
	if(QFileInfo(conf->ExternalContentViewerPath).exists()) {
		lineeditPathExternalContentViewer->setPaletteBackgroundColor(QColor("#99FF66"));
		lineeditPathExternalContentViewer->setToolTip( tr("Path to external content viewer (found)"));
	}
	else {
		lineeditPathExternalContentViewer->setPaletteBackgroundColor(QColor("#FF9999"));
		lineeditPathExternalContentViewer->setToolTip( tr("Path to external content viewer (not found)"));
	}
	cbFileInfo->setChecked ( conf->usefileinfo );
#ifdef USE_LIBEXIF
	cbExif->setChecked ( conf->storeExifData );
#endif
	cbThumb->setChecked ( conf->storeThumb );
	thumbWidthSpinBox->setValue(conf->thumbWidth);
	thumbHeightSpinBox->setValue(conf->thumbHeight);
	lineFiles->setText ( conf->readcfiles );
	thumbLineExts->setText( conf->ThumbExtsList.join(";"));
	maxSpinBox->setValue ( ( int ) ( conf->readclimit / 1024 ) );
	cbInfo->setChecked ( conf->readinfo );
	cbaInfo->setChecked ( conf->readavii );
	if (conf->v1_over_v2)
		cbDefaultMp3TagVersion->setCurrentIndex(0);
	else
		cbDefaultMp3TagVersion->setCurrentIndex(1);
	
	if ( conf->doScanArchive ) {
#ifdef USE_LIB7ZIP
		C7ZipLibrary lib;
		WStringArray exts;
#endif
		SupportedExtensions = "";
		QStringList SupportedExtensionsList;
		// libtar
		
		SupportedExtensionsList.append ( "tar" );
		SupportedExtensionsList.append ( "tar.gz" );
		SupportedExtensionsList.append ( "tar.bz2" );
#ifdef USE_LIB7ZIP
		// lib7zip
		bool sevenzip_libfound = false;
		if ( lib.Initialize() ) {
			if ( *DEBUG_INFO_ENABLED )
				fprintf ( stderr, "lib7zip initialize succeeded, lib7zip scanning enabled\n" );
			if ( lib.GetSupportedExts ( exts ) ) {
				sevenzip_libfound = true;
				for ( WStringArray::const_iterator extIt = exts.begin(); extIt != exts.end(); extIt++ ) {
					SupportedExtensionsList.append ( QString().fromWCharArray ( ( *extIt ).c_str() ) );
				}
			}
		}
		else {
			fprintf ( stderr, "lib7zip initialize failed, lib7zip scanning disabled\n" );
		}
#else
	   if ( *DEBUG_INFO_ENABLED )
		std::cerr << "lib7zip library not supported" << std::end;
#endif
		SupportedExtensionsList.sort();
		int linelen = 0;
		for ( int i = 0; i < SupportedExtensionsList.size(); i++ ) {
			if ( linelen > 200 ) {
				linelen = 0;
				SupportedExtensions += "<br />&nbsp;&nbsp;";
				SupportedExtensions += SupportedExtensionsList.at ( i );
				linelen += SupportedExtensionsList.at ( i ).size();
			}
			else {
				if ( linelen != 0 )
					SupportedExtensions += " ";
				SupportedExtensions += SupportedExtensionsList.at ( i );
				linelen += SupportedExtensionsList.at ( i ).size() + 2;
			}
		}
		QString sevenzip_libfound_text = "";
#ifdef USE_LIB7ZIP
		sevenzip_libfound_text = "<font color=\"red\">" + tr ( "lib7zip found" ) + "</font>";
		if ( sevenzip_libfound )
			sevenzip_libfound_text = "<font color=\"green\">" + tr ( "lib7zip found" ) + "</font>";
		
#else
		sevenzip_libfound_text = "<font color=\"red\">" + tr ( "lib7zip not supported" ) + "</font>";
#endif
 		labArchiveExtensions->setText(  tr ( "Archive support:" )+ " "+sevenzip_libfound_text );
		labArchiveExtensionsStatusIcon->setToolTip ( tr ( "Supported extensions:" ) + "&nbsp;" + SupportedExtensions );
	}
	
	
	// FIXME: get from fileinfo
	CdcatMediaInfo me;
	
	bool fileinfo_libfound = me.getMediaInfoLibFound();
	QStringList SupportedFileInfoExtensionsList = me.getSupportedExtensions();
	
	SupportedFileInfoExtensionsList.sort();
	QString SupportedFileInfoExtensions = "";
	int linelen = 0;
	for ( int i = 0; i < SupportedFileInfoExtensionsList.size(); i++ ) {
		if ( linelen > 200 ) {
			linelen = 0;
			SupportedFileInfoExtensions += "<br />&nbsp;&nbsp;";
			SupportedFileInfoExtensions += SupportedFileInfoExtensionsList.at ( i );
			linelen += SupportedFileInfoExtensionsList.at ( i ).size();
		}
		else {
			if ( linelen != 0 )
				SupportedFileInfoExtensions += " ";
			SupportedFileInfoExtensions += SupportedFileInfoExtensionsList.at ( i );
			linelen += SupportedFileInfoExtensionsList.at ( i ).size() + 2;
		}
	}
	QString fileinfo_libfound_text = "<font color=\"red\">" + tr ( "mediainfo not found" ) + "</font>";
	if ( fileinfo_libfound )
		fileinfo_libfound_text = "<font color=\"green\">" + tr ( "mediainfo found" ) + ": " + me.getMediaInfoVersion() + "</font>";
	labFileInfoExtensions->setText ( tr ( "mediainfo status:" )+ " "+fileinfo_libfound_text);
	labFileInfoExtensionsStatusIcon->setToolTip(tr("Supported extensions:")+"&nbsp;" + SupportedFileInfoExtensions );
	
	cbDoExcludeFiles->setChecked(conf->doExcludeFiles);
	lineExcludeFiles->setText(conf->ExcludeFileList.join(";"));
	
	schanged ( 0 );
}

int SelReadable::schanged ( int ) {
	if ( cbCont->isChecked() ) {
		lineFiles->setEnabled ( true );
		labelContentSize->setEnabled ( true );
		labelContentSizeUnit->setEnabled ( true );
		maxSpinBox->setEnabled ( true );
		labelContent->setEnabled ( true );
		cbUseExternalContentViewer->setEnabled ( true );
		if(cbUseExternalContentViewer->isChecked()) {
			lineeditPathExternalContentViewer->setEnabled ( true );
			buttonUseExternalContentViewer->setEnabled ( true );
		}
		else {
			lineeditPathExternalContentViewer->setEnabled ( false );
			buttonUseExternalContentViewer->setEnabled ( false );
		}
	}
	else {
		lineFiles->setEnabled ( false );
		maxSpinBox->setEnabled ( false );
		labelContent->setEnabled ( false );
		labelContentSize->setEnabled ( false );
		labelContentSizeUnit->setEnabled ( false );
		cbUseExternalContentViewer->setEnabled ( false );
		lineeditPathExternalContentViewer->setEnabled ( false );
		buttonUseExternalContentViewer->setEnabled ( false );
	}

	if ( cbTag->isChecked() )
		cbDefaultMp3TagVersion->setEnabled ( true );
	else
		cbDefaultMp3TagVersion->setEnabled ( false );
	
	if ( cbThumb->isChecked() ) {
		labThumbExts->setEnabled ( true );
		labThumbXSize->setEnabled ( true );
		thumbLineExts->setEnabled ( true );
		thumbWidthSpinBox->setEnabled ( true );
		thumbHeightSpinBox->setEnabled ( true );
	}
	else {
		labThumbExts->setEnabled ( false );
		labThumbXSize->setEnabled ( false );
		thumbLineExts->setEnabled ( false );
		thumbWidthSpinBox->setEnabled ( false );
		thumbHeightSpinBox->setEnabled ( false );
	}
	if(cpScanArchive->isChecked()) {
		groupBoxShowArchiveFileOpts->setEnabled(true);
		labArchiveExtensions->setEnabled(true);
		labArchiveExtensionsStatusIcon->setEnabled(true);
	}
	else {
		groupBoxShowArchiveFileOpts->setEnabled(false);
		labArchiveExtensions->setEnabled(false);
		labArchiveExtensionsStatusIcon->setEnabled(false);
	}
	
	if(cbDoExcludeFiles->isChecked())
		lineExcludeFiles->setEnabled(true);
	else
		lineExcludeFiles->setEnabled(false);
	
	return 0;
}


int SelReadable::sok ( void ) {
	conf->doScanArchive  = cpScanArchive->isChecked();
	conf->show_archive_file_perms  = cpShowArchiveFilePerms->isChecked();
	conf->show_archive_file_user  = cpShowArchiveFileUser->isChecked();
	conf->show_archive_file_group  = cpShowArchiveFileGroup->isChecked();
	conf->show_archive_file_size  = cpShowArchiveFileSize->isChecked();
	conf->show_archive_file_date  = cpShowArchiveFileDate->isChecked();
	conf->show_archive_file_comment  = cpShowArchiveFileComment->isChecked();
	conf->readcontent = cbCont->isChecked();
	conf->usefileinfo = cbFileInfo->isChecked();
	conf->storeThumb = cbThumb->isChecked();
	conf->thumbWidth = thumbWidthSpinBox->value();
	conf->thumbHeight = thumbHeightSpinBox->value();
#ifdef USE_LIBEXIF
	conf->storeExifData = cbExif->isChecked();
#endif
	conf->readcfiles  = lineFiles->text();
	conf->useExternalContentViewer = cbUseExternalContentViewer->isChecked();
	conf->ExternalContentViewerPath = lineeditPathExternalContentViewer->text();
	conf->ThumbExtsList = thumbLineExts->text().split(";");
	conf->readclimit  = maxSpinBox->value() * 1024;
	conf->readinfo    = cbInfo->isChecked();
	conf->readavii    = cbaInfo->isChecked();
	conf->readtag     = cbTag->isChecked();
	if (cbDefaultMp3TagVersion->currentIndex() == 0)
		conf->v1_over_v2  = true;
	else
		conf->v1_over_v2  = false;
	conf->doExcludeFiles  = cbDoExcludeFiles->isChecked();
	conf->ExcludeFileList = lineExcludeFiles->text().split(";");
	
	close();
	return 0;
}

int SelReadable::scan ( void ) {
	close();
	return 0;
}

int SelReadable::selectExternalContentViewer(void ) {
	// get file.name
	QString s = QFileDialog::getOpenFileName( 0, tr ( "Choose path to external context viewer" ), conf->lastDir );
	selectExternalContentViewerString(s);
	if ( !s.isEmpty() )
		return 0;
	else
		return 1;
}

void SelReadable::selectExternalContentViewerString(const QString &s ) {
	if ( !s.isEmpty() ) {
		lineeditPathExternalContentViewer->setText ( s );
	}
	if(QFileInfo(lineeditPathExternalContentViewer->text()).exists()) {
			lineeditPathExternalContentViewer->setPaletteBackgroundColor(QColor("#99FF66"));
			lineeditPathExternalContentViewer->setToolTip( tr("Path to external content viewer (found)"));
			return;
	}
	
	lineeditPathExternalContentViewer->setPaletteBackgroundColor(QColor("#FF9999"));
	lineeditPathExternalContentViewer->setToolTip( tr("Path to external content viewer (not found)"));
	return;
}


/*
 *  Destroys the object and frees any allocated resources
 */
SelReadable::~SelReadable() {
	// no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelReadable::languageChange() {
	setCaption ( tr ( "Select readable items" ) );
	groupBoxShowArchiveFileOpts->setTitle ( tr ( "Archive file display options" ) );
	cpScanArchive->setText ( tr ( "Scan for archive file list" ) );
	labArchiveExtensions->setText ( tr ( "Supported extensions:" ) + " " + SupportedExtensions );
	cpShowArchiveFilePerms->setText ( tr ( "Permission" ) );
	cpShowArchiveFileUser->setText ( tr ( "User" ) );
	cpShowArchiveFileGroup->setText ( tr ( "Group" ) );
	cpShowArchiveFileSize->setText ( tr ( "Size" ) );
	cpShowArchiveFileDate->setText ( tr ( "Date" ) );
	cpShowArchiveFileComment->setText ( tr ( "Comment" ) );
	cbTag->setText ( tr ( "Read mp3 tags" ) );
	cbThumb->setText( tr("Read thumbnails") );
	cbThumb->setToolTip( tr("Read thumbnails from pictures") );
	labThumbSize->setText( tr("Size:") );
	thumbWidthSpinBox->setToolTip( tr("Thumbnail size (width) in pixels") );
	thumbHeightSpinBox->setToolTip( tr("Thumbnail size (height) in pixels") );
	labThumbXSize->setText ( tr("x") );
#ifdef USE_LIBEXIF
	cbExif->setText( tr("Read EXIF data from pictures") );
#endif
	
	labelDefaultMp3TagVersion->setText ( tr ( "Default tag" ) );
	cbInfo->setText ( tr ( "Read mp3 info as comment" ) );
	cbInfo->setToolTip( tr ( "Read mp3 technical info as comment (bitrate,freq,length...)" ) );
	cbaInfo->setText ( tr ( "Read avi info as comment" ) );
	cbaInfo->setToolTip ( tr ( "Read avi technical info as comment (codecs,length,...)" ) );
	cbCont->setText ( tr ( "Store content of some files" ) );
	cbUseExternalContentViewer->setText( tr("Use external file content viewer") );
	buttonUseExternalContentViewer->setText( tr("...") );
	buttonUseExternalContentViewer->setToolTip( tr("Select external viewer...") );
	cbFileInfo->setText ( tr ( "Read some technical info using mediainfo" ) );
	labelContent->setText ( tr ( "file patterns:" ) );
	lineFiles->setToolTip ( tr ( "; separated list of readable file patterns" ) );
	labThumbExts->setText ( tr ( "File extensions:" ) );
	thumbLineExts->setToolTip ( tr ( "; separated list of image file extensions, e.g. png;jpg;gif" ) );
	labelContentSize->setText ( tr ( "max size:" ) );
	maxSpinBox->setToolTip( tr ( "content size limit in kByte" ) );
	cbDoExcludeFiles->setText(tr("exclude files"));
	lineExcludeFiles->setToolTip(tr("; separated list of skip file patterns (regular expression)"));
	buttonOK->setText ( tr ( "Ok" ) );
	buttonCancel->setText ( tr ( "Cancel" ) );
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
