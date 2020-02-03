#include "patches.h"

patches_names_t PATCHES_NAMES = {
	std::make_pair(FindAmmoCode, "Primary Ammo"),
	std::make_pair(FindSecondaryAmmoCode, "Secondary Ammo"),
	std::make_pair(FindSuppresorCode, "Supressor"),
	std::make_pair(FindWeaponKickbackCode, "Weapon kickback"),
	std::make_pair(FindWeaponSpreadCode, "Weapon spread"),
	std::make_pair(FindWeaponSwayCode, "Weapon sway"),
	std::make_pair(FindPlayerHealthCode, "Player Health"),
	std::make_pair(FindSetWeaponReserveAmmoCode, "Weapon reserve"),
	std::make_pair(FindSetWeaponClipCode, "Weapon clip"),
	std::make_pair(FindSetSupportWeaponAmmoCode, "Support Weapon Ammo"),
	std::make_pair(FindSetFultonCode, "Inventory (Fulton)")
};

patches_map_t PATCHES = {
	std::make_pair(FindAmmoCode, 5),
	std::make_pair(FindSecondaryAmmoCode, 8),
	std::make_pair(FindSuppresorCode, 5),
	std::make_pair(FindWeaponKickbackCode, 9),
	std::make_pair(FindWeaponSpreadCode, 5),
	std::make_pair(FindWeaponSwayCode, 7),
	std::make_pair(FindPlayerHealthCode, 9),
	std::make_pair(FindSetWeaponReserveAmmoCode, 7),
	std::make_pair(FindSetWeaponClipCode, 7),
	std::make_pair(FindSetSupportWeaponAmmoCode, 5),
	std::make_pair(FindSetFultonCode, 5)
};

BOOL GetAddresses(addresses_map_t& addresses)
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
		INSTRUCTIONS_BYTES = (unsigned char*)malloc(TOTAL_INSTRUCTION_BYTES);
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

BOOL PatchAddresses(addresses_map_t addresses)
{
	addresses_map_t::iterator addr = addresses.begin();

	while (addr != addresses.end())
	{
		DWORD OldProtect;

		// TODO: Print which function is getting patched
		printf("Patching (%016llx)\n", (unsigned long long) addr->first);

		VirtualProtect(addr->first, addr->second, PAGE_EXECUTE_READWRITE, &OldProtect);
		memset(addr->first, 0x90, addr->second);
		VirtualProtect(addr->first, addr->second, OldProtect, &OldProtect);

		addr++;
	}

	return TRUE;
}

BOOL UnpatchAddresses(addresses_map_t addresses)
{
	addresses_map_t::iterator addr = addresses.begin();
	unsigned int bytes_offset = 0;

	while (addr != addresses.end())
	{
		DWORD OldProtect;

		// TODO: Print which function is getting unpatched
		printf("Unpatching (%016llx)\n", (unsigned long long) addr->first);

		VirtualProtect(addr->first, addr->second, PAGE_EXECUTE_READWRITE, &OldProtect);
		// Restore the N bytes from INSTRUCTION_BYTES + offset
		memcpy(addr->first, INSTRUCTIONS_BYTES + bytes_offset, addr->second);
		VirtualProtect(addr->first, addr->second, OldProtect, &OldProtect);

		// Increment the offset by the amount of bytes we just copied
		bytes_offset += addr->second;

		TOTAL_INSTRUCTION_BYTES -= addr->second;
		addr++;
	}

	return TRUE;
}