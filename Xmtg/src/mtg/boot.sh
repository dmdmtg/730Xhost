#	Copyright (c) 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)make:boot.sh	1.0"

#  boot.sh (Shell file)

# This script is used to bootstrap the MTG X host Makefiles and
# create binaries.

#   Modified by:	Karl Larson
#   Date:		8/2/89
#   Reason:		Added "./" in front of call to the imake program.
#			This is kind of a kludge to avoid an error that
#			occurs on the 3B2.  The imake script calls the
#			"type" facility, which behaves differently on
#			the 3B2 than on the 6386.  Putting "./" in front
#			of utils/imake/imake causes the call to "type"
#			to be bypassed in imake.
#
#			Also removed references to i386 and att3b2, since
#			these are not necessary.  i386 and u3b2 are defined
#			automatically on the respective machines by cc.

# Create runnable version of imake.
echo "Making imake and makedepend"
cd util/imake
ln imake.sh imake
chmod +x imake
cc -o atsign atsign.c
chmod +x atsign

# Create runnable version of makedepend.
cd ../..
cd util/makedepend
ln makedepnd.sh makedepend
chmod +x makedepend

# Make top-level Makefile.  Then, build all other Makefiles and depend them.
cd ../..
echo "Making top-level Makefile\c"
./util/imake/imake -DSYSV -DTOPDIR=.
echo "\nMaking other Makefiles\n"
make Makefiles
make depend

# Create binaries and install them.
echo "\nCreating binaries...\n"
make install

# Tell that were all done.
echo "\nboot.sh: Done!"
