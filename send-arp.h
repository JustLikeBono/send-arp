#pragma once
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

#define ARPHDSIZE 28
#define ETHHDRSIZ 14

typedef struct arp_hdr{
	uint16_t ar_hrd;
	uint16_t ar_pro;
	uint8_t ar_hln;
	uint8_t ar_pln;
	uint16_t ar_op;
	uint8_t ar_sha[6];
	uint8_t ar_sip[4];
	uint8_t ar_tha[6];
	uint8_t ar_tip[4];
}arp_hdr;

unsigned long get_ip(char *if_name, int sockfd);
struct arp_hdr buildArpPacket(char if_name[], int sockfd, struct in_addr dst, struct ifreq if_hwaddr, unsigned char *dst_mac, unsigned long sip);
void send_packet(char if_name[], struct sockaddr_ll sk_addr, char hw_addr[], char payload[], int sockfd, int type, struct ifreq if_hwaddr, int sizePayload);


