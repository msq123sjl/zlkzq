#!/bin/sh

if [[ "x" = x$1 || $# != 1 ]]; then
    echo "argv error"
    exit 1;
fi

MYPATH=$(cd `dirname $0`; pwd)
cd ${MYPATH}

sed -i "s/#define VERSION.*/#define VERSION      \"$1\"/g" ./daemon/common/tinz_base_def.h
VERSION=${1//\./_};

NAME=zlkzq
PACKAGE=${NAME}_${VERSION}.zip

test -d ./${NAME}/bin/ || mkdir -p ./${NAME}/bin/

cd ./daemon
./makeallclean || exit 1
./makeall || exit 1

rm -rf ../${NAME}/bin/interface
cd ../../temp/build-interface-ARM-Release/
make clean || exit 1
make || exit 1

cd -
cd ../


test -f ./${PACKAGE} && rm ./${PACKAGE}

chmod +x ./install.sh
chmod -R +x ./${NAME}/bin/

zip -r ${PACKAGE} ./${NAME}/ ./install.sh
