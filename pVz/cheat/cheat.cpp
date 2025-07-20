#include "cheat.h"

void CheatManager::Loop()
{
	static CheatManager::GetAddr RetAddr;

	if (GuiVars::Basic::Sun) {
		*RetAddr.GetSunAddr() = 9999;
	}

	if (GuiVars::Basic::Coin) {
		*RetAddr.GetCoinAddr() = 9999;
	}

	if (GuiVars::Basic::SlotsUnlock) {
		*RetAddr.GetSlotsAddr() = 10;
	}

	if (GuiVars::Basic::NoCoolingSlots) {
		for (size_t i = 1; i <= 10; i++) {
			*(RetAddr.GetCoolingSlotsAddr() + (i * 0x50)) = 0;
		}
	}

	if (GuiVars::Basic::JumpStage) {
		*RetAddr.GetJumpStageAddr() = atoi(GuiInputBuffer::Basic::StageID);
	}

	if(GuiVars::Basic::StopGame) {
		*RetAddr.GetStopGameAddr() = 2;
	}

	if (GuiVars::Esp::Box) {
		for (size_t i = 0; i < 32; i++) {
			uintptr_t ZombieList = *RetAddr.GetZombieListAddr() + (i * 0x15C);
			ZombieClass Zombie{};
			Zombie.health = *((uintptr_t*)(ZombieList + 0xC8));
			if (Zombie.health < 1) { continue; }
			Zombie.MaxHealth = *((uintptr_t*)(ZombieList + 0xCC));
			Zombie.pos.x = *((float*)(ZombieList + 0x2C));
			if (Zombie.pos.x > 800.f || Zombie.pos.x < - 100.0f) { continue; }
			Zombie.pos.y = *((float*)(ZombieList + 0x30));
			if (Zombie.pos.x == 0 || Zombie.pos.y == 0) { continue; }

			ImGui::GetForegroundDrawList()->AddRect(
				ImVec2(Zombie.pos.x, Zombie.pos.y),
				ImVec2(Zombie.pos.x + 100, Zombie.pos.y + 120),
				ImColor(255, 0, 0, 255), 0.0f, 0, 1.0f);
		}
	}
}
CheatManager::GetAddr::GetAddr()
{
	m_ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	m_ModuleBaseAddress = (uintptr_t)GetModuleHandleA("PlantsVsZombies.exe");
	m_BaseAddress = *(uintptr_t*)(m_ModuleBaseAddress + 0x2A9EC0);
	std::cout << "Process Handle: " << m_ProcessHandle << std::endl;
	std::cout << "m_ModuleBaseAddress: " << std::hex << m_ModuleBaseAddress << std::endl;
	std::cout << "m_BaseAddress: " << std::hex << m_BaseAddress << std::endl;
}

CheatManager::GetAddr::~GetAddr()
{
	CloseHandle(m_ProcessHandle);
}

uintptr_t* CheatManager::GetAddr::GetSunAddr()
{
	uintptr_t* sunAddress = (uintptr_t*)(m_BaseAddress + 0x768);
	if (sunAddress == nullptr) { return nullptr; }

	uintptr_t* sunValue = (uintptr_t*)(*sunAddress + 0x5560);
	if (sunValue == nullptr) { return nullptr; }
	return sunValue;
}

uintptr_t* CheatManager::GetAddr::GetCoinAddr()
{
	uintptr_t* coinAddress = (uintptr_t*)(m_BaseAddress + 0x82C);
	if (coinAddress == nullptr) { return nullptr; }
	uintptr_t* coinValue = (uintptr_t*)(*coinAddress + 0x28);
	if (coinValue == nullptr) { return nullptr; }

	return coinValue;
}

uintptr_t* CheatManager::GetAddr::GetSlotsAddr()
{
	uintptr_t* SlotsAddress = (uintptr_t*)(m_BaseAddress + 0x768);
	if (SlotsAddress == nullptr) { return nullptr; }
	SlotsAddress = (uintptr_t*)(*SlotsAddress + 0x144);
	if (SlotsAddress == nullptr) { return nullptr; }
	uintptr_t* SlotsValue = (uintptr_t*)(*SlotsAddress + 0x24);

	return SlotsValue;
}

uintptr_t* CheatManager::GetAddr::GetCoolingSlotsAddr()
{
	uintptr_t* NoCoolingAddress = (uintptr_t*)(m_BaseAddress + 0x768);
	if (NoCoolingAddress == nullptr) { return nullptr; }
	NoCoolingAddress = (uintptr_t*)(*NoCoolingAddress + 0x144);
	if (NoCoolingAddress == nullptr) { return nullptr; }

	return NoCoolingAddress;
}

uintptr_t* CheatManager::GetAddr::GetJumpStageAddr()
{
	uintptr_t* JumpStageAddress = (uintptr_t*)(m_BaseAddress + 0x82C);
	if (JumpStageAddress == nullptr) { return nullptr; }
	uintptr_t* JumpStageValue = (uintptr_t*)(*JumpStageAddress + 0x24);
	if (JumpStageValue == nullptr) { return nullptr; }

	return JumpStageValue;
}

uintptr_t* CheatManager::GetAddr::GetZombieListAddr()
{
	uintptr_t* ZombieListAddress = (uintptr_t*)(m_BaseAddress + 0x768);
	if (ZombieListAddress == nullptr) { return nullptr; }
	ZombieListAddress = (uintptr_t*)(*ZombieListAddress + 0x90);
	if (ZombieListAddress == nullptr) { return nullptr; }

	return ZombieListAddress;
}

uintptr_t* CheatManager::GetAddr::GetStopGameAddr()
{
	uintptr_t* StopGameAddress = (uintptr_t*)(m_BaseAddress + 0x32C);
	if (StopGameAddress == nullptr) { return nullptr; }
	return StopGameAddress;
}

//void CheatManager::GetAddr::EspHook()
//{
//	uintptr_t hookAddr = m_ModuleBaseAddress + 0x12D035;
//
//	LPVOID NewAddr = VirtualAllocEx(m_ProcessHandle, nullptr, 0x100,
//		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//	ZombieListAddr = VirtualAllocEx(m_ProcessHandle, nullptr, 0x100,
//		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//
//	std::cout << "NewAddr: " << std::hex << NewAddr << std::endl;	
//	std::cout << "ZombieListAddr: " << std::hex << ZombieListAddr << std::endl;
//
//
//
//	byte originalCode[] = {
//		0x89, 0x0D,
//		0x00, 0x00, 0x00, 0x00, //僵尸地址
//		0x8B, 0x09,
//		0x8B, 0x91, 0x20, 0x08, 0x00, 0x00,
//		0xE9, 0x00, 0x00, 0x00, 0x00 // jmp PlantsVsZombies.exe+12D03D
//	};
//
//	//僵尸空白地址转换
//	memcpy(&originalCode[2], &ZombieListAddr, sizeof(ZombieListAddr));
//
//	//跳回原地址转换
//
//	uintptr_t targetAddr = hookAddr + 8;
//
//	uintptr_t currentAddr = (uintptr_t)NewAddr + 0xE;
//
//	uintptr_t offset = targetAddr - (currentAddr + 5);
//
//	memcpy(&originalCode[15], &offset, sizeof(offset));
//
//	//原代码
//	memcpy(NewAddr, originalCode, sizeof(originalCode));
//
//	//Hook开始
//	//PlantsVsZombies.exe + 12D035 - 8B 09 - mov ecx, [ecx]
//	byte hookCode[] = {
//		0xE9,
//		0x00, 0x00, 0x00, 0x00, // jmp NewAddr
//		0x0F, 0x1F, 0x00
//	};
//
//	uintptr_t hookoffset = (uintptr_t)NewAddr - (hookAddr + 5);
//
//	memcpy(&hookCode[1], &hookoffset, sizeof(hookoffset));
//
//	//PlantsVsZombies.exe + 12D035 - E9 C62F6300 - jmp 00B60000
//	//PlantsVsZombies.exe + 12D03A - 0F1F 00 - nop dword ptr[eax]
//
//	DWORD oldProtect;
//
//	VirtualProtectEx(m_ProcessHandle, (LPVOID)hookAddr, sizeof(hookCode), PAGE_READWRITE, &oldProtect);
//
//	memcpy((void*)hookAddr, hookCode, sizeof(hookCode));
//}
//
//bool CheatManager::GetAddr::EspUnHook()
//{
//	return false;
//}