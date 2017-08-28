#include "safefields.h"
#include "util.h"
#include <iostream>
#include <map>


const char CYBS_LT_FILESTART[]  = "FILESTART ";
const char CYBS_LT_TRANSTART[]  = "TRANSTART ";
const char CYBS_LT_ERROR[]      = "ERROR     ";
const char CYBS_LT_SUCCESS[]    = "SUCCESS     ";
const char CYBS_LT_CONFIG[]     = "CONFIG    ";
const char CYBS_LT_REQUEST[]    = "REQUEST   ";
const char CYBS_LT_REPLY[]      = "REPLY     ";

typedef enum
{
	CYBS_LE_OK,
	CYBS_LE_FOPEN,
	CYBS_LE_FSEEK,
	CYBS_LE_RENAME
} CybsLogError;

void cybs_log(config cfg, const char *szType, const char *szText );
CybsLogError cybs_prepare_log(config cfg);
void cybs_log_map(config config, CybsMap *cfg, const char *szType );
void cybs_get_string(CybsMap *cfg, char szBuffer[], const char *szDelim, bool fMaskSensitiveData, SafeFields::MessageType eType, int length );
void cybs_log_xml(config cfg, const char *szType, char *xmlString);
void cybs_log_NVP(config config, std::map <std::wstring, std::wstring> map, const char *szType);
const char *cybs_get_log_error( CybsLogError error );