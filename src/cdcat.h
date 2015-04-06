/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCAT_CDCAT
#define CDCAT_CDCAT

#include <QTranslator>
#include "config.h"

// #define IS_RELEASE


// CdCat version text:

#define VERSION  "2.4 alpha"


// Datafile version:
// The known versions listed in guibase.cpp - checkversion !
#define DVERS    "2.2"

// CdCat homepage location:
#define HOMEPAGE "<a href=\"http://cdcat.sf.net\">http://cdcat.sf.net</a>"
#define PROGRAM_NAME "cdCat"
// + help.ui


class QString;

char *mstr( const char *imp );
QString applicationDirPath( char **argc );
QString applicationFilePath( char **argc );
static QTranslator *translator;
bool *init_debug_info();
#endif
