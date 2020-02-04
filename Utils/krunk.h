#pragma once
#include <Windows.h>
#include <unordered_map>

#define TrampolineByHook(X) (decltype(&X)(hook_tramp_map[X])) 
#define TrampolineByFunc(X) (decltype(&X)(func_tramp_map[X]))

extern std::unordered_map<void*, void*> hook_tramp_map;  // Hook     -> Function Trampoline
extern std::unordered_map<void*, void*> func_tramp_map;  // Function -> Function Trampoline

int Hook(void* function, void* hook_function);
void Unhook(void* function);
void ClearHooks();