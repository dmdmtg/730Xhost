/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef	NOIDENT
#ident	"@(#)BSD:lib/time.h	1.1"
#endif
/*
 time.h (C header file)
	Acc: 575558273 Mon Mar 28 08:17:53 1988
	Mod: 573061455 Sun Feb 28 10:44:15 1988
	Sta: 573831507 Tue Mar  8 08:38:27 1988
	Owner: 2011
	Group: 1985
	Permissions: 664
*/
/*
	START USER STAMP AREA
*/
/*
	END USER STAMP AREA
*/

#ifndef  _TIME_H 
#define _TIME_H
#include "time.h"

struct timeval {
	long	tv_sec;
	long	tv_usec;
};

struct itimerval {
	struct timeval it_interval;
	struct timeval it_value;
};

struct Timezone {
	int	tz_minuteswest;
	int	tz_dsttime;
};

#endif
