/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include "config.h"
#include "cdcat.h"
#include "mp3tag.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#define TITLE  0
#define ARTIST 1
#define ALBUM  2
#define TRACK  3
#define GENRE  4
#define COMM   5
#define YEAR   8

#define FREE_AND_ALLOC( a, b ) { if (a) { delete [] a; a = NULL; } a = new char[b]; if (b) { *a = 0; } }

#define MAX    9

char markers[][MAX] = { "TIT2", "TPE1", "TALB", "TRCK", "TCON", "COMM", "TCOP", "TENC", "TYER" };

unsigned char found_error;


ReadMp3Tag::ReadMp3Tag ( void ) : bTAGreaded( false ), bTAGexist( false ), tnumber( 0 ) {
    mp3 = NULL;
    techinfo = NULL;
    Year = NULL;
    Album = NULL;
    Artist = NULL;
    Title = NULL;
    Comment = NULL;
    Track = NULL;

    bTAGreaded = false;
    techinfo = NULL;
    v1_over_v2 = false;
}

ReadMp3Tag::ReadMp3Tag ( const char *fn, bool v1_over_v2p ) : bTAGreaded( false ), bTAGexist( false ), tnumber( 0 ) {
    mp3 = NULL;
    techinfo = NULL;
    Year = NULL;
    Album = NULL;
    Artist = NULL;
    Title = NULL;
    Comment = NULL;
    Track = NULL;
    techinfo = NULL;
    v1_over_v2 = false;

    v1_over_v2 = v1_over_v2p;
    Work( fn );
}

ReadMp3Tag::~ReadMp3Tag ( void ) {
    if (techinfo != NULL) {
        delete [] techinfo; techinfo = NULL;
    }
    if (Year != NULL) {
        delete [] Year;     Year = NULL;
    }
    if (Album != NULL) {
        delete [] Album;    Album = NULL;
    }
    if (Artist != NULL) {
        delete [] Artist;   Artist = NULL;
    }
    if (Title != NULL) {
        delete [] Title;    Title = NULL;
    }
    if (Comment != NULL) {
        delete [] Comment;  Comment = NULL;
    }
}

char *mmstr( const char *imp ) {
    if (imp == NULL) {
        return NULL;
    }
    size_t len = strlen( imp );
    char *x = new char[len + 1];
    return (char *)memcpy( x, imp, len + 1 );
}

char *ReadMp3Tag::year( void ) const {
    return Year == NULL ? mmstr( " " ) : mmstr( Year );
}

char *ReadMp3Tag::album( void ) const {
    return Album == NULL ? mmstr( " " ) : mmstr( Album );
}

char *ReadMp3Tag::artist( void ) const {
    return Artist == NULL ? mmstr( " " ) : mmstr( Artist );
}

char *ReadMp3Tag::title( void ) const {
    return Title == NULL ? mmstr( " " ) : mmstr( Title );
}

char *ReadMp3Tag::comment( void ) const {
    return Comment == NULL ? mmstr( " " ) : mmstr( Comment );
}

char *ReadMp3Tag::gettechinfo( void ) const {
    return mmstr( techinfo );
}




int ReadMp3Tag::tnum( void ) const {
    return tnumber;
}


char *ReadMp3Tag::strbcut( char *a ) {   // Oly used by A ReadTAG function
    for (int i = strlen( a ) - 1; i > 0; i--)
        if (a[i] != ' ') {
            a[i + 1] = '\0';
            return a;
        }
    a[1] = '\0';
    return a;
}

int ReadMp3Tag::Work( const char *filename ) {
    DEBUG_INFO_ENABLED = init_debug_info();
    if (*DEBUG_INFO_ENABLED) {
        cerr << "Load file: " << filename << " ...";
    }
    bTAGreaded = 1;
    bTAGexist = 0;
    if ((mp3 = fopen( filename, "rb" )) == NULL) {
        bTAGexist = 0;
        found_error = 1;
        fprintf( stderr, "I can't open a .mp3 file: %s\n", filename );
        return -1;
    }
    if (v1_over_v2) {
        bTAGexist = ReadTAGv1();
        if (bTAGexist == 0) {
            bTAGexist = ReadTAGv2();
        }
    } else {
        bTAGexist = ReadTAGv2();
        if (bTAGexist == 0) {
            bTAGexist = ReadTAGv1();
        }
    }

    technicalInfo();
    fclose( mp3 );
    return 0;
}

/*********************************************************************
 *  Read ID3 tag version 1
 **********************************************************************/
int ReadMp3Tag::ReadTAGv1( void ) {
    char TAG[4] = { 0 };

    //         X AL AR TI TAG
    fseek( mp3, -128, SEEK_END );

    fread( TAG, sizeof(char), 3, mp3 );
    if (strncmp( TAG, "TAG", 3 )) {
        return 0; // Id3 tag not exists
    }
    FREE_AND_ALLOC( Title, 31 );
    FREE_AND_ALLOC( Artist, 31 );
    FREE_AND_ALLOC( Album, 31 );
    FREE_AND_ALLOC( Year, 5 );
    FREE_AND_ALLOC( Comment, 31 );

    fread( Title, sizeof(char), 30, mp3 );  Title  [30] = '\0';
    fread( Artist, sizeof(char), 30, mp3 );  Artist [30] = '\0';
    fread( Album, sizeof(char), 30, mp3 );  Album  [30] = '\0';
    fread( Year, sizeof(char), 4, mp3 );    Year   [4] = '\0';
    fread( Comment, sizeof(char), 30, mp3 ); Comment[30] = '\0';

    tnumber = 0;
    if (Comment[28] == 0) {
        tnumber = Comment[29];
        Comment[28] = '\0';
    }

    strbcut( Artist );
    strbcut( Title );
    strbcut( Year );
    strbcut( Album );
    strbcut( Comment );
    return 1;
}

/*********************************************************************
 *  Read ID3 tag version 2
 **********************************************************************/
int ReadMp3Tag::ReadTAGv2( void ) {
    char *tag = NULL;
    unsigned long len;
    unsigned long flen;
    unsigned char c1, c2, c3, c4, c5;

    unsigned int pos = 0;
    unsigned int framelen;
    char id[5];

    id[4] = '\0';

    fseek( mp3, 0, SEEK_END );
    flen = ftell( mp3 );
    fseek( mp3, 0, SEEK_SET );

    c1 = getc( mp3 );
// I
    c2 = getc( mp3 );
// D
    c3 = getc( mp3 );
// 3
    c4 = getc( mp3 );
// ver
    c5 = getc( mp3 );
// ver

    if (!(c1 == 'I' && c2 == 'D' && c3 == '3' && (c4 == 2 || c4 == 3))) {
        return 0; // tag doesn't exist
    }
    fseek( mp3, 6, SEEK_SET );
    c1 = getc( mp3 );
    c2 = getc( mp3 );
    c3 = getc( mp3 );
    c4 = getc( mp3 );
    len = c1 * 2097152 + c2 * 16384 + c3 * 128 + c4;


    if (len > flen) {
        return 0; // tag corrupt
    }
    // read tag:
    tag = new char[len];

    fread( tag, sizeof(char), len, mp3 );

    for (pos = 0; pos < len; ) {
        id[0] = *(tag + pos++);
        id[1] = *(tag + pos++);
        id[2] = *(tag + pos++);
        id[3] = *(tag + pos++);

        framelen = 0;
        framelen += (*(tag + pos++)) * 2097152;
        framelen += (*(tag + pos++)) * 16384;
        framelen += (*(tag + pos++)) * 128;
        framelen += (*(tag + pos++));

        pos += 2; // skip 2 bytes

        if (framelen > len || pos + framelen > len) { // Corrupt frame (There is a lot time this need to run :-( )
            break;
        }

        if (!strncmp( id, "", 4 )) { // End of potential tags
            break;
        }

        if (!strncmp( id, markers[TITLE], 4 )) {
            if (framelen > 1) {
                FREE_AND_ALLOC( Title, framelen );
                memcpy( Title, tag + pos + 1, framelen - 1 );
                Title[framelen - 1] = '\0';
            }
        }
        if (!strncmp( id, markers[ARTIST], 4 )) {
            if (framelen > 1) {
                FREE_AND_ALLOC( Artist, framelen );
                memcpy( Artist, tag + pos + 1, framelen - 1 );
                Artist[framelen - 1] = '\0';
            }
        }
        if (!strncmp( id, markers[ALBUM], 4 )) {
            if (framelen > 1) {
                FREE_AND_ALLOC( Album, framelen );
                memcpy( Album, tag + pos + 1, framelen - 1 );
                Album[framelen - 1] = '\0';
            }
        }
        if (!strncmp( id, markers[YEAR], 4 )) {
            if (framelen > 1) {
                FREE_AND_ALLOC( Year, framelen );
                memcpy( Year, tag + pos + 1, framelen - 1 );
                Year[framelen - 1] = '\0';
            }
        }
        if (!strncmp( id, markers[COMM], 4 )) {
            if (framelen > 5) {
                FREE_AND_ALLOC( Comment, framelen );
                memcpy( Comment, tag + pos + 5, framelen - 5 );
                Comment[framelen - 5] = '\0';
            }
        }
        if (!strncmp( id, markers[TRACK], 4 )) {
            if (framelen > 5) {
                FREE_AND_ALLOC( Track, framelen );
                memcpy( Track, tag + pos + 1, framelen - 1 );
                Track[framelen - 1] = '\0';
                tnumber = atoi( Track );
            }
        }
        pos += framelen;
    } // end frame cycle
    delete [] tag;
    return 1;
}

/*********************************************************************
*
* Get some technical info from mp3
*
* I used some code from:
*   mp3guessenc version 0.2 by Naoki Shibata
*  (shibatch.sf.net)
*
*********************************************************************/
#define NUMTOCENTRIES 100

int GetVbrTag( unsigned char *buf );

#define MPG_MD_STEREO       0
#define MPG_MD_JOINT_STEREO 1
#define MPG_MD_DUAL_CHANNEL 2
#define MPG_MD_MONO         3

static long freqs[9] = { 44100, 48000, 32000,
                         22050, 24000, 16000,
                         11025, 12000, 8000 };

static int tabsel_123[2][3][16] = {
    { { 128, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, },
        { 128, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, },
        { 128, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, } },

    { { 128, 32, 48, 56, 64,  80,  96,  112, 128, 144, 160, 176, 192, 224, 256, },
        { 128, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, },
        { 128, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, } }
};

#define VBRHEADERSIZE (NUMTOCENTRIES + 4 + 4 + 4 + 4 + 4 + 20)

int Gmode = 0, Gbitrate = 0, Gfreq = 0, Gfsize = 0, Glsf = 0;

static const char VBRTag[] = { "Xing" };

int GetVbrTag( unsigned char *buf ) {
    int h_id, h_mode, h_sr_index;

    h_id = (buf[1] >> 3) & 1;
    h_sr_index = (buf[2] >> 2) & 3;
    h_mode = (buf[3] >> 6) & 3;

    if (h_id) {
        if (h_mode != 3) {
            buf += (32 + 4);
        } else {
            buf += (17 + 4);
        }
    } else {
        if (h_mode != 3) {
            buf += (17 + 4);
        } else {
            buf += (9 + 4);
        }
    }

    if (buf[0] != VBRTag[0]) {
        return 0;
    }
    if (buf[1] != VBRTag[1]) {
        return 0;
    }
    if (buf[2] != VBRTag[2]) {
        return 0;
    }
    if (buf[3] != VBRTag[3]) {
        return 0;
    }
    return 1;
}

int head_check2( unsigned long head ) {
    int lsf, srate, nch, lay, mpeg25, freq, mode;
    int padding, bitrate_index, framesize;

    if ((head & 0xffe00000) != 0xffe00000) {
        return 0;
    }
    if (!((head >> 17) & 3)) {
        return 0;
    }
    if (((head >> 12) & 0xf) == 0xf) {
        return 0;
    }
    if (((head >> 10) & 0x3) == 0x3) {
        return 0;
    }
    // if ((head & 0xffff0000) == 0xfffe0000) return 0;

    if (head & (1 << 20)) {
        lsf = (head & (1 << 19)) ? 0x0 : 0x1;
        mpeg25 = 0;
    } else {
        lsf = 1;
        mpeg25 = 1;
    }

    if (mpeg25) {
        srate = 6 + ((head >> 10) & 0x3);
    } else {
        srate = ((head >> 10) & 0x3) + (lsf * 3);
    }

    bitrate_index = ((head >> 12) & 0xf);
    padding = ((head >> 9) & 0x1);
    mode = ((head >> 6) & 0x3);
    freq = freqs[srate];
    nch = (mode == MPG_MD_MONO) ? 1 : 2;
    lay = 4 - ((head >> 17) & 3);

    Gmode = mode;
    Gbitrate = tabsel_123[lsf][lay - 1][bitrate_index];
    Gfreq = freq;
    Glsf = lsf;

    if (lay != 3) {
        return 0;
    }
    if (bitrate_index == 0) {
        return -1;                       // free format bitstream
    }
    framesize = tabsel_123[lsf][lay - 1][bitrate_index] * 144000 / (freqs[srate] << lsf) + padding;
    return framesize;
}


int resync_mp3( FILE *fp, int pos ) {
    int i;
    unsigned long head;

    for (i = 0; i < 65536; i++) {
        fseek( fp, pos + i, SEEK_SET );
        head = 0;
        head |= getc( fp ); head = head << 8;
        head |= getc( fp ); head = head << 8;
        head |= getc( fp ); head = head << 8;
        head |= getc( fp );

        if (feof( fp )) {
            return -1;
        }
        if ((Gfsize = head_check2( head ))) {
            return pos + i;
        }
    }

    return -1;
}

int ReadMp3Tag::technicalInfo( void ) {
    char retvalue[512];
    int pos, time;
    long flength;

    fseek( mp3, 0, SEEK_END );
    flength = ftell( mp3 );

    fseek( mp3, 0, SEEK_SET );
    pos = 0;

    {
        unsigned char c1, c2, c3, c4;
        c1 = getc( mp3 );
        c2 = getc( mp3 );
        c3 = getc( mp3 );
        c4 = getc( mp3 );
        if (c1 == 'I' && c2 == 'D' && c3 == '3' && c4 == 2) {
            fseek( mp3, 6, SEEK_SET );
            c1 = getc( mp3 );
            c2 = getc( mp3 );
            c3 = getc( mp3 );
            c4 = getc( mp3 );
            pos = c1 * 2097152 + c2 * 16384 + c3 * 128 + c4;
        }
    }

    // find first frame
    pos = resync_mp3( mp3, pos );

    if (pos == -1) {
        return -1;
    }

    {
        unsigned char buf[VBRHEADERSIZE + 36];
        fseek( mp3, pos, SEEK_SET );
        fread( &buf, 1, VBRHEADERSIZE + 36, mp3 );
        if (GetVbrTag( buf )) {
            Gbitrate = 0;// VBR
        }
    }

    time = ((flength - pos) / Gfsize) * 576 * (Glsf ? 1 : 2) / Gfreq;

    if (Gbitrate == 0) {
        sprintf( retvalue, "VBR,%dHz#%s",
                 Gfreq,
                 Gmode == 3 ? "Mono" : Gmode == 0 ? "Simple stereo" : Gmode == 1 ? "Joint stereo" : "Dual channel" );
    } else {
        sprintf( retvalue, "%d:%d, %d kbps#%dHz, %s",
                 (int)(time / 60),
                 time % 60,
                 Gbitrate,
                 Gfreq,
                 Gmode == 3 ? "Mono" : Gmode == 0 ? "Simple stereo" : Gmode == 1 ? "Joint stereo" : "Dual channel" );
    }

    techinfo = mmstr( retvalue );
    return 0;
}
/*end of file*/
