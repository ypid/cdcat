VERSION         =   0.99
FORMS      	= help.ui
TARGET          = cdcat
TRANSLATIONS	= lang/cdcat_hu.ts \
                  lang/cdcat_de.ts \
		  lang/cdcat_es.ts \
		  lang/cdcat_cz.ts \		  
		  lang/cdcat_pl.ts \		  		  
	          lang/cdcat_sk.ts \		  		  		  
	  	  lang/cdcat_el.ts \		  		  
	  	  lang/cdcat_id.ts \		  		  
	  	  lang/cdcat_pt.ts \		  		  		  
                  lang/start/cdcat_nolang.ts

translations.files += lang/cdcat_de.qm \
                      lang/cdcat_es.qm \
		      lang/cdcat_cz.qm \
		      lang/cdcat_pl.qm \
		      lang/cdcat_el.qm \
		      lang/cdcat_sk.qm \
	  	      lang/cdcat_pt.qm \
	  	      lang/cdcat_id.qm \		      		  		  		      
		      lang/cdcat_hu.qm
HEADERS		=   adddialog.h\
		    cdcat.h \
		    commwidget.h \
		    config.h \
		    dbase.h \
		    dirview.h \
		    find.h \
		    guibase.h \
		    hdirview.h \
		    mainwidget.h \
		    mp3tag.h \
		    newdbdialog.h \
		    import.h \
		    importdialog.h \
		    wdbfile.h \
		    icons.h \
		    info.h \
		    exportcdcatdb.h \
		    showcontent.h \
		    selreadable.h \
		    borrow.h \
		    tparser.h \
		    colorsettings.h \
		    misc.h \
		    dmetaph.h
SOURCES		=   adddialog.cpp \
		    cdcat.cpp \
		    commwidget.cpp \
		    config.cpp \
		    dbase.cpp \
		    dirview.cpp \
		    find.cpp \
		    guibase.cpp \
		    hdirview.cpp \
		    icons.cpp \
		    import.cpp \
		    importdialog.cpp \
		    mainwidget.cpp \
		    mp3tag.cpp \
		    newdbdialog.cpp \
		    wdbfile.cpp \
		    info.cpp \		    
		    exportcdcatdb.cpp \		  
		    showcontent.cpp \  
		    selreadable.cpp \
		    colorsettings.cpp \
		    borrow.cpp \
		    tparser.cpp \		    
		    misc.cpp \
		    dmetaph.cpp

#            unix:system(lrelease cdcat.pro)

#            win32:system(lrelease cdcat.pro)

PRECOMPILED_HEADER = cdcat_pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
   DEFINES += USING_PCH
   CONFIG += precompile_header
 }

CONFIG		+= qt debug
MOC_DIR         = moc_files/
OBJECTS_DIR     = obj_files/ 
unix:LIBS       += -lz 
win32:LIBS       += "c:\zlib\lib\libz.a"

win32:INCLUDEPATH   += c:\Expat\Source\lib c:\zlib\include c:\pcre\include"


unix:distfiles.files += ../README_CSV_IMPORT ../Authors ../README ../ChangeLog \
                        ../COPYING ../TRANSLATORS_README ../cdcat.png 
unix:distfiles.path = /usr/local/share/cdcat
unix:target.path += /usr/local/bin
unix:translations.path += /usr/local/share/cdcat/translations


win32:distfiles.files += ..\README_CSV_IMPORT ..\Authors ..\README ..\ChangeLog \
                         ..\COPYING ..\TRANSLATORS_README ..\cdcat.ico ..\cdcat.png
win32:target.path += c:\program files\cdcat
win32:translations.path += c:\program files\cdcat
win32:distfiles.path +=c:\program files\cdcat
INSTALLS += target translations distfiles

QT += xml

#The following line was inserted by qt3to4
QT +=  qt3support 
CONFIG += uic

