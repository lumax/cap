#!/bin/bash

mkdir cap_test
cp cap cap_test
cp cap.conf cap_test
cp aispec.ttf cap_test
cp LiberationMono-Regular.ttf cap_test
cp v4l_capture.o cap_test
cp start_cap.sh cap_test
cp readme cap_test
cp appcontroller.sh cap_test


tar cfz `date +"%y%m%d%H%M%S"`_cap.tar.gz cap_test
rm -R cap_test
