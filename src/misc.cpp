/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <qregexp.h>
#include <qfile.h>
#include <q3textstream.h>

#ifdef _WIN32
#include <windows.h>
#endif


#ifndef _WIN32
//Linux:

FILE *openDevice ( const char *CDpath ) {
    char *devicename = new char[64];
    FILE *result;

    QString cfgcdpath = QString ( CDpath ).replace ( QRegExp ( "/$" ),"" );

    strcpy ( devicename,"" );


    QFile f ( "/etc/fstab" );
    QString line;

    if ( f.open ( QIODevice::ReadOnly ) ) {	// file opened successfully
        Q3TextStream t ( &f );	// use a text stream

        while ( !t.eof() ) {
            line = t.readLine();	// line of text excluding '\n'

            if ( !line.startsWith ( "#" ) && !line.isEmpty() ) {
                if ( ( ( line.section ( "\t",1,1,QString::SectionSkipEmpty ) ).replace ( QRegExp ( "/$" ),"" ) ).compare ( cfgcdpath ) == 0 ) {
                    strcpy ( devicename, ( const char * ) line.section ( "\t",0,0 ) );
                }
                if ( ( ( line.section ( " " ,1,1,QString::SectionSkipEmpty ) ).replace ( QRegExp ( "/$" ),"" ) ).compare ( cfgcdpath ) == 0 ) {
                    strcpy ( devicename, ( const char * ) line.section ( " " ,0,0 ) );
                }
            }
        }
        f.close();
    } else
        return NULL;
    if ( !strcmp ( devicename,"" ) )
        return NULL;

    //fprintf(stderr,"devicename: |%s|\n",devicename);
    result = fopen ( devicename,"r" );
    delete devicename;
    return result;
}

bool diskIsDVD ( const char *CDpath ) {
    int size=0;
    char *field = new char[8];
    FILE *deviceptr  = NULL;
    deviceptr = openDevice ( CDpath );
    if ( !deviceptr ) return false;
    fseek ( deviceptr,32848,SEEK_SET );
    fread ( field,sizeof ( char ),8,deviceptr );
    fclose ( deviceptr );

    size = ( ( field[0] & 0xff ) | ( ( field[1] & 0xff ) << 8 )
             | ( ( field[2] & 0xff ) << 16 ) | ( ( field[3] & 0xff ) << 24 ) );
    delete field;

    //fprintf(stderr,"Size: %i\n",size);
    // return DVD if there is more data than a CD can ever have
    return ( size>450000 );
}

char *getCDName ( const char *CDpath ) {
    char *name       = new char[64]; //return value
    FILE *deviceptr  = NULL;

    strcpy ( name      ,"" );

    /** Read the volume name of the device *******************/

    deviceptr = openDevice ( CDpath );
    if ( !deviceptr ) return name;
    fseek ( deviceptr,32808,SEEK_SET );
    fread ( name,sizeof ( char ),32,deviceptr );
    fclose ( deviceptr );
    name[32]='\0';

    //strip whitespaces
    strcpy ( name, ( const char * ) QString ( name ).stripWhiteSpace() );

    return name;
}

#else
// Win32:
char * getCDName ( const char *CDpath ) {
    char  *name = new char[64];
    DWORD  dwDummy;
    DWORD  dwFlags;

    if ( !GetVolumeInformation ( ( LPCSTR ) CDpath, ( LPSTR ) name,64,NULL,
                                 &dwDummy,&dwFlags,NULL,0 ) )
        strcpy ( name,"" );
    return name;
}
#endif

