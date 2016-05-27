#ifndef	NOIDENT
#ident	"@(#)fontstruct.h	2.1.2.1"
#endif
/*
 fontstruct.h (C header file)
	Acc: 575066380 Tue Mar 22 15:39:40 1988
	Mod: 575066380 Tue Mar 22 15:39:40 1988
	Sta: 575066542 Tue Mar 22 15:42:22 1988
	Owner: 8321
	Group: 1985
	Permissions: 644
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
#ifndef FONTSTRUCT_H
#define FONTSTRUCT_H 1
#include "font.h"
#include "misc.h"

/*
 * This file describes the Server Natural Font format.
 * SNF fonts are both CPU-dependent and frame buffer bit order dependent.
 * This file is used by:
 *	1)  the server, to hold font information read out of font files.
 *	2)  font converters
 *
 * Each font file contains the following
 * data structures, with no padding in-between.
 *
 *	1)  The XFONTINFO structure
 *		hand-padded to a two-short boundary.
 *
 *	2)  The XCHARINFO array
 *		indexed directly with character codes, both on disk
 *		and in memory.
 *
 *	3)  Character glyphs
 *		padded in the server-natural way, and
 *		ordered in the device-natural way.
 *		End of glyphs padded to 32-bit boundary.
 *
 *	4)  nProps font properties
 *
 *	5)  a sequence of null-terminated strings, for font properties
 */

typedef struct _FontProp {
	CARD32	name;		/* offset of string */
	INT32	value;		/* number or offset of string */
	INT32	indirect;	/* value is a string offset */
} FontPropRec;

typedef struct _CharInfo {
    xCharInfo	metrics;	/* info preformatted for Queries */
    unsigned long	 byteOffset:24;	/* byte offset of the raster from pGlyphs */
    Bool	exists:1;	/* true iff glyph exists for this char */
    unsigned	pad:7;		/* must be zero for now */
} CharInfoRec;

/*
 * maxbounds.byteOffset is the total number of bytes in the glyph array,
 * and maxbounds.bitOffset is total width of the unpadded font.
 */
typedef struct _FontInfo {
    unsigned long	version1;   /* version stamp */
    unsigned long	allExist;
    unsigned long	drawDirection;
    unsigned long	noOverlap;	/* true if:
					 * max(rightSideBearing-characterWidth)
					 * <= minbounds->metrics.leftSideBearing
					 */
    unsigned long	constantMetrics;
    unsigned long	terminalFont;	/* Should be deprecated!  true if:
					   constant metrics &&
					   leftSideBearing == 0 &&
					   rightSideBearing == characterWidth &&
					   ascent == fontAscent &&
					   descent == fontDescent
					*/
    unsigned long	linear:1;	/* true if firstRow == lastRow */
    unsigned long	constantWidth:1;  /* true if minbounds->metrics.characterWidth
					   *      == maxbounds->metrics.characterWidth
					   */
    unsigned long	inkInside:1;    /* true if for all defined glyphs:
					 * leftSideBearing >= 0 &&
					 * rightSideBearing <= characterWidth &&
					 * -fontDescent <= ascent <= fontAscent &&
					 * -fontAscent <= descent <= fontDescent
					 */
    unsigned long       inkMetrics:1;    /* inkMetrics != bitmap metrics */
					 /* used with terminalFont */
					 /* see font's pInk(CI,Min,Max) */
    unsigned long	padding:28;
    unsigned long	firstCol;
    unsigned long 	lastCol;
    unsigned long	firstRow;
    unsigned long	lastRow;
    unsigned long	nProps;
    unsigned long	lenStrings; /* length in bytes of string table */
    unsigned long	chDefault;  /* default character */ 
    unsigned long	fontDescent; /* minimum for quality typography */
    unsigned long	fontAscent;  /* minimum for quality typography */
    CharInfoRec		minbounds;  /* MIN of glyph metrics over all chars */
    CharInfoRec		maxbounds;  /* MAX of glyph metrics over all chars */
    unsigned long	pixDepth;   /* intensity bits per pixel */
    unsigned long	glyphSets;  /* number of sets of glyphs, for
					    sub-pixel positioning */
    unsigned long	version2;   /* version stamp double-check */
} FontInfoRec;

typedef struct _ExtentInfo {
    DrawDirection	drawDirection;
    long		fontAscent;
    long		fontDescent;
    long		overallAscent;
    long		overallDescent;
    long		overallWidth;
    long		overallLeft;
    long		overallRight;
} ExtentInfoRec;

#endif /* FONTSTRUCT_H */

