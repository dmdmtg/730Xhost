/*copyright	"%c%"*/
#ident	"@(#)r3fontutil:mkfontdir/mkfontdir.c	1.5"
/***********************************************************
Copyright 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved



******************************************************************/

/* $XConsortium: mkfontdir.c,v 1.11 88/11/15 11:52:06 jim Exp $ */

#include <stdio.h>
#include <X11/Xos.h>
/* #include <X11/Xmu.h>  don't include this */
void XmuCopyISOLatin1Lowered();
#ifndef SVR4_0
/* #include <sys/dir.h> */
/*
 * Instead of including this file, since it isn't part of the
 * 730X source package, put its (Xwin 3.0 source) contents here.
 */
#ifndef _DIR_H
#define _DIR_H

#include "/./usr/include/dirent.h"
#define direct dirent

#endif
#endif
#define MAXPATHLEN 256
#include <sys/param.h>
#ifdef SVR4_0
#include <dirent.h>
#endif
#include "fontdir.h"

#define  XK_LATIN1
#include <X11/keysymdef.h>

typedef void (*GetNameProc)(/* fp, font_name */);
typedef struct _FileType {
    char 		*extension;
    GetNameProc		proc;
    char		**filter;
} FileTypeRec, *FileType;

#ifndef UNCOMPRESSFILT
#define UNCOMPRESSFILT "/usr/ucb/uncompress"
#endif
#ifndef FCFLAGS
#define FCFLAGS "-t"
#endif
#ifndef BDFTOSNFFILT
#define BDFTOSNFFILT "/usr/bin/X11/bdftosnf"
#endif
#ifndef SHELLPATH
#define SHELLPATH "/bin/sh"
#endif
#ifndef ZBDFTOSNFFILT
#define ZBDFTOSNFFILT "/usr/ucb/uncompress | /usr/bin/X11/bdftosnf -t"
#endif

extern void GetSNFFontName();

#ifdef FONT_SNF
#ifdef COMPRESSED_FONTS
static char *
snfZFilter[] = {UNCOMPRESSFILT, NULL};
#endif
#endif

#ifdef FONT_BDF
static char *
bdfFilter[] = {BDFTOSNFFILT, FCFLAGS, NULL};
#ifdef COMPRESSED_FONTS
static char *
bdfZFilter[] = {SHELLPATH, "-c", ZBDFTOSNFFILT, NULL};
#endif
#endif

/*
 * The table of font file formats that are understood.  Order is important.
 */
static FileTypeRec fileTypes[] = {
#ifdef FONT_SNF
    {".snf", GetSNFFontName, NULL},
#ifdef COMPRESSED_FONTS
    {".snf.Z", GetSNFFontName, snfZFilter},
#endif
#endif
#ifdef FONT_BDF
    {".bdf", GetSNFFontName, bdfFilter},
#ifdef COMPRESSED_FONTS
    {".bdf.Z", GetSNFFontName, bdfZFilter},
#endif
#endif
    NULL
};

/*
 * Only works when table is freshly built. I.e. it cannot contain any aliases.
 */
static void WriteFontDir(table)
    FontTable table;
{
    char name[MAXPATHLEN];
    FILE *file;
    int i;
    int width = 0;

    /* XXX unix dependent file-name mangling */
    strcpy(name, table->directory);
    strcat(name, FontDirFile);
    file = fopen(name, "w");
    if (file == NULL) {
	fprintf(stderr, "%s: unable to open for writing\n", name);
	return;
    }
    for (i = 0; i < table->name.used; i++) {
	int idx = table->name.fn[i].u.index;
	int len = strlen(table->file.ff[idx].name);
	if (len > width)
	    width = len;
    }
    width += 4;
    fprintf(file, "%d\n", table->name.used);
    for (i = 0; i < table->name.used; i++) {
	int idx = table->name.fn[i].u.index;
	fprintf(file, "%-*s %s\n",
		width, table->file.ff[idx].name, table->name.fn[i].name);
    }
    fclose(file);
}

#if defined(macII) || (defined(SYSV) && !defined(hpux))
#define vfork() fork()
#endif

static int
FontFilter(fp, filter)
    FILE *fp;
    char **filter;
{
    int pfd[2];
    int pid;

    if (pipe(pfd) < 0) {
	fclose(fp);
	return (-1);
    }
    switch(pid = vfork()) {
    case 0:
	dup2(fileno(fp), 0);
	close(fileno(fp));
	dup2(pfd[1], 1);
	close(pfd[0]);
	close(pfd[1]);
	execvp(filter[0], filter);
	_exit(127);
    case -1:
	close(pfd[0]);
	close(pfd[1]);
	fclose(fp);
	return(-1);
    default:
	dup2(pfd[0], fileno(fp));
	close(pfd[0]);
	close(pfd[1]);
	return(pid);
    }    
}

static Boolean ProcessFile (dir, path, table, fileType)
#ifndef SVR4_0
    struct direct *dir;
#else
    struct dirent *dir;
#endif
    char *path;
    FontTable table;
    FileType fileType;
{
    int i, j;
    Boolean found;
    FILE *file;
    char font_name[MAXPATHLEN];
    int pid = -1, child;

    file = fopen(path, "r");
    if (file == NULL) {
	perror(path);
	return False;
    }
    font_name[0] = NUL;
    i = strlen(dir->d_name) - strlen(fileType->extension);
    strncpy (font_name, dir->d_name, i);
    font_name[i] = NUL;
    if (fileType->filter) {
	pid = FontFilter(file, fileType->filter);
	if (pid < 0) {
	    fprintf (stderr, "%s: failed to run %s filter, skipping\n",
		     path, fileType->filter[0]);
	    return False;
	}
    }
    (*fileType->proc)(file, font_name);
    fclose(file);
    if (pid >= 0)
       do { child = wait(0); } while (child != pid && child != -1);
    if (strlen(font_name) == 0) {
	fprintf(stderr, "%s: invalid font file, skipping\n", path);
	return False;
    }
    XmuCopyISOLatin1Lowered (font_name, font_name);
    i = AddFileEntry(table, dir->d_name, False);
    j = FindNameInFontTable (table, font_name, &found);
    if (found) {
	fprintf (stderr, "%s: same font name (%s) as %s, skipping\n",
	     path, font_name, table->file.ff[table->name.fn[j].u.index].name);
	return False;
    }
    (void) AddNameEntry(table, font_name, i);
    return True;
}

static Boolean Ematch(ext, name)
    char *ext;
    char *name;
{
    int i,j;

    i = strlen(name);
    j = strlen(ext);
    return ((i > j) && (strcmp(ext, name + i - j) == 0));
}

static Boolean Fmatch(name, ext, table)
    char *name;
    char *ext;
    FontTable table;
{
    int i, j;
    char *file;
    FileType ft;

    j = strlen(name) - strlen(ext);
    for (i = 0; i != table->file.used; i++) {
	file = table->file.ff[i].name;
	if ((strlen(file) > j) && !strncmp(name, file, j)) {
	    file += j;
	    for (ft = fileTypes; ft->extension != ext; ft++) {
		if (!strcmp(file, ft->extension))
		    return True;
	    }
	}
    }
    return False;
}

static int EnumFiles(directory, fileType, data)
    char *directory;
    FileType fileType;
    FontTable data;		/* should be an opaque type */
{
    DIR * dirp;
#ifndef SVR4_0
    struct direct  *file;
#else
    struct dirent *file;
#endif

    int     count = 0;
    int     i;
    char    filespec[MAXPATHLEN];
    char    filepath[MAXPATHLEN];

    if ((dirp = opendir (directory)) == NULL) {
	fprintf(stderr, "%s: unable to open directory, skipping\n", directory);
	return -1;
    }

    while ((file = readdir (dirp)) != NULL) {
	if (Ematch (fileType->extension, file->d_name) &&
	    !Fmatch (file->d_name, fileType->extension, data)) {
	    strcpy (filepath, directory);
	    if (directory[strlen(directory) - 1] != '/')
		strcat (filepath, "/");
	    strcat (filepath, file->d_name);
	    if (ProcessFile(file, filepath, data, fileType))
		count++;
	}
    }
    closedir (dirp);
    return count;
}

static void DoDirectory(dir)
    char *dir;
{
    FontTable table;
    char dirName[MAXPATHLEN];
    FileType fileType;

    /* XXX unix dependent file-name mangling */
    strcpy(dirName, dir);
    if (dirName[strlen(dirName) - 1] != '/')
	strcat(dirName, "/");
    table = MakeFontTable(dirName, 5);
    for (fileType = fileTypes; fileType->extension; fileType++) {
	if ((EnumFiles(dir, fileType, table) < 0) && (table->file.used == 0))
	    break;
    }
    if (table->file.used > 0)
	WriteFontDir(table);
    FreeFontTable(table);
}

main (argc, argv)
    int argc;
    char **argv;
{
    int i;

    if (argc == 1)
	DoDirectory(".");
    else
	for (i = 1; i < argc; i++) {
	    DoDirectory(argv[i]);
 	}
    exit(0);	
}

void
Xfree (l)
unsigned char	*l;
{
	free ((char *) l);
}

unsigned long *
Xalloc (n)
unsigned long	n;
{
	extern char	*malloc ();

	return (unsigned long *) malloc ((unsigned) n);
}

unsigned long *
Xrealloc (l, n)
unsigned char	*l;
unsigned long	n;
{
	extern char	*realloc ();

	return (unsigned long *) realloc ((char *) l, (unsigned) n);
}
