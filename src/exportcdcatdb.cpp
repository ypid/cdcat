

/****************************************************************************
                            Hyper's CD Catalog
	A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke <crissi99@gmx.de>
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/


#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qstring.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <iostream>

#include "icons.h"
#include "cdcat.h"
#include "exportcdcatdb.h"
#include "adddialog.h"

using namespace std;

/*
 *  Constructs a exportCdcatDB as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
exportCdcatDB::exportCdcatDB ( CdCatMainWidget *mainw, QWidget* parent, const char* name, bool modal, Qt::WFlags fl ) : QDialog ( parent, name, modal, fl ) {
    this->mainw = mainw;
    p = mainw->db;
    app = mainw->app;
    
    pww = NULL;

    if ( !name )
        setName ( "exportCdcatDB" );
    setSizeGripEnabled ( TRUE );

    this->setCaption ( tr ( "Export database..." ) );

    exportCdcatDBLayout = new Q3GridLayout ( this, 1, 1, 12, 6, "exportCdcatDBLayout" );

    layout32 = new Q3VBoxLayout ( 0, 0, 6, "layout32" );
    layout21 = new Q3HBoxLayout ( 0, 0, 6, "layout21" );
    layout10_2 = new Q3VBoxLayout ( 0, 0, 6, "layout10_2" );
    textLabel2 = new QLabel ( this, "textLabel2" );
    layout10_2->addWidget ( textLabel2 );

    listAllMedia = new Q3ListBox ( this, "listAllMedia" );
    listAllMedia->setSelectionMode ( Q3ListBox::Extended );

    layout10_2->addWidget ( listAllMedia );
    layout21->addLayout ( layout10_2 );

    layout19 = new Q3VBoxLayout ( 0, 0, 6, "layout19" );
    QSpacerItem* spacer = new QSpacerItem ( 20, 180, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout19->addItem ( spacer );

    layout10 = new Q3HBoxLayout ( 0, 0, 6, "layout10" );

    buttonRemove = new QPushButton ( this, "buttonRemove" );
    buttonRemove->setMinimumSize ( QSize ( 50, 23 ) );
    buttonRemove->setMaximumSize ( QSize ( 50, 23 ) );
    buttonRemove->setPixmap ( *get_t_remove_export_icon() );
    layout10->addWidget ( buttonRemove );

    buttonAdd = new QPushButton ( this, "buttonAdd" );
    buttonAdd->setMinimumSize ( QSize ( 50, 23 ) );
    buttonAdd->setMaximumSize ( QSize ( 50, 23 ) );
    buttonAdd->setPixmap ( *get_t_add_export_icon() );
    layout10->addWidget ( buttonAdd );
    layout19->addLayout ( layout10 );
    QSpacerItem* spacer_2 = new QSpacerItem ( 20, 210, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout19->addItem ( spacer_2 );
    layout21->addLayout ( layout19 );

    layout10_2_2 = new Q3VBoxLayout ( 0, 0, 6, "layout10_2_2" );

    textLabel2_2 = new QLabel ( this, "textLabel2_2" );
    layout10_2_2->addWidget ( textLabel2_2 );

    listSelectedMedia = new Q3ListBox ( this, "listSelectedMedia" );
    listSelectedMedia->setSelectionMode ( Q3ListBox::Extended );
    layout10_2_2->addWidget ( listSelectedMedia );
    layout21->addLayout ( layout10_2_2 );
    layout32->addLayout ( layout21 );

    layout27 = new Q3HBoxLayout ( 0,0,4,"layout27" );

    checkAllMedia = new QCheckBox ( this, "checkAllMedia" );
    layout27->addWidget ( checkAllMedia );

    layout32->addLayout ( layout27 );

    buttonGroup1 = new Q3ButtonGroup ( 3, Qt::Horizontal, tr ( "Type" ),this, "buttonGroup1" );
    layout26 = new Q3HBoxLayout ( 0, 0, 6, "layout26" );

    radioCsv = new QRadioButton ( buttonGroup1, "radioCsv" );

    seperatorLabel = new QLabel ( buttonGroup1, "seperatorLabel" );

    separatorInput = new QLineEdit ( buttonGroup1, "separatorInput" );

    separatorInput->setMaximumSize ( QSize ( 25, 32767 ) );

    radioHtml = new QRadioButton ( buttonGroup1, "radioHtml" );
    layout26->addWidget ( radioHtml );

    radioXml = new QRadioButton ( buttonGroup1, "radioXml" );
    layout26->addWidget ( radioXml );


    checkOnlyMedia = new QCheckBox ( buttonGroup1, "checkOnlyMedia" );

    seperatorLabel->setAlignment ( Qt::AlignRight );

    QSpacerItem* spacer_4 = new QSpacerItem ( 130, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout26->addItem ( spacer_4 );

    layout32->addWidget ( buttonGroup1 );

    buttonGroupFields = new Q3ButtonGroup ( 2, Qt::Horizontal,tr ( "What to export" ) ,this,"buttonGroupFields" );

    checkExportTitle = new QCheckBox ( buttonGroupFields,"checkExportTitle" );
    buttonGroupFields->insert ( checkExportTitle );

    checkExportTableHeader	= new QCheckBox ( buttonGroupFields,"checkExportTableHeader" );
    buttonGroupFields->insert ( checkExportTableHeader );

    checkExportMediaName = new	QCheckBox ( buttonGroupFields,"checkExportMediaName" );
    buttonGroupFields->insert ( checkExportMediaName );

    checkExportMediaNumber = new	QCheckBox ( buttonGroupFields,"checkExportMediaNumber" );
    buttonGroupFields->insert ( checkExportMediaNumber );

    checkExportPath = new QCheckBox ( buttonGroupFields,"checkExportPath" );
    buttonGroupFields->insert ( checkExportPath );


    checkExportSize = new QCheckBox ( buttonGroupFields,"checkExportSize" );
    buttonGroupFields->insert ( checkExportSize );

    checkExportDate = new QCheckBox ( buttonGroupFields,"checkExportDate" );
    buttonGroupFields->insert ( checkExportDate );

    checkExportComment = new QCheckBox ( buttonGroupFields,"checkExportComment" );
    buttonGroupFields->insert ( checkExportComment );

    checkExportCategory = new QCheckBox ( buttonGroupFields,"checkExportCategory" );
    buttonGroupFields->insert ( checkExportCategory );

    checkExportMp3Tag = new QCheckBox ( buttonGroupFields,"checkExportMp3Tag" );
    buttonGroupFields->insert ( checkExportMp3Tag );

    checkExportBorrow = new QCheckBox ( buttonGroupFields,"checkExportBorrow" );
    buttonGroupFields->insert ( checkExportBorrow );

    layout32->addWidget ( buttonGroupFields );

    layout29 = new Q3HBoxLayout ( 0, 0, 6, "layout29" );

    textLabel3 = new QLabel ( this, "textLabel3" );
    layout29->addWidget ( textLabel3 );

    fileName = new QLineEdit ( this, "fileName" );
    layout29->addWidget ( fileName );

    buttonFile = new QPushButton ( this, "buttonFile" );
    layout29->addWidget ( buttonFile );
    layout32->addLayout ( layout29 );

    layout4 = new Q3HBoxLayout ( 0, 0, 6, "layout4" );
    QSpacerItem* spacer_6 = new QSpacerItem ( 163, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem ( spacer_6 );

    buttonOk = new QPushButton ( this, "buttonOk" );
    buttonOk->setAutoDefault ( TRUE );
    buttonOk->setDefault ( TRUE );
    layout4->addWidget ( buttonOk );

    buttonCancel = new QPushButton ( this, "buttonCancel" );
    buttonCancel->setAutoDefault ( TRUE );
    layout4->addWidget ( buttonCancel );
    QSpacerItem* spacer_5 = new QSpacerItem ( 163, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem ( spacer_5 );
    layout32->addLayout ( layout4 );

    exportCdcatDBLayout->addLayout ( layout32, 0, 0 );
    languageChange();
    resize ( QSize ( 418, 389 ).expandedTo ( minimumSizeHint() ) );

    // signals and slots connections
    connect ( buttonOk, SIGNAL ( clicked() ), this, SLOT ( ok() ) );
    connect ( buttonCancel, SIGNAL ( clicked() ), this, SLOT ( cancel() ) );
    connect ( buttonAdd, SIGNAL ( clicked() ), this, SLOT ( addMedia() ) );
    connect ( buttonRemove, SIGNAL ( clicked() ), this, SLOT ( removeMedia() ) );
    connect ( checkAllMedia, SIGNAL ( toggled ( bool ) ), this, SLOT ( listAllMediaToggled() ) );
    connect ( buttonFile, SIGNAL ( clicked() ), this, SLOT ( browseFile() ) );
    connect ( radioCsv, SIGNAL ( clicked() ), this, SLOT ( exportTypeToggled() ) );
    connect ( radioHtml, SIGNAL ( clicked() ), this, SLOT ( exportTypeToggled() ) );
    connect ( radioXml, SIGNAL ( clicked() ), this, SLOT ( exportTypeToggled() ) );

    connect ( checkOnlyMedia, SIGNAL ( clicked() ), this, SLOT ( checkOnlyMediaToggled() ) );

    connect ( checkExportTitle, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportTableHeader, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportMediaName, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportPath, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportSize, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportDate, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportComment, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );
    connect ( checkExportCategory, SIGNAL ( clicked() ), this, SLOT ( checkExportToggled() ) );

    separatorInput->setText ( ";" );
    separatorInput->setEnabled ( false );
    seperatorLabel->setEnabled ( false );
    radioHtml->setChecked ( true );

    if ( p != NULL )
        fileName->setText ( mainw->cconfig->lastDir +"/"+ ( ( DBCatalog * ) ( ( p->getRootNode() )->data ) )->name+".html" );

    checkOnlyMedia->setChecked ( false );

    checkExportTableHeader->setChecked ( true );
    checkExportTitle->setChecked ( true );
    checkExportMediaName->setChecked ( true );
    checkExportPath->setChecked ( true );
    checkExportSize->setChecked ( true );
    checkExportDate->setChecked ( true );
    checkExportComment->setChecked ( true );
    checkExportCategory->setChecked ( true );

    outstring = "";
    medianame = "";
    currentpath = "";
    lastMedianame="";
    medianumber = 0;
    fillMedia();
}

/*
 *  Destroys the object and frees any allocated resources
 */
exportCdcatDB::~exportCdcatDB() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void exportCdcatDB::languageChange() {

    textLabel2->setText ( tr ( "Availiable media" ) );
    QToolTip::add
    ( listAllMedia, tr ( "Add media" ) );
    buttonRemove->setText ( QString::null );
    QToolTip::add
    ( buttonRemove, tr ( "Remove media" ) );
    buttonAdd->setText ( QString::null );
    QToolTip::add
    ( buttonAdd, tr ( "Add media" ) );
    textLabel2_2->setText ( tr ( "Media to export" ) );
    QToolTip::add
    ( listSelectedMedia, tr ( "Media to export" ) );
    checkAllMedia->setText ( tr ( "All media" ) );
    checkOnlyMedia->setText ( tr ( "Only media" ) );
    //spinColsLabel->setText( tr( "columns:" ) );
    buttonGroup1->setTitle ( tr ( "Type" ) );
    radioHtml->setText ( tr ( "export as HTML" ) );
    radioCsv->setText ( tr ( "export as CSV" ) );
    radioXml->setText ( tr ( "export as XML" ) );

    seperatorLabel->setText ( tr ( "separator:" ) );
    textLabel3->setText ( tr ( "File to export:" ) );
    buttonFile->setText ( tr ( "..." ) );
    buttonOk->setText ( tr ( "&OK" ) );
    checkExportMediaName->setText ( tr ( "Media name" ) );
    checkExportMediaNumber->setText ( tr ( "Media number" ) );
    checkExportPath->setText ( tr ( "Path" ) );
    //	checkExportFile->setText( tr( "export file" ) );
    checkExportSize->setText ( tr ( "Size" ) );
    checkExportDate->setText ( tr ( "Date" ) );
    checkExportComment->setText ( tr ( "Comment" ) );
    checkExportCategory->setText ( tr ( "Category" ) );
    checkExportMp3Tag->setText( tr( "export mp3 tag" ) );
    checkExportBorrow->setText( tr( "export borrow information" ) );
    checkExportTitle->setText ( tr ( "HTML headline" ) );
    checkExportTableHeader->setText ( tr ( "table header/comment line" ) );

#ifndef _WIN32

    buttonOk->setAccel ( QKeySequence ( QString::null ) );
#endif

    buttonCancel->setText ( tr ( "&Cancel" ) );

#ifndef _WIN32

    buttonCancel->setAccel ( QKeySequence ( QString::null ) );
#endif
}

void exportCdcatDB::fillMedia() {
    if ( p == NULL ) {
        buttonOk ->setEnabled ( false );
        ;
        listAllMedia ->setEnabled ( false );
        listSelectedMedia->setEnabled ( false );
        buttonAdd->setEnabled ( false );
        buttonRemove ->setEnabled ( false );
        buttonFile ->setEnabled ( false );
        fileName ->setEnabled ( false );
        checkAllMedia->setEnabled ( false );
        return ;
    }
    Node *source = p->getRootNode();
    setCaption ( tr ( "Export CdCat database: " ) + ( ( DBCatalog * ) ( source->data ) ) ->name );
    Node *tmp;
    tmp = source->child;

    while ( tmp != NULL ) {
        if ( tmp->type != HC_MEDIA ) {
            tmp = tmp->next;
            continue;
        }

        this->listAllMedia->insertItem ( tmp->getNameOf() );
        tmp = tmp->next;
    }

    listAllMedia->sort();
}

bool exportCdcatDB::doHtmlExport() {
    //fileName->setText("/data/test.html");
    return true;
}

bool exportCdcatDB::doCsvExport() {
    //fileName->setText("/data/test.txt");
    return true;
}

void exportCdcatDB::ok() {
    if ( ( fileName->text() ).isEmpty() )
        QMessageBox::critical ( 0, tr ( "Filename missing" ), tr ( "Please enter a filename!" ) );
    else if ( ( separatorInput->text() ).isEmpty() )
        QMessageBox::critical ( 0, tr ( "Separator missing" ), tr ( "Please enter a separator!" ) );
    else {
        //QApplication::setOverrideCursor( waitCursor );

        separator = separatorInput->text();

        /*extension correction:*/
        if ( radioHtml->isChecked() &&
                ! ( ( ( fileName->text() ).right ( 5 ) ).lower() == ".html" ||
                    ( ( fileName->text() ).right ( 4 ) ).lower() == ".htm" ) ) {
            QString q ( fileName->text() );
            fileName->setText ( q.append ( ".html" ) );
        }

        else if ( radioCsv->isChecked() &&
                ( ( fileName->text() ).right ( 4 ) ).lower() != ".csv" ) {
            QString q ( fileName->text() );
            fileName->setText ( q.append ( ".csv" ) );
        }

        else if ( radioXml->isChecked() &&
                ( ( fileName->text() ).right ( 4 ) ).lower() != ".xml" ) {
            QString q ( fileName->text() );
            fileName->setText ( q.append ( ".xml" ) );
        }

	/* === header === */
	if ( radioHtml->isChecked() ) {

		outstring += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n";
		outstring += "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
		outstring += "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"de\" lang=\"de\">\n";
		outstring += "<head>\n";
		outstring += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n";
		outstring += "\n";
		outstring += " <!-- This file was generated by CdCat ";
		outstring += HOMEPAGE;
		outstring += " -->\n";
		outstring += "<title> "+ tr ( "Export from cdcat:" ) + " " + ( ( DBCatalog * ) ( ( p->getRootNode() ) ->data ) ) ->name + "</title>\n";
		outstring += "<style>\n";	// we're gonna define style for our html document
		outstring += ".m { color: red; }\n"; // m - media
		outstring += ".f { color: blue; }\n"; // f - file
		outstring += ".d { color: green; }\n"; // d - date
		outstring += ".c { color: darkgray; }\n"; // c - comment
		outstring += "table { border-collapse: collapse; }"; // extremely usefull table feature,
		// this causes table border to collapse into 1px wide border
		// and there is no need 4 putting "&nbsp;" instead " " (space) to display empty cells correcty
		//  (in mozilla/ie.. konqueror does seem to handle this)
		outstring += "</style>\n";

		outstring += "</head>\n";
		outstring += "<body>\n";
		if ( checkExportTitle->isChecked() )
		outstring += "<h1>" + tr ( "Export from cdcat, catalog:" ) + " " + ( ( DBCatalog * ) ( ( p->getRootNode() ) ->data ) ) ->name + "</h1>\n";
		outstring += "<p>" + tr ( "Generated at:" ) + " " + QDateTime().currentDateTime().toString() + "</p>\n";
		outstring += "<table border=\"1\">";
		if ( checkExportTableHeader->isChecked() ) {
			if ( !checkOnlyMedia->isChecked() ) {
				outstring += "<tr>";
				if ( checkExportMediaName->isChecked() )
					outstring += "<th>" + tr ( "Media" ) + "</th>";
				if ( checkExportMediaNumber->isChecked() )
					outstring += "<th>" + tr ( "#" ) + "</th>";
				if ( checkExportPath->isChecked() )
					outstring += "<th>" + tr ( "File" ) + "</th>";
				if ( checkExportSize->isChecked() )
					outstring += "<th>" + tr ( "Size" ) + "</th>";
				if ( checkExportDate->isChecked() )
					outstring += "<th>" + tr ( "Date" ) + "</th>";
				if ( checkExportComment->isChecked() )
					outstring += "<th>" + tr ( "Comment" ) + "</th>";
				if ( checkExportMp3Tag->isChecked() )
					outstring += "<th>" + tr ( "MP3 tag" ) + "</th>";
				if ( checkExportBorrow->isChecked() )
					outstring += "<th>" + tr ( "Borrow" ) + "</th>";
				if ( checkExportCategory->isChecked() )
					outstring += "<th>" + tr ( "Category" ) + "</th>";
				outstring += "</tr>\n";
			} else {
				outstring += "<tr><td><strong>" + tr ( "Media" ) + "</strong></td></tr>\n";
			}
		}

	}
	if ( radioCsv->isChecked() ) {
		if(checkExportTableHeader->isChecked()) {
			outstring += "# "+tr ( "Export from cdcat, catalog:" ) + " " + ( ( DBCatalog * ) ( ( p->getRootNode() ) ->data ) ) ->name + "\n";
			outstring += "# " + tr ( "Generated at:" ) + " " + QDateTime().currentDateTime().toString() + "\n";
			outstring += "# "+tr("field list")+":\n";
			outstring += "# ";
			if(checkExportMediaName->isChecked()) {
				outstring += tr("Media name");
				outstring += separator;
			}
			
			if ( checkExportMediaNumber->isChecked() ) {
				outstring += tr("Media number");
				outstring += separator;
			}
			
			if ( checkExportPath->isChecked() ) {
				outstring += tr("Path");
				outstring += separator;
			}
			
			if(checkExportSize->isChecked()) {
				outstring += tr("Size");
				outstring += separator;
			}
			
			if ( checkExportDate->isChecked() ) {
				outstring += tr("Date");
				outstring += separator;
			}
			
			if ( checkExportComment->isChecked() ) {
				outstring += tr("Comment");
				outstring += separator;
			}
			
			if ( checkExportMp3Tag->isChecked() ) {
				outstring += tr("MP3 tag");
				outstring += separator;
			}
			if ( checkExportBorrow->isChecked() ) {
				outstring += tr("Borrow");
				outstring += separator;
			}
			if ( checkExportCategory->isChecked() ) {
				outstring += tr("Category");
				outstring += separator;
			}
			
			outstring += "\n";
		}
	}

	/* header end */

        f.setName ( fileName->text() );

        int overwrite=0;
        if ( f.exists() ) {
            //QApplication::restoreOverrideCursor();
            QMessageBox mb ( tr ( "Overwrite?" ),
                             tr ( "Do you want overwrite this file?" ),
                             QMessageBox::Critical,
                             QMessageBox::Yes,
                             QMessageBox::Cancel | QMessageBox::Escape | QMessageBox::Default,
                             0 );
            mb.setButtonText ( QMessageBox::Yes, tr ( "Yes" ) );
            mb.setButtonText ( QMessageBox::Cancel, tr ( "Discard" ) );
            overwrite = mb.exec();
        }


        pww = new PWw ( this,app, false, 0, tr("Exporting, please wait...") );
        progress ( pww );
	if ( !f.exists() || overwrite == QMessageBox::Yes ) {
		if (!radioXml->isChecked()) {
			//QApplication::setOverrideCursor( waitCursor );
			if ( f.open ( QIODevice::WriteOnly ) ) {
				QTextStream str ( &f );   // we will serialize the data into file f
				str.setEncoding ( QTextStream::UnicodeUTF8 );
				//str << " # output generated by cdcat" << endl;
				writeDown ( p->getRootNode() );

				/* === footer === */
				if ( radioHtml->isChecked() )
				outstring += "</table></body></html>";

				/* footer end */

				str << outstring;
				f.close();

				//QApplication::restoreOverrideCursor();

			} else {
				//QApplication::restoreOverrideCursor();
				QMessageBox::critical ( 0, tr ( "Error during write export..." ), tr ( "I can't create or rewrite the file" ) );
			}
		}
		else {
			// XML
			int retv=0;
			char fnc[256];
			char fnc2[256];
			char tmpbuffer[2048];
			QString fn;

			fn = fileName->text().replace(".xml", "");

			sprintf ( fnc,"%s.xml.gz", ( const char * ) fn );
			sprintf ( fnc2,"%s.xml", ( const char * ) fn );

// 			std::cerr << "fnc: " << fnc << ", fnc2: " << fnc2 << std::endl; 
			
			QFile f(fnc2);
			bool fnc2_openend = f.open(QIODevice::ReadWrite);
			
			if ( fnc2_openend ) {
				mainw->db->XML_ENCODING = "UTF-8";
				if ( mainw->db->saveAsDB ( fnc ) != 0 ) { // An error occured
					QMessageBox::critical ( 0, tr ( "Error during write export..." ), tr ( "I can't create or rewrite the file" ) );
				}
				else {
					// now lets rewrite it
					gzFile ff = gzopen ( fnc,"r" );
					if(ff == NULL) {
						std::cerr << "open gz file failed!" << std::endl;
					}
					else {
						int readcount=0;
						readcount = gzread(ff, tmpbuffer, 2048);
						while (readcount > 0) {
// 							std::cerr << "read " << readcount << " bytes" << std::endl; 
							f.write(QByteArray(tmpbuffer));
							progress(pww);
							readcount = gzread(ff, tmpbuffer, 2048);
						}
						gzclose(ff);
						unlink(fnc);
					}
					f.close();
				}
			}
			else {

				if(!fnc2_openend) {
					std::cerr << "write file open failed!" << std::endl;
				}
			}

		}
	}
	else {
// 		QApplication::restoreOverrideCursor();
		}
	}
    close();
    QApplication::restoreOverrideCursor();
    pww->end();
    delete pww;
}

void exportCdcatDB::cancel() {
    close();
}

void exportCdcatDB::addMedia() {
    uint i;
    for ( i = 0; i < listAllMedia->count();i++ )
        if ( listAllMedia->isSelected ( i ) )
            listSelectedMedia->insertItem ( listAllMedia->text ( i ) );


    for ( i = 0; i < listAllMedia->count();i++ )
        while ( i < listAllMedia->count() && listAllMedia->isSelected ( i ) )
            listAllMedia->removeItem ( i );
    listAllMedia->sort();
    listSelectedMedia->sort();

}

void exportCdcatDB::removeMedia() {
    uint i;
    for ( i = 0; i < listSelectedMedia->count();i++ )
        if ( listSelectedMedia->isSelected ( i ) )
            listAllMedia->insertItem ( listSelectedMedia->text ( i ) );


    for ( i = 0; i < listSelectedMedia->count();i++ )
        while ( i < listSelectedMedia->count() && listSelectedMedia->isSelected ( i ) )
            listSelectedMedia->removeItem ( i );
    listAllMedia->sort();
    listSelectedMedia->sort();

}

void exportCdcatDB::browseFile() {
    QString s = QFileDialog::getSaveFileName(0, tr ( "Choose a file!" ), "", tr ( "All files (*.*)" ));
    if ( !s.isEmpty() )
        fileName->setText ( s );
}

int exportCdcatDB::writeDown ( Node *source ) {
    int i = 0;
    progress ( pww );
    switch ( source->type ) {
    case HC_UNINITIALIZED:
        return 1;
    case HC_CATALOG:
        i = writeCatalog ( source );
        break;
    case HC_MEDIA:
        i = writeMedia ( source );
        break;
    case HC_DIRECTORY:
        i = writeDirectory ( source );
        break;
    case HC_FILE:
        i = writeFile ( source );
        break;
    case HC_MP3TAG:
        i = writeMp3Tag ( source );
        break;
    }
    return i;
}

int exportCdcatDB::writeCatalog ( Node *source ) {

    if ( source->child != NULL )
        writeDown ( source->child );

    if ( source->next != NULL )
        writeDown ( source->next );

    return 0;
}

int exportCdcatDB::writeMedia ( Node *source ) {
	bool inList = false;
	medianame = ( ( DBMedia * ) ( source->data ) ) ->name;
	medianumber = ( ( DBMedia * ) ( source->data ) ) ->number;
	mediaborrow = ( ( DBMedia * ) ( source->data ) ) ->borrowing;
	if(mediaborrow.isEmpty())
		mediaborrow = "-";
	
	if ( checkAllMedia->isChecked() )
		inList = true;
	else
		// need to check if in listSelectedMedia
		for ( uint i = 0; i < listSelectedMedia->count();i++ ) {
			if ( listSelectedMedia->text ( i ) == medianame ) {
				inList = true;
				break;
			}
		}
	
        if ( inList ) {
		if ( radioHtml->isChecked() ) {
			outstring += "<tr>";
			outstring += "<td class=\"m\">" + medianame + "</td>";
			outstring += "<td class=\"m\">/</td>";
			double tsize = 0;
			outstring += "<td class=\"m\">" + QString().setNum ( ( long int ) tsize ) + "</td>";
			outstring += "<td class=\"m\">" + date_to_str (( ( DBMedia * ) ( source->data ) )->modification) + "</td>";
			outstring += "</tr>";

			outstring += "<tr>";
			if ( checkExportMediaName->isChecked() ) {
				outstring += "<td>" + medianame + "</td>";
			}
			if ( checkExportMediaNumber->isChecked() ) {
				outstring += "<td>" + QString().setNum( (( DBMedia * ) ( source->data ) )->number) + "</td>";
			}
			if ( checkExportPath->isChecked() ) {
				outstring += "<td>/</td>";
			}
			if ( checkExportSize->isChecked() ) {
				double tsize = 0;
				outstring += "<td>" + QString().setNum ( ( long int ) tsize ) + "</td>";
			}
			if ( checkExportDate->isChecked() ) {
				outstring += "<td>" + date_to_str (( ( DBMedia * ) ( source->data ) )->modification) + "</td>";
			}
			if ( checkExportComment->isChecked() ) {
				outstring += "<td>" + ( ( DBMedia * ) ( source->data ) )->comment + "</td>";
			}
			if ( checkExportMp3Tag->isChecked() ) {
				outstring += "<td></td>";
			}
			if ( checkExportBorrow->isChecked() ) {
				outstring += "<td>" + ( ( DBMedia * ) ( source->data ) )->borrowing + "</td>";
			}
			if ( checkExportCategory->isChecked() ) {
				outstring += "<td>" + ( ( DBMedia * ) ( source->data ) )->category + "</td>";
			}
			outstring += "</tr>\n";
		}
		else if ( radioCsv->isChecked() ) {
			if ( checkExportMediaName->isChecked() ) {
				outstring += medianame;
				outstring += separator;
			}
			if ( checkExportMediaNumber->isChecked() ) {
				outstring += QString().setNum( (( DBMedia * ) ( source->data ) )->number);
				outstring += separator;
			}
			if ( checkExportPath->isChecked() ) {
				outstring += "/";
				outstring += separator;
			}
			if ( checkExportSize->isChecked() ) {
				double tsize = 0;
				outstring += QString().setNum ( ( long int ) tsize );
				outstring += separator;
			}
			if ( checkExportDate->isChecked() ) {
				outstring += date_to_str (( ( DBMedia * ) ( source->data ) )->modification);
				outstring += separator;
			}
			if ( checkExportComment->isChecked() ) {
				outstring += ( ( DBMedia * ) ( source->data ) )->comment;
				outstring += separator;
			}
			if ( checkExportMp3Tag->isChecked() ) {
				outstring += "";
				outstring += separator;
			}
			if ( checkExportBorrow->isChecked() ) {
				outstring += ( ( DBMedia * ) ( source->data ) )->borrowing;
				outstring += separator;
			}
			if ( checkExportCategory->isChecked() ) {
				outstring += ( ( DBMedia * ) ( source->data ) )->category;
				outstring += separator;
			}
			outstring += "\n";
		}
		
		if ( ! checkOnlyMedia->isChecked() ) {
			if ( source->child != NULL )
				writeDown ( source->child );
		}
	}
	if ( source->next != NULL )
		writeDown ( source->next );
	
	return 0;
}

int exportCdcatDB::writeDirectory ( Node *source ) {
    if ( source->child != NULL )
        writeDown ( source->child );

    if ( source->next != NULL )
        writeDown ( source->next );

    return 0;
}

int exportCdcatDB::writeFile ( Node *source ) {
    double tsize=0;

    //==== CSV ===========
    if ( radioCsv->isChecked() ) {
	if(checkExportMediaName->isChecked()) {
		outstring += medianame;
		outstring += separator;
	}


        if ( checkExportMediaNumber->isChecked() ) {
            outstring += QString().setNum(medianumber);
	    outstring += separator;
	}

        if ( checkExportPath->isChecked() ) {
            outstring += getFilePath ( source );
	    outstring += separator;
	}

	if(checkExportSize->isChecked()) {
		tsize = ( ( DBFile * ) ( source->data ) ) ->size;
		switch ( ( ( DBFile * ) ( source->data ) ) ->sizeType ) {
			case 0:                            break;
			case 1:  tsize *= 1024;            break;
			case 2:  tsize *= 1024*1024;       break;
			case 3:  tsize *= 1024*1024*1024;  break;
			case 4:  tsize *= (1024 * 1024)*(1024*1024);  break;
		}
		outstring += QString().setNum ( ( long int ) tsize );
		outstring += separator;
	}

        if ( checkExportDate->isChecked() ) {
            outstring += date_to_str ( ( ( DBFile * ) ( source->data ) ) ->modification );
	    outstring += separator;
        }

        if ( checkExportComment->isChecked() ) {
            if ( ! ( ( ( DBFile* ) ( source->data ) )->comment.isEmpty() ) )
                outstring += QString(( ( DBFile * ) ( source->data ) )->comment).replace("\n", "\\\n");
		outstring += separator;
        }

	 if ( checkExportMp3Tag->isChecked() ) {
		QString Mp3Artist="";
		QString Mp3Title="";
		QString Mp3Album="";
		QString Mp3Year="";
		bool hasMp3Tag=false;
		Node *tmp=NULL;
		tmp= ( ( DBFile * ) ( source->data ) )->prop;
		while ( tmp != NULL ) {
			/*mp3tag*/
			if ( tmp->type == HC_MP3TAG ) {
	// 			cerr << "export: artist: mp3tag found (1)" << endl;
				Mp3Artist = ( ( DBMp3Tag * ) ( tmp->data ) )->artist;
				Mp3Title = ( ( DBMp3Tag * ) ( tmp->data ) )->title;
				Mp3Album = ( ( DBMp3Tag * ) ( tmp->data ) )->album;
				Mp3Year = ( ( DBMp3Tag * ) ( tmp->data ) )->year;
				hasMp3Tag = true;
				break;
			}
			tmp = tmp->next;
		}

// 		cerr << "export: artist: " << qPrintable(Mp3Artist) <<  endl;
// 		cerr << "export: title: " << qPrintable(Mp3Title) <<  endl;
// 		cerr << "export: album: " << qPrintable(Mp3Album) <<  endl;
// 		cerr << "export: year: " << qPrintable(Mp3Year) << endl;

		if(hasMp3Tag) 
			outstring += QString(Mp3Artist)+"/"+QString(Mp3Title)+"/"+QString(Mp3Album)+"/"+QString(Mp3Year);
			outstring += separator;
        }
        if ( checkExportBorrow->isChecked() ) {
            outstring += mediaborrow;
		outstring += separator;
	}
        if ( checkExportCategory->isChecked() ) {
            if ( ! ( ( ( DBFile* ) ( source->data ) )->category.isEmpty() ) )
                outstring += QString(( ( DBFile * ) ( source->data ) )->category).replace("\n", "\\\n");
		outstring += separator;
        }

        outstring += "\n";
    }

    //==== HTML ===========
	if ( radioHtml->isChecked() ) {
		outstring += "<tr>";

		//======== items ==========
		if ( checkExportMediaName->isChecked() )
		outstring += "<td class=\"m\">" + medianame + "</td>";

		if ( checkExportMediaNumber->isChecked() )
		outstring += "<td class=\"m\"  align=\"right\">" + QString().setNum(medianumber)+ "</td>";

		if ( checkExportPath->isChecked() )
		outstring += "<td class=\"f\">" + getFilePath ( source ) + "</td>";

		if ( checkExportSize->isChecked() ) {
		outstring += "<td align=\"right\">";
		outstring += QString().setNum ( ( ( DBFile * ) ( source->data ) ) ->size );
		outstring +=" ";
		outstring += QString ( getSType ( ( ( ( DBFile * ) ( source->data ) ) ->sizeType), true ) );
		outstring += "</td>";
		}

		if ( checkExportDate->isChecked() ) {
		outstring += "<td class=\"d\">";
		outstring += date_to_str ( ( ( DBFile * ) ( source->data ) ) ->modification );
		outstring += "</td>";
		}

		if ( checkExportComment->isChecked() ) {

		outstring += "<td class=\"c\">";
		if ( ! ( ( ( DBFile* ) ( source->data ) )->comment.isEmpty() ) )
			outstring += ( ( DBFile * ) ( source->data ) )->comment
				.replace ( QRegExp ( "#" ),"<br/>\n" );
		else
			outstring += "";
		outstring += "</td>";
		}

		if ( checkExportMp3Tag->isChecked() ) {
			QString Mp3Artist="";
			QString Mp3Title="";
			QString Mp3Album="";
			QString Mp3Year="";
			bool hasMp3Tag=false;
			Node *tmp=NULL;
			tmp= ( ( DBFile * ) ( source->data ) )->prop;
			while ( tmp != NULL ) {
				/*mp3tag*/
				if ( tmp->type == HC_MP3TAG ) {
		// 			cerr << "export: artist: mp3tag found (1)" << endl;
					Mp3Artist = ( ( DBMp3Tag * ) ( tmp->data ) )->artist;
					Mp3Title = ( ( DBMp3Tag * ) ( tmp->data ) )->title;
					Mp3Album = ( ( DBMp3Tag * ) ( tmp->data ) )->album;
					Mp3Year = ( ( DBMp3Tag * ) ( tmp->data ) )->year;
					hasMp3Tag = true;
					break;
				}
				tmp = tmp->next;
			}

	// 		cerr << "export: artist: " << qPrintable(Mp3Artist) <<  endl;
	// 		cerr << "export: title: " << qPrintable(Mp3Title) <<  endl;
	// 		cerr << "export: album: " << qPrintable(Mp3Album) <<  endl;
	// 		cerr << "export: year: " << qPrintable(Mp3Year) << endl;

		outstring += "<td class=\"m\">\n"; 
		if(hasMp3Tag) {
			outstring += "<table>\n";
			outstring += "<tr><td>"+tr("Artist:")+"</td><td>"+QString(Mp3Artist)+"</td></tr>\n";
			outstring += "<tr><td>"+tr("Title:")+"</td><td>"+QString(Mp3Title)+"</td></tr>\n";
			outstring += "<tr><td>"+tr("Album:")+"</td><td>"+QString(Mp3Album)+"</td></tr>\n";
			outstring += "<tr><td>"+tr("Year:")+"</td><td>"+QString(Mp3Year)+"</td></tr>\n";
			outstring += "</table>\n";
		}
		outstring += "</td>\n";
		}
		if ( checkExportBorrow->isChecked() )
		outstring += "<td class=\"m\">" + mediaborrow + "</td>";
		if ( checkExportCategory->isChecked() ) {

		outstring += "<td class=\"c\">";
		if ( ! ( ( ( DBFile* ) ( source->data ) )->comment.isEmpty() ) )
			outstring += ( ( DBFile * ) ( source->data ) )->category
				.replace ( QRegExp ( "#" ),"<br/>\n" );
		else
			outstring += "";
		outstring += "</td>";
		}

		outstring += "</tr>";
	}
    if ( source->child != NULL )
        writeDown ( source->child );

    if ( ( ( DBFile * ) ( source->data ) ) ->prop != NULL )
        writeDown ( ( ( DBFile * ) ( source->data ) ) ->prop );

    if ( source->next != NULL )
        writeDown ( source->next );

    return 0;
}

int exportCdcatDB::writeMp3Tag ( Node* ) {
    return 0;
}

QString exportCdcatDB::getFilePath ( Node *current ) {

    Node * up = current;
    QString a ( "" );
    if ( up != NULL ) {
        while ( up != NULL && QString ( up->getNameOf() ) != medianame ) {
            a.prepend ( up->getNameOf() );
            a.prepend ( "/" );
            up = up->parent;
        }
    }
    return a;

}

void exportCdcatDB::checkExportToggled() {
    if ( !checkExportTitle->isChecked() && !checkExportTableHeader->isChecked() && !checkExportPath->isChecked() &&
            !checkExportMediaName->isChecked() && !checkExportDate->isChecked() && !checkExportSize->isChecked() && !checkExportComment->isChecked() ) {
        buttonOk->setEnabled ( false );
    } else {
        buttonOk->setEnabled ( true );
    }

}


void exportCdcatDB::listAllMediaToggled() {
    if ( checkAllMedia->isChecked() ) {
        listAllMedia->setEnabled ( false );
        listSelectedMedia->setEnabled ( false );
        buttonAdd->setEnabled ( false );
        buttonRemove->setEnabled ( false );
    } else {
        listAllMedia->setEnabled ( true );
        listSelectedMedia->setEnabled ( true );
        buttonAdd->setEnabled ( true );
        buttonRemove->setEnabled ( true );
    }
}

void exportCdcatDB::exportTypeToggled() {
    if ( radioHtml->isChecked() ) {
        seperatorLabel->setEnabled ( false );
        separatorInput->setEnabled ( false );

        checkOnlyMedia->setEnabled ( true );

        checkExportTitle->setEnabled ( true );;
        checkExportTableHeader->setEnabled ( true );;
        checkExportMediaName->setEnabled ( true );
        checkExportMediaNumber->setEnabled ( true );
        checkExportPath->setEnabled ( true );
        checkExportDate->setEnabled ( true );
        checkExportSize->setEnabled ( true );
        checkExportComment->setEnabled ( true );
        checkExportMp3Tag->setEnabled ( true );
        checkExportBorrow->setEnabled ( true );
        checkExportCategory->setEnabled ( true );
        checkOnlyMediaToggled();

        if ( p != NULL )
            fileName->setText ( mainw->cconfig->lastDir +"/"+ ( ( DBCatalog * ) ( ( p->getRootNode() )->data ) )->name+".html" );

    }

    if ( radioCsv->isChecked() ) {
        seperatorLabel->setEnabled ( true );
        separatorInput->setEnabled ( true );
//         checkOnlyMedia->setEnabled ( false );
//         checkExportTitle->setEnabled ( false );
//         checkExportTableHeader->setEnabled ( false );
//         checkExportMediaName->setEnabled ( false );
// 	checkExportMediaNumber->setEnabled ( false );
//         checkExportPath->setEnabled ( false );
//         checkExportDate->setEnabled ( false );
//         checkExportSize->setEnabled ( false );
//         checkExportComment->setEnabled ( false );
//         checkExportMp3Tag->setEnabled ( false );
//         checkExportBorrow->setEnabled ( false );

        if ( p != NULL )
            fileName->setText (mainw->cconfig->lastDir +"/"+ ( ( DBCatalog * ) ( ( p->getRootNode() )->data ) )->name+".csv" );

    }
    if ( radioXml->isChecked() ) {
        seperatorLabel->setEnabled ( false );
        separatorInput->setEnabled ( false );
        checkOnlyMedia->setEnabled ( false );
        checkExportTitle->setEnabled ( false );
        checkExportTableHeader->setEnabled ( false );
        checkExportMediaName->setEnabled ( false );
	checkExportMediaNumber->setEnabled ( false );
        checkExportPath->setEnabled ( false );
        checkExportDate->setEnabled ( false );
        checkExportSize->setEnabled ( false );
        checkExportComment->setEnabled ( false );
        checkExportMp3Tag->setEnabled ( false );
        checkExportBorrow->setEnabled ( false );
        checkExportCategory->setEnabled ( false );
	checkAllMedia->setEnabled(false);
	listSelectedMedia->setEnabled(false);
	listAllMedia->setEnabled(false);

        if ( p != NULL )
            fileName->setText (mainw->cconfig->lastDir +"/"+ ( ( DBCatalog * ) ( ( p->getRootNode() )->data ) )->name+".xml" );

    }
}

void exportCdcatDB::checkOnlyMediaToggled() {
    if ( checkOnlyMedia->isChecked() && radioHtml->isChecked() ) {

        checkExportMediaName->setEnabled ( false );
        checkExportPath->setEnabled ( false );
        checkExportDate->setEnabled ( false );
        checkExportSize->setEnabled ( false );
        checkExportComment->setEnabled ( false );

    } else {

        checkExportMediaName->setEnabled ( true );
        checkExportPath->setEnabled ( true );
        checkExportDate->setEnabled ( true );
        checkExportSize->setEnabled ( true );
        checkExportComment->setEnabled ( true );
    }
}
