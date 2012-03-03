CLEANFILES = *~ $(wbardesktop:.desktop.in.in=.desktop)

defaultdir = @DEFAULT_CONFIGDIR@
default_DATA = $(wbarcfg:.cfg.in=.cfg)

autostartdir = @DEFAULT_CONFIGDIR@
autostart_DATA = $(wbardesktop:.desktop.in.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

wbardesktop = wbar.desktop.in.in
wbarcfg = wbar.cfg.in

EXTRA_DIST = \
$(wbardesktop) \
$(wbarcfg) \
$(wbardesktop:.desktop.in.in=.desktop.in)

