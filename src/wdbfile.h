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
#include <expat.h>
#include <zlib.h>
//Added by qt3to4:
#include <Q3CString>

class Node;
class PWw;
class QString;
class Q3CString;
class QTextCodec;
class QDateTime;

const char *getSType ( int t );
const char *getMType ( int t );
float       getSizeFS ( const char *str );
int         getSizetFS ( const char *str );
int         getTypeFS ( const char *str );


class FileWriter {
private:
    inline char * to_cutf8 ( QString   s );
    inline char * to_dcutf8 ( QDateTime d );

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
    QString get_cutf8 ( char *s );
    QDateTime get_dcutf8 ( char *s );
    QString XML_ENCODING;
private:
    gzFile f;

    QTextCodec *converter;


public:

    PWw *pww;
    QString errormsg;
    int  error;
    int  insert;

    long long int allocated_buffer_len;

    Node *sp;
    char *dataBuffer;
    

    int      readFrom ( Node *source );
    float    getFloat ( const char **from,char *what,char *err );
    char *   getStr ( const char **from,char *what,char *err );
    int      isthere ( const char **from,char *what );

    FileReader ( gzFile ff, char *allocated_buffer, long long int allocated_buffer_len, int ins = 0 );

    QString getCatName ( void );
};


#endif
