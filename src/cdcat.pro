lessThan(QT_VERSION, 5.2.0): error(This project requires Qt 5.2.0 or later)

TRANSLATIONS = lang/cdcat_hu.ts \
               lang/cdcat_de.ts \
               lang/cdcat_en_us.ts \
               lang/cdcat_es.ts \
               lang/cdcat_fr.ts \
               lang/cdcat_cz.ts \
               lang/cdcat_it.ts \
               lang/cdcat_pl.ts \
               lang/cdcat_sk.ts \
               lang/cdcat_sr.ts \
               lang/cdcat_el.ts \
               lang/cdcat_id.ts \
               lang/cdcat_pt.ts \
               lang/cdcat_ru.ts \
               lang/start/cdcat_nolang.ts

translations.files += lang/cdcat_de.qm \
                      lang/cdcat_en_us.qm \
                      lang/cdcat_es.qm \
                      lang/cdcat_fr.qm \
                      lang/cdcat_cz.qm \
                      lang/cdcat_pl.qm \
                      lang/cdcat_el.qm \
                      lang/cdcat_it.qm \
                      lang/cdcat_sk.qm \
                      lang/cdcat_sr.qm \
                      lang/cdcat_pt.qm \
                      lang/cdcat_ru.qm \
                      lang/cdcat_id.qm \
                      lang/cdcat_hu.qm

HEADERS = adddialog.h\
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
          cdcatmediainfo.h \
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
          mustache.h \
          dmetaph.h

SOURCES = adddialog.cpp \
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
          cdcatmediainfo.cpp \
          cdcatexif.cpp \
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
          mustache.cpp \
          dmetaph.cpp

PRECOMPILED_HEADER = cdcat_pch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
    CONFIG  += precompile_header
}

RESOURCES = lang.qrc
RC_ICONS = img/cdcat.ico
ICON = img/cdcat.icns

TEMPLATE     =  app
FORMS        =  help.ui
TARGET       =  cdcat
INSTALLS    += target translations distfiles
QT          += xml widgets printsupport
CONFIG      += qt xml uic

## Release is the default and will be overwritten by debug
## Don‘t specify it in the file. Use 'CONFIG+=debug' for qmake.
# CONFIG      += release
CONFIG      += debug

# DEFINES     += QT_NO_DEBUG_OUTPUT

## Compiler warnings {{{
CONFIG      += warn_on
# CONFIG      += warn_off
## }}}

DEFINES     += QT_FORCE_ASSERTS
## QT_FORCE_ASSERTS is needed for the code to work …
MOC_DIR      =  moc_files/
OBJECTS_DIR  =  obj_files/

win32 {
    ########## lib7zip
    # use lib7zip?
    DEFINES+=USE_LIB7ZIP
    LIBS       += c:/zlib/lib/libz.a c:/libs/lib7zip.a c:/libs/bzip2.dll c:/libs/libtar.a  -loleaut32 -luuid
    INCLUDEPATH   += c:/zlib/include c:/pcre/include C:/includes c:/mediainfo

    ###### encryption {{{
    DEFINES += CATALOG_ENCRYPTION
    LIBS+= c:/libs/libcrypto++.a -lpthread
    ###### end encryption }}}

    # enable if cdcat should made console output
    #CONFIG           += console
    distfiles.files   += ../README_CSV_IMPORT ../AUTHORS ../README ../ChangeLog ../COPYING ../TRANSLATORS_README ../cdcat.ico ../cdcat.png
    target.path       += c:/program files/cdcat
    translations.path += c:/program files/cdcat
    distfiles.path    += c:/program files/cdcat
} else:mac {
    ########## lib7zip
    # use lib7zip?
    DEFINES += USE_LIB7ZIP
    LIBS    += -lz -ldl /usr/lib/libtar.a /usr/lib/libbz2.a /usr/lib/lib7zip.a
} else:os2 {
    # OS/2
    CONFIG += console

    ########## lib7zip
    # use lib7zip?
    DEFINES+=USE_LIB7ZIP

    # STATIC
    LIBS+=c:/usr/lib/lib7zip.a

    # DYNAMIC
    #LIBS+= -llib7zip
    ######### end lib7zip

    ######### mediainfo {{{
    # use libmediainfo as static library?
    # STATIC
    #DEFINES += MEDIAINFO_STATIC
    #LIBS+=c:/usr/lib/mediainfo.a c:/usr/lib/zen.a c:/usr/lib/pthread.a

    # DYNAMIC. no pkgconfig
    #LIBS+= -lmediainfo

    # temporary kluge until it's decided how to get char type from libmediainfo,
    # maybe also via pkgconfig (Debian Bug #656929, could remove the extra
    # hack in cdcatmediainfo.h when it's ready)
    #DEFINES += MEDIAINFO_UNICODE

    # disable mediainfo
    DEFINES += NO_MEDIAINFO
    ######### end mediainfo }}}

    ########## exif {{{
    # use exif?
    #DEFINES += USE_LIBEXIF
    #LIBS += -lexif
    ######### end exif }}}

    #LIBS             += -lz -ltar -lbz2 -ldl
    LIBS              += -lz c:/usr/lib/libtar.a -lbz2
    INCLUDEPATH       += c:/usr/include
    distfiles.files   += ../README_CSV_IMPORT ../AUTHORS ../README ../ChangeLog ../COPYING ../TRANSLATORS_README ../cdcat.png
    distfiles.path     = c:/usr/share/cdcat
    target.path       += c:/usr/bin
    translations.path += c:/usr/share/cdcat/translations
    QMAKE_CXXFLAGS    += -O2 -D_OS2


} else {
    # unix

    ########## lib7zip {{{
    # use lib7zip?
    # DEFINES+=USE_LIB7ZIP

    # STATIC
    #LIBS+=/usr/local/lib/lib7zip.a

    # DYNAMIC
    # LIBS+= -l7zip
    ######### end lib7zip }}}

    ######### mediainfo {{{
    # use libmediainfo as static library?
    # STATIC
    # DEFINES += MEDIAINFO_STATIC
    # LIBS+=/usr/local/lib/libmediainfo.a

    # DYNAMIC. no pkgconfig
    # LIBS+= -lmediainfo

    # disable mediainfo
    #DEFINES += NO_MEDIAINFO

    # libmediainfo ships API info via pkgconfig so use it!
    # CONFIG += link_pkgconfig
    # PKGCONFIG += libmediainfo
    # temporary kluge until it's decided how to get char type from libmediainfo,
    # maybe also via pkgconfig (Debian Bug #656929, could remove the extra
    # hack in cdcatmediainfo.h when it's ready)
    #DEFINES += MEDIAINFO_UNICODE
    ######### end mediainfo }}}

    ########## exif {{{
    # use exif?
    #DEFINES += USE_LIBEXIF
    #LIBS += -lexif
    ######### end exif }}}

    #LIBS       += -lz -ltar -lbz2 -ldl
    # libmediainfo ships API info via pkgconfig so use it!
    #CONFIG += link_pkgconfig
    #PKGCONFIG += libmediainfo
    # temporary kluge until it's decided how to get char type from libmediainfo,
    # maybe also via pkgconfig (Debian Bug #656929, could remove the extra
    # hack in cdcatmediainfo.h when it's ready)
    #DEFINES += MEDIAINFO_UNICODE

    ###### libtar {{{
    # STATIC
    #LIBS += /usr/local/lib/libtar.a

    # DYNAMIC
    LIBS += -ltar
    ###### end libtar }}}


    ###### encryption {{{
    DEFINES += CATALOG_ENCRYPTION
    LIBS+= -lcrypto++ -lpthread
    ###### end encryption }}}

    LIBS              += -lz -lbz2 -ldl
    distfiles.files   += ../README_CSV_IMPORT ../AUTHORS ../README ../ChangeLog ../COPYING ../TRANSLATORS_README ../cdcat.png
    distfiles.path     = /usr/local/share/cdcat
    target.path       += /usr/local/bin
    translations.path += /usr/local/share/cdcat/translations

    # security hardening flags
    #DEFINES += _FORTIFY_SOURCE=2
    DEFINES += _FORTIFY_SOURCE=1
    #QMAKE_CXXFLAGS += -std=c++0x -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -Werror=format-security
}
