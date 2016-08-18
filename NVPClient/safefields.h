#include "util.h"

#ifndef _CYBS_SAFEFIELDS_H
#define _CYBS_SAFEFIELDS_H

class SafeFields {

public:
	typedef enum {
		Request, Reply
	} MessageType;

	SafeFields();
	~SafeFields();
	bool IsSafe( SafeFields::MessageType eType, char *szField ); 
	bool IsSafe( char *szParent, char *szChild ); 

private:
	CybsMap *m_pMap;

	void RemoveIndices( char *szField );

	typedef enum {
		Boundary, NonIndex, PossibleIndex, Done
	} FieldNameState;
};

#endif // _CYBS_SAFEFIELDS_H