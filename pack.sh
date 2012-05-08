#!/bin/bash
appname=capmb
datum=`cat CapCompileDate`
#`date +"%y%m%d%H%M%S"`

git checkout BEISPIEL.cap.conf

mkdir ${datum}_${appname}
cp cap ${datum}_${appname}
cp BEISPIEL.cap.conf ${datum}_${appname}
cp aispec.ttf ${datum}_${appname}
cp LiberationMono-Regular.ttf ${datum}_${appname}
cp v4l_capture.o ${datum}_${appname}
cp readme ${datum}_${appname}
cp appcontroller.sh ${datum}_${appname}
cp RaLogo.png ${datum}_${appname}


tar cfz ${datum}_${appname}.tar.gz ${datum}_${appname}
rm -R ${datum}_${appname}
