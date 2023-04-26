/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#include "cf-parser.h"

void cf_adderror(struct cf_parser *p, const char *error, int level,
		 const char *val1, const char *val2, const char *val3)
{
	uint32_t row, col;
	lexer_getstroffset(&p->cur_token->lex->base_lexer,
			   p->cur_token->unmerged_str.array, &row, &col);

	if (!val1 && !val2 && !val3) {
		error_data_add(&p->error_list, p->cur_token->lex->file, row,
			       col, error, level);
	} else {
		struct dstr formatted;
		dstr_init(&formatted);
		dstr_safe_printf(&formatted, error, val1, val2, val3, NULL);

		error_data_add(&p->error_list, p->cur_token->lex->file, row,
			       col, formatted.array, level);

		dstr_free(&formatted);
	}
}

bool cf_pass_pair(struct cf_parser *p, char in, char out)
{
	if (p->cur_token->type != CFTOKEN_OTHER ||
	    *p->cur_token->str.array != in)
		return p->cur_token->type != CFTOKEN_NONE;

	p->cur_token++;

	while (p->cur_token->type != CFTOKEN_NONE) {
		if (*p->cur_token->str.array == in) {
			if (!cf_pass_pair(p, in, out))
				break;
			continue;

		} else if (*p->cur_token->str.array == out) {
			p->cur_token++;
			return true;
		}

		p->cur_token++;
	}

	return false;
}
