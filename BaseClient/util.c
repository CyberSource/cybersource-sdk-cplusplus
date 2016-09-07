#ifdef WIN32
	#include "stdafx.h"
#else
	#include "ctype.h"
	#include "string.h"
#endif
#include "stdio.h"
#include "util.h"

static const char EQUALS[] = "=";
#define NULL_STRING	(char *) 0;

  /* character used to comment-out lines in the ini file */
static const char INI_COMMENT_CHAR = '#';

static void split( char *szLine, char **pszName, char **pszValue )
{
	if (szLine)
	{
		char *pEquals = strchr( szLine, (int) EQUALS[0] );
		if (pEquals)
		{
			*pEquals = '\0';
			*pszName = szLine;
			*pszValue = pEquals + 1;
		}
		else
		{
			*pszName = NULL_STRING;
			*pszValue = NULL_STRING;
		}
	}
	else
	{
		*pszName = NULL_STRING;
		*pszValue = NULL_STRING;
	}
}

static void trim( char **pszString )
{
	int nIndex;

	while (**pszString && isspace( **pszString ))
	{
		++(*pszString);
	}

	nIndex = strlen( *pszString ) - 1;
	while (nIndex >= 0 && isspace( (*pszString)[nIndex] ))
	{
		(*pszString)[nIndex] = '\0';
		--nIndex;
	}	
}

void cybs_load_config (const char *configFilename, CybsMap *map) {
	FILE *f;
	char szLine[256];
	char *szName, *szValue;
	int i = 0;
	f = fopen( configFilename, "rt" );
	if (!f) {
		(CybsMap *) 0;
	}

	//CybsMap *cybsConfig = cybs_create_map();
	while (fgets( szLine, 256, f )) {
		split( szLine, &szName, &szValue );
		if (szValue && !strlen(szValue) == 0)
		{
			trim( &szName );
			if (szName[0] != INI_COMMENT_CHAR) 
			{
				trim( &szValue );
				cybs_add(map, szName, szValue);
				i = i + 1;
			}
		} 
	}

	fclose( f );
}
