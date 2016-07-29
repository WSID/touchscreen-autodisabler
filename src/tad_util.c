/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tad_util.c
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

#include <stdlib.h>
#include <string.h>

#include "tad_app.h"
#include "tad_util.h"


/*
 * tad_str_is_integer:
 * @str:			string to check.
 * @result: (out):  the integer result, if @str is integer.
 *
 * Checks that @str is integer.
 *
 * Returns: whether @str is integer.
 */
gboolean
tad_str_is_integer (const char* str,
                    int*		result)
{
	char* str_pos;
	*result = (int) strtol(str, &str_pos, 10);
	return (*str_pos == '\0');
}

/*
 * tad_str_is_bool:
 * @str:			string to check.
 * @result: (out):  the bool result, if @str means logical value.
 *
 * Checks that @str is boolean.
 * It means if @str means following, (case insensitive) it returns true.
 * - y			n
 * - yes		no
 * - t			f
 * - true		false
 * - (number)   0
 *
 * And also returns the value of @str into @result.
 *
 * Returns: whether @str is boolean.
 */
gboolean
tad_str_is_bool	(const char*	str,
                 gboolean*		result)
{
	int		int_value;
	char*   str_l;
	char*   str_d;

	bool	is_bool;

	str_l = (char*) malloc (strlen(str) + 1);

	sscanf (str, "%s", str_l);
	str_d = g_ascii_strdown (str_l, -1);

	is_bool = true;
	if ((strcmp(str_l, "y") == 0) ||		(strcmp(str_l, "yes") == 0) ||
		(strcmp(str_l, "t") == 0) ||		(strcmp(str_l, "true") == 0))
		*result = true;
	else if ((strcmp(str_l, "n") == 0) ||   (strcmp(str_l, "no") == 0) ||
		(strcmp(str_l, "f") == 0) ||		(strcmp(str_l, "false") == 0))
		*result = false;
	else if (tad_str_is_integer (str_l, &int_value))
		*result = (int_value != 0);
	else is_bool = false;

	free (str_d);
	free (str_l);
	return is_bool;
}
