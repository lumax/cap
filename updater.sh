#!/bin/sh
#
# Prototype for software update script

# script mit popen aus c-Programm aufrufen um auf die Ausgaben zugreifen zu können

# Global definitions
VERSION="2.0"
INSTALL_DIR="/opt/"
TMP_USBSTICK=""


UBITOOLS="/opt/app/mtd-utils/ubi-utils"
#set -x

#cp -f updater.sh /home/sewerin/denx/arm/etc/.
#./updater.sh /usr/EA20IMG_20110405151042.tar validate copy flipboot

die()
{
    echo "$*" >&2
    #stage_clean
    exit 1
}

# Helper functions

# Find active set from U-Boot environment
read_activeset()
{
    # Only read active set once
    if [ -n "${ACTIVE_SET}" ]; then
		 return
    fi

    ACTIVE_SET=`fw_printenv as || echo as=3`
    ACTIVE_SET=`echo ${ACTIVE_SET} | sed 's/^.\+=//'`
    echo "active set is ${ACTIVE_SET}"

    if [ "${ACTIVE_SET}" = "3" ]; then
	MTD_ROOTFS="4"
    else
	MTD_ROOTFS="3"
    fi
    echo MTD_ROOTFS is ${MTD_ROOTFS}
}

# Implementation of individual stages
stage_validate()
{
    echo "updater Version ${VERSION}"
    echo "- validate"
    tar xOf ${update_file} ${UPD_MD5SUMS} > ${TMP_MD5SUMS}
    for file in ${COMPONENTS}; do
		 tar tf ${update_file} | grep -q $file || die "needed component '$file' not found in ${update_file}"
		 exp_sum=`awk '{ if ($2 == "'${file}'") print $1; }' ${TMP_MD5SUMS}`
		 sum=`tar xOf ${update_file} ${file} | md5sum | awk '{ print $1; }'`
		 if [ "$exp_sum" != "$sum" ]; then
		     die "md5sum mismatch for file '$file'"
		 fi
    done
    exit 0;
}

stage_copy()
{
    echo "- copy"
    read_activeset

    echo "read Image Size"
    #TMP_IMAGESIZE=`tar xOf ${update_file} ${UPD_IMAGESIZE} | awk '{ print $1; }'`
    TMP_IMAGESIZE=`tar tvf ${update_file} | awk '/'"${UPD_ROOTFS}"'/ { print $3; }'`
    echo "ImageSize = ${TMP_IMAGESIZE}"
    echo "copy Image to ${MTD_ROOTFS}"
    tar xOf ${update_file} ${UPD_ROOTFS} | ${UBITOOLS}/./ubiformat /dev/mtd$MTD_ROOTFS -q -f - -S $TMP_IMAGESIZE

    RETURNVAL=$?
    exit $RETURNVAL
}

stage_verify()
{
    echo "- verify"
    read_activeset

${UBITOOLS}/./ubiattach /dev/ubi_ctrl -m ${MTD_ROOTFS} -d 5
#./ubidetach -d 5
if [ $? -ne 0 ]
    then
    echo "Fehler ubiattach 0"
    RETURNVAL=$?
    exit $RETURNVAL
fi


#    echo "dd if=${MTD_KERNEL} of=${FIFO} &"
#    echo "BGJOB=$!"
#    echo "tar xOf ${update_file} ${UPD_KERNEL} | cmp - ${FIFO}"
#    echo "kill $BGJOB"
#    dd if=/dev/mtd${MTD_ROOTFS} of=${FIFO} &
    dd if=/dev/ubi5_0 of=${FIFO} &
    BPID=$!
    tar xOf ${update_file} ${UPD_ROOTFS} | cmp - ${FIFO} 2>&1
    RETURNVAL=$?
    exit $RETURNVAL
}


stage_flipboot()
{
    echo "- flipboot"
    read_activeset
    if [ "${ACTIVE_SET}" = "3" ]; then
		 ACTIVE_SET="4"
    else
		 ACTIVE_SET="3"
    fi
    echo "fw_setenv as ${ACTIVE_SET}"
    fw_setenv as ${ACTIVE_SET}
    RETURNVAL=$?
    exit $RETURNVAL
}

stage_search_stick2()
{
for file in "/dev/disk/by-label"
do
#echo $file
#echo
  ls -l $file | awk '{ print $8 "         file size: " $5 }'  # Print 2 fields.
#  whatis `basename $file`   # File info.
  # Note that the whatis database needs to have been set up for this to work.
  # To do this, as root run /usr/bin/makewhatis.
#  echo
done 
}


REGEXP='[0-9]\{12\}_capmb.tar.gz$'

stage_search_stick()
{
let num=0
ls -1 "/media/SWAP/" | while read file
do
    filelen=${#file}
    reg_match=`expr match "$file" $REGEXP` 
    if [[  filelen -eq 25 && reg_match -eq 25 ]]
	then echo $file
    fi
done 
echo `expr match "$file" $REGEXP`
}

stage_search1()
{
    let cnt=0
    ls /dev | while read line && [[ $line != end ]] #while read line
    do
	cnt=$(( $cnt + 1 ))
	echo $cnt $line $?
	arr[$cnt]="$line"
    done
    echo $line $arr[0]
    echo $cnt
}

stage_search2()
{
    echo "searching"
    #TMP_USBSTICK=`ls /dev/disk/by-label`
ls /dev/
 # | while read blah
#do
 # process lines.
echo huhu $? 
#done
#"/media/"`ls /dev/disk/by-label | awk '/'"${UPD_ROOTFS}"'/ { print $3; }'`
}

# Update file is mandatory argument
#if [ $# -lt 2 ]; then
#    die
#fi

#update_file=$1
#shift

# When no stage is specified, do all of them
if [ $# -eq 0 ]; then
    echo "stage needed!"
    exit -1
fi

while [ $# -gt 0 ]; do
    case $1 in
		 search_stick|search_updatefile|verify|copy)
		     eval "stage_$1"
		     ;;
		 *)
		     echo "unknown stage '$1'" >&2
		     die
		     ;;
    esac
    shift
done