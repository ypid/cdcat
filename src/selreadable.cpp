/****************************************************************************
*                        Hyper's CD Catalog
*        A multiplatform qt and xml based catalog program
*
*     Author    : Peter Deak (hyperr@freemail.hu)
*     License   : GPL
*     Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include "selreadable.h"

#include <QDebug>
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
#include <QListView>
#include <QStringListModel>
#include <QStandardItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImageReader>

#ifdef USE_LIB7ZIP
#include <lib7zip.h>
#endif

#include <iostream>

#include "config.h"
#include "icons.h"
#include "dbase.h"
#include "cdcat.h"

#ifndef NO_MEDIAINFO
#include "cdcatmediainfo.h"
#endif


SelReadable::SelReadable ( CdCatConfig *confp, QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {
    conf = confp;
    DEBUG_INFO_ENABLED = init_debug_info();
    if (!name) {
        setObjectName( "SelReadable" );
    }
    setModal( modal );
    setWindowIcon( *get_t_config_icon());

    SelReadableLayout = new QVBoxLayout( this );
    setSizeGripEnabled( true );

    layoutArchiveScan = new QHBoxLayout( this );
    cbScanArchive = new QCheckBox( this );
//     cbScanArchive->setMaximumWidth ( 80 );

    QSpacerItem *archiveInfospacer1 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    labArchiveExtensions = new QLabel( this );
    labArchiveExtensionsStatusIcon = new QLabel( this );
    labArchiveExtensionsStatusIcon->setPixmap( *get_t_info_icon());
    QSpacerItem *archiveInfospacer2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layoutArchiveScan->addWidget( cbScanArchive );
    layoutArchiveScan->addItem( archiveInfospacer1 );
    layoutArchiveScan->addWidget( labArchiveExtensions );
    layoutArchiveScan->addWidget( labArchiveExtensionsStatusIcon );
    layoutArchiveScan->addItem( archiveInfospacer2 );

    SelReadableLayout->addLayout( layoutArchiveScan );



    layoutShowArchiveFileOptions = new QHBoxLayout( this );
    layoutShowArchiveFileOptions->addSpacing( 25 );
    groupBoxShowArchiveFileOpts = new QGroupBox( this );
    layoutShowArchiveFileOptions->addWidget( groupBoxShowArchiveFileOpts );
    layoutShowArchiveFileOptionsGroup = new QGridLayout( this );
    groupBoxShowArchiveFileOpts->setLayout( layoutShowArchiveFileOptionsGroup );
    cbShowArchiveFilePerms = new QCheckBox( this );
    cbShowArchiveFileUser = new QCheckBox( this );
    cbShowArchiveFileGroup = new QCheckBox( this );
    cbShowArchiveFileSize = new QCheckBox( this );
    cbShowArchiveFileDate = new QCheckBox( this );
    cbShowArchiveFileComment = new QCheckBox( this );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFilePerms, 0, 0 );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFileUser, 0, 1 );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFileGroup, 0, 2 );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFileSize, 1, 0 );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFileDate, 1, 1 );
    layoutShowArchiveFileOptionsGroup->addWidget( cbShowArchiveFileComment, 1, 2 );

    SelReadableLayout->addLayout( layoutShowArchiveFileOptions );
    layoutShowProgressedArchiveFileInStatus = new QHBoxLayout( this );
    layoutShowArchiveFileOptions->addSpacing( 25 );
    cbShowProgressedArchiveFileInStatus = new QCheckBox( this );
    layoutShowArchiveFileOptions->addWidget( cbShowProgressedArchiveFileInStatus );
    SelReadableLayout->addLayout( layoutShowProgressedArchiveFileInStatus );

    line6 = new QFrame( this );
    line6->setFrameShape( QFrame::HLine );
    line6->setFrameShadow( QFrame::Sunken );
    line6->setFrameShape( QFrame::HLine );
    SelReadableLayout->addWidget( line6 );

    cbTag = new QCheckBox( this );
    cbDefaultMp3TagVersion = new QComboBox( this );
    cbDefaultMp3TagVersion->addItem( tr( "version" ) + " 1" );
    cbDefaultMp3TagVersion->addItem( tr( "version" ) + " 2" );
    labelDefaultMp3TagVersion = new QLabel( this );
    QSpacerItem *mp3tagspacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    layout1 = new QHBoxLayout( this );
    layout1->addSpacing( 25 );
    layout1->addWidget( cbTag );
    layout1->addItem( mp3tagspacer );
    layout1->addWidget( labelDefaultMp3TagVersion );
    layout1->addWidget( cbDefaultMp3TagVersion );
    layout1->addSpacing( 25 );

    SelReadableLayout->addLayout( layout1 );

    layoutInfo = new QHBoxLayout( this );
    cbInfo = new QCheckBox( this );
    cbaInfo = new QCheckBox( this );
    layoutInfo->addWidget( cbaInfo );
    layoutInfo->addWidget( cbInfo );
    SelReadableLayout->addLayout( layoutInfo );

    line1 = new QFrame( this );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );
    SelReadableLayout->addWidget( line1 );

    cbCont = new QCheckBox( this );
    SelReadableLayout->addWidget( cbCont );

    layout12 = new QHBoxLayout( this );
    layout12->addSpacing( 25 );
//     QSpacerItem* spacer = new QSpacerItem ( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//     layout12->addItem ( spacer );

    layout11 = new QVBoxLayout( this );

    layout9 = new QHBoxLayout( this );

    labelContent = new QLabel( this );
    lineFiles = new QLineEdit( this );
    lineFiles->setMinimumWidth( 150 );


    QSpacerItem *contentspacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addWidget( labelContent );
    layout9->addWidget( lineFiles );
    layout9->addItem( contentspacer );

    maxSpinBox = new QSpinBox( this );
    maxSpinBox->setMinimum( 1 );
    maxSpinBox->setMaximum( MAX_STORED_SIZE / 1024 );
    maxSpinBox->setSingleStep( 1 );
    labelContentSize = new QLabel( this );
    labelContentSizeUnit = new QLabel( this );
    labelContentSizeUnit->setText( tr( "KiB" ));
    QSpacerItem *contentSizeSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    layout9->addItem( contentSizeSpacer );
    layout9->addWidget( labelContentSize );
    layout9->addWidget( maxSpinBox );
    layout9->addWidget( labelContentSizeUnit );

    layout11->addLayout( layout9 );
    layout12->addLayout( layout11 );
    SelReadableLayout->addLayout( layout12 );

    layoutExternalContentViewer = new QHBoxLayout( this );
    cbUseExternalContentViewer = new QCheckBox( this );
//      labelExternalContentViewer = new QLabel ( this );
    lineeditPathExternalContentViewer = new QLineEdit( this );
    buttonUseExternalContentViewer = new QPushButton( this );
    layoutExternalContentViewer->addSpacing( 25 );
    layoutExternalContentViewer->addWidget( cbUseExternalContentViewer );
//      layoutExternalContentViewer->addWidget( labelExternalContentViewer );
    layoutExternalContentViewer->addWidget( lineeditPathExternalContentViewer );
    layoutExternalContentViewer->addWidget( buttonUseExternalContentViewer );
    SelReadableLayout->addLayout( layoutExternalContentViewer );

    cbFileInfo = new QCheckBox( this );
    SelReadableLayout->addWidget( cbFileInfo );

    layoutFileInfo = new QHBoxLayout( this );
    labFileInfoExtensions = new QLabel( this );
    labFileInfoExtensionsStatusIcon = new QLabel( this );
    labFileInfoExtensionsStatusIcon->setPixmap( *get_t_info_icon());
    QSpacerItem *fileInfospacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layoutFileInfo->addSpacing( 25 );
    layoutFileInfo->addWidget( labFileInfoExtensions );
    layoutFileInfo->addWidget( labFileInfoExtensionsStatusIcon );
    layoutFileInfo->addItem( fileInfospacer );
    SelReadableLayout->addLayout( layoutFileInfo );

    layoutThumbGeneral = new QHBoxLayout( this );
    cbThumb = new QCheckBox( this );
    layoutThumbGeneral->addWidget( cbThumb );
#ifdef USE_LIBEXIF
    cbExif = new QCheckBox( this );
    layoutThumbGeneral->addWidget( cbExif );
#endif
    SelReadableLayout->addLayout( layoutThumbGeneral );

    layoutThumbExts = new QHBoxLayout( this );
    labThumbExts = new QLabel( this );
    thumbLineExts = new QLineEdit( this );
    thumbLineExts->setMinimumWidth( 150 );
    labThumbExtsStatusIcon = new QLabel( this );
    labThumbExtsStatusIcon->setPixmap( *get_t_info_icon());
    QSpacerItem *thumpspacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    thumbWidthSpinBox = new QSpinBox( this );
    thumbWidthSpinBox->setMinimum( 20 );
    thumbWidthSpinBox->setMaximum( 400 );
    thumbWidthSpinBox->setSingleStep( 150 );

    labThumbXSize = new QLabel( this );
    thumbHeightSpinBox = new QSpinBox( this );
    thumbHeightSpinBox->setMinimum( 20 );
    thumbHeightSpinBox->setMaximum( 400 );
    thumbHeightSpinBox->setSingleStep( 150 );
    labThumbSize = new QLabel( this );

    layoutThumbExts->addSpacing( 25 );
    layoutThumbExts->addWidget( labThumbExts );
    layoutThumbExts->addWidget( thumbLineExts );
    layoutThumbExts->addWidget( labThumbExtsStatusIcon );
    layoutThumbExts->addItem( thumpspacer );
    layoutThumbExts->addWidget( labThumbSize );
    layoutThumbExts->addWidget( thumbWidthSpinBox );
    layoutThumbExts->addWidget( labThumbXSize );
    layoutThumbExts->addWidget( thumbHeightSpinBox );
    SelReadableLayout->addLayout( layoutThumbExts );

    layoutExcludeMain = new QHBoxLayout( this );
    layoutExcludeLeft = new QVBoxLayout( this );
    cbDoExcludeFiles = new QCheckBox( this );
    cbUseWildcardInsteadRegexForExclude = new QCheckBox( this );
    labelExcludeFiles = new QLabel( this );
    labExcludeRexexInfo = new QLabel( this );
    labExcludeRexexInfoButton = new QPushButton( this );
    connect( labExcludeRexexInfoButton, SIGNAL( clicked()), this, SLOT( labExcludeRexexInfoButtonClicked()));
    QSpacerItem *excludeRegexspacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    buttonAddExcludeRule = new QPushButton( this );
    listviewExcludeFiles = new QListView( this );
    layoutExcludeLeft->addWidget( cbDoExcludeFiles );
    layoutExcludeLeft->addWidget( cbUseWildcardInsteadRegexForExclude );

    layoutExcludeRegexInfo = new QHBoxLayout( this );
    layoutExcludeRegexInfo->addWidget( labExcludeRexexInfo );
    layoutExcludeRegexInfo->addWidget( labExcludeRexexInfoButton );

    layoutExcludeRegexInfo->addItem( excludeRegexspacer );
    layoutExcludeLeft->addLayout( layoutExcludeRegexInfo );

    layoutExcludeLeft->addWidget( buttonAddExcludeRule );
    layoutExcludeMain->addLayout( layoutExcludeLeft );
    layoutExcludeMain->addWidget( listviewExcludeFiles );

    SelReadableLayout->addLayout( layoutExcludeMain );

    layoutButtons = new QHBoxLayout( this );
    QSpacerItem *spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layoutButtons->addItem( spacer_2 );

    buttonOK = new QPushButton( this );
    buttonOK->setMinimumSize( QSize( 80, 0 ));
    layoutButtons->addWidget( buttonOK );

    buttonCancel = new QPushButton( this );
    buttonCancel->setMinimumSize( QSize( 80, 0 ));
    layoutButtons->addWidget( buttonCancel );
    QSpacerItem *spacer_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layoutButtons->addItem( spacer_3 );
    SelReadableLayout->addLayout( layoutButtons );

    resize( QSize( 350, 216 ).expandedTo( minimumSizeHint()));

    connect( cbTag, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( cbCont, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( cbThumb, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( cbScanArchive, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( cbUseExternalContentViewer, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( cbDoExcludeFiles, SIGNAL( stateChanged( int )), this, SLOT( schanged( int )));
    connect( buttonOK, SIGNAL( clicked()), this, SLOT( sok()));
    connect( buttonCancel, SIGNAL( clicked()), this, SLOT( scan()));
    connect( buttonUseExternalContentViewer, SIGNAL( clicked()), this, SLOT( selectExternalContentViewer()));
    connect( lineeditPathExternalContentViewer, SIGNAL( textEdited( const QString & )), this, SLOT( selectExternalContentViewerString( const QString & )));
    connect( buttonAddExcludeRule, SIGNAL( clicked()), this, SLOT( addExcludeRulesClicked()));

    listviewExcludeFiles->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( listviewExcludeFiles, SIGNAL( customContextMenuRequested( const QPoint & )), this, SLOT( excludeContextMenuRequested( const QPoint & )));

    cbScanArchive->setChecked( conf->doScanArchive );
    cbShowProgressedArchiveFileInStatus->setChecked( conf->showProgressedArchiveFileInStatus );
    if (!conf->showProgressedFileInStatus) {
        cbShowProgressedArchiveFileInStatus->setEnabled( false );
    }
    cbShowArchiveFilePerms->setChecked( conf->show_archive_file_perms );
    cbShowArchiveFileUser->setChecked( conf->show_archive_file_user );
    cbShowArchiveFileGroup->setChecked( conf->show_archive_file_group );
    cbShowArchiveFileSize->setChecked( conf->show_archive_file_size );
    cbShowArchiveFileDate->setChecked( conf->show_archive_file_date );
    cbShowArchiveFileComment->setChecked( conf->show_archive_file_comment );
    cbTag->setChecked( conf->readtag );
    cbCont->setChecked( conf->readcontent );
    cbUseExternalContentViewer->setChecked( conf->useExternalContentViewer );
    lineeditPathExternalContentViewer->setText( conf->ExternalContentViewerPath );
    if (QFileInfo( conf->ExternalContentViewerPath ).exists()) {
        QPalette palette;
        palette.setColor( lineeditPathExternalContentViewer->backgroundRole(), QColor( "#99FF66" ));
        lineeditPathExternalContentViewer->setPalette( palette );
        lineeditPathExternalContentViewer->setToolTip( tr( "Path to external content viewer (found)" ));
    } else {
        QPalette palette;
        palette.setColor( lineeditPathExternalContentViewer->backgroundRole(), QColor( "#FF9999" ));
        lineeditPathExternalContentViewer->setPalette( palette );
        lineeditPathExternalContentViewer->setToolTip( tr( "Path to external content viewer (not found)" ));
    }
    cbFileInfo->setChecked( conf->usefileinfo );
#ifdef USE_LIBEXIF
    cbExif->setChecked( conf->storeExifData );
#endif
    cbThumb->setChecked( conf->storeThumb );
    thumbWidthSpinBox->setValue( conf->thumbWidth );
    thumbHeightSpinBox->setValue( conf->thumbHeight );
    lineFiles->setText( conf->readcfiles );
    thumbLineExts->setText( conf->ThumbExtsList.join( ";" ));
    maxSpinBox->setValue((int)(conf->readclimit / 1024));
    cbInfo->setChecked( conf->readinfo );
    cbaInfo->setChecked( conf->readavii );
    if (conf->v1_over_v2) {
        cbDefaultMp3TagVersion->setCurrentIndex( 0 );
    } else {
        cbDefaultMp3TagVersion->setCurrentIndex( 1 );
    }

    if (conf->doScanArchive) {
#ifdef USE_LIB7ZIP
        C7ZipLibrary lib;
        WStringArray exts;
#endif
        SupportedExtensions = "";
        QStringList SupportedExtensionsList;
        // libtar

        SupportedExtensionsList.append( "tar" );
        SupportedExtensionsList.append( "tar.gz" );
        SupportedExtensionsList.append( "tar.bz2" );
#ifdef USE_LIB7ZIP
        // lib7zip
        bool sevenzip_libfound = false;
        if (lib.Initialize()) {
            if (*DEBUG_INFO_ENABLED) {
                fprintf( stderr, "lib7zip initialize succeeded, lib7zip scanning enabled\n" );
            }
            if (lib.GetSupportedExts( exts )) {
                sevenzip_libfound = true;
                for (WStringArray::const_iterator extIt = exts.begin(); extIt != exts.end(); extIt++)
                    SupportedExtensionsList.append( QString().fromWCharArray((*extIt).c_str()));
            }
            lib.Deinitialize();
        } else {
            fprintf( stderr, "lib7zip initialize failed, lib7zip scanning disabled\n" );
        }
#else
        if (*DEBUG_INFO_ENABLED) {
            qWarning() << "lib7zip library not supported";
        }
#endif
        SupportedExtensionsList.sort();
        int linelen = 0;
        for (int i = 0; i < SupportedExtensionsList.size(); i++) {
            if (linelen > 200) {
                linelen = 0;
                SupportedExtensions += "<br />&nbsp;&nbsp;";
                SupportedExtensions += SupportedExtensionsList.at( i );
                linelen += SupportedExtensionsList.at( i ).size();
            } else {
                if (linelen != 0) {
                    SupportedExtensions += " ";
                }
                SupportedExtensions += SupportedExtensionsList.at( i );
                linelen += SupportedExtensionsList.at( i ).size() + 2;
            }
        }
        sevenzip_libfound_text = "";
#ifdef USE_LIB7ZIP
        sevenzip_libfound_text = "<font color=\"red\">" + tr( "lib7zip not found" ) + "</font>";
        if (sevenzip_libfound) {
#ifdef LIB_7ZIP_VERSION
            QString Lib7ZipVersion = LIB_7ZIP_VERSION;
#else
            QString Lib7ZipVersion = tr( "unknown" );
#endif
            sevenzip_libfound_text = "<font color=\"green\">" + tr( "lib7zip found" ) + ": " + Lib7ZipVersion + "</font>";
        }

#else
        sevenzip_libfound_text = "<font color=\"red\">" + tr( "lib7zip not supported" ) + "</font>";
        labArchiveExtensionsStatusIcon->setToolTip( tr( "support not compiled in" ));
#endif
        labArchiveExtensions->setText(  tr( "Archive support:" ) + " " + sevenzip_libfound_text );
        labArchiveExtensionsStatusIcon->setToolTip( tr( "Supported extensions:" ) + "&nbsp;" + SupportedExtensions );
    }


#ifndef NO_MEDIAINFO
    // FIXME: get from fileinfo
    CdcatMediaInfo me;

    bool fileinfo_libfound = me.getMediaInfoLibFound();
    QStringList SupportedFileInfoExtensionsList = me.getSupportedExtensions();

    SupportedFileInfoExtensionsList.sort();
    QString SupportedFileInfoExtensions = "";
    int linelen = 0;
    for (int i = 0; i < SupportedFileInfoExtensionsList.size(); i++) {
        if (linelen > 200) {
            linelen = 0;
            SupportedFileInfoExtensions += "<br />&nbsp;&nbsp;";
            SupportedFileInfoExtensions += SupportedFileInfoExtensionsList.at( i );
            linelen += SupportedFileInfoExtensionsList.at( i ).size();
        } else {
            if (linelen != 0) {
                SupportedFileInfoExtensions += " ";
            }
            SupportedFileInfoExtensions += SupportedFileInfoExtensionsList.at( i );
            linelen += SupportedFileInfoExtensionsList.at( i ).size() + 2;
        }
    }
    QString fileinfo_libfound_text = "<font color=\"red\">" + tr( "mediainfo not found" ) + "</font>";
    if (fileinfo_libfound) {
        fileinfo_libfound_text = "<font color=\"green\">" + tr( "mediainfo found" ) + ": " + me.getMediaInfoVersion() + "</font>";
    }
    labFileInfoExtensions->setText( tr( "mediainfo status:" ) + " " + fileinfo_libfound_text );
    labFileInfoExtensionsStatusIcon->setToolTip( tr( "Supported extensions:" ) + "&nbsp;" + SupportedFileInfoExtensions );
#else
    QString fileinfo_libfound_text = "<font color=\"red\">" + tr( "mediainfo not supported" ) + "</font>";
    labFileInfoExtensions->setText( tr( "mediainfo status:" ) + " " + fileinfo_libfound_text );
    labFileInfoExtensionsStatusIcon->setToolTip( tr( "support not compiled in" ));
#endif

    thumb_supported_ext_text = "";
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    for (int i = 0; i < supportedFormats.size(); ++i) {
        thumb_supported_ext_text += " ";
        thumb_supported_ext_text += QString( supportedFormats.at( i ));
    }

    cbDoExcludeFiles->setChecked( conf->doExcludeFiles );
    cbUseWildcardInsteadRegexForExclude->setChecked( conf->useWildcardInsteadRegexForExclude );

    QStringList excludeList = conf->ExcludeFileList;
    exclude_model = new QStandardItemModel( 0, 1, this );
    for (int i = 0; i < excludeList.size(); i++) {
        // qDebug() << "insert rule:" << qPrintable(excludeList.at(i));
        if (QString( excludeList.at( i )).isEmpty()) {
            continue;
        }
        QStandardItem *modelitem = new QStandardItem( QString( excludeList.at( i )));
        exclude_model->appendRow( modelitem );
    }
    listviewExcludeFiles->setModel( exclude_model );
    checkExcludeRules();
    connect( exclude_model, SIGNAL( itemChanged( QStandardItem * )), this, SLOT( excludeDataChanged( QStandardItem * )));

    languageChange();
    schanged( 0 );
}

int SelReadable::schanged( int ) {
    if (cbCont->isChecked()) {
        lineFiles->setEnabled( true );
        labelContentSize->setEnabled( true );
        labelContentSizeUnit->setEnabled( true );
        maxSpinBox->setEnabled( true );
        labelContent->setEnabled( true );
        cbUseExternalContentViewer->setEnabled( true );
        if (cbUseExternalContentViewer->isChecked()) {
            lineeditPathExternalContentViewer->setEnabled( true );
            buttonUseExternalContentViewer->setEnabled( true );
        } else {
            lineeditPathExternalContentViewer->setEnabled( false );
            buttonUseExternalContentViewer->setEnabled( false );
        }
    } else {
        lineFiles->setEnabled( false );
        maxSpinBox->setEnabled( false );
        labelContent->setEnabled( false );
        labelContentSize->setEnabled( false );
        labelContentSizeUnit->setEnabled( false );
        cbUseExternalContentViewer->setEnabled( false );
        lineeditPathExternalContentViewer->setEnabled( false );
        buttonUseExternalContentViewer->setEnabled( false );
    }

    if (cbTag->isChecked()) {
        cbDefaultMp3TagVersion->setEnabled( true );
    } else {
        cbDefaultMp3TagVersion->setEnabled( false );
    }

    if (cbThumb->isChecked()) {
        labThumbExts->setEnabled( true );
        labThumbXSize->setEnabled( true );
        thumbLineExts->setEnabled( true );
        thumbWidthSpinBox->setEnabled( true );
        thumbHeightSpinBox->setEnabled( true );
    } else {
        labThumbExts->setEnabled( false );
        labThumbXSize->setEnabled( false );
        thumbLineExts->setEnabled( false );
        thumbWidthSpinBox->setEnabled( false );
        thumbHeightSpinBox->setEnabled( false );
    }
    if (cbScanArchive->isChecked()) {
        groupBoxShowArchiveFileOpts->setEnabled( true );
        if (conf->showProgressedArchiveFileInStatus) {
            cbShowProgressedArchiveFileInStatus->setEnabled( true );
        }
        labArchiveExtensions->setEnabled( true );
        labArchiveExtensionsStatusIcon->setEnabled( true );
    } else {
        groupBoxShowArchiveFileOpts->setEnabled( false );
        cbShowProgressedArchiveFileInStatus->setEnabled( false );
        labArchiveExtensions->setEnabled( false );
        labArchiveExtensionsStatusIcon->setEnabled( false );
    }

    if (cbDoExcludeFiles->isChecked()) {
        listviewExcludeFiles->setEnabled( true );
    } else {
        listviewExcludeFiles->setEnabled( false );
    }
    return 0;
}


int SelReadable::sok( void ) {
    conf->doScanArchive = cbScanArchive->isChecked();
    conf->showProgressedArchiveFileInStatus = cbShowProgressedArchiveFileInStatus->isChecked();
    conf->show_archive_file_perms = cbShowArchiveFilePerms->isChecked();
    conf->show_archive_file_user = cbShowArchiveFileUser->isChecked();
    conf->show_archive_file_group = cbShowArchiveFileGroup->isChecked();
    conf->show_archive_file_size = cbShowArchiveFileSize->isChecked();
    conf->show_archive_file_date = cbShowArchiveFileDate->isChecked();
    conf->show_archive_file_comment = cbShowArchiveFileComment->isChecked();
    conf->readcontent = cbCont->isChecked();
    conf->usefileinfo = cbFileInfo->isChecked();
    conf->storeThumb = cbThumb->isChecked();
    conf->thumbWidth = thumbWidthSpinBox->value();
    conf->thumbHeight = thumbHeightSpinBox->value();
#ifdef USE_LIBEXIF
    conf->storeExifData = cbExif->isChecked();
#endif
    conf->readcfiles = lineFiles->text();
    conf->useExternalContentViewer = cbUseExternalContentViewer->isChecked();
    conf->ExternalContentViewerPath = lineeditPathExternalContentViewer->text();
    conf->ThumbExtsList = thumbLineExts->text().split( ";" );
    conf->readclimit = maxSpinBox->value() * 1024;
    conf->readinfo = cbInfo->isChecked();
    conf->readavii = cbaInfo->isChecked();
    conf->readtag = cbTag->isChecked();
    if (cbDefaultMp3TagVersion->currentIndex() == 0) {
        conf->v1_over_v2 = true;
    } else {
        conf->v1_over_v2 = false;
    }
    conf->doExcludeFiles = cbDoExcludeFiles->isChecked();
    conf->ExcludeFileList.clear();
    for (int i = 0; i < exclude_model->rowCount(); i++)
        conf->ExcludeFileList.append( exclude_model->index( i, 0 ).data().toString());
    conf->useWildcardInsteadRegexForExclude = cbUseWildcardInsteadRegexForExclude->isChecked();
    close();
    return 0;
}

int SelReadable::scan( void ) {
    close();
    return 0;
}

int SelReadable::selectExternalContentViewer( void ) {
    // get file.name
    QString s = QFileDialog::getOpenFileName( 0, tr( "Choose path to external context viewer" ), conf->lastDir );

    selectExternalContentViewerString( s );
    if (!s.isEmpty()) {
        return 0;
    } else {
        return 1;
    }
}

void SelReadable::selectExternalContentViewerString( const QString &s ) {
    if (!s.isEmpty()) {
        lineeditPathExternalContentViewer->setText( s );
    }
    if (QFileInfo( lineeditPathExternalContentViewer->text()).exists()) {
        QPalette palette;
        palette.setColor( lineeditPathExternalContentViewer->backgroundRole(), QColor( "#99FF66" ));
        lineeditPathExternalContentViewer->setPalette( palette );
        lineeditPathExternalContentViewer->setToolTip( tr( "Path to external content viewer (found)" ));
        return;
    }

    QPalette palette;
    palette.setColor( lineeditPathExternalContentViewer->backgroundRole(), QColor( "#FF9999" ));
    lineeditPathExternalContentViewer->setPalette( palette );
    lineeditPathExternalContentViewer->setToolTip( tr( "Path to external content viewer (not found)" ));
    return;
}

void SelReadable::addExcludeRulesClicked() {
    bool ok;
    QString text = QInputDialog::getText( this, tr( "Add exclude rule …" ), tr( "Enter regular expression for exclude:" ), QLineEdit::Normal, "", &ok );

    if (ok) {
        QStandardItem *modelitem = new QStandardItem( text );
        exclude_model->appendRow( modelitem );
        qDebug() << "add rule:" << qPrintable( text );
        checkExcludeRules();
    }
}

void SelReadable::excludeContextMenuRequested( const QPoint& pos ) {
    QPoint globalPos = listviewExcludeFiles->mapToGlobal( pos );
    QModelIndex modelindex = listviewExcludeFiles->indexAt( pos );
    QString itemtext = exclude_model->data( modelindex, 0 ).toString();
    qDebug() << qPrintable( itemtext ) << "is clicked";

    contextmenu_modelindex = modelindex;
    QMenu excludeContextMenu( this );
    QAction EditRuleAction( tr( "edit rule …" ), this );
    excludeContextMenu.addAction( &EditRuleAction );
    connect( &EditRuleAction, SIGNAL( triggered()), this, SLOT( editExcludeRuleClicked()));

    QAction DeleteRuleAction( tr( "delete rule …" ), this );
    excludeContextMenu.addAction( &DeleteRuleAction );
    connect( &DeleteRuleAction, SIGNAL( triggered()), this, SLOT( deleteExcludeRuleClicked()));

    excludeContextMenu.exec( QCursor::pos());
}


void SelReadable::editExcludeRuleClicked() {
    QString itemtext = exclude_model->data( contextmenu_modelindex, 0 ).toString();
    qDebug() << qPrintable( itemtext ) << "is to be edited";
    bool ok;
    QString text = QInputDialog::getText( this, tr( "Add exclude rule …" ), tr( "Enter regular expression for exclude:" ), QLineEdit::Normal, itemtext, &ok );

    if (ok) {
        exclude_model->setData( contextmenu_modelindex, text );
        qDebug() << "edited rule:" << qPrintable( text );
        checkExcludeRules();
    }
}

void SelReadable::deleteExcludeRuleClicked() {
    QString itemtext = exclude_model->data( contextmenu_modelindex, 0 ).toString();
    qDebug() << qPrintable( itemtext ) << "is to be deleted";

    exclude_model->removeRows( contextmenu_modelindex.row(), 1 );
}

void SelReadable::checkExcludeRules() {
    for (int i = 0; i < exclude_model->rowCount(); i++) {
        QString itemtext = exclude_model->index( i, 0 ).data().toString();
        qDebug() << qPrintable( itemtext ) << "is to be checked";
        QRegExp re;
        re.setPattern( itemtext );
        if (cbUseWildcardInsteadRegexForExclude->isChecked()) {
            re.setPatternSyntax( QRegExp::Wildcard );
        } else {
            re.setPatternSyntax( QRegExp::RegExp );
        }
        re.setCaseSensitivity( Qt::CaseInsensitive );
        QStandardItem *item = exclude_model->item( i, 0 );
        if (!re.isValid()) {
            item->setBackground( QColor( "#FF9999" ));
            item->setToolTip( tr( "regular expression is invalid" ));
        } else {
            item->setBackground( QColor( "#99FF66" ));
            item->setToolTip( tr( "regular expression is valid" ));
        }
    }
}

void SelReadable::labExcludeRexexInfoButtonClicked() {
    RegexInfoHtml = "";
    RegexInfoHtml += "<h4><span class=\"mw-headline\" id=\"POSIX_Basic_Regular_Expressions\">+" + tr( "POSIX Basic Regular Expressions" ) + "</span></h4>";
    // RegexInfoHtml += "<p>"+tr("Traditional <a href=\"/wiki/Unix\" title=\"Unix\">Unix</a> regular expression syntax followed common conventions but often differed from tool to tool. The <a href=\"/wiki/Institute_of_Electrical_and_Electronics_Engineers\" title=\"Institute of Electrical and Electronics Engineers\">IEEE</a> ";
    // RegexInfoHtml += "<a href=\"/wiki/POSIX\" title=\"POSIX\">POSIX</a> Basic Regular Expressions (BRE) standard (ISO/IEC 9945-2:1993 <i>Information technology -- Portable Operating System Interface (POSIX) -- Part 2: Shell and Utilities</i>, successively revised as ISO/IEC 9945-2:2002 <i>Information technology -- Portable Operating System Interface (POSIX) -- Part 2: System Interfaces</i>, ISO/IEC 9945-2:2003, and currently ISO/IEC/IEEE 9945:2009 <i>Information technology -- Portable Operating System Interface (POSIX®) Base Specifications, Issue 7</i>) was designed mostly for backward compatibility with the traditional (Simple Regular Expression) syntax but provided a common standard which has since been adopted as the default syntax of many Unix regular expression tools, though there is often some variation or additional features.")+"</p>";

    // RegexInfoHtml += "<p>"+tr("BRE was released alongside an alternative flavor called Extended Regular Expressions or ERE. Many Unix tools also provide support for ERE syntax with <a href=\"/wiki/Command_line_argument\" title=\"Command line argument\" class=\"mw-redirect\">command line arguments</a>.")+"</p>";
    // RegexInfoHtml += "<p>"+tr("In the BRE syntax, most characters are treated as <a href=\"/wiki/Literal_(computer_science)\" title=\"Literal (computer science)\" class=\"mw-redirect\">literals</a>&#160;— they match only themselves (e.g., <code>a</code> matches \"<i>a</i>\"). The exceptions, listed below, are called <a href=\"/wiki/Metacharacter\" title=\"Metacharacter\">metacharacters</a> or metasequences.")+"</p>";

    // RegexInfoHtml += "<table class=\"wikitable\">";
    // RegexInfoHtml += "<caption>"+tr("Examples for char selection")+"</caption>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code>.</code></td>";
    // RegexInfoHtml += "<td>"+tr("most every char except special chars like &quot;\\&quot;")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code><b>[egh]</b></code></td>";
    // RegexInfoHtml += "<td>"+tr("one of the chars of &quot;e&quot;, &quot;g&quot; or &quot;h&quot;")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code><b>[0-6]</b></code></td>";
    // RegexInfoHtml += "<td>"+tr("one digit in range from &quot;0&quot; to &quot;6&quot; (hyphens are indicators for a range)")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code><b>[A-Za-z0-9]</b></code></td>";
    // RegexInfoHtml += "<td>"+tr("any latin char or any digit")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code><b>[^a]</b></code></td>";
    // RegexInfoHtml += "<td>"+tr("any digit except &quot;a&quot; (&quot;^&quot; at beginning of a char class negates it)")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr>";
    // RegexInfoHtml += "<td><code><b>[-A-Z]</b></code>, <code><b>[A-Z-]</b></code> (+"+tr("respectively ")+". <code><b>[A-Z-a-z]</b></code>, "+tr("but not corresponding POSIX")+"</td>";
    // RegexInfoHtml += "<td>"+tr("Selection contains also a hyphen &quot;-&quot; if the first or last char of a enumeration from a char class respectively PCRE and its meta function is disabled by a &quot;\\&quot; char before it.")+"</td>";
    // RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "</table>";
    //
    RegexInfoHtml += "<table class=\"wikitable\">";
    RegexInfoHtml += "<tr>";
    RegexInfoHtml += "<th>" + tr( "Metacharacter" ) + "</th>";
    RegexInfoHtml += "<th>" + tr( "Description" ) + "</th>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>.</code></td>";
    RegexInfoHtml += "<td>" + tr( "Matches any single character (many applications exclude <a href=\"http://en.wikipedia.org/wiki/Newline\" title=\"Newline\">newlines</a>, and exactly which characters are considered newlines is flavor-, character-encoding-, and platform-specific, but it is safe to assume that the line feed character is included). Within POSIX bracket expressions, the dot character matches a literal dot. For example, <code>a.c</code> matches \"<i>abc</i>\", etc., but <code>[a.c]</code> matches only \"<i>a</i>\", \"<i>.</i>\", or \"<i>c</i>\"." ) + "</td>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>[&#160;]</code></td>";
    RegexInfoHtml += "<td>" + tr( "A bracket expression. Matches a single character that is contained within the brackets. For example, <code>[abc]</code> matches \"<i>a</i>\", \"<i>b</i>\", or \"<i>c</i>\". <code>[a-z]</code> specifies a range which matches any lowercase letter from \"<i>a</i>\" to \"<i>z</i>\". These forms can be mixed: <code>[abcx-z]</code> matches \"<i>a</i>\", \"<i>b</i>\", \"<i>c</i>\", \"<i>x</i>\", \"<i>y</i>\", or \"<i>z</i>\", as does <code>[a-cx-z]</code>." );
    RegexInfoHtml += "<p>" + tr( "The <code>-</code> character is treated as a literal character if it is the last or the first (after the <code>^</code>) character within the brackets: <code>[abc-]</code>, <code>[-abc]</code>. Note that backslash escapes are not allowed. The <code>]</code> character can be included in a bracket expression if it is the first (after the <code>^</code>) character: <code>[]abc]</code>." ) + "</p>";
    RegexInfoHtml += "</td>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>[^&#160;]</code></td>";
    RegexInfoHtml += "<td>" + tr( "Matches a single character that is not contained within the brackets. For example, <code>[^abc]</code> matches any character other than \"<i>a</i>\", \"<i>b</i>\", or \"<i>c</i>\". <code>[^a-z]</code> matches any single character that is not a lowercase letter from \"<i>a</i>\" to \"<i>z</i>\". Likewise, literal characters and ranges can be mixed." ) + "</td>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>^</code></td>";
    RegexInfoHtml += "<td>" + tr( "Matches the starting position within the string. In line-based tools, it matches the starting position of any line." ) + "</td>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>$</code></td>";
    RegexInfoHtml += "<td>Matches the ending position of the string or the position just before a string-ending newline. In line-based tools, it matches the ending position of any line.</td>";
    RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr valign=\"top\">";
    // RegexInfoHtml += "<td><span class=\"nowrap\">BRE: <code>\\(&#160;\\)</code></span><br />";
    // RegexInfoHtml += "<span class=\"nowrap\">ERE: <code>(&#160;)</code></span></td>";
    // RegexInfoHtml += "<td>"+tr("Defines a marked subexpression. The string matched within the parentheses can be recalled later (see the next entry, <code>\\<i>n</i></code>). A marked subexpression is also called a block or capturing group.")+"</td>";
    // RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>\\<i>n</i></code></td>";
    RegexInfoHtml += "<td>" + tr( "Matches what the <i>n</i>th marked subexpression matched, where <i>n</i> is a digit from 1 to 9. This construct is theoretically <b>irregular</b> and was not adopted in the POSIX ERE syntax. Some tools allow referencing more than nine capturing groups." ) + "</td>";
    RegexInfoHtml += "</tr>";
    RegexInfoHtml += "<tr valign=\"top\">";
    RegexInfoHtml += "<td><code>*</code></td>";
    RegexInfoHtml += "<td>" + tr( "Matches the preceding element zero or more times. For example, <code>ab*c</code> matches \"<i>ac</i>\", \"<i>abc</i>\", \"<i>abbbc</i>\", etc. <code>[xyz]*</code> matches \"\", \"<i>x</i>\", \"<i>y</i>\", \"<i>z</i>\", \"<i>zx</i>\", \"<i>zyx</i>\", \"<i>xyzzy</i>\", and so on. <code>\\(ab\\)*</code> matches \"\", \"<i>ab</i>\", \"<i>abab</i>\", \"<i>ababab</i>\", and so on." ) + "</td>";
    RegexInfoHtml += "</tr>";
    // RegexInfoHtml += "<tr valign=\"top\">";
    // RegexInfoHtml += "<td><span class=\"nowrap\">BRE: <code>\\{<i>m</i>,<i>n</i>\\}</code></span><br />";
    // RegexInfoHtml += "<span class=\"nowrap\">ERE: <code>{<i>m</i>,<i>n</i>}</code></span></td>";
    // RegexInfoHtml += "<td>"+tr("Matches the preceding element at least <i>m</i> and not more than <i>n</i> times. For example, <code>a\\{3,5\\}</code> matches only //"<i>aaa</i>\", \"<i>aaaa</i>\", and \"<i>aaaaa</i>\". This is not found in a few older instances of regular expressions.")+"</td>";
    // RegexInfoHtml += "</tr>";
    RegexInfoHtml += "</table>";
    RegexInfoHtml += "<p><b>" + tr( "Examples:" ) + "</b></p>";
    RegexInfoHtml += "<ul>";
    RegexInfoHtml += "<li><code>.at</code> " + tr( "matches any three-character string ending with \"at\", including \"<i>hat</i>\", \"<i>cat</i>\", and \"<i>bat</i>\"." ) + "</li>";
    RegexInfoHtml += "<li><code>[hc]at</code> " + tr( "matches \"<i>hat</i>\" and \"<i>cat</i>\"." ) + "</li>";
    RegexInfoHtml += "<li><code>[^b]at</code> " + tr( "matches all strings matched by <code>.at</code> except \"<i>bat</i>\"." ) + "</li>";
    RegexInfoHtml += "<li><code>^[hc]at</code> " + tr( "matches \"<i>hat</i>\" and \"<i>cat</i>\", but only at the beginning of the string or line." ) + "</li>";
    RegexInfoHtml += "<li><code>[hc]at$</code> " + tr( "matches \"<i>hat</i>\" and \"<i>cat</i>\", but only at the end of the string or line." ) + "</li>";
    RegexInfoHtml += "<li><code>\\[.\\]</code> " + tr( "matches any single character surrounded by \"[\" and \"]\" since the brackets are escaped, for example: \"<i>[a]</i>\" and \"<i>[b]</i>\"." ) + "</li>";
    RegexInfoHtml += "</ul>";
    RegexInfoHtml += "<br>";
    RegexInfoHtml += tr( "Source:" ) + " " + tr( "<a href=\"http://en.wikipedia.org/wiki/Regex\">regular expressions</a>" );
    RegexInfoHtml += "<br>";
    RegexInfoHtml += tr( "From" ) + " <a href=\"http://www.wikipedia.org\">http://www.wikipedia.org</a>";

    QDialog di( this );
    di.setModal( true );
    di.setWindowTitle( tr( "About regular expressions" ));
    QVBoxLayout *layoutRegexInfoDialog = new QVBoxLayout( this );
    QPushButton *closeButton = new QPushButton( tr( "close" ), this );
    connect( closeButton, SIGNAL( clicked( bool )), &di, SLOT( close()));
    QTextBrowser *textBrowser1 = new QTextBrowser( &di );
    textBrowser1->setHtml( RegexInfoHtml );
    textBrowser1->setOpenExternalLinks( true );
    layoutRegexInfoDialog->addWidget( textBrowser1 );
    layoutRegexInfoDialog->addWidget( closeButton );
    di.setLayout( layoutRegexInfoDialog );
    di.setModal( false );
    di.resize( QSize( 750, 500 ));
    di.exec();
}

void SelReadable::excludeDataChanged( QStandardItem * ) {
    checkExcludeRules();
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelReadable::~SelReadable() {
    // no need to delete child widgets, Qt does it all for us
    delete exclude_model;
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelReadable::languageChange() {
    setWindowTitle( tr( "Select readable items" ));
    groupBoxShowArchiveFileOpts->setTitle( tr( "Archive file display options" ));
    cbScanArchive->setText( tr( "Scan for archive file list" ));
    cbShowProgressedArchiveFileInStatus->setText( tr( "show archive file in status" ));
    cbShowProgressedArchiveFileInStatus->setToolTip( tr( "show archive file at scanning in status" ));
    labArchiveExtensions->setText( tr( "Supported extensions:" ) + " " + sevenzip_libfound_text );
    cbShowArchiveFilePerms->setText( tr( "Permission" ));
    cbShowArchiveFileUser->setText( tr( "User" ));
    cbShowArchiveFileGroup->setText( tr( "Group" ));
    cbShowArchiveFileSize->setText( tr( "Size" ));
    cbShowArchiveFileDate->setText( tr( "Date" ));
    cbShowArchiveFileComment->setText( tr( "Comment" ));
    cbTag->setText( tr( "Read mp3 tags" ));
    cbThumb->setText( tr( "Read thumbnails" ));
    cbThumb->setToolTip( tr( "Read thumbnails from pictures" ));
    labThumbSize->setText( tr( "Size:" ));
    thumbWidthSpinBox->setToolTip( tr( "Thumbnail size (width) in pixels" ));
    thumbHeightSpinBox->setToolTip( tr( "Thumbnail size (height) in pixels" ));
    labThumbXSize->setText( tr( "x" ));
#ifdef USE_LIBEXIF
    cbExif->setText( tr( "Read EXIF data from pictures" ));
#endif

    labelDefaultMp3TagVersion->setText( tr( "Default tag" ));
    cbInfo->setText( tr( "Read mp3 info as comment" ));
    cbInfo->setToolTip( tr( "Read mp3 technical info as comment (bitrate,freq,length …)" ));
    cbaInfo->setText( tr( "Read avi info as comment" ));
    cbaInfo->setToolTip( tr( "Read avi technical info as comment (codecs,length,...)" ));
    cbCont->setText( tr( "Store content of some files" ));
    cbUseExternalContentViewer->setText( tr( "Use external file content viewer" ));
    buttonUseExternalContentViewer->setText( tr( "..." ));
    buttonUseExternalContentViewer->setToolTip( tr( "Select external viewer …" ));
    cbFileInfo->setText( tr( "Read some technical info using mediainfo" ));
    labelContent->setText( tr( "file patterns:" ));
    lineFiles->setToolTip( tr( "; separated list of readable file patterns" ));
    labThumbExts->setText( tr( "File extensions:" ));
    thumbLineExts->setToolTip( tr( "; separated list of image file extensions, e.g. png;jpg;gif" ));
    labThumbExtsStatusIcon->setToolTip( tr( "Supported image extensions found: " ) + thumb_supported_ext_text );
    labelContentSize->setText( tr( "max size:" ));
    maxSpinBox->setToolTip( tr( "content size limit in kByte" ));
    cbDoExcludeFiles->setText( tr( "exclude files/directories" ));
    cbUseWildcardInsteadRegexForExclude->setText( tr( "Use wildcard instead regex" ));
    cbUseWildcardInsteadRegexForExclude->setToolTip( tr( "Use wildcard expression instead regular expression" ));
    labExcludeRexexInfo->setText( tr( "About regex:" ));
    labExcludeRexexInfo->setToolTip( tr( "Information about regular expressions" ));
    labExcludeRexexInfoButton->setText( tr( "About regular expressions …." ));
    labExcludeRexexInfoButton->setToolTip( tr( "Show introduction into regular expressions" ));
    buttonAddExcludeRule->setText( tr( "add exclude rule …" ));
    listviewExcludeFiles->setToolTip( tr( "list of patterns (regular expression) for files/directories to skip on reading from filesystem" ));
    buttonOK->setText( tr( "Ok" ));
    buttonCancel->setText( tr( "Cancel" ));
}

