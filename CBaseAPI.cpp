#include "CBaseAPI.h"
#include <time.h>

bool CBaseAPI::bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for( ; *szMask; ++szMask, ++pData, ++bMask )
			if( *szMask == 'x' && *pData != *bMask )
				return false;

	return ( *szMask ) == NULL;
}

DWORD CBaseAPI::dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
	for( DWORD i = NULL; i < dwLen; i++ )
		if( bDataCompare( (BYTE*) ( dwAddress + i ), bMask, szMask ) )
			return (DWORD)( dwAddress + i );

	return 0;
}

HMODULE CBaseAPI::GetModuleHandleSafe( const char* pszModuleName )
{
	HMODULE hmModuleHandle = NULL;

	do
	{
		hmModuleHandle = GetModuleHandleA( pszModuleName );
		Sleep( 1 );
	}
	while(hmModuleHandle == NULL);

	return hmModuleHandle;
}

void CBaseAPI::LogToFile( const char * pszMessage, ... )
{
	va_list va_alist;
	char szLogbuf[4096];
	FILE * fp;
	struct tm * current_tm;
	time_t current_time;

	time (&current_time);
	current_tm = localtime (&current_time);

	sprintf ( szLogbuf, "[%02d:%02d:%02d] ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec );

	va_start ( va_alist, pszMessage );
	_vsnprintf ( szLogbuf + strlen( szLogbuf ), sizeof( szLogbuf ) - strlen( szLogbuf ), pszMessage, va_alist);
	va_end ( va_alist );
	sprintf ( szLogbuf,  "%s\n", szLogbuf );
	if ( (fp = fopen ( GetDirectoryFile("PDS.log"), "a") ) != NULL )
	{
		fprintf ( fp, "%s" , szLogbuf );
		fclose (fp);
	}
#ifdef DEBUG
	::WriteConsole( hDebugConsole, szLogbuf, strlen(szLogbuf), &dwMessage, NULL );
#endif
}

DWORD CBaseAPI::GetClientSignature ( char* chPattern, char* chMask )
{
	static DWORD dwClientBase = (DWORD)GetModuleHandleSafe("client.dll");
	return dwFindPattern( dwClientBase, 0x640000, ( PBYTE )chPattern, chMask );
}
//===================================================================================
DWORD CBaseAPI::GetEngineSignature ( char* chPattern, char* chMask )
{
	static DWORD dwEngineBase = (DWORD)GetModuleHandleSafe("engine.dll");
	return dwFindPattern( dwEngineBase, 0x640000, ( PBYTE )chPattern, chMask );
}
//===================================================================================
void CBaseAPI::BaseUponModule( HMODULE hmModule )
{
	GetModuleFileNameA( hmModule, szDirectory, 512 );
	for(int i = ( int )strlen( szDirectory ); i > 0; i-- ) 
	{ 
		if(szDirectory[i] == '\\') 
		{ 
			szDirectory[i+1] = 0; break; 
		} 
	}
}
//===================================================================================
char* CBaseAPI::GetDirectoryFile( char *szFile )
{
	static char path[ 320 ];
	strcpy( path, szDirectory );
	strcat( path, szFile );
	return path;
}
//===================================================================================
CBaseAPI gBaseAPI;
