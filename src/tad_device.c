/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_device.c
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib-object.h>
#include <glib-unix.h>

#include "tad_def.h"
#include "tad_util.h"
#include "tad_device.h"



/* **** Static variables *****************************************************/

static XIEventMask watch_mask_template;
static XIEventMask control_mask_template;


/* **** Signals and Properties ************************************************/

static guint SIG_EVENT;

enum {
  PROP_0,
  PROP_WATCH,
  PROP_CONTROL,

  PROP_CONTROL_ENABLED,
  PROP_COUNT
};

static GParamSpec *pspecs[PROP_COUNT] = {NULL};


/* **** Object Definition *****************************************************/

struct _TadWatcher {
  GObject _parent;
  Display *display;
  Window   window;

  gint     xi_opcode;
  gint     xi_event_code;
  gint     xi_error_code;

  XDeviceInfo *devinfos;
  gint        ndevinfos;

  Atom     atom_wac_ser_id;

  gchar  **watch;
  gchar  **control;

  guint   nwatch;
  guint   ncontrol;

  GArray  *masks;

  GSource *source;

  guint    touch_count;

  gboolean control_enabled;
};

G_DEFINE_TYPE (TadWatcher, tad_watcher, G_TYPE_OBJECT)



/*
 * TadWatchSource:
 * _parent: Parent Structure.
 * event: XEvent for callback function.
 *
 * Source structure for watching X Events.
 */
typedef struct _TadWatchSource
{
  GSource _parent;

  Display *display;
  XEvent   event;
} TadWatchSource;






/* **** GObject Virtual Functions *********************************************/

static void tad_watcher_constructed (GObject *object);

static void tad_watcher_finalize (GObject *object);

static void tad_watcher_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec);

static void tad_watcher_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec);




/* **** Private Functions *****************************************************/

static XDeviceInfo     *tad_watcher_get_device   (TadWatcher  *watcher,
                                                  const gchar *name);

static XDeviceInfo    **tad_watcher_get_devices  (TadWatcher    *watcher,
                                                  gchar * const *name,
                                                  guint         *length);

static XDeviceInfo    **tad_watcher_get_devices_by_types (TadWatcher    *watcher,
                                                          gchar * const *types,
                                                          guint         *length);


static gboolean tad_watcher_device_is_proximity (TadWatcher *watcher,
                                                 XID         device);



static void		tad_watcher_mask_set_enabled (TadWatcher  *watcher,
                                              XIEventMask *mask,
                                              gboolean     grabbed);




static guint tad_watcher_update_masks (TadWatcher *watcher,
			 						   GArray     *masks,
		 							   gchar     **devices,
									   gchar     **types,
									   XIEventMask *template);

static void tad_watcher_update_all_masks (TadWatcher *watcher);





static gboolean tad_watcher_watch_func (gpointer userdata);


static TadEvent	tad_watcher_transform_event (TadWatcher    *watcher,
                                             XIDeviceEvent *event);



/* **** Source Functions ******************************************************/

static GSource *tad_watch_source_new        (Display *display);

static XEvent  *tad_watch_source_get_event  (GSource *source);

static gboolean tad_watch_prepare           (GSource *source,
                                             gint    *timeout);

static gboolean	tad_watch_check             (GSource *source);

static gboolean	tad_watch_dispatch			(GSource     *source,
                           				     GSourceFunc  callback,
                           				     gpointer     userdata);



/* **** GTypeInstance Function Implementations ********************************/

static void
tad_watcher_init (TadWatcher *self)
{
  self->control_enabled = TRUE;
}

static void
tad_watcher_class_init (TadWatcherClass *c)
{
  GObjectClass *c_g_object;

  SIG_EVENT = g_signal_new ("event",
							TAD_TYPE_WATCHER,
							G_SIGNAL_RUN_LAST,
							0,
							NULL, NULL,
							g_cclosure_marshal_VOID__INT,
							G_TYPE_NONE,
							1,
							G_TYPE_INT);



  c_g_object = G_OBJECT_CLASS (c);

  c_g_object->constructed = tad_watcher_constructed;
  c_g_object->finalize = tad_watcher_finalize;

  c_g_object->get_property = tad_watcher_get_property;
  c_g_object->set_property = tad_watcher_set_property;

  pspecs[PROP_WATCH] = g_param_spec_boxed ("watch", "Watched Devices",
										   "Watched Device Names",
										   G_TYPE_STRV,
										   G_PARAM_READWRITE |
										   G_PARAM_STATIC_STRINGS );

  pspecs[PROP_CONTROL] = g_param_spec_boxed ("control", "Controlled Devices",
											 "Controlled Device Names",
											 G_TYPE_STRV,
											 G_PARAM_READWRITE |
											 G_PARAM_STATIC_STRINGS );

  pspecs[PROP_CONTROL_ENABLED] = g_param_spec_boolean ("control-enabled", "Controlled Device Enabled",
													   "Whether controlled devices are enabled or disabled",
													   TRUE,
													   G_PARAM_READWRITE |
													   G_PARAM_STATIC_STRINGS );

  g_object_class_install_properties (c_g_object, PROP_COUNT, pspecs);

  // Initialize template mask.
  watch_mask_template.mask_len = XIMaskLen (XI_LASTEVENT);
  watch_mask_template.mask = g_new0 (guchar, watch_mask_template.mask_len);

  XISetMask (watch_mask_template.mask, XI_PropertyEvent);

  control_mask_template.mask_len = XIMaskLen (XI_LASTEVENT);
  control_mask_template.mask = g_new0 (guchar, control_mask_template.mask_len);

  XISetMask (control_mask_template.mask, XI_RawTouchBegin);
  XISetMask (control_mask_template.mask, XI_RawTouchEnd);
}





/* **** GObject Function Implementations **************************************/

static void
tad_watcher_constructed (GObject *object)
{
  TadWatcher *self = (TadWatcher*)object;

  ((GObjectClass*)tad_watcher_parent_class)->constructed (object);

  self->display = XOpenDisplay (g_getenv ("DISPLAY"));
  self->window = DefaultRootWindow (self->display);

  if (!XQueryExtension(self->display,
					   "XInputExtension",
					   & self->xi_opcode,
					   & self->xi_event_code,
					   & self->xi_error_code))
    g_error (_("XInput Extension is not availiable!\n"));

  self->devinfos = XListInputDevices(self->display, & self->ndevinfos);
  self->atom_wac_ser_id = XInternAtom (self->display, "Wacom Serial IDs", FALSE);

  self->masks = g_array_new (FALSE, FALSE,  sizeof (XIEventMask));
  self->touch_count = 0;
}

static void
tad_watcher_finalize (GObject *object)
{
  TadWatcher *self = (TadWatcher*) object;

  ((GObjectClass*)tad_watcher_parent_class)->finalize (object);

  if (self->source != NULL)
	{
	  g_source_destroy (self->source);
	  g_source_unref (self->source);
	  self->source = NULL;
	}

  g_clear_pointer (& self->masks, (GDestroyNotify) g_array_unref);
  g_clear_pointer (& self->devinfos, (GDestroyNotify) XFreeDeviceList);
  g_clear_pointer (& self->display, (GDestroyNotify) XCloseDisplay);

  g_clear_pointer (& self->watch, (GDestroyNotify) g_strfreev);
  g_clear_pointer (& self->control, (GDestroyNotify) g_strfreev);
}


static void
tad_watcher_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  TadWatcher *self = (TadWatcher*) object;

  switch (prop_id)
	{
	case PROP_WATCH:
	  g_value_set_boxed (value, tad_watcher_get_watch (self));
	  break;

	case PROP_CONTROL:
	  g_value_set_boxed (value, tad_watcher_get_control (self));
	  break;

	case PROP_CONTROL_ENABLED:
	  g_value_set_boolean (value, tad_watcher_get_control_enabled (self));
	  break;

	default:
	  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}


static void
tad_watcher_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  TadWatcher *self = (TadWatcher*) object;

  switch (prop_id)
	{
	case PROP_WATCH:
	  tad_watcher_set_watch (self, g_value_get_boxed (value));
	  break;

	case PROP_CONTROL:
	  tad_watcher_set_control (self, g_value_get_boxed (value));
	  break;

	case PROP_CONTROL_ENABLED:
      tad_watcher_set_control_enabled (self, g_value_get_boolean (value));
	  break;

	default:
	  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}




/* **** Private Functions *****************************************************/

static XDeviceInfo*
tad_watcher_get_device (TadWatcher  *self,
                        const gchar *name)
{
  guint i = 0;

  int device_id_int;

  if (tad_str_is_integer (name, &device_id_int))
	{
	  if (device_id_int == 0)
		{
		  g_warning (_("0 is invalid device id."));
		  return NULL;
		}
	  else if (self->ndevinfos < device_id_int)
		{
		  g_warning (_("Device ID out of bound: %d / %d"),
			  	     device_id_int, self->ndevinfos);
		  return NULL;
		}

	  for (i = 0; i < self->ndevinfos; i++)
		{
		  if (self->devinfos[i].id == device_id_int)
			return self->devinfos + i;
		}
	}


  else for (i = 0 ; i < self->ndevinfos; i++)
	{
	  if (strcmp (self->devinfos[i].name, name) == 0)
		return self->devinfos + i;
	}
  return NULL;
}


static XDeviceInfo**
tad_watcher_get_devices (TadWatcher    *self,
                         gchar * const *names,
                         guint         *length)
{
  GPtrArray *result = g_ptr_array_new ();
  guint i;

  for (i = 0; names[i] != NULL; i++)
	{
      XDeviceInfo *device = tad_watcher_get_device (self, names[i]);

	  if (device == NULL)
		g_warning (_("device %s is not found."), names[i]);
	  else
	    g_ptr_array_add (result, device);
	}

  if (length != NULL) *length = result->len;
  return (XDeviceInfo**) g_ptr_array_free (result, FALSE);
}


static XDeviceInfo**
tad_watcher_get_devices_by_types (TadWatcher    *self,
                                  gchar * const *types,
                                  guint         *length)
{
  GPtrArray *result = g_ptr_array_new ();
  GArray *atoms = g_array_new (FALSE, FALSE, sizeof (Atom));
  guint i;
  guint j;

  // Gets Atoms
  for (i = 0; types[i] != NULL; i++)
	{
	  Atom type_atom = XInternAtom (self->display, types[i], FALSE);
	  g_array_append_val (atoms, type_atom);
	}

  // Gets Result.
  for (i = 0; i < self->ndevinfos; i++)
	{
	  XDeviceInfo *devinfo = self->devinfos + i;

      for (j = 0; j < atoms->len; j++)
		{
		  if (devinfo->type == g_array_index (atoms, Atom, j))
			{
			  g_ptr_array_add (result, devinfo);
			  break;
			}
		}
	}

  g_array_free (atoms, TRUE);

  if (length != NULL) *length = result->len;
  return (XDeviceInfo**) g_ptr_array_free (result, FALSE);
}


static gboolean
tad_watcher_device_is_proximity (TadWatcher *self,
                                 XID         tablet)
{
  Atom      type_return;	// for wacom side.
  gint		format_return;  // ''

  gulong	num_item;		// ''
  gulong    bytes_after;	// ''
  guint32  *data;			// ''

  gboolean proximity;

  XIGetProperty (self->display, tablet, self->atom_wac_ser_id,
				 3, 1,
				 FALSE,
				 XA_INTEGER,
				 &type_return, &format_return,
				 &num_item, &bytes_after,
				 (guchar**) &data);

  proximity = (*data != 0);
  XFree (data);

  return proximity;
}

/*
 * tad_device_set_enabled:
 * @device_id:  XID of device to disable.
 * @enabled:	enabled status to set.
 *
 * Enables or disables device.
 */
static void
tad_watcher_mask_set_enabled (TadWatcher  *self,
                              XIEventMask *mask,
                              gboolean     enabled)
{
	if (!enabled) {
		XIGrabDevice (self->display,
			          mask->deviceid,
			          self->window,
			          CurrentTime,
			          None,
			          GrabModeAsync,
			          GrabModeAsync,
			          true,
			          mask);
	}
	else {
		XIUngrabDevice (self->display,
		                mask->deviceid,
		                CurrentTime);
	}
}

static guint
tad_watcher_update_masks (TadWatcher  *self,
                          GArray      *masks,
                          gchar      **device_names,
                          gchar      **types,
                          XIEventMask *template)
{
  guint plen = masks->len;
  guint i = 0;

  XDeviceInfo **devices = NULL;
  guint        ndevices = 0;

  // Add devices from device names.
  if (device_names != NULL && device_names[0] != NULL)
	devices = tad_watcher_get_devices (self, device_names, &ndevices);


  // Find devices from device types.
  if (ndevices == 0 && types != NULL && types[0] != NULL)
	devices = tad_watcher_get_devices_by_types (self, types, &ndevices);


  if (devices != NULL)
	{
	  // Add masks.
	  for (i = 0; i < ndevices; i++)
		{
          template->deviceid = devices[i]->id;
		  g_array_append_val (masks, *template);

		  g_message ("    - %s", devices[i]->name);
		}

	  g_free (devices);
	}
  else
	{
      // If still none found, throw some warnings.
	  g_warning (_("None of devices are added. "
                   "touchscreen-autodisabler won't work properly."));

	  if ((device_names == NULL || device_names [0] == NULL) &&
		  types != NULL)
		{
		  gchar *types_join = g_strjoinv (", ", types);
		  g_warning (_("It seems that devices of type %s doesn't be detected. "
                       "How about setting device list directly?"),
					 types_join);

		  g_free (types_join);
		}
	}

  return (masks->len - plen);
}

static void
tad_watcher_update_all_masks (TadWatcher *self)
{
  static gchar *watch_types[] = { "TABLET", "STYLUS", "ERASER", NULL };
  static gchar *control_types[] = { "TOUCHSCREEN", NULL };

  g_array_set_size (self->masks, 0);

  g_message (_("  Watched Devices"));
  self->nwatch =
  tad_watcher_update_masks (self, self->masks, self->watch,
						    watch_types, &watch_mask_template);

  g_message (_("  Controlled Devices"));
  self->ncontrol =
  tad_watcher_update_masks (self, self->masks, self->control,
							control_types, &control_mask_template);

  XISelectEvents (self->display, self->window,
				  (XIEventMask*) self->masks->data, self->masks->len);
}

static gboolean
tad_watcher_watch_func (gpointer userdata)
{
  TadWatcher *self = (TadWatcher*) userdata;

  XEvent *event = tad_watch_source_get_event (g_main_current_source ());
  XGenericEventCookie* ev_cookie;
  XIDeviceEvent* ev_dev;

  ev_cookie = & event->xcookie;

  TadEvent stimulus;

  if ((XGetEventData (self->display, ev_cookie)) &&
	  (ev_cookie->type == GenericEvent) &&
	  (ev_cookie->extension == self->xi_opcode))
	{
	  ev_dev = (XIDeviceEvent*) ev_cookie->data;
	  stimulus = tad_watcher_transform_event (self, ev_dev);

	  if (stimulus != TAD_EVENT_INVAILD)
        g_signal_emit (self, SIG_EVENT, 0, stimulus);
	}
  return TRUE;


}

/*
 * tad_device_event_transform:
 * @event: event to be transformed.
 *
 * Transforms #XIDeviceEvent structure into #TadEvent.
 *
 * Returns: a corresponding #TadEvent
 */
static TadEvent
tad_watcher_transform_event (TadWatcher    *self,
                             XIDeviceEvent *event)
{
  XIPropertyEvent  *prop_event;	    // ''

  guint i;

  switch (event->evtype)
	{
	case XI_PropertyEvent:
	  prop_event = (XIPropertyEvent*)event;

	  if (prop_event->property == self->atom_wac_ser_id)
		{
		  return tad_watcher_device_is_proximity (self, event->deviceid) ?
		    TAD_EVENT_PEN_IN :
			TAD_EVENT_PEN_OUT;
		}
	  break;


	case XI_RawTouchBegin:
      self->touch_count++;
	  return (self->touch_count == 1) ? TAD_EVENT_TOUCH_IN : TAD_EVENT_INVAILD;

	case XI_RawTouchEnd:
      self->touch_count--;
	  return (self->touch_count == 0) ? TAD_EVENT_TOUCH_OUT : TAD_EVENT_INVAILD;
	}
  return TAD_EVENT_INVAILD;
}

/* **** Object Constructors ***************************************************/

TadWatcher*
tad_watcher_new (void)
{
  return (TadWatcher*) g_object_new (TAD_TYPE_WATCHER, NULL);
}





/* **** Property Getter/Setters ***********************************************/




gchar**
tad_watcher_get_watch (TadWatcher *self)
{
  return self->watch;
}


void
tad_watcher_set_watch (TadWatcher  *self,
                     gchar      **watch)
{
  self->watch = g_strdupv (watch);

  if (self->source != NULL)
	{
	  g_message (_("Updating Event Masks."));
      tad_watcher_update_all_masks (self);
	}

  g_object_notify_by_pspec ((GObject*)self, pspecs[PROP_WATCH]);
}




gchar**
tad_watcher_get_control (TadWatcher *self)
{
  return self->control;
}


void
tad_watcher_set_control (TadWatcher  *self,
                         gchar      **control)
{
  self->control = g_strdupv (control);

  if (self->source != NULL)
	{
	  g_message (_("Updating Event Masks."));
      tad_watcher_update_all_masks (self);
	}

  g_object_notify_by_pspec ((GObject*)self, pspecs[PROP_CONTROL]);
}



gboolean
tad_watcher_get_control_enabled (TadWatcher *self)
{
  return self->control_enabled;
}


void
tad_watcher_set_control_enabled (TadWatcher *self,
                                 gboolean    enabled)
{
  guint i;

  self->control_enabled = enabled;

  for (i = self->nwatch; i < self->masks->len; i++)
	tad_watcher_mask_set_enabled (self,
							      & g_array_index (self->masks, XIEventMask, i),
								  enabled);

  g_object_notify_by_pspec ((GObject*)self, pspecs[PROP_CONTROL_ENABLED]);

}


guint
tad_watcher_start_watch (TadWatcher *self)
{
  if (self->source != NULL)
	return g_source_get_id (self->source);

  g_message (_("Start Watching."));
  tad_watcher_update_all_masks (self);

  self->source = tad_watch_source_new (self->display);
  g_source_set_callback(self->source, tad_watcher_watch_func, self, NULL);
  return g_source_attach (self->source, NULL);
}




void
tad_watcher_control_set_enabled (TadWatcher *self,
                                 gboolean    enabled)
{
  guint i;

  for (i = self->nwatch; i < self->masks->len; i++)
	{
	  tad_watcher_mask_set_enabled (self,
									& g_array_index (self->masks, XIEventMask, i),
							        enabled);
	}
}






/* **** Source Functions ******************************************************/

static GSource*
tad_watch_source_new (Display *display)
{
  static GSourceFuncs funcs = {
	tad_watch_prepare,
	tad_watch_check,
	tad_watch_dispatch,
	NULL
  };

  GSource *result = g_source_new (&funcs, sizeof (TadWatchSource));

  ((TadWatchSource*)result)->display = display;

  g_source_add_unix_fd (result, ConnectionNumber (display), G_IO_IN);

  return result;
}



static XEvent*
tad_watch_source_get_event (GSource *source)
{
  return & ((TadWatchSource*)source)->event;
}



static gboolean
tad_watch_prepare (GSource *source,
                   gint    *timeout)
{
  TadWatchSource *self = (TadWatchSource*)source;

  *timeout = -1;
  return XPending (self->display);
}



static gboolean
tad_watch_check (GSource *source)
{
  TadWatchSource *self = (TadWatchSource*)source;

  return XPending (self->display);
}


static gboolean
tad_watch_dispatch (GSource     *source,
                    GSourceFunc  callback,
                    gpointer     userdata)
{
  TadWatchSource *self = (TadWatchSource*)source;
  while (XPending (self->display))
	{
      XNextEvent (self->display, & self->event);

	  if (! callback (userdata))
		return FALSE;
	}
  return TRUE;
}
