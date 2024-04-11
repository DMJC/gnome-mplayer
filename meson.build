project('gnome-mplayer', 'c')
cc = meson.get_compiler('c')

conf = configuration_data()

pkg = 'gnome-mplayer'
ver = '1.0.9c'

alsa = dependency('alsa', required: false)
x11 = dependency('x11')
gtk = dependency('gtk+-3.0', version : '>= 3')
glib2_24 = dependency('glib-2.0', version : '>= 2.24.0')
gtk2 = dependency('gtk+-2.0')
glib = dependency('glib-2.0')
gtk2_12 = dependency('gtk+-2.0', version : '>=2.12.0')
gtk2_14 = dependency('gtk+-2.0', version : '>=2.14.0')
gtk2_18 = dependency('gtk+-2.0', version : '>=2.18.0')
gtk2_20 = dependency('gtk+-2.0', version : '>=2.20.0')
gtk2_22 = dependency('gtk+-2.0', version : '>=2.22.0')
gtk2_24 = dependency('gtk+-2.0', version : '>=2.24.0')
glib2_1 = dependency('glib-2.0', version : '>=2.14.0')
glib2 = dependency('glib-2.0', version : '>=2.24.0')
gmlib = dependency('gmlib', version : '>=1.0.7')
gmtk = dependency('gmtk', version : '>=1.0.7')
gio = dependency('gio-2.0')
dbus = dependency('dbus-1', version : '>=0.95 dbus-glib-1', version : '>=0.70')
xscrnsaver = dependency('xscrnsaver')
#libgda4 = dependency('libgda-4.0')
libgda5 = dependency('libgda-5.0')
pulse = dependency('libpulse libpulse-mainloop-glib')
notify = dependency('libnotify')
#notify_7 = dependency('libnotify', version : '>=0.7')
#gpod6 = dependency('libgpod-1.0 <= 0.6.0')
gpod7 = dependency('libgpod-1.0', version : '>=0.7.0')
musicbrainz5 = dependency('libmusicbrainz5')
nautilus = dependency('libnautilus-extension', required : false)
nemo = dependency('libnemo-extension', required : false)

#if gtk2.found()
#if gtk2_12.found()
#if gtk2_14.found()
#if gtk2_18.found()
#if gtk2_20.found()
#if gtk2_22.found()
#if gtk2_24.found()
#if glib2_1.found()
#if glib2.found()
#if gmlib.found()
#if gmtk.found()
#if libgda4.found()
#if libgda5.found()
#if pulse.found()
#if #gpod6.found()
#if gpod7.found()
if musicbrainz5.found()
	conf.set('HAVE_MUSICBRAINZ', 1)
else
	conf.set('HAVE_MUSICBRAINZ', 0)
endif
#if nautilus.found()
#if nemo.found()

if dbus.found()
	conf.set('DBUS_ENABLED', 1)
endif
#conf.set('ENABLE_NLS', 1)
#endif

#conf.set('GETTEXT_PACKAGE pkg)
#endif

if gio.found()
	conf.set('GIO_ENABLED', 1)
endif

#if glib.found()
if glib2_24.found()
	conf.set('GLIB2_24_ENABLED', 1)
endif

#conf.set('GTK2_12_ENABLED', 1)
#endif

#conf.set('GTK2_14_ENABLED', 1)
#endif

#conf.set('GTK2_18_ENABLED', 1)
#endif

#conf.set('GTK2_20_ENABLED', 1)
#endif

#conf.set('GTK2_22_ENABLED', 1)
#endif

#conf.set('GTK2_24_ENABLED', 1)
#endif

if gtk.found()
	conf.set('GTK3_ENABLED', 1)
endif

if alsa.found() 
	conf.set('HAVE_ASOUNDLIB', 1)
endif

#conf.set('HAVE_BIND_TEXTDOMAIN_CODESET', 1)
#endif
#conf.set('HAVE_DCGETTEXT', 1)
#endif
if cc.has_header('fstab.h')
	conf.set('HAVE_FSTAB_H', 1)
endif
#conf.set('HAVE_GETTEXT', 1)
#endif
#conf.set('HAVE_INTTYPES_H', 1)
#endif
#conf.set('HAVE_LC_MESSAGES', 1)
#endif
if cc.has_header('locale.h')
	conf.set('HAVE_LOCALE_H', 1)
endif

if cc.has_header('mntent.h')
	conf.set('HAVE_MNTENT_H', 1)
endif

if cc.has_header('signal.h')
	conf.set('HAVE_SIGNAL_H', 1)
endif

if cc.has_header('stdint.h')
	conf.set('HAVE_STDINT_H', 1)
endif

if cc.has_header('stdlib.h')
	conf.set('HAVE_STDLIB_H', 1)
endif

if cc.has_header('strings.h')
	conf.set('HAVE_STRINGS_H', 1)
endif

if cc.has_header('string.h')
	conf.set('HAVE_STRING_H', 1)
endif

if cc.has_header('sys/mount.h')
	conf.set('HAVE_SYS_MOUNT_H', 1)
endif

if cc.has_header('sys/stat.h')
	conf.set('HAVE_SYS_STAT_H', 1)
endif

if cc.has_header('sys/types.h')
	conf.set('HAVE_SYS_TYPES_H', 1)
endif

if cc.has_header('unistd.h')
	conf.set('HAVE_UNISTD_H', 1)
endif

#if notify7.found()	
#	conf.set('NOTIFY0_7_ENABLED', 1)
#endif

if notify.found()
	conf.set('NOTIFY_ENABLED', 1)
endif

conf.set('PACKAGE', pkg)
conf.set('PACKAGE_BUGREPORT', '')
conf.set('PACKAGE_NAME', pkg)
conf.set('PACKAGE_STRING', pkg + ' ' + ver)
conf.set('PACKAGE_TARNAME', pkg)
conf.set('PACKAGE_URL', '')
conf.set('PACKAGE_VERSION', ver)

if cc.has_header('stdio.h')
	conf.set('HAVE_STDIO_H', 1)
	conf.set('STDC_HEADERS', 1)
endif

conf.set('VERSION', ver)

if x11.found()
	conf.set('X11_ENABLED', 1)
endif

if xscrnsaver.found()
	conf.set('XSCRNSAVER_ENABLED', 1)
endif

if nautilus.found()
	conf.set('HAVE_NAUTILUS')
else
	conf.set('HAVE_NAUTILUS', false)
endif

if nemo.found()
	conf.set('HAVE_NEMO')
else
	conf.set('HAVE_NEMO', false)
endif

configure_file(
    output : 'config.h',
    configuration : conf
)

sources = [
	  'build/config.h',
        'src/gui.c',
        'src/gui.h',
        'src/playlist.c',
        'src/playlist.h',
        'src/support.c',
        'src/support.h',
        'src/mntent_compat.h',
        'src/mntent_compat.c',
        'src/main.c',
        'src/dbus-interface.c',
        'src/dbus-interface.h',
        'src/common.h',
#        'src/../pixmaps/gnome_mplayer.xpm',
#        'src/../pixmaps/media-playback-pause.xpm',
#        'src/../pixmaps/media-playback-start.xpm',
#        'src/../pixmaps/media-playback-stop.xpm',
#        'src/../pixmaps/media-seek-backward.xpm',
#        'src/../pixmaps/media-seek-forward.xpm',
#        'src/../pixmaps/media-skip-backward.xpm',
#        'src/../pixmaps/media-skip-forward.xpm',
#        'src/../pixmaps/view-fullscreen.xpm',
        'src/langlist.h',
        'src/mpris-interface.c',
        'src/mpris-interface.h',
        'src/database.c',
        'src/database.h'
]

headers = [
	  'src/mntent_compat.h',
	  'src/database.h',
	  'src/mpris-interface.h',
	  'src/support.h',
	  'src/gui.h',
	  'src/common.h',
	  'src/property_page_common.h',
	  'src/mime_types.h',
	  'src/playlist.h',
	  'src/dbus-interface.h',
	  'src/langlist.h'
]

executable('gnome-mplayer', sources, headers, dependencies: [alsa, x11, gtk, glib, gmlib, gmtk, gio, dbus, xscrnsaver, pulse, notify])


#glib2,
#glib2_24,
#gtk2,
#gtk2_12,
#gtk2_14,
#gtk2_18,
#gtk2_20,
#gtk2_22,
#gtk2_24,
#glib2_1,
#libgda4
#libgda5,
#notify_7 = dependency('libnotify', version : '>=0.7')
#gpod6 = dependency('libgpod-1.0 <= 0.6.0')
#gpod7,
#musicbrainz5,
#nautilus,
#nemo
