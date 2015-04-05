// #define DoAviDebug
#include <qstring.h>
#include  <stdio.h>


QString parseAviHeader( FILE *plik );

void fourccSwitch( unsigned long fcc, QString &ReturnData );
void fourccSwitch2( unsigned short fcc, QString &ReturnData );
void freeINFOLIST( void );
