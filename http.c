//********************************************************************************************
//
// File : http.c implement for Hyper Text transfer Protocol
//
//********************************************************************************************
#include "includes.h"
//********************************************************************************************
//
// Global variable for http.c
//
//********************************************************************************************
extern unsigned int bulb_1,bulb_2,fan;
//********************************************************************************************
//
// Function : http_webserver_process
// Description : Initial connection to web server
//
//********************************************************************************************
void http_webserver_process ( BYTE *rxtx_buffer, BYTE *dest_mac, BYTE *dest_ip )
{
	WORD dlength, dest_port;
	BYTE count_time_temp[3];
	BYTE generic_buf[64];
	BYTE mob_num[64];
	BYTE sec_code[64];
	BYTE receiver[64];
	BYTE ammount[64];
	char msg[64];

	unsigned int i,statusFlag;

	static unsigned int numCount = 0;

	unsigned char page;
	
	dest_port = (rxtx_buffer[TCP_SRC_PORT_H_P]<<8)|rxtx_buffer[TCP_SRC_PORT_L_P];
	// tcp port 80 start for web server
	if ( rxtx_buffer [ IP_PROTO_P ] == IP_PROTO_TCP_V && rxtx_buffer[ TCP_DST_PORT_H_P ] == 0 && rxtx_buffer[ TCP_DST_PORT_L_P ] == 80 )
	{
		// received packet with flags "SYN", let's send "SYNACK"
		if ( (rxtx_buffer[ TCP_FLAGS_P ] & TCP_FLAG_SYN_V) )
		{
//			tcp_send_synack ( rxtx_buffer, dest_mac, dest_ip );
			tcp_send_packet (
				rxtx_buffer,
				(WORD_BYTES){dest_port},
				(WORD_BYTES){80},					// source port
				TCP_FLAG_SYN_V|TCP_FLAG_ACK_V,			// flag
				1,						// (bool)maximum segment size
				0,						// (bool)clear sequence ack number
				1,						// (bool)calculate new seq and seqack number
				0,						// tcp data length
				dest_mac,		// server mac address
				dest_ip );		// server ip address
			flag1.bits.syn_is_received = 1;
			return;
		}

		if ( (rxtx_buffer [ TCP_FLAGS_P ] & TCP_FLAG_ACK_V) )
		{
			// get tcp data length
			dlength = tcp_get_dlength( rxtx_buffer );
			if ( dlength == 0 )
			{
				// finack, answer with ack
				if ( (rxtx_buffer[TCP_FLAGS_P] & TCP_FLAG_FIN_V) )
				{
//					tcp_send_ack ( rxtx_buffer, dest_mac, dest_ip );
					tcp_send_packet (
						rxtx_buffer,
						(WORD_BYTES){dest_port},
						(WORD_BYTES){80},						// source port
						TCP_FLAG_ACK_V,			// flag
						0,						// (bool)maximum segment size
						0,						// (bool)clear sequence ack number
						1,						// (bool)calculate new seq and seqack number
						0,						// tcp data length
						dest_mac,		// server mac address
						dest_ip );		// server ip address
				}
				return;
			}

			if ( http_get_variable ( rxtx_buffer, dlength, PSTR( "reg_send_msg" ), generic_buf ) ) {
			//	USART_SendString("data received");
				if(generic_buf[0] == '1') {
					if( http_get_variable ( rxtx_buffer, dlength, PSTR( "mobNum" ), mob_num ) ) {
						if( http_get_variable ( rxtx_buffer, dlength, PSTR( "secCode" ), sec_code ) ) {
							//USART_SendString("sms sending");
							//sprintf(msg,"emBank verification code: %s",sec_code);
							GSM_SendMessage(mob_num,sec_code);
						}
					}

					dlength = http_test( rxtx_buffer );
				}
			} else if ( http_get_variable ( rxtx_buffer, dlength, PSTR( "bal_trf" ), generic_buf ) ) {
			//	USART_SendString("data received");
				if(generic_buf[0] == '1') {
					if( http_get_variable ( rxtx_buffer, dlength, PSTR( "receiver" ), receiver ) ) {
						if( http_get_variable ( rxtx_buffer, dlength, PSTR( "ammount" ), ammount ) ) {
							//USART_SendString("sms sending");
							//sprintf(msg,"emBank verification code: %s",sec_code);
							GSM_TrfBalance(receiver,ammount);
						}
					}

					dlength = http_test( rxtx_buffer );
				}
			} else if( http_get_variable ( rxtx_buffer, dlength, PSTR( "check" ), generic_buf ) ) {
				if(generic_buf[0] == '1') {
					dlength = http_test( rxtx_buffer );
				}
			} else if (http_get_variable ( rxtx_buffer, dlength, PSTR( "control" ), generic_buf )) {
				if(generic_buf[0] == '1') {
					if (http_get_variable ( rxtx_buffer, dlength, PSTR( "bulb1" ), generic_buf )) {
						if(generic_buf[0] == '1') {
							bulb_1 = 1;
							CONTROL_PORT |= (1<<BULB_1_PIN);
						} else {
							bulb_1 = 0;
							CONTROL_PORT &= ~(1<<BULB_1_PIN);
						}
					}
					if (http_get_variable ( rxtx_buffer, dlength, PSTR( "bulb2" ), generic_buf )) {
						if(generic_buf[0] == '1') {
							bulb_2 = 1;
							CONTROL_PORT |= (1<<BULB_2_PIN);
						} else {
							bulb_2 = 0;
							CONTROL_PORT &= ~(1<<BULB_2_PIN);
						}
					}
					if (http_get_variable ( rxtx_buffer, dlength, PSTR( "fan" ), generic_buf )) {
						if(generic_buf[0] == '1') {
							fan = 1;
							CONTROL_PORT |= (1<<FAN_PIN);
						} else {
							fan = 0;
							CONTROL_PORT &= ~(1<<FAN_PIN);
						}
					}
					dlength = http_control( rxtx_buffer );
				}
			} else {
				dlength = http_home( rxtx_buffer );
			}

			// print webpage
			//dlength = http_page( rxtx_buffer );
			// send ack before send data
//			tcp_send_ack ( rxtx_buffer, dest_mac, dest_ip );
			tcp_send_packet (
						rxtx_buffer,
						(WORD_BYTES){dest_port},
						(WORD_BYTES){80},						// source port
						TCP_FLAG_ACK_V,			// flag
						0,						// (bool)maximum segment size
						0,						// (bool)clear sequence ack number
						1,						// (bool)calculate new seq and seqack number
						0,						// tcp data length
						dest_mac,		// server mac address
						dest_ip );		// server ip address
			// send tcp data
//			tcp_send_data ( rxtx_buffer, dest_mac, dest_ip, dlength );
			tcp_send_packet (
						rxtx_buffer,
						(WORD_BYTES){dest_port},
						(WORD_BYTES){80},						// source port
						TCP_FLAG_ACK_V | TCP_FLAG_PSH_V | TCP_FLAG_FIN_V,			// flag
						0,						// (bool)maximum segment size
						0,						// (bool)clear sequence ack number
						0,						// (bool)calculate new seq and seqack number
						dlength,				// tcp data length
						dest_mac,		// server mac address
						dest_ip );		// server ip address
			flag1.bits.syn_is_received = 0;
		}		
	}
}
//********************************************************************************************
//
// Function : http_get_ip
// Description : Get IP address from buffer (stored after call http_get_variable function)
// example after call http_get_variable function ip address (ascii) has been stored in buffer
// 10.1.1.1 (ascii), http_get_ip function convert ip address in ascii to binary and stored
// in BYTE *dest
//
//********************************************************************************************
unsigned char http_get_ip ( unsigned char *buf, BYTE *dest )
{
	unsigned char i, ch, digit, temp;

	i = 0;
	digit = 1;
	temp = 0;

	while ( 1 )
	{
		ch = *buf++;

		if ( ch >= '0' && ch <= '9' )
		{
			ch = ch - '0';
			temp = (temp * digit) + ch;
			digit *= 10;
		}
		else if ( ch == '.' || ch == '\0' )
		{
			dest[ i ] = temp;
			i++;
			digit = 1;
			temp = 0;
		}
		else
		{
			return 0;
		}
		if ( i == 4 )
			return i;
	}
}
//********************************************************************************************
//
// Function : http_get_variable
// Description : Get http variable from GET method, example http://10.1.1.1/?pwd=123456
//		when you call http_get_variable with val_key="pwd", then function stored "123456"
//		to dest buffer.
//
//********************************************************************************************
BYTE http_get_variable ( BYTE *rxtx_buffer, WORD dlength, PGM_P val_key, BYTE *dest )
{
	WORD data_p;
	PGM_P key;
	BYTE match=0, temp;

	key = val_key;
	
	// get data position
	data_p = tcp_get_hlength( rxtx_buffer ) + sizeof(ETH_HEADER) + sizeof(IP_HEADER);

	// Find '?' in rx buffer, if found '?' in rx buffer then let's find variable key (val_key)
	for ( ; data_p<dlength; data_p++ )
	{
		if ( rxtx_buffer [ data_p ] == '?' )
			break;
	}
	// not found '?' in buffer
	if ( data_p == dlength )
		return 0;
	
	// find variable key in buffer 
	for ( ; data_p<dlength; data_p++ )
	{
		temp = pgm_read_byte ( key );

		// end of variable keyword
		if ( rxtx_buffer [ data_p ] == '=' && match != 0 )
		{
			if ( temp == '\0' )
			{
				data_p++;
				break;
			}
		}
		// variable keyword match with rx buffer
		if ( rxtx_buffer [ data_p ] == temp )
		{
			key++;
			match++;
		}
		else
		{
			// no match in rx buffer reset match and find again
			key = val_key;
			match = 0;
		}
	}
	
	// if found variable keyword, then store variable value in destination buffer ( dest )
	if ( match != 0 )
	{
		match = 0;

		for ( ;; )
		{
			// end of variable value break from loop
			if ( rxtx_buffer [ data_p ] == '&' || rxtx_buffer [ data_p ] == ' ' )
			{
				dest [ match ] = '\0';
				break;
			}
			dest [ match ] = rxtx_buffer [ data_p ];
			match++;
			data_p++;
		}
	}

	// return with variable value length
	return match;
}
//********************************************************************************************
//
// Function : hex2int
// Description : convert a single hex digit character to its integer value
//
//********************************************************************************************
unsigned char hex2int(char c)
{
	if (c >= '0' && c <='9')
		return((unsigned char)c - '0');

	if (c >= 'a' && c <='f')
		return((unsigned char)c - 'a' + 10);
	
	if (c >= 'A' && c <='F')
		return((unsigned char)c - 'A' + 10);

	return 0;
}
//********************************************************************************************
//
// Function : urldecode
// Description : decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe"
//
//********************************************************************************************
void urldecode(unsigned char *urlbuf)
{
	unsigned char c;
	unsigned char *dst;

	dst=urlbuf;
	while ((c = *urlbuf))
	{
		if (c == '+') c = ' ';
		if (c == '%')
		{
			urlbuf++;
			c = *urlbuf;
			urlbuf++;
			c = (hex2int(c) << 4) | hex2int(*urlbuf);
		}
		*dst = c;
		dst++;
		urlbuf++;
	}
	*dst = '\0';
}
//*****************************************************************************************
//
// Function : http_put_request
// Description : put http request to tx buffer contain 2-variables pwd and temp.
// webserver receive pwd, temp and save to text file by PHP script on webserver.
//
//*****************************************************************************************
WORD http_put_request ( BYTE *rxtx_buffer )
{
	BYTE temp_value;
	WORD dlength;
	BYTE generic_buf[64];
	

	generic_buf[ 2 ] = '\0';

	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "GET /avrnet/save.php?pwd=secret&temp=" ), 0 );
	dlength = tcp_puts_data ( rxtx_buffer, (BYTE *)generic_buf, dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( " HTTP/1.0\r\n" ), dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "Host: 10.1.1.76\r\n" ), dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "User-Agent: AVR ethernet\r\n" ), dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "Accept: text/html\r\n" ), dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "Keep-Alive: 300\r\n" ), dlength );
	dlength = tcp_puts_data_p ( rxtx_buffer, PSTR ( "Connection: keep-alive\r\n\r\n" ), dlength );

	return dlength;
}
//*****************************************************************************************
//
// Function : http_home
// Description : prepare the webpage by writing the data to the tcp send buffer
//
//*****************************************************************************************
WORD http_home( BYTE *rxtx_buffer )
{
	WORD dlen;
	BYTE generic_buf[64];

	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR ( "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" ), 0 );
	
	return (dlen);
}

WORD http_test( BYTE *rxtx_buffer ) {
	WORD dlen;
	
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n"), 0 );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("success"), dlen );
	return (dlen);
}

WORD http_control( BYTE *rxtx_buffer ) {
	WORD dlen;
	
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n"), 0 );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<!DOCTYPE html>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<html><head>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<title>Control Room || emBank</title>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<title>Control Room || emBank</title>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<link rel=stylesheet type=text/css href=http://192.168.53.3/a/c/s.css />"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</head><body>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<section id=layout>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<section class=\"home\">"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<header>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=logo><a href=http://192.168.53.3/ ></a></div>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=\"tag-line\"><span>Embedded Server based Tansaction System</span></div>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<nav><ul>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<li><a href=http://192.168.53.3/>home</a></li>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<li><a href=http://192.168.53.3/>account</a></li>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</ul></nav>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=clear></div></header>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<section id=user-reg class=\"splash-content register\">"), dlen );
	
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=control><div class=cItem>"), dlen );

	if(bulb_1 == 1) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=bulb src=http://192.168.53.3/a/i/b-n.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&bulb1=0><img class=switch src=http://192.168.53.3/a/i/s-n.png></a>"), dlen );
	} else if(bulb_1 == 0) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=bulb src=http://192.168.53.3/a/i/b-f.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&bulb1=1><img class=switch src=http://192.168.53.3/a/i/s-f.png></a>"), dlen );
	}

	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=clear></div>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</div><div class=cItem>"), dlen );

	if(bulb_2 == 1) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=bulb src=http://192.168.53.3/a/i/b-n.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&bulb2=0><img class=switch src=http://192.168.53.3/a/i/s-n.png></a>"), dlen );
	} else if(bulb_2 == 0) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=bulb src=http://192.168.53.3/a/i/b-f.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&bulb2=1><img class=switch src=http://192.168.53.3/a/i/s-f.png></a>"), dlen );
	}

	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=clear></div>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</div><div class=cItem>"), dlen );

	if(fan == 1) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=\"fan rotating\" src=http://192.168.53.3/a/i/fan.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&fan=0><img class=switch src=http://192.168.53.3/a/i/s-n.png></a>"), dlen );
	} else if(fan == 0) {
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<img class=\"fan\" src=http://192.168.53.3/a/i/fan.png />"), dlen );
		dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<a href=/?control=1&fan=1><img class=switch src=http://192.168.53.3/a/i/s-f.png></a>"), dlen );
	}

	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("<div class=clear></div>"), dlen );

	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</div></div></section></section><footer>"), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("Copyright &copy; 2014 emBank. All rights reserved."), dlen );
	dlen = tcp_puts_data_p ( rxtx_buffer, PSTR("</footer></section></body></html>"), dlen );

	return (dlen);
}
