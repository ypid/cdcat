LINUX compile help

requirements:
zlib, bzip2, libtar, qt4

Most modern Linux distros have packages:
install zlib1g-dev, libtar-dev, libzip2-dev, p7zip-full, libqt4-dev, qt4-dev-tools, 

p7zip (optional) source: http://dfn.dl.sourceforge.net/project/p7zip/p7zip/9.20.1/p7zip_9.20.1_src_all.tar.bz2
		1. unpack p7zip source

lib7zip (>= 1.4.1, optional): http://code.google.com/p/lib7zip/downloads/detail?name=lib7zip-1.4.1.tar.gz&can=3&q=
	- compile:
		1. unpack 7zip source 
		2. set P7ZIP_SOURCE_DIR=<full path to p7zip_9.20.1 dir>
		3. cd lib7zip-1.4.1
		4. ./configure
		5. make
		6. copy lib7zip-1.4.1/Lib7Zip/lib7zip.a to /usr/local/lib

libcrypto++ (optional) source: http://www.cryptopp.com/wiki/Linux
	- it will be used for catalog encryption, enable in cdcat.pro (can be disabled in cdcat.pro)

mediainfo: http://mediainfo.sourceforge.net/
	- for use dynamic mediainfo support only MediaInfoDLL is at compile time required (MediaInfoDLL/MediaInfoDLL.h), it can be downloaded here: http://mediainfo.svn.sourceforge.net/viewvc/mediainfo/MediaInfoLib/trunk/Source/MediaInfoDLL/MediaInfoDLL.h?revision=3213
	- for disable mediainfo completly use -DNO_MEDIAINFO or tweak cdcat.pro
	- for usage and/or compiling install mediainfo (http://mediainfo.sourceforge.net/Download)

cdcat compile:
	library path could be /usr/lib, include path could be /usr/include
	- add libtar_listhash.h to include path
	- edit cdcat.pro (for disable lib7zip, e.g.)
	- qmake cdcat.pro
	- make
	- lrelease cdcat.pro

cdcat run:
	- runtime: 7z.so in library path