%define ver 1.0.9b
%define use_gconf 
%define use_gsettings 1

Name: gnome-mplayer
Summary: GNOME Frontend for MPlayer
Version: %{ver}
Release: 1%{?dist}
License: GPLv2+
Group: Multimedia
Packager: Kevin DeKorte <kdekorte@gmail.com>
Source0: http://gnome-mplayer.googlecode.com/files/gnome-mplayer-%{ver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: mplayer mencoder libgda-sqlite
BuildRequires: gcc-c++, pkgconfig, gettext, glib2-devel gtk2-devel GConf2-devel dbus-devel dbus-glib-devel alsa-lib-devel pulseaudio-libs-devel libnotify-devel libmusicbrainz3-devel libcurl-devel pulseaudio gnome-power-manager nautilus-devel libXScrnSaver-devel libgda-devel 

%description
GNOME MPlayer is a GTK GUI for MPlayer based on the Gnome HIG. However, the code is not dependent on any Gnome library

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -rf %buildroot
make install DESTDIR=%buildroot

%clean
rm -rf $buildroot

%post
update-desktop-database %{_datadir}/applications

%if %{use_gconf}
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null
%endif

%if %{use_gsettings}
glib-compile-schemas %{_datadir}/glib-2.0/schemas
%endif

%files
%defattr(-,root,root,-)
%{_docdir}/%{name}
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons
%{_datadir}/gnome-control-center
%{_datadir}/locale
%{_datadir}/man
%if %{use_gconf}
%config %{_sysconfdir}/gconf/schemas/%{name}.schemas
%endif
%if %{use_gsettings}
%{_datadir}/glib-2.0/schemas/apps.gecko-mediaplayer.preferences.gschema.xml
%{_datadir}/glib-2.0/schemas/apps.gnome-mplayer.preferences.enums.xml
%{_datadir}/glib-2.0/schemas/apps.gnome-mplayer.preferences.gschema.xml
%endif
%{_libdir}/nautilus/extensions-2.0/libgnome-mplayer-properties-page.so
