#pragma once

#ifdef XMLCYBERSOURCE_EXPORTS
#define XMLCYBERSOURCE_API __declspec(dllexport) 
#else
#define XMLCYBERSOURCE_API 
#endif

#include <iostream>
#include "soapITransactionProcessorProxy.h"
#include "stdsoap2.h"
#include "util.h"

#ifdef WIN32
static const char DEFAULT_LOG_DIRECTORY[] = "\\"; /* root of current directory */
#else
static const char DEFAULT_LOG_DIRECTORY[] = "/";  /* root directory */
#endif
static const char DEFAULT_LOG_FILENAME[] = "cybs.log";
static const char DEFAULT_LOG_MAX_SIZE[] = "10";

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

#ifdef WIN32
	static const char DIR_SEPARATOR = '\\';
#else
	static const char DIR_SEPARATOR = '/';
#endif

XMLCYBERSOURCE_API int cybs_runTransaction(ITransactionProcessorProxy *proxy, ns2__RequestMessage *ns2__requestMessage, ns2__ReplyMessage **ns2__replyMessage, CybsMap *configMap);
XMLCYBERSOURCE_API void cybs_add_request(char *xml, ns2__RequestMessage *req);
int getKeyFilePath (char szDest[], char *szDir, const char *szFilename, char *merhantID);