/*

GetLocalPlayer

*/


#include <Windows.h>
#include "verinfo.h"


int GameDll = 0;
int pJassEnvAddress = 0;
int GetLocalPlayerOffset = 0;

typedef int( __cdecl * pGetLocalPlayer )( );
pGetLocalPlayer GetLocalPlayer;

typedef int( __cdecl * pGetPlayerId )( int player );
pGetPlayerId GetPlayerId;

typedef int( __cdecl * pPlayer )( int playerid );
pPlayer Player;

#define IsKeyPressed(CODE) ((GetAsyncKeyState(CODE) & 0x8000) > 0)



int __cdecl GetLocalPlayerMy( )
{
	/*if ( GetPlayerId( GetLocalPlayer( ) ) <= 6 )
	{
		return Player( 7 );
	}
	return Player( 1 );*/
	if ( IsKeyPressed( VK_LMENU ) )
	{
		if ( IsKeyPressed( VK_LCONTROL ) )
		{
			if ( GetPlayerId( GetLocalPlayer( ) ) <= 6 )
			{
				return Player( 7 );
			}
			return Player( 1 );
		}

		return GetLocalPlayer( );
	}
	return Player( 15 );
}

int CreateJassNativeHook( int oldaddress, int newaddress )
{
	int FirstAddress = *( int * ) pJassEnvAddress;
	if ( FirstAddress )
	{
		FirstAddress = *( int * ) ( FirstAddress + 20 );
		if ( FirstAddress )
		{

			FirstAddress = *( int * ) ( FirstAddress + 32 );
			if ( FirstAddress )
			{

				int NextAddress = FirstAddress;

				while ( TRUE )
				{
					if ( *( int * ) ( NextAddress + 12 ) == oldaddress )
					{
						*( int * ) ( NextAddress + 12 ) = newaddress;

						return NextAddress + 12;
					}

					NextAddress = *( int* ) NextAddress;

					if ( NextAddress == FirstAddress || NextAddress <= 0 )
						break;
				}
			}
		}

	}
	return 0;
}
HANDLE JustWatchForJassEnvHNDL;

DWORD WINAPI JustWatchForJassEnv( LPVOID )
{
	while ( true )
	{
		CreateJassNativeHook( ( int ) GetLocalPlayerOffset, ( int ) &GetLocalPlayerMy );
		Sleep( 2000 );
	}

	return 0;
}



void Init126aVer( )
{
	pJassEnvAddress = GameDll + 0xADA848;
	GetLocalPlayerOffset = GameDll + 0x3BBB60;
	GetLocalPlayer = ( pGetLocalPlayer ) GetLocalPlayerOffset;
	GetPlayerId = ( pGetPlayerId ) ( GameDll + 0x3C9640 );
	Player = ( pPlayer ) ( GameDll + 0x3BBB30 );
	CreateThread( 0, 0, JustWatchForJassEnv, 0, 0, 0 );
}


void Init127aVer( )
{
	pJassEnvAddress = GameDll + 0xBE3740;
	GetLocalPlayerOffset = GameDll + 0x1E3150;
	GetLocalPlayer = ( pGetLocalPlayer ) GetLocalPlayerOffset;
	GetPlayerId = ( pGetPlayerId ) ( GameDll + 0x1E3D20 );
	Player = ( pPlayer ) ( GameDll + 0x1F1E70 );
	CreateThread( 0, 0, JustWatchForJassEnv, 0, 0, 0 );
}




void InitializeLocalPlayerHacker( )
{
	HMODULE hGameDll = GetModuleHandle( "Game.dll" );
	if ( !hGameDll )
	{
		MessageBox( 0, "LocalPlayerHacker problem!\nNo game.dll found.", "Game.dll not found", 0 );
		return;
	}

	GameDll = ( int ) hGameDll;

	CFileVersionInfo gdllver;
	gdllver.Open( hGameDll );
	// Game.dll version (1.XX)
	int GameDllVer = gdllver.GetFileVersionQFE( );
	gdllver.Close( );

	if ( GameDllVer == 6401 )
	{
		Init126aVer( );
	}
	else if ( GameDllVer == 52240 )
	{
		Init127aVer( );
	}
	else
	{
		MessageBox( 0, "LocalPlayerHacker problem!\nGame version not supported.", "\nGame version not supported", 0 );
		return;
	}



	// 6401 - 126
	// 52240 - 127

}



BOOL __stdcall DllMain( HINSTANCE i, DWORD r, LPVOID )
{
	if ( r == DLL_PROCESS_ATTACH )
	{
		InitializeLocalPlayerHacker( );
	}
	else if ( r == DLL_PROCESS_DETACH )
	{
		TerminateThread( JustWatchForJassEnvHNDL, 0 );
	}

	return TRUE;
}