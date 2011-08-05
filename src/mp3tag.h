/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#include <stdio.h>


#ifndef MP3_TAG_H
#define MP3_TAG_H

class ReadMp3Tag {
public:

    ReadMp3Tag ( void );
    ReadMp3Tag ( const char *fn,bool v1_over_v2p );
    ~ReadMp3Tag ( void );

    int  Work ( const char *filename );

    bool readed ( void ) const { return bTAGreaded;     };
    bool exist ( void ) const { return bTAGexist;      };

    char *year ( void ) const;
    char *album ( void ) const;
    char *artist ( void ) const;
    char *title ( void ) const;
    char *comment ( void ) const;
    char *gettechinfo ( void ) const;
    int         tnum ( void ) const;



private:

    int  ReadTAGv1 ( void );
    int  ReadTAGv2 ( void );
    int  technicalInfo ( void );

    FILE *mp3;
    bool v1_over_v2;
    bool bTAGreaded,bTAGexist;
    int  tnumber;

    char *Year;
    char *Album;
    char *Artist;
    char *Title;
    char *Comment;
    char *Track;

    char *techinfo;

    char * strbcut ( char *a );
};




#endif

