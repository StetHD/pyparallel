/***********************************************************
Copyright 1991, 1992, 1993 by Stichting Mathematisch Centrum,
Amsterdam, The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.

STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/*
Mapping object type -- mapping from object to object.
These functions set errno for errors.  Functions mappingremove() and
mappinginsert() return nonzero for errors, getmappingsize() returns -1,
the others NULL.  A successful call to mappinginsert() calls INCREF()
for the inserted item.
*/

extern typeobject Mappingtype;

#define is_mappingobject(op) ((op)->ob_type == &Mappingtype)

extern object *newmappingobject PROTO((void));
extern object *mappinglookup PROTO((object *dp, object *key));
extern int mappinginsert PROTO((object *dp, object *key, object *item));
extern int mappingremove PROTO((object *dp, object *key));
extern int getmappingsize PROTO((object *dp));
extern object *getmappingkey PROTO((object *dp, int i));
extern object *getmappingkeys PROTO((object *dp));
