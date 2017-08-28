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
#include <map>

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

NVPCYBERSOURCE_API int runTransaction(INVPTransactionProcessorProxy *proxy, CybsMap *configMap, std::map <std::wstring, std::wstring> request, std::map <std::wstring, std::wstring> &resMap);
int getKeyFilePath (char szDest[], char *szDir, const char *szFilename, char *merhantID);
