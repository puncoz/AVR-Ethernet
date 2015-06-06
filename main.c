//********************************************************************************************
//
// AVR Ethernet
//
// MCU : ATMEGA32 @ 16MHz
// Ethernet controller : ENC28J60
// IDE & Compiler : AVR Studio version 4.13.528 & WINAVR version 20070122
// Author : Puncoz Nepal
// Website : http://www.puncoz.com/
//
//********************************************************************************************
//
// File : main.c main program for AVR Ethernet.
//
//********************************************************************************************

// Including all required dependencies
#include "includes.h"

// Global variables
MAC_ADDR avr_mac;
IP_ADDR avr_ip;

MAC_ADDR server_mac;
IP_ADDR server_ip;

//BYTE generic_buf[128];

// Change your avr and server ip address here
// avr and server ip address are stored in eeprom
BYTE ee_avr_ip[4] EEMEM = { 192, 168, 53, 160 };
BYTE ee_server_ip[4] EEMEM = { 192, 168, 53, 1 };

// initialize status/states of bulb and fan
// 0: OFF; 1: ON
unsigned int bulb_1 = 0;
unsigned int bulb_2 = 0;
unsigned int fan = 0;

//*****************************************************************************************
//
// Function : server_process
// Description : Run web server and listen on port 80
//
//*****************************************************************************************
void server_process ( void )
{
	MAC_ADDR client_mac;
	IP_ADDR client_ip;
	// you can change rx,tx buffer size in includes.h
	BYTE rxtx_buffer[MAX_RXTX_BUFFER];
	WORD plen;
	
	if ( flag1.bits.syn_is_sent )
		return;
	// get new packet
	plen = enc28j60_packet_receive( (BYTE*)&rxtx_buffer, MAX_RXTX_BUFFER );
	
	//plen will ne unequal to zero if there is a valid packet (without crc error)
	if(plen==0)
		return;

	// copy client mac address from buffer to client mac variable
	memcpy ( (BYTE*)&client_mac, &rxtx_buffer[ ETH_SRC_MAC_P ], sizeof(MAC_ADDR) );
	
	// check arp packet if match with avr ip let's send reply
	if ( arp_packet_is_arp( rxtx_buffer, (WORD_BYTES){ARP_OPCODE_REQUEST_V} ) )
	{
		arp_send_reply ( (BYTE*)&rxtx_buffer, (BYTE*)&client_mac );
		return;
	}

	// get client ip address
	memcpy ( (BYTE*)&client_ip, &rxtx_buffer[ IP_SRC_IP_P ], sizeof(IP_ADDR) );
	// check ip packet send to avr or not?
	if ( ip_packet_is_ip ( (BYTE*)&rxtx_buffer ) == 0 )
	{
		return;
	}

	// check ICMP packet, if packet is icmp packet let's send icmp echo reply
	if ( icmp_send_reply ( (BYTE*)&rxtx_buffer, (BYTE*)&client_mac, (BYTE*)&client_ip ) )
	{
		return;
	}

	// check UDP packet
	if (udp_receive ( (BYTE *)&rxtx_buffer, (BYTE *)&client_mac, (BYTE *)&client_ip ))
	{
		return;
	}
	
	// tcp start here
	// start web server at port 80, see http.c
	http_webserver_process ( (BYTE*)rxtx_buffer, (BYTE*)&client_mac, (BYTE*)&client_ip );
}
//*****************************************************************************************
//
// Function : main
// Description : main program, 
//
//*****************************************************************************************
int main (void)
{
	// Initializing heartbeat of the system
	initTimer_heartBeat();
	
	// Initializing USART
	Init_USART(BAUD_PRESCALE);

	// Initializing GSM Modem
	Init_GSM();

	//USART_SendString("program initialized");

	// change your mac address here
	avr_mac.byte[0] = 'AA';
	avr_mac.byte[1] = 'FE';
	avr_mac.byte[2] = '56';
	avr_mac.byte[3] = 'D4';
	avr_mac.byte[4] = '76';
	avr_mac.byte[5] = '87';

	// Changing the status of bulb and fan into initial states	
	CONTROL_DDR |= (1<<BULB_1_PIN);
	CONTROL_DDR |= (1<<BULB_2_PIN);
	CONTROL_DDR |= (1<<FAN_PIN);

	// read avr and server ip from eeprom
	eeprom_read_block ( &avr_ip, ee_avr_ip, 4 );
	eeprom_read_block ( &server_ip, ee_server_ip, 4 );
	

	// initialiing enc28j60/Ehternet IC
	enc28j60_init( (BYTE*)&avr_mac );
	
	// loop forever
	// Infinite looping for continuous process
	for(;;)
	{
		// server process response for arp, icmp, http
		server_process ();
	}

	return 0;
}
