/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_util.h
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

#ifndef __TAD_UTIL_HEADER
#define __TAD_UTIL_HEADER

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#define BSTR(b)			((b) ? "true " : "false")


gboolean	tad_str_is_integer			(const char*	str,
					                     int*			result);

gboolean	tad_str_is_bool             (const char*	str,
						                 gboolean*	    result);

#endif  //TAD_UTIL_HEADER
