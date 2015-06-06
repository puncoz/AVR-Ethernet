//********************************************************************************************
//
// File : include.h includes all header file for AVR Ethernet.
//
//********************************************************************************************
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

// struct.h MUST be include first
#include "struct.h"
#include "timer.h"
#include "enc28j60.h"
#include "ethernet.h"
#include "ip.h"
#include "tcp.h"
#include "icmp.h"
#include "arp.h"
#include "udp.h"
#include "http.h"
#include "delay.h"
#include "usart.h"
#include "gsm.h"


// define I/O interface
#define CONTROL_DDR		DDRC
#define CONTROL_PORT	PORTC
#define BULB_1_PIN	PINC3
#define BULB_2_PIN	PINC4
#define FAN_PIN		PINC5

#define LOW(uint) (uint&0xFF)
#define HIGH(uint) ((uint>>8)&0xFF)

#define MAX_RXTX_BUFFER		1500//1518

// global variables
extern MAC_ADDR avr_mac;
extern IP_ADDR avr_ip;

//extern MAC_ADDR client_mac;
//extern IP_ADDR client_ip;

extern MAC_ADDR server_mac;
extern IP_ADDR server_ip;

extern BYTE generic_buf[];
extern BYTE ee_avr_ip[];
extern BYTE ee_server_ip[];
//********************************************************************************************
//
// Prototype function from main.c
//
//********************************************************************************************
extern void initial_system( void );

