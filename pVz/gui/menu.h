#pragma once
#include <string>
#include "imgui/imgui.h"
#include "../cheat/cheat.h"

namespace GuiVars
{
	namespace Basic
	{
		inline bool Sun = false;
		inline bool Coin = false;
		inline bool NoCoolingSlots = false;
		inline bool SlotsUnlock = false;
		inline bool JumpStage = false;
		inline bool StopGame = false;
	}
	namespace Esp
	{
		inline bool Box = false;
		inline bool healthBox = false;
	}
}

namespace GuiInputBuffer
{
	namespace Basic
	{
		inline char StageID[128]{};
	}
}