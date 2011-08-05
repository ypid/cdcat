LINUX compile help

requirements:
zlib, bzip2, libtar, qt4

Most modern Linux distros have packages:
install zlib1g-dev, libtar-dev, libzip2-dev, p7zip-full, libqt4-dev, qt4-dev-tools, 

p7zip source: http://dfn.dl.sourceforge.net/project/p7zip/p7zip/9.20.1/p7zip_9.20.1_src_all.tar.bz2
		1. unpack p7zip source

lib7zip (>= 1.4.1): http://code.google.com/p/lib7zip/downloads/detail?name=lib7zip-1.4.1.tar.gz&can=3&q=
	- compile:
		1. unpack 7zip source 
		2. set P7ZIP_SOURCE_DIR=<full path to p7zip_9.20.1 dir>
		3. cd lib7zip-1.4.1
		4. ./configure
		5. make 7z

cdcat compile:
	library path could be /usr/lib, include path could be /usr/include
	- add libtar_listhash.h to include path

cdcat run:
	- runtime: 7z.so in library path