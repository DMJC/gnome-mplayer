/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * dbus-interface.c
 * Copyright (C) Kevin DeKorte 2006 <kdekorte@gmail.com>
 * 
 * dbus-interface.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * dbus-interface.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with dbus-interface.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "dbus-interface.h"
#include "support.h"
#include <unistd.h>
#include <sys/wait.h>

#define WAIT_FOR_REPLY_TIMEOUT_MSEC 200

#ifdef DBUS_ENABLED
static DBusConnection *connection;
#endif
static guint ss_cookie;
static guint sm_cookie;
static guint fd_cookie;
static gboolean ss_cookie_is_valid;
static gboolean sm_cookie_is_valid;
static gboolean fd_cookie_is_valid;

/*

To send command to ALL running gnome-mplayers (multihead applications)
dbus-send  --type=signal / com.gnome.mplayer.Play string:'http://www.hotmail.com/playfile.asx'


When windowid is not specified
dbus-send  --type=signal /pid/[pid] com.gnome.mplayer.Play string:'http://www.hotmail.com/playfile.asx'


When windowid is specified
dbus-send  --type=signal /window/[windowid] com.gnome.mplayer.Play string:'http://www.hotmail.com/playfile.asx'

When controlid is specified
dbus-send  --type=signal /control/[controlid] com.gnome.mplayer.Play string:'http://www.hotmail.com/playfile.asx'

cleanup
indent -kr -l100 -i4 -nut

*/

#ifdef DBUS_ENABLED

static DBusHandlerResult filter_func(DBusConnection *connection, DBusMessage *message, void *user_data)
{

    //const gchar *sender;
    //const gchar *destination;
    gint message_type;
    gchar *s = NULL;
    gchar *hrefid = NULL;
    gchar *path = NULL;
    gchar *keyname = NULL;
    DBusError error;
    DBusMessage *reply_message;
    gchar *path1;
    gchar *path2;
    gchar *path3;
    gchar *path4;
    GString *xml;
    gchar *xml_string;
    gint source_id;
    gint bitrate;
    ButtonDef *b;
    gdouble volume;
    gdouble percent;
    gdouble duration;

    message_type = dbus_message_get_type(message);
    //sender = dbus_message_get_sender(message);
    //destination = dbus_message_get_destination(message);

    gm_log(verbose, G_LOG_LEVEL_DEBUG, "path=%s; interface=%s; member=%s; data=%s",
           dbus_message_get_path(message), dbus_message_get_interface(message), dbus_message_get_member(message), s);

    path1 = g_strdup_printf("/control/%i", control_id);
    path2 = g_strdup_printf("/window/%i", embed_window);
    path3 = g_strdup_printf("/pid/%i", getpid());
    path4 = g_strdup_printf("/console/%s", rpconsole);

    if (dbus_message_get_path(message)) {

        if (g_ascii_strcasecmp(dbus_message_get_path(message), "/") == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), path1) == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), path2) == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), path3) == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), path4) == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), "/org/gnome/SettingsDaemon") == 0 ||
            g_ascii_strcasecmp(dbus_message_get_path(message), "/org/gnome/SettingsDaemon/MediaKeys") == 0) {

            gm_log(verbose, G_LOG_LEVEL_DEBUG, "Path matched %s", dbus_message_get_path(message));
            if (message_type == DBUS_MESSAGE_TYPE_SIGNAL) {
                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Open") == 0) {
                    if (media != NULL) {
                        if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) != MEDIA_STATE_UNKNOWN) {
                            dontplaynext = TRUE;
                            gmtk_media_player_set_state(GMTK_MEDIA_PLAYER(media), MEDIA_STATE_QUIT);
                        }
                    }
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        if (s != NULL && strlen(s) > 0) {
                            gm_log(verbose, G_LOG_LEVEL_DEBUG, "opening %s", s);
                            gm_log(verbose, G_LOG_LEVEL_DEBUG, "rpconsole = %s, control_instance=%i", rpconsole,
                                   control_instance);
                            if ((strcmp(rpconsole, "NONE") == 0 || control_instance == FALSE)
                                && s != NULL) {
                                gm_log(verbose, G_LOG_LEVEL_DEBUG,
                                       "calling clear_playlist_and_play rpconsole = %s, control_instance=%i", rpconsole,
                                       control_instance);
                                g_idle_add(clear_playlist_and_play, g_strdup(s));
                            }
                        } else {
                            gm_log(verbose, G_LOG_LEVEL_MESSAGE, "Open requested, but value is blank");
                        }

                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "OpenPlaylist") == 0) {
                    g_idle_add(set_kill_mplayer, NULL);
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        selection = NULL;
                        if (s != NULL)
                            g_idle_add(clear_playlist_and_play, g_strdup(s));

                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "OpenButton") == 0) {
                    g_idle_add(set_kill_mplayer, NULL);
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s,
                                              DBUS_TYPE_STRING, &hrefid, DBUS_TYPE_INVALID)) {
                        b = (ButtonDef *) g_new0(ButtonDef, 1);
                        b->uri = g_strdup(s);
                        b->hrefid = g_strdup(hrefid);
                        g_idle_add(idle_make_button, b);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Add") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        if (strlen(s) > 0) {
                            g_idle_add(add_to_playlist_and_play, g_strdup(s));
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetPlaylistName") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        if (playlistname != NULL)
                            g_free(playlistname);
                        playlistname = g_strdup(s);
                        g_idle_add(set_update_gui, NULL);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Close") == 0) {
                    g_idle_add(set_kill_mplayer, NULL);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Quit") == 0) {
                    g_idle_add(set_kill_mplayer, NULL);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Play") == 0 && idledata != NULL) {
                    g_idle_add(set_play, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_Play") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_play, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Pause") == 0 && idledata != NULL) {
                    g_idle_add(set_pause, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_Pause") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_pause, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Stop") == 0 && idledata != NULL) {
                    g_idle_add(set_stop, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_Stop") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_stop, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Prev") == 0 && idledata != NULL) {
                    g_idle_add(set_prev, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Next") == 0 && idledata != NULL) {
                    g_idle_add(set_next, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }


                if (g_ascii_strcasecmp(dbus_message_get_member(message), "FastForward") == 0 && idledata != NULL) {
                    g_idle_add(set_ff, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_FastForward") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_ff, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }


                if (g_ascii_strcasecmp(dbus_message_get_member(message), "FastReverse") == 0 && idledata != NULL) {
                    g_idle_add(set_rew, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_FastReverse") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_rew, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Seek") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_DOUBLE, &(idledata->position), DBUS_TYPE_INVALID)) {
                        g_idle_add(set_position, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Terminate") == 0) {
/*
					shutdown();
                                        gm_log(verbose, G_LOG_LEVEL_DEBUG, "waiting for all events to drain and thread to become NULL");
					while(gtk_events_pending() || thread != NULL) {
						gtk_main_iteration();
					}					
					dbus_unhook();
                    gtk_main_quit();
*/
                    if (media != NULL) {
                        if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) != MEDIA_STATE_UNKNOWN)
                            dontplaynext = TRUE;
                    }
                    g_idle_add(set_quit, idledata);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Volume") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_DOUBLE, &volume, DBUS_TYPE_INVALID)) {
                        audio_device.volume = volume / 100.0;
                        g_idle_add(set_volume, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "VolumeUp") == 0 && idledata != NULL) {
                    if (audio_device.volume < 1.0) {
                        audio_device.volume += 0.10;
                        if (audio_device.volume > 1.0)
                            audio_device.volume = 1.0;
                        g_idle_add(set_volume, idledata);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "VolumeDown") == 0 && idledata != NULL) {
                    if (audio_device.volume > 0.0) {
                        audio_device.volume -= 0.10;
                        if (audio_device.volume < 0.0)
                            audio_device.volume = 0.0;
                        g_idle_add(set_volume, idledata);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_Volume") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_DOUBLE, &(volume), DBUS_TYPE_INT32, &source_id,
                         DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            audio_device.volume = volume / 100.0;
                            g_idle_add(set_volume, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetFullScreen") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_BOOLEAN, &(idledata->fullscreen), DBUS_TYPE_INVALID)) {
                        g_idle_add(set_fullscreen, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetPercent") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_DOUBLE, &percent, DBUS_TYPE_INVALID)) {
                        if (media != NULL) {
                            gmtk_media_player_seek(GMTK_MEDIA_PLAYER(media),
                                                   gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media),
                                                                                          ATTRIBUTE_LENGTH) * percent,
                                                   SEEK_ABSOLUTE);
                        }
                        g_idle_add(set_progress_value, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_SetPercent") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_DOUBLE, &percent, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            if (media != NULL) {
                                gmtk_media_player_seek(GMTK_MEDIA_PLAYER(media),
                                                       gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media),
                                                                                              ATTRIBUTE_LENGTH) *
                                                       percent, SEEK_ABSOLUTE);
                            }
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetCachePercent") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_DOUBLE, &(idledata->cachepercent), DBUS_TYPE_INVALID)) {
                        g_idle_add(set_progress_value, idledata);
                        if (idledata->cachepercent > 0.99 && idledata->retry_on_full_cache) {
                            if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(playliststore), &iter)) {
                                gm_log(verbose, G_LOG_LEVEL_DEBUG, "retrying on full cache");
                                play_iter(&iter, 0);
                            }
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetProgressText") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        g_strlcpy(idledata->progress_text, s, sizeof(idledata->progress_text));
                        if (media != NULL) {
                            if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) != MEDIA_STATE_PLAY)
                                g_idle_add(set_progress_text, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_SetProgressText") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        g_strlcpy(idledata->progress_text, s, sizeof(idledata->progress_text));
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_progress_text, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }


                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetMediaLabel") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        g_strlcpy(idledata->media_info, s, sizeof(idledata->progress_text));
                        g_idle_add(set_media_label, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_SetMediaLabel") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        g_strlcpy(idledata->media_info, s, sizeof(idledata->progress_text));
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_media_label, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetInfo") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        g_strlcpy(idledata->info, s, sizeof(idledata->info));
                        g_idle_add(set_title_bar, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetURL") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        if (s != NULL) {
                            if (media != NULL)
                                gmtk_media_player_set_uri(GMTK_MEDIA_PLAYER(media), s);
                            g_strlcpy(idledata->url, s, sizeof(idledata->url));
                            g_idle_add(show_copyurl, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }


                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetShowControls") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_BOOLEAN, &(idledata->showcontrols), DBUS_TYPE_INVALID)) {
                        g_idle_add(set_show_controls, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "ResizeWindow") == 0 && idledata != NULL) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_INT32, &window_x, DBUS_TYPE_INT32, &window_y, DBUS_TYPE_INVALID)) {
                        if (window_x > 0 && window_y > 0)
                            g_idle_add(resize_window, idledata);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "SetGUIState") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_INT32, &guistate, DBUS_TYPE_INVALID)) {
                        g_idle_add(set_gui_state, NULL);
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "RP_SetGUIState") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_INT32, &guistate, DBUS_TYPE_INT32, &source_id, DBUS_TYPE_INVALID)) {
                        if (source_id != control_id) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_gui_state, NULL);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (g_ascii_strcasecmp(dbus_message_get_member(message), "MediaPlayerKeyPressed") == 0) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args
                        (message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_STRING, &keyname, DBUS_TYPE_INVALID)) {
                        if (g_ascii_strncasecmp(keyname, "Play", strlen("Play")) == 0) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_play, idledata);
                        }
                        if (g_ascii_strncasecmp(keyname, "Stop", strlen("Stop")) == 0) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_stop, idledata);
                        }
                        if (g_ascii_strncasecmp(keyname, "Previous", strlen("Previous")) == 0) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_prev, idledata);
                        }
                        if (g_ascii_strncasecmp(keyname, "Next", strlen("Next")) == 0) {
                            idledata->fromdbus = TRUE;
                            g_idle_add(set_next, idledata);
                        }
                    } else {
                        dbus_error_free(&error);
                    }
                    return DBUS_HANDLER_RESULT_HANDLED;
                }


                if (g_ascii_strcasecmp(dbus_message_get_member(message), "Ping") == 0) {
                    if (control_id != 0) {
                        path = g_strdup_printf("/control/%i", control_id);
                    }
                    if (embed_window != 0) {
                        path = g_strdup_printf("/window/%i", embed_window);
                    }
                    if (path == NULL) {
                        path = g_strdup_printf("/");
                    }

                    reply_message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "Pong");
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    g_free(path);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
            } else if (message_type == DBUS_MESSAGE_TYPE_METHOD_CALL) {
                gm_log(verbose, G_LOG_LEVEL_DEBUG, "Got member %s", dbus_message_get_member(message));
                if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect")) {

                    xml =
                        g_string_new
                        ("<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
                         "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
                         "<node>\n" "  <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
                         "    <method name=\"Introspect\">\n"
                         "      <arg name=\"data\" direction=\"out\" type=\"s\"/>\n" "    </method>\n"
                         "  </interface>\n");

                    xml = g_string_append(xml,
                                          "<interface name=\"com.gnome.mplayer\">\n"
                                          "    <method name=\"Test\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetVolume\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetFullScreen\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetShowControls\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetTime\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetDuration\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetPercent\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetCacheSize\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetPlayState\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetBitrate\">\n"
                                          "        <arg name=\"url\" type=\"s\" />\n"
                                          "    </method>\n"
                                          "    <method name=\"GetURI\">\n"
                                          "    </method>\n"
                                          "    <method name=\"GetTitle\">\n"
                                          "    </method>\n"
                                          "    <signal name=\"Open\">\n"
                                          "        <arg name=\"url\" type=\"s\" />\n"
                                          "    </signal>\n"
                                          "    <signal name=\"OpenPlaylist\">\n"
                                          "        <arg name=\"url\" type=\"s\" />\n"
                                          "    </signal>\n"
                                          "    <signal name=\"OpenButton\">\n"
                                          "        <arg name=\"url\" type=\"s\" />\n"
                                          "        <arg name=\"hrefid\" type=\"s\" />\n"
                                          "    </signal>\n"
                                          "    <signal name=\"Close\">\n"
                                          "    </signal>\n"
                                          "    <signal name=\"Quit\">\n"
                                          "    </signal>\n"
                                          "    <signal name=\"ResizeWindow\">\n"
                                          "        <arg name=\"width\" type=\"i\" />\n"
                                          "        <arg name=\"height\" type=\"i\" />\n"
                                          "    </signal>\n" "  </interface>\n");
                    xml = g_string_append(xml, "</node>\n");

                    xml_string = g_string_free(xml, FALSE);
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_STRING, &xml_string, DBUS_TYPE_INVALID);
                    g_free(xml_string);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }

                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "Test")) {
                    reply_message = dbus_message_new_method_return(message);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetVolume")) {
                    reply_message = dbus_message_new_method_return(message);
                    volume = audio_device.volume * 100.0;
                    dbus_message_append_args(reply_message, DBUS_TYPE_DOUBLE, &volume, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetFullScreen")) {
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_BOOLEAN, &fullscreen, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetShowControls")) {
                    showcontrols = get_show_controls();
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_BOOLEAN, &showcontrols, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetTime")) {
                    reply_message = dbus_message_new_method_return(message);
                    duration = gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_POSITION);
                    dbus_message_append_args(reply_message, DBUS_TYPE_DOUBLE, &duration, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetDuration")) {
                    reply_message = dbus_message_new_method_return(message);
                    duration = gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_LENGTH);
                    dbus_message_append_args(reply_message, DBUS_TYPE_DOUBLE, &duration, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetPercent")) {
                    reply_message = dbus_message_new_method_return(message);
                    percent =
                        gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_POSITION_PERCENT);
                    dbus_message_append_args(reply_message, DBUS_TYPE_DOUBLE, &percent, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetCacheSize")) {
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &plugin_video_cache_size,
                                             DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    cache_size = plugin_video_cache_size;
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetPluginAudioCacheSize")) {
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &plugin_audio_cache_size,
                                             DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    cache_size = plugin_audio_cache_size;
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetPluginVideoCacheSize")) {
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &plugin_video_cache_size,
                                             DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    cache_size = plugin_video_cache_size;
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetPlayState")) {
                    reply_message = dbus_message_new_method_return(message);
                    if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) == MEDIA_STATE_PAUSE) {
                        js_state = STATE_PAUSED;
                    }

                    if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) == MEDIA_STATE_STOP) {
                        js_state = STATE_STOPPED;
                    }

                    if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) == MEDIA_STATE_PLAY) {
                        js_state = STATE_PLAYING;
                    }

                    if (gmtk_media_player_get_media_state(GMTK_MEDIA_PLAYER(media)) == MEDIA_STATE_BUFFERING) {
                        js_state = STATE_BUFFERING;
                    }

                    if (gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_POSITION) == 0) {
                        if (gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_CACHE_PERCENT) >
                            0.0) {
                            js_state = STATE_BUFFERING;
                        }
                    }
                    dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &js_state, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);

                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetBitrate")) {
                    dbus_error_init(&error);
                    if (dbus_message_get_args(message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
                        bitrate = get_bitrate(s);
                    } else {
                        dbus_error_free(&error);
                        bitrate =
                            gmtk_media_player_get_attribute_integer(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_VIDEO_BITRATE);
                        if (bitrate == 0)
                            bitrate =
                                gmtk_media_player_get_attribute_integer(GMTK_MEDIA_PLAYER(media),
                                                                        ATTRIBUTE_AUDIO_BITRATE);
                    }
                    reply_message = dbus_message_new_method_return(message);
                    dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &bitrate, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetURI")) {
                    reply_message = dbus_message_new_method_return(message);
                    s = g_strdup(idledata->info);
                    dbus_message_append_args(reply_message, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetTitle")) {
                    reply_message = dbus_message_new_method_return(message);
                    if (gmtk_media_player_get_attribute_string(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_TITLE) == NULL) {
                        s = g_strdup(idledata->display_name);
                    } else {
                        s = g_strdup(gmtk_media_player_get_attribute_string(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_TITLE));
                    }
                    dbus_message_append_args(reply_message, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                if (dbus_message_is_method_call(message, "com.gnome.mplayer", "GetCachePercent")) {
                    reply_message = dbus_message_new_method_return(message);
                    percent = gmtk_media_player_get_attribute_double(GMTK_MEDIA_PLAYER(media), ATTRIBUTE_CACHE_PERCENT);
                    dbus_message_append_args(reply_message, DBUS_TYPE_DOUBLE, &percent, DBUS_TYPE_INVALID);
                    dbus_connection_send(connection, reply_message, NULL);
                    dbus_message_unref(reply_message);
                    return DBUS_HANDLER_RESULT_HANDLED;
                }
                gm_log(verbose, G_LOG_LEVEL_MESSAGE, "Unable to find method call");
            } else {
                gm_log(verbose, G_LOG_LEVEL_INFO, "path didn't match - %s", dbus_message_get_path(message));
            }

        }
    }

    g_free(path1);
    g_free(path2);
    g_free(path3);
    g_free(path4);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
#endif

void dbus_open_by_hrefid(gchar *hrefid)
{
#ifdef DBUS_ENABLED
    gchar *path;
    DBusMessage *message;
    gchar *id;

    id = g_strdup(hrefid);
    gm_log(verbose, G_LOG_LEVEL_INFO, "requesting id = %s", id);
    if (connection != NULL && control_id != 0) {
        path = g_strdup_printf("/control/%i", control_id);
        message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "RequestById");
        dbus_message_append_args(message, DBUS_TYPE_STRING, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(path);
    }
    g_free(id);
#endif
}

void dbus_open_next()
{
#ifdef DBUS_ENABLED
    gchar *path;
    DBusMessage *message;

    gm_log(verbose, G_LOG_LEVEL_INFO, "requesting next item from plugin");
    if (connection != NULL && control_id != 0) {
        path = g_strdup_printf("/control/%i", control_id);
        message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "Next");
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(path);
    }
#endif
}

void dbus_open(gchar *arg)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localarg;
    DBusMessage *message;

    if (connection != NULL) {
        path = g_strdup_printf("/");
        if (replace_and_play) {
            message = dbus_message_new_signal(path, "com.gnome.mplayer", "Open");
        } else {
            message = dbus_message_new_signal(path, "com.gnome.mplayer", "Add");
        }
        localarg = g_strdup(arg);
        dbus_message_append_args(message, DBUS_TYPE_STRING, &localarg, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localarg);
        g_free(path);
    }
#endif
}

void dbus_set_playlist_name(gchar *arg)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localarg;
    DBusMessage *message;

    if (connection != NULL) {
        path = g_strdup_printf("/");
        message = dbus_message_new_signal(path, "com.gnome.mplayer", "SetPlaylistName");
        localarg = g_strdup(arg);
        dbus_message_append_args(message, DBUS_TYPE_STRING, &localarg, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localarg);
        g_free(path);
    }
#endif
}

void dbus_cancel()
{
#ifdef DBUS_ENABLED
    gchar *path;
    DBusMessage *message;
    gint id;

    id = control_id;
    if (connection != NULL && control_id != 0) {
        path = g_strdup_printf("/control/%i", control_id);
        message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "Cancel");
        dbus_message_append_args(message, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(path);
    }
#endif
}

void dbus_send_rpsignal(gchar *signal)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localsignal;
    DBusMessage *message;
    gint id;

    if (g_ascii_strcasecmp(rpconsole, "NONE") == 0)
        return;

    id = control_id;
    if (connection != NULL) {
        path = g_strdup_printf("/console/%s", rpconsole);
        localsignal = g_strdup(signal);
        message = dbus_message_new_signal(path, "com.gnome.mplayer", localsignal);
        dbus_message_append_args(message, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localsignal);
        g_free(path);
    }
#endif
}

void dbus_send_rpsignal_with_int(gchar *signal, int value)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localsignal;
    DBusMessage *message;
    gint id;

    if (g_ascii_strcasecmp(rpconsole, "NONE") == 0)
        return;

    id = control_id;
    if (connection != NULL) {
        path = g_strdup_printf("/console/%s", rpconsole);
        localsignal = g_strdup(signal);
        message = dbus_message_new_signal(path, "com.gnome.mplayer", localsignal);
        dbus_message_append_args(message, DBUS_TYPE_INT32, &value, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localsignal);
        g_free(path);
    }
#endif
}

void dbus_send_rpsignal_with_double(gchar *signal, gdouble value)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localsignal;
    DBusMessage *message;
    gint id;

    if (g_ascii_strcasecmp(rpconsole, "NONE") == 0)
        return;

    id = control_id;
    if (connection != NULL) {
        path = g_strdup_printf("/console/%s", rpconsole);
        localsignal = g_strdup(signal);
        message = dbus_message_new_signal(path, "com.gnome.mplayer", localsignal);
        dbus_message_append_args(message, DBUS_TYPE_DOUBLE, &value, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localsignal);
        g_free(path);
    }
#endif
}

void dbus_send_rpsignal_with_string(gchar *signal, gchar *value)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localsignal;
    DBusMessage *message;
    gint id;
    gchar *localstr;

    if (g_ascii_strcasecmp(rpconsole, "NONE") == 0)
        return;

    id = control_id;
    if (connection != NULL) {
        path = g_strdup_printf("/console/%s", rpconsole);
        localsignal = g_strdup(signal);
        localstr = g_strdup(value);
        message = dbus_message_new_signal(path, "com.gnome.mplayer", localsignal);
        dbus_message_append_args(message, DBUS_TYPE_STRING, &localstr, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(localstr);
        g_free(localsignal);
        g_free(path);
    }
#endif
}

void dbus_reload_plugins()
{
#ifdef DBUS_ENABLED
    gchar *path;
    DBusMessage *message;

    if (connection != NULL && control_id != 0) {
        path = g_strdup_printf("/control/%i", control_id);
        message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "ReloadPlugins");
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(path);
    }
#endif
}

void dbus_send_event(gchar *event, gint button)
{
#ifdef DBUS_ENABLED
    gchar *path;
    gchar *localevent;
    gint localbutton = 0;
    DBusMessage *message;

    localbutton = button;


    if (connection != NULL && control_id != 0) {
        path = g_strdup_printf("/control/%i", control_id);
        localevent = g_strdup_printf("%s", event);
        gm_log(verbose, G_LOG_LEVEL_INFO, "Posting Event %s", localevent);
        message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "Event");
        dbus_message_append_args(message, DBUS_TYPE_STRING, &localevent, DBUS_TYPE_INT32,
                                 &localbutton, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, message, NULL);
        dbus_message_unref(message);
        g_free(path);
        g_free(localevent);
    }
#endif
}

gboolean GetProperty(gchar *property)
{

    return TRUE;
}

gboolean dbus_hookup(gint windowid, gint controlid)
{
#ifdef DBUS_ENABLED

    DBusError error;
    DBusBusType type = DBUS_BUS_SESSION;
    gchar *match;
    gchar *path;
    DBusMessage *reply_message;
    gint id;
    gint ret;
    gchar *filename;
    gboolean local_rnp;

    dbus_error_init(&error);
    connection = dbus_bus_get(type, &error);
    if (connection == NULL) {
        gm_log(verbose, G_LOG_LEVEL_MESSAGE, "Failed to open connection to %s message bus: %s",
               (type == DBUS_BUS_SYSTEM) ? "system" : "session", error.message);
        dbus_error_free(&error);
        return FALSE;
    }
    dbus_connection_setup_with_g_main(connection, NULL);

    // vtable.message_function = &message_handler;


    // dbus_bus_request_name(connection,"com.gnome.mplayer",0,NULL);
    // dbus_connection_register_object_path (connection,"/com/gnome/mplayer", &vtable,NULL);

    match = g_strdup_printf("type='signal',interface='com.gnome.mplayer'");
    dbus_bus_add_match(connection, match, &error);
    gm_log(verbose, G_LOG_LEVEL_INFO, "Using match: %s", match);
    g_free(match);
    dbus_error_free(&error);

    match = g_strdup_printf("type='signal',interface='org.gnome.SettingsDaemon'");
    dbus_bus_add_match(connection, match, &error);
    gm_log(verbose, G_LOG_LEVEL_INFO, "Using match: %s", match);
    g_free(match);
    dbus_error_free(&error);
    if (use_mediakeys) {
        match = g_strdup_printf("type='signal',interface='org.gnome.SettingsDaemon.MediaKeys'");
        dbus_bus_add_match(connection, match, &error);
        gm_log(verbose, G_LOG_LEVEL_INFO, "Using match: %s", match);
        g_free(match);
        dbus_error_free(&error);
    }

    dbus_connection_add_filter(connection, filter_func, NULL, NULL);

    gm_log(verbose, G_LOG_LEVEL_INFO, "Proxy connections and Command connected");

    if (control_id != 0) {
        path = g_strdup_printf("com.gnome.mplayer.cid%i", control_id);
        ret = dbus_bus_request_name(connection, path, 0, NULL);
        g_free(path);
        path = g_strdup_printf("/control/%i", control_id);
        id = control_id;
        reply_message = dbus_message_new_signal(path, "com.gecko.mediaplayer", "Ready");
        dbus_message_append_args(reply_message, DBUS_TYPE_INT32, &id, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply_message, NULL);
        dbus_message_unref(reply_message);
        g_free(path);
    } else {
        ret = dbus_bus_request_name(connection, "com.gnome.mplayer", 0, NULL);
        if (single_instance && ret > 1) {
            if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(playliststore), &iter)) {
                gtk_tree_model_get(GTK_TREE_MODEL(playliststore), &iter, ITEM_COLUMN, &filename, -1);
                dbus_open(filename);
                local_rnp = replace_and_play;
                while (gtk_tree_model_iter_next(GTK_TREE_MODEL(playliststore), &iter)) {
                    replace_and_play = FALSE;
                    gtk_tree_model_get(GTK_TREE_MODEL(playliststore), &iter, ITEM_COLUMN, &filename, -1);
                    dbus_open(filename);
                }
                if (playlistname != NULL && local_rnp)
                    dbus_set_playlist_name(playlistname);
            }
            exit(EXIT_SUCCESS);
        }
    }
#endif
    screensaver_disabled = FALSE;
    ss_cookie_is_valid = FALSE;
    sm_cookie_is_valid = FALSE;
    fd_cookie_is_valid = FALSE;
    return TRUE;
}

void dbus_unhook()
{
    dbus_enable_screensaver();
#ifdef DBUS_ENABLED
    if (connection != NULL) {
        // dbus_connection_close(connection);
        dbus_connection_unref(connection);
        connection = NULL;
    }
#endif
}

#ifdef DBUS_ENABLED
static gboolean switch_screensaver_dbus_gnome_screensaver(gboolean enabled)
{
    DBusMessage *message;
    DBusError error;
    DBusMessage *reply_message;
    gchar *app;
    gchar *reason;
    const gchar *busname = "org.gnome.ScreenSaver";
    const gchar *objpath = "/org/gnome/ScreenSaver";
    dbus_bool_t has_owner;
    gboolean retval = FALSE;

    if (g_getenv("GM_DISABLE_ORG_GNOME_SCREENSAVER")) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: disabled with env var", busname);
        return FALSE;
    }

    if (connection == NULL) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no connection", busname);
        return FALSE;
    }

    dbus_error_init(&error);

    has_owner = dbus_bus_name_has_owner(connection, busname, &error);

    if (!has_owner || dbus_error_is_set(&error)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no owner", busname);
        dbus_error_free(&error);
        return FALSE;
    }

    if (enabled) {
        if (!ss_cookie_is_valid) {
            return FALSE;
        }
        ss_cookie_is_valid = FALSE;

        message = dbus_message_new_method_call(busname, objpath, busname, "UnInhibit");
        dbus_message_append_args(message, DBUS_TYPE_UINT32, &ss_cookie, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);
        dbus_message_unref(message);
        if (error.message == NULL && reply_message) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
            retval = TRUE;
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    } else {
        message = dbus_message_new_method_call(busname, objpath, busname, "Inhibit");
        app = g_strdup_printf("gnome-mplayer");
        reason = g_strdup_printf("playback");
        dbus_message_append_args(message, DBUS_TYPE_STRING, &app, DBUS_TYPE_STRING, &reason, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);

        dbus_message_unref(message);
        g_free(reason);
        g_free(app);

        if (error.message == NULL && reply_message
            && dbus_message_get_args(reply_message, &error, DBUS_TYPE_UINT32, &ss_cookie, NULL)) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
            ss_cookie_is_valid = TRUE;
            retval = TRUE;
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    }
}

static gboolean switch_screensaver_dbus_gnome_sessionmanager(gboolean enabled)
{

    DBusError error;
    DBusMessage *reply_message;
    DBusMessage *message;
    gchar *app;
    gchar *reason;
    gint flags;
    gint windowid;
    const gchar *busname = "org.gnome.SessionManager";
    const gchar *objpath = "/org/gnome/SessionManager";
    dbus_bool_t has_owner;
    gboolean retval = FALSE;

    if (g_getenv("GM_DISABLE_ORG_GNOME_SESSIONMANAGER")) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: disabled with env var", busname);
        return FALSE;
    }

    if (connection == NULL) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no connection", busname);
        return FALSE;
    }
    dbus_error_init(&error);

    has_owner = dbus_bus_name_has_owner(connection, busname, &error);

    if (!has_owner || dbus_error_is_set(&error)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no owner", busname);
        dbus_error_free(&error);
        return FALSE;
    }
    if (enabled) {
        if (!sm_cookie_is_valid) {
            return FALSE;
        }
        sm_cookie_is_valid = FALSE;

        message = dbus_message_new_method_call(busname, objpath, busname, "UnInhibit");
        dbus_message_append_args(message, DBUS_TYPE_UINT32, &sm_cookie, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);
        dbus_message_unref(message);
        if (error.message == NULL && reply_message) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
            retval = TRUE;
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    } else {
        message = dbus_message_new_method_call(busname, objpath, busname, "Inhibit");
        app = g_strdup_printf("gnome-mplayer");
        reason = g_strdup_printf("playback");
        flags = 8;
        windowid = GDK_WINDOW_XID(gmtk_get_window(window));
        dbus_message_append_args(message, DBUS_TYPE_STRING, &app, DBUS_TYPE_UINT32,
                                 &windowid, DBUS_TYPE_STRING, &reason, DBUS_TYPE_UINT32, &flags, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);

        dbus_message_unref(message);
        g_free(reason);
        g_free(app);

        if (error.message == NULL && reply_message
            && dbus_message_get_args(reply_message, &error, DBUS_TYPE_UINT32, &sm_cookie, NULL)) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
            sm_cookie_is_valid = TRUE;
            retval = TRUE;
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    }
}

static gboolean switch_screensaver_dbus_freedesktop_screensaver(gboolean enabled)
{
    DBusMessage *message;
    DBusError error;
    DBusMessage *reply_message;
    gchar *app;
    gchar *reason;
    const gchar *busname = "org.freedesktop.ScreenSaver";
    const gchar *objpath = "/ScreenSaver";
    dbus_bool_t has_owner;
    gboolean retval = FALSE;

    if (g_getenv("GM_DISABLE_ORG_FREEDESKTOP_SCREENSAVER")) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: disabled with env var", busname);
        return FALSE;
    }

    if (connection == NULL) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no connection", busname);
        return FALSE;
    }

    dbus_error_init(&error);

    has_owner = dbus_bus_name_has_owner(connection, busname, &error);

    if (!has_owner || dbus_error_is_set(&error)) {
        dbus_error_free(&error);
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no owner", busname);
        return FALSE;
    }

    if (enabled) {

        if (!fd_cookie_is_valid) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: not enabling since disabled with a different method", busname);
            return FALSE;
        }
        fd_cookie_is_valid = FALSE;

        message = dbus_message_new_method_call(busname, objpath, busname, "UnInhibit");
        dbus_message_append_args(message, DBUS_TYPE_UINT32, &fd_cookie, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);
        dbus_message_unref(message);
        if (error.message == NULL && reply_message) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
            retval = TRUE;
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    } else {
        message = dbus_message_new_method_call(busname, objpath, busname, "Inhibit");
        app = g_strdup_printf("gnome-mplayer");
        reason = g_strdup_printf("playback");
        dbus_message_append_args(message, DBUS_TYPE_STRING, &app, DBUS_TYPE_STRING, &reason, DBUS_TYPE_INVALID);
        reply_message =
            dbus_connection_send_with_reply_and_block(connection, message, WAIT_FOR_REPLY_TIMEOUT_MSEC, &error);

        dbus_message_unref(message);
        g_free(reason);
        g_free(app);

        if (error.message == NULL && reply_message
            && dbus_message_get_args(reply_message, &error, DBUS_TYPE_UINT32, &fd_cookie, NULL)) {
            fd_cookie_is_valid = TRUE;
            retval = TRUE;
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: got a reply, yay", busname);
        } else {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "%s: no reply, nobody seems to be answering here", busname);
        }
        if (reply_message) {
            dbus_message_unref(reply_message);
        }
        dbus_error_free(&error);
        return retval;
    }
}
#endif                          // DBUS_ENABLED

static gboolean switch_screensaver_xdg_screensaver(gboolean enabled)
{
    gchar *exe;
    gint windowid;
    gchar *windowid_s;
    gboolean spawn_ret;
    gchar **argv = NULL;
    GSpawnFlags flags;
    gchar *standard_output = NULL;
    gchar *standard_error = NULL;
    GError *error = NULL;
    gint exit_status = 0;

    if (g_getenv("GM_DISABLE_XDG_SCREENSAVER")) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver: disabled with env var");
        return FALSE;
    }

    exe = g_find_program_in_path("xdg-screensaver");
    if (exe == NULL) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver executable not found");
        return FALSE;
    }

    windowid = GDK_WINDOW_XID(gmtk_get_window(window));
    windowid_s = g_strdup_printf("%d", windowid);

    argv = g_new0(gchar *, 4);
    argv[0] = exe;
    argv[1] = g_strdup(enabled ? "resume" : "suspend");
    argv[2] = windowid_s;
    argv[3] = NULL;

    gm_log(verbose, G_LOG_LEVEL_DEBUG, "executing %s %s %s", argv[0], argv[1], argv[2]);

    flags = 0;

    spawn_ret = g_spawn_sync(NULL,      /* const gchar * working dir */
                             argv,      /* */
                             NULL,      /* gchar **envp */
                             flags,     /* */
                             NULL,      /* GSpawnChildSetupFunc child_setup */
                             NULL,      /* gpointer user_data */
                             &standard_output, &standard_error, &exit_status, &error);

    g_free(argv[1]);
    g_free(windowid_s);
    windowid_s = NULL;
    g_free(exe);
    exe = NULL;
    g_free(argv);
    argv = NULL;

    if (standard_output) {
        g_strstrip(standard_output);
        if (standard_output[0] != '\0') {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver STDOUT: %s", standard_output);
        }
        g_free(standard_output);
        standard_output = NULL;
    }
    if (standard_error) {
        g_strstrip(standard_error);
        if (standard_error[0] != '\0') {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver STDERR: %s", standard_error);
        }
        g_free(standard_error);
        standard_error = NULL;
    }

    if (error) {
        if (error->message) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver error: %s", error->message);
        }
        g_error_free(error);
        error = NULL;
    }

    if (exit_status) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver exit status: %d", WEXITSTATUS(exit_status));
        if (WIFSIGNALED(exit_status)) {
            gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver terminated by signal %d", WTERMSIG(exit_status));
#ifdef WCOREDUMP
            if (WCOREDUMP(exit_status)) {
                gm_log(verbose, G_LOG_LEVEL_DEBUG, "xdg-screensaver dumped core");
            }
#endif
        }
    }

    if (spawn_ret) {
        return TRUE;
    } else {
        return FALSE;
    }

}

#ifdef XSCRNSAVER_ENABLED
static gboolean switch_screensaver_x11(gboolean enabled)
{
    Display *dpy;
    int event_base_return, error_base_return;

    if (g_getenv("GM_DISABLE_XSCREENSAVERSUSPEND")) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "XScreenSaverSuspend: disabled with env var");
        return FALSE;
    }

    dpy = GDK_WINDOW_XDISPLAY(gmtk_get_window(window));
    if (!XScreenSaverQueryExtension(dpy, &event_base_return, &error_base_return)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "XScreenSaverSuspend: extension not found");
        return FALSE;
    }
    XScreenSaverSuspend(dpy, !enabled);
    return TRUE;
}
#endif

static gboolean switch_screensaver(gboolean enabled)
{
    gm_log(verbose, G_LOG_LEVEL_INFO, "trying to %s screensaver", enabled ? "enable" : "disable");

    if (enabled && !screensaver_disabled) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "no need to enable screensaver - already is");
        return TRUE;
    }
    if (!enabled && screensaver_disabled) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "no need to disable screensaver - already is");
        return TRUE;
    }

    screensaver_disabled = !enabled;

#ifdef DBUS_ENABLED
    if (switch_screensaver_dbus_freedesktop_screensaver(enabled)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "switched screensaver using dbus org.freedesktop.ScreenSaver");
        return TRUE;
    }
    if (switch_screensaver_dbus_gnome_sessionmanager(enabled)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "switched screensaver using dbus org.gnome.SessionManager");
        return TRUE;
    }
    if (switch_screensaver_dbus_gnome_screensaver(enabled)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "switched screensaver using dbus org.gnome.ScreenSaver");
        return TRUE;
    }
#endif
    if (switch_screensaver_xdg_screensaver(enabled)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "switched screensaver using xdg-screensaver");
        return TRUE;
    }
#ifdef XSCRNSAVER_ENABLED
    if (switch_screensaver_x11(enabled)) {
        gm_log(verbose, G_LOG_LEVEL_DEBUG, "switched screensaver using X11");
        return TRUE;
    }
#endif
    gm_log(verbose, G_LOG_LEVEL_INFO, "did not find any valid method to switch screensaver");
    return FALSE;
}

void dbus_enable_screensaver()
{
    (void) switch_screensaver(TRUE);
}

void dbus_disable_screensaver()
{
    (void) switch_screensaver(FALSE);
}
