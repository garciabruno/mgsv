#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#include "hooks.h"
#include "Utils/krunk.h"

uint64_t HookSetPos(uint64_t a1, int32_t a2)
{	
	unsigned char* p_playerClass = (unsigned char*)a1 + 0x16F70;
	unsigned char* p_xPos = p_playerClass + 0x20;
	unsigned char* p_yPos = p_playerClass + 0x24;
	unsigned char* p_zPos = p_playerClass + 0x28;

	float x;
	float y;
	float z;

	memcpy_s(&x, 4, p_xPos, 4);
	memcpy_s(&y, 4, p_yPos, 4);
	memcpy_s(&z, 4, p_zPos, 4);

	printf("\n\n\n\nPlayerClass:	%016llx\n", (uint64_t)p_playerClass);
	printf("x: %f y: %f z: %f\n", x, y, z);
	//printf("LookAt: %.2f\n\n", GetLookAtAngle());

	return TrampolineByHook(HookSetPos)(a1, a2);
}
