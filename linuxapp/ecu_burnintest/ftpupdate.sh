#!/bin/sh
DEST=192.168.50.2
/etc/init.d/network reload
ping -c 1 $DEST > /dev/null
if [ $? != 0 ];then
	exit 1
fi
ftp -n <<!
open $DEST 
user fac fac
prompt <<!
bin
mput $1
bye
!
