#include "send-arp.h"


void send_packet(char if_name[], struct sockaddr_ll sk_addr, char hw_addr[], char payload[], int sockfd, int type, struct ifreq if_hwaddr, int sizePayload){
	struct ether_header frame;
	memset(&frame,0,sizeof(struct ether_header));
	memcpy(frame.ether_dhost, hw_addr, 6);
	memcpy(frame.ether_shost, if_hwaddr.ifr_hwaddr.sa_data, 6);
	switch(type){
		case 1:
			frame.ether_type = htons(ETH_P_IP);
			break;
		case 2:
			frame.ether_type = htons(ETHERTYPE_ARP);
			break;
		default:
			frame.ether_type = htons(ETH_P_IP);
			break;

	}

	struct ifreq if_idx;
	memset(&if_idx,0,sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, if_name, IFNAMSIZ-1);
	if(ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");

	unsigned char buff[256];
	char *eth_header = (char *)&frame;
	memcpy(buff,eth_header,ETHHDRSIZ);
	memcpy(&buff[ETHHDRSIZ],payload,sizePayload);

	sk_addr.sll_ifindex = if_idx.ifr_ifindex;
	sk_addr.sll_halen = ETH_ALEN;
	int byteSent = sendto(sockfd, buff, ETHHDRSIZ+sizePayload, 0, (struct sockaddr*)&sk_addr, sizeof(struct sockaddr_ll));
}

struct arp_hdr buildArpPacket(char if_name[], int sockfd, struct in_addr dst, struct ifreq if_hwaddr, unsigned char *dst_mac, unsigned long sip){
	struct arp_hdr arphdr;
	arphdr.ar_hrd = htons(0x0001);
	arphdr.ar_pro = htons(0x0800); 
	arphdr.ar_hln = 6;
	arphdr.ar_pln = 4;
	arphdr.ar_op = htons(0x0001);

	memcpy(arphdr.ar_sha, if_hwaddr.ifr_hwaddr.sa_data, 6);

	memcpy(arphdr.ar_sip, &sip, 4);
	memcpy(arphdr.ar_tha, dst_mac, 6);
	memcpy(arphdr.ar_tip, &dst.s_addr, 4);
	
	return arphdr;
}

unsigned long get_ip(char *if_name, int sockfd){
	struct ifreq if_idx;
	memset(&if_idx,0,sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, if_name, IFNAMSIZ-1);
	if(ioctl(sockfd, SIOCGIFADDR, &if_idx) < 0)
		perror("SIOCGIFADDR");
	return ((struct sockaddr_in *)&if_idx.ifr_addr)->sin_addr.s_addr;
}
