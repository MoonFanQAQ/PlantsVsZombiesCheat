#pragma once
#include <Windows.h>
#include <iostream>
#include "Struct.h"
#include "../gui/imgui/imgui.h"
#include "../gui/menu.h"

namespace CheatManager {
	void Loop();
	class GetAddr
	{
	public:
		GetAddr();
		~GetAddr();
		//无限阳光 金币
		uintptr_t* GetSunAddr();
		uintptr_t* GetCoinAddr();
		//解锁全部卡槽
		uintptr_t* GetSlotsAddr();
		//卡槽无冷却
		uintptr_t* GetCoolingSlotsAddr();
		//跳关
		uintptr_t* GetJumpStageAddr();
		//透视功能
		uintptr_t* GetZombieListAddr();
		//游戏暂停
		uintptr_t* GetStopGameAddr();
		//void EspHook();
		//
		//bool EspUnHook();
	private:
		HANDLE m_ProcessHandle; // 进程句柄
		uintptr_t m_ModuleBaseAddress; // 模块基础地址
		uintptr_t m_BaseAddress; // 基础地址
	};

}

