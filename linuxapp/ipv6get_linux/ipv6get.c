/*************************************************************************
	> File Name: ipv6get.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 25 Dec 2014 01:11:51 PM CST
 ************************************************************************/

#include<stdio.h>

in6_addr GetIPv6( char const * iface )
{
	struct ifaddrs *ifa, *p;
	int family;
//	char address[200];
	struct in6_addr addr6;

	if (!iface)
	{
		return in6addr_any;
	}

	if (getifaddrs(&ifa))
	{
		perror("getifaddrs error");
		return in6addr_any;
	}

	for (p = ifa; p != NULL; p = p->ifa_next)
	{
		if (strcmp(iface, p->ifa_name) != 0)
			continue;
		family = p->ifa_addr->sa_family;
		/* Just check IPv6 address */
		if (family != AF_INET6)
			continue;

		addr6 = ((struct sockaddr_in6 *) (p->ifa_addr))->sin6_addr;
		/* Just get IPv6 linklocal address of the interface */
		if (IN6_IS_ADDR_LINKLOCAL(&addr6))
		{
			break;
		}else{
			memset(&addr6,0,sizeof(addr6));
		}
	}
	freeifaddrs(ifa);
	return addr6;

}
