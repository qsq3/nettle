/* sexp.h
 *
 * Parsing s-expressions.
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2002 Niels M�ller
 *  
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 * 
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */
 
#ifndef NETTLE_SEXP_H_INCLUDED
#define NETTLE_SEXP_H_INCLUDED

#include <inttypes.h>
#include <stdarg.h>

enum sexp_type
  { SEXP_ATOM, SEXP_LIST, SEXP_END };

struct sexp_iterator
{
  unsigned length;
  const uint8_t *buffer;

  /* Points at the start of the current sub expression. */
  unsigned start;
  /* If type is SEXP_LIST, pos points at the start of the current
   * element. Otherwise, it points at the end. */
  unsigned pos;
  unsigned level;

  enum sexp_type type;
  
  unsigned display_length;
  const uint8_t *display;

  unsigned atom_length;
  const uint8_t *atom;
};


/* All these functions return 1 on success, 0 on failure */

/* Initializes the iterator. */
int
sexp_iterator_first(struct sexp_iterator *iterator,
		    unsigned length, const uint8_t *input);

/* NOTE: Decodes the input string in place */
int
sexp_transport_iterator_first(struct sexp_iterator *iterator,
			      unsigned length, uint8_t *input);

int
sexp_iterator_next(struct sexp_iterator *iterator);

/* Current element must be a list. */
int
sexp_iterator_enter_list(struct sexp_iterator *iterator);

/* Skips the rest of the current list */
int
sexp_iterator_exit_list(struct sexp_iterator *iterator);

/* Gets start and length of the current subexpression. Implies
 * sexp_iterator_next. */
const uint8_t *
sexp_iterator_subexpr(struct sexp_iterator *iterator,
		      unsigned *length);

int
sexp_iterator_get_uint32(struct sexp_iterator *iterator,
			 uint32_t *x);


/* Checks the type of the current expression, which should be a list
 *
 *  (<type> ...)
 */
int
sexp_iterator_check_type(struct sexp_iterator *iterator,
			 const uint8_t *type);

const uint8_t *
sexp_iterator_check_types(struct sexp_iterator *iterator,
			  unsigned ntypes,
			  const uint8_t **types);

/* Current element must be a list. Looks up element of type
 *
 *   (key rest...)
 *
 * For a matching key, the corresponding iterator is initialized
 * pointing at the start of REST.
 *
 * On success, exits the current list.
 */
int
sexp_iterator_assoc(struct sexp_iterator *iterator,
		    unsigned nkeys,
		    const uint8_t **keys,
		    struct sexp_iterator *values);


/* Output functions. What is a reasonable API for this? It seems
 * ugly to have to reimplement string streams. */

/* Declared for real in buffer.h */
struct nettle_buffer;

/* Returns the number of output characters, or 0 on out of memory. If
 * buffer == NULL, just compute length.
 *
 * Format strings can contained matched parentheses, and the following
 * formatting specifiers:
 *
 *   %z   NUL-terminated string, const uint8_t *.
 *
 *   %s   String represented as unsigned length, const uint8_t *data.
 *
 *   %i   Non-negative small integer, uint32_t.
 *
 *   %b   Non-negative bignum, mpz_t.
 *
 *   %l   Literal string (no length added), typically a balanced
 *        subexpression. Represented as unsigned length, const uint8_t
 *        *data.
 */

unsigned
sexp_format(struct nettle_buffer *buffer,
	    const char *format, ...);

unsigned
sexp_vformat(struct nettle_buffer *buffer,
	     const char *format, va_list args);

/* FIXME: Add argument LINE_WIDTH. If non-zero, break lines to at most
 * that width. */

unsigned
sexp_transport_format(struct nettle_buffer *buffer,
		      const char *format, ...);

unsigned
sexp_transport_vformat(struct nettle_buffer *buffer,
		       const char *format, va_list args);

#endif /* NETTLE_SEXP_H_INCLUDED */
