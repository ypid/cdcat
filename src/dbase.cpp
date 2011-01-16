/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <qdatetime.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <zlib.h>
#include <pcre.h>

#include "dbase.h"
#include "cdcat.h"
#include "mp3tag.h"
#include "wdbfile.h"
#include "adddialog.h"

#include "tparser.h"

#include <iostream>
using namespace std;


QString date_to_str ( QDateTime dt ) {

    QString text;
    if ( dt.isValid() )
        text.sprintf ( "%d-%02d-%02d %02d:%02d:%02d",dt.date().year(), dt.date().month(),  dt.date().day(),
                   dt.time().hour(), dt.time().minute(), dt.time().second() );
    else
        text = DataBase::tr( "Not available" );

    return text;
}

/***************************************************************************/
void caseSensConversion ( char *p ) {
    int s,t,ss = strlen ( p );
    char *tmp=new char[2048];

    for ( s=0,t=0;s<=ss;s++,t++ ) {
        if ( ( p[s] >= 'a' && p[s]<='z' ) ||
                ( p[s] >= 'A' && p[s]<='Z' ) ) {
            tmp[t++]='[';
            tmp[t++]=tolower ( p[s] );
            tmp[t++]='|';
            tmp[t++]=toupper ( p[s] );
            tmp[t  ]=']';
        } else {
            tmp[t]=p[s];
        }
    }
    strcpy ( p,tmp );
    delete [] tmp;
}

/***************************************************************************/
void easyFormConversion ( char *p ) {
    int s,t,ss = strlen ( p );
    char *tmp=new char[2048];

    for ( s=0,t=0;s<=ss;s++,t++ ) {
        if ( p[s] == '?' ) {
            tmp[t  ]='.';
        } else if ( p[s] == '*' ) {
            tmp[t++]='.';
            tmp[t  ]='*';
        } else if ( ( p[s] >= 'a' && p[s]<='z' ) ||
                    ( p[s] >= 'A' && p[s]<='Z' ) ||
                    ( p[s] >= '0' && p[s]<='9' ) ) {
            tmp[t]=p[s];
        } else if ( p[s] == '\0' ) {
            tmp[t]='\0';
        } else {
            tmp[t++]='\\';
            tmp[t  ]=p[s];
        }
    }
    sprintf ( p,"^%s$",tmp );
    delete [] tmp;
}

/***************************************************************************

   DataBase Class

***************************************************************************/

Node::Node ( void ) {
    type   = HC_UNINITIALIZED;
    next   = NULL;
    child  = NULL;
    parent = NULL;
    data   = NULL;
}

Node::Node ( int t,Node *p ) {
    type   = t;
    next   = NULL;
    child  = NULL;
    parent = p;
    data   = NULL;
}

Node::~Node ( void ) {
    //if(child != NULL) delete child;
    if ( next  != NULL ) delete next;
    if ( data  != NULL ) {
        switch ( type ) {
        case HC_UNINITIALIZED: break;
        case HC_CATALOG      : delete ( ( DBCatalog   * ) data ); break;
        case HC_MEDIA        : delete ( ( DBMedia     * ) data ); break;
        case HC_DIRECTORY    : delete ( ( DBDirectory * ) data ); break;
        case HC_FILE         : delete ( ( DBFile      * ) data ); break;
        case HC_MP3TAG       : delete ( ( DBMp3Tag    * ) data ); break;
        case HC_CONTENT      : delete ( ( DBContent   * ) data ); break;
        case HC_CATLNK       : delete ( ( DBCatLnk    * ) data ); break;
        }
    }
    child=next=NULL;
    data=NULL;
}

QString Node::getFullPath ( void ) {
    Node *up=parent;
    QString a ( "" );
    up=this;
    while ( up != NULL ) {
        a.prepend ( up->getNameOf() );
        a.prepend ( "/" );
        up=up->parent;
    }
    a.prepend ( "/" );
    return a;
}

void Node::touchDB ( void ) {
    if ( type != HC_CATALOG )
        parent->touchDB();
    else
        ( ( DBCatalog * ) data )->touch();
}


QString Node::getNameOf ( void ) {
    switch ( type ) {
    case HC_UNINITIALIZED: return QString ( "" );
    case HC_CATALOG      : return ( ( DBCatalog   * ) data )->name;
    case HC_MEDIA        : return ( ( DBMedia     * ) data )->name;
    case HC_DIRECTORY    : return ( ( DBDirectory * ) data )->name;
    case HC_FILE         : return ( ( DBFile      * ) data )->name;
    case HC_MP3TAG       : return QString ( "" );
    case HC_CONTENT      : return QString ( "" );
    case HC_CATLNK       : return ( ( DBCatLnk    * ) data )->name;
    }
    return QString ( "" );
}



DBCatalog::DBCatalog ( QString n,QString o,QString c ) {
    name    = n;
    owner   = o;
    comment = c;
    writed  = 1;
    strcpy ( filename,"" );
    fileversion = "";
    modification = QDateTime::currentDateTime();
}

DBCatalog::DBCatalog ( QString n,QString o,QString c,QDateTime mod ) {
    name    = n;
    owner   = o;
    comment = c;
    writed  = 1;
    strcpy ( filename,"" );
    fileversion = "";
    modification = mod;
}

DBCatalog::DBCatalog ( void ) {
    name    = "";
    owner   = "";
    comment = "";
    writed  = 1;
    strcpy ( filename,"" );
    modification = QDateTime::currentDateTime();
    fileversion  = "";
}

DBCatalog::~DBCatalog ( void ) {
}

DBMedia::DBMedia ( QString n,int nu,QString o,int t,QString c ) {
    name   = n;
    number = nu;
    owner  = o;
    type   = t;
    modification = QDateTime::currentDateTime();
    comment = c;
    borrowing = "";
}

DBMedia::DBMedia ( QString n,int nu,QString o,int t,QString c,QDateTime mod ) {
    name    = n;
    number  = nu;
    owner   = o;
    type    = t;
    modification = mod;
    comment = c;
    borrowing = "";
}

DBMedia::DBMedia ( void ) {
    name      = "";
    number    = 0;
    owner     = "";
    type      = 0;
    modification = QDateTime::currentDateTime();
    comment   = "";
    borrowing = "";
}

DBMedia::~DBMedia ( void ) {
}


DBDirectory::DBDirectory ( QString n,QDateTime mod,QString c ) {
    name         = n;
    modification = mod;
    comment      = c;
}

DBDirectory::DBDirectory ( void ) {
    name    = "";
    modification = QDateTime();
    comment = "";
}

DBDirectory::~DBDirectory ( void ) {
}



DBFile::DBFile ( QString n,QDateTime mod,QString c,float s,int st ) {
    name    = n;
    modification= mod;
    comment = c;
    size    = s;
    sizeType= st;
    prop    = NULL;
}

DBFile::DBFile ( void ) {
    name    = "";
    modification = QDateTime();
    comment = "";
    size    = 0;
    sizeType= 0;
    prop    = NULL;
}

DBFile::~DBFile ( void ) {
    if ( prop    != NULL ) delete prop;
}

DBMp3Tag::DBMp3Tag ( void ) {
    artist  = "";
    title   = "";
    comment = "";
    album   = "";
    year    = "";
}

DBMp3Tag::DBMp3Tag ( QString a,QString t,QString c,QString al,QString y ) {
    artist  = a;
    title   = t;
    comment = c;
    album   = al;
    year    = y;

}

DBMp3Tag::~DBMp3Tag ( void ) {
}

DBContent::DBContent ( void ) {
    bytes      = NULL;
    storedSize = 0;
}

DBContent::DBContent ( unsigned char *pbytes,unsigned long  pstoredSize ) {
    bytes      = pbytes;
    storedSize = pstoredSize;
}

DBContent:: ~DBContent ( void ) {
    if ( bytes != NULL ) {
        delete [] bytes;
        bytes = NULL;
    }
    storedSize = 0;
}

DBCatLnk::DBCatLnk ( QString pname,char *plocation,QString pcomment ) {
    name      = pname;
    location  = mstr ( plocation );
    comment   = pcomment;
}

DBCatLnk::DBCatLnk ( void ) {
    location = NULL;
}

DBCatLnk::~DBCatLnk ( void ) {
    if ( location != NULL ) delete[] location;
}

DataBase::DataBase ( void ) {
    nicef           = true;
    errormsg        = "";
    pww             = NULL;
    storeMp3tags    = true;
    storeContent    = true;
    storedFiles     = "*.nfo;*.diz;readme.txt";
    storeLimit      = 32*1024;
    root            = new Node ( HC_CATALOG,NULL );
    root->data      = ( void * ) new DBCatalog();
    XML_ENCODING ="UTF-8";
}

DataBase::~DataBase ( void ) {
    delete root;
}


void DataBase::setDBName ( QString n ) {
    ( ( DBCatalog * ) ( root->data ) )->name = n;
}

void DataBase::setDBOwner ( QString o ) {
    ( ( DBCatalog * ) ( root->data ) )->owner = o;
}

void DataBase::setComment ( QString c ) {
    ( ( DBCatalog * ) ( root->data ) )->comment = c;
}

QString& DataBase::getDBName ( void ) {
    return ( ( DBCatalog * ) ( root->data ) )->name;
}

QString& DataBase::getDBOwner ( void ) {
    return ( ( DBCatalog * ) ( root->data ) )->owner;
}

QString& DataBase::getComment ( void ) {
    return ( ( DBCatalog * ) ( root->data ) )->comment;
}

/***************************************************************************/
char *pattern;
int DataBase::addMedia ( QString what,QString name,int number,int type ) {
    return addMedia ( what,name,number,type,"" );
}

int DataBase::addMedia ( QString what,QString name,int number,int type,QString owner ) {
    int returnv=0;
    Node *tt=root->child;

    progress ( pww );
    ( ( DBCatalog * ) ( root->data ) )->touch();
    if ( root->child == NULL ) root->child = tt = new Node ( HC_MEDIA,root );
    else {
        while ( tt->next != NULL ) {
            tt = tt->next;
        }
        tt->next = new Node ( HC_MEDIA,root );
        tt = tt->next;
    }

    progress ( pww );
    /* Fill the media Node (tt) */
    tt->data = ( void * )
               new DBMedia ( name,
                             number,
                             owner.isEmpty() ? ( ( DBCatalog * ) ( root->data ) )->owner : owner,
                             type,
                             QString ( "" ) );
    progress ( pww );

    /* make the regex pattern for storecontent */

    progress ( pww );
    pattern = new char[1024];
    returnv = scanFsToNode ( what,tt );
    delete [] pattern;
    return returnv;
}
/***************************************************************************/
int   DataBase::saveDB ( void) {
    int i;
    gzFile f=NULL;
    FileWriter *fw = NULL;

    progress ( pww );

    if ( strcmp ( ( ( DBCatalog * ) ( root->data ) )->filename,"" ) == 0 ) return 1;

    f = gzopen ( ( ( DBCatalog * ) ( root->data ) )->filename,"wb" );
    if ( f==NULL ) {
        errormsg = tr ( "I can't rewrite the file: %1" ).arg ( ( ( DBCatalog * ) ( root->data ) )->filename );
        return 2;
    }
    progress ( pww );

    fw = new FileWriter ( f,nicef, this->XML_ENCODING);
    fw->pww = pww;
    progress ( pww );
    i=fw->writeDown ( root );
    ( ( DBCatalog * ) ( root->data ) )->writed = 1;
    progress ( pww );
    gzclose ( f );
    delete fw;
    return 0;
}
/***************************************************************************/
int   DataBase::saveAsDB ( char *filename ) {
    int i;
    gzFile f=NULL;
    FileWriter *fw = NULL;

    progress ( pww );
    /*Check overwriting !!! */
    f = gzopen ( filename,"wb" );
    if ( f==NULL ) {
        errormsg = tr ( "I can't create the file: %1" ).arg ( filename );
        return 1;
    }

    progress ( pww );
    fw = new FileWriter ( f,nicef, this->XML_ENCODING );
    fw->pww = pww;
    i=fw->writeDown ( root );
    ( ( DBCatalog * ) ( root->data ) )->writed = 1;
    strcpy ( ( ( DBCatalog * ) ( root->data ) )->filename,filename );

    progress ( pww );
    gzclose ( f );
    delete fw;
    return 0;
}
/***************************************************************************/
int   DataBase::insertDB ( char *filename ) {
    int i;
    gzFile f=NULL;
    FileReader *fw = NULL;


    if ( root==NULL ) {
        errormsg = tr ( "No database opened!" );
        return 1;
    }
    progress ( pww );
    f = gzopen ( filename,"rb" );
    if ( f==NULL ) {
        errormsg = tr ( "I can't open the file: %1" ).arg ( filename );
        return 1;
    }

    progress ( pww );
    fw = new FileReader ( f,1 );

    fw->pww=pww;
    progress ( pww );

    i=fw->readFrom ( root );

    if ( i==1 ) {
        progress ( pww );
        errormsg = fw->errormsg;
        // cerr <<"error:"<< fw->errormsg <<endl;
        delete fw;
        gzclose ( f );
        return 1;
    }

    ( ( DBCatalog * ) ( root->data ) )->touch();
    progress ( pww );

    gzclose ( f );
    delete fw;
    return 0;
}

/***************************************************************************/
int   DataBase::openDB ( char *filename ) {
    int i;
    gzFile f=NULL;
    FileReader *fw = NULL;

    progress ( pww );
    f = gzopen ( filename,"rb" );
    if ( f==NULL ) {
        errormsg = tr ( "I can't open the file: %1" ).arg ( filename );
        return 1;
    }

    progress ( pww );
    fw = new FileReader ( f );

    fw->pww=pww;
    progress ( pww );

    if ( root != NULL )
        delete root; //Free previous database in memory

    progress ( pww );
    root = new Node ( HC_CATALOG,NULL ); //Malloc root node.

    root->data = ( void * ) new DBCatalog();

    progress ( pww );
    i=fw->readFrom ( root );
    this->XML_ENCODING = fw->XML_ENCODING;


    if ( i==1 ) {
        progress ( pww );
        errormsg = fw->errormsg;
        // cerr <<"error:"<< fw->errormsg <<endl;
        delete root;
        root = NULL;
        delete fw;
        gzclose ( f );
        return 1;
    }


    ( ( DBCatalog * ) ( root->data ) )->writed = 1;
    strcpy ( ( ( DBCatalog * ) ( root->data ) )->filename,filename );

    progress ( pww );

    gzclose ( f );
    delete fw;
    return 0;
}
/***************************************************************************/
void DBCatalog::touch ( void ) {
    writed = 0;
    modification=QDateTime::currentDateTime();
}
/*************************************************************************/
int DataBase::scanFsToNode ( QString what,Node *to ) {
    cerr <<"Loading node:"<< qPrintable ( what ) <<endl;

    int ret;
    QString comm=NULL;
    QDir *dir=NULL;
    QFileInfoList *dirlist = NULL;

    ret=0;
    dir = new QDir ( what );
    if ( !dir->isReadable() ) {
        cerr << "dir " << qPrintable ( what ) << " is not readable" << endl;
        int i;
        if ( QFileInfo ( what ).isDir() )
            errormsg = tr ( "Cannot read directory: %1" ).arg ( what );
        else
            errormsg = tr ( "Cannot read file: %1" ).arg ( what ); /* socket files and dead symbolic links end here */

        i = 1 + ( QMessageBox::warning ( NULL,tr ( "Error" ),errormsg,tr ( "Ignore" ),tr ( "Cancel scanning" ) ) );
        return i;
    }
    dirlist = new QFileInfoList ( dir->entryInfoList ( QString ( "*" ), QDir::All|QDir::Hidden|QDir::System ) );

    for ( int fi = 0; fi < dirlist->size(); ++fi ) {
        QFileInfo *fileInfo = new QFileInfo ( dirlist->at ( fi ) );
        if ( fileInfo->fileName() == "." || fileInfo->fileName() == ".." ) {
            continue;
        }
        cerr << "processing in dir " << qPrintable ( what ) << " node: " << qPrintable ( fileInfo->filePath() ) << endl;

        /* Make a new node */
        Node *tt=to->child;
        if ( to->child == NULL )
            to->child = tt = new Node ( fileInfo->isDir() ? HC_DIRECTORY : HC_FILE,to );
        else {
            while ( tt->next != NULL ) tt = tt->next;
            tt->next = new Node ( fileInfo->isDir() ? HC_DIRECTORY : HC_FILE,to );
            tt = tt->next;
        }
        /*Fill the data field */
        if ( fileInfo->isFile() ) { /* FILE */
            cerr << "adding file: " << qPrintable ( fileInfo->fileName() ) << endl;
            uint size = fileInfo->size();
            float s;
            int   st;

            if ( size > ( uint ) ( 1024*1024*1024*2 ) ) {
                s  = ( double ) size / ( double ) ( 1024*1024*1024 );
                st = GBYTE;
            } else if ( size > ( uint ) ( 1024*1024 ) ) {
                s  = ( double ) size / ( double ) ( 1024*1024 );
                st = MBYTE;
            } else if ( size > ( uint ) 1024 ) {
                s  = ( double ) size / ( double ) 1024;
                st = KBYTE;
            } else {
                s  = size;
                st = BYTE;
            }

            progress ( pww );

            if ( fileInfo->isSymLink() ) { /* SYMBOLIC LINK to a FILE */
                comm = tr ( "Symbolic link to file:#" )
                            + dir->relativeFilePath(fileInfo->symLinkTarget());
            } else {
                comm =(char*) NULL;
            }
            tt->data= ( void * ) new DBFile ( fileInfo->fileName(),fileInfo->lastModified(),
                                              comm,s,st );
            scanFileProp ( fileInfo, ( DBFile * ) tt->data );
        } else if ( fileInfo->isDir() ) { /* DIRECTORY */
            cerr << "adding dir: " << qPrintable ( fileInfo->fileName() ) << endl;
            progress ( pww );

            if ( fileInfo->isSymLink() ) { /* SYMBOLIC LINK to a DIRECTORY */
                /* These links appear as empty directories in the GUI */
                /* Change to DBFile for show them as files */
                tt->data= ( void * ) new DBDirectory (
                          fileInfo->fileName(), fileInfo->lastModified(),
                          tr ( "Symbolic link to directory:#" )
                               + dir->relativeFilePath(fileInfo->symLinkTarget()) );
                continue; /* Do not recurse into symbolically linked directories */
            } else {
                tt->data= ( void * ) new DBDirectory (
                          fileInfo->fileName(), fileInfo->lastModified(), (char*) NULL);
            }

            /* Start recursion: */

            QString thr ( what );
            thr=thr.append ( "/" );
            thr=thr.append ( fileInfo->fileName() );

            if ( ( ret=scanFsToNode ( thr,tt ) ) == 2 )
                return ret;

        } else if ( fileInfo->isSymLink() ) { /* DEAD SYMBOLIC LINK */
            cerr << "adding dead symlink: " << qPrintable ( fileInfo->fileName() ) << endl;
            progress ( pww );

            comm = tr ( "DEAD Symbolic link to:#" )
                        + dir->relativeFilePath(fileInfo->symLinkTarget());
            tt->data=( void * ) new DBFile ( fileInfo->fileName(),QDateTime(),
                                          comm,0,BYTE );
        } else { /* SYSTEM FILE (e.g. FIFO, socket or device file) */
            cerr << "adding system file: " << qPrintable ( fileInfo->fileName() ) << endl;
            progress ( pww );

            comm = tr( "System file (e.g. FIFO, socket or device file)" );
            tt->data=( void * ) new DBFile ( fileInfo->fileName(),fileInfo->lastModified(),
                                          comm,0,BYTE );
        }
    }/*end of for,..next directory entry*/
    return ret;
}
/***************************************************************************/

int DataBase::scanFileProp ( QFileInfo *fi,DBFile *fc ) {
    /***MP3 tag scanning */

    if ( storeMp3tags || storeMp3techinfo )
        if ( ( fi->extension ( FALSE ) ).lower() ==  "mp3" ||
                ( fi->extension ( FALSE ) ).lower() ==  "mp2" ) {
            ReadMp3Tag *reader =
                new ReadMp3Tag ( ( const char * ) QFile::encodeName ( fi->absFilePath() ),v1_over_v2 );
            if ( storeMp3tags )
                if ( reader->readed() && reader->exist() ) {
                    Node *tt = fc->prop;
                    if ( tt == NULL ) fc->prop = tt = new Node ( HC_MP3TAG,NULL );
                    else {
                        while ( tt->next != NULL ) tt = tt->next;
                        tt->next = new Node ( HC_MP3TAG,fc->prop );
                        tt = tt->next;
                    }
                    /*Fill the fields:*/
                    tt->data = ( void * ) new DBMp3Tag ( QString::fromLocal8Bit ( reader->artist() ) ,
                                                         QString::fromLocal8Bit ( reader->title() )  ,
                                                         QString::fromLocal8Bit ( reader->comment() ),
                                                         QString::fromLocal8Bit ( reader->album() )  ,
                                                         QString::fromLocal8Bit ( reader->year() ) );

                }//storetag-if

            // Put some technical info to comment
            if ( storeMp3techinfo ) {
                char *info=reader->gettechinfo();
                if ( info != NULL ) {
                    if ( ! fc->comment.isEmpty() )
                        fc->comment.append ( "#" );
                    fc->comment.append ( info );
                    free ( info );
                }
            }//storeinfo-if

            if ( reader != NULL ) {
                delete reader;
                reader = NULL;
            }
        }

    /***Experimental AVI Header Scanning  */
    if ( storeAvitechinfo )
        if ( ( fi->extension ( FALSE ) ).lower() == "avi" ) {
            FILE* filePTR;
            filePTR=fopen ( ( const char * ) QFile::encodeName ( fi->absFilePath() ),"r" );
            if ( filePTR!=NULL ) {
                QString got = parseAviHeader ( filePTR ).replace ( QRegExp ( "\n" ),"#" );
                fclose ( filePTR );

                //store it as comment
                if ( !got.isEmpty() ) {
                    if ( !fc->comment.isEmpty() )
                        fc->comment.append ( "#" );
                    fc->comment.append ( got );
                }
            }
        }

    /***File content scanning */
    if ( storeContent ) {
        pcre       *pcc = NULL;
        const char *error;
        int         erroroffset;
        int         ovector[30];
        bool match = false;

        QStringList exts ( QStringList::split ( ";",storedFiles ) );
        QStringList::Iterator it = exts.begin();

        for ( ;it != exts.end(); ++it ) { // stepping on the ; separated patterns
            strcpy ( pattern, ( const char * ) ( *it ) );
            easyFormConversion ( pattern );
            caseSensConversion ( pattern );
            pcc   = pcre_compile ( pattern,0,&error,&erroroffset,NULL );
            if ( 1 == pcre_exec ( pcc,NULL, ( const char * ) QFile::encodeName ( fi->fileName() )
                                  ,strlen ( ( const char * ) QFile::encodeName ( fi->fileName() ) )
                                  ,0,0,ovector,30 ) ) {
                match = true;
                break;
            }
        }

        if ( match ) { // the file need to be read
            FILE *f;
            bool success = true;
            unsigned long rsize=0,rrsize;
            unsigned char *rdata=0;
            Node *tt = fc->prop;

            if ( storeLimit > MAX_STORED_SIZE )
                storeLimit = MAX_STORED_SIZE;
            //read the file
            if ( ( rsize = fi->size() ) > storeLimit )
                rsize = storeLimit;
            f = fopen ( ( const char * ) QFile::encodeName ( fi->absFilePath() ),"rb" );
            if ( f == NULL ) {
                errormsg = QString ( "I couldn't open the \"%1\" file. (content read 1)\n" )
                           .arg ( fi->absFilePath() );
                fprintf ( stderr, ( const char * ) errormsg );
                success = false;
            }

            rdata = new unsigned char[rsize + 1];
            fseek ( f,0,SEEK_SET );
            rrsize = fread ( rdata,sizeof ( unsigned char ),rsize,f );
            if ( rsize != rrsize ) {
                errormsg = QString ( "I couldn't correctly read the content of file \"%1\" . (content read 2)\n"
                                     "size difference %2 != %3\n" )
                           .arg ( fi->absFilePath() )
                           .arg ( rsize )
                           .arg ( rrsize );
                fprintf ( stderr, ( const char * ) errormsg );
                success = false;
            }

            fclose ( f );
            rdata[rsize] = '\0';

            //make the node in the db
            if ( success ) {
                if ( tt == NULL ) fc->prop = tt = new Node ( HC_CONTENT,NULL );
                else {
                    while ( tt->next != NULL ) tt = tt->next;
                    tt->next = new Node ( HC_CONTENT,fc->prop );
                    tt = tt->next;
                }
                /*Fill the fields:*/
                tt->data = ( void * ) new DBContent ( rdata,rsize );
            }
        }//end of if(match)
    }//end of if(storeContent)

    /***Other properties: */

    return 0;
}

void DataBase::addLnk ( const char *loc ) {
    QString catname;
    gzFile f=NULL;
    FileReader *fw = NULL;

    Node *tmp=root->child,
              *n = new Node ( HC_CATLNK,root );

    /* Reading database name from the pointed file */
    f = gzopen ( loc,"rb" );
    if ( f==NULL ) {
        QMessageBox::warning ( NULL,tr ( "Error" ),tr ( "I can't open the file: %1" ).arg ( loc ) );
        return;
    }

    fw = new FileReader ( f );
    catname=fw->getCatName();
    if ( catname.isEmpty() ) {
        QMessageBox::warning ( NULL,tr ( "Error" ),tr ( "Error while parsing file: %1" ).arg ( loc ) );
        delete fw;
        gzclose ( f );
        return;
    }
    gzclose ( f );
    delete fw;
    /* end reading from the file */

    catname.prepend ( "@" );

    n->data = new DBCatLnk ( catname,mstr ( loc ),"" );

    if ( root->child == NULL ) {
        root->child = n;
    } else {
        while ( tmp->next != NULL ) tmp = tmp->next;
        tmp->next = n;
    }

    root->touchDB();
}
/***************************************************************************/
void DataBase::deleteNode ( Node *d ) {
    Node *p;

    if ( d==NULL ) return;
    p=d->parent;
    if ( p->child == d ) {
        p->child = p->child->next;
        d->next = NULL;
        delete d;
    } else {
        p = p->child;
        while ( p->next != d )
            p = p->next;
        p->next=p->next->next;
        d->next = NULL;
        delete d;
    }
    ( ( DBCatalog * ) ( ( getRootNode() )->data ) )->touch();
}
/***************************************************************************/
double DataBase::getSize ( Node *s,int level ) {
    double v=0.0;
    if ( s->type == HC_FILE ) {
        switch ( ( ( DBFile * ) ( s->data ) )->sizeType ) {
        case 0: v += ( ( ( DBFile * ) ( s->data ) )->size ) / ( 1024*1024 );      break; //byte
        case 1: v += ( ( ( DBFile * ) ( s->data ) )->size ) /1024;             break; //Kb
        case 2: v += ( ( ( DBFile * ) ( s->data ) )->size );                  break; //Mb
        case 3: v += ( ( ( DBFile * ) ( s->data ) )->size ) *1024;             break; //Gb
        }
    }
    if ( s->child != NULL ) v += getSize ( s->child,level+1 );
    if ( level != 0 )
        if ( s->next  != NULL ) v += getSize ( s->next,level+1 );
    return v;
}

unsigned long DataBase::getCountDirs ( Node *s,int level ) {
    unsigned long v=0;
    if ( s->type == HC_DIRECTORY )
        v=1;
    if ( s->child != NULL ) v += getCountDirs ( s->child,level+1 );
    if ( level != 0 )
        if ( s->next  != NULL ) v += getCountDirs ( s->next,level+1 );
    return v;
}

unsigned long DataBase::getCountFiles ( Node *s,int level ) {
    unsigned long v=0;
    if ( s->type == HC_FILE )
        v=1;
    if ( s->child != NULL ) v += getCountFiles ( s->child,level+1 );
    if ( level != 0 )
        if ( s->next  != NULL ) v += getCountFiles ( s->next,level+1 );
    return v;
}

void DataBase::setNice ( bool nic ) {
    nicef = nic;
}
/*****************************************************************/
/* ???
int compare(char *a,char *b)
 {
  int i;
  for(i=0;;i++)
   {
     if(a[i]>b[i]) return 0;
     if(a[i]<b[i]) return 1;
     if(a[i]=='\0') return 1;
     if(b[i]=='\0') return 0;
   }
 }
*/

Node * DataBase::getMOnPos ( int p ) {
    int i;
    Node *step;

    step=root->child;
    for ( i=0;i<p;i++ )
        step = step->next;
    return step;
}

void DataBase::sortM ( int mode ) {
    Node *step;
    void *data;
    int length;
    int type;
    int i,j;

    if ( root==NULL ) return;
    length=0;
    step=root->child;
    while ( step != NULL ) {
        length++;
        step=step->next;
    }

    for ( i=0;i<length;i++ )
        for ( j=i;j<length;j++ ) {
            if ( getMOnPos ( i )->type == HC_CATLNK )
                continue;
            else if ( getMOnPos ( j )->type == HC_CATLNK )
                ; /* nothing */
            else {
                switch ( mode ) {
                case NUMBER:
                    if ( ( ( DBMedia * ) ( getMOnPos ( i )->data ) )->number <
                            ( ( DBMedia * ) ( getMOnPos ( j )->data ) )->number )
                        continue;
                    break;

                case NAME:
                    if ( 0 < QString::localeAwareCompare (
                                ( ( DBMedia * ) ( getMOnPos ( i )->data ) )->name ,
                                ( ( DBMedia * ) ( getMOnPos ( j )->data ) )->name ) )
                        continue;
                    break;

                case TYPE:
                    if ( ( ( DBMedia * ) ( getMOnPos ( i )->data ) )->type <=
                            ( ( DBMedia * ) ( getMOnPos ( j )->data ) )->type )
                        continue;
                    break;

                case TIME:
                    if ( ( ( DBMedia * ) ( getMOnPos ( i )->data ) )->modification <
                            ( ( DBMedia * ) ( getMOnPos ( j )->data ) )->modification )
                        continue;
                    break;
                }
            }
            //swap
            step = getMOnPos ( i )->child;
            data = getMOnPos ( i )->data ;
            type = getMOnPos ( i )->type;

            getMOnPos ( i )->child = getMOnPos ( j )->child;
            getMOnPos ( i )->data  = getMOnPos ( j )->data;
            getMOnPos ( i )->type  = getMOnPos ( j )->type;

            getMOnPos ( j )->child = step;
            getMOnPos ( j )->data  = data;
            getMOnPos ( j )->type  = type;

        }
    root->touchDB();
}
/*************************************************************************/
const char *shortMonthName0 ( int i ) {
    switch ( i ) {
    case 1:  return "Jan";
    case 2:  return "Feb";
    case 3:  return "Mar";
    case 4:  return "Apr";
    case 5:  return "May";
    case 6:  return "Jun";
    case 7:  return "Jul";
    case 8:  return "Aug";
    case 9:  return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    }
    return "Err";
}

/* ???
char *getCurrentTime(void)
 {
  char *m=new char[30];
  QTime t;
  QDate d;
  t = QTime::currentTime();
  d = QDate::currentDate();
    sprintf(m,// "%s.%d %d:%d %d"
    "%s %02d %02d:%02d %d",(const char *)
    (shortMonthName0(d.month())),d.day(),t.hour(),t.minute(),d.year());


  return recodeI(m,&dbb6);
 }
*/
/* ???
char *getTime(QDateTime dt)
 {
  char *m=new char[30];
  QTime t;
  QDate d;
  t = dt.time();
  d = dt.date();
  sprintf(m,// "%s.%d %d:%d %d"
           "%s %02d %02d:%02d %d",(const char *)
    (shortMonthName0(d.month())),d.day(),t.hour(),t.minute(),d.year());

  return recodeI(m,&dbb6);
 }
*/

/*********************************************************************************
Import functions:
*********************************************************************************/
Node * DataBase::getMediaNode ( QString name ) {
    Node *t=NULL;

    t=root->child; //first media
    while ( t != NULL ) {
        if ( t->type != HC_MEDIA ) continue;
        if ( t->getNameOf() == name )
            return t;
        t=t->next;
    }
    return NULL;
}

Node * DataBase::putMediaNode ( QString name,int number,QString owner,int type,QString comment ) {
    Node *t=NULL,*n=NULL;

    n = new Node ( HC_MEDIA,root );
    n->data = ( void * ) new DBMedia ( name,number,owner,type,comment );

    if ( root->child == NULL )
        root->child = n;
    else {
        t=root->child; //first media
        while ( t->next != NULL )  t=t->next;
        t->next = n;
    }
    return n;
}

Node * DataBase::getMediaNode ( int id ) {
    Node *t=NULL;

    t=root->child; //first media
    while ( t != NULL ) {
        if ( ( ( DBMedia * ) ( t->data ) )->number == id ) {
            return t;
        }
        t=t->next;
    }
    return NULL;
}

Node * DataBase::getDirectoryNode ( Node *meddir,QString name ) {
    Node *t=NULL;

    t=meddir->child;
    while ( t != NULL ) {
        if ( t->type != HC_DIRECTORY ) {
            t=t->next;
            continue;
        }
        if ( t->getNameOf() == name )
            return t;
        t=t->next;
    }
    return NULL;
}

Node * DataBase::putDirectoryNode ( Node *meddir,QString name,QDateTime modification,QString comment ) {
    Node *t=NULL,*n=NULL;

    n = new Node ( HC_DIRECTORY,meddir );
    n->data = ( void * ) new DBDirectory ( name,modification,comment );

    if ( meddir->child == NULL )
        meddir->child = n;
    else {
        t=meddir->child;
        while ( t->next != NULL )  t=t->next;
        t->next = n;
    }
    return n;
}

Node * DataBase::getFileNode ( Node *directory,QString name ) {
    Node *t=NULL;

    t=directory->child;
    while ( t != NULL ) {
        if ( t->type != HC_FILE ) {
            t=t->next;
            continue;
        }
        if ( t->getNameOf() == name )
            return t;
        t=t->next;
    }
    return NULL;

}

Node * DataBase::putFileNode ( Node *directory,QString name,QDateTime modification,QString comment,int sizeType,float size ) {
    Node *t=NULL,*n=NULL;

    n = new Node ( HC_FILE,directory );
    n->data = ( void * )
              new DBFile ( name,modification,comment,size,sizeType );

    if ( directory->child == NULL )
        directory->child = n;
    else {
        t=directory->child;
        while ( t->next != NULL )  t=t->next;
        t->next = n;
    }
    return n;
}


Node * DataBase::putTagInfo ( Node *file,QString artist,QString title,QString comment,QString album,QString year ) {
    Node *t=NULL,*n=NULL;

    n = new Node ( HC_MP3TAG,NULL );
    n->data = ( void * )
              new DBMp3Tag ( artist,title,comment,album,year );

    if ( ( ( DBFile * ) ( file->data ) )->prop == NULL )
        ( ( DBFile * ) ( file->data ) )->prop = n;
    else {
        t= ( ( DBFile * ) ( file->data ) )->prop;
        while ( t->next != NULL )  t=t->next;
        t->next = n;
    }
    return n;
}



