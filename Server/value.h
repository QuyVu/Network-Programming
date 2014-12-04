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
#ifndef	VALUE_H
#define	VALUE_H

/* The Jval -- a type that can hold any 8-byte type */

typedef union {
    int i;
    long l;
    float f;
    double d;
    void *v;
    char *s;
    char c;
    unsigned char uc;
    short sh;
    unsigned short ush;
    unsigned int ui;
    int iarray[2];
    float farray[2];
    char carray[8];
    unsigned char ucarray[8];
} Value;

Value new_val_i(int);
Value new_val_l(long);
Value new_val_f(float);
Value new_val_d(double);
Value new_val_v(/* void */);
Value new_val_s(char *);
Value new_val_c(char);
Value new_val_uc(unsigned char);
Value new_val_sh(short);
Value new_val_ush(unsigned short);
Value new_val_ui(unsigned int);
Value new_val_iarray(int, int);
Value new_val_farray(float, float);
Value new_val_carray_nt(char *); /* Carray is null terminated */
Value new_val_carray_nnt(char *); /* Carray is not null terminated */
/* For ucarray -- use carray, because it uses memcpy */

Value VNULL;

int val_i(Value);
long val_l(Value);
float val_f(Value);
double val_d(Value);
void *val_v(Value);
char *val_s(Value);
char val_c(Value);
unsigned char val_uc(Value);
short val_sh(Value);
unsigned short val_ush(Value);
unsigned int val_ui(Value);

/* @Deprecated
int *val_iarray(Value);
float *val_farray(Value);
char *val_carray(Value);
 */

#endif