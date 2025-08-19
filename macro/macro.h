/* Macro Utilities
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef MVPG_MACRO_H
#define MVPG_MACRO_H

/* Ignore gcc/clangs’ '-Wvariadic-macro-arguments-omitted' when passsing no argument to varadic macro (no side effect on the macro and can be ommited) */
#define DD__ IGNORE_STDC90__

/* @CAT: Catenate to tokens */
#define CAT__(a, b) ICAT__(a, b)
#define ICAT__(a, b) a ## b

/* @CH1, CH2: choose argument 1 or 2 */
#define CH1__(...)     ICH1__(__VA_ARGS__)
#define CH2__(...)     ICH2__(__VA_ARGS__)
#define ICH1__(f, ...)    f
#define ICH2__(f, s, ...) s

/* @PAREN: Open a parenthesized, first argument of a macro.
 * The argument is checked for parentheis by @PAREN_CHK, and further checked by @PAREN_CHK2 if found parenthesis is pure (is not ’(’, ’()N’, and so on). Both check returns 0 on finding a paenthesis (pure) and follows @PAREN_FALSE_ ## 0 which opens the parenthesis. Non-parenthesized arguments are returned unchanged.
 */
#define __PAREN_IMPURE_0(...)  0, 0
#define __PAREN_FALSE_0(...) __PAREN_FALSE_1 __VA_ARGS__
#define __PAREN_FALSE_1(...) __VA_ARGS__
#define   PAREN_CHK2__(P) CH2__( CAT__(__PAREN_IMPURE_, P)(DD__), 1, DD__)
#define   PAREN_CHK__(...)  PAREN_CHK2__( CH2__(MAC__ __VA_ARGS__, 1, DD__) )
#define   PAREN__(...) CAT__(__PAREN_FALSE_, PAREN_CHK__( CH1__(__VA_ARGS__, DD__) ))(__VA_ARGS__)

/* @MAC5: Check if a macro is (not) given an argument (see macro.md) */
#define   MAC_(...)   0,
#define   MAC__(...) ,0
#define __MAC0(...) 0, __MAC2( CH1__(__VA_ARGS__, DD__) ), __MAC2
#define __MAC1(...) CH2__(MAC_  __VA_ARGS__( PAREN__(__VA_ARGS__) ), 1, DD__)
#define __MAC2(...) CH2__(MAC__ __VA_ARGS__( PAREN__(__VA_ARGS__) ), 1, DD__)
#define __MAC3(...) __MAC2( __MAC1( __MAC1(__VA_ARGS__) ) )
#define __MAC4(...) CAT__(__MAC, __MAC3(__VA_ARGS__) )
#define __MAC5(...) CH2__( __MAC4(__VA_ARGS__)( CH1__(__VA_ARGS__, DD__) ), 1, DD__ )


/* @MAC_VA_OPT: C++/C23 __VA_OPT__ */
#define __MAC_VA_OPT_0(...)
#define __MAC_VA_OPT_1(...) __VA_ARGS__
#define   MAC_VA_OPT__(...)   CAT__(__MAC_VA_OPT_, __MAC5(__VA_ARGS__) )(__VA_ARGS__)

/* @MAC_SELECT: Selct A if __VA_ARGS__ is non-empty else B. varadic
   arguments to any of A or B must be parenthesized */
#define __MAC_SELECT_0(A, B)    PAREN__(B)
#define __MAC_SELECT_1(A, B)    PAREN__(A)
#define   MAC_SELECT__(A, B, ...) CAT__(__MAC_SELECT_, __MAC5(__VA_ARGS__))(A, B)

#endif
