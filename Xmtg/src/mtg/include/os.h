/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	NOIDENT
#ident	"@(#)server:include/os.h	1.2"
#endif
/*
 os.h (C header file)
	Acc: 575775772 Wed Mar 30 20:42:52 1988
	Mod: 574479469 Tue Mar 15 20:37:49 1988
	Sta: 575775832 Wed Mar 30 20:43:52 1988
	Owner: 5815
	Group: 1985
	Permissions: 664
*/
/*
	START USER STAMP AREA
*/
/*
	END USER STAMP AREA
*/
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: os.h,v 1.23 88/01/04 08:34:02 rws Exp $ */

#ifndef OS_H
#define OS_H
#include "misc.h"

#define NullFID ((FID) 0)

#define SCREEN_SAVER_ON   0
#define SCREEN_SAVER_OFF  1
#define SCREEN_SAVER_FORCER 2

#define MAX_REQUEST_SIZE 16384

typedef pointer	FID;
typedef struct _FontPathRec *FontPathPtr;
typedef struct _NewClientRec *NewClientPtr;

/*
 * os-dependent definition of local allocation and deallocation
 * If you need something other than malloc/free for ALLOCATE/DEALLOCATE
 * LOCAL then you add that to the beginning of this set.  Note that
 * some machines do not return a valid pointer for malloc(0), in
 * which case we provide an alternate under the control of the
 * define MALLOC_0_RETURNS_NULL.  This is necessary because some
 * server code expects malloc(0) to return a valid pointer to storage.
 */
#if defined(ibm032) && !defined(_pcc_)

char *alloca();

#define ALLOCATE_LOCAL(size) alloca((int)(size))
#define DEALLOCATE_LOCAL(ptr)
pragma on(alloca);

#else
# ifdef DMD630

extern char *allocz();
#define ALLOCATE_LOCAL(size) (allocz((unsigned long)(size)))
#define DEALLOCATE_LOCAL(ptr)

#else
# ifdef i386

extern char *allocz();
/* NOTE: allocz() is like alloca(), in that it allocates local storage
 *	 on the stack, but it also null-fills it.  This addition is
 *	 required in our current server implementation.	 alloca() is
 *	 much faster than malloc(), and allocz() is faster still,
 *	 but you could get away with defining the macro to be alloca()
 *	 followed by a memset().  (Just don't make it a procedure!)
 */

#define ALLOCATE_LOCAL(size) (allocz((unsigned long)(size)))
#define DEALLOCATE_LOCAL(ptr)

#else /* everyone else */

char *malloc();

# ifdef MALLOC_0_RETURNS_NULL
# define ALLOCATE_LOCAL(size) malloc((unsigned)((size) > 0 ? (size) : 1))
# else
# define ALLOCATE_LOCAL(size) malloc((unsigned)(size))
# endif

#define DEALLOCATE_LOCAL(ptr) free((char *)(ptr))

#  endif
# endif
#endif

#define xalloc(size) Xalloc((unsigned long)(size))
#define xrealloc(ptr, size) Xrealloc((pointer)(ptr), (unsigned long)(size))
#define xfree(ptr) Xfree((pointer)(ptr))

#ifdef DMD630
#define bzero(b,length)            lmemset(b,(char)0,(long)(length))
#define memset(b,fillchar,length)  lmemset(b,fillchar,(long)(length))
#define bcopy(src,dst,length)      lmemcpy(dst,src,(long)(length))
#define memcpy(dst,src,length)     lmemcpy(dst,src,(long)(length))
#endif

char *ReadRequestFromClient();   /* should be xReq but then include Xproto.h */
Bool CloseDownConnection();
void CreateWellKnownSockets();
void SetDefaultFontPath();
void FreeFontRecord();
void SetFontPath();
FontPathPtr GetFontPath();
FontPathPtr ExpandFontNamePattern();
FID FiOpenForRead();
void AbortServer();
void ErrorF();
unsigned long *Xalloc();
unsigned long *Xrealloc();
void Xfree();
char *lmemset() ;
char *lmemcpy() ;

char *strcat();
char *strncat();
char *strcpy();
char *strncpy();

#endif /* OS_H */
