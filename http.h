//********************************************************************************************
//
// File : http.h implement for Hyper Text transfer Protocol
//
//********************************************************************************************

#define HOME		1
#define REGISTER	2
#define LOGIN		3

//********************************************************************************************
//
// Prototype function
//
//********************************************************************************************
extern WORD http_home( BYTE *rxtx_buffer );
extern WORD http_control( BYTE *rxtx_buffer );
extern WORD http_page( BYTE *rxtx_buffer );
extern WORD http_test( BYTE *rxtx_buffer );
extern BYTE http_get_variable ( BYTE *rxtx_buffer, WORD dlength, PGM_P val_key, BYTE *dest );
extern BYTE http_get_ip ( BYTE *buf, BYTE *dest );
extern void urldecode( BYTE *urlbuf);
extern void http_webserver_process ( BYTE *rxtx_buffer, BYTE *dest_mac, BYTE *dest_ip );
extern WORD http_put_request ( BYTE *rxtx_buffer );
