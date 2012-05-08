OS2/ecomstation compile help

requirements:
mingw: http://sourceforge.net/projects/mingw/files/Automated MinGW Installer/mingw-get-inst/mingw-get-inst-20110802/mingw-get-inst-20110802.exe/download

libtar: ftp://ftp.feep.net/pub/software/libtar/libtar-1.2.11.tar.gz
	- unpack and apply libtar_win32.patch:
		1. tar xvfz libtar-1.2.11.tar.gz
		2. cd libtar-1.2.11
		4. configure
		5. make

libzip2: installable via yum

7zip (optional) source: http://dfn.dl.sourceforge.net/project/p7zip/p7zip/9.20.1/p7zip_9.20.1_src_all.tar.bz2
	- only needed for compiling lib7zip
	- requires os2 patch (see p7zip_9.20.1-os2.patch)

7zip bin: ftp://hobbes.nmsu.edu/pub/os2/util/archiver/p7zip-9.20.1-os2.zip

lib7zip (>= 1.4.1, optional): http://code.google.com/p/lib7zip/downloads/detail?name=lib7zip-1.6.0.tar.gz&can=3&q=
	- compile:
		1. unpack p7zip source
		2. cd p7zip-9.20.1
		3. patch -p1 < p7zip_9.20.1-os2.patch
		4. cd ..
		5. unpack lib7zip source
		6. set P7ZIP_SOURCE_DIR=<full path to p7zip-9.20.1 source dir>
		7. patch -p0 < lib7zip-1.6.0-os2.patch
		8. cd lib7zip-1.6.0/Lib7Zip
		5. sh ../configure --enable-static CFLAGS="-DUNICODE=1 -D_UNICODE -D_OS2" CXXFLAGS="-DUNICODE=1 -D_UNICODE" CPPFLAGS="-DUNICODE=1 -D_UNICODE -D_OS2"
		6. make

zlib: installable via yum

Qt4: installable via yum

mediainfo: http://mediainfo.sourceforge.net/
	- for compiling mediainfo.a is required
	- for disable mediainfo completly use -DNO_MEDIAINFO or tweak cdcat.pro
	- for usage and/or compiling install mediainfo (http://mediainfo.sourceforge.net/Download)
	- if you compile from source use apply mediainfo-0.7.56_crash_fix.patch

cdcat compile:
	- add libtar.h to include path
	- add libtar.a to library path
	- edit cdcat.pro (for disable lib7zip, e.g.)
	- qmake cdcat.pro
	- make
	- lrelease cdcat.pro

cdcat run:
	- runtime: 7z.dll