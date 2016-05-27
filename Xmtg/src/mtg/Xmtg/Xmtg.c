/* Copyright (c) 1989 AT&T  */
/* All Rights Reserved      */


/*
** Xmtg - is the file server for the MTG X server. 
** It handles all requests for file I/O from the server.
**
*/


#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>
#include <sys/tiuser.h>
#include <sys/types.h>
#include <dirent.h>
#include <memory.h>
#include <signal.h>
#include "comm.h"

#define TRUE		1
#define FALSE		0
#define NULLP		(char *)0
#define DMDBUFSIZ	1024	
#define DMDSHORT	 2
#define DMDLONG		4
#define MAXNAMEBUF	256
#define DEFMODE		0644
#define MAXFILES  	16	
#define JXBUFSIZ  	DMDBUFSIZ + DMDSHORT + 1
#define IDSTRING        "Xmtg file server 1.1"

typedef union {
	short s;
	char c[2];
	} schar;

typedef union {
	long l;
	char c[4];
	} lchar;



char rcvbuf[JXBUFSIZ];
char sndbuf[JXBUFSIZ];
char *sndptr=sndbuf+DMDSHORT;
static short plen=0;
int fd[MAXFILES];
FILE *files[MAXFILES];
DIR *dp;

extern int t_errno;
FILE *ferr;
char *progname;
int xmtg_fd=0;

long lseek();
char *getenv();
char *malloc();
int shutdown();


main (argc, argv)
int argc;
char *argv[];
{
  	int i;
	
	signal(SIGTERM,shutdown);
	signal(SIGHUP,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	progname = argv[0];
		
	/* we assume t_sync will always succeed, since it is comming from the
	** transport provider. This is because any error messages that
	** we try to print out will go through the network to the X server.
	** We do not want to worry about that.
	*/
	t_sync(xmtg_fd);

	/* send verification string */
        t_snd(xmtg_fd,IDSTRING,strlen(IDSTRING),0);


#ifdef DEBUG
	ferr = fopen("/tmp/Xmtgerr","a");

	files[1] = files[2] = ferr;
#endif

	/* initialize fd array */
	for (i=0; i<MAXFILES; ++i)
	    fd[i] = -1;

#ifdef DEBUG
#ifdef u3b2
	log("u3b2 defined");
#endif
#ifdef i386
	log("i386 defines");
#endif
#endif

	for (;;) {
	    listen();
  	}
}


getlook(xmtg_fd)
int xmtg_fd;
{
	int event;

	if (t_errno == TLOOK) {
		event = t_look(xmtg_fd);
		if (event == T_DISCONNECT) {
#ifdef DEBUG
			log("got a disconnect");
#endif
			exit(0);
		} else if (event == T_ERROR) {
#ifdef DEBUG
			log("get a T_ERROR");
#endif
			exit(1);
		}
	}
	else {
		/* some other rare error occured */
#ifdef DEBUG
		log("get some other error besides T_LOOK");
#endif
		exit(1);
	}

}


/**********************************************************
		PUT ROUTINES
**********************************************************/


jputnchars(n,p)
register int n;
register char*p;
{
#ifdef DEBUG
	log("in jputnchars");
	logint("n",n);
#endif
	
	for (; n > 0 ; n--) {
			
		*sndptr++ = *p++;
		plen++;
		}
#ifdef DEBUG
	logint("plen",plen);
#endif
}


jputchar(c)
char c;
{
#ifdef DEBUG
	log("in jputchar");
#endif
	*sndptr++ = c;
	plen++;
	jflush();
}


jputshort(s)
short s;
{
	schar u;
	register unsigned x;
	short s1;	
	u.s = s;
		/* set schar = to value passed */
#ifdef DEBUG
	logint("s" ,s);
	logint("c0" ,u.c[0]);
	logint("c1" ,u.c[1]);
	logint("jputshort u.s" ,u.s);
#endif
	
#ifdef i386
	jputnchars(1, &u.c[1]);
	jputnchars(1, &u.c[0]);
#endif
#ifdef u3b2
	jputnchars(1, &u.c[0]);
	jputnchars(1, &u.c[1]);
#endif
	jflush();
	
}

jputlong(l)
long l;
{
	lchar u;
	u.l  = l;
#ifdef DEBUG
	loglong("l" ,l);
	logint("c0" ,u.c[0]);
	logint("c1" ,u.c[1]);
	logint("c2" ,u.c[2]);
	logint("c3" ,u.c[3]);
	logint("jputlong u.l" ,u.l);
#endif
#ifdef i386
	jputnchars(1, &u.c[3]);
	jputnchars(1, &u.c[2]);
	jputnchars(1, &u.c[1]);
	jputnchars(1, &u.c[0]);
#endif
#ifdef u3b2
	jputnchars(1, &u.c[0]);
	jputnchars(1, &u.c[1]);
	jputnchars(1, &u.c[2]);
	jputnchars(1, &u.c[3]);
#endif
	jflush();
}



jflush()
{
	int i, flags = 0;
	schar s;
	int result;
		
#ifdef DEBUG
	log("jflush: ");
	logint("plen", plen);
#endif
	sndptr=sndbuf;
	s.s = plen;
	
#ifdef i386
	jputnchars(1, &s.c[1]);
	jputnchars(1, &s.c[0]);
	
#endif
#ifdef u3b2
	jputnchars(1, &s.c[0]);
	jputnchars(1, &s.c[1]);
#endif

	/* insert expected length at the beginning of each msg */
	result = t_snd(xmtg_fd, sndbuf, plen , &flags);
	if (result < 0) getlook(xmtg_fd);
	
#ifdef DEBUG
	logint("t_snd result=",result);
	logint("plen=", plen);
	
#endif
	plen = 0;
}

jputbuf(p,n)
register char *p;
register short n;
{
	
#ifdef DEBUG
	log("in jputbuf");
	logint(" n=",n);
	
#endif

	jputnchars(n,p);
	jflush();
		

}



/**********************************************************
		GET ROUTINES
**********************************************************/


int 
jgetchar()
{
	int flags;
	int result;
		
	result = t_rcv(xmtg_fd, rcvbuf, 1, &flags);
	if (result < 0) getlook(xmtg_fd);
	
	return(rcvbuf[0]);
}

short
jgetshort()
{
	schar	s;
	short s1;
	int flags;
	int result;
	
	
	result =  t_rcv(xmtg_fd, &s, DMDSHORT, &flags);
	if (result < 0) getlook(xmtg_fd);

#ifdef i386
	swaps(&s, s1);
#endif
#ifdef u3b2
#ifdef DEBUG
	logint("s.s",s.s);
#endif
#endif
	return(s.s);
}

long
jgetlong()
{
	lchar	l;
	long l1;
	int flags;
	int result;
	
	
	result =  t_rcv(xmtg_fd, &l, DMDLONG, &flags);
	if (result < 0) getlook(xmtg_fd);

#ifdef i386
	swapl(&l, l1);
#endif
#ifdef u3b2
#ifdef DEBUG
	logint("l.l",l.l);
#endif
#endif

	return(l.l);
}


jgetbuf(p)
register char *p;
{
	register short i,n;

	n = jgetshort();
	for (i = 0; i < n; i++)
		*p++ = jgetchar();
	return(n);
}

jgetstr(s)
register char *s;
{
	
#ifdef DEBUG
	log("jgetstr: ");
#endif
	while (*s++ = jgetchar());
}



/* listen() is the main file server routine which waits for 
** file I/O request from the server and performs the requested
** operation. The result is then sent back to the server.
*/
listen()
{
	int x,n,c,i;
	int oflag;
	long fmode, strtol();
	long offset,lresult;
	char amode[7];
	char filename[50],buf[JXBUFSIZ],mode[2];
    	register struct dirent *nextdent;
	

	mode[1] = '\0';

	while ((c = jgetchar()) != EOF) {
#ifdef DEBUG 
	loghex("c", c);
#endif
	switch (c) {
		case _SYSFOPEN:
			
#ifdef DEBUG
			log("in fopen ");
#endif
			i = jgetshort();
			jgetstr(filename);
			*mode = jgetchar();
#ifdef DEBUG
			logint("file idx",i);
			log("filename=%s\n",filename);
			logchar("mode=",*mode);
#endif
			if ((files[i] = fopen(filename,mode)) == NULL)
				jputshort(-1);
			else
				jputshort(i);
			break;
	
		case _OPENSYS:
#ifdef DEBUG
			log("in open\n ");
#endif
			i = jgetshort();
			jgetstr(filename);
			oflag = jgetshort();
			jgetstr(amode);
			fmode = strtol(amode,(char **)NULL,8);
			
#ifdef DEBUG
			logint("file idx",i);
			logint("oflag",oflag);
			log("filename=%s\n",filename);
#endif
			if (fmode == 0 && (oflag & O_CREAT)) fmode = DEFMODE;
			if (i < 3 || i >=  MAXFILES) {
				jputshort(-1);
				break;
				}
			fd[i] = open(filename,oflag, fmode);
			if (fd[i] == -1)
			    jputshort(-1);
			else
			    jputshort(i);
			break;
	


	
		case _SYSFCLOSE:
#ifdef DEBUG
			log("in fclose\n ");
#endif
			i = jgetshort();
			jputshort(fclose(files[i]));
			files[i] = 0;
			break;

		case _CLOSESYS:
#ifdef DEBUG
			log("in close\n ");
#endif
			i = jgetshort();
			if (fd[i] < 0 || fd[i] >=  MAXFILES) {
				jputshort(-1);
				break;
			}
			jputshort(close(fd[i]));
			fd[i] = -1;
			break;



		case _SYSFWRITE:
#ifdef DEBUG
			log("in fwrite\n ");
#endif
			i = jgetshort();
#ifdef DEBUG
			logint("file idx" , i);
#endif		
			n = jgetbuf(buf);
			jputshort(fwrite(buf,1,n,files[i]));
			fflush(files[i]);
			break;

		case _WRITESYS:

#ifdef DEBUG
			log("in write\n ");
#endif
			i = jgetshort();
			n = jgetbuf(buf);
			if (fd[i] < 0 || fd[i] >=  MAXFILES) {
				jputshort(-1);
				break;
			}
			n= write(fd[i], buf, n);
			jputshort(n);
			break;
		
		case _SYSEXIT:

#ifdef DEBUG
			
			log("in exit\n ");
#endif
			jputshort(0);
			for (i=0;i < MAXFILES; i++)
			{
				if (fd[i] != -1) 
				    close(fd[i]);
				if (files[i]) 
				    fclose(files[i]);
			}
			t_close(xmtg_fd);
			exit(0);
		
		case _SYSFREAD:

#ifdef DEBUG
			
			log("in fread\n");
	
#endif
			i = jgetshort();
			n = jgetshort();
		
#ifdef DEBUG
			logint("fd=",i);
			logint("cnt=",n);	
#endif
			x = fread(buf,1,n,files[i]);
#ifdef DEBUG
			logint("fread result=",x);
#endif
			jputshort(x);
			jputbuf(buf,x);
			break;
		
		case _READSYS:

#ifdef DEBUG
			log("in read\n");
#endif
			i = jgetshort();
			n = jgetshort();
#ifdef DEBUG
			logint("fd=",i);
			logint("fd[i]=",fd[i]);	
			logint("cnt=",n);	
#endif
			if (fd[i] < 0 || fd[i] >=  MAXFILES) {
				jputshort(-1);
				break;
			}
			n = read(fd[i], buf, n);
#ifdef DEBUG
			logint("read result ",n);
#endif
			jputshort(n);
			jputbuf(buf,n);
			break;
		
		case _SYSACCESS:
#ifdef DEBUG
			log("In access\n ");
			log("filename=%s\n",filename);
#endif
			i = jgetshort();
			jgetstr(filename);
			n=access(filename,i);
#ifdef DEBUG
			logint("access mode=",i);
			logint("access result=",n);
#endif
			jputshort(access(filename,i));
			break;

		case _LSEEK:
#ifdef DEBUG
            		log("In lseek\n ");
#endif
			i = jgetshort();
			offset = jgetlong();
			x = jgetshort();
			if (fd[i] < 0 || fd[i] >=  MAXFILES) {
				jputlong((long)-1);
				break;
			}
			lresult = lseek(fd[i],offset,x);
			
#ifdef DEBUG
			log("offset =%ld\n",offset);
			logint("fd=",i);
			log("lresult=%ld\n",lresult);
#endif
			jputlong(lresult);
			break;
		
		case _OPENDIR:
			
#ifdef DEBUG
			log("in opendir ");
#endif
			jgetstr(buf);
			dp = opendir(buf);
#ifdef DEBUG
			log("in opendir dir =%s",buf);
#endif
			if (dp == NULL) jputshort(-1);
			else jputshort(0);
			break;
		
		case _READDIR:
#ifdef DEBUG
		   	log("in readdir\n");
#endif
			if (dp == NULL) {
				/*jputshort(0);*/
				jputshort(-1);
				break;
			}
			nextdent = readdir(dp);
			if (nextdent == NULL)  {
				/*jputshort(0);*/
				jputshort(-1);
				break;
			}
			for (x=0;x< NAME_MAX && nextdent->d_name[x]!='\0'; x++) {
				buf[x]=nextdent->d_name[x];
			}
			buf[x++]= 0;
			
#ifdef DEBUG
			log("dir file=%s\n",buf);
#endif
			jputshort(x);
			jputbuf(buf,x);
			break;
		
		case _CLOSEDIR:
#ifdef DEBUG
			log("in closedir\n");
#endif
			if (dp == NULL) {
				jputshort(0);
				break;
			}
			closedir(dp);
			dp = 0;
			jputshort(0);
			break;
		
		default:
			
#ifdef DEBUG
		log("in default of main switch ");

#endif 
		break;
		}
	}
}




#ifdef DEBUG

log(format,string)
char *format;
char *string;
{

	fprintf(ferr, format, string);
	fflush(ferr);
}

logint(format, var)
char *format;
int var;
{
	fprintf(ferr, "%s=%d\n", format, var);
	fflush(ferr);
}

logchar(str,c)
char *str;
char c;
{
	fprintf(ferr, "%s=%c\n", str, c);
	fflush(ferr);
}

loghex(format, var)
char *format;
int var;
{
	fprintf(ferr, "%s=%x\n", format, var);
	fflush(ferr);
}


loglong(format, var)
char *format;
long var;
{
	fprintf(ferr, "%s=%ld\n", format, var);
	fflush(ferr);
}
#endif



shutdown(sig)
    	int sig;
{
	if (t_snddis(xmtg_fd, NULLP) < 0)
	{
	    t_error("t_snddis failed");
	    exit(1);
	}
	exit(0);	
}
