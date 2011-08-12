MAC OS/X compile help

requirements:

libtar: ftp://ftp.feep.net/pub/software/libtar/libtar-1.2.11.tar.gz
	- unpack and compile:
		1. tar xvfz libtar-1.2.11.tar.gz
		2. cd libtar-1.2.11
		4. configure
		5. make

libzip2: http://bzip.org/1.0.6/bzip2-1.0.6.tar.gz
	- unpack and compile:
		1. tar bzip2-1.0.6.tar.gz
		2. cd bzip2-1.0.6
		4. configure
		5. make


p7zip source: http://dfn.dl.sourceforge.net/project/p7zip/p7zip/9.20.1/p7zip_9.20.1_src_all.tar.bz2
	- compile:
		1. unpack p7zip source
		2. copy makefile.macosx_32bits / makefile.macosx_64bits to makefile.machine
		3. configure
		4. make


lib7zip (>= 1.4.1): http://code.google.com/p/lib7zip/downloads/detail?name=lib7zip-1.4.1.tar.gz&can=3&q=
	- compile:
		1. unpack 7zip source 
		2. set P7ZIP_SOURCE_DIR=<full path to p7zip_9.20.1 dir>
		3. cd lib7zip-1.4.1
		4. ./configure
		5. make 7z

zlib: http://zlib.net/zlib-1.2.5.tar.gz
	- compile:
		1. unpack zlib source
		2. cd zlib-1.2.5
		3. configure
		4. make

Qt4: http://get.qt.nokia.com/qt/source/qt-mac-opensource-4.7.1.dmg


mediainfo: http://mediainfo.sourceforge.net/
	- for compiling only MediaInfoDLL is required (MediaInfoDLL/MediaInfoDLL.h), it can be downloaded here: http://mediainfo.svn.sourceforge.net/viewvc/mediainfo/MediaInfoLib/trunk/Source/MediaInfoDLL/MediaInfoDLL.h?revision=3213
	- for usage and/or compiling install mediainfo (http://mediainfo.sourceforge.net/Download)

cdcat compile:
	library path could be /usr/lib, include path could be /usr/include
	- add libtar.h to include path
	- add libtar_listhash.h to include path
	- add bzlib.h to include path
	- add zlib.h to include path
	- add bzip2.a to library path
	- add libtar.a to library path
	- add libz.a to library path

cdcat run:
	- runtime: 7z.so in library path