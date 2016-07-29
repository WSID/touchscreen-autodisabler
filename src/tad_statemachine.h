/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_statemachine.h
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
#ifndef __TAD_STATE_MACHINE
#define __TAD_STATE_MACHINE

#include <glib.h>
#include <glib-object.h>



#define TAD_TYPE_STATE_MACHINE (tad_state_machine_get_type ())
G_DECLARE_FINAL_TYPE (TadStateMachine, tad_state_machine, TAD, STATE_MACHINE,
                      GObject);



TadStateMachine *tad_state_machine_new (void);


TadTransType  tad_state_machine_get_transition (TadStateMachine *machine);

void          tad_state_machine_set_transition (TadStateMachine *machine,
                                                TadTransType     transition);


TadState      tad_state_machine_get_state (TadStateMachine *machine);


gboolean      tad_state_machine_get_control_enabled (TadStateMachine *machine);


void          tad_state_machine_reset_state (TadStateMachine *machine);


void          tad_state_machine_transit (TadStateMachine *machine,
                                         TadEvent         stimulus);

#endif  //__TAD_STATE_MACHINE
