// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <thread>

#define DEBUG
std::thread GuiThread;

extern int MainThread();

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
#ifdef DEBUG
		AllocConsole();
		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
#endif // DEBUG


		
		GuiThread = std::thread(MainThread);
		GuiThread.detach();
	}

	if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{

	}
	return TRUE;
}