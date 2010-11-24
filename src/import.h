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
#include <q3listview.h>
#include <q3filedialog.h>
#include <qwidget.h>
#include <qdatetime.h>

#include <q3progressdialog.h>
#include <expat.h>

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
                 float size, QDateTime datetime );
    ~lineObject();
    lineObject ( const lineObject& );
    lineObject& operator= ( const lineObject& );

    QString getMediaName();
    QString getPath();
    QString getFileName();
    float getSize();
    QDateTime getDateTime();

protected:
    QString medianame;
    QString path;
    QString filename;
    QDateTime datetime;
    float size;
};


class importGtktalogCsv: public QObject {
    Q_OBJECT
public:
    importGtktalogCsv ( GuiSlave * parent, QString separator, QString filename, bool createdatabase, bool correctbadstyle );
    ~importGtktalogCsv();

    /*
      * @param medianame Name of media @param path Full path of file.
      * Example: folder1/subfolder/subsubfolder/file @param size Size of
      * file @param datetime Date + time of file
      */
    int addNewItem ( QString medianame, QString path, QString filename,
                     float size, QDateTime datetime );

    /*
     * @param new_medianame Name of new media
     */

    int addNewMedia ( QString new_medianame, QList < lineObject > *medialines );

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


class importGtktalogXml : public QObject {
Q_OBJECT public:
    importGtktalogXml ( GuiSlave * parent, QString filename, bool createdatabase );
    ~importGtktalogXml();

    /*
     * @param medianame Name of media @param path Full path of file.
     * Example: folder1/subfolder/subsubfolder/file @param size Size of
     * file @param datetime Date + time of file
     */
    int addNewItem ( QString medianame, QString path, QString filename,
                     float size, QDateTime datetime );

    /*
     * @param new_medianame Name of new media
     */
#if QT_VERSION >= 0x030100 // Qt 3.1.0 or better
    int addNewMedia ( QString new_medianame, QList < lineObject > *medialines );
#else
    int addNewMedia ( QString new_medianame, QList < lineObject > *medialines );
#endif

    int lines;
    int linecount;
    int mediacount;
    int filecount;
    int dircount;
    int refreshcount;
    QString fullpath;
    QString path;
    QString filename;
    float size;
    QDate date;
    QString new_medianame;
    QString medianame;
    QString datetimestring;
    QString line;
    QString catalogName;
    QString categorie;
    QString description;
    QString information;
    QDateTime datetime;
    QString directory;
    XML_Parser parser;
    QString last_tag;
    GuiSlave * guislave;
    QList < lineObject > *medialines;
    Q3ProgressDialog *progress;

protected:
    DataBase *db;
    bool createdatabase;

};

enum tag_type {empty, media, file, folder};

class importWhereIsItXml : public QObject {
Q_OBJECT public:
    importWhereIsItXml ( GuiSlave * parent, QString filename, bool createdatabase );
    ~importWhereIsItXml();

    void setProgress();

//	int lines;
    int linecount;
    int mediacount;
    int filecount;
    int dircount;
    int refreshcount;
    QString fullpath;
    QString path;
    QString filename;
    float size;
    QDate date;
    QString new_medianame;
    QString medianame;
    int number;
    int filenumber;
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
    XML_Parser parser;
    QString last_tag;
    tag_type last_type;
    GuiSlave * guislave;
    Q3ProgressDialog *progress;
    Node *last_upper_container_node;
    DataBase *db;
protected:

    bool createdatabase;

};

class import : public QObject {
    Q_OBJECT

public:
    import ( GuiSlave * parent );

protected:

};

#endif
