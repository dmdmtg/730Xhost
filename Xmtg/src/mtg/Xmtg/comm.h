/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


#define XMTG_VERSION	1
#define XMTG_VERSION_STRING	"0.1"

#define _SYSFWRITE	3
#define _SYSTAG		0375
#define _SYSINIT	0374
#define _SYSSYNC	0373
#define _SYSFOPEN	1
#define _SYSFCLOSE	2
#define _SYSACCESS	8
#define _SYSFREAD	5
#define _SYSPOPEN	6
#define _SYSPCLOSE	7
#define _SYSEXIT	4
#define _GETENV		9
#define	_OPENSYS	10
#define	_READSYS	11
#define	_CLOSESYS	12
#define _WRITESYS	13	
#define	_OPENDIR	14
#define	_READDIR	15		
#define	_CLOSEDIR	16
#define _LSEEK		17
#define _ARGS		18
#define _VERSION	19
#define _STOP		20
#define _HELP		21



/* byte swap a long */
#define swapl(x, n) n = ((char *) (x))[0];\
		 ((char *) (x))[0] = ((char *) (x))[3];\
		 ((char *) (x))[3] = n;\
		 n = ((char *) (x))[1];\
		 ((char *) (x))[1] = ((char *) (x))[2];\
		 ((char *) (x))[2] = n;

/* byte swap a short */
#define swaps(x, n) n = ((char *) (x))[0];\
		 ((char *) (x))[0] = ((char *) (x))[1];\
		 ((char *) (x))[1] = n



#define	MAXTRIES	3	/* number of times to try XOpenDisplay */
#define WAITTIME	1	/* wait time between tries */
