/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <qregexp.h>
#include <qfile.h>
#include <qtextstream.h>

#include "config.h"
#include "cdcat.h"
#include <iostream>

using namespace std;

#ifdef _WIN32
#include <windows.h>
#endif

#include <QMessageBox>


#ifndef _WIN32
// Linux:

FILE *openDevice( const char *CDpath ) {
    QString devicename = "";
    FILE *result;
    QString cfgcdpath = QString( CDpath ).replace( QRegExp( "/$" ), "" );
    bool device_found = false;

    QFile f( "/etc/fstab" );
    QString line;

    if (f.open( QIODevice::ReadOnly )) { // file opened successfully
        QTextStream t( &f );             // use a text stream
        while (!t.atEnd()) {
            line = t.readLine();         // line of text excluding '\n'

            if (!line.startsWith( "#" ) && !line.isEmpty()) {
                if (((line.section( "\t", 1, 1, QString::SectionSkipEmpty )).replace( QRegExp( "/$" ), "" )).compare( cfgcdpath ) == 0) {
                    devicename = line.section( "\t", 0, 0 );
                    device_found = true;
                }
                if (((line.section( " ", 1, 1, QString::SectionSkipEmpty )).replace( QRegExp( "/$" ), "" )).compare( cfgcdpath ) == 0) {
                    devicename = line.section( " ", 0, 0 );
                    device_found = true;
                }
            }
        }
        f.close();
    }

    if (!device_found) {
        // its the pure device name (e.g. /dev/sr0)
        devicename = QString( CDpath );
    }

    fprintf( stderr, "openDevice(): devicename: |%s|\n", devicename.toLocal8Bit().constData());
    result = fopen( devicename.toLocal8Bit().constData(), "r" );
    return result;
}

bool diskIsDVD( const char *CDpath ) {
    int size = 0;
    FILE *deviceptr = NULL;
    char *field = NULL;

    deviceptr = openDevice( CDpath );
    if (!deviceptr) {
        return false;
    }
    fseek( deviceptr, 32848, SEEK_SET );
    field = new char(9);
    fread( field, sizeof(char), 8, deviceptr );
    fclose( deviceptr );

    size = ((field[0] & 0xff) | ((field[1] & 0xff) << 8)
            | ((field[2] & 0xff) << 16) | ((field[3] & 0xff) << 24));
    delete field;

    // fprintf(stderr,"Size: %i\n",size);
    // return DVD if there is more data than a CD can ever have
    return size > 450000;
}

QString getCDName( const char *CDpath ) {
    char *name = new char[64];           // return value
    FILE *deviceptr = NULL;

    DEBUG_INFO_ENABLED = init_debug_info();


    strcpy( name, "" );
    if (strlen( CDpath ) != 0) {
        /** Read the volume name of the device *******************/

        deviceptr = openDevice( CDpath );
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "getCDName(): could not open device" << endl;
        }
        if (!deviceptr) {
            return name;
        }
        fseek( deviceptr, 32808, SEEK_SET );
        fread( name, sizeof(char), 32, deviceptr );
        fclose( deviceptr );

        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "getCDName(): " << name << endl;
        }
        name[32] = '\0';

        // strip whitespaces
        strcpy( name, (const char *)QString( name ).trimmed().toLocal8Bit().constData());
    }
    return QString( name );
}
QString getCDSerial( const char *CDpath ) {
    // FIXME



    return "";
}
#endif

#ifdef _WIN32
// Win32:
void TCharToChar( const wchar_t *Src, char *Dest, int Size ) {
    WideCharToMultiByte( CP_ACP, 0, Src, wcslen( Src ) + 1, Dest, Size, NULL, NULL );
}

QString getCDName( const char *CDpath ) {
    /* wide char */
    /*
     * // this does not work correctly!
     * char  *name = new char[64];
     *
     * DWORD  dwDummy;
     * DWORD  dwFlags;
     * LPWSTR WSTR_CDpath = NULL;
     *
     * int length = MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,NULL,0);
     * MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,WSTR_CDpath,length);
     * LPWSTR LPW_name=L"";
     * DWORD dwVolumeSerialNumber;
     * if ( !GetVolumeInformationW(WSTR_CDpath, LPW_name, 64, NULL, NULL, NULL, NULL, 0) )
     * //if ( !GetVolumeInformation ( WSTR_CDpath, LPW_name, 0, 0, 0, 0, 0, 0 ))
     * {
     *      return QString("");
     * }
     *
     * WideCharToMultiByte(CP_ACP, 0, (LPWSTR)LPW_name, 64,name, 64, NULL, NULL);
     * QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
     * return QString(name);
     */
    /*end wide char */

    /* test2 */
    /*
     * char  *name = new char[64];
     * DWORD  dwDummy;
     * DWORD  dwFlags;
     *
     * if(!GetVolumeInformationW((LPCSTR)CDpath,(LPSTR)name,64,NULL,&dwDummy,&dwFlags,NULL,0))
     *  strcpy(name,"");
     * QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
     * return QString(name);
     */
    /* end test2 */


    /* ANSI */
    // works with german special chars too :)
    char bufName1[64];
    DWORD dwSerial;
    char *name;

    if (!GetVolumeInformationA( CDpath, bufName1, 64, &dwSerial, NULL, NULL, NULL, 0 )) {
        return QString( "" );
    }
    name = QString( bufName1 ).toLocal8Bit().data();
    // QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
    return QString( bufName1 );
    /* end ANSI */
}

QString getCDSerial( const char *CDpath ) {
    /* wide char */
    /*
     * // this does not work correctly!
     * char  *name = new char[64];
     *
     * DWORD  dwDummy;
     * DWORD  dwFlags;
     * LPWSTR WSTR_CDpath = NULL;
     *
     * int length = MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,NULL,0);
     * MultiByteToWideChar(CP_ACP,0,CDpath ,strlen(CDpath)+1,WSTR_CDpath,length);
     * LPWSTR LPW_name=L"";
     * DWORD dwVolumeSerialNumber;
     * if ( !GetVolumeInformationW(WSTR_CDpath, LPW_name, 64, NULL, NULL, NULL, NULL, 0) )
     * //if ( !GetVolumeInformation ( WSTR_CDpath, LPW_name, 0, 0, 0, 0, 0, 0 ))
     * {
     *      return QString("");
     * }
     *
     * WideCharToMultiByte(CP_ACP, 0, (LPWSTR)LPW_name, 64,name, 64, NULL, NULL);
     * QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
     * return QString(name);
     */
    /*end wide char */

    /* test2 */
    /*
     * char  *name = new char[64];
     * DWORD  dwDummy;
     * DWORD  dwFlags;
     *
     * if(!GetVolumeInformationW((LPCSTR)CDpath,(LPSTR)name,64,NULL,&dwDummy,&dwFlags,NULL,0))
     *  strcpy(name,"");
     * QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
     * return QString(name);
     */
    /* end test2 */


    /* ANSI */
    // works with german special chars too :)
    char bufName1[64];
    DWORD dwSerial;
    char *name;

    if (!GetVolumeInformationA( CDpath, bufName1, 64, &dwSerial, NULL, NULL, NULL, 0 )) {
        return QString( "" );
    }
//      char *myChar = "Hello World";
    // DWORD dw = reinterpret_cast<DWORD> ( myChar );
    // bufName1 = reinterpret_cast<char*> ( dw );
//      bufName1 = strncpy(&bufName1, myChar, 63);
    // name = QString ( bufName1 ).toLocal8Bit().data();
    // QMessageBox::information(0, "vol1", "path: "+QString(CDpath)+"\nname: \""+QString(name)+"\"");
    return QString( bufName1 );
    /* end ANSI */
}

#endif

// kate: indent-mode cstyle; replace-tabs off; tab-width 8;
