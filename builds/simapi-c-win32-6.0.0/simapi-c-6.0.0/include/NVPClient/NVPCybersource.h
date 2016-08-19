#pragma once

#ifdef NVPCYBERSOURCE_EXPORTS
#define NVPCYBERSOURCE_API __declspec(dllexport) 
#else
#define NVPCYBERSOURCE_API 
#endif

#include "util.h"
#include <iostream>
#include "soapINVPTransactionProcessorProxy.h"
#include "stdsoap2.h"
//#include "log.h"

/*const char CYBS_C_MERCHANT_ID[]             = "merchantID";
const char CYBS_C_KEYS_DIRECTORY[]          = "keysDirectory";
const char CYBS_C_TARGET_API_VERSION[]      = "targetAPIVersion";
const char CYBS_C_KEY_FILENAME[]            = "keyFilename";
const char CYBS_C_SERVER_URL[]              = "serverURL";
const char CYBS_C_NAMESPACE_URI[]           = "namespaceURI";
const char CYBS_C_PASSWORD[]                = "password";
const char CYBS_C_ENABLE_LOG[]              = "enableLog";
const char CYBS_C_LOG_DIRECTORY[]           = "logDirectory";
const char CYBS_C_LOG_FILENAME[]            = "logFilename";
const char CYBS_C_LOG_MAXIMUM_SIZE[]        = "logMaximumSize";
const char CYBS_C_TIMEOUT[]                 = "timeout";
const char CYBS_C_PROXY_SERVER[]            = "proxyServer";
const char CYBS_C_PROXY_PORT[]              = "proxyPort";
const char CYBS_C_PROXY_USERNAME[]          = "proxyUsername";
const char CYBS_C_PROXY_PASSWORD[]          = "proxyPassword";
const char CYBS_C_SSL_CERT_FILE[]           = "sslCertFile";
const char CYBS_C_SEND_TO_PROD[]            = "sendToProduction";
const char CYBS_C_CLIENT_VERSION[]          = "sendToProduction";
const char CYBS_C_USE_SIGN_AND_ENCRYPTION[] = "useSignAndEncrypted";
const char CYBS_C_USE_AKAMAI[]              = "sendToAkamai";
const char CYBS_SK_ERROR_INFO[]             = "_errorInfo";*/

#ifdef WIN32
static const char DEFAULT_LOG_DIRECTORY[] = "\\"; /* root of current directory */
#else
static const char DEFAULT_LOG_DIRECTORY[] = "/";  /* root directory */
#endif
static const char DEFAULT_LOG_FILENAME[] = "cybs.log";
static const char DEFAULT_LOG_MAX_SIZE[] = "10";

/*#define CYBS_NUM_BUCKETS 256;

#define CYBS_MAX_NAMESPACE_URI	64
#define CYBS_MAX_MERCHANT_ID	32
#define CYBS_MAX_PASSWORD	32
#define CYBS_MAX_PATH		256
#define CYBS_MAX_URL		128
#define CYBS_MAX_PROXY_USERNAME	64
#define CYBS_MAX_PROXY_PASSWORD 64*/

typedef enum
{
	CYBS_S_OK = 0,
	CYBS_S_PRE_SEND_ERROR = 1,
	CYBS_S_SEND_ERROR = 2,
	CYBS_S_RECEIVE_ERROR = 3,
	CYBS_S_POST_RECEIVE_ERROR = 4,
	CYBS_S_CRITICAL_SERVER_FAULT = 5,
	CYBS_S_SERVER_FAULT = 6,
	CYBS_S_OTHER_FAULT = 7,
	CYBS_S_HTTP_ERROR = 8
} CybsStatus;

/*typedef struct config
{
	char merchantID[CYBS_MAX_MERCHANT_ID + 1];
	char keyFile[CYBS_MAX_PATH + 1];
	char keyFileName[CYBS_MAX_PATH + 1];
	char serverURL[CYBS_MAX_URL + 1];
	char password[CYBS_MAX_PASSWORD + 1];
	char proxyServer[CYBS_MAX_URL + 1];
	char proxyUsername[CYBS_MAX_PROXY_USERNAME + 1];
	char proxyPassword[CYBS_MAX_PROXY_PASSWORD + 1];
	char sslCertFile[CYBS_MAX_PATH + 1];
	char logFileDir[CYBS_MAX_PATH + 1];
	char logFileName[CYBS_MAX_PATH + 1];
	char logFilePath[CYBS_MAX_PATH + 1];
	int  nLogMaxSizeInMB;
	int proxyPort;
	char isLogEnabled;
} CybsEffectiveConfig;*/

#ifdef WIN32
	static const char DIR_SEPARATOR = '\\';
#else
	static const char DIR_SEPARATOR = '/';
#endif

NVPCYBERSOURCE_API int runTransaction(INVPTransactionProcessorProxy *proxy, CybsMap *configMap, CybsMap *reqMap, CybsMap *responseMap);
int getKeyFilePath (char szDest[], char *szDir, const char *szFilename, char *merhantID);
//NVPCYBERSOURCE_API KVSstore *load_config (const char *configFilename);