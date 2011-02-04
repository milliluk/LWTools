/*
lwbasic.h

Copyright © 2011 William Astle

This file is part of LWTOOLS.

LWTOOLS is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
*/

/*
definitions used throughout lwbasic
*/

#ifndef __lwbasic_h_seen__
#define __lwbasic_h_seen__

#include <stdint.h>

/* note: integer and uinteger will be the same for positive values from 0
through 0x7FFFFFFF; the unsigned type should be used for doing ascii
conversions and then if a negative value was discovered, it should be
negated IFF it is in range. */

union lexer_numbers
{
	uint32_t uinteger;
	int32_t integer;
};

typedef struct
{
	char *output_file;
	char *input_file;
	
	int debug_level;

	char *lexer_token_string;
	union lexer_numbers lexer_token_number;
	int lexer_token;
	int lexer_curchar;
	int lexer_ignorechar;
	
	int parser_state;
	
	void *input_state;
	
	char *currentsub;
} cstate;

/* parser states */
enum
{
	parser_state_global = 0,			/* only global decls allowed */
	parser_state_error
};

/* token types */
enum
{
	token_kw_sub,				/* SUB keyword */
	token_kw_function,			/* FUNCTION keyword */
	token_kw_as,				/* AS keyword */
	token_kw_public,			/* PUBLIC keyword */
	token_kw_private,			/* PRIVATE keyword */
	token_kw_params,			/* PARAMS keyword */
	token_kw_returns,			/* RETURNS keyword */
	token_kw_integer,			/* INTEGER keyword */
	token_kw_endsub,			/* ENDSUB keyword */
	token_kw_endfunction,		/* ENDFUNCTION keyword */
	token_identifier,			/* an identifier (variable, function, etc. */
	token_char,					/* single character; fallback */
	token_uint,					/* unsigned integer up to 32 bits */
	token_int,					/* signed integer up to 32 bits */
	token_eol,					/* end of line */
	token_eof					/* end of file */
};

#ifndef __input_c_seen__
extern int input_getchar(cstate *state);
#endif

#ifndef __main_c_seen__
extern void lwb_error(const char *fmt, ...);
#endif

#ifndef __lexer_c_seen__
extern void lexer(cstate *state);
extern char *lexer_return_token(cstate *state);
#endif

#ifndef __emit_c_seen__
extern void emit_prolog(cstate *state, int vis, int framesize);
extern void emit_epilog(cstate *state, int framesize);
#endif


#endif /* __lwbasic_h_seen__ */