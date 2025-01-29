#pragma once

#ifdef ICSUTIL_EXPORTS
#define ICSUTIL_API __declspec(dllexport) 
#else
#define ICSUTIL_API 
#endif


#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

const char CYBS_INI_FILE[] = "../resources/cybs.ini";


ics_msg* processRequest(ics_msg* icsRequest);


