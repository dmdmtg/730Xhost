/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	NOIDENT
#ident	"@(#)server:include/site.h	1.2"
#endif
/*
 site.h (C header file)
	Acc: 575775773 Wed Mar 30 20:42:53 1988
	Mod: 574548409 Wed Mar 16 15:46:49 1988
	Sta: 575775834 Wed Mar 30 20:43:54 1988
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
/************************************************************
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

********************************************************/

#ifndef SITE_H
#define SITE_H
/*
 * Site.h contains default values of all variables which can be initialized
 * on the server command line or in the environment, and only those variables
 */

/* NOTE:
 * The following macros are also defined in X.h
 * X.h was not included as it clashes with font.h ("Font" is defined
 * differently in font.h and X.h), which is also
 * included along with this file in the xadmin options.c file.
 */
#define DontPreferBlanking	0
#define PreferBlanking		1
#define DefaultBlanking		2

#define DontAllowExposures	0
#define AllowExposures		1
#define DefaultExposures	2


#ifndef COMPILEDDEFAULTFONTPATH
#define COMPILEDDEFAULTFONTPATH	"/usr/Xmtg/lib/fonts/misc/"
#endif
#define FIRMWARETEXTFONT	"rom11x16"
#define FIRMWARECURSORFONT	"cursor"
#define COMPILEDDEFAULTFONT	FIRMWARETEXTFONT
#define COMPILEDCURSORFONT	FIRMWARECURSORFONT
#ifndef RGB_DB
#define RGB_DB			"/usr/Xmtg/lib/rgb.mtg"
#endif
#define DEFAULT_TIMEOUT		15	/* seconds */
#define DEFAULT_KEYBOARD_CLICK 	0
#define DEFAULT_BELL		50
#define DEFAULT_BELL_PITCH	400
#define DEFAULT_BELL_DURATION	100
#define DEFAULT_AUTOREPEAT	FALSE
#define DEFAULT_AUTOREPEATS	{\
	0, 0, 0, 0, 0, 0, 0, 0,\
	0, 0, 0, 0, 0, 0, 0, 0,\
	0, 0, 0, 0, 0, 0, 0, 0,\
	0, 0, 0, 0, 0, 0, 0, 0 }
#define DEFAULT_LEDS		0x0        /* all off */
#define DEFAULT_TIME_BEFORE_REPEAT	31  /* 60ths of a second */
#define DEFAULT_TIME_BETWEEN_REPEATS	4   /* 60ths of a second */

#define DEFAULT_PTR_NUMERATOR	2
#define DEFAULT_PTR_DENOMINATOR	1
#define DEFAULT_PTR_THRESHOLD	4

#define DEFAULT_SCREEN_SAVER_TIME (10L * (60L * (1000L)))
#define DEFAULT_SCREEN_SAVER_INTERVAL (10L * (60L * (1000L)))
#define DEFAULT_SCREEN_SAVER_BLANKING DontPreferBlanking
#define DEFAULT_SCREEN_SAVER_EXPOSURES AllowExposures
#ifndef NOLOGOHACK
#define DEFAULT_LOGO_SCREEN_SAVER 1
#endif

#endif /* SITE_H */
