/* MVPG utils

Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "include.h"


/* DEBUG */
void _debugAssert(const char *file, const unsigned long int linenum, const char *func, const char *expr, const char *msg) {

  char *format;

  format = msg && *msg ? "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed <err: \'%s\'>.\n" : "MVPG DEBUG: %s:%lu %s: Assertion \'%s\' Failed.%s\n";
  fprintf(stderr, format, file, linenum, func, expr, msg && *msg ? msg : "");
  abort();
}
