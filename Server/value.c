/*
Libraries for fields, doubly-linked lists and red-black trees.
Copyright (C) 2001 James S. Plank

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

---------------------------------------------------------------------------
Please see http://www.cs.utk.edu/~plank/plank/classes/cs360/360/notes/Libfdr/
for instruction on how to use this library.

Jim Plank
plank@cs.utk.edu
http://www.cs.utk.edu/~plank

Associate Professor
Department of Computer Science
University of Tennessee
203 Claxton Complex
1122 Volunteer Blvd.
Knoxville, TN 37996-3450

     865-974-4397
Fax: 865-974-4404
 */
#include <stdio.h>
#include <string.h>
#include "value.h"

Value VNULL;

Value new_val_i(int i) {
    Value val;
    val.i = i;
    return val;
}

Value new_val_l(long l) {
    Value val;
    val.l = l;
    return val;
}

Value new_val_f(float f) {
    Value val;
    val.f = f;
    return val;
}

Value new_val_d(double d) {
    Value val;
    val.d = d;
    return val;
}

Value new_val_v(void *v) {
    Value val;
    val.v = v;
    return val;
}

Value new_val_s(char *s) {
    Value val;
    val.s = s;
    return val;
}

Value new_val_c(char c) {
    Value val;
    val.c = c;
    return val;
}

Value new_val_uc(unsigned char uc) {
    Value val;
    val.uc = uc;
    return val;
}

Value new_val_sh(short sh) {
    Value val;
    val.sh = sh;
    return val;
}

Value new_val_ush(unsigned short ush) {
    Value val;
    val.ush = ush;
    return val;
}

Value new_val_ui(unsigned int i) {
    Value val;
    val.i = i;
    return val;
}

Value new_val_iarray(int i0, int i1) {
    Value val;
    val.iarray[0] = i0;
    val.iarray[1] = i1;
    return val;
}

Value new_val_farray(float f0, float f1) {
    Value val;
    val.farray[0] = f0;
    val.farray[1] = f1;
    return val;
}

Value new_val_carray_nt(char *carray) {
    Value val;
    int i;

    for (i = 0; i < 8 && carray[i] != '\0'; i++) {
        val.carray[i] = carray[i];
    }
    if (i < 8) val.carray[i] = carray[i];
    return val;
}

Value new_val_carray_nnt(char *carray) {
    Value val;

    memcpy(val.carray, carray, 8);
    return val;
}

int val_i(Value val) {
    return val.i;
}

long val_l(Value val) {
    return val.l;
}

float val_f(Value val) {
    return val.f;
}

double val_d(Value val) {
    return val.d;
}

void *val_v(Value val) {
    return val.v;
}

char *val_s(Value val) {
    return val.s;
}

char val_c(Value val) {
    return val.c;
}

unsigned char val_uc(Value val) {
    return val.uc;
}

short val_sh(Value val) {
    return val.sh;
}

unsigned short val_ush(Value val) {
    return val.ush;
}

unsigned int val_ui(Value val) {
    return val.ui;
}

/* @Deprecated
int *val_iarray(Value val) {
    return val.iarray;
}

float *val_farray(Value val) {
    return val.farray;
}

char *val_carray(Value val) {
    return val.carray;
}
*/