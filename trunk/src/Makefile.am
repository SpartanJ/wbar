CLEANFILES = *~

bin_PROGRAMS = wbar wbar-config

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

wbar_config_SOURCES = \
$(top_srcdir)/src/config/Run.h \
$(top_srcdir)/src/config/Run.cc \
$(top_srcdir)/src/config/Functions.cc \
$(top_srcdir)/src/config/Functions.h \
$(top_srcdir)/src/config/Main.cc \
$(UTILS)

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

wbar_CPPFLAGS = @modules_CFLAGS@ -I$(top_srcdir)/src/utils
wbar_LDFLAGS = @modules_LIBS@

wbar_config_CPPFLAGS = @gui_modules_CFLAGS@ -I$(top_srcdir)/src/utils
wbar_config_LDFLAGS = @gui_modules_LIBS@

