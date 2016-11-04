#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <map>

#ifndef WIN32
#include <time.h>
#endif

#include "log.h"
#ifdef WIN32
	#include "..\lib\gsoap-2.8\gsoap\plugin\threads.h"
#else
	#include "../lib/gsoap-2.8/gsoap/plugin/threads.h"
#endif


extern SafeFields gSafeFields;

static const long MB = 1048576;
static const char UNIX_NEWLINE = '\n';
static const char CONFIG_DELIM[] = ", ";
static const char TRANSTART_MARKER[] = "===============================================================================";
static const char NULL_ENTRY[] = "(null)";
static const char FILESTART_ENTRY[] = "CYBERSOURCE LOG FILE";
static const char TRANSTART_ENTRY[] = "";

static const char ERR_OK[]      = "No log error.";
static const char ERR_FOPEN[]   = "Failed to open log file during preparation.";
static const char ERR_FSEEK[]   = "Failed to seek to end of log file during preparation.";
static const char ERR_RENAME[]  = "Failed to archive log file.";
static const char ERR_UNKNOWN[] = "Unknown log error code.";

#ifdef WIN32
static const char ARCHIVE_TIMESTAMP[] = "%04u%02u%02uT%02u%02u%02u%03u"; /* 18 */
static const char ENTRY_TIMESTAMP[] = "%04u-%02u-%02u %02u:%02u:%02u.%03u"; /* 23 */
static const char NEWLINE[] = "\r\n";
#else
static const char ARCHIVE_TIMESTAMP[] = "%04u%02u%02uT%02u%02u%02u";
static const char ENTRY_TIMESTAMP[] = "%04u-%02u-%02u %02u:%02u:%02u";
static const char NEWLINE[] = "\n";
#endif

#define FORMATTED_TIME_LENGTH	32  /* more than enough actually */
#define NULL_STRING	(char *) 0
void *gCybsLogMutex;

#ifdef WIN32
static MUTEX_TYPE mutexLock;
#else
static MUTEX_TYPE mutexLock = PTHREAD_MUTEX_INITIALIZER;
#endif

/****************************************************************************/
/* PROTOTYPES OF INTERNAL FUNCTIONS                                         */
/****************************************************************************/

void get_formatted_time( const char *szFormat, char *szDest );
char *get_log_string (CybsMap *cfg, const char *szDelim, bool fMaskSensitiveData, SafeFields::MessageType eType);
static char *mask( const char *szField, const char *szValue );
static std::wstring mask( const std::wstring szField, const std::wstring szValue );
void read_doc( xmlNode *a_node, char *parentName, char *grandParent, SafeFields::MessageType eType);
char *cybs_strdup( const char * szStringToDup );

/****************************************************************************/
/* EXTERNAL FUNCTIONS                                                       */
/****************************************************************************/

/* Function to create log file and prepare it for logging */
CybsLogError cybs_prepare_log(config cfg)
{
	MUTEX_LOCK(mutexLock);

	FILE *pFile;
	long nSize;
	pFile = fopen( cfg.logFilePath, "a+" );
	if (!pFile)
	{
		MUTEX_UNLOCK(mutexLock);
		return( CYBS_LE_FOPEN );
	}

	if (fseek( pFile, 0, 2 ))
	{
		fclose( pFile );
		MUTEX_UNLOCK(mutexLock);
		return( CYBS_LE_FSEEK );
	}
	nSize = ftell( pFile );
	fclose( pFile );
	/* if size has exceeded the maximum, archive it */
	if (nSize > cfg.nLogMaxSizeInMB * MB)
	{
		char szFormattedTime[FORMATTED_TIME_LENGTH + 1];
		char szArchiveName[CYBS_MAX_PATH + 1];

		get_formatted_time( ARCHIVE_TIMESTAMP, szFormattedTime );
		sprintf(
			szArchiveName, "%s.%s", cfg.logFilePath, szFormattedTime );
		// TODO: test this, esp. on Linux.  Not sure if the new name has to include the path on Linux.  On Windows, it doesn't.
		if (rename( cfg.logFilePath, szArchiveName ))
		{
			MUTEX_UNLOCK(mutexLock);
			return( CYBS_LE_RENAME );
		}

		nSize = 0;
	}

	MUTEX_UNLOCK(mutexLock);
	if (nSize == 0)
	{
		cybs_log(cfg, 
			 CYBS_LT_FILESTART, FILESTART_ENTRY );
	}
	return( CYBS_LE_OK );
}

/* Function to log text in the log file*/
void cybs_log(
	config cfg, const char *szType, const char *szText )
{
	MUTEX_LOCK(mutexLock);
	//CybsEffectiveConfig& rEffConfig = pTransaction->effConfig;

	const char *szTextToLog = szText ? szText : NULL_ENTRY;

	FILE *pFile = fopen( cfg.logFilePath, "a" );
	if (pFile)
	{
		char szFormattedTime[FORMATTED_TIME_LENGTH + 1];

		if (!strcmp( szType, CYBS_LT_TRANSTART ))
		{
			fprintf( pFile, "%s%s%s", NEWLINE, TRANSTART_MARKER, NEWLINE );
		}

		get_formatted_time( ENTRY_TIMESTAMP, szFormattedTime );
		fprintf(
			pFile, "%s %4lu %s > %s%s%s",
			szFormattedTime, "1", szType,
			strchr( szTextToLog, UNIX_NEWLINE ) ? NEWLINE : "",
			szTextToLog, NEWLINE );

		// TODO: see how long the thread id's usually are on Linux and adjust the length in the fprint format accordingly.

		fclose( pFile );
	}
	MUTEX_UNLOCK(mutexLock);
}

void cybs_log(
	config cfg, const char *szType, const wchar_t *szText )
{
	MUTEX_LOCK(mutexLock);

	wchar_t *szTextToLog = szText ? const_cast< wchar_t* >(szText) : const_cast< wchar_t* >(L"(null)");

	FILE *pFile = fopen( cfg.logFilePath, "a" );
	if (pFile)
	{
		char szFormattedTime[FORMATTED_TIME_LENGTH + 1];

		if (!strcmp( szType, CYBS_LT_TRANSTART ))
		{
			fprintf( pFile, "%s%s%s", NEWLINE, TRANSTART_MARKER, NEWLINE );
		}

		get_formatted_time( ENTRY_TIMESTAMP, szFormattedTime );
		wchar_t *wszFormattedTime = new wchar_t[FORMATTED_TIME_LENGTH + 1];
		mbstowcs (wszFormattedTime, szFormattedTime, FORMATTED_TIME_LENGTH + 1);

		wchar_t *type = new wchar_t[10];
		mbstowcs (type, szType, 10);

		fwprintf(
			pFile, wszFormattedTime);
		
		fwprintf(
			pFile, L" " );

		fwprintf(
			pFile, type, L"\n");

		fwprintf(
			pFile, szTextToLog, L"\r\n" );

		// TODO: see how long the thread id's usually are on Linux and adjust the length in the fprint format accordingly.
		
		fclose( pFile );
		
		delete[] wszFormattedTime;
		delete[] type;
	}
	MUTEX_UNLOCK(mutexLock);
}

/* Function to log text in name value form */
void cybs_log_map(config config , CybsMap *cfg, const char *szType) {
	const char *szDelim;

	bool isRequest = strcmp( szType, CYBS_LT_REQUEST ) == 0;
	bool isReply = strcmp( szType, CYBS_LT_REPLY ) == 0;
	bool isConfig = strcmp( szType, CYBS_LT_CONFIG ) == 0;
	
	szDelim = (isConfig) ? CONFIG_DELIM : NEWLINE;
	char *szMapString = get_log_string (cfg, szDelim, isRequest || isReply, isRequest ? SafeFields::Request : SafeFields::Reply);
	cybs_log(config, szType, szMapString);
	if (szMapString)
	{
		free( szMapString );
	}

}

char *get_log_string (CybsMap *cfg, const char *szDelim, bool fMaskSensitiveData, SafeFields::MessageType eType) {
	if (cfg) {
		char *szMapString
			= (char *) malloc(
				/* total length of all names and values */
				cfg->totallength +  
				/* delimiters for each name-value pair */
				((strlen( "=" ) + strlen( szDelim )) * cfg->length) +
				/* null-terminator */
				1 );
		
		if (szMapString) {
		cybs_get_string(
			cfg, szMapString, szDelim,
			fMaskSensitiveData, eType, cfg->length );
		return( szMapString );
		}
	}
	return( NULL_STRING );
}

void cybs_get_string(
	CybsMap *map, char szBuffer[], const char *szDelim,
	bool fMaskSensitiveData, SafeFields::MessageType eType, int length ) {
		char fPrependDelim = 0;
		CybsTable pair;
		szBuffer[0] = '\0';
		int i;

		for (i = 0; i < length; i++) {
			if (fPrependDelim) {
				strcat( szBuffer, szDelim );
			}
			pair = map->pairs[i];
			fPrependDelim = 1;
			strcat( szBuffer, (char *)pair.key );
			strcat( szBuffer, "=" );

			if (fMaskSensitiveData &&
			    !gSafeFields.IsSafe( eType, (char *)pair.key ))
			{
				char *szMasked = mask((char*)pair.key, (char *)pair.value );
				strcat( szBuffer, szMasked );
				free( szMasked );
			} else {
				strcat( szBuffer, (char *)pair.value );
			}
		}
}

void cybs_log_NVP(config config, std::map <std::wstring, std::wstring> map, const char *szType) {
	std::wstring buffer;
	bool isRequest = strcmp( szType, CYBS_LT_REQUEST ) == 0;
	bool isReply = strcmp( szType, CYBS_LT_REPLY ) == 0;
	bool isConfig = strcmp( szType, CYBS_LT_CONFIG ) == 0;
	SafeFields::MessageType eType = isRequest ? SafeFields::Request : SafeFields::Reply;

	std::wstring szBuffer;
	
	typedef std::map <std::wstring, std::wstring>::const_iterator it_type;
	for(it_type iterator = map.begin(); iterator != map.end(); iterator++) {
		char *key = new char[iterator->first.length() + 1];
		memset( key, 0, iterator->first.length() + 1);
		wcstombs(key, iterator->first.c_str(), (iterator->first.length() + 1));
		szBuffer.append (iterator->first);
		szBuffer.append (L"=");
		if (!gSafeFields.IsSafe (eType, key)) {
			std::wstring szMasked = mask(iterator->first, iterator->second );
			szBuffer.append (szMasked);
			szBuffer.append (L"\n");
		} else {
			szBuffer.append (iterator->second);
			szBuffer.append (L"\n");
		}
		delete[] key;
	}
	szBuffer.append (L"\n");
	cybs_log (config, szType, szBuffer.c_str());

}

/* Function to log xml text */
void cybs_log_xml(config config, const char *szType, char *xmlString) {
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	xmlChar *temp = (xmlChar *)(xmlString);

	bool isRequest = strcmp( szType, CYBS_LT_REQUEST ) == 0;
	bool isReply = strcmp( szType, CYBS_LT_REPLY ) == 0;
	bool isConfig = strcmp( szType, CYBS_LT_CONFIG ) == 0;

	// parsing the xmlstring to xmldoc
	doc = xmlParseDoc(temp);

	if (doc == NULL) {
		cybs_log(config, CYBS_LT_ERROR, "Error while parsing xml for logging");
	}

	xmlNodePtr new_node = NULL;

	root_element = xmlDocGetRootElement(doc);
	read_doc (root_element, (char *)root_element->name, "", (isRequest || isReply == 0, isRequest ? SafeFields::Request : SafeFields::Reply));
	xmlChar *xmlbuff;
	int buffersize;
	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	cybs_log(config, szType, (const char *)xmlbuff);
	xmlFree(xmlbuff);
    xmlFreeDoc(doc);
	//xmlCleanupParser();

}

void read_doc (xmlNode *a_node, char *parentName, char *grandParent, SafeFields::MessageType eType) {
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_TEXT_NODE) {
			if (!isspace(((char *)cur_node->content)[0])) {
				char *tempParentName = 0, *tempGrandPrnt = 0;
				if (grandParent != NULL && strcmp(grandParent, "RequestMessage") != 0 && strcmp(grandParent, "ReplyMessage") != 0 ) {
					//tempGrandPrnt = cybs_strdup(grandParent);
					tempGrandPrnt = (char*) malloc(strlen(grandParent) +2 + strlen(parentName));

					memcpy(tempGrandPrnt, grandParent, strlen(grandParent));
					memcpy(tempGrandPrnt+strlen(grandParent), "_", 2);
					tempParentName = cybs_strdup(strcat(tempGrandPrnt,parentName));
				} else {
					tempParentName = cybs_strdup(parentName);
				}
				if (!gSafeFields.IsSafe( eType, tempParentName )) {
					//printf("not safe %s %s \n", tempParentName, cur_node->content);
					char *szMasked = mask( tempParentName, (const char *)cur_node->content );
					//printf("After Mask %s: \n", szMasked);
					strcpy((char *)cur_node->content, szMasked);
					//cur_node->content = (xmlChar *)szMasked;
					free(szMasked);
					//printf("Changed content %s: \n", cur_node->content);
				}
				if (tempGrandPrnt)
				free(tempGrandPrnt);
				free(tempParentName);
			}
		}
		read_doc(cur_node->children, (char *)cur_node->name, (char *)cur_node->parent->name, eType);
	}
}
static const char MASK_CHAR = 'x';
static const char TRACK_DATA[] = "trackData";
void cybs_mask_in_place( const char *szField, char *szValue )
{
        size_t nLen = szValue != NULL ? strlen( szValue ) : 0;
	if (nLen == 0) return;

	if (strstr( szField, TRACK_DATA ) != NULL ||
	    (nLen >= 1 && nLen <= 9)) {
		// mask everything
		for (size_t i = 0; i < nLen; ++i) {
			szValue[i] = MASK_CHAR;
		}
	} else if (nLen >= 10 && nLen <= 15) {
		// mask everything but the first and last two
		size_t upperLimit = nLen - 2;
		for (size_t i = 2; i < upperLimit; ++i) {
			szValue[i] = MASK_CHAR;
		}
	} else if (nLen >= 16) {
		// mask everything but the first and last four
		size_t upperLimit = nLen - 4;
		for (size_t i = 4; i < upperLimit; ++i) {
			szValue[i] = MASK_CHAR;
		}
	}
}

static const wchar_t W_MASK_CHAR = L'x';
static const wchar_t W_TRACK_DATA[] = L"trackData";
void cybs_mask_in_place( const std::wstring szField, std::wstring &szValue )
{
	size_t nLen = !szValue.empty() ? szValue.length() : 0;
	
	//size_t nLen = szValue != NULL ? strlen( szValue ) : 0;
	if (nLen == 0) return;

	if (wcscmp (szField.c_str(), W_TRACK_DATA) == 0 ||
	    (nLen >= 1 && nLen <= 9)) {
		// mask everything
		for (size_t i = 0; i < nLen; ++i) {
			szValue[i] = W_MASK_CHAR;
		}
	} else if (nLen >= 10 && nLen <= 15) {
		// mask everything but the first and last two
		size_t upperLimit = nLen - 2;
		for (size_t i = 2; i < upperLimit; ++i) {
			szValue[i] = W_MASK_CHAR;
		}
	} else if (nLen >= 16) {
		// mask everything but the first and last four
		size_t upperLimit = nLen - 4;
		for (size_t i = 4; i < upperLimit; ++i) {
			szValue[i] = W_MASK_CHAR;
		}
	}
}

char *cybs_strdup( const char * szStringToDup )
{
	char *szDup
		= (char *) malloc( strlen( szStringToDup ) + sizeof( char ) );

	if (szDup)
	{
		strcpy( szDup, szStringToDup );
		return( szDup );
	}

	return( 0 );
}

static char *mask( const char *szField, const char *szValue )
{
	char *szMasked = cybs_strdup( szValue );
	if (!szMasked) {
		return( 0 );
	}
	cybs_mask_in_place( szField, szMasked );
	return( szMasked );
}

static std::wstring mask( const std::wstring szField, const std::wstring szValue )
{
	std::wstring szMasked (szValue);// = _wcsdup( szValue );
	if (szMasked.empty()) {
		return( 0 );
	}
	cybs_mask_in_place( szField, szMasked );
	return( szMasked );
}

const char *cybs_get_log_error( CybsLogError error )
{
	switch( error )
	{
		case CYBS_LE_OK     : return( ERR_OK );
		case CYBS_LE_FOPEN  : return( ERR_FOPEN );
		case CYBS_LE_FSEEK  : return( ERR_FSEEK );
		case CYBS_LE_RENAME : return( ERR_RENAME );
		default             : return( ERR_UNKNOWN );
	}
}

void get_formatted_time( const char *szFormat, char *szDest )
{

#ifdef WIN32

	SYSTEMTIME time;
	GetLocalTime( &time );
	sprintf(
		szDest, szFormat, time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );
#else

	tm *loc;
	time_t clock;

	time( &clock );

	/* localtime() is not reentrant.  However, we protect the main logging
	   functions so this is not an issue. */
	loc = localtime( &clock );

	sprintf(
		szDest, szFormat, loc->tm_year + 1900, loc->tm_mon + 1, loc->tm_mday,
		loc->tm_hour, loc->tm_min, loc->tm_sec );

#endif

}

