//********************************************************************************************
//
// File : udp.h implement for User Datagram Protocol
//
//********************************************************************************************
#define UDP_AVR_PORT_V		3000
#define UDP_AVR_PORT_H_V	(UDP_AVR_PORT_V>>8)
#define UDP_AVR_PORT_L_V	(UDP_AVR_PORT_V&0xff)

#define UDP_SRC_PORT_H_P	0x22
#define UDP_SRC_PORT_L_P	0x23
#define UDP_DST_PORT_H_P	0x24
#define UDP_DST_PORT_L_P	0x25
#define UDP_LENGTH_H_P		0x26
#define UDP_LENGTH_L_P		0x27
#define UDP_CHECKSUM_H_P	0x28
#define UDP_CHECKSUM_L_P	0x29
#define UDP_DATA_P			0x2A

extern void udp_generate_header ( BYTE *rxtx_buffer, WORD_BYTES dest_port, WORD_BYTES length );
extern WORD udp_puts_data ( BYTE *rxtx_buffer, BYTE *data, WORD offset );
extern WORD udp_puts_data_p ( BYTE *rxtx_buffer, PGM_P data, WORD offset );
extern BYTE udp_receive ( BYTE *rxtx_buffer, BYTE *dest_mac, BYTE *dest_ip );
