/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCAT_WDBFILE
#define CDCAT_WDBFILE

#include <stdio.h>
#include <zlib.h>
//Added by qt3to4:
#include <Q3CString>

#include <QtXml/QXmlDefaultHandler>
#include <QtXml/QXmlLocator>

class Node;
class PWw;
class QString;
class Q3CString;
class QTextCodec;
class QDateTime;

QString getSType ( int t, bool localized=false );
const char *getMType ( int t );
float       getSizeFS ( const char *str );
int         getSizetFS ( const char *str );
int         getTypeFS ( const char *str );


class FileWriter {
private:
    inline QString to_cutf8 ( QString   s );
    inline QString to_dcutf8 ( QDateTime d );

    QTextCodec *converter;
    gzFile f;
    int level;

public:
    FileWriter ( gzFile ff,bool nicefp, QString encoding );
    ~FileWriter();

    PWw *pww;
    int writeDown ( Node *source );
    bool nicef;

private:
    char **spgtable;
    inline char * spg ( int spn );
    void commentWriter ( QString& c );
    void categoryWriter ( QString& c );

    int  writeHeader ( void );
    int  writeCatalog ( Node *source );
    int  writeMedia ( Node *source );
    int  writeDirectory ( Node *source );
    int  writeFile ( Node *source );
    int  writeMp3Tag ( Node *source );
    int  writeContent ( Node *source );
    int  writeCatLnk ( Node *source );
    QString XML_ENCODING;
};


class FileReader {
public:
    QString get_cutf8 ( QString s );
    QDateTime get_dcutf8 ( QString s );
    QString XML_ENCODING;
private:
    gzFile f;

    QTextCodec *converter;


public:

    PWw *pww;
    QString errormsg;
    int  error;
    int  insert;
    int linecount;
    long long int allocated_buffer_len;
    

    Node *sp;
    char *dataBuffer;
    

    int      readFrom ( Node *source );
    QString getStr2(const QXmlAttributes &atts,char *what,char *err );
    float getFloat2 (const QXmlAttributes &atts,char *what,char *err );
    int      isthere ( const char **from,char *what );
    FileReader ( gzFile ff, char *allocated_buffer, long long int allocated_buffer_len, int ins = 0 );
    QXmlSimpleReader xmlReader;

    QString getCatName ( void );



};

class CdCatXmlHandler : public QXmlDefaultHandler {
	public:
		CdCatXmlHandler(FileReader *r, bool onlyCatalog=false);
		~CdCatXmlHandler();
		bool characters(const QString & ch);
		bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName );
		bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
		bool fatalError(const QXmlParseException &exception);
		void setDocumentLocator ( QXmlLocator * locator );
		void setPww(PWw *pww);
	private:
		FileReader *r;
		QString currentText;
		bool onlyCatalog;
		QXmlLocator *locator;
		long long int lines;
		PWw *pww;
};



#endif
