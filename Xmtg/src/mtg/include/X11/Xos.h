/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	NOIDENT
#ident	"@(#)include:Xos.h	1.1"
#endif
/*
 Xos.h (C header file)
	Acc: 575569768 Mon Mar 28 11:29:28 1988
	Mod: 572844818 Thu Feb 25 22:33:38 1988
	Sta: 575585906 Mon Mar 28 15:58:26 1988
	Owner: 2011
	Group: 1985
	Permissions: 444
*/
/*
	START USER STAMP AREA
*/
/*
	END USER STAMP AREA
*/
/* $Header: Xos.h,v 1.3 87/12/16 16:45:16 rws Exp $ */
/* 
 * Copyright 1987 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The X Window System is a Trademark of MIT.
 *
 */

/* This is a collection of things to try and minimize system dependencies
 * in a "signficant" number of source files.
 */

#ifndef _XOS_H_
#define _XOS_H_

#include <sys/types.h>

#ifdef SYSV
#include <string.h>
#define index strchr
#define rindex strrchr
#else /* SYSV */
#include <strings.h>
#endif /* SYSV */

#ifdef SYSV
#include <fcntl.h>
#endif /* SYSV */
#include <sys/file.h>

#ifdef SYSV
#include <time.h>
#else /* SYSV */
#include <sys/time.h>
#endif /* SYSV */

#ifdef SYSV
#define SIGCHLD SIGCLD
#endif /* SYSV */

#ifdef hpux
#define sigvec sigvector
#endif /* hpux */

#endif /* _XOS_H_ */
