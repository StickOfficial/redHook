#pragma once

#include "SDK.h"
#include "crc32.h"

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

class CBaseAPI
{
private:
	bool bDataCompare( const BYTE*, const BYTE*, const char* );
	char szDirectory [ 255 ];

public:
	DWORD dwFindPattern( DWORD, DWORD, BYTE*, char*);

	DWORD dwFindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
	{
		const char* pat = szPattern;
		DWORD firstMatch = NULL;
		for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;
			}
			else {
				pat = szPattern;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	HMODULE GetModuleHandleSafe( const char* pszModuleName );
	void LogToFile( const char * pszMessage, ... );
	DWORD GetClientSignature ( char* chPattern, char* chMask );
	DWORD GetEngineSignature ( char* chPattern, char* chMask );
	void BaseUponModule( HMODULE hmModule );
	char* GetDirectoryFile( char *szFile );

};

extern CBaseAPI gBaseAPI;