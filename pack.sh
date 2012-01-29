#!/bin/bash
appname=capmb
datum=`cat CapCompileDate`
#`date +"%y%m%d%H%M%S"`

mkdir ${datum}_${appname}
cp cap ${datum}_${appname}
cp cap.conf ${datum}_${appname}
cp aispec.ttf ${datum}_${appname}
cp LiberationMono-Regular.ttf ${datum}_${appname}
cp v4l_capture.o ${datum}_${appname}
cp readme ${datum}_${appname}
cp appcontroller.sh ${datum}_${appname}


tar cfz ${datum}_${appname}.tar.gz ${datum}_${appname}
rm -R ${datum}_${appname}
