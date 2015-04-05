/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCAT_MISC_HEADER
#define CDCAT_MISC_HEADER

#include <QString>

QString getCDName( const char *CDpath );
QString getCDSerial( const char *CDpath );
bool diskIsDVD( const char *CDpath );

#endif
