#pragma once

#ifdef BASECLIENT_EXPORTS
#define BASECLIENT_API __declspec(dllexport) 
#else
#define BASECLIENT_API 
#endif

#ifndef __BASECLIENT_H__
#define __BASECLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN64)
static const wchar_t CLIENT_ENVIRONMENT_VALUE[] = L"Win64";
#elif defined(_WIN32)
static const wchar_t CLIENT_ENVIRONMENT_VALUE[] = L"Win32";
#else
#if defined(__linux)
static const wchar_t CLIENT_ENVIRONMENT_VALUE[] = L"Linux";
#else
static const wchar_t CLIENT_ENVIRONMENT_VALUE[] = L"Unknown";
#endif
#endif


#include <stdlib.h>

	
#define CYBS_NUM_BUCKETS 256;

#define CYBS_MAX_NAMESPACE_URI	64
#define CYBS_MAX_MERCHANT_ID	32
#define CYBS_MAX_PASSWORD	32
#define CYBS_MAX_PATH		256
#define CYBS_MAX_URL		128
#define CYBS_MAX_PROXY_USERNAME	64
#define CYBS_MAX_PROXY_PASSWORD 64

const char CYBS_C_MERCHANT_ID[]             = "merchantID";
const char CYBS_C_KEYS_DIRECTORY[]          = "keysDirectory";
const char CYBS_C_TARGET_API_VERSION[]      = "targetAPIVersion";
const char CYBS_C_KEY_FILENAME[]            = "keyFilename";
const char CYBS_C_SERVER_URL[]              = "serverURL";
const char CYBS_C_NAMESPACE_URI[]           = "namespaceURI";
const char CYBS_C_PWD[]                     = "password";
const char CYBS_C_ENABLE_LOG[]              = "enableLog";
const char CYBS_C_LOG_DIRECTORY[]           = "logDirectory";
const char CYBS_C_LOG_FILENAME[]            = "logFilename";
const char CYBS_C_LOG_MAXIMUM_SIZE[]        = "logMaximumSize";
const char CYBS_C_TIMEOUT[]                 = "timeout";
const char CYBS_C_PROXY_SERVER[]            = "proxyServer";
const char CYBS_C_PROXY_PORT[]              = "proxyPort";
const char CYBS_C_PROXY_USERNAME[]          = "proxyUsername";
const char CYBS_C_PROXY_PWD[]               = "proxyPassword";
const char CYBS_C_SSL_CERT_FILE[]           = "sslCertFile";
const char CYBS_C_SEND_TO_PROD[]            = "sendToProduction";
const char CYBS_C_USE_SIGN_AND_ENCRYPTION[] = "useSignAndEncrypted";
const char CYBS_SK_ERROR_INFO[]             = "_errorInfo";
const char CYBS_C_USE_AKAMAI[]              = "sendToAkamai";


/* clientLibraryVersion */
static const wchar_t CLIENT_LIBRARY_VERSION_VALUE[] = L"6.0.1";
static const wchar_t CLIENT_LIBRARY_VALUE[] = L"C SOAP";
static const wchar_t CLIENT_APPLICATION_VALUE[] = L"Simple Order API";

typedef struct {
    const void *key;
    void *value;
} CybsTable;

typedef struct {
    CybsTable *pairs;
    size_t length;
	size_t totallength;
} CybsMap;

typedef struct config
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
	char isEncryptionEnabled;
	char useAkamai;
} CybsEffectiveConfig;

BASECLIENT_API CybsMap *cybs_create_map(void);

BASECLIENT_API void cybs_destroy_map(CybsMap *store);

BASECLIENT_API void cybs_add(CybsMap *store, const void *key, void *value);

BASECLIENT_API void *cybs_get(CybsMap *store, const void *key);

BASECLIENT_API void cybs_load_config (const char *configFilename, CybsMap *map);

void printdata(CybsMap *store);

#ifdef __cplusplus
}
#endif

#endif /* #define __KVS_H__ */

	
