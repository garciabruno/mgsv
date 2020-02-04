#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "patches.h"
#include "hooks.h"
#include "Utils/krunk.h"

// Memory pointer to instruction length map
addresses_map_t ADDRESSES_MAP;

unsigned char* INSTRUCTIONS_BYTES = NULL;
// Amount of instruction's bytes we have patched
unsigned int TOTAL_INSTRUCTION_BYTES = 0;

unsigned char* GAME_MODULE_BASE = (unsigned char*)GetModuleHandle("mgsvtpp.exe");
unsigned int GAME_MODULE_LENGTH = 0xE1B1000;

/* Hooks definitions */

uint64_t(*setpos_code_base)(uint64_t, int32_t);

DWORD WINAPI ModThread(LPVOID lpParam)
{
	printf("[ Loading patches ]\n");

	if (!GetAddresses(ADDRESSES_MAP))
	{
		printf("Failed to get addresses to patch!\n");
	}
	else
	{
		PatchAddresses(ADDRESSES_MAP);
	}

	setpos_code_base = (decltype(setpos_code_base)) FindSetPosCode(GAME_MODULE_BASE, GAME_MODULE_LENGTH);

	printf("\t SetPosCode: %016llx\n", (uint64_t) setpos_code_base);
	Hook(setpos_code_base, HookSetPos);

	while (TRUE)
	{
		if (GetAsyncKeyState(VK_F10) & 1)
		{
			HMODULE hModule = GetModuleHandle("mod.dll");

			if (hModule == NULL)
			{
				printf("Could not unload DLL: Handle to mod.dll not found\n");
				continue;
			}

			printf("Unloading mgsvmod.dll\n");

			printf("Unpatching addresses\n");
			UnpatchAddresses(ADDRESSES_MAP);

			if (INSTRUCTIONS_BYTES != NULL)
				free(INSTRUCTIONS_BYTES);

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
