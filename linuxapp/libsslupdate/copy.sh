#!/bin/bash
#########################################################################
# File Name: copy.sh
# Author: suchao.wang
# mail: suchao.wang@advantech.com.cn
# Created Time: Tue 23 Jan 2018 11:19:10 AM CST
#########################################################################
if [ $# -ne 1 ]; then
	echo "Pleset input home path"
	exit 1
fi

HOME_PATH=$1
if [ -d $HOME_PATH ]; then
	echo "TISDK Path:" $HOME_PATH
else
	echo "Pleset input right path"
	exit 1
fi

echo "copy arm so file"
cp lib/libcrypto.so.1.0.0 $HOME_PATH/linux-devkit/sysroots/armv7ahf-vfp-neon-3.2-oe-linux-gnueabi/lib/libcrypto.so.1.0.0
cp lib/libssl.so.1.0.0 $HOME_PATH/linux-devkit/sysroots/armv7ahf-vfp-neon-3.2-oe-linux-gnueabi/usr/lib/libssl.so.1.0.0
cp -af include/openssl $HOME_PATH/linux-devkit/sysroots/armv7ahf-vfp-neon-3.2-oe-linux-gnueabi/usr/include/openssl

