/*
 * TEcHNO Own Multimedia Files Parser Procedures v0.1-pre1.1-CdCat
 * Currently released under GNU LGPL Licence (or somthing like that)
 * Bla Bla, will add Later
 *
 * Currently Works on AVI's only.
 * TODO:	-Rewrite to make scanning Linear, not Bulk
 *	-Add "bla bla" part
 *	-add idxh parser!
 *	-Control Flags
 *	-Minimise memmory usage
 *	-LIST/INFO Still not working
 *	-change that name
 *	-Warnings, hell lot of them
 *
 * FIXED:-Fixed dang LIST/INFO missing
 *	-Now returns QString (for CDCat only)
 *	-(obsolet)define some returning structure!
 *	-Quick fix for some files that didn't work correctly
 *
 * Wonder if It gets to CdCat.
 * It maid it's way.
 *
 * Made with kate, greates editor EVER ^__^
 *
 * Copyright Piotr "|TEcHNO|" Lewandowski, 2004
 */
#include <iostream>
#include <stdio.h>

#include <QString>

#include "tparser.h"
#include "config.h"
#include "cdcat.h"

#ifdef _WIN32
    #define _UINT4 unsigned long
    #define _INT4 long
#else
    #define _UINT4 unsigned int
    #define _INT4 int
#endif

/*
 * this version assumes only 2 streams in avi file, one video, one audio
 */

/*
 * FOURCC codes
 * These are reversed ie. RIFF=FFIR
 * Do NOT change these
 */
#define RIFF 0x46464952
#define _AVI 0x20495641 /*"AVI "*/
#define LIST 0x5453494c
#define HDRL 0x6c726468
#define STRL 0x6c727473
#define STRH 0x68727473 /*Not used*/
#define VIDS 0x73646976 /*vids*/
#define AUDS 0x73647561 /*auds*/
#define MIDS 0x7364696d /*mids*/
#define TXTS 0x73747874 /*txts*/
#define STRF 0x66727473
#define JUNK 0x4b4e554a
#define ODML 0x6c6d646f
#define INFO 0x4f464e49
/*
 * Format specific fourCC's
 */
#define _div3 0x33766964
#define _DIV3 0x33564944
#define _xvid 0x64697678
#define _XVID 0x44495658
#define _divx 0x78766964
#define _DIVX 0x58564944
#define _DX50 0x30355844
#define _wmv3 0x33766d77
#define _WMV3 0x33564d57
#define _wmv2 0x32766d77
#define _wmv1 0x31766d77


/*
 * here are audio codec fourCC's
 * I've selected only the most obvious/common
 * all are taken from Mmreg.h from MSVS
 * Renaming done be me
 * UPDATE: Some new added, basing on what I
 * found in my avi's
 */
#define WADPCM  0x0002
#define WFLOAT  0x0003
#define WMSNAU  0x0032
#define WMP3    0x0055
#define WVIVO   0x0111
#define WVIVOS  0x0112
#define WCADPCM 0x0200
#define WESSAC3 0x0241
#define WMPEG   0x0050
#define WDVM    0x2000

/*
 * INFOLIST header helper data
 */
#define _IART 0x54524149
#define _ICMT 0x544d4349
#define _ICOP 0x504f4349
#define _ICRD 0x44524349
#define _IENG 0x474e4549
#define _IGNR 0x524e4749
#define _INAM 0x4d414e49
#define _IPRD 0x44525049
#define _ISFT 0x54465349
#define _ISRC 0x43525349
#define _ISBJ 0x4a425349
#define _ITHC 0x43485449



/*
 * Pointed out to me by someone on #C++ that
 * I shoudl be using:
 * const static unsigned long mids = 0x7364696d;
 * and such, Must try it out later.
 */

/*
 * Definitions of common and/or long
 */
#define _FPS (1000000 / (float)avih.dwMicroSecPerFrame)
#define HX__ "0x%08x"
#define HXT_ "0x%08x, %c%c%c%C"
#define HXU_ "0x%08x, %u"
#define _INFOSIZE infoH.fSize + ((infoH.fSize % 4) == 0 ? 0 : (4 - infoH.fSize % 4))
#define _INFOSIZE_2 infoH.fSize + ((infoH.fSize % 2) == 0 ? 0 : (2 - infoH.fSize % 4))

/*
 * Global Variable
 * It shoudl NOT be done so
 */

/*
 * experimental, to be used later
 * 13.IV.2004 - First test passed
 * Remeber: int and long int are 4 bytes
 *	   short int is 2 bytes long
 *         char is 1 byte long
 */
union _DWORD {
    _UINT4 hex;
    char   txt[4];
} strfFourCC, strfFourCC2, listDW;

union _WORD {
    unsigned short hex;
    char           txt[2];
};

/*
 * Rather messy helper tool
 */
union _ALLC {
    _UINT4         hex;
    unsigned short shex[2];
    char           txt[4];
} allc[10];

struct myStdHeader {
    _DWORD header;
    _UINT4 fSize;
    _DWORD fType;
} topHeader, listH, strl, strf, strf2, junk, junk2, list1, list2, listI, listI2;

struct srtHeader {
    _DWORD header;
    _UINT4 fSize;
} infoH;
/*
 * taken from:
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directx9_c/directx/htm/avimainheaderstructure.asp
 * modified later.
 * Chceck your implementation if it's 4 bytes long (sizeof(unsigned long)==4);
 */
struct avih {
    _DWORD fcc;
    _UINT4 cb;
    _UINT4 dwMicroSecPerFrame;
    _UINT4 dwMaxBytesPerSec;
    _UINT4 dwPaddingGranularity;
    _UINT4 dwFlags;
    _UINT4 dwTotalFrames;
    _UINT4 dwInitialFrames;
    _UINT4 dwStreams;
    _UINT4 dwSuggestedBufferSize;
    _UINT4 dwWidth;
    _UINT4 dwHeight;
    _UINT4 dwReserved[4];
} avih;

/*
 * taken from:
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directx9_c/directx/htm/avistreamheaderstructure.asp
 * and later modified.
 */
struct _avistreamheader {
    _DWORD fcc;
    _UINT4 cb;
    _DWORD fccType;
    _DWORD fccHandler;
    _UINT4 dwFlags;
    _WORD  wPriority;
    _WORD  wLanguage;
    _UINT4 dwInitialFrames;
    _UINT4 dwScale;
    _UINT4 dwRate;
    _UINT4 dwStart;
    _UINT4 dwLength;
    _UINT4 dwSuggestedBufferSize;
    _INT4  dwQuality;    /*yes, this is signed data, -1==default*/
    _UINT4 dwSampleSize;
    struct {
        short int left;
        short int top;
        short int right;
        short int bottom;
    }  rcFrame;
} streamH, streamH2; /*16*/

/*
 * From:
 * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directx9_c/directx/htm/waveformatexstructure.asp
 */
struct WAVEFORMATEX {
    _WORD          wFormatTag;
    unsigned short nChannels;
    _UINT4         nSamplesPerSec;
    _UINT4         nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short wBitsPerSample;
    unsigned short cbSize;
} waveH;

/*
 * All of these well be null-treminated
 *(or some strange sequence-terminated)
 */
struct INFOLIST {
    char *IART;             /*Artist*/
    char *ICMT;             /*Comments*/
    char *ICOP;             /*Copyrights*/
    char *ICRD;             /*Creation Date*/
    char *IENG;             /*Engineer*/
    char *IGNR;             /*Gener*/
    char *INAM;             /*Name*/
    char *IPRD;             /*Product*/
    char *ISFT;             /*Software*/
    char *ISRC;             /*Source*/
    char *ISBJ;             /*Subject*/
    char *ITHC;             /*Technician*/
} infoList;                 /*12*/

unsigned int allcCount = 0; /*temportarily used for additional data stored after waveH*/

#include <iostream>
using namespace std;

QString parseAviHeader( FILE *file ) {
    long filelength = 0;

    DEBUG_INFO_ENABLED = init_debug_info();

    infoList.IART = NULL;
    infoList.ICMT = NULL;
    infoList.ICOP = NULL;
    infoList.ICRD = NULL;
    infoList.IENG = NULL;
    infoList.IGNR = NULL;
    infoList.INAM = NULL;
    infoList.IPRD = NULL;
    infoList.ISFT = NULL;
    infoList.ISRC = NULL;
    infoList.ISBJ = NULL;
    infoList.ITHC = NULL;

    fseek( file, 0L, SEEK_END );
    filelength = ftell( file );
    rewind( file );      /*fseek(stream, 0L, SEEK_SET);*/
    QString ReturnData = "";
    /*
     * Load Data
     * Since these are structs, we can point at the beginning and
     * overflow to other memebers.
     */
    if (filelength - ftell( file ) < (long)sizeof(_UINT4) * (3 + 3 + 16 + 3 + 16 + 3)) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
        return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
    }

    fread((void *)&topHeader.header, sizeof(_UINT4), 3, file );
    if (topHeader.header.hex != RIFF && topHeader.fType.hex != _AVI) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
#ifdef DoAviDebug
        ReturnData += QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        ReturnData += "\n";
        ReturnData += QString( "FOURCC1	= %2%3%4%5 (0x%1)\n" )
                      .arg( topHeader.header.hex )
                      .arg( topHeader.header.txt[0] )
                      .arg( topHeader.header.txt[1] )
                      .arg( topHeader.header.txt[2] )
                      .arg( topHeader.header.txt[3] );
        ReturnData += "\000";
        cout << qPrintable( ReturnData ) << endl;
#endif
        ReturnData += "\000";
        return ReturnData;
    }
    fread((void *)&listH.header.hex, sizeof(_UINT4), 3, file );
    if (listH.header.hex != LIST && listH.fType.hex != HDRL) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
#ifdef DoAviDebug
        ReturnData += QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        ReturnData += "\n";
        ReturnData += QString( "FOURCC1	= %2%3%4%5 (0x%1)\n" )
                      .arg( listH.header.hex )
                      .arg( listH.header.txt[0] )
                      .arg( listH.header.txt[1] )
                      .arg( listH.header.txt[2] )
                      .arg( listH.header.txt[3] );
        ReturnData += "\000";
        cout << qPrintable( ReturnData ) << endl;
#endif
        ReturnData += "\000";
        return ReturnData;
    }
    fread((void *)&avih.fcc.hex, sizeof(_UINT4), 16, file );
    fread((void *)&strl.header.hex, sizeof(_UINT4), 3, file );
    if (listH.header.hex != LIST && listH.fType.hex != STRL) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
#ifdef DoAviDebug
        ReturnData += QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        ReturnData += "\n";
        ReturnData += QString( "FOURCC1	= %1%2%3%4 (0x%5)\n" )
                      .arg( topHeader.header.txt[0] )
                      .arg( topHeader.header.txt[1] )
                      .arg( topHeader.header.txt[2] )
                      .arg( topHeader.header.txt[3] )
                      .arg( topHeader.header.hex );
        ReturnData += "\000";
        cout << qPrintable( ReturnData ) << endl;
#endif
        ReturnData += "\000";
        return ReturnData;
    }
    fread((void *)&streamH.fcc.hex, sizeof(_UINT4), 16, file );
    fread((void *)&strf.header.hex, sizeof(_UINT4), 3, file );
    /*
     * You can also find an idxh index header
     * Add IT!!!
     */
    if (strf.header.hex != STRF && strf.fType.hex != 0x00000028) {       /*Broken? 0x28=="(   "*/
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
#ifdef DoAviDebug
        ReturnData += QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        ReturnData += "\n";
        ReturnData += QString( "FOURCC1	= %1%2%3%4 (0x%5)\n" )

                      .arg( strf.header.txt[0] )
                      .arg( strf.header.txt[1] )
                      .arg( strf.header.txt[2] )
                      .arg( strf.header.txt[3] )
                      .arg( strf.header.hex );
        ReturnData += QString( "FOURCC2	= %1%2%3%4 (0x%5)\n" )
                      .arg( strf.fType.txt[0] )
                      .arg( strf.fType.txt[1] )
                      .arg( strf.fType.txt[2] )
                      .arg( strf.fType.txt[3] )
                      .arg( strf.fType.hex );
        ReturnData += "\000";
        cout << qPrintable( ReturnData ) << endl;
#endif
        ReturnData += "\000";
        return ReturnData;
    }
    fseek( file, 12, SEEK_CUR );
    if (filelength - ftell( file ) < (long)sizeof(_UINT4) * 1) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
        return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
    }
    fread((void *)&strfFourCC.hex, sizeof(_UINT4), 1, file );
    fseek( file, strf.fSize - 20, SEEK_CUR );
    fread((void *)&junk.header.hex, sizeof(_UINT4), 3, file );

    if (junk.header.hex != JUNK) {
        fseek( file, -12, SEEK_CUR );
    } else {
        fseek( file, junk.fSize - 4, SEEK_CUR );
    };

    if (filelength - ftell( file ) < (long)sizeof(_UINT4) * 21) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
        return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
    }
    fread((void *)&list1.header.hex, sizeof(_UINT4), 3, file );
    fread((void *)&streamH2.fcc.hex, sizeof(_UINT4), 16, file );
    fread((void *)&strf2.header.hex, sizeof(_UINT4), 3, file );
    if (strf2.header.hex != STRF) {       /*Broken? 0x28=="(   "*/
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
#ifdef DoAviDebug
        ReturnData += QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        ReturnData += "\n";
        ReturnData += QString( "FOURCC1	= %1%2%3%4 (0x%5)\n" )
                      .arg( strf2.header.txt[0] )
                      .arg( strf2.header.txt[1] )
                      .arg( strf2.header.txt[2] )
                      .arg( strf2.header.txt[3] )
                      .arg( strf2.header.hex );
        ReturnData += QString( "FOURCC2	= %1%2%3%4 (0x%5)\n" )
                      .arg( strf2.fType.txt[0] )
                      .arg( strf2.fType.txt[1] )
                      .arg( strf2.fType.txt[2] )
                      .arg( strf2.fType.txt[3] )
                      .arg( strf2.fType.hex );
        ReturnData += "\000";
        cout << qPrintable( ReturnData ) << endl;
#endif
        ReturnData += "\000";
        return ReturnData;
    }
    ;
    fseek( file, -4, SEEK_CUR );
    if (filelength - ftell( file ) < (long)sizeof(short int) * 9) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
        return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
    }
    fread((void *)&waveH.wFormatTag.hex, sizeof(short int), 9, file );
    if (waveH.cbSize > 0) {
        allcCount = (waveH.cbSize / 4) + (waveH.cbSize % 4 > 0 ? 1 : 0);
        int i = 0;

        if (filelength - ftell( file ) < (long)(allcCount * sizeof(long int))) {
            return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
        }
        while (i < (int)allcCount) {
            if (i >= 10) {
                break;                 // have to end the loop becuse we alloc the "allc" only 10 element
            }
            // (caused seg. fault. with some .avi)
            fread((void *)&allc[i].hex, sizeof(long int), 1, file );
            i++;
        }
        ;
    }
    ;
    /*
     * You can also find an idxh index header
     * Add IT!!!
     */
    fseek( file, -(waveH.cbSize), SEEK_CUR );         /*We'll chcek it if it's Junk or Something*/
    /*
     * everything is broken from here to writing data out
     */
    /*
     * Since most implementations are bad
     * we have to seek for LIST sequence
     * Quite Crude, but should be efficient in finding
     * not protected against EOF and such (ie. Borken Files)
     */
    listDW.hex = 0x0;
    if (*DEBUG_INFO_ENABLED) {
        std::cerr << "\t=> We'll chcek it if it's Junk or Something" << std::endl;
    }
    while (listDW.hex != LIST) {
        fread((void *)&listDW.txt[0], sizeof(char), 1, file );
        if ((int)listDW.txt[0] == EOF) {
            return ReturnData;
        }
        if ((int)listDW.txt[0] == 0x4c) {
            fread((void *)&listDW.txt[1], sizeof(char), 1, file );
        }
        if ((int)listDW.txt[1] == 0x49) {
            fread((void *)&listDW.txt[2], sizeof(char), 1, file );
        }
        ;
        if ((int)listDW.txt[2] == 0x53) {
            fread((void *)&listDW.txt[3], sizeof(char), 1, file );
        }
        ;
    }
    ;
    fseek( file, -4, SEEK_CUR );

    if (filelength - ftell( file ) < 4 * 3) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "Not a RIFF/AVI file OR header broken! " << std::endl;
        }
        return QObject::tr( "Not a RIFF/AVI file OR header broken!" );
    }
    fread((void *)&listI.header.hex, 4, 3, file );
    /*
     * We skip ODML data for now
     */
    if (listI.fType.hex == ODML) {
        listDW.hex = 0x00000000;         /*clear data*/
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "\t=> skip ODML data for now" << std::endl;
        }
        while (listDW.hex != LIST) {
            fread((void *)&listDW.txt[0], sizeof(char), 1, file );
            // if ( *DEBUG_INFO_ENABLED )
            //	std::cerr << "\t=> ( int ) listDW.txt[0]: " << ( int ) listDW.txt[0] << std::endl;
            if ((int)listDW.txt[0] == EOF || (int)listDW.txt[0] == 0) {
                return ReturnData;
            }
            if ((int)listDW.txt[0] == 0x4c) {
                fread((void *)&listDW.txt[1], sizeof(char), 1, file );
            }
            if ((int)listDW.txt[1] == 0x49) {
                fread((void *)&listDW.txt[2], sizeof(char), 1, file );
            }
            ;
            if ((int)listDW.txt[2] == 0x53) {
                fread((void *)&listDW.txt[3], sizeof(char), 1, file );
            }
            ;
        }
        ;
        fseek( file, -4, SEEK_CUR );
    }
    ;
    /*
     * now we are somewhere around LIST/INFO
     */
    if (filelength - ftell( file ) < 4 * 3) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "broken header " << std::endl;
        }
        return QString( "Broken header" );
    }
    fread((void *)&listI2.header.hex, 4, 3, file );
    if (listI2.fType.hex == INFO) {
        if (*DEBUG_INFO_ENABLED) {
            std::cerr << "\t=> now we are somewhere around LIST/INFO" << std::endl;
        }
        while (infoH.header.hex != JUNK && infoH.header.hex != LIST) {
            /*
             * Make it linear later
             * Ten minutes later: made it, and what a hack it is ><'
             * Two minutes later: made a better hack --'
             * Ten minutes later discovered it actualy fails, reverted to old version
             * Another three minutes showed why it wasn't working
             * Ten minutes later, donnerwetter, still fails
             */
            infoH.header.txt[0] = infoH.header.txt[1];
            infoH.header.txt[1] = infoH.header.txt[2];
            infoH.header.txt[2] = infoH.header.txt[3];
            /*infoH.header.hex >> 1;*/ /*
             * Why it doesn't work?
             * It does now, but it shoudl
             * have been working with 8
             *(ie. 8 bits, not 8 bytes)
             * Still brokem
             */
            fread((void *)&infoH.header.txt[3], 1, 1, file );
            /*
             * Too lazy to make it a separate function for it now
             * Remember to free memory of non-NULL pointers
             */
            switch (infoH.header.hex) {
            case _IART:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.IART = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.IART, _INFOSIZE_2, 1, file );
                break;
            case _ICMT:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ICMT = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ICMT, _INFOSIZE_2, 1, file );
                break;
            case _ICOP:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ICOP = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ICOP, _INFOSIZE_2, 1, file );
                break;
            case _ICRD:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ICRD = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ICRD, _INFOSIZE_2, 1, file );
                break;
            case _IENG:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.IENG = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.IENG, _INFOSIZE_2, 1, file );
                break;
            case _IGNR:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.IGNR = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.IGNR, _INFOSIZE_2, 1, file );
                break;
            case _INAM:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.INAM = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.INAM, _INFOSIZE_2, 1, file );
                break;
            case _IPRD:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.IPRD = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.IPRD, _INFOSIZE_2, 1, file );
                break;
            case _ISFT:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ISFT = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ISFT, _INFOSIZE_2, 1, file );
                break;
            case _ISRC:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ISRC = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ISRC, _INFOSIZE_2, 1, file );
                break;
            case _ISBJ:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ISBJ = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ISBJ, _INFOSIZE_2, 1, file );
                break;
            case _ITHC:
                fread((void *)&infoH.fSize, 4, 1, file );
                infoList.ITHC = (char *)calloc( _INFOSIZE_2, 1 );
                fread((void *)infoList.ITHC, _INFOSIZE_2, 1, file );
                break;
            case JUNK:
                break;
            default:
                break;
            }
            /*
             * if last sign is 0x0 move 2 bytes back,
             * if it's JUNK, leave, it may be also a miss aligned data
             */
        }
    }
    ;

    /*
     * We shoudl be done by now, nothing of interests here
     *
     * Here we actually write the data out,
     * this part can, and will be modified to print
     * out only that what we need.
     */
    QString sH, sM, sS;
    sH.sprintf( "%d", (int)((float)avih.dwTotalFrames / _FPS) / 3600 );
    sM.sprintf( "%02d", (int)((float)avih.dwTotalFrames / _FPS) % 3600 / 60 );
    sS.sprintf( "%02d", (int)((float)avih.dwTotalFrames / _FPS) % 60 );

    ReturnData += QObject::tr( "Video:\n" );
    fourccSwitch( strfFourCC.hex, ReturnData );
    ReturnData += QObject::tr( "Total Time" ) + QString( " = %1:%2:%3\n" ).arg( sH ).arg( sM ).arg( sS );
    ReturnData += QObject::tr( "Framerate" ) + QString( " = %1 f/s\n" ).arg( _FPS, 0, 'f', 3 );
    ReturnData += QObject::tr( "Resolution" ) + QString( " = %1x%2\n" ).arg( avih.dwWidth ).arg( avih.dwHeight );
    ReturnData += "\n";
    ReturnData += QObject::tr( "Audio:\n" );
    fourccSwitch2( waveH.wFormatTag.hex, ReturnData );
    ReturnData += QObject::tr( "Channels" ) + QString( " = %1 \n" ).arg( waveH.nChannels );
    ReturnData += QObject::tr( "Sample/s" ) + QString( " = %1kHz \n" ).arg((float)waveH.nSamplesPerSec / 1000, 0, 'f', 1 );            /*use waveH.nSamplesPerSec if you want to*/
    ReturnData += QObject::tr( "Bitrate" ) + QString( " = %1 kBit\n" ).arg((waveH.nAvgBytesPerSec * 8) / 1000 );

    ReturnData += "\n";
    ReturnData += QObject::tr( "More Data:\n" );

    if (infoList.IART != NULL) {
        ReturnData += QObject::tr( "Artist" ) + QString( " = %1\n" ).arg( infoList.IART );
    }
    if (infoList.ICMT != NULL) {
        ReturnData += QObject::tr( "Comments" ) + QString( " = %1\n" ).arg( infoList.ICMT );
    }
    if (infoList.ICOP != NULL) {
        ReturnData += QObject::tr( "Copyright" ) + QString( " = %1\n" ).arg( infoList.ICOP );
    }
    if (infoList.ICRD != NULL) {
        ReturnData += QObject::tr( "Creation Date" ) + QString( " = %1\n" ).arg( infoList.ICRD );
    }
    if (infoList.IENG != NULL) {
        ReturnData += QObject::tr( "Engineer" ) + QString( " = %1\n" ).arg( infoList.IENG );
    }
    if (infoList.IGNR != NULL) {
        ReturnData += QObject::tr( "Genre" ) + QString( " = %1\n" ).arg( infoList.IGNR );
    }
    if (infoList.INAM != NULL) {
        ReturnData += QObject::tr( "Name" ) + QString( " = %1\n" ).arg( infoList.INAM );
    }
    if (infoList.IPRD != NULL) {
        ReturnData += QObject::tr( "Product" ) + QString( " = %1\n" ).arg( infoList.IPRD );
    }
    if (infoList.ISFT != NULL) {
        ReturnData += QObject::tr( "Software" ) + QString( " = %1\n" ).arg( infoList.ISFT );
    }
    if (infoList.ISRC != NULL) {
        ReturnData += QObject::tr( "Source" ) + QString( " = %1\n" ).arg( infoList.ISRC );
    }
    if (infoList.ISBJ != NULL) {
        ReturnData += QObject::tr( "Subject" ) + QString( " = %1\n" ).arg( infoList.ISBJ );
    }
    if (infoList.ITHC != NULL) {
        ReturnData += QObject::tr( "Technician" ) + QString( " = %1\n" ).arg( infoList.ITHC );
    }

    freeINFOLIST();
    ReturnData += "\0";     // Protection?
//	cout << "\n" << ReturnData;
    return ReturnData;
}


/*
 * This is used to print out
 * information about found codec FourCC's
 */
void fourccSwitch( unsigned long fcc, QString &ReturnData ) {
    switch (fcc) {
    case _DIV3:
    case _div3:
        ReturnData += "DivX MPEG-4 ver.3 (Low Motion)\n";
        break;
    case _DX50:
        ReturnData += "DivX MPEG-4 ver.5 \n";
        break;
    case _XVID:
    case _xvid:
        ReturnData += "XVID MPEG-4\n";
        break;
    case _DIVX:
    case _divx:
        ReturnData += "DivX MPEG-4 v4.x/v5.x OR OpenDivX MPEG-4 or derived from it\n";
        break;
    case _WMV3:
    case _wmv3:
        ReturnData += "Microsoft Windows Media Video 9\n";
        break;
    case _wmv2:
        ReturnData += "Microsoft Windows Media Video 8\n";
        break;
    case _wmv1:
        ReturnData += "Microsoft Windows Media Video 7\n";
        break;
    case 0x0:
        ReturnData += "Broken/Look in strf\n";
        break;
    default:
        ReturnData += "Unknown/not implemented/broken header\n";
    }
    ;
}

/*
 * these are not actually fourcc's!
 * they are 2 bytes long
 */
void fourccSwitch2( unsigned short fcc, QString &ReturnData ) {
    switch (fcc) {
    case WADPCM:
        ReturnData += "Microsoft ADPCM  \n";
        break;
    case WFLOAT:
        ReturnData += "Microsoft IEEE-Float ADPCM \n";
        break;
    case WMSNAU:
        ReturnData += "Microsoft NAUDIO(?) \n";
        break;
    case WMP3:
        ReturnData += "ISO/MPEG Layer-3 \n";
        break;
    case WMPEG:
        ReturnData += "MPEG Layer-2 or MPEG Layer-3 \n";
        break;
    case WVIVO:
        ReturnData += "Vivo Software G.723 \n";
        break;
    case WVIVOS:
        ReturnData += "Vivo Software Siren \n";
        break;
    case WCADPCM:
        ReturnData += "Creative ADPCM \n";
        break;
    case WESSAC3:
        ReturnData += "ESS Technology, Inc. AC3 \n";
        break;
    case WDVM:
        ReturnData += "AC3 Stream OR FAST Multimedia AG DVM\n";
        break;
    default:
        ReturnData += QObject::tr( "Unknown/not implemented/broken header\n" );
    }
    ;
}

void freeINFOLIST( void ) {
    if (infoList.IART != NULL) {
        free( infoList.IART );
        infoList.IART = NULL;
    }
    ;
    if (infoList.ICMT != NULL) {
        free( infoList.ICMT );
        infoList.ICMT = NULL;
    }
    ;
    if (infoList.ICOP != NULL) {
        free( infoList.ICOP );
        infoList.ICOP = NULL;
    }
    ;
    if (infoList.ICRD != NULL) {
        free( infoList.ICRD );
        infoList.ICRD = NULL;
    }
    ;
    if (infoList.IENG != NULL) {
        free( infoList.IENG );
        infoList.IENG = NULL;
    }
    ;
    if (infoList.IGNR != NULL) {
        free( infoList.IGNR );
        infoList.IGNR = NULL;
    }
    ;
    if (infoList.INAM != NULL) {
        free( infoList.INAM );
        infoList.INAM = NULL;
    }
    ;
    if (infoList.IPRD != NULL) {
        free( infoList.IPRD );
        infoList.IPRD = NULL;
    }
    ;
    if (infoList.ISFT != NULL) {
        free( infoList.ISFT );
        infoList.ISFT = NULL;
    }
    ;
    if (infoList.ISRC != NULL) {
        free( infoList.ISRC );
        infoList.ISRC = NULL;
    }
    ;
    if (infoList.ISBJ != NULL) {
        free( infoList.ISBJ );
        infoList.ICOP = NULL;
    }
    ;
    if (infoList.ITHC != NULL) {
        free( infoList.ITHC );
        infoList.ITHC = NULL;
    }
    ;
}
