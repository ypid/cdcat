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

#include "config.h"
#include "cdcat.h"
#include <iostream>

using namespace std;

#ifdef _WIN32
#include <windows.h>
#endif


#include <QMessageBox>


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

QString getCDName ( const char *CDpath ) {
    char *name       = new char[64]; //return value
    FILE *deviceptr  = NULL;

    DEBUG_INFO_ENABLED = init_debug_info();


    strcpy ( name      ,"" );
    if (strlen(CDpath) != 0 ) {
		/** Read the volume name of the device *******************/
		
		deviceptr = openDevice ( CDpath );
		if ( !deviceptr ) return name;
		fseek ( deviceptr,32808,SEEK_SET );
		fread ( name,sizeof ( char ),32,deviceptr );
		fclose ( deviceptr );

    if(*DEBUG_INFO_ENABLED)
	std::cerr <<"getCDName: " << name <<endl;
		name[32]='\0';
		
		//strip whitespaces
		strcpy ( name, ( const char * ) QString ( name ).stripWhiteSpace() );
	 }
    return QString(name);
}
#endif

#ifdef _WIN32
// Win32:
void TCharToChar(const wchar_t* Src, char* Dest, int Size) 
 { 
 WideCharToMultiByte(CP_ACP, 0, Src, wcslen(Src)+1, Dest , Size, NULL, NULL); 
 } 

QString getCDName ( const char *CDpath ) {
    char bufName1[128];
    DWORD dwSerial;
    //char  *name = new char[64];
    char *name;
    //DWORD  dwDummy;
    //DWORD  dwFlags;
    //LPWSTR WSTR_CDpath = NULL;
    //int length = MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,NULL,0);
    //MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,WSTR_CDpath,length);
    //LPWSTR LPW_name=L"";

        DWORD dwVolumeSerialNumber;
    if ( !GetVolumeInformationA(CDpath, bufName1, sizeof(bufName1), &dwSerial, NULL, NULL, NULL, 0) )
    //if ( !GetVolumeInformation ( WSTR_CDpath, LPW_name,                0,   0, 0, 0, 0, 0 ))
    //  if(!GetVolumeInformation(LPWSTR("D:\\"),LPW_name,64,NULL,&dwDummy,&dwFlags,NULL,0))
	{
		//strcpy ( name,"\0" );
		return QString("");
	}
    name = QString(bufName1).toLocal8Bit().data();
    //WideCharToMultiByte(CP_ACP, 0, (LPWSTR)LPW_name, -1,name, 1000, NULL, NULL);    
    QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\""); 
    //return name;
    return QString(bufName1);
}
#endif

