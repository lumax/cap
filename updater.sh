#!/bin/sh
#
# Prototype for software update script

# script mit popen aus c-Programm aufrufen um auf die Ausgaben zugreifen zu können

# Global definitions
VERSION="2.0"
#INSTALL_DIR="/opt/"
TMP_USBSTICK=""

#set -x


die()
{
    echo "$*" >&2
    #stage_clean
    exit 1
}

# Helper functions

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




REGEXP='[0-9]\{12\}_capmb.tar.gz$'
EXEX_DIR_LINK="capmb"
UPD_FILE_LEN=25
UPD_DIR_LEN=18
COMPONENTS="cap cap.conf v4l_capture.o appcontroller.sh"
UPD_FILE="" #/media/SWAP/111016114939_capmb.tar.gz"
UPD_DISK="/media/"
UPD_DIR_NAME=""
#UPDATE_STEPS="stage_search_stick stage_search_updatefile stage_check_updatefile"
UPDATE_STEPS="stage_check_current_version"

stage_update()
{
    echo "try to update the system"
    for func in ${UPDATE_STEPS}; do
	#echo "eval $func"
	eval "$func"
	if [[ $? -ne 0 ]]
	then
	    echo "no update done, $func returned non zero value"
	    return 1
	fi
    done
    echo "DISK = $UPD_DISK"
    echo "update file  = $UPD_FILE"
    echo "update dirname = $UPD_DIR_NAME"
    echo "update done!"
    #reboot
    return 0
}

stage_copy_update()
{
    tar xfz ${UPD_FILE}
#entpacken in pwd/../
#link erzeugen in pwd/../
#reboot oder script verlassen wenn das script mit respawn aufgerufen wird, dann muss allerdings
# geprüft werden, ob das update neuer ist als das aktuell laufende Programm!!!
# also ein reboot !
}

stage_check_updatefile()
{
#echo "stage_check_updatefile() hat tar: write errors"
retval=0
    for file in ${COMPONENTS}; do
	#echo $file
	tar tf ${UPD_FILE} | grep -q $file || retval=1 #echo "needed component '$file' not found in ${UPD_FILE}"
    done
    return $retval;
}

stage_check_current_version()
{
echo "stage_check_current_version()"
retval=0

ls -l ${EXEX_DIR_LINK}
retvalue=$?
if [[ $retvalue -ne 0 ]]
then 
    echo "stage_check_current_version() failed $retvalue"
    return $retval;
fi

cur_filename=`ls -l ${EXEX_DIR_LINK} | awk '{ print $10; }' ${cur_filename} `
cur_number=${cur_filename:0:12}
echo $cur_filename
echo $cur_number
if [ $cur_number -gt 999999999999 ]
then
    echo grosser
else
    echo nicht groesser
fi


return $retval;
}

stage_search_stick()
{
retval=1
line=`ls -1 /dev/disk/by-label`
for file in $line
do
    retval=0
    UPD_DISK=$UPD_DISK$line"/"
done
return $retval
}

stage_search_updatefile()
{
retval=1
line=`ls -1 $UPD_DISK`
for file in $line
do
    filelen=${#file}
    reg_match=`expr match "$file" $REGEXP`
    if [[  filelen -eq $UPD_FILE_LEN && reg_match -eq $UPD_FILE_LEN ]]
    then
	retval=0
	#echo $file  
	UPD_FILE=$UPD_DISK$file
	UPD_DIR_NAME=${file:0:$UPD_DIR_LEN}
    fi
done
return $retval
}



stage_update
retvalue=$?
if [[ $retvalue -ne 0 ]]
then 
    echo "stage_update failed $retvalue"
    OK=0
fi
echo "execute normal programm"
exit 0

# Update file is mandatory argument
#if [ $# -lt 2 ]; then
#    die
#fi

#update_file=$1
#shift











OK=1

if [[ $OK -ne 0 ]]
    then
    stage_search_stick
    retvalue=$?
    if [[ $retvalue -ne 0 ]]
    then 
	echo "stage_search_stick failed $retvalue"
	OK=0
    fi
fi

if [[ $OK -ne 0 ]]
    then
    stage_search_updatefile
    retvalue=$?
    if [[ $retvalue -ne 0 ]]
    then 
	echo "stage_search_updatefile failed $retvalue"
	OK=0
    fi   
fi

if [[ $OK -ne 0 ]]
    then
    stage_check_updatefile
    retvalue=$?
    if [[ $retvalue -ne 0 ]]
    then 
	echo "check_updatefile failed $retvalue"
	OK=0
    fi   
fi
exit

# When no stage is specified, do all of them
if [ $# -eq 0 ]; then
    echo "stage needed!"
    exit -1
fi

while [ $# -gt 0 ]; do
    case $1 in
		 search_stick|search_updatefile|check_updatefile|copy_update|verify|copy)
		     eval "stage_$1"
		     ;;
		 *)
		     echo "unknown stage '$1'" >&2
		     die
		     ;;
    esac
    shift
done