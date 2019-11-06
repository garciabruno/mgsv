#pragma once

#include <Windows.h>
#include <stdbool.h>
#include <map>

typedef unsigned char* (*find_t)(unsigned char*, unsigned long);
typedef std::map<unsigned char*, int> addresses_map_t;
typedef std::map<find_t, unsigned long> patches_map_t;
typedef std::map<find_t, const char*> patches_names_t;

// Pointer to an array containing all patched instruction's bytes
extern unsigned char* INSTRUCTIONS_BYTES;
// Amount of instruction's bytes we have patched
extern unsigned int TOTAL_INSTRUCTION_BYTES;

extern unsigned char* GAME_MODULE_BASE;
extern unsigned int GAME_MODULE_LENGTH;

BOOL GetAddresses(addresses_map_t&);
BOOL PatchAddresses(addresses_map_t);
BOOL UnpatchAddresses(addresses_map_t);

/*
	AmmoCode
	0000000148e9ea30 (5 bytes)

	SIG: \x66\x44\x89\x1C\x48\x49\x8B\x4A\x58\x45\x89\xD8
	MSK: xxxx?xx??xx?
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindAmmoCode(unsigned char* start, unsigned long length)
{
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x0C\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x1A\x81\x38\x66\x44\x89\x1C\x75\xF0\x66\x81\x78\x05\x49\x8B\x75\xE8\x66\x81\x78\x09\x45\x89\x75\xE0\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SuppresorCode
	0000000148e9f7b0 (5 bytes)

	SIG: \x46\x88\x4C\x01\x04\x0F\x95\xD0\x84\xC0
	MSK: xxx??xx?x?
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindSuppresorCode(unsigned char* start, unsigned long length)
{
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x0A\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x1D\x66\x81\x38\x46\x88\x75\xF1\x80\x78\x02\x4C\x75\xEB\x66\x81\x78\x05\x0F\x95\x75\xE3\x80\x78\x08\x84\x75\xDD\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	WeaponKickbackCode
	000000014935231b (9 bytes)

	SIG: \xF3\x44\x0F\x11\x96\xA8\x00\x00\x00\x44\x0F\x28\x54\x24\x70\x41\x0F\x28\xCD
	MSK: xxxx?????xxxx??xxx?
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindWeaponKickbackCode(unsigned char* start, unsigned long length)
{
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x13\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x21\x81\x38\xF3\x44\x0F\x11\x75\xF0\x81\x78\x09\x44\x0F\x28\x54\x75\xE7\x66\x81\x78\x0F\x41\x0F\x75\xDF\x80\x78\x11\x28\x75\xD9\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	WeaponSpreadCode
	0000000140fe615f (5 bytes)

	SIG: \xF3\x0F\x10\x71\x3C\xF3\x41\x0F\x59\xC2\xF3\x0F\x5C\xF0\x0F\x2F\xF4\x73\x03\x0F\x28\xF4\x0F\xB6\x41\x30
	MSK: xxx??xxxx?xxx?xx?xxxx?xx??
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindWeaponSpreadCode(unsigned char* start, unsigned long length)
{
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x1A\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x3F\x66\x81\x38\xF3\x0F\x75\xF1\x80\x78\x02\x10\x75\xEB\x81\x78\x05\xF3\x41\x0F\x59\x75\xE2\x66\x81\x78\x0A\xF3\x0F\x75\xDA\x80\x78\x0C\x5C\x75\xD4\x66\x81\x78\x0E\x0F\x2F\x75\xCC\x81\x78\x11\x73\x03\x0F\x28\x75\xC3\x66\x81\x78\x16\x0F\xB6\x75\xBB\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	WeaponSwayCode
	0000000149356ee9 (7 bytes)

	SIG: \x0F\x29\x8F\x80\x00\x00\x00\x48\x8B\x8D\xA7\xFF\xFF\xFF\x48\x31\xE1\xE8\x91\x24\x6A\xF8\x4C\x8D\x9C\x24\x00\x01\x00\x00\x49\x8B\x5B\x18\x49\x8B\x7B\x20\x41\x0F\x28\xB3\xF0\xFF\xFF\xFF\x41\x0F\x28\xBB\xE0\xFF\xFF\xFF\x45\x0F\x28\x83\xD0\xFF\xFF\xFF\x45\x0F\x28\x8B\xC0\xFF\xFF\xFF\x45\x0F\x28\x93\xB0\xFF\xFF\xFF\x45\x0F\x28\x9B\xA0\xFF\xFF\xFF\x45\x0F\x28\xA3\x90\xFF\xFF\xFF\x45\x0F\x28\xAB\x80\xFF\xFF\xFF\x44\x0F\x28\x74\x24\x70\x44\x0F\x28\x7C\x24\x60\x4C\x89\xDC\x5D
	MSK: xx?????xx?????xx?x????xxx?????xx??xx??xxx?????xxx?????xxx?????xxx?????xxx?????xxx?????xxx?????xxx?????xxxx??xxxx??xx?x
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindWeaponSwayCode(unsigned char* start, unsigned long length)
{
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x76\x48\x01\xC2\xEB\x10\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x0F\x83\xFD\x00\x00\x00\x66\x81\x38\x0F\x29\x75\xED\x66\x81\x78\x07\x48\x8B\x75\xE5\x66\x81\x78\x0E\x48\x31\x75\xDD\x80\x78\x11\xE8\x75\xD7\x66\x81\x78\x16\x4C\x8D\x75\xCF\x80\x78\x18\x9C\x75\xC9\x66\x81\x78\x1E\x49\x8B\x75\xC1\x66\x81\x78\x22\x49\x8B\x75\xB9\x66\x81\x78\x26\x41\x0F\x75\xB1\x80\x78\x28\x28\x75\xAB\x66\x81\x78\x2E\x41\x0F\x75\xA3\x80\x78\x30\x28\x75\x9D\x66\x81\x78\x36\x45\x0F\x75\x95\x80\x78\x38\x28\x75\x8F\x66\x81\x78\x3E\x45\x0F\x75\x87\x80\x78\x40\x28\x75\x81\x66\x81\x78\x46\x45\x0F\x0F\x85\x75\xFF\xFF\xFF\x80\x78\x48\x28\x0F\x85\x6B\xFF\xFF\xFF\x66\x81\x78\x4E\x45\x0F\x0F\x85\x5F\xFF\xFF\xFF\x80\x78\x50\x28\x0F\x85\x55\xFF\xFF\xFF\x66\x81\x78\x56\x45\x0F\x0F\x85\x49\xFF\xFF\xFF\x80\x78\x58\x28\x0F\x85\x3F\xFF\xFF\xFF\x66\x81\x78\x5E\x45\x0F\x0F\x85\x33\xFF\xFF\xFF\x80\x78\x60\x28\x0F\x85\x29\xFF\xFF\xFF\x81\x78\x66\x44\x0F\x28\x74\x0F\x85\x1C\xFF\xFF\xFF\x81\x78\x6C\x44\x0F\x28\x7C\x0F\x85\x0F\xFF\xFF\xFF\x66\x81\x78\x72\x4C\x89\x0F\x85\x03\xFF\xFF\xFF\x80\x78\x75\x5D\x0F\x85\xF9\xFE\xFF\xFF\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SecondaryAmmoCode
	000000001463ED554 (8 bytes)

	SIG: \x46\x89\x8C\x19\x84\x01\x00\x00\xC3\x53
	MSK: xxx?????xx
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindSecondaryAmmoCode(unsigned char* start, unsigned long length) {
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x0A\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x17\x66\x81\x38\x46\x89\x75\xF1\x80\x78\x02\x8C\x75\xEB\x66\x81\x78\x08\xC3\x53\x75\xE3\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SetPlayerHealthCode

	SIG: \x66\x41\x89\x84\x24\x0C\x01\x00\x00\xB8\xE8\x03\x00\x00\x66\x41\x89\x84\x24\x0E\x01\x00\x00
	MSK: xxxx?????x????xxxx?????
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindPlayerHealthCode(unsigned char* start, unsigned long length) {
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x17\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x19\x81\x38\x66\x41\x89\x84\x75\xF0\x80\x78\x09\xB8\x75\xEA\x81\x78\x0E\x66\x41\x89\x84\x75\xE1\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SetWeaponReserveAmmoCode (7 bytes)

	SIG: \x66\x89\x85\xB0\x00\x00\x00\x4D\x8B\x04\x24\x41\xFF\x90\x88\x00\x00\x00
	MSK: xx?????xxx?xx?????
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindSetWeaponReserveAmmoCode(unsigned char* start, unsigned long length) {
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x12\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x1F\x66\x81\x38\x66\x89\x75\xF1\x66\x81\x78\x07\x4D\x8B\x75\xE9\x80\x78\x09\x04\x75\xE3\x66\x81\x78\x0B\x41\xFF\x75\xDB\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SetWeaponClipCode (7 bytes)

	SIG: \x66\x89\x83\x60\x02\x00\x00\x83\xFD\x05\x74\x28
	MSK: xx?????x??xx
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindSetWeaponClipCode(unsigned char* start, unsigned long length) {
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x0C\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x17\x66\x81\x38\x66\x89\x75\xF1\x80\x78\x07\x83\x75\xEB\x66\x81\x78\x0A\x74\x28\x75\xE3\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}

/*
	SetSupportWeaponAmmoCode (5 bytes)

	SIG: \x66\x41\x89\x14\x00\xC3\x4C\x8B\x2C\x24
	MSK: xxxx?xxxx?
	MODULE: mgsvtpp.exe
	LENGTH: 0xE1B1000
*/

inline unsigned char* FindSetSupportWeaponAmmoCode(unsigned char* start, unsigned long length) {
#pragma section(".text",read,execute)
	__declspec(align(16))
		__declspec(allocate(".text"))
		static char scan[] = "\x48\x89\xC8\x48\x83\xEA\x0A\x48\x01\xC2\xEB\x0C\x90\x90\x90\x90\x48\xFF\xC0\x48\x39\xD0\x73\x13\x81\x38\x66\x41\x89\x14\x75\xF0\x81\x78\x05\xC3\x4C\x8B\x2C\x75\xE7\xEB\x02\x31\xC0\xC3";
	return ((unsigned char* (*)(unsigned char*, unsigned long)) & scan)((unsigned char*)start, length);
}
