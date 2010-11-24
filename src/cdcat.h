/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCAT_CDCAT
#define CDCAT_CDCAT

// CdCat version text:
#define VERSION  "UNICODE 1.0 (beta release)"

// Datafile version:
// The known versions listed in guibase.cpp - checkversion !
#define DVERS    "2.0"

// CdCat homepage location:
#define HOMEPAGE "http://cdcat.sf.net"
// + help.ui


class QString;

char  * mstr ( const char *imp );
QString applicationDirPath ( char **argc );
QString applicationFilePath ( char **argc );

#endif

