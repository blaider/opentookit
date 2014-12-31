#########################################################################
# File Name: ping.sh
# Author: suchao.wang
# mail: suchao.wang@advantech.com.cn
# Created Time: Mon 22 Dec 2014 02:56:39 PM CST
#########################################################################
#!/bin/bash
ping -c 2 192.168.50.2 >/dev/null
if [ $? = 0 ];then
    echo "连接成功"
else
	echo "连接不成功"
fi
