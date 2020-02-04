#include "krunk.h"
#include <unordered_map>
#include "udis86/udis86.h"

std::unordered_map<void*, void*> hook_map;        // Function -> Hook
std::unordered_map<void*, void*> hook_tramp_map;  // Hook     -> Function Trampoline
std::unordered_map<void*, void*> func_tramp_map;  // Function -> Function Trampoline

PVOID handler_inst = NULL;

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* pEI)
{
	if (pEI->ExceptionRecord->ExceptionCode == STATUS_PRIVILEGED_INSTRUCTION) {
#if _WIN64 
		void* IP = (void*)pEI->ContextRecord->Rip;
		bool is_hook = hook_map.find(IP) != hook_map.end();
		if (is_hook) {
			pEI->ContextRecord->Rip = (uint64_t)hook_map[IP];
			return EXCEPTION_CONTINUE_EXECUTION;
		}
#else
		void* IP = (void*)pEI->ContextRecord->Eip;
		bool is_hook = hook_map.find(IP) != hook_map.end();
		if (is_hook) {
			pEI->ContextRecord->Eip = (uint32_t)hook_map[IP];
			return EXCEPTION_CONTINUE_EXECUTION;
		}
#endif
		return EXCEPTION_CONTINUE_SEARCH;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}


/*	Creates trampoline link to a hooked function
	so that it may be called like normal.
*/
void* MakeTrampoline(void* instruction) {
	// Setup udis
	ud_t u;
	ud_init(&u);
	ud_set_vendor(&u, UD_VENDOR_ANY);
	ud_set_input_buffer(&u, (uint8_t*)instruction, 15);

	// Alloc space for the routine
	void* trampoline_ptr = VirtualAlloc(NULL, 29, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Build trampoline
#if _WIN64
	ud_set_mode(&u, 64);
	int instruction_size = ud_disassemble(&u);
	memcpy(trampoline_ptr, instruction, instruction_size);
	*((uint16_t*)((uint64_t)trampoline_ptr + (uint64_t)instruction_size))     = 0x25FF;
	*((uint32_t*)((uint64_t)trampoline_ptr + (uint64_t)instruction_size + 2)) = 0x00000000UL;
	*((uint64_t*)((uint64_t)trampoline_ptr + (uint64_t)instruction_size + 6)) = (uint64_t)instruction + (uint64_t)instruction_size;
#else
	ud_set_mode(&u, 32);
	int instruction_size = ud_disassemble(&u);
	memcpy(trampoline_ptr, instruction, instruction_size);
	*((uint8_t*) ((uint32_t)trampoline_ptr + (uint32_t)instruction_size))     = 0xE9;
	*((uint32_t*)((uint32_t)trampoline_ptr + (uint32_t)instruction_size + 1)) = ((uint32_t)instruction    + (uint32_t)instruction_size)
																			  - ((uint32_t)trampoline_ptr + (uint32_t)instruction_size) - 5;
#endif
	return trampoline_ptr;
}

							 
/*	Return Codes:
	0 = Success
	1 = Memory read failure
	2 = Hook exists
*/
int Hook(void* function, void* hook_function)
{
	// Make sure handler is initialized (happens once)
	if (!handler_inst)
		handler_inst = AddVectoredExceptionHandler(true, ExceptionFilter);

	// Do cursory failure checks
	char readtst;
	if (!ReadProcessMemory(HANDLE(-1), function, &readtst, 1, NULL))
		return 1;

	bool is_hook = hook_map.find(function) != hook_map.end();
	if (is_hook)
		return 2;

	// Install hook map reference
	hook_map[function] = hook_function;

	void* trampoline_ptr = MakeTrampoline(function);

	// Install reverse hook map trampoline reference
	hook_tramp_map[hook_function] = trampoline_ptr;
	func_tramp_map[function]      = trampoline_ptr;

	// Finally, install hlt opcode on function (assume no faults)
	DWORD pflags, tmp;
	VirtualProtect(function, 1, PAGE_EXECUTE_READWRITE, &pflags);
	*((uint8_t*)(function)) = 0xF4; // 0xF4 (HLT)
	VirtualProtect(function, 1, pflags, &tmp);

	return 0; // Return success
}

void Unhook(void* function) {
	auto hook = hook_map.find(function);
	bool is_hook = hook != hook_map.end();
	if (is_hook) {
		uint8_t original_byte = *(uint8_t*)func_tramp_map[function];

		// Restore original first byte
		DWORD pflags, tmp;
		VirtualProtect(function, 1, PAGE_EXECUTE_READWRITE, &pflags);
		*((uint8_t*)(function)) = original_byte;
		VirtualProtect(function, 1, pflags, &tmp);

		// Free function trampoline
		VirtualFree(func_tramp_map[function], 0, MEM_RELEASE);

		hook_tramp_map.erase(hook->second);
		func_tramp_map.erase(function);
		hook_map.erase(function);
	}
}

void ClearHooks() {
	for (auto it = hook_map.cbegin(); it != hook_map.cend();) {
		uint8_t original_byte = *(uint8_t*)func_tramp_map[it->first];

		// Restore original first byte
		DWORD pflags, tmp;
		VirtualProtect(it->first, 1, PAGE_EXECUTE_READWRITE, &pflags);
		*((uint8_t*)(it->first)) = original_byte;
		VirtualProtect(it->first, 1, pflags, &tmp);

		// Free function trampoline
		VirtualFree(func_tramp_map[it->first], 0, MEM_RELEASE);

		hook_tramp_map.erase(it->second);
		func_tramp_map.erase(it->first);
		hook_map.erase(it++);
	}
}