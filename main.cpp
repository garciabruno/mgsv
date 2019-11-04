#include <Windows.h>

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "patches.h"

typedef unsigned char* (*find_t)(unsigned char*, unsigned long);
typedef std::map<unsigned char*, int> addresses_map_t;
typedef std::map<find_t, unsigned long> patches_map_t;
typedef std::map<find_t, const char*> patches_names_t;

CRITICAL_SECTION cs_print;

// Memory pointer to instruction length map
addresses_map_t ADDRESSES_MAP;
// Pointer to an array containing all patched instruction's bytes
unsigned char* INSTRUCTIONS_BYTES = NULL;
// Amount of instruction's bytes we have patched
unsigned int TOTAL_INSTRUCTION_BYTES = 0;

patches_names_t PATCHES_NAMES = {
	std::make_pair(FindAmmoCode, "Primary Ammo"),
	std::make_pair(FindSecondaryAmmoCode, "Secondary Ammo"),
	std::make_pair(FindSuppresorCode, "Supressor"),
	std::make_pair(FindWeaponKickbackCode, "Weapon kickback"),
	std::make_pair(FindWeaponSpreadCode, "Weapon spread"),
	std::make_pair(FindWeaponSwayCode, "Weapon sway"),
	std::make_pair(FindPlayerHealthCode, "Player Health")
};

patches_map_t PATCHES = {
	std::make_pair(FindAmmoCode, 5),
	std::make_pair(FindSecondaryAmmoCode, 8),
	std::make_pair(FindSuppresorCode, 5),
	std::make_pair(FindWeaponKickbackCode, 9),
	std::make_pair(FindWeaponSpreadCode, 5),
	std::make_pair(FindWeaponSwayCode, 7),
	std::make_pair(FindPlayerHealthCode, 9)
};

unsigned char* GAME_MODULE_BASE = (unsigned char*)GetModuleHandle("mgsvtpp.exe");
unsigned long GAME_MODULE_LENGTH = 0xE1B1000;

BOOL GetPatchAddresses(addresses_map_t& addresses)
{
	/*
		Loop all elements in PATCHES, on each run execute the search function and
		append it to addresses map with their corresponding bytes to patch
	*/
	patches_map_t::iterator patch = PATCHES.begin();

	while (patch != PATCHES.end())
	{
		printf("Searching address for %s\n", PATCHES_NAMES[patch->first]);
		unsigned char* code_ptr = patch->first(GAME_MODULE_BASE, GAME_MODULE_LENGTH);

		// Check if we actually got a hit
		if (code_ptr == NULL || (unsigned long long) code_ptr == 0)
		{
			printf("[ERROR] Address for %s not found\n", PATCHES_NAMES[patch->first]);
			patch++;
		}

		printf("\t[*] Address found at %016llx\n", (unsigned long long) code_ptr);
		addresses.insert(std::make_pair(code_ptr, patch->second));

		// Increment the amount of instruction's bytes we have to keep track of
		TOTAL_INSTRUCTION_BYTES += patch->second;

		patch++;
	}

	if (TOTAL_INSTRUCTION_BYTES > 0)
	{
		// Allocate an array to store all the instruction's bytes
		INSTRUCTIONS_BYTES = (unsigned char*) malloc(TOTAL_INSTRUCTION_BYTES);
		ZeroMemory(INSTRUCTIONS_BYTES, TOTAL_INSTRUCTION_BYTES);
	}

	addresses_map_t::iterator addr = addresses.begin();
	unsigned int bytes_index = 0;

	while (addr != addresses.end() && INSTRUCTIONS_BYTES != NULL)
	{
		// Copy the bytes from the original instruction to our allocated array
		memcpy(INSTRUCTIONS_BYTES + bytes_index, addr->first, addr->second);

		// Increment the bytes_index by the amount of bytes we just stored
		bytes_index += addr->second;
		
		addr++;
	}

	return TRUE;
}

DWORD WINAPI ModThread(LPVOID lpParam)
{
	printf("[ Loading patches ]\n");

	if (!GetPatchAddresses(ADDRESSES_MAP))
	{
		printf("Failed to get addresses to patch!\n");
	}
	else
	{
		addresses_map_t::iterator addr = ADDRESSES_MAP.begin();

		while (addr != ADDRESSES_MAP.end())
		{
			DWORD oldProtect;

			printf("Patching (%016llx)\n", (unsigned long long) addr->first);

			VirtualProtect(addr->first, addr->second, PAGE_EXECUTE_READWRITE, &oldProtect);
			memset(addr->first, 0x90, addr->second);
			VirtualProtect(addr->first, addr->second, oldProtect, &oldProtect);

			addr++;
		}
	}

	while (TRUE)
	{
		/*
			TODO:
			  * Unhook all functions
			  * Unpatch all instructions
			  * Free instruction's bytes array (if != NULL)
		*/

		if (GetAsyncKeyState(VK_F10) & 1)
		{
			HMODULE hModule = GetModuleHandle("mod.dll");

			if (hModule == NULL)
			{
				printf("Could not unload DLL: Handle to mod.dll not found\n");
				continue;
			}

			printf("Unloading mgsvmod.dll\n");
			
			fclose(stdout);
			
			if (!FreeConsole())
			{
				printf("Failed to release console\n");
				continue;
			}

			FreeLibraryAndExitThread(hModule, 0);
			break;
		}

		Sleep(200);
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hinstDLL);
			InitializeCriticalSection(&cs_print);

			if (!AllocConsole())
			{
				MessageBoxA(NULL, "The console window was not created", NULL, MB_ICONEXCLAMATION);
			}

			AttachConsole(GetProcessId(hinstDLL));
			SetConsoleCtrlHandler(NULL, true);

			(void) freopen("CON", "w", stdout);

			printf("Loading mgsvmod.dll\n");
			CreateThread(0, 0, ModThread, 0, 0, 0);	

			break;
		}
	}	

	return true;
}
