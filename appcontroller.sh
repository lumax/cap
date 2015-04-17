#!/bin/bash

# Start und updatescript für capmb

# Global definitions
VERSION="2.0"
#set -x

REGEXP='[0-9]\{12\}_capmb.tar.gz$'
EXEC_DIR_LINK="capmb"
UPD_FILE_LEN=25
UPD_DIR_LEN=18
COMPONENTS="cap cap.conf v4l_capture.o appcontroller.sh"
UPD_FILE="" #/media/SWAP/111016114939_capmb.tar.gz"
UPD_DISK="/media/SERVICE_USB_STICK/"
UPD_DIR_NAME=""
UPDATE_STEPS="stage_search_updatefile stage_check_updatefile stage_check_current_version stage_copy_update"
PROGRAMS_ROOT=""
SHUTDOWNCMD="/sbin/halt"
SHUTDOWNCMD_CONFIG="/opt/shutdowncmd.config"


function stage_update()
{
    echo " "
    echo " "
    echo "############################################################"
    echo "try to update the system"
    for func in ${UPDATE_STEPS}; do
	#echo "eval $func"
	eval "$func"
	if [[ $? -ne 0 ]]
	then
	    #echo "no update done, $func returned non zero value"
	    echo "no update done"
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

function stage_copy_update()
{
    retval=0
    tar xfz ${UPD_FILE}

    retvalue=$?
    if [[ $retvalue -ne 0 ]]
    then 
	echo "stage_copy_update: unpack update failed $retvalue"
	return $retval;
    fi
#echo ln -fns ${PROGRAMS_ROOT}/${UPD_DIR_NAME} ${PROGRAMS_ROOT}/${EXEC_DIR_LINK}
    ln -fns ${PROGRAMS_ROOT}/${UPD_DIR_NAME} ${PROGRAMS_ROOT}/${EXEC_DIR_LINK}
    retvalue=$?
    if [[ $retvalue -ne 0 ]]
    then 
	echo "stage_copy_update: set link failed $retvalue"
	return $retval;
    fi
    return $retval
}

function stage_check_updatefile()
{
#echo "stage_check_updatefile() hat tar: write errors"
retval=0
    for file in ${COMPONENTS}; do
	#echo $file
	tar tf ${UPD_FILE} | grep -q $file || retval=1 #echo "needed component '$file' not found in ${UPD_FILE}"
    done
    return $retval;
}

function stage_check_current_version()
{
retval=0

#echo ls -l ${PROGRAMS_ROOT}/${EXEC_DIR_LINK}
ls -l ${PROGRAMS_ROOT}/${EXEC_DIR_LINK}
retvalue=$?
if [[ $retvalue -ne 0 ]]
then 
    echo "stage_check_current_version() failed $retvalue"
    return $retval;
fi

cur_filename=`ls -l --time-style=long-iso ${PROGRAMS_ROOT}/${EXEC_DIR_LINK} | awk '{ print $10; }' ${cur_filename} `
cur_number=${cur_filename:${#cur_filename}-${UPD_DIR_LEN}:12}
upd_number=${UPD_DIR_NAME:0:12}
echo $cur_filename
echo $cur_number
echo $upd_number
if [ $upd_number -gt $cur_number ]
then
    echo found update ${upd_number} which is newer than ${cur_number}
    return 0
else
    echo no newer update found
    return 1
fi


return $retval;
}

function stage_search_stick()
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

function stage_search_updatefile()
{
retval=1
line=`ls -1 $UPD_DISK`
for file in $line
do
    filelen=${#file} # die Länge eines Strings
    reg_match=`expr match "$file" $REGEXP`
    if [[  filelen -eq $UPD_FILE_LEN && reg_match -eq $UPD_FILE_LEN ]]
    then
	retval=0
	#echo $file  
	UPD_FILE=$UPD_DISK$file
	UPD_DIR_NAME=${file:0:$UPD_DIR_LEN}
	#echo stage_search_updatefile found updatefile: ${UPD_FILE}
    fi
done
return $retval
}

function run_cap()
{
./cap
#echo run_cap
}

function run_video()
{
    echo "PLAYING VIDEO"
    mplayer -vo fbdev -nosound -zoom -fs ../PlateExakt.avi
    clear
}

while true
do
    run_cap
    rc=$?
    if [ $rc -eq 12 ]
    then
	cd ..
	PROGRAMS_ROOT=`pwd`
	stage_update
	echo "shutdown system!"

	if [ -e $SHUTDOWNCMD_CONFIG ];
	then
	    SHUTDOWNCMD=`cat ${SHUTDOWNCMD_CONFIG}`
	fi
	exec ${SHUTDOWNCMD}
	exit
    fi
    if [ $rc -eq 11 ]
    then
	echo "cap returns 11! show video"
	run_video
    else
	echo "cap returns in Service Mode"
	exit
    fi
done