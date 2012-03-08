/****************************************************************************
                         Hyper's CD Catalog
         A multiplatform qt and xml based catalog program

      Author    : Peter Deak (hyperr@freemail.hu)
      License   : GPL
      Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "selreadable.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
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
	
	SelReadableLayout = new Q3VBoxLayout ( this, 11, 6, "SelReadableLayout" );
	setSizeGripEnabled ( TRUE );
	
	cpScanArchive = new QCheckBox ( this, "cpScanArchive" );
//     cpScanArchive->setMaximumWidth ( 80 );
	SelReadableLayout->addWidget ( cpScanArchive );
	
	layout62 = new Q3HBoxLayout ( 0, 0, 6, "layout62" );
	labArchiveExtensions = new QLabel ( this, "labArchiveExtensions" );
	layout62->addWidget ( labArchiveExtensions );
	SelReadableLayout->addLayout ( layout62 );
	
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
	
	line6 = new Q3Frame ( this, "line1" );
	line6->setFrameShape ( Q3Frame::HLine );
	line6->setFrameShadow ( Q3Frame::Sunken );
	line6->setFrameShape ( Q3Frame::HLine );
	SelReadableLayout->addWidget ( line6 );

	cbTag = new QCheckBox ( this, "cbTag" );
	SelReadableLayout->addWidget ( cbTag );

	tagselector = new Q3ButtonGroup ( 2, Qt::Horizontal, this, "TagSelectorG" );
	tagselector->setRadioButtonExclusive ( true );
	rad_v1 = new QRadioButton ( tagselector );
	rad_v2 = new QRadioButton ( tagselector );

	layout1 = new Q3HBoxLayout ( 0, 0, 6, "layout1" );
	layout1->addSpacing ( 25 );
	layout1->addWidget ( tagselector );
	layout1->addSpacing ( 25 );

	SelReadableLayout->addLayout ( layout1 );

	cbInfo = new QCheckBox ( this, "cbInfo" );
	SelReadableLayout->addWidget ( cbInfo );

	line0 = new Q3Frame ( this, "line0" );
	line0->setFrameShape ( Q3Frame::HLine );
	line0->setFrameShadow ( Q3Frame::Sunken );
	line0->setFrameShape ( Q3Frame::HLine );
	SelReadableLayout->addWidget ( line0 );

	cbaInfo = new QCheckBox ( this, "cbaInfo" );
	SelReadableLayout->addWidget ( cbaInfo );

	line1 = new Q3Frame ( this, "line1" );
	line1->setFrameShape ( Q3Frame::HLine );
	line1->setFrameShadow ( Q3Frame::Sunken );
	line1->setFrameShape ( Q3Frame::HLine );
	SelReadableLayout->addWidget ( line1 );

	cbCont = new QCheckBox ( this, "cbCont" );
	SelReadableLayout->addWidget ( cbCont );

	layout12 = new Q3HBoxLayout ( 0, 0, 6, "layout12" );
	layout12->addSpacing ( 25 );
//     QSpacerItem* spacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//     layout12->addItem ( spacer );

	layout11 = new Q3VBoxLayout ( 0, 0, 6, "layout11" );

	layout9 = new Q3HBoxLayout ( 0, 0, 6, "layout9" );

	lineFiles = new QLineEdit ( this, "lineFiles" );
	lineFiles->setMinimumWidth ( 150 );
	layout9->addWidget ( lineFiles );

	textLabel1 = new QLabel ( this, "textLabel1" );
	layout9->addWidget ( textLabel1 );
	layout11->addLayout ( layout9 );

	layout10 = new Q3HBoxLayout ( 0, 0, 6, "layout10" );

	maxSpinBox = new QSpinBox ( 1, MAX_STORED_SIZE / 1024, 1, this, "maxSpinBox" );
	layout10->addWidget ( maxSpinBox );

	textLabel2 = new QLabel ( this, "textLabel2" );
	layout10->addWidget ( textLabel2 );
	layout11->addLayout ( layout10 );
	layout12->addLayout ( layout11 );
	SelReadableLayout->addLayout ( layout12 );

	cbFileInfo = new QCheckBox ( this, "cbFileInfo" );
	SelReadableLayout->addWidget ( cbFileInfo );
	labFileInfoExtensions = new QLabel ( this, "labFileInfoExtensions" );
	SelReadableLayout->addWidget ( labFileInfoExtensions );
	
	cbThumb = new QCheckBox ( this, "cbThumb" );
	SelReadableLayout->addWidget ( cbThumb );
	labThumb = new QLabel ( this, "labThumb" );
	SelReadableLayout->addWidget ( labThumb );
	
	layoutThumbSize = new Q3HBoxLayout ( 0, 0, 6, "layoutThumbSize" );
	thumbWidthSpinBox = new QSpinBox ( 20, 400, 150, this, "thumbWidthSpinBox" );
	thumbHeightSpinBox = new QSpinBox ( 20, 400, 150, this, "thumbHeightSpinBox" );
	labThumbSize = new QLabel ( this, "labThumbSize" );
	labThumbXSize = new QLabel ( this, "labThumbXSize" );
	layoutThumbSize->addWidget(labThumbSize);
	layoutThumbSize->addWidget(thumbWidthSpinBox);
	layoutThumbSize->addWidget(labThumbXSize);
	layoutThumbSize->addWidget(thumbHeightSpinBox);
	SelReadableLayout->addLayout ( layoutThumbSize );
	
#ifdef USE_LIBEXIF
	cbExif = new QCheckBox ( this, "cbExif" );
	SelReadableLayout->addWidget ( cbExif );
	labExif = new QLabel ( this, "labExif" );
	SelReadableLayout->addWidget ( labExif );
#endif
	
	line2 = new Q3Frame ( this, "line2" );
	line2->setFrameShape ( Q3Frame::HLine );
	line2->setFrameShadow ( Q3Frame::Sunken );
	line2->setFrameShape ( Q3Frame::HLine );
	SelReadableLayout->addWidget ( line2 );

	layout3 = new Q3HBoxLayout ( 0, 0, 6, "layout3" );
	QSpacerItem* spacer_2 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout3->addItem ( spacer_2 );

	buttonOK = new QPushButton ( this, "buttonOK" );
	buttonOK->setMinimumSize ( QSize ( 80, 0 ) );
	layout3->addWidget ( buttonOK );

	buttonCancel = new QPushButton ( this, "buttonCancel" );
	buttonCancel->setMinimumSize ( QSize ( 80, 0 ) );
	layout3->addWidget ( buttonCancel );
	QSpacerItem* spacer_3 = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout3->addItem ( spacer_3 );
	SelReadableLayout->addLayout ( layout3 );
	languageChange();
	resize ( QSize ( 410, 216 ).expandedTo ( minimumSizeHint() ) );

	connect ( cbTag, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged ( int ) ) );
	connect ( cbCont, SIGNAL ( stateChanged ( int ) ), this, SLOT ( schanged ( int ) ) );
	connect ( buttonOK, SIGNAL ( clicked() ), this, SLOT ( sok() ) );
	connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( scan() ) );

	cpScanArchive->setChecked ( conf->doScanArchive );
	cpShowArchiveFilePerms->setChecked ( conf->show_archive_file_perms );
	cpShowArchiveFileUser->setChecked ( conf->show_archive_file_user );
	cpShowArchiveFileGroup->setChecked ( conf->show_archive_file_group );
	cpShowArchiveFileSize->setChecked ( conf->show_archive_file_size );
	cpShowArchiveFileDate->setChecked ( conf->show_archive_file_date );
	cpShowArchiveFileComment->setChecked ( conf->show_archive_file_comment );
	cbTag->setChecked ( conf->readtag );
	cbCont->setChecked ( conf->readcontent );
	cbFileInfo->setChecked ( conf->usefileinfo );
#ifdef USE_LIBEXIF
	cbExif->setChecked ( conf->storeExifData );
#endif
	cbThumb->setChecked ( conf->storeThumb );
	thumbWidthSpinBox->setValue(conf->thumbWidth);
	thumbHeightSpinBox->setValue(conf->thumbHeight);
	lineFiles->setText ( conf->readcfiles );
	maxSpinBox->setValue ( ( int ) ( conf->readclimit / 1024 ) );
	cbInfo->setChecked ( conf->readinfo );
	cbaInfo->setChecked ( conf->readavii );
	rad_v1->setChecked ( conf->v1_over_v2 );
	rad_v2->setChecked ( !conf->v1_over_v2 );

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
			if ( linelen > 120 ) {
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
		sevenzip_libfound_text = "<font color=\"red\">" + tr ( "lib7zip library not found" ) + "</font>";
		if ( sevenzip_libfound )
			sevenzip_libfound_text = "<font color=\"green\">" + tr ( "lib7zip library found" ) + "</font>";

#else
		sevenzip_libfound_text = "<font color=\"red\">" + tr ( "lib7zip library not supported" ) + "</font>";
#endif
		labArchiveExtensions->setText ( tr ( "Supported extensions" ) + " (" + sevenzip_libfound_text + ")" + ":<br />&nbsp;&nbsp;" + SupportedExtensions );
	}


	// FIXME: get from fileinfo
	CdcatMediaInfo me;

	bool fileinfo_libfound = me.getMediaInfoLibFound();
	QStringList SupportedFileInfoExtensionsList = me.getSupportedExtensions();

	SupportedFileInfoExtensionsList.sort();
	QString SupportedFileInfoExtensions = "";
	int linelen = 0;
	for ( int i = 0; i < SupportedFileInfoExtensionsList.size(); i++ ) {
		if ( linelen > 120 ) {
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
	QString fileinfo_libfound_text = "<font color=\"red\">" + tr ( "mediainfo library not found" ) + "</font>";
	if ( fileinfo_libfound )
		fileinfo_libfound_text = "<font color=\"green\">" + tr ( "mediainfo library found" ) + ": " + me.getMediaInfoVersion() + "</font>";
	labFileInfoExtensions->setText ( tr ( "Supported extensions" ) + " (" + fileinfo_libfound_text + ")" + ":<br />&nbsp;&nbsp;" + SupportedFileInfoExtensions );

	schanged ( 0 );
}

int SelReadable::schanged ( int ) {
	if ( cbCont->isChecked() ) {
		lineFiles->setEnabled ( true );
		maxSpinBox->setEnabled ( true );
	}
	else {
		lineFiles->setEnabled ( false );
		maxSpinBox->setEnabled ( false );
	}

	if ( cbTag->isChecked() )
		tagselector->setEnabled ( true );
	else
		tagselector->setEnabled ( false );

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
	conf->readclimit  = maxSpinBox->value() * 1024;
	conf->readinfo    = cbInfo->isChecked();
	conf->readavii    = cbaInfo->isChecked();
	conf->readtag     = cbTag->isChecked();
	conf->v1_over_v2  = rad_v1->isChecked();

	close();
	return 0;
}

int SelReadable::scan ( void ) {
	close();
	return 0;
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
	cpShowArchiveFilePerms->setText ( tr ( "Show permission" ) );
	cpShowArchiveFileUser->setText ( tr ( "Show user" ) );
	cpShowArchiveFileGroup->setText ( tr ( "Show group" ) );
	cpShowArchiveFileSize->setText ( tr ( "Show size" ) );
	cpShowArchiveFileDate->setText ( tr ( "Show date" ) );
	cpShowArchiveFileComment->setText ( tr ( "Show optional comment" ) );
	cbTag->setText ( tr ( "Read mp3 tags" ) );
	cbThumb->setText( tr("Read thumbnails from pictures") );
	labThumbSize->setText( tr("Thumbnail size:") );
	labThumbXSize->setText ( tr("x") );
#ifdef USE_LIBEXIF
	cbExif->setText( tr("Read EXIF data from pictures") );
#endif
	
	tagselector->setTitle ( tr ( "Default tag" ) );
	rad_v1->setText ( tr ( "version" ) + " 1" );
	rad_v2->setText ( tr ( "version" ) + " 2" );
	
	cbInfo->setText ( tr ( "Read mp3 technical info as comment (bitrate,freq,length...)" ) );
	cbaInfo->setText ( tr ( "Read avi technical info as comment (codecs,length,...)" ) );
	cbCont->setText ( tr ( "Store content of some files" ) );
	cbFileInfo->setText ( tr ( "Read some technical info using mediainfo" ) );
	lineFiles->setText ( "*.nfo;*.dzi" );
	textLabel1->setText ( tr ( "; separated list of readable file patterns" ) );
	textLabel2->setText ( tr ( "content size limit in kByte" ) );
	buttonOK->setText ( tr ( "Ok" ) );
	buttonCancel->setText ( tr ( "Cancel" ) );
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
