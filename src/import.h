/****************************************************************************
                         Hyper's CD Catalog
A multiplatform qt and xml based catalog program

Author    : Christoph Thielecke (crissi99@gmx.de)
License   : GPL
Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/

#ifndef IMPORT__
#define IMPORT__


#include <qobject.h>
#include <qfiledialog.h>
#include <qwidget.h>
#include <qdatetime.h>

#include <qprogressdialog.h>

#include <QtXml/QXmlDefaultHandler>
#include <QtXml/QXmlLocator>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "guibase.h"
#include "dbase.h"


#include <qlist.h>


class Node;
class CdCatMainWidget;

class lineObject: public QObject {
    Q_OBJECT
public:

    /*
     * @param medianame Name of media @param path Full path of file.
     * Example: folder1/subfolder/subsubfolder/file @param size Size of
     * file @param date Date of file @param time Mod time of file
     */
    lineObject ( QString medianame, QString path, QString filename,
                 double size, QDateTime datetime, QString comment, QString information, QString category, QList<ArchiveFile> archivecontent = QList<ArchiveFile>(), bool is_archive=false  );
    ~lineObject();
    lineObject ( const lineObject& );
    lineObject& operator= ( const lineObject& );

    QString getMediaName();
    QString getPath();
    QString getFileName();
    double getSize();
    QDateTime getDateTime();
    QString getComment();
    QString getInformation();
    QString getCategory();
    QList<ArchiveFile> getArchiveFileContent();
    bool isArchive();
    void setArchiveFileContent(QList<ArchiveFile> archivecontent);
    void appendArchiveFileContent(ArchiveFile af);

protected:
    QString medianame;
    QString path;
    QString filename;
    QDateTime datetime;
    QString comment;
    QString information;
    QString category;
    
    double size;
    bool is_archive;
public:
	QList<ArchiveFile> archivecontent;
};


class importGtktalogCsv: public QObject {
    Q_OBJECT
public:
    importGtktalogCsv ( GuiSlave * parent, QString separator, QString filename, bool createdatabase, bool correctbadstyle, QString csvtype="gtktalog" );
    ~importGtktalogCsv();

    /*
      * @param medianame Name of media @param path Full path of file.
      * Example: folder1/subfolder/subsubfolder/file @param size Size of
      * file @param datetime Date + time of file
      */
    int addNewItem ( QString medianame, QString path, QString filename,
                     double size, QDateTime datetime, QString comment, QString category );

    /*
     * @param new_medianame Name of new media
     */

    int addNewMedia ( QString new_medianame, QDateTime media_modification, QString media_comment, QString media_category, QList < lineObject > *medialines );

protected:
    GuiSlave * guislave;
    QList < lineObject > *medialines;
    DataBase *db;
    int mediacount;
    int filecount;
    int dircount;
    int refreshcount;
    bool createdatabase;
    bool correctbadstyle;
    QString filename;
    QString separator;

};


class importGtktalogXml : public QObject, public QXmlDefaultHandler {
Q_OBJECT public:
    importGtktalogXml ( GuiSlave * parent, QString filename, bool createdatabase );
    ~importGtktalogXml();

    /*
     * @param medianame Name of media @param path Full path of file.
     * Example: folder1/subfolder/subsubfolder/file @param size Size of
     * file @param datetime Date + time of file
     */
    int addNewItem ( QString medianame, QString path,
                                    QString filename, long size, QDateTime datetime, QString comment, QString information, QString category,  QList<ArchiveFile> archivecontent = QList<ArchiveFile>(), bool is_archive=false );

    /*
     * @param new_medianame Name of new media
     */
    int addNewMedia ( QString new_medianame, QDateTime media_modification, QString media_comment, QString media_category, QList < lineObject > *medialines );

    int lines;
    int linecount;
    int mediacount;
    int filecount;
    int dircount;
    int refreshcount;
    QString fullpath;
    QString path;
    QString filename;
    double size;
    QDate date;
    QString new_medianame;
    QString medianame;
    QString datetimestring;
    QString line;
    QString catalogName;
    QString category;
    QString description;
    QString comment;
    QString information;
    QDateTime datetime;
    QString directory;
    QString last_tag;
    QString tag_content;
    GuiSlave * guislave;
    QList < lineObject > *medialines;
    QProgressDialog *progress;
	bool startDocument();
	bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
	bool endElement( const QString&, const QString&, const QString& );
	bool characters ( const QString & ch ) ;
	bool fatalError(const QXmlParseException &exception);
	void setDocumentLocator ( QXmlLocator * locator );
	QXmlLocator *locator;
	QXmlAttributes attr;
	bool is_directory;
	bool is_archive;

	protected:
protected:
    DataBase *db;
    bool createdatabase;
    QString currentText;

};

//enum tag_type {empty, media,file,folder};

class importWhereIsItXml : public QObject, public QXmlDefaultHandler {
Q_OBJECT
public:
    importWhereIsItXml ( GuiSlave * parent, QString filename, bool createdatabase );
    ~importWhereIsItXml();


//	int lines;
    int linecount;
    int mediacount;
    int filecount;
    int dircount;
    int refreshcount;
    int tagcount;
    int number;
    int filenumber;
    int last_media_type;
    double size;
    QString fullpath;
    QString path;
    QString filename;
    QString new_medianame;
    QString medianame;
    QString datetimestring;
    QString line;
    QString folder;
    QString file;
    QString catalogName;
    QString categorie;
    QString description;
    QString information;
    QDateTime datetime;
    QString directory;
    QString comment;
    QString last_tag;
    QString last_type;
    GuiSlave * guislave;
    QProgressDialog *progress;
    Node *last_upper_container_node;
    DataBase *db;
    QDate date;
		//tag_type last_type;
protected:


		bool startDocument();
		bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
		bool endElement( const QString&, const QString&, const QString& );
		bool characters ( const QString & ch ) ;
		bool fatalError(const QXmlParseException &exception);
		void setDocumentLocator ( QXmlLocator * locator );
		QXmlLocator *locator;
		QXmlAttributes attr;
	
	protected:
		bool createdatabase;
		QString currentText;

};

class import : public QObject {
    Q_OBJECT

public:
    import ( GuiSlave * parent );

protected:

};

#endif
