#include "menu.h"	


void MenuInit()
{
	CheatManager::Loop();
	ImGui::Begin(u8"植物大战僵尸内部作弊",0,ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginTabBar("Tabs"))
	{

		if (ImGui::BeginTabItem(u8"基础功能"))
		{
			ImGui::Checkbox(u8"无限阳光", &GuiVars::Basic::Sun);
			ImGui::SameLine();
			ImGui::Checkbox(u8"无限金币", &GuiVars::Basic::Coin);
			ImGui::SameLine();
			ImGui::Checkbox(u8"卡槽无冷却", &GuiVars::Basic::NoCoolingSlots);
			ImGui::SameLine();
			ImGui::Checkbox(u8"解锁全部卡槽", &GuiVars::Basic::SlotsUnlock);
			ImGui::SameLine();
			ImGui::Checkbox(u8"保持游戏运行", &GuiVars::Basic::StopGame);

			
			ImGui::Button(u8"冒险模式跳关");


			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			
			ImGui::InputText(u8"跳关ID", GuiInputBuffer::Basic::StageID, 10);


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"透视功能"))
		{
			if (ImGui::Checkbox(u8"显示方框", &GuiVars::Esp::Box))ImGui::SameLine();
			{
				
				ImGui::Checkbox(u8"显示血量", &GuiVars::Esp::healthBox);
			}




			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}







	
	ImGui::End();
}