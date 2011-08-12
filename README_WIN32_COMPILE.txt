WIN32 compile help

requirements:
mingw: http://sourceforge.net/projects/mingw/files/Automated MinGW Installer/mingw-get-inst/mingw-get-inst-20110802/mingw-get-inst-20110802.exe/download

libtar: ftp://ftp.feep.net/pub/software/libtar/libtar-1.2.11.tar.gz
	- unpack and apply libtar_win32.patch:
		1. tar xvfz libtar-1.2.11.tar.gz
		2. cd libtar-1.2.11
		3. patch -p1 < ../libtar_win32.patch
		4. configure
		5. make

libzip2: http://downloads.sourceforge.net/project/gnuwin32/bzip2/1.0.5/bzip2-1.0.5-bin.zip

7zip source: http://sourceforge.net/projects/sevenzip/files/7-Zip/9.22/7z922.tar.bz2/download
	- only needed for compilung lib7zip

7zip bin: http://sourceforge.net/projects/sevenzip/files/7-Zip/9.22/7z922.exe/download

lib7zip (>= 1.4.1): http://code.google.com/p/lib7zip/downloads/detail?name=lib7zip-1.4.1.tar.gz&can=3&q=
	- compile:
		1. unpack 7zip source 
		2. set P7ZIP_SOURCE_DIR=<full path to 7z922 dir>
		3. cd lib7zip-1.4.1\Lib7Zip
		4. sh
		5. CFLAGS="-DUNICODE=1 -D_UNICODE" CXXFLAGS="-DUNICODE=1 -D_UNICODE" CPPFLAGS="-DUNICODE=1 -D_UNICODE" ../configure
		6. make 7z

zlib: http://zlib.net/zlib-1.2.5.tar.gz
	- compile:
		1. unpack zlib source
		2. cd zlib-1.2.5
		3. configure
		4. make

Qt4: http://qt.nokia.com/downloads/windows-cpp

mediainfo: http://mediainfo.sourceforge.net/
	- for compiling only MediaInfoDLL is required (MediaInfoDLL/MediaInfoDLL.h), it can be downloaded here: http://mediainfo.svn.sourceforge.net/viewvc/mediainfo/MediaInfoLib/trunk/Source/MediaInfoDLL/MediaInfoDLL.h?revision=3213
	- for usage and/or compiling install mediainfo (http://mediainfo.sourceforge.net/Download)

cdcat compile:
	- add libtar.h to include path
	- add bzlib.h to include path
	- add zlib.h to include path
	- add bzip2.dll to library path
	- add libtar.a to library path
	- add libz.a to library path

cdcat run:
	- runtime: bzip2.dll, 7z.dll