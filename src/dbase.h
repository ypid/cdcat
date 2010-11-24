/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/


#ifndef CDCAT_DBASE
#define CDCAT_DBASE

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>

// values for class Node::type
#define HC_UNINITIALIZED 0
#define HC_CATALOG       1
#define HC_MEDIA         2
#define HC_DIRECTORY     3
#define HC_FILE          4
#define HC_MP3TAG        5
#define HC_CONTENT       6
#define HC_CATLNK        7
#define HC_ARCHIV        8

//values for class DBMedia::type
#define UNKNOWN          0
#define CD               1
#define DVD              2
#define HARDDISC         3
#define FLOPPY           4
#define NETPLACE         5
#define FLASHDRV         6
#define OTHERD           7

//values for class DBFile::sizeType
#define BYTE             0
#define KBYTE            1
#define MBYTE            2
#define GBYTE            3

//values for class DataBase::sortM( value )
#define NAME             1
#define NUMBER           0
#define TYPE             2
#define TIME             3

#define MAX_STORED_SIZE (128*1024)

class QFileInfo;
class PWw;

void caseSensConversion ( char *p );
void easyFormConversion ( char *p );

QString date_to_str ( QDateTime dt );

class Node {
public:
    int type; /*
   0 - uninitialized
   1 - catalog
   2 - media
   3 - directory
   4 - file
   5 - mp3tag
   6 - content
   7 - catlnk
   8 - archive.prop.
  */

    Node *next;   //neighbour node. (Same level)
    Node *child;  //one level below
    Node *parent; //the parent element (Upper level)

    void *data; // see the DB* classes below

    QString getNameOf ( void );
    // return the name of the node,which can be media name, catalog name,
    // file name, directory name

    QString getFullPath ( void );
    //return the full path of the node.

    void touchDB ( void );
    //tell to the database that you modified it, and save the modification time.

    Node ( void );
    // t:UNINITIALIZED , p:NULL

    Node ( int t,Node *p );
    // t:type , p:parent

    ~Node ( void );
};

/*
 *    DB ... classes:
 */

class DBCatalog { //type is 1
public:
    QString name;
    QString owner;
    QString comment;

    int  writed;
    //Writed to the disc  1=yes 0=no

    char filename[256];
    //the file name of the database. (If you opened it or save it yet.)

    QDateTime modification;

    // If the database opened from an existing file it contains
    // the version of the datafile.
    QString fileversion;

    void touch ( void );
    //tell to the database that you modified it, and save the modification time.

    DBCatalog ( void );
    ~DBCatalog ( void );

    DBCatalog ( QString n,QString o,QString c );
    DBCatalog ( QString n,QString o,QString c,QDateTime mod );
    // n:name , o:owner , c:comment, mod:modification
};

class DBMedia { //type is 2
public:
    QString name;
    //the media name, must be unique

    int  number;
    //the unique serial name of the media

    QString owner;
    //if not defined is NULL

    int  type;
    // 0-unknown 1-CD 2-DVD 3-HardDisc 4-Floppy 5-NetworPlace 6-FlashDrive 7-OtherDevice

    QDateTime modification;
    QString   comment;

    //Is the media borrowed? NULL=>No ; String=>Yes , contains: for who?
    QString   borrowing;

    DBMedia ( QString n,int nu,QString o,int t,QString c );
    DBMedia ( QString n,int nu,QString o,int t,QString c,QDateTime mod );
    // n:name , nu:number o:owner , t:type , c:comment, mod:modification
    DBMedia ( void );
    ~DBMedia ( void );
};

class DBDirectory { //type is 3
public:
    QString   name;
    QDateTime modification;
    QString   comment;

    DBDirectory ( void );
    DBDirectory ( QString n,QDateTime mod,QString c );
    // n:name , mod:modification , c:comment

    ~DBDirectory ( void );
};

class DBFile { //type is 4
public:
    QString   name;
    QDateTime modification;
    QString  comment;
    float size;
    //filesize

    int   sizeType;
    //Tell the meaning of size 0=byte 1=Kb 2=Mb 3=Gb

    Node *prop;
    //the root pointer of other Nodes. can be mp3tag of etc...

    DBFile ( void );
    DBFile ( QString n,QDateTime mod,QString c,float s,int st );
    // n:name , mod:modification , c:comment , s:size , st:sizeType

    ~DBFile ( void );
};

class DBMp3Tag { //type is 5
public:
    QString artist;
    QString title;
    QString comment;
    QString album;
    QString year;

    DBMp3Tag ( void );
    DBMp3Tag ( QString a,QString t,QString c,QString al,QString y );
    //a:artist , t:title , c:commnet , al:album , y:year

    ~DBMp3Tag ( void );
};

class DBContent { //type is 6
public:

    DBContent ( void );
    DBContent ( unsigned char *pbytes,unsigned long pstoredSize );

    ~DBContent ( void );

    unsigned char *bytes;
    unsigned long  storedSize;
};


class DBCatLnk { //type is 7
public:
    QString name;
    char    *location;
    QString comment;


    DBCatLnk ( void );
    DBCatLnk ( QString pname,char *plocation,QString pcomment );

    ~DBCatLnk ( void );
};


class DataBase : public QObject {
    Q_OBJECT

public:

    DataBase ( void );
    ~DataBase ( void );

    QString errormsg;
    //If some function return with fail code, the system will be able to determine the reason of
    //fail from this message.

    bool nicef;
    // saving option (niceFormat) pass to the FileWriter class

    bool storeMp3tags;
    // strore the mp3 tags

    bool v1_over_v2;
    // true is the version 1 tag the default

    bool storeMp3techinfo;
    // store mp3 technical informations: time,bitrate,freq, stereo type

    bool storeAvitechinfo;
    // store avi technical informations: time,bitrate,...

    //The database will store some file's contetn if ..
    bool storeContent;
    // the storing enabled...
    QString storedFiles;
    // ; separated wildcard plattern list of files
    unsigned long storeLimit;
    // up to this size limitation (bytes)



    void  setDBName ( QString n );
    void  setDBOwner ( QString o );
    void  setComment ( QString c );
    void  setNice ( bool nic );

    QString& getDBName ( void );
    QString& getDBOwner ( void );
    QString& getComment ( void );
    Node *getRootNode ( void )
    { return root; }

    int   saveDB ( void );
    //save the full database to the file, if the file specified before:
    // The file opened or saved before this function.

    int   saveAsDB ( char *filename );
    //Save the full database to the specified file.

    int   openDB ( char *filename );
    //Open the catalog from a file

    int   insertDB ( char *filename );
    //insert a new catalog from filename into the existing catalog.

    int   addMedia ( QString what,QString name,int number,int type );
    int   addMedia ( QString what,QString name,int number,int type,QString owner );
    //Scan a new media from the disk specified in "what" with name "name" and "number" ...
    //and immediately added to the database!

    void  deleteNode ( Node *d );

    void  sortM ( int mode );
    //sort the media in the catalog

    void addLnk ( const char *loc );

    double        getSize ( Node *s,int level=0 );
    unsigned long getCountFiles ( Node *s,int level=0 );
    unsigned long getCountDirs ( Node *s,int level=0 );

    PWw *pww; //Pease Wait Window :-)

private:
    Node *root;

    Node *getMOnPos ( int p );

    int   scanFsToNode ( QString what,Node *to );
    int   scanFileProp ( QFileInfo *fi,DBFile *fc );

public:

    /*
     funtions /below/ for make import from another formats:
      The get..  function try to specify a node and return that node's pointer.
      If the return value is NULL the node is not exist in that environment
      (environment: catalog/needn't to specify of course/ in case getMedia..
                     media or directory in case getDir..
      	   media but rather directory in case getFile.. )
      so you have to use the put... functions to make the node.

      Always test the existion a node before you try to create it !!!

      example:
       you want to add "MY_CD/install/sources/cdcat.tar.bz2" where is MY_CD is the media name:

        Node *env,*curr;
        curr = getMediaNode("MY_CD");
        if(curr == NULL) curr = putMediaNode("MY_CD", ... );

        env = curr;

        curr = getDirectoryNode(env,"install");
        if(curr == NULL) curr = putDirectoryNode(env,"install", ... );

        env = curr; //we want to search/insert the "sources" directory in "install" directory.

        curr = getDirectoryNode(env,"sources");
        if(curr == NULL) curr = putDirectoryNode(env,"sources", ... );

        env = curr;

        curr = getFileNode(env,"cdcat.tar.bz2");
        if(curr == NULL) curr = putFileNode(env,"cdcat.tar.bz2", ... );
    */

    Node * getMediaNode ( QString name );
    Node * putMediaNode ( QString name,int number,QString owner,int type,QString comment );
    Node * getMediaNode ( int id );

    //step only one directory with one call!
    //the meddir can be media or directoy depend it's will be the first directory or not.
    Node * getDirectoryNode ( Node *meddir,QString name );
    Node * putDirectoryNode ( Node *meddir,QString name,QDateTime modification,QString comment );

    Node * getFileNode ( Node *directory,QString name );
    Node * putFileNode ( Node *directory,QString name,QDateTime modification,QString comment,int sizeType,float size );

    Node * putTagInfo ( Node *file,QString artist,QString title,QString comment,QString album,QString year );


};


//char *getTime        (QDateTimeTime dt); ???
//char *getCurrentTime (void); ???
#endif



