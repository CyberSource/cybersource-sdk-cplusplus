// XMLClient.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <sstream>
#include "soapITransactionProcessorProxy.h"
#include "ITransactionProcessor.nsmap"
#include "stdsoap2.h"
#include "XMLCybersource.h"
#include "util.h"
#include "wsseapi.h"
#include <openssl/pkcs12.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#ifdef WIN32
	#include "..\NVPClient\log.h"
#else
	#include "../NVPClient/log.h"
#endif

#include <libxml/xmlmemory.h>
#include "plugin.h"

#ifdef WIN32
static HANDLE *lock_cs;
#endif /* WIN32 */

void cybs_openssl_init(void);
void cybs_openssl_cleanup(void);
void win32_locking_callback(int mode, int type, const char *file, int line);
void gsoapCleanup(ITransactionProcessorProxy proxy);
void opensslCleanup (EVP_PKEY *pkey1, X509 *cert1, STACK_OF(X509) *ca);
void pthreads_locking_callback(int mode,int type,const char *file,int line);
unsigned long pthreads_thread_id(void );

#pragma comment(lib, "BASECLIENT.lib")

	char DEFAULT_CERT_FILE[]        =    "ca-bundle";
	char SERVER_PUBLIC_KEY_NAME[]   =    "CyberSource_SJC_US/serialNumber";

	const char  casserver[]         =    "https://ics2wstest.ic3.com/commerce/1.x/transactionProcessor";
	const char  prodserver[]        =    "https://ics2ws.ic3.com/commerce/1.x/transactionProcessor";
	const char  akamaiProdserver[]  =    "https://ics2wsa.ic3.com/commerce/1.x/transactionProcessor";
	const char  akamaiCasserver[]   =    "https://ics2wstesta.ic3.com/commerce/1.x/transactionProcessor";

#define RETURN_ERROR( status, info ) \
{ \
	cybs_add( configMap, CYBS_SK_ERROR_INFO, info ); \
	if (cfg.isLogEnabled) cybs_log( cfg, CYBS_LT_ERROR, info ); \
	return( status ); \
}

#define RETURN_SUCCESS( status, info ) \
{ \
	cybs_add( configMap, CYBS_SK_ERROR_INFO, info ); \
	if (cfg.isLogEnabled) cybs_log( cfg, CYBS_LT_SUCCESS, info ); \
	return( status ); \
}

#define RETURN_ERROR1( status, info, arg1 ) \
{ \
	char szErrorBuf[128]; \
	sprintf( szErrorBuf, info, arg1 ); \
	RETURN_ERROR( status, szErrorBuf ); \
}

#define RETURN_ERROR2( status, info, arg1, arg2 ) \
{ \
	char szErrorBuf[128]; \
	sprintf( szErrorBuf, info, arg1, arg2 ); \
	RETURN_ERROR( status, szErrorBuf ); \
}

#define RETURN_LENGTH_ERROR( name, maxlen ) RETURN_ERROR2( CYBS_S_PRE_SEND_ERROR, "%s cannot exceed %d characters.", name, maxlen )

#define RETURN_REQUIRED_ERROR( name ) RETURN_ERROR1( CYBS_S_PRE_SEND_ERROR, "%s is required.", name )

#define CHECK_LENGTH( name, maxlen, value ) \
	if (strlen( value ) > maxlen) \
		RETURN_LENGTH_ERROR( name, maxlen );

#ifdef WIN32
void cybs_openssl_init(void)
{
	int i;
	OpenSSL_add_all_algorithms();
	//ERR_load_crypto_strings();
	lock_cs=(HANDLE *) OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
	for (i=0; i<CRYPTO_num_locks(); i++)
		{
		lock_cs[i]=CreateMutex(NULL,FALSE,NULL);
		}

	CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))win32_locking_callback);
	/* id callback defined */
}

void cybs_openssl_cleanup(void)
{
	int i;

	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++)
		CloseHandle(lock_cs[i]);
	OPENSSL_free(lock_cs);
}

void win32_locking_callback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK)
		{
		WaitForSingleObject(lock_cs[type],INFINITE);
		}
	else
		{
		ReleaseMutex(lock_cs[type]);
		}
}
#endif /* WIN32 */

#ifdef PTHREADS

static pthread_mutex_t *lock_cs;
static long *lock_count;

void cybs_openssl_init(void)
	{
	int i;

	lock_cs= (pthread_mutex_t *) OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
	lock_count= (long *) OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));
	for (i=0; i<CRYPTO_num_locks(); i++)
		{
		lock_count[i]=0;
		pthread_mutex_init(&(lock_cs[i]),NULL);
		}

	CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
	/* CRYPTO_set_locking_callback((void (*)())pthreads_locking_callback); */
        CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))pthreads_locking_callback);
	}

void cybs_openssl_cleanup(void)
	{
	int i;

	CRYPTO_set_locking_callback(NULL);
	/* fprintf(stderr,"cleanup\n"); */
	for (i=0; i<CRYPTO_num_locks(); i++)
		{
		pthread_mutex_destroy(&(lock_cs[i]));
		/* fprintf(stderr,"%8ld:%s\n",lock_count[i],
			CRYPTO_get_lock_name(i)); */
		}
	OPENSSL_free(lock_cs);
	OPENSSL_free(lock_count);

	/* fprintf(stderr,"done cleanup\n"); */
	}

void pthreads_locking_callback(int mode, int type, const char *file,
	     int line)
      {
#ifdef undef
	fprintf(stderr,"thread=%4d mode=%s lock=%s %s:%d\n",
		CRYPTO_thread_id(),
		(mode&CRYPTO_LOCK)?"l":"u",
		(type&CRYPTO_READ)?"r":"w",file,line);
#endif

	if (mode & CRYPTO_LOCK)
		{
		pthread_mutex_lock(&(lock_cs[type]));
		lock_count[type]++;
		}
	else
		{
		pthread_mutex_unlock(&(lock_cs[type]));
		}
	}

unsigned long pthreads_thread_id(void)
	{
	unsigned long ret;

	ret=(unsigned long)pthread_self();
	return(ret);
	}

unsigned long cybs_get_thread_id(void)
{
	return( pthreads_thread_id() );
}

#endif /* PTHREADS */

class CYBSCPP_BEGIN_END
{
public:
	CYBSCPP_BEGIN_END()
	{
		/* This is a place for any one-time initializations needed by
		   the client. */
		cybs_openssl_init();
	}

	~CYBSCPP_BEGIN_END()
	{
		/* This is a place for any one-time cleanup tasks. */
		cybs_openssl_cleanup();
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		xmlCleanupParser();
		
	}
};

CYBSCPP_BEGIN_END gCybsBeginEnd;

char cybs_flag_value( const char *szFlagString )
{
	// TODO:  document this as case-sensitive.  stricmp is not on linux.
	return(
		(!strcmp( szFlagString, "true" ) ||
		 !strcmp( szFlagString, "1" ) ) ? 1 : 0 );
}

int configure (ITransactionProcessorProxy **proxy, config cfg,  PKCS12 **p12, EVP_PKEY **pkey1, X509 **cert1, STACK_OF(X509) **ca) 
{
	char *sslCertFile = cfg.sslCertFile;

	soap_ssl_init();
	soap_register_plugin((*proxy)->soap, soap_wsse);

	FILE *fp;

	if (!(fp = fopen(cfg.keyFile, "rb"))) {
		return ( 1 );
	}

	*p12 = d2i_PKCS12_fp(fp, NULL);
	fclose(fp);

	if (!p12) {
		return ( 1 );
		ERR_print_errors_fp(stderr);
	}

	if (!PKCS12_parse(*p12, cfg.password, pkey1, cert1, ca)) {
		return ( 2 );
		ERR_print_errors_fp(stderr);
	}

	PKCS12_free(*p12);

	/****Read pkcs12 completed*************/

	/****Set up configuration for signing the request*************/

	/* Decalre tags that will have wsu id */
	soap_wsse_set_wsu_id((*proxy)->soap, "wsse:BinarySecurityToken SOAP-ENV:Body");
	
	if ( soap_wsse_add_BinarySecurityTokenX509((*proxy)->soap, "X509Token", *cert1 )
	|| soap_wsse_add_KeyInfo_SecurityTokenReferenceX509((*proxy)->soap, "#X509Token")
	|| soap_wsse_sign_body((*proxy)->soap, SOAP_SMD_SIGN_RSA_SHA256, *pkey1, 0)
	|| soap_wsse_sign_only((*proxy)->soap, "SOAP-ENV:Body") ) {
		return ( 3 );
	}

	char *token1, *token2;
	if ( cfg.isEncryptionEnabled ) {
		for (int i = 0; i < sk_X509_num(*ca); i++) {

			#ifdef WIN32
			for (token1 = strtok_s(sk_X509_value(*ca, i)->name, "=", &token2); token1; token1 = strtok_s(NULL, "=", &token2))
			{
				if (strcmp(SERVER_PUBLIC_KEY_NAME, token1) == 0)
					if (soap_wsse_add_EncryptedKey((*proxy)->soap, SOAP_MEC_AES256_CBC, "Cert", sk_X509_value(*ca, i), NULL, NULL, NULL)) {
						return ( 4 );
					}
			}
		#else

			for (token1 = strtok_r(sk_X509_value(*ca, i)->name, "=", &token2); token1; token1 = strtok_r(NULL, "=", &token2))
			{
				if (strcmp(SERVER_PUBLIC_KEY_NAME, token1) == 0)
					if (soap_wsse_add_EncryptedKey((*proxy)->soap, SOAP_MEC_AES256_CBC, "Cert", sk_X509_value(*ca, i), NULL, NULL, NULL)) {
						return ( 4 );
					}
			}
		#endif
		}
	}

	/****Set up configuration for signing the request ends*************/

	/****Set up SSL context to enable SSL*******	*****/
	if (soap_ssl_client_context((*proxy)->soap, SOAP_SSL_SKIP_HOST_CHECK, NULL, NULL, 
		cfg.sslCertFile, NULL, NULL ))
	{
		return ( 5 );
	}

	return ( 0 );
}

int cybs_runTransaction(ITransactionProcessorProxy *proxy, ns2__RequestMessage *ns2__requestMessage, ns2__ReplyMessage **ns2__replyMessage, CybsMap *configMap) {
	char szDest[256];
	char *mercID = (char *)cybs_get(configMap, CYBS_C_MERCHANT_ID);
	char *keyDir = (char *)cybs_get(configMap, CYBS_C_KEYS_DIRECTORY);

	const char *temp;

	PKCS12 *p12 = NULL;
	EVP_PKEY *pkey1 = NULL;
	X509 *cert1 = NULL;
	STACK_OF(X509) *ca = NULL;
	
	config cfg;
	memset(&cfg, '\0', sizeof (cfg));

	temp = (const char *)cybs_get(configMap, CYBS_C_ENABLE_LOG);
	if (temp)
		cfg.isLogEnabled = cybs_flag_value(temp);

	/* If log enabled then prepare log file */
	if (cfg.isLogEnabled) {
		// Log File Name
		temp = (const char *)cybs_get(configMap, CYBS_C_LOG_FILENAME);
		if (!temp)
			temp = DEFAULT_LOG_FILENAME;
		strcpy(cfg.logFileName, temp);
		CHECK_LENGTH(CYBS_C_LOG_FILENAME, CYBS_MAX_PATH, cfg.logFileName);

		// Log File Directory
		temp = (const char *)cybs_get(configMap, CYBS_C_LOG_DIRECTORY);
		if (!temp)
			temp = DEFAULT_LOG_DIRECTORY;
		strcpy(cfg.logFileDir, temp);
		CHECK_LENGTH(CYBS_C_LOG_DIRECTORY, CYBS_MAX_PATH, cfg.logFileDir);

		// Get complete log path
		if(getKeyFilePath (szDest, cfg.logFileDir, cfg.logFileName, "") == -1) 
	    {
		RETURN_LENGTH_ERROR(CYBS_C_KEYS_DIRECTORY, CYBS_MAX_PATH);
	    }
		
		temp = (const char *)cybs_get(configMap, CYBS_C_LOG_MAXIMUM_SIZE);
		if (temp)
			cfg.nLogMaxSizeInMB = atoi(temp);
		else
			cfg.nLogMaxSizeInMB = atoi(DEFAULT_LOG_MAX_SIZE);

		strcpy(cfg.logFilePath, szDest);
		CybsLogError nLogError = cybs_prepare_log (cfg);

		if (nLogError != CYBS_LE_OK) {
			RETURN_ERROR (CYBS_S_PRE_SEND_ERROR, (char *) cybs_get_log_error( nLogError ));
		}
	}

	/* Config data is mandatory */
	if (configMap->length == 0)
	{
		RETURN_REQUIRED_ERROR( "Parameter config" );
	}

	/* Request should be present */
	if (!ns2__requestMessage) 
	{
		RETURN_REQUIRED_ERROR( "Parameter pRequest" );
	}

	/* Key directory is mandatory */
	temp = (const char *)cybs_get(configMap, CYBS_C_KEYS_DIRECTORY);
    if (!temp) 
	{
		RETURN_REQUIRED_ERROR( CYBS_C_KEYS_DIRECTORY );
	}

	/* Check if merchant id is present in request, if not present then 
	check config file. If not present in both the location then return 
	with error */

	temp = (const char *)cybs_get(configMap, CYBS_C_MERCHANT_ID);
	std::string merchantID;

	if (!ns2__requestMessage->merchantID) 
	{
		if (!(char *)cybs_get(configMap, CYBS_C_MERCHANT_ID)) 
		{
			RETURN_REQUIRED_ERROR( CYBS_C_MERCHANT_ID );
		}
		merchantID = temp;
		wchar_t *w = NULL;
		soap_s2wchar(proxy->soap, merchantID.c_str(), &w, -1, -1, NULL);
		ns2__requestMessage->merchantID = w;
	}

	strcpy(cfg.merchantID, soap_wchar2s(proxy->soap, ns2__requestMessage->merchantID));

	/* Check whether ssl cert file path is present in config file if not
	then check in key directory with default config file name (ca-bundle) */

	temp = (const char *)cybs_get(configMap, CYBS_C_SSL_CERT_FILE);
	if (temp)
	{
		CHECK_LENGTH(
			CYBS_C_SSL_CERT_FILE, CYBS_MAX_PATH, temp );
		strcpy(cfg.sslCertFile, temp);
	}
	else
	{
		if ( getKeyFilePath (szDest, keyDir, DEFAULT_CERT_FILE, ".crt" ) == -1 ) 
		{
			RETURN_LENGTH_ERROR(CYBS_C_SSL_CERT_FILE, CYBS_MAX_PATH);
		}
		temp = szDest;
		strcpy(cfg.sslCertFile, temp);
		strcat(cfg.sslCertFile, ".crt");
	}

	/* Check if key file name is present in config file if not then use
	merchant-id.p12 as default file name */

	temp = (char *)cybs_get(configMap, CYBS_C_KEY_FILENAME);
	if (!temp)
	{
		temp = cfg.merchantID;
		strcpy(cfg.keyFileName, temp);
		strcat(cfg.keyFileName, ".p12");
	} else {
		strcpy(cfg.keyFileName, temp);
	}
	
	/* Get effective key file path */
	if(getKeyFilePath (szDest, keyDir, cfg.keyFileName, ".p12") == -1) 
	{
		RETURN_LENGTH_ERROR(CYBS_C_KEYS_DIRECTORY, CYBS_MAX_PATH);
	}	 
	strcpy(cfg.keyFile, szDest);

	/* Check if password is present in config file if not then use merchant
	id as the password */

	temp = (const char *)cybs_get(configMap, CYBS_C_PWD);
	if (!temp)
	{
		temp = cfg.merchantID;
	}
	CHECK_LENGTH(CYBS_C_PWD, CYBS_MAX_PASSWORD, cfg.password);
	strcpy(cfg.password, temp);

	/* Get proxy port from config file */
	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_PORT);
	if (temp) {
		cfg.proxyPort = atoi(temp);
		proxy->soap->proxy_port = cfg.proxyPort;
	}

	/* Get proxy server from config file */
	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_SERVER);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_SERVER, CYBS_MAX_URL, temp);
		strcpy(cfg.proxyServer, temp);
		proxy->soap->proxy_host = cfg.proxyServer;
	}

	/* Get proxy password from config file */
	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_PWD);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_PWD, CYBS_MAX_PASSWORD, temp);
		strcpy(cfg.proxyPassword, temp);
		proxy->soap->proxy_passwd = cfg.proxyPassword;
	}

	/* Get proxy username from config file */
	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_USERNAME);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_USERNAME, CYBS_MAX_PASSWORD, temp);
		strcpy(cfg.proxyUsername, temp);
		proxy->soap->proxy_userid = cfg.proxyUsername;
	}

	/* if prod flag is true then use production url,
	else check config file for server url other wise use default url
	to send the request */

	char *prodFlag = (char *)cybs_get(configMap, CYBS_C_SEND_TO_PROD);

	temp = (const char *)cybs_get(configMap, CYBS_C_USE_AKAMAI);
	if (temp)
		cfg.useAkamai = cybs_flag_value(temp);

	if (!prodFlag) {
	   prodFlag = "false";
	}

	temp = (char *)cybs_get(configMap, CYBS_C_SERVER_URL);

	if ( temp ) {
		CHECK_LENGTH(CYBS_C_SERVER_URL, CYBS_MAX_URL, temp);
		strcpy(cfg.serverURL, temp);
	} else {
		if ( strcmp(prodFlag, "true") == 0 ) {
			if ( cfg.useAkamai ) {
				strcpy(cfg.serverURL, akamaiProdserver);
			} else {
				strcpy(cfg.serverURL, prodserver);
			}
		} else {
			if ( cfg.useAkamai ) {
				strcpy(cfg.serverURL, akamaiCasserver);
			} else {
				strcpy(cfg.serverURL, casserver);
			}
		}
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_USE_SIGN_AND_ENCRYPTION);
	if (temp)
		cfg.isEncryptionEnabled = cybs_flag_value(temp);

	//ITransactionProcessorProxy proxy = ITransactionProcessorProxy ();
	//int errFlag = configure(&proxy, cfg, &p12, &pkey1, &cert1, &ca);
	int errFlag = configure(&proxy, cfg, &p12, &pkey1, &cert1, &ca);

	if ( errFlag != 0 )
	{
		switch ( errFlag ) {
			case 1 :
				opensslCleanup(pkey1, cert1, ca);
				RETURN_ERROR1(CYBS_S_PRE_SEND_ERROR, "Failed to open merchant certificate file %s",  (char *)cfg.keyFile);
			break;

			case 2 :
				opensslCleanup(pkey1, cert1, ca);
				RETURN_ERROR1(CYBS_S_PRE_SEND_ERROR, "Failed to parse merchant certificate check password %s",  (char *)cfg.keyFile);
			break;

			case 3 :
				opensslCleanup(pkey1, cert1, ca);
				RETURN_ERROR1(CYBS_S_PRE_SEND_ERROR, "Failed to set security signature configuration %s",  "");
			break;

			case 4 :
				opensslCleanup(pkey1, cert1, ca);
				RETURN_ERROR1(CYBS_S_PRE_SEND_ERROR, "Failed to set encryption configuration %s",  "");
			break;

			case 5 :
				opensslCleanup(pkey1, cert1, ca);
				RETURN_ERROR1(CYBS_S_PRE_SEND_ERROR, "Failed to set SSL context %s",  "");
			break;
		}
		
	}

	// Set client library version in request
	//std::string clientLibVersion (CLIENT_LIBRARY_VERSION_VALUE);
	ns2__requestMessage->clientLibraryVersion = const_cast< wchar_t* >(CLIENT_LIBRARY_VERSION_VALUE);

	/* converting ns2__requestMessage to xml */
	std::stringstream ss;
	proxy->soap->os = &ss;
	soap_write_ns2__RequestMessage(proxy->soap, ns2__requestMessage);
	proxy->soap->os = NULL;
	
	/* Log request */
	if (cfg.isLogEnabled)
		cybs_log_xml(cfg, CYBS_LT_REQUEST, (char *)ss.str().c_str());

	proxy->soap_endpoint = cfg.serverURL;

	if (cfg.isLogEnabled)
		cybs_log (cfg, CYBS_LT_CONFIG, proxy->soap_endpoint);

	int status = proxy->runTransaction(ns2__requestMessage, **ns2__replyMessage);

	opensslCleanup(pkey1, cert1, ca);

	char *responseMsg = proxy->soap->msgbuf;

	if (status == SOAP_OK) {
		if (cfg.isLogEnabled)
		cybs_log( cfg, CYBS_LT_SUCCESS, responseMsg );

		/* converting ns2__replyMessage to xml */
		ss.str(std::string());
		proxy->soap->os = &ss;
		soap_write_ns2__ReplyMessage(proxy->soap, *ns2__replyMessage);
		proxy->soap->os = NULL;

			/* Log request */
		if (cfg.isLogEnabled)
			cybs_log_xml(cfg, CYBS_LT_REPLY, (char *)ss.str().c_str());
	} else {
		const char *faultString = proxy->soap_fault_string();

		if (faultString) {
			if (cfg.isLogEnabled)
				cybs_log( cfg, CYBS_LT_ERROR, faultString );
		}
	}

	return status;
}

/* Create effective path */
int getKeyFilePath (char szDest[], char *szDir, const char *szFilename, char *ext) {
	
	int nDirLen = strlen( szDir );
	char fAddSeparator = szDir[nDirLen - 1] == DIR_SEPARATOR ? 0 : 1;
	if (nDirLen + fAddSeparator + strlen( szFilename ) + strlen(ext) >
		CYBS_MAX_PATH)
	{
		return( -1 );
	}

	strcpy( szDest, szDir );
	if (fAddSeparator)
	{
		szDest[nDirLen] = DIR_SEPARATOR;
		szDest[nDirLen + 1] = '\0';
	}
	//strcat( szDest, strcat((char *)szFilename, ext ));
	strcat( szDest, szFilename );
	return( 0 );
	
}

/* Function to deserialze requestxml to request object*/
void cybs_add_request(char *xml, ns2__RequestMessage *req) {
	struct soap *ctx = soap_new();
	//struct ns2__RequestMessage req;
	std::istringstream iss;
	iss.str (xml);
	ctx->is = &iss;
	soap_read_ns2__RequestMessage(ctx, req);
	soap_destroy(ctx);
	free(ctx);
	//return req;
}

void opensslCleanup (EVP_PKEY *pkey1, X509 *cert1, STACK_OF(X509) *ca) {
	sk_X509_pop_free(ca, X509_free);
	X509_free(cert1);
	EVP_PKEY_free(pkey1);
}
