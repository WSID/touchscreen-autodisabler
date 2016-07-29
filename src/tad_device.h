/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_device.h
 * Copyright (C) 2014 WSID <jongsome@naver.com>
 *
 * touchscreen-autodisabler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * touchscreen-autodisabler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef __TAD_DEVICE_HEADER
#define __TAD_DEVICE_HEADER

#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

#include <glib.h>
#include <glib-object.h>

#include "tad_def.h"

/* **** Object Types **********************************************************/

typedef struct _TadWatcher TadWatcher;

#define TAD_TYPE_WATCHER (tad_watcher_get_type())

G_DECLARE_FINAL_TYPE(TadWatcher, tad_watcher, TAD, WATCHER, GObject);



/* **** Object Constructors ***************************************************/

TadWatcher *tad_watcher_new (void);




/* **** Property Getter/Setters ***********************************************/

gchar **tad_watcher_get_watch (TadWatcher *watcher);

void    tad_watcher_set_watch (TadWatcher  *watcher,
                               gchar      **watch);


gchar **tad_watcher_get_control (TadWatcher *watcher);

void    tad_watcher_set_control (TadWatcher  *watcher,
                                 gchar      **control);


void    tad_watcher_set_control_enabled (TadWatcher *watcher,
                                         gboolean    enabled);

gboolean tad_watcher_get_control_enabled (TadWatcher *watcher);



guint   tad_watcher_start_watch (TadWatcher  *watcher);



#endif  //TAD_DEVICE_HEADER
