/* ics.h  ***************************************************************/
/*                                                                      */
/* CyberSource Internet Commerce Services (ICS) libraries               */
/* Copyright 1996, all rights reserved, CyberSource Corporation.        */
/*                                                                      */
/************************************************************************/

#ifndef _ICS_H
#define _ICS_H

#ifdef WIN32
#  define ICS2_CLIENT_LIB_VERSION "WIN32/C/5.1.1"
#  define DECLSPEC __declspec(dllexport)
#else
#  define DECLSPEC  
#endif

#define ICS_MAX_FIELDS  1024
#define ICS_DEBUG	1
#define ICS_TRACE	2

#define MASK_NONE	0
#define MASK_PARTIAL	1
#define MASK_FULL	2

typedef struct {
    int length;
    int debug;
    char* names[ICS_MAX_FIELDS];
    char* values[ICS_MAX_FIELDS];
    void* config; 
} ics_msg;

typedef struct {
    size_t merchant_cert_length;
    unsigned char *merchant_cert;
    size_t server_cert_length;
    unsigned char *server_cert;
    char *password;
} cert_buffer;

typedef struct {
    size_t length;
    unsigned char *data;
} buffer;


#ifdef __cplusplus
extern "C" {
#endif
DECLSPEC ics_msg *ics_init(int debug);
DECLSPEC ics_msg *ics_send(ics_msg *msg);
DECLSPEC int ics_print(ics_msg *msg);
DECLSPEC int ics_fadd(ics_msg *msg, char *name, char *value);
DECLSPEC int ics_fremove(ics_msg *msg, char *name);
DECLSPEC char *ics_fget(ics_msg *msg, int index);
DECLSPEC char *ics_fname(ics_msg *msg, int index);
DECLSPEC char *ics_fgetbyname(ics_msg *msg, char *name);
DECLSPEC int ics_fcount(ics_msg *msg);
DECLSPEC void ics_destroy(ics_msg *msg);
DECLSPEC void ics_set_config_file(ics_msg *request, char *filename);
DECLSPEC cert_buffer *sendCertRequest( unsigned char *request, size_t requestLen, 
			      int update, unsigned char *senderName, 
			      unsigned char *serverHost, 
			      unsigned char *serverPort,
			      char *serverCert, char *merchantCert, 
			      char *merchantPrivateKey, char *proxyuser,
			      char *proxypwd, char *proxyurl, char *hostName );
DECLSPEC buffer *generateCertRequest( char *merchant, char *keysPath );
DECLSPEC void base64_encode(char *input, int length,char *output);
DECLSPEC void base64_encode_wonl(char *input, size_t length,char *output);
DECLSPEC int base64_decode(char *s, void *data);
DECLSPEC void ics_open_debug_file(char *fn);
DECLSPEC void ics_close_debug_file(void);
DECLSPEC int ics_get_mask_type(int isOfferField, char *name);

extern char *(*get_secret_key_func)(char *id, size_t *size, void *);

#ifdef __cplusplus
}
#endif

#endif /* _ICS_H */

/* Copyright 1996, all rights reserved, CyberSource Corporation.        */
