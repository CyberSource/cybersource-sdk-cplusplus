// NVPClient.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "soapINVPTransactionProcessorProxy.h"
#include "INVPTransactionProcessor.nsmap"
#include "NVPCybersource.h"
#include "util.h"
#include "wsseapi.h"
#include <openssl/pkcs12.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include <map>

#ifdef WIN32
static HANDLE *lock_cs;
#endif /* WIN32 */

void cybs_openssl_init(void);
void cybs_openssl_cleanup(void);
void win32_locking_callback(int mode, int type, const char *file, int line);
void gsoapCleanup(INVPTransactionProcessorProxy proxy);
void opensslCleanup (EVP_PKEY *pkey1, X509 *cert1, STACK_OF(X509) *ca);
void pthreads_locking_callback(int mode,int type,const char *file,int line);
unsigned long pthreads_thread_id(void );

#pragma comment(lib, "BASECLIENT.lib")

	const char  casserver[]         =    "https://ics2wstest.ic3.com/commerce/1.x/transactionProcessor";
	const char  prodserver[]        =    "https://ics2ws.ic3.com/commerce/1.x/transactionProcessor";
	const char  akamaiProdserver[]  =    "https://ics2wsa.ic3.com/commerce/1.x/transactionProcessor";
	const char  akamaiCasserver[]   =    "https://ics2wstesta.ic3.com/commerce/1.x/transactionProcessor";

char DEFAULT_CERT_FILE[]   = "ca-bundle";
char SERVER_PUBLIC_KEY_NAME[]  =    "CyberSource_SJC_US/serialNumber";
static const wchar_t CLIENT_LIBRARY_VERSION[] = L"clientLibraryVersion";
static const wchar_t CLIENT_LIBRARY[] = L"clientLibrary";
static const wchar_t CLIENT_ENVIRONMENT[] = L"clientEnvironment";
static const wchar_t CLIENT_APPLICATION[] = L"clientApplication";

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
		
	}
};

CYBSCPP_BEGIN_END gCybsBeginEnd;

static std::wstring convertMaptoString (std::map <std::wstring, std::wstring> m) {
	int i = 0;
	std::wstring reqString;
	typedef std::map <std::wstring, std::wstring>::const_iterator it_type;
	for(it_type iterator = m.begin(); iterator != m.end(); iterator++) {
		reqString.append(std::wstring(iterator->first.begin(), iterator->first.end()));
		reqString.append(L"=");
		reqString.append(iterator->second);
		reqString.append(L"\n");
	}
	return reqString;
}

std::map <std::wstring, std::wstring> convertStringtoMap (wchar_t *res) {
	std::map <std::wstring, std::wstring> resmap;
	std::wstring strResp = std::wstring(res);
	wchar_t *res1 =  const_cast< wchar_t* > (strResp.c_str());
	wchar_t *temp1;
	wchar_t *token;
	wchar_t *buf;
	#ifdef WIN32
		temp1 = wcstok(res1, L"\n");
	#else
		temp1 = wcstok(res1, L"\n", &buf);
	#endif

	while(temp1 != NULL) 
	{	
		token = wcschr(temp1, L'=');
		*token = L'\0';
		std::wstring ws(temp1);
		std::wstring temp2(token+1);
		resmap.insert(std::pair<std::wstring, std::wstring>(ws,temp2));

		#ifdef WIN32
			temp1 = wcstok(NULL, L"\n");
		#else
			temp1 = wcstok(NULL, L"\n", &buf);
		#endif
	}
	return resmap;
}

char cybs_flag_value( const char *szFlagString )
{
	// TODO:  document this as case-sensitive.  stricmp is not on linux.
	return(
		(!strcmp( szFlagString, "true" ) ||
		 !strcmp( szFlagString, "1" ) ) ? 1 : 0 );
}

int configure (INVPTransactionProcessorProxy **proxy, config cfg, PKCS12 **p12, EVP_PKEY **pkey1, X509 **cert1, STACK_OF(X509) **ca) 
{
	
	char *sslCertFile = cfg.sslCertFile;
	
	soap_ssl_init();
	soap_register_plugin((*proxy)->soap, soap_wsse);

	/****Read pkcs12*************/
	 FILE *fp;

	 if (!(fp = fopen(cfg.keyFile, "rb"))) {
		return ( 1 );
        fprintf(stderr, "Error opening file %s\n");
        //exit(1);
     }

	 *p12 = d2i_PKCS12_fp(fp, NULL);
	 fclose(fp);

	  if (!p12) {
		  return ( 1 );
		  ERR_print_errors_fp(stderr);
	  }

	  if (!PKCS12_parse(*p12, cfg.password, pkey1, cert1, ca)) {
		 //std::cout << "\nerror >>>>>" << getErrorInfo(kvsStore);
		 return ( 2 );
       // fprintf(stderr, "Error parsing PKCS#12 file\n");
        ERR_print_errors_fp(stderr);
	  }

	  PKCS12_free(*p12);
	 /****Read pkcs12 completed*************/
	  /****Set up configuration for signing the request*************/
   soap_wsse_set_wsu_id((*proxy)->soap, "wsse:BinarySecurityToken SOAP-ENV:Body");
   if (soap_wsse_add_BinarySecurityTokenX509((*proxy)->soap, "X509Token", *cert1)
 || soap_wsse_add_KeyInfo_SecurityTokenReferenceX509((*proxy)->soap, "#X509Token")
 || soap_wsse_sign_body((*proxy)->soap, SOAP_SMD_SIGN_RSA_SHA256, *pkey1, 0)
 || soap_wsse_sign_only((*proxy)->soap, "Body")) {
	 return ( 3 );
   }

	char *token1, *token2;
	if ( cfg.isEncryptionEnabled ) {
		for (int i = 0; i < sk_X509_num(*ca); i++) {
			//token1 = strchr(sk_X509_value(*ca, i)->name, '=');
			//token2 = strchr(token1, '=');

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

    if (soap_ssl_client_context((*proxy)->soap, SOAP_SSL_SKIP_HOST_CHECK, NULL, NULL, 
		cfg.sslCertFile, NULL, NULL ))
     {
		return ( 5 );
     }

	return ( 0 );
}

int runTransaction(INVPTransactionProcessorProxy *proxy, CybsMap *configMap, std::map <std::wstring, std::wstring> req, std::map <std::wstring, std::wstring> &resMap) {
	char szDest[256];
	char *mercID = (char *)cybs_get(configMap, CYBS_C_MERCHANT_ID);
	char *keyDir = (char *)cybs_get(configMap, CYBS_C_KEYS_DIRECTORY);

	PKCS12 *p12 = NULL;
	EVP_PKEY *pkey1 = NULL;
	X509 *cert1 = NULL;
	STACK_OF(X509) *ca = NULL;

	const char *temp;

	config cfg;
	memset(&cfg, '\0', sizeof (cfg));

	temp = (const char *)cybs_get(configMap, CYBS_C_ENABLE_LOG);
	if (temp)
		cfg.isLogEnabled = cybs_flag_value(temp);

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

	if (configMap->length == 0)
	{
		RETURN_REQUIRED_ERROR( "Parameter config" );
	}

	if (req.empty()) 
	{
		RETURN_REQUIRED_ERROR( "Parameter Request" );
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_KEYS_DIRECTORY);
    if (!temp) 
	{
		RETURN_REQUIRED_ERROR( CYBS_C_KEYS_DIRECTORY );
	}

	if (req.find(L"merchantID") == req.end()) {
		if (!mercID) {
			RETURN_REQUIRED_ERROR( CYBS_C_MERCHANT_ID );
		}

		// convert char to wchar and put it in request
		wchar_t *w = NULL;
		soap_s2wchar(proxy->soap, mercID, &w, -1, -1, NULL);
		req[L"merchantID"] = w;
	}
	wcstombs(cfg.merchantID, req.find(L"merchantID")->second.c_str(), (req.find(L"merchantID")->second).length() + 1);

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

	temp = (char *)cybs_get(configMap, CYBS_C_KEY_FILENAME);
	if (!temp)
	{
		temp = cfg.merchantID;
		strcpy(cfg.keyFileName, temp);
		strcat(cfg.keyFileName, ".p12");
	} else {
		strcpy(cfg.keyFileName, temp);
	}
	
	if(getKeyFilePath (szDest, keyDir, cfg.keyFileName, ".p12") == -1) 
	{
		RETURN_LENGTH_ERROR(CYBS_C_KEYS_DIRECTORY, CYBS_MAX_PATH);
	}	 
	strcpy(cfg.keyFile, szDest);

	temp = (const char *)cybs_get(configMap, CYBS_C_PWD);
	if (!temp)
	{
		temp = cfg.merchantID;
	}
	CHECK_LENGTH(CYBS_C_PWD, CYBS_MAX_PASSWORD, cfg.password);
	strcpy(cfg.password, temp);
	
	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_PORT);

	if (temp) {
		cfg.proxyPort = atoi(temp);
		proxy->soap->proxy_port = cfg.proxyPort;
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_SERVER);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_SERVER, CYBS_MAX_URL, temp);
		strcpy(cfg.proxyServer, temp);
		proxy->soap->proxy_host = cfg.proxyServer;
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_PWD);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_PWD, CYBS_MAX_PASSWORD, temp);
		strcpy(cfg.proxyPassword, temp);
		proxy->soap->proxy_passwd = cfg.proxyPassword;
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_PROXY_USERNAME);
	if (temp) {
		CHECK_LENGTH(CYBS_C_PROXY_USERNAME, CYBS_MAX_PASSWORD, temp);
		strcpy(cfg.proxyUsername, temp);
		proxy->soap->proxy_userid = cfg.proxyUsername;
	}

	char *prodFlag = (char *)cybs_get(configMap, CYBS_C_SEND_TO_PROD);

	if (!prodFlag) {
	   prodFlag = "false";
	}

	temp = (const char *)cybs_get(configMap, CYBS_C_USE_AKAMAI);
	if (temp)
		cfg.useAkamai = cybs_flag_value(temp);

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

	req[CLIENT_LIBRARY_VERSION] = CLIENT_LIBRARY_VERSION_VALUE;

	req[CLIENT_LIBRARY] = CLIENT_LIBRARY_VALUE;

	req[CLIENT_ENVIRONMENT] = CLIENT_ENVIRONMENT_VALUE;

	req[CLIENT_APPLICATION] = CLIENT_APPLICATION_VALUE;


	if (cfg.isLogEnabled)
	  cybs_log_NVP(cfg, req, CYBS_LT_REQUEST);

	proxy->soap_endpoint = cfg.serverURL;
	
	if (cfg.isLogEnabled)
		cybs_log (cfg, CYBS_LT_CONFIG, proxy->soap_endpoint);

	//std:: string rep;
	
	wchar_t *rep;
	int status = proxy->runTransaction( const_cast< wchar_t* >(convertMaptoString (req).c_str()), rep );

	sk_X509_pop_free(ca, X509_free);
	X509_free(cert1);
	EVP_PKEY_free(pkey1);

	char *responseMsg = "\0";
	responseMsg = proxy->soap->msgbuf;
	
	if (status == SOAP_OK) {
		if(rep != NULL && wcslen(rep) > 0)
		resMap = convertStringtoMap(rep);
		
		if (cfg.isLogEnabled)
		cybs_log( cfg, CYBS_LT_SUCCESS, responseMsg );

		if (cfg.isLogEnabled)
		cybs_log_NVP(cfg, resMap, CYBS_LT_REPLY);
	} else {
		const char *faultString = proxy->soap_fault_string();

		if (faultString) {
			if (cfg.isLogEnabled)
				cybs_log( cfg, CYBS_LT_ERROR, faultString );
		}
	}

	return status;
}

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
	strcat( szDest, szFilename);
	return( 0 );
	
}

void opensslCleanup (EVP_PKEY *pkey1, X509 *cert1, STACK_OF(X509) *ca) {
	sk_X509_pop_free(ca, X509_free);
	X509_free(cert1);
	EVP_PKEY_free(pkey1);
}
