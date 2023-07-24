#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#include "DLL_Data.h"
#include "functions.h"
using namespace std;

struct InjectInfo
{
	string namespase;
	string class_name;
	string method_name;
	unsigned char* data;
	int data_length;
};

bool InitAPI() {
	HMODULE hMono = GetModuleHandleA("mono-2.0-bdwgc.dll");

	mono_get_root_domain = (MONO_GET_ROOT_DOMAIN)GetProcAddress(hMono, "mono_get_root_domain");
	mono_thread_attach = (MONO_THREAD_ATTACH)GetProcAddress(hMono, "mono_thread_attach");
	mono_image_open_from_data_full = (MONO_IMAGE_OPEN_FROM_DATA_FULL)GetProcAddress(hMono, "mono_image_open_from_data_full");
	mono_assembly_load_from_full = (MONO_ASSEMBLY_LOAD_FROM_FULL)GetProcAddress(hMono, "mono_assembly_load_from_full");
	mono_class_from_name_case = (MONO_CLASS_FROM_NAME_CASE)GetProcAddress(hMono, "mono_class_from_name_case");
	mono_class_from_name = (MONO_CLASS_FROM_NAME)GetProcAddress(hMono, "mono_class_from_name");
	mono_class_get_method_from_name = (MONO_CLASS_GET_METHOD_FROM_NAME)GetProcAddress(hMono, "mono_class_get_method_from_name");
	mono_runtime_invoke = (MONO_RUNTIME_INVOKE)GetProcAddress(hMono, "mono_runtime_invoke");
	mono_thread_attach(mono_get_root_domain());

	return true;
}

namespace MonoInjector
{

	bool InjectDll(InjectInfo dll)
	{
		InitAPI();

		//printf("domain =%p\n", mono_get_root_domain());
		
		int status;
		void* raw_image = mono_image_open_from_data_full(dll.data, dll.data_length, true, &status, false);//注册mono到游戏里
		printf("[image] raw_image = %p\n", raw_image);
		void* assembly = mono_assembly_load_from_full(raw_image, new BYTE[1], &status, false);
		printf("[assembly] assembly = %p\n", assembly);
		void* klass = mono_class_from_name(raw_image, (char*)dll.namespase.c_str(), (char*)dll.class_name.c_str());
		printf("[class] klass = %p\n", klass);
		void* loader = mono_class_get_method_from_name(klass, (char*)dll.method_name.c_str(), 0);
		printf("[method] loader = %p\n", loader);
		if (loader) {
			mono_runtime_invoke(loader, nullptr, nullptr, nullptr);
		}
		else
		{
			printf("获取方法失败\n");
		}
			
		
		printf("[Inject] 注入!\n");

		return true;
	}


}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		InjectInfo dll;
		dll.namespase = "RavenfieldCheat";
		dll.class_name = "Loader";
		dll.method_name = "Load";
		dll.data = RavenfieldCheatData;
		dll.data_length = sizeof(RavenfieldCheatData);
		MonoInjector::InjectDll(dll);
	}
	return TRUE;
}