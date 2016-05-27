#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)makedepend:makedepend.sh	1.1"

#  makedepend.sh (Shell file)
# 	Acc: 575302677 Fri Mar 25 09:17:57 1988
# 	Mod: 575150554 Wed Mar 23 15:02:34 1988
# 	Sta: 575302716 Fri Mar 25 09:18:36 1988
# 	Owner: 2011
# 	Group: 1985
# 	Permissions: 775


# 	START USER STAMP AREA


# 	END USER STAMP AREA

GETMAKEFILE="0"
GETOBJFILE="0"
GETSTARTAT="0"
MAKEFILE="Makefile"
OBJFILE=".o"
CFLAGS=""
VERBOSE=0
LIBFORE=""
LIBAFT=""
CC=/bin/cc
TAG="# DO NOT DELETE THIS LINE --- make depend depends on it."

for i in $*
do
if [ ${GETMAKEFILE} = "1" ]
then
   MAKEFILE=$i
   GETMAKEFILE="0"
else
   if [ ${GETOBJFILE} = "1" ]
   then
      OBJFILE=$i
      GETOBJFILE="0"
   else
      if [ ${GETSTARTAT} = "1" ]
      then
         TAG=$i
         GETSTARTAT="0"
      else
      case $i in
	-l) LIBFORE="\$(LIB)("; LIBAFT=")";;
	-l*) LIBFORE="`echo $i | cut -c3-`(";LIBAFT=")";;
	-w*) echo "${i} not implemented!";;
	-v) VERBOSE="1";;
	-o) GETOBJFILE="1";;
	-o*) OBJFILE=`echo $i | cut -c3-`;;
	-f) GETMAKEFILE="1";;
	-f*) MAKEFILE=`echo $i | cut -c3-`;;
	-D* | -I*) CFLAGS="${CFLAGS} ${i}";;
	-s) GETSTARTAT="1";;
	-s*) TAG=`echo $i | cut -c3-`;;
	-C*) CC=`echo $i | cut -c3-`;;
	-*) echo "ignoring option ${i}";;
	*) FILELIST="${FILELIST} ${i}";;
      esac
      fi
   fi
fi
done

if [ ${VERBOSE} = "1" ]
then
   echo "makedepending..."
   echo "         CC = $CC "
   echo "   FILELIST = $FILELIST "
   echo "   MAKEFILE = $MAKEFILE "
   echo "        TAG = $TAG "
   echo "     CFLAGS = $CFLAGS "
fi

if [ -z "${FILELIST}" ]
then
   echo "makedepend: no file(s) specified.  Check your SRCS variable."
   exit 1
fi
sed -e "/^${TAG}/,\$d" < ${MAKEFILE} > /tmp/Makefile$$
echo "${TAG}" >> /tmp/Makefile$$

for FILE in $FILELIST
do
      file=`echo ${FILE} | sed -e "s/\.c//"`
      echo "depending $file ..."
      ${CC} -E ${CFLAGS} ${FILE} | grep "^# " | cut -f3 -d" " > /tmp/maked$$ 
cat /tmp/maked$$ | sort -u | sed -e "s%^\"%${LIBFORE}${file}${OBJFILE}${LIBAFT}: %" -e "s%\"%%" >> /tmp/Makefile$$
done
   
grep -v " /usr/include/" /tmp/Makefile$$ > ${MAKEFILE}
rm /tmp/Makefile$$
rm /tmp/maked$$
