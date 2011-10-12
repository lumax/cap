#!/bin/bash


function run_cap()
{
./cap
#echo run_cap
}

function run_video()
{
    mplayer -loop 0 -fs PlateExakt08.wmv
}

export LD_LIBRARY_PATH+=:.
ln -fs libdsp_jpeg.so.1.0 libdsp_jpeg.so

while true
do
    run_cap
    rc=$?
    if [ $rc -eq 11 ]
    then
	echo "cap returns 12! show video"
	run_video
    else
	echo abschalten!
	exit
    fi
done