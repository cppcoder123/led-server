/*
 *
 *
 *
 */
#ifndef PARSE_H
#define PARSE_H

#include "matrix.h"

void parse_init ();

volatile struct matrix_t* parse_get_matrix ();

void parse ();

#endif
