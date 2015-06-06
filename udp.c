//********************************************************************************************
//
// File : udp.c implement for User Datagram Protocol
//
//********************************************************************************************
#include "includes.h"
//********************************************************************************************
// The User Datagram Protocol offers only a minimal transport service 
// -- non-guaranteed datagram delivery 
// -- and gives applications direct access to the datagram service of the IP layer. 
// UDP is used by applications that do not require the level of service of TCP or 
// that wish to use communications services (e.g., multicast or broadcast delivery) 
// not available from TCP.
//
// +------------+-----------+-------------+----------+
// + MAC header + IP header +  UDP header + Data ::: +
// +------------+-----------+-------------+----------+
//
// UDP header
//
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// +00+01+02+03+04+05+06+07+08+09+10+11+12+13+14+15+16+17+18+19+20+21+22+23+24+25+26+27+28+29+30+31+
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// +                  Source port                  +               Destination port                +
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// +                  Length                       +               Checksum                        +
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// +                                           Data :::                                            +
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//********************************************************************************************
//
// Function : udp_generate_header
// Argument : BYTE *rxtx_buffer is a pointer point to UDP tx buffer
//			  WORD_BYTES dest_port is a destiantion port
//			  WORD_BYTES length is a UDP header and data length
// Return value : None
//
// Description : generate udp header
//
//********************************************************************************************
void udp_generate_header ( BYTE *rxtx_buffer, WORD_BYTES dest_port, WORD_BYTES length )
{
	WORD_BYTES ck;

	// setup source port, default value is 3000
	rxtx_buffer[UDP_SRC_PORT_H_P] = UDP_AVR_PORT_H_V;
	rxtx_buffer[UDP_SRC_PORT_L_P] = UDP_AVR_PORT_L_V;

	// setup destination port
	rxtx_buffer[UDP_DST_PORT_H_P] = dest_port.byte.high;
	rxtx_buffer[UDP_DST_PORT_L_P] = dest_port.byte.low;

	// setup udp length
	rxtx_buffer[UDP_LENGTH_H_P] = length.byte.high;
	rxtx_buffer[UDP_LENGTH_L_P] = length.byte.low;

	// setup udp checksum
	rxtx_buffer[UDP_CHECKSUM_H_P] = 0;
	rxtx_buffer[UDP_CHECKSUM_L_P] = 0;
	// length+8 for source/destination IP address length (8-bytes)
	ck.word = software_checksum ( (BYTE*)&rxtx_buffer[IP_SRC_IP_P], length.word+8, length.word+IP_PROTO_UDP_V);
	rxtx_buffer[UDP_CHECKSUM_H_P] = ck.byte.high;
	rxtx_buffer[UDP_CHECKSUM_L_P] = ck.byte.low;
}
//********************************************************************************************
//
// Function : udp_puts_data
// Description : puts data from RAM to UDP tx buffer
//
//********************************************************************************************
WORD udp_puts_data ( BYTE *rxtx_buffer, BYTE *data, WORD offset )
{
	while( *data )
	{
		rxtx_buffer[ UDP_DATA_P + offset ] = *data++;
		offset++;
	}

	return offset;
}
//********************************************************************************************
//
// Function : udp_puts_data_p
// Description : puts data from program memory to tx buffer
//
//********************************************************************************************
WORD udp_puts_data_p ( BYTE *rxtx_buffer, PGM_P data, WORD offset )
{
	BYTE ch;
	
	while( (ch = pgm_read_byte(data++)) )
	{
		rxtx_buffer[ UDP_DATA_P + offset ] = ch;
		offset++;
	}

	return offset;
}
//********************************************************************************************
//
// Function : udp_receive
// Argument : BYTE *rxtx_buffer is a pointer, point to UDP tx buffer
//			  BYTE *dest_mac is a pointer, point to destiantion MAC address
//			  BYTE *dest_ip is a pointer, point to destiantion IP address
// Return value : if received packet is UDP and destination port matching with AVR port, return true
//				  other return false
//
// Description : check received packet and process UDP command.
//
//********************************************************************************************
BYTE udp_receive ( BYTE *rxtx_buffer, BYTE *dest_mac, BYTE *dest_ip )
{
	WORD_BYTES dlength, adc0;
	BYTE generic_buf[64], temp, count_time_temp[3], tmp;

	// check UDP packet and check destination port
	if ( rxtx_buffer[IP_PROTO_P] != IP_PROTO_UDP_V || rxtx_buffer[UDP_DST_PORT_H_P] != UDP_AVR_PORT_H_V || rxtx_buffer[ UDP_DST_PORT_L_P ] != UDP_AVR_PORT_L_V )
		return 0;
	
	// unknown command, send "ERROR" to client
	dlength.word = udp_puts_data_p ( rxtx_buffer, PSTR("ERROR\r\n"), 0 );

	// set ethernet header
	eth_generate_header (rxtx_buffer, (WORD_BYTES){ETH_TYPE_IP_V}, dest_mac );
	
	// generate ip header and checksum
	ip_generate_header (rxtx_buffer, (WORD_BYTES){sizeof(IP_HEADER)+sizeof(UDP_HEADER)+dlength.word}, IP_PROTO_UDP_V, dest_ip );

	// generate UDP header
	udp_generate_header (rxtx_buffer, (WORD_BYTES){(rxtx_buffer[UDP_SRC_PORT_H_P]<<8)|rxtx_buffer[UDP_SRC_PORT_L_P]}, (WORD_BYTES){sizeof(UDP_HEADER)+dlength.word});

	// send packet to ethernet media
	enc28j60_packet_send ( rxtx_buffer, sizeof(ETH_HEADER)+sizeof(IP_HEADER)+sizeof(UDP_HEADER)+dlength.word );

	return 1;
}
