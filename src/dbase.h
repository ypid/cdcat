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
#include <qstringlist.h>
#include <qdatetime.h>
#include <QLocale>
#include <QImage>



#ifdef _WIN32
typedef int uid_t;
typedef int gid_t;
#endif

#ifndef _WIN32
#include <pwd.h>
#include <grp.h>
#endif

#include <libtar.h>

#ifndef NO_MEDIAINFO
#include "cdcatmediainfo.h"
#endif

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
#define HC_EXIF        9
#define HC_THUMB        10

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
#define UNIT_BYTE             0
#define UNIT_KBYTE            1
#define UNIT_MBYTE            2
#define UNIT_GBYTE            3
#define UNIT_TBYTE            4

#define SIZE_ONE_BYTE double(1)
#define SIZE_ONE_KBYTE double(1024.0)
#define SIZE_ONE_MBYTE double(SIZE_ONE_KBYTE*1024.0)
#define SIZE_ONE_GBYTE double(SIZE_ONE_MBYTE*1024.0)
#define SIZE_ONE_TBYTE double(SIZE_ONE_GBYTE*1024.0)

//values for class DataBase::sortM( value )
#define NAME             1
#define NUMBER           0
#define TYPE             2
#define TIME             3

#define MAX_STORED_SIZE (128*1024)

class QFileInfo;
class PWw;
class ArchiveFile;

void caseSensConversion ( char *p );
void easyFormConversion ( char *p );



QString date_to_str ( QDateTime dt );

class Node {
	public:
		/*
			0 - uninitialized
			1 - catalog
			2 - media
			3 - directory
			4 - file
			5 - mp3tag
			6 - content
			7 - catlnk
			8 - archive.prop.
			9 - exif
			10 - thumbnail
		*/
		int type; 
		
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
		
		Node ( int t, Node *p );
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
		QString category;
		
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
		
		DBCatalog ( QString n, QString o, QString c, QDateTime mod = QDateTime::currentDateTime(), QString pcategory = "" );
		// n:name , o:owner , c:comment, mod:modification, pcategory:category
		
		int sortedBy;
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
		QString category;
		
		//Is the media borrowed? NULL=>No ; String=>Yes , contains: for who?
		QString   borrowing;
		
		DBMedia ( QString n, int nu, QString o, int t, QString c, QDateTime mod = QDateTime::currentDateTime(), QString pcategory = "" );
		// n:name , nu:number o:owner , t:type , c:comment, mod:modification, pcategory:category
		DBMedia ( void );
		~DBMedia ( void );
};

class DBDirectory { //type is 3
	public:
		QString   name;
		QDateTime modification;
		QString   comment;
		QString category;
		
		DBDirectory ( void );
		DBDirectory ( QString n, QDateTime mod, QString c, QString pcategory = "" );
		// n:name , mod:modification , c:comment, pcategory:category
		
		~DBDirectory ( void );
};

class DBFile { //type is 4
	public:
		QString   name;
		QDateTime modification;
		QString  comment;
		QString category;
		QString fileinfo;
		QList<ArchiveFile> archivecontent;
		double size;
		//filesize
		
		int   sizeType;
		//Tell the meaning of size 0=byte 1=Kb 2=Mb 3=Gb
		
		Node *prop;
		//the root pointer of other Nodes. can be mp3tag of etc...
		
		DBFile ( void );
		DBFile ( QString n, QDateTime mod, QString c, double s, int st, QString pcategory = "", QList<ArchiveFile> parchivecontent = QList<ArchiveFile>(), QString fileinfo = "" );
		// n:name , mod:modification , c:comment , s:size , st:sizeType, pcategory:category, fileinfo:fileinfo
		
		~DBFile ( void );
};

class DBMp3Tag { //type is 5
	public:
		QString artist;
		QString title;
		QString comment;
		QString album;
		QString year;
		int tnumber;
		
		DBMp3Tag ( void );
		DBMp3Tag ( QString a, QString t, QString c, QString al, QString y, int tnum );
		//a:artist , t:title , c:commnet , al:album , y:year, tnum:tnumber

		~DBMp3Tag ( void );
};

class DBContent { //type is 6
	public:
		DBContent ( void );
		DBContent ( unsigned char *pbytes, unsigned long pstoredSize );
		~DBContent ( void );
		
		unsigned char *bytes;
		unsigned long  storedSize;
};


class DBCatLnk { //type is 7
	public:
		QString name;
		char    *location;
		QString comment;
		QString category;
		
		DBCatLnk ( void );
		DBCatLnk ( QString pname, char *plocation, QString pcomment, QString pcategory = "" );
		~DBCatLnk ( void );
};

class ArchiveFile : public QObject { //type is 8
	public:
		ArchiveFile ( QString fileattr = "-rw- rw- rw-", QString user = tr ( "unknown" ), QString group = tr ( "unknown" ), long long int size = 0, QDateTime date = QDateTime().currentDateTime(), QString path = "/", QString filetype = tr ( "" ) );
		ArchiveFile ( const ArchiveFile& );
		ArchiveFile& operator = ( const ArchiveFile& );
		QString toPrettyString ( bool showAttr = true, bool showUser = true, bool showGroup = true, bool showSize = true, bool showDate = true, bool showFileType = true, bool doHtmlTableLine = false, int fontsize = 8 );
		QString toDbString();
		void setDbString ( QString DbString );

		QString fileattr;
		QString user;
		QString group;
		long long int size;
		QDateTime date;
		QString path;
		QString filetype;

	private:
		QChar div;
};


class DBExifData : public QObject { //type is 9
	public:
		DBExifData ( void );
		DBExifData ( QStringList ExifDataList );
		~DBExifData ( void );
		
		QStringList ExifDataList;
};

class DBThumb : public QObject { //type is 10
	public:
		DBThumb ( void );
		DBThumb ( QImage ThumbImage );
		~DBThumb ( void );
		
		QImage ThumbImage;
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
		
		bool storeFileInfo;
		// store informations got from fileinfo
		
		//The database will store some file's contetn if ..
		bool storeContent;
		// the storing enabled...
		QString storedFiles;
		// ; separated wildcard plattern list of files
		unsigned long storeLimit;
		// up to this size limitation (bytes)
		
		bool showProgressedFileInStatus;
		bool doScanArchive;
		bool doScanArchiveTar;
		bool doScanArchiveLib7zip;
		bool showProgressedArchiveFileInStatus;
		bool displayCurrentScannedFileInTray;
		bool storeExifData;
		bool storeThumb;
		bool doExcludeFiles;
		bool useWildcardInsteadRegexForExclude;
		bool ignoreReadErrors;
		bool doWork;
		
		int thumbWidth;
		int thumbHeight;
		
		QString XML_ENCODING;
		QStringList ThumbExtsList;
		QStringList ExcludeFileList;
		
		QList<QRegExp> excludeFileRegExList;
		
		void  setDBName ( QString n );
		void  setDBOwner ( QString o );
		void  setComment ( QString c );
		void  setCategory ( QString category );
		void  setNice ( bool nic );
		
		QString& getDBName ( void );
		QString& getDBOwner ( void );
		QString& getComment ( void );
		QString& getCategory ( void );
		Node *getRootNode ( void ) {
			return root;
		}
		
		int   saveDB ( void );
		//save the full database to the file, if the file specified before:
		// The file opened or saved before this function.
		
		int   saveAsDB ( char *filename );
		//Save the full database to the specified file.
		
		int   openDB ( char *filename );
		//Open the catalog from a file
		
		int   insertDB ( char *filename, bool skipDuplicatesOnInsert = false, bool isGzFile = true );
		//insert a new catalog from filename into the existing catalog.
		
		int   addMedia ( QString what, QString name, int number, int type );
		int   addMedia ( QString what, QString name, int number, int type, QString owner, QString Category = "" );
		//Scan a new media from the disk specified in "what" with name "name" and "number" ...
		//and immediately added to the database!
		
		void  deleteNode ( Node *d );
		
		void  sortM ( int mode, bool ascending = true );
		//sort the media in the catalog
		
		void addLnk ( const char *loc );
		
		double getSize ( Node *s, int level = 0 );
		unsigned long getCountFiles ( Node *s, int level = 0 );
		unsigned long getCountDirs ( Node *s, int level = 0 );
		
		PWw *pww; //Pease Wait Window :-)
		
		QStringList SupportedFileInfoExtensionsList;
	
	private:
		Node *root;
		Node *getMOnPos ( int p );
		
		int   scanFsToNode ( QString what, Node *to );
		int   scanFileProp ( QFileInfo *fi, DBFile *fc );
		
		QStringList Lib7zipTypes;
		
		void strmode ( mode_t mode, char *p );
		
		enum ArchiveType { Archive_tar = 0, Archive_bzip2 =  1, Archive_targz = 2, Archive_tarbz2 = 3, Archive_zip = 4, Archive_sevenzip = 5, Archive_rar = 6, Archive_lib7zip = 7 };
		
		QList<ArchiveFile> scanArchive ( QString path, ArchiveType type );
		QString pcategory;
		
#ifndef NO_MEDIAINFO
		CdcatMediaInfo me;
#endif
	
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
		Node * putMediaNode ( QString name, int number, QString owner, int type, QString comment, QDateTime modification, QString category = "" );
		Node * getMediaNode ( int id );
		
		//step only one directory with one call!
		//the meddir can be media or directoy depend it's will be the first directory or not.
		Node * getDirectoryNode ( Node *meddir, QString name );
		Node * putDirectoryNode ( Node *meddir, QString name, QDateTime modification, QString comment, QString category = "" );
		
		Node * getFileNode ( Node *directory, QString name );
		Node * putFileNode ( Node *directory, QString name, QDateTime modification, QString comment, int sizeType, double size, QString category = "", QList<ArchiveFile> archivecontent = QList<ArchiveFile>(), QString fileinfo = "" );
		
		Node * putTagInfo ( Node *file, QString artist, QString title, QString comment, QString album, QString year, int tnumber );
		
		void setShowProgressedFileInStatus ( bool showProgressedFileInStatus );
		void setShowProgressedArchiveFileInStatus (bool showProgressedArchiveFileInStatus);
		void setDoArchiveScan ( bool doScanArchive ) {
			this->doScanArchive = doScanArchive;
		};
		void setDoArchiveScanTar ( bool doScanArchiveTar ) {
			this->doScanArchiveTar = doScanArchiveTar;
		};
		void setDoScanArchiveLib7zip ( bool doScanArchiveLib7zip ) {
			this->doScanArchiveLib7zip = doScanArchiveLib7zip;
		};
	
	signals:
		void pathScanned ( QString path );
		void fileScanned ( QString filepath );
		void pathExtraInfoAppend( QString extrainfo );
};




//char *getTime        (QDateTimeTime dt); ???
//char *getCurrentTime (void); ???
#endif



// kate: indent-mode cstyle; replace-tabs off; tab-width 8; 
