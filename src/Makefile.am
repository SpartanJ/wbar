CLEANFILES = *~

bin_PROGRAMS = wbar

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-Wall \
	-Wunused \
	-Werror \
	-I$(top_srcdir)/src/utils

if WBAR_CONFIG
 bin_PROGRAMS += wbar-config
endif

wbar_SOURCES = \
$(top_srcdir)/src/core/Bar.cc \
$(top_srcdir)/src/core/Bar.h \
$(top_srcdir)/src/core/Icon.cc \
$(top_srcdir)/src/core/Icon.h \
$(top_srcdir)/src/core/ImgWrap.h \
$(top_srcdir)/src/core/Main.cc \
$(top_srcdir)/src/core/SuperBar.cc \
$(top_srcdir)/src/core/SuperBar.h \
$(top_srcdir)/src/core/SuperIcon.cc \
$(top_srcdir)/src/core/SuperIcon.h \
$(top_srcdir)/src/core/XWin.cc \
$(top_srcdir)/src/core/XWin.h \
$(UTILS)

if WBAR_CONFIG
wbar_config_SOURCES = \
$(top_srcdir)/src/config/Run.h \
$(top_srcdir)/src/config/Run.cc \
$(top_srcdir)/src/config/Functions.cc \
$(top_srcdir)/src/config/Functions.h \
$(top_srcdir)/src/config/MainConfig.cc \
$(UTILS)
endif

UTILS = \
$(top_srcdir)/src/utils/App.cc \
$(top_srcdir)/src/utils/App.h \
$(top_srcdir)/src/utils/Config.cc \
$(top_srcdir)/src/utils/Config.h \
$(top_srcdir)/src/utils/Utils.cc \
$(top_srcdir)/src/utils/Utils.h \
$(top_srcdir)/src/utils/OptParser.cc \
$(top_srcdir)/src/utils/OptParser.h \
$(top_srcdir)/src/utils/i18n.h

wbar_CXXFLAGS = @modules_CFLAGS@
wbar_LDADD = @modules_LIBS@

if WBAR_CONFIG
wbar_config_CXXFLAGS = @gui_modules_CFLAGS@
wbar_config_LDADD = @gui_modules_LIBS@
endif

