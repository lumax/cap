#!/bin/bash


function run_cap()
{
#  ./cap
echo run_cap
}

function run_video()
{
    mplayer cube4.avi
}

run_cap
if [ $? -eq 0 ]
then
  run_video()
  run_cap()
else
  exit
fi

#mplayer cube4.avi
#rc=$?
#if [[ $rc != 0 ]] ; then
#    echo return ist ungleich null
#    exit
#fi


echo return ist anscheinend gleich Null