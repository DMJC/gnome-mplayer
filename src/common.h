/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * common.h
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 *
 * common.h is free software.
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * common.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with common.h.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <gtk/gtk.h>
#include <gmlib.h>
#ifdef LIBGDA_ENABLED
#include <libgda/libgda.h>
#endif



#ifdef GIO_ENABLED
#include <gio/gio.h>
#endif

#ifndef _COMMON_H
#define _COMMON_H

#define VO					"vo"
#define AUDIO_DEVICE_NAME   "audio-device-name"
#define ALSA_MIXER			"alsa-mixer"
#define VOLUME				"volume"
#define AUDIO_CHANNELS		"audio-channels"
#define USE_HARDWARE_CODECS "use-hardware-codecs"
#define USE_CRYSTALHD_CODECS "use-crystalhd-codecs"
#define USE_HW_AUDIO		"use-hw-audio"
#define SOFTVOL				"softvol"
#define VOLUME_GAIN			"volume-gain"
#define REMEMBER_SOFTVOL	"remember-softvol"
#define VOLUME_SOFTVOL		"volume-softvol"

#define CACHE_SIZE				"cache-size"
#define PLUGIN_AUDIO_CACHE_SIZE	"plugin-audio-cache-size"
#define PLUGIN_VIDEO_CACHE_SIZE	"plugin-video-cache-size"
#define OSDLEVEL				"osd-level"
#define PPLEVEL					"pp-level"
#define VERBOSE					"verbose"
#define VERTICAL				"vertical"
#define FORCECACHE				"force-cache"
#define LAST_DIR				"last-dir"
#define SHOWPLAYLIST			"show-playlist"
#define SHOWDETAILS				"show-details"
#define SHOW_CONTROLS			"show-controls"
#define RESUME_MODE				"resume-mode"

#define AUDIO_LANG				"audio-lang"
#define SUBTITLE_LANG			"subtitle-lang"

#define DISABLEASS      		"disable-ass"
#define DISABLEEMBEDDEDFONTS    "disable-embeddedfonts"
#define DISABLEDEINTERLACE		"disable-deinterlace"
#define DISABLEFRAMEDROP		"disable-framedrop"
#define DISABLEFULLSCREEN		"disable-fullscreen"
#define DISABLECONTEXTMENU		"disable-contextmenu"
#define DISABLEPAUSEONCLICK		"disable-pause-on-click"
#define DISABLEANIMATION		"disable-animation"
#define DISABLE_COVER_ART_FETCH "disable-cover-art-fetch"

#define AUTOHIDETIMEOUT		"auto-hide-timeout"
#define METADATACODEPAGE	"metadata-codepage"

#define SHOW_SUBTITLES		"show-subtitles"
#define SUBTITLEFONT		"subtitle-font"
#define SUBTITLESCALE		"subtitle-scale"
#define SUBTITLECODEPAGE	"subtitle-codepage"
#define SUBTITLECOLOR		"subtitle-color"
#define SUBTITLEOUTLINE		"subtitle-outline"
#define SUBTITLESHADOW		"subtitle-shadow"
#define SUBTITLE_MARGIN		"subtitle-margin"
#define SUBTITLE_FUZZINESS  "subtitle-fuzziness"

#define MOUSE_WHEEL_CHANGES_VOLUME "mouse-wheel-changes-volume"
#define USE_MEDIAKEYS		"use-mediakeys"
#define USE_DEFAULTPL		"use-defaultpl"
#define FULLSCREEN			"fullscreen"
#define MPLAYER_BIN			"mplayer-bin"
#define EXTRAOPTS			"extraopts"
#define MPLAYER_DVD_DEVICE  "mplayer-dvd-device"

#define REMEMBER_LOC		"remember-loc"
#define WINDOW_X			"window-x"
#define WINDOW_Y			"window-y"
#define WINDOW_HEIGHT		"window-height"
#define WINDOW_WIDTH		"window-width"
#define PANEL_POSITION	    "panel-position"
#define RESIZE_ON_NEW_MEDIA "resize-on-new-media"
#define KEEP_ON_TOP			"keep-on-top"
#define SINGLE_INSTANCE		"single-instance"
#define REPLACE_AND_PLAY	"replace-and-play"
#define BRING_TO_FRONT		"bring-to-front"
#define SHOW_NOTIFICATION   "show-notification"
#define SHOW_STATUS_ICON	"show-status-icon"

#define DISABLE_QT			"disable-qt"
#define DISABLE_REAL		"disable-real"
#define DISABLE_WMP			"disable-wmp"
#define DISABLE_DVX			"disable-dvx"
#define DISABLE_MIDI		"disable-midi"
#define DISABLE_EMBEDDING	"disable-embedding"
#define DISABLE_EMBEDDED_SCALING "disable-embedded-scaling"
#define DEBUG_LEVEL			"debug-level"

#define ACCELERATOR_KEYS	"accelerator-keys"

// set this to true in gconf/dconf when using with gnome global menus
#define ENABLE_GLOBAL_MENU  "enable-global-menu"

#define ENABLE_NAUTILUS_PLUGIN "enable-nautilus-plugin"

// JavaScript Playstates
#define STATE_UNDEFINED     0
#define STATE_STOPPED       1
#define STATE_PAUSED        2
#define STATE_PLAYING       3
#define STATE_SCANFORWARD   4
#define STATE_SCANREVERSE   5
#define STATE_BUFFERING	    6
#define STATE_WAITING       7
#define STATE_MEDIAENDED    8
#define STATE_TRANSITIONING 9
#define STATE_READY	        10
#define STATE_RECONNECTING  11


typedef enum {
    PLAYING,
    PAUSED,
    STOPPED,
    QUIT
} PLAYSTATE;

static PLAYSTATE guistate;
static PLAYSTATE lastguistate;

typedef struct _IdleData {
    gchar info[1024];
    gchar display_name[1024];
    gchar media_info[2048];
    gchar media_notification[2048];
    gchar af_export[1024];
    gchar url[2048];
    GMappedFile *mapped_af_export;
    gchar *device;
    gdouble cachepercent;
    gint streaming;
    gchar progress_text[1024];
    gchar vol_tooltip[128];
    gint width;
    gint height;
    gboolean videopresent;
    gboolean fullscreen;
    gboolean showcontrols;
    gdouble position;
    gdouble length;
    gdouble start_time;
    glong byte_pos;
    gchar demuxer[64];
    gchar metadata[1024];
    gboolean fromdbus;
    gboolean window_resized;
    gboolean tmpfile;
    gboolean retry_on_full_cache;
#ifdef GIO_ENABLED
    GFile *src;
    GFile *tmp;
    GCancellable *cancel;
    GMutex *caching;
    GCond *caching_complete;
#endif
} IdleData;

static IdleData *idledata;

enum {
    ITEM_COLUMN,
    DESCRIPTION_COLUMN,
    COUNT_COLUMN,
    PLAYLIST_COLUMN,
    ARTIST_COLUMN,
    ALBUM_COLUMN,
    LENGTH_COLUMN,
    LENGTH_VALUE_COLUMN,
    SUBTITLE_COLUMN,
    AUDIOFILE_COLUMN,
    COVERART_COLUMN,
    AUDIO_CODEC_COLUMN,
    VIDEO_CODEC_COLUMN,
    DEMUXER_COLUMN,
    VIDEO_WIDTH_COLUMN,
    VIDEO_HEIGHT_COLUMN,
    PLAY_ORDER_COLUMN,
    ADD_ORDER_COLUMN,
    START_COLUMN,
    END_COLUMN,
    PLAYABLE_COLUMN,
    N_COLUMNS
};

typedef enum {
    FILE_OPEN_LOCATION,
    EDIT_SCREENSHOT,
    EDIT_PREFERENCES,
    VIEW_PLAYLIST,
    VIEW_INFO,
    VIEW_DETAILS,
    VIEW_METER,
    VIEW_FULLSCREEN,
    VIEW_ASPECT,
    VIEW_SUBTITLES,
    VIEW_DECREASE_SIZE,
    VIEW_INCREASE_SIZE,
    VIEW_ANGLE,
    VIEW_CONTROLS,
    KEY_COUNT
} AcceleratorKeys;

typedef enum {
    RESUME_ALWAYS_ASK,
    RESUME_BUT_NEVER_ASK,
    DO_NOT_RESUME
} ResumeMode;

#define ACCEL_PATH_OPEN_LOCATION "<GNOME MPlayer>/File/Open Location"
#define ACCEL_PATH_EDIT_SCREENSHOT "<GNOME MPlayer>/Edit/Screenshot"
#define ACCEL_PATH_EDIT_PREFERENCES "<GNOME MPlayer>/Edit/Preferences"
#define ACCEL_PATH_VIEW_PLAYLIST "<GNOME MPlayer>/View/Playlist"
#define ACCEL_PATH_VIEW_INFO "<GNOME MPlayer>/View/Info"
#define ACCEL_PATH_VIEW_DETAILS "<GNOME MPlayer>/View/Details"
#define ACCEL_PATH_VIEW_METER "<GNOME MPlayer>/View/Meter"
#define ACCEL_PATH_VIEW_FULLSCREEN "<GNOME MPlayer>/View/Fullscreen"
#define ACCEL_PATH_VIEW_ASPECT "<GNOME MPlayer>/View/Aspect"
#define ACCEL_PATH_VIEW_SUBTITLES "<GNOME MPlayer>/View/Subtitles"
#define ACCEL_PATH_VIEW_DECREASE_SIZE "<GNOME MPlayer>/View/Decrease Size"
#define ACCEL_PATH_VIEW_INCREASE_SIZE "<GNOME MPlayer>/View/Increase Size"
#define ACCEL_PATH_VIEW_ANGLE "<GNOME MPlayer>/View/Angle"
#define ACCEL_PATH_VIEW_CONTROLS "<GNOME MPlayer>/View/Controls"
#define ACCEL_PATH_VIEW_NORMAL "<GNOME MPlayer>/View/Normal"
#define ACCEL_PATH_VIEW_DOUBLE "<GNOME MPlayer>/View/Double"

typedef struct _MetaData {
    gchar *uri;
    gchar *title;
    gchar *artist;
    gchar *album;
    gchar *length;
    gfloat length_value;
    gchar *subtitle;
    gchar *audio_codec;
    gchar *video_codec;
    gchar *demuxer;
    gint width;
    gint height;
    gboolean playable;
    gboolean resumable;
    gfloat position;
    gboolean valid;
} MetaData;

typedef struct _ButtonDef {
    gchar *uri;
    gchar *hrefid;
} ButtonDef;

#define METER_BARS 		40
static gint buckets[METER_BARS];
static gint max_buckets[METER_BARS];
static gchar **accel_keys;
static gchar **accel_keys_description;

//Define MIME for DnD
#define DRAG_NAME_0		"text/plain"
#define DRAG_INFO_0		0
#define DRAG_NAME_1		"text/uri-list"
#define DRAG_INFO_1		1
#define DRAG_NAME_2		"STRING"
#define DRAG_INFO_2		2

static gint cache_size;
static gint plugin_audio_cache_size;
static gint plugin_video_cache_size;
static gboolean forcecache;
static gint osdlevel;
static gint pplevel;
static gint streaming;
static gboolean showcontrols;
static gboolean showsubtitles;
static gint fullscreen;
static gint init_fullscreen;
static gboolean videopresent;
static gboolean playlist;
static gint embed_window;
static gint window_x;
static gint window_y;
static gint control_id;
static gboolean softvol;
static gboolean remember_softvol;
static gdouble volume_softvol;
static gint volume_gain;
static gint pref_volume;
static gint verbose;
static gint autostart;
//static gint actual_x, actual_y;
//static gint play_x, play_y;
static gint last_window_width, last_window_height;
//static gchar vm[10];
static gchar *vo;
static gchar *option_vo;
static gboolean use_hardware_codecs;
static gboolean use_crystalhd_codecs;
static AudioDevice audio_device;
static gchar *audio_device_name;
static gint audio_channels;
static gboolean use_hw_audio;
static gboolean disable_deinterlace;
static gboolean disable_framedrop;
static gboolean disable_context_menu;
static gboolean disable_fullscreen;
static gboolean disable_pause_on_click;
static gboolean enable_global_menu;
static gboolean enable_nautilus_plugin;
static gint loop;
static gint start_second;
static gint play_length;
static gint random_order;
static gboolean dontplaynext;
static gboolean autopause;
static gchar *path;
static gint js_state;
static gchar *rpconsole;
static gchar *rpcontrols;
static gboolean control_instance;
static gchar *playlistname;
static gboolean ok_to_play;
static gchar *subtitle;
static gchar *alang;
static gchar *slang;
static gchar *metadata_codepage;
static gboolean use_mplayer2;
static gboolean vertical_layout;
static gboolean playlist_visible;
static gboolean details_visible;
static gboolean restore_controls;
static gboolean update_control_flag;
//static gint restore_pane;
static gboolean disable_ass;
static gboolean disable_embeddedfonts;
static gboolean disable_animation;
static gint auto_hide_timeout;
static gboolean always_hide_after_timeout;
static gboolean mouse_over_controls;
static gchar *subtitlefont;
static gdouble subtitle_scale;
static gchar *subtitle_codepage;
static gchar *subtitle_color;
static gboolean subtitle_outline;
static gboolean subtitle_shadow;
static gint subtitle_margin;
static gint subtitle_fuzziness;
static gboolean quit_on_complete;
static gchar *mplayer_bin;
static gchar *mplayer_dvd_device;
static gchar *option_dvd_device;
static gchar *extraopts;
static gboolean resize_on_new_media;
static gboolean single_instance;
static gboolean replace_and_play;
static gboolean bring_to_front;
static gboolean show_notification;
static gboolean show_status_icon;
static gchar *gpod_mount_point;
static gboolean disable_cover_art_fetch;
//static gboolean updating_recent;
static gboolean large_buttons;
static GtkIconSize button_size;
static gboolean skip_fixed_allocation_on_show;
static gboolean skip_fixed_allocation_on_hide;
static gboolean mouse_wheel_changes_volume;
static ResumeMode resume_mode;

static gboolean remember_loc;
static gboolean use_remember_loc;
static gboolean save_loc;
static gint loc_window_x;
static gint loc_window_y;
static gint loc_window_height;
static gint loc_window_width;
static gint loc_panel_position;
static gboolean keep_on_top;

static gboolean cancel_folder_load;

static GThreadPool *retrieve_metadata_pool;
static GMutex *retrieve_mutex;
static GMutex *set_mutex;

static gboolean use_mediakeys;
static gboolean use_defaultpl;

static gboolean qt_disabled;
static gboolean real_disabled;
static gboolean wmp_disabled;
static gboolean dvx_disabled;
static gboolean midi_disabled;
static gboolean embedding_disabled;
static gboolean disable_embedded_scaling;

static GArray *data;
static GArray *max_data;
static gboolean reading_af_export;

//gboolean sub_source_file;

// layout variables
static gint non_fs_width;
static gint non_fs_height;

// playlist stuff
static GtkListStore *playliststore;
static GtkTreeIter iter;
static GtkTreeIter *next_iter;
static GtkTreeSelection *selection;
static GtkWidget *list;

// preference store
static GmPrefStore *gm_store;
static GmPrefStore *gmp_store;

// cover
static gchar *cover_art_uri;

GtkWidget *create_window(gint windowid);
void show_window(gint windowid);
void present_main_window();
gint get_player_window();
void adjust_layout();
gboolean hide_buttons(void *data);
gboolean show_copyurl(void *data);

gboolean set_gui_state(void *data);
gboolean set_title_bar(void *data);
gboolean set_media_label(void *data);
gboolean set_cover_art(gpointer pixbuf);
gboolean set_progress_value(void *data);
gboolean set_progress_text(void *data);
gboolean set_progress_time(void *data);
gboolean set_volume_from_slider(gpointer data);
gboolean set_volume_tip(void *data);
gboolean resize_window(void *data);
gboolean set_play(void *data);
gboolean set_pause(void *data);
gboolean set_stop(void *data);
gboolean set_ff(void *data);
gboolean set_rew(void *data);
gboolean set_prev(void *data);
gboolean set_next(void *data);
gboolean set_quit(void *data);
gboolean set_kill_mplayer(void *data);
gboolean set_position(void *data);
gboolean hookup_volume(void *data);
gboolean set_volume(void *data);
gboolean set_fullscreen(void *data);
gboolean set_show_controls(void *data);
gboolean get_show_controls();
gboolean set_window_visible(void *data);
gboolean set_update_gui(void *data);
gboolean set_subtitle_visibility(void *data);
gboolean set_item_add_info(void *data);
gboolean set_metadata(gpointer data);
gboolean set_pane_position(void *data);
gboolean set_raise_window(void *data);

void remove_langs(GtkWidget * item, gpointer data);
gboolean set_new_lang_menu(gpointer data);
gboolean set_new_audio_menu(gpointer data);
gboolean make_panel_and_mouse_invisible(gpointer data);
gboolean idle_make_button(gpointer data);
gboolean set_show_seek_buttons(gpointer data);
void make_button(gchar * src, gchar * href);
void dbus_open_by_hrefid(gchar * hrefid);
void dbus_open_next();
void dbus_cancel();
void dbus_reload_plugins();
void dbus_send_rpsignal(gchar * signal);
void dbus_send_rpsignal_with_int(gchar * signal, int value);
void dbus_send_rpsignal_with_double(gchar * signal, gdouble value);
void dbus_send_rpsignal_with_string(gchar * signal, gchar * value);
void dbus_send_event(gchar * event, gint button);
void dbus_unhook();
void dbus_enable_screensaver();
void dbus_disable_screensaver();
void menuitem_edit_random_callback(GtkMenuItem * menuitem, void *data);

void mpris_send_signal_PlaybackStatus();
void mpris_send_signal_Updated_Metadata();
void mpris_send_signal_Seeked();
void mpris_send_signal_VolumeChanged();
void mpris_send_signal_RateChanged();

gboolean update_audio_meter(gpointer data);

void set_media_player_attributes(GtkWidget * widget);

void retrieve_metadata(gpointer data, gpointer user_data);

static gchar *default_playlist;
static gboolean safe_to_save_default_playlist;
gboolean async_play_iter(void *data);

#ifdef GTK2_12_ENABLED
static GtkRecentManager *recent_manager;
void recent_manager_changed_callback(GtkRecentManager * recent_manager, gpointer data);
#endif

#ifdef LIBGDA_ENABLED
static GdaConnection *db_connection;
#endif

#endif                          /* _COMMON_H */
