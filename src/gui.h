/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gui.h
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 *
 * gui.h is free software.
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * gui.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gui.h.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */
//#include <X11/Xlib.h>

#pragma once
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#ifdef X11_ENABLED
#include <gdk/gdkx.h>
#endif
#include <gmlib.h>
#include <gmtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <math.h>
#ifdef HAVE_ASOUNDLIB
#include <alsa/asoundlib.h>
#endif

#include "playlist.h"

static GtkWidget *window;

static GtkMenuItem *menuitem_edit_random;
static GtkMenuItem *menuitem_edit_loop;
static GtkMenuItem *menuitem_edit_play_single;
static GtkWidget *repeat;
static GtkWidget *shuffle;
static GtkMenuItem *menuitem_edit_select_audio_lang;
static GtkMenuItem *menuitem_edit_select_sub_lang;
static GtkWidget *tracks;
static GtkWidget *subtitles;

static GtkMenuItem *menuitem_view_info;
static GtkMenuItem *menuitem_view_playlist;

static GtkMenuItem *menuitem_prev;
static GtkMenuItem *menuitem_next;

static GtkWidget *pane;
static GtkWidget *hbox;

static GtkWidget *media;
static GtkWidget *cover_art;
static GtkWidget *audio_meter;

static GtkWidget *details_table;

static GtkWidget *play_event_box;

static GtkWidget *prev_event_box;
static GtkWidget *next_event_box;
static GtkWidget *menu_event_box;
static GtkWidget *fs_event_box;

static GmtkMediaTracker *tracker;
static GtkWidget *vol_slider;

static GtkWidget *conf_volume_label;

#ifdef GTK2_12_ENABLED
#else
GtkTooltips *tooltip;
#endif

// Playlist container
static GtkWidget *plvbox;
static GSList *lang_group;
static GSList *audio_group;

static GtkAccelGroup *accel_group;

gboolean popup_handler(GtkWidget * widget, GdkEvent * event, void *data);
gboolean delete_callback(GtkWidget * widget, GdkEvent * event, void *data);
void config_close(GtkWidget * widget, void *data);

gboolean rew_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean play_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean pause_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean stop_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean ff_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean prev_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean next_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
void vol_slider_callback(GtkRange * range, gpointer user_data);
gboolean fs_callback(GtkWidget * widget, GdkEventExpose * event, void *data);
gboolean make_panel_and_mouse_visible(gpointer data);
void menuitem_open_callback(GtkMenuItem * menuitem, void *data);
void menuitem_quit_callback(GtkMenuItem * menuitem, void *data);
void menuitem_about_callback(GtkMenuItem * menuitem, void *data);
void menuitem_play_callback(GtkMenuItem * menuitem, void *data);
void menuitem_pause_callback(GtkMenuItem * menuitem, void *data);
void menuitem_stop_callback(GtkMenuItem * menuitem, void *data);
void menuitem_fs_callback(GtkMenuItem * menuitem, void *data);
void menuitem_showcontrols_callback(GtkCheckMenuItem * menuitem, void *data);
void menuitem_quit_callback(GtkMenuItem * menuitem, void *data);
void menuitem_details_callback(GtkMenuItem * menuitem, void *data);
void menuitem_view_decrease_subtitle_delay_callback(GtkMenuItem * menuitem, void *data);
void menuitem_view_increase_subtitle_delay_callback(GtkMenuItem * menuitem, void *data);
void clear_playlist(GtkWidget * widget, void *data);
void create_playlist_widget();
void add_folder_to_playlist(GtkWidget * widget, void *data);
gboolean playlist_drop_callback(GtkWidget * widget, GdkDragContext * dc,
                                gint x, gint y, GtkSelectionData * selection_data, guint info, guint t, gpointer data);
void create_folder_progress_window();
void destroy_folder_progress_window();
void update_status_icon();
void setup_accelerators();
gboolean set_software_volume(gdouble * data);
gboolean set_adjust_layout(gpointer data);
gboolean get_key_and_modifier(gchar * keyval, guint * key, GdkModifierType * modifier);
gboolean accel_key_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer data);
void assign_default_keys();
void reset_keys_callback(GtkButton * button, gpointer data);
gint get_index_from_key_and_modifier(guint key, GdkModifierType modifier);

void show_fs_controls();
void hide_fs_controls();
gboolean set_destroy(gpointer data);
