/*************************************************************************
	> File Name: socket_route.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 29 Sep 2016 03:00:15 PM CST
 ************************************************************************/

/*
 * author: wangsd
 * description: setting the static route
 * copyright: GPL
 * history:
 * 2012.10.31    1.0 version    ipv4/ipv6 route add/del
 * ### IPv4
my_route -A inet add -net 192.56.76.0 netmask 255.255.255.0 dev eth0
my_route -A inet add -net 192.56.76.0 netmask 255.255.255.0 dev eth0
### IPv6
my_route -A inet6 add -net ::/0 dev eth0 mtu 1500
my_route -A inet6 del -net ::/0 dev eth0 mtu 1500
 */

#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <net/if.h>       /* ifreq struct */
#include <netdb.h>


#define RTACTION_ADD 1   /* add action */
#define RTACTION_DEL 2   /* del action */

void usage();
int inet_setroute(int action, char **args);
int inet6_setroute(int action, char **args);

/* main function */
int main(int argc, char **argv)
{
    int action = 0;
    if(argc < 5)
    {
        usage();
        return -1;
    }
    if(strcmp(argv[1], "-A"))
    {
        usage();
        return -1;
    }

    if(!strcmp(argv[3], "add"))
    {
        action = RTACTION_ADD;
    }
    if(!strcmp(argv[3], "del"))
    {
        action = RTACTION_DEL;
    }

    /* add or del a ipv4 route item */
    if(!strcmp(argv[2], "inet"))
    {
        inet_setroute(action, argv+4);
    }
    /* add  or del a ipv6 route item */
    if(!strcmp(argv[2], "inet6"))
    {
        inet6_setroute(action, argv+4);
    }
    return 0;
}


/* print usage information */
void usage()
{
    printf("IPv4 Command: route -A inet add/del -net/-host TARGET netmask "
            "NETMASK gw GETWAY dev DEVICE mtu MTU\n");
    printf("IPv6 Command: route -A inet6 add/del -net TARGET/PREFIX "
            "gw GETWAY dev DEVICE mtu MTU\n");
    return ;
}

/*
 *  IPv4 add/del route item in route table
 */
int inet_setroute(int action, char **args)
{
    struct rtentry route;  /* route item struct */
    char target[128] = {0};
    char gateway[128] = {0};
    char netmask[128] = {0};

    struct sockaddr_in *addr;

    int skfd;

    /* clear route struct by 0 */
    memset((char *)&route, 0x00, sizeof(route));

    /* default target is net (host)*/
    route.rt_flags = RTF_UP ;

    args++;
    while(args)
    {
        if(*args == NULL)
        {
            break;
        }
        if(!strcmp(*args, "-net"))
        {/* default is a network target */
            args++;
            strcpy(target, *args);
            addr = (struct sockaddr_in*) &route.rt_dst;
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = inet_addr(target);
            args++;
            continue;
        }
        else if(!strcmp(*args, "-host"))
        {/* target is a host */
            args++;
            strcpy(target, *args);
            addr = (struct sockaddr_in*) &route.rt_dst;
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = inet_addr(target);
            route.rt_flags |= RTF_HOST;
            args++;
            continue;
        }
        else

        if(!strcmp(*args, "netmask"))
        {/* netmask setting */
            args++;
            strcpy(netmask, *args);
            addr = (struct sockaddr_in*) &route.rt_genmask;
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = inet_addr(netmask);
            args++;
            continue;
        } else
        if(!strcmp(*args, "gw") || !strcmp(*args, "gateway"))
        {/* gateway setting */
            args++;
            strcpy(gateway, *args);
            addr = (struct sockaddr_in*) &route.rt_gateway;
            addr->sin_family = AF_INET;
            addr->sin_addr.s_addr = inet_addr(gateway);
            route.rt_flags |= RTF_GATEWAY;
            args++;
            continue;
        }else
        if(!strcmp(*args, "device") || !strcmp(*args, "dev"))
        {/* device setting */
            args++;
            route.rt_dev = *args;
            args++;
            continue;
        }else
        if(!strcmp(*args, "mtu"))
        {/* mtu setting */
            args++;
            route.rt_flags |= RTF_MTU;
            route.rt_mtu = atoi(*args);
            args++;
            continue;
        }

        /* if you have other options, please put them in this place,
          like the options above. */
    }

    /* create a socket */
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd < 0)
    {
        perror("socket");
        return -1;
    }

    /* tell the kernel to accept this route */
    if(action == RTACTION_DEL)
    {/* del a route item */
        if(ioctl(skfd, SIOCDELRT, &route) < 0)
        {
            perror("SIOCDELRT");
            close(skfd);
            return -1;
        }
    }
    else
    {/* add a route item */
        if(ioctl(skfd, SIOCADDRT, &route) < 0)
        {
            perror("SIOCADDRT");
            close(skfd);
            return -1;
        }
    }
    (void) close(skfd);
    return 0;
}

int INET6_resolve(char *name, struct sockaddr_in6 *sin6);
int INET6_input(int type, char *bufp, struct sockaddr *sap);
int INET6_getsock(char *bufp, struct sockaddr *sap);

/* IPv6 add/del route item in route table */
/* main part of this function is from net-tools inet6_sr.c file */
int inet6_setroute(int action, char **args)
{
    struct in6_rtmsg rt;          /* ipv6 route struct */
    struct ifreq ifr;             /* interface request struct */
    struct sockaddr_in6 sa6;      /* ipv6 socket address */
    char target[128];
    char gateway[128] = "NONE";
    int metric;
    int prefix_len;               /* network prefix length */
    char *devname = NULL;         /* device name */
    char *cp;
    int mtu = 0;

    int skfd = -1;

    if (*args == NULL )
    {
        usage();
        return -1;
    }

    args++;
    strcpy(target, *args);

    if (!strcmp(target, "default"))
    {
        prefix_len = 0;
        memset(&sa6, 0, sizeof(sa6));
    } else
    {
        if ((cp = strchr(target, '/')))
        {
            prefix_len = atol(cp + 1);
            if ((prefix_len < 0) || (prefix_len > 128))
                usage();
            *cp = 0;
        } else
        {
            prefix_len = 128;
        }
        if (INET6_input(1, target, (struct sockaddr *) &sa6) < 0
                && INET6_input(0, target, (struct sockaddr *) &sa6) < 0)
        {
            return (1);
        }
    }

    /* Clean out the RTREQ structure. */
    memset((char *) &rt, 0, sizeof(struct in6_rtmsg));

    memcpy(&rt.rtmsg_dst, sa6.sin6_addr.s6_addr, sizeof(struct in6_addr));

    /* Fill in the other fields. */
    rt.rtmsg_flags = RTF_UP;
    if (prefix_len == 128)
        rt.rtmsg_flags |= RTF_HOST;
    rt.rtmsg_metric = 1;
    rt.rtmsg_dst_len = prefix_len;

    args++;
    while (*args)
     {
         if (!strcmp(*args, "metric"))
         {
             args++;
             if (!*args || !isdigit(**args))
             {
                 usage();
                 return -1;
             }
             metric = atoi(*args);
             rt.rtmsg_metric = metric;
             args++;
             continue;
         }
         if (!strcmp(*args, "gw") || !strcmp(*args, "gateway"))
         {
             args++;
             if (!*args)
                 return -1;
             if (rt.rtmsg_flags & RTF_GATEWAY)
                 return -1;
             strcpy(gateway, *args);
             if (INET6_input(1, gateway, (struct sockaddr *) &sa6) < 0)
             {
                 return -1;
             }
             memcpy(&rt.rtmsg_gateway, sa6.sin6_addr.s6_addr,
                     sizeof(struct in6_addr));
             rt.rtmsg_flags |= RTF_GATEWAY;
             args++;
             continue;
         }
         if (!strcmp(*args, "mod"))
         {
             args++;
             rt.rtmsg_flags |= RTF_MODIFIED;
             continue;
         }
         if (!strcmp(*args, "dyn"))
         {
             args++;
             rt.rtmsg_flags |= RTF_DYNAMIC;
             continue;
         }
         if (!strcmp(*args, "mtu"))
         {
             args++;
             mtu = atoi(*args);
             args++;
             continue;
         }
         if (!strcmp(*args, "device") || !strcmp(*args, "dev"))
         {
             args++;
             if (!*args)
                 return -1;
         } else if (args[1])
             return -1;

         devname = *args;
         args++;
     }

    /* Create a socket to the INET6 kernel. */
    if ((skfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));

    if (devname)
    {/* device setting */
        strcpy(ifr.ifr_name, devname);

        if (ioctl(skfd, SIOGIFINDEX, &ifr) < 0)
        {
            perror("SIOGIFINDEX");
            return -1;
        }
        rt.rtmsg_ifindex = ifr.ifr_ifindex;
    }

    if (mtu)
    {/* mtu setting */
        ifr.ifr_mtu = mtu;

        if (ioctl(skfd, SIOCSIFMTU, &ifr) < 0)
        {
            perror("SIOCGIFMTU");
            return -1;
        }
    }

    /* Tell the kernel to accept this route. */
    if (action == RTACTION_DEL)
    {
        if (ioctl(skfd, SIOCDELRT, &rt) < 0)
        {
            perror("SIOCDELRT");
            close(skfd);
            return -1;
        }
    } else
    {
        if (ioctl(skfd, SIOCADDRT, &rt) < 0)
        {
            perror("SIOCADDRT");
            close(skfd);
            return -1;
        }
    }

    /* Close the socket. */
    (void) close(skfd);
    return (0);
}


/*
 * following functions are ipv6 address transfrom
 * (from string to address struct and so on.)
 * these functions from net-tools inet6.c file.
 */

int INET6_resolve(char *name, struct sockaddr_in6 *sin6)
{
    struct addrinfo req, *ai;
    int s;

    memset (&req, '\0', sizeof req);
    req.ai_family = AF_INET6;
    if ((s = getaddrinfo(name, NULL, &req, &ai)))
    {
        //perror("getaddrinfo");
        fprintf(stderr, "getaddrinfo: %s: %d\n", name, s);
        return -1;
    }
    memcpy(sin6, ai->ai_addr, sizeof(struct sockaddr_in6));

    freeaddrinfo(ai);

    return (0);
}
int INET6_getsock(char *bufp, struct sockaddr *sap)
{
    struct sockaddr_in6 *sin6;

    sin6 = (struct sockaddr_in6 *) sap;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = 0;

    if (inet_pton(AF_INET6, bufp, sin6->sin6_addr.s6_addr) <= 0)
    return (-1);

    return 16;            /* ?;) */
}

int INET6_input(int type, char *bufp, struct sockaddr *sap)
{
    switch (type)
    {
    case 1:
        return (INET6_getsock(bufp, sap));
    default:
        return (INET6_resolve(bufp, (struct sockaddr_in6 *) sap));
    }
}

/*------------------------end-----------------------------------*/

