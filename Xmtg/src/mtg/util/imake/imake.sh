#	Copyright (c) 1988 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)imake:imake.sh	1.1"

#  imake.sh (Shell file)
# 	Acc: 575242814 Thu Mar 24 16:40:14 1988
# 	Mod: 575242824 Thu Mar 24 16:40:24 1988
# 	Sta: 575242824 Thu Mar 24 16:40:24 1988
# 	Owner: 2011
# 	Group: 1985
# 	Permissions: 775


# 	START USER STAMP AREA


# 	END USER STAMP AREA

echo "...\c"
makefile=Makefile
imakefile=Imakefile
tempfile1=/tmp/imake1_$$
tempfile2=/tmp/imake2_$$

TEMPLATE=Imake.tmpl
imakecpp=${IMAKECPP:-/bin/cc}
imakemake=${IMAKEMAKE:-make}

case $0 in
.* | /*) imakesrc=`echo ${0} | sed -e "s;/imake$;;"`;;
*) imakesrc=`type ${0} | cut -f3 -d" " | sed -e "s;/imake$;;"`
esac

imakeinclude="-I. -I${imakesrc}.includes ${IMAKEINCLUDE:+-I${IMAKEINCLUDE}}"

DEFINES=""
showflag=0
verbose=0
MAKEFLAGS=""
filename=0
mfilename=0

for i in $*
   do
   if [ ${filename} = 1 ]
   then
      filename=0
      imakefile=$i
   else
      if [ ${mfilename} = 1 ]
      then
         mfilename=0
         makefile=$i
      else
         case $i in 
         -f)  filename=1;;
         -o)  mfilename=1;;
         -T*) TEMPLATE=`echo $i | cut -c3-`;;
         -D*) DEFINES="${DEFINES} ${i}";;
         -s)  showflag=1;;
         -v)  verbose=1;;
         *)   MAKEFLAGS="${MAKEFLAGS} ${i}";;
         esac
      fi
   fi
   done

echo "...\c"
echo "/* `date` */" > ${tempfile1}
echo "#define IMAKE_TEMPLATE \"${TEMPLATE}\"" >> ${tempfile1}
echo "#define INCLUDE_IMAKEFILE \"${imakefile}\"" >> ${tempfile1}
echo "#include \"${TEMPLATE}\"" >> ${tempfile1}
echo "...\c"

if [ "${showflag}" = "1" ]
then
   makefile=/dev/`who am i | cut -c12-21 | cut -f1 -d" "`
fi

${imakecpp} ${DEFINES} ${imakeinclude} -E -Wp,-P ${tempfile1} > ${tempfile2}
echo "...\c"
cat ${tempfile2} | ${imakesrc}/atsign > ${makefile}
echo "...\c"
rm ${tempfile1}
rm ${tempfile2}
chmod 664 ${makefile}
echo "...\c"

if [ ${verbose} = 1 ]
then
   echo ${imakemake} ${MAKEFLAGS} Makefiles
fi
