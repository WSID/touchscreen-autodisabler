/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_app.c
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

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include "config.h"
#include "tad_def.h"
#include "tad_device.h"
#include "tad_util.h"
#include "tad_statemachine.h"
#include "tad_app.h"


static GDBusNodeInfo *tad_dbus_info;
static GDBusInterfaceInfo *tad_dbus_interface_info;

/* Application Class Definition ***********************************************/
typedef struct _TadApplication {
  GApplication _parent;
  guint        dbus_iface_ticket;

  TadStateMachine *machine;
  TadWatcher  *watcher;

  GSettings *settings;

} TadApplication;

G_DEFINE_TYPE (TadApplication, tad_application, G_TYPE_APPLICATION)


/* **** GObject Virtual Functions *********************************************/

static void tad_application_constructed (GObject *object);



/* **** GApplication Virtual Functions ****************************************/

static void tad_application_shutdown (GApplication *application);

static void tad_application_startup (GApplication *application);

static void tad_application_activate (GApplication *application);

static gboolean tad_application_dbus_register (GApplication     *application,
                                               GDBusConnection  *connection,
                                               const gchar      *object_path,
                                               GError          **error);

static void tad_application_dbus_unregister (GApplication    *application,
                                             GDBusConnection *connection,
                                             const gchar     *object_path);


/* **** DBus Object Functions *************************************************/
static GVariant *tad_dbus_get_property (GDBusConnection  *connection,
                                        const gchar      *sender,
                                        const gchar      *object_path,
                                        const gchar      *interface_name,
                                        const gchar      *property_name,
                                        GError          **error,
                                        gpointer          userdata);

static void tad_dbus_method_call (GDBusConnection       *connection,
                                  const gchar           *sender,
                                  const gchar           *object_path,
                                  const gchar           *interface_name,
                                  const gchar           *method_name,
                                  GVariant              *parameters,
                                  GDBusMethodInvocation *invocation,
                                  gpointer               userdata);


static void tad_dbus_transit (TadStateMachine *machine,
                              TadEvent         stimulus,
                              gpointer         userdata);


/* **** Command line functions ************************************************/

static gboolean tad_application_version (const gchar  *option,
                                         const gchar  *value,
                                         gpointer      data,
                                         GError      **error);




/* **** constants *************************************************************/
static GOptionEntry entries[] = {
	{ "version", 'v',
	  G_OPTION_FLAG_NO_ARG,
	  G_OPTION_ARG_CALLBACK, tad_application_version,
	  N_("Print version information"), NULL},
	{NULL}
};





/* **** GTypeInstance Functions Implementations *******************************/

static void
tad_application_init (TadApplication *inst)
{
}

static void
tad_application_class_init (TadApplicationClass *c)
{
  GObjectClass *c_g_object;
  GApplicationClass *c_g_application;

  GBytes *dbus_info_bytes;

  c_g_object = G_OBJECT_CLASS (c);

  c_g_object->constructed = tad_application_constructed;


  c_g_application = G_APPLICATION_CLASS (c);

  c_g_application->startup = tad_application_startup;
  c_g_application->activate = tad_application_activate;
  c_g_application->shutdown = tad_application_shutdown;

  c_g_application->dbus_register = tad_application_dbus_register;
  c_g_application->dbus_unregister = tad_application_dbus_unregister;


  dbus_info_bytes = g_resources_lookup_data ("/wsid/Tad/wsid.Tad.dbus.xml",
											 G_RESOURCE_LOOKUP_FLAGS_NONE,
											 NULL);

  tad_dbus_info = g_dbus_node_info_new_for_xml (g_bytes_get_data (dbus_info_bytes,
																  NULL),
												NULL);
  tad_dbus_interface_info = g_dbus_node_info_lookup_interface (tad_dbus_info,
														       "wsid.Tad");

}





/* **** GObject Functions Implementations *************************************/

static void
tad_application_constructed (GObject *inst)
{
  GApplication *self_gapplication = (GApplication*) inst;
  TadApplication *self = (TadApplication*) inst;

  G_OBJECT_CLASS (tad_application_parent_class)->constructed (inst);

  g_application_add_main_option_entries (self_gapplication, entries);
}




/* **** GApplication Functions Implementations ********************************/


static void
tad_application_startup (GApplication *application)
{
  TadApplication *self = (TadApplication*) application;

  G_APPLICATION_CLASS (tad_application_parent_class)->startup (application);

  // Keep application running.
  g_application_hold (application);

  self->machine = tad_state_machine_new ();
  self->settings = g_settings_new ("wsid.Tad");
  self->watcher = tad_watcher_new ();

  g_signal_connect_swapped (self->watcher, "event",
							(GCallback) tad_state_machine_transit, self->machine);

  g_signal_connect (self->machine, "transit",
					(GCallback) tad_dbus_transit, self);

  g_object_bind_property (self->machine, "control-enabled",
						  self->watcher, "control-enabled", G_BINDING_DEFAULT);

  g_settings_bind (self->settings, "watch",
				   self->watcher, "watch", G_SETTINGS_BIND_GET);

  g_settings_bind (self->settings, "control",
				   self->watcher, "control", G_SETTINGS_BIND_GET);

  g_settings_bind (self->settings, "transition",
				   self->machine, "transition", G_SETTINGS_BIND_GET);

  tad_watcher_start_watch (self->watcher);
}

static void
tad_application_activate (GApplication *application)
{
  G_APPLICATION_CLASS (tad_application_parent_class)->activate (application);
  // Do nothing!!
  // This is not an application, but actually service!
}

static void
tad_application_shutdown (GApplication *application)
{
  TadApplication *self = (TadApplication*) application;

  g_clear_object (& self->machine);
  g_clear_object (& self->watcher);
  g_clear_object (& self->settings);
  G_APPLICATION_CLASS (tad_application_parent_class)->shutdown (application);
}

static gboolean
tad_application_dbus_register (GApplication     *application,
                               GDBusConnection  *connection,
                               const gchar      *object_path,
                               GError          **error)
{
  static GDBusInterfaceVTable vtable = {
	tad_dbus_method_call,
	tad_dbus_get_property,
	NULL
  };

  TadApplication *self = (TadApplication*) application;

  if (! G_APPLICATION_CLASS(tad_application_parent_class)->
	      dbus_register (application, connection, object_path, error))
	return FALSE;



  self->dbus_iface_ticket =
  g_dbus_connection_register_object (connection,
									 object_path,
									 tad_dbus_interface_info,
									 &vtable,
									 application,
									 NULL,
									 NULL);

  return TRUE;
}


static void
tad_application_dbus_unregister (GApplication    *application,
                                 GDBusConnection *connection,
                                 const gchar     *object_path)
{
  TadApplication *self = (TadApplication*) application;

  g_dbus_connection_unregister_object (connection, self->dbus_iface_ticket);

  G_APPLICATION_CLASS (tad_application_parent_class)->
      dbus_unregister(application, connection, object_path);
}






/* **** DBus Implementations **************************************************/
static GVariant*
tad_dbus_get_property (GDBusConnection  *connection,
                       const gchar      *sender,
                       const gchar      *object_path,
                       const gchar      *interface_name,
                       const gchar      *property_name,
                       GError          **error,
                       gpointer          userdata)
{
  TadApplication *self = (TadApplication*)userdata;

  if (strcmp (property_name, "State") == 0)
    return g_variant_new_uint32 (tad_state_machine_get_state (self->machine));

  else if (strcmp (property_name, "ControlEnabled") == 0)
	return g_variant_new_boolean (tad_state_machine_get_control_enabled (self->machine));

  else return NULL;
}

static void
tad_dbus_method_call (GDBusConnection       *connection,
                      const gchar           *sender,
                      const gchar           *object_path,
                      const gchar           *interface_name,
                      const gchar           *method_name,
                      GVariant              *parameters,
                      GDBusMethodInvocation *invocation,
                      gpointer               userdata)
{
  TadApplication *self = (TadApplication*)userdata;

  if (strcmp (method_name, "ResetState") == 0)
	tad_state_machine_reset_state (self->machine);

  else if (strcmp (method_name, "Quit") == 0)
    g_application_release ((GApplication*) self);

  g_dbus_method_invocation_return_value (invocation, NULL);
}

static void
tad_dbus_transit (TadStateMachine *machine,
                  TadEvent         stimulus,
                  gpointer         userdata)
{
  GApplication *self_g_application = (GApplication*)userdata;
  GDBusConnection *conn;
  GVariant *variant;

  variant = g_variant_new ("(uub)",
						   stimulus,
						   tad_state_machine_get_state (machine),
						   tad_state_machine_get_control_enabled (machine));

  conn = g_application_get_dbus_connection (self_g_application);

  g_dbus_connection_emit_signal (conn, NULL, "/wsid/Tad", "wsid.Tad", "Transit",
								 variant, NULL);
}




/* Command line functions *****************************************************/

static gboolean
tad_application_version (const gchar  *option,
                         const gchar  *value,
                         gpointer      data,
                         GError      **error)
{
  g_print ("touchscreen autodisabler " VERSION
           " - copyright wsid <jongsome@gmail.com>\n");

  exit (0);
  return TRUE;
}


/* Application Public functions ***********************************************/

TadApplication*
tad_application_new (void)
{
  return (TadApplication*) g_object_new (TAD_TYPE_APPLICATION,
										 "application-id", "wsid.Tad",
										 "flags", G_APPLICATION_FLAGS_NONE,
										 NULL);
}

