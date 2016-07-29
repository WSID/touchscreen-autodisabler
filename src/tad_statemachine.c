/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_statemachine.c
 * Copyright (C) 2014 WSID <jongsome@gmail.com>
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
#include <glib.h>
#include <glib-object.h>

#include "tad_def.h"
#include "tad_enumtypes.h"
#include "tad_statemachine.h"

/* **** State Mappings ********************************************************/

/*
 * trans_enable:
 *
 * Touchscreen enabled state for each state of this application.
 * %true means touchscreen is enabled, and %false means touchscreen disabled.
 */
static const gboolean trans_control_enable[TAD_STATE_COUNT] = {
	TRUE,   //TAD_STATE_ZERO
	FALSE,  //TAD_STATE_PEN
	FALSE,  //TAD_STATE_REST_PEN
	FALSE,  //TAD_STATE_REST_STILL
	TRUE	//TAD_STATE_TOUCHING
};

/*
 * trans_maps:
 *
 * An array of transition maps (which is [#TadState][#TadEvent] -> #TadState)
 */
static const TadState
trans_maps[TAD_TRANS_COUNT][TAD_STATE_COUNT][TAD_EVENT_COUNT] = {
	{   //TAD_TRANS_ZERO
		{   //TAD_STATE_ZERO
			TAD_STATE_PEN,
			-1,
			TAD_STATE_ZERO,
			TAD_STATE_ZERO
		},
		{   //TAD_STATE_PEN
			-1,
			TAD_STATE_ZERO,
			TAD_STATE_PEN,
			TAD_STATE_PEN,
		}
	},
	{   //TAD_TRANS_CLEAR_TO_ENABLE
		{   //TAD_STATE_ZERO
			TAD_STATE_PEN,
			-1,
			TAD_STATE_TOUCHING,
			-1,
		},
		{   //TAD_STATE_PEN
			-1,
			TAD_STATE_ZERO,
			TAD_STATE_REST_PEN,
			-1,
		},
		{   //TAD_STATE_REST_PEN
			-1,
			TAD_STATE_REST_STILL,
			-1,
			TAD_STATE_PEN,
		},
		{   //TAD_STATE_REST_STILL
			TAD_STATE_REST_PEN,
			-1,
			-1,
			TAD_STATE_ZERO,
		},
		{   //TAD_STATE_TOUCHING
			TAD_STATE_REST_PEN,
			-1,
			-1,
			TAD_STATE_ZERO
		}
	}
};


/* **** Properties and signals ************************************************/

static guint SIG_TRANSIT = 0;

enum {
  PROP_0,
  PROP_TRANSITION,
  PROP_STATE,

  PROP_CONTROL_ENABLED,

  PROP_COUNT
};

static GParamSpec *pspecs[PROP_COUNT] = {NULL};

/* **** Class Definitions *****************************************************/

struct _TadStateMachine
{
  GObject _parent;

  TadTransType transition;
  TadTransType transition_use;

  TadState     state;
};

G_DEFINE_TYPE (TadStateMachine, tad_state_machine, G_TYPE_OBJECT)



/* **** GObject Virtual Functions *********************************************/

static void tad_state_machine_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec);

static void tad_state_machine_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec);


/* **** Private Functions *****************************************************/

static void tad_state_machine_set_state (TadStateMachine *object,
                                         TadState         state);



/* **** GTypeInstance Function Implementations ********************************/

static void
tad_state_machine_init (TadStateMachine *self)
{
  self->transition = TAD_TRANS_ZERO;
  self->transition_use = TAD_TRANS_ZERO;
  self->state = TAD_STATE_ZERO;
}

static void
tad_state_machine_class_init (TadStateMachineClass *c)
{
  GObjectClass *c_g_object;

  c_g_object = G_OBJECT_CLASS (c);

  c_g_object->get_property = tad_state_machine_get_property;
  c_g_object->set_property = tad_state_machine_set_property;

  pspecs[PROP_TRANSITION] = g_param_spec_enum (
  "transition", "Transition",
  "Transition type",
  TAD_TYPE_TRANS_TYPE,
  TAD_TRANS_ZERO,
  G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS );

  pspecs[PROP_STATE] = g_param_spec_enum (
  "state", "State",
  "State of state machine",
  TAD_TYPE_STATE,
  TAD_STATE_ZERO,
  G_PARAM_READABLE | G_PARAM_STATIC_STRINGS );

  pspecs[PROP_CONTROL_ENABLED] = g_param_spec_boolean (
  "control-enabled", "Control Enabled",
  "Whether control device should be enabled or not",
  TRUE,
  G_PARAM_READABLE | G_PARAM_STATIC_STRINGS );

  g_object_class_install_properties (c_g_object, PROP_COUNT, pspecs);



  SIG_TRANSIT = g_signal_new ("transit", TAD_TYPE_STATE_MACHINE,
                              G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                              0,
                              NULL, NULL,
                              g_cclosure_marshal_VOID__ENUM,
                              G_TYPE_NONE, 1, TAD_TYPE_TRANS_TYPE);
}



/* **** GObject Virtual Functions Implementations *****************************/

static void
tad_state_machine_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  TadStateMachine *self = (TadStateMachine*) object;

  switch (prop_id)
    {
    case PROP_TRANSITION:
      g_value_set_enum (value, tad_state_machine_get_transition (self));
      break;

    case PROP_STATE:
      g_value_set_enum (value, tad_state_machine_get_state(self));
      break;

    case PROP_CONTROL_ENABLED:
      g_value_set_boolean (value, tad_state_machine_get_control_enabled (self));
     break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
tad_state_machine_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  TadStateMachine *self = (TadStateMachine *)object;

  switch (prop_id)
    {
    case PROP_TRANSITION:
      tad_state_machine_set_transition (self,
                                        g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}



/* **** Private Setter Functions **********************************************/

static void
tad_state_machine_set_state (TadStateMachine *machine,
                             TadState         state)
{
  gboolean prev_enable = trans_control_enable[machine->state];
  gboolean new_enable = trans_control_enable[state];

  machine->state = state;

  if (prev_enable != new_enable)
    g_object_notify_by_pspec ((GObject*)machine, pspecs[PROP_CONTROL_ENABLED]);
}


/* **** Constructors **********************************************************/

TadStateMachine*
tad_state_machine_new (void)
{
  return (TadStateMachine*)
  g_object_new (TAD_TYPE_STATE_MACHINE, NULL);
}





/* **** Property Getter *******************************************************/

TadTransType
tad_state_machine_get_transition (TadStateMachine *machine)
{
  return machine->transition;
}

void
tad_state_machine_set_transition (TadStateMachine *machine,
                                  TadTransType     transition)
{
  machine->transition = transition;
}


TadState
tad_state_machine_get_state (TadStateMachine *machine)
{
  return machine->state;
}


gboolean
tad_state_machine_get_control_enabled (TadStateMachine *machine)
{
  return trans_control_enable[machine->state];
}



/* **** Public Functions ******************************************************/

void
tad_state_machine_reset_state (TadStateMachine *machine)
{
  tad_state_machine_set_state (machine, TAD_STATE_ZERO);
}


void
tad_state_machine_transit (TadStateMachine *machine,
                           TadEvent         stimulus)
{
  TadState  new_state;

  new_state = trans_maps [machine->transition_use][machine->state][stimulus];

  if (new_state == TAD_STATE_ZERO)
	machine->transition_use = machine->transition;

  if (new_state != -1)
	{
      tad_state_machine_set_state (machine, new_state);
      g_signal_emit (machine, SIG_TRANSIT, 0, stimulus);
	}
}
