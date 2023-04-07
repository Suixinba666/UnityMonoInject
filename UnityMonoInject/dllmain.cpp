#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#include "DLL_Data.h"
#include "functions.h"
using namespace std;

#define Invoke_method(fn, args, ...) using t_##fn = args; static t_##fn fn{0}; if(!fn) fn = reinterpret_cast< t_##fn>(method(##__VA_ARGS__))
//#define Invoke_method(fn, args, ...) using t_##fn = args; static t_##fn fn{0}; if(!fn) fn = reinterpret_cast< t_#fn >(method(##__VA+ARGS__))
inline UINT64 method(HMODULE hModule, const char* name)
{
    return (UINT64)GetProcAddress(hModule, name);
}

struct InjectInfo
{
    string namespase;
    string class_name;
    string method_name;
    unsigned char* data;
    int data_length;
};

WINBASEAPI FARPROC WINAPI GetProcAddress(_In_ HMODULE hModule,_In_ LPCSTR lpProcName);

bool InitAPI() {
    
    auto hMono = GetModuleHandleA("mono-2.0-bdwgc.dll");
    if (hMono == 0)
    {
        cout << "Could not find module mono.dll" << endl;
        return false;
    }
	mono_get_root_domain = (MONO_GET_ROOT_DOMAIN)GetProcAddress(hMono, "mono_get_root_domain");
	mono_thread_attach = (MONO_THREAD_ATTACH)GetProcAddress(hMono, "mono_thread_attach");
	mono_image_open_from_data_full = (MONO_IMAGE_OPEN_FROM_DATA_FULL)GetProcAddress(hMono, "mono_image_open_from_data_full");
	mono_assembly_load_from_full = (MONO_ASSEMBLY_LOAD_FROM_FULL)GetProcAddress(hMono, "mono_assembly_load_from_full");
    mono_class_from_name_case = (MONO_CLASS_FROM_NAME_CASE)GetProcAddress(hMono, "mono_class_from_name_case");
    mono_class_from_name = (MONO_CLASS_FROM_NAME_CASE)GetProcAddress(hMono, "mono_class_from_name");
    mono_class_get_method_from_name = (MONO_CLASS_GET_METHOD_FROM_NAME)GetProcAddress(hMono, "mono_class_get_method_from_name");
	mono_runtime_invoke = (MONO_RUNTIME_INVOKE)GetProcAddress(hMono, "mono_runtime_invoke");
    mono_thread_attach(mono_get_root_domain());

    /*Invoke_method(mono_get_root_domain, void* (__cdecl*)(void), hMono, "momo_get_root_domain");
    printf("mono_get_root_domain:%p\n", mono_get_root_domain);
    Invoke_method(mono_thread_attach,              void* (__cdecl*)(void* domain), hMono, "mono_thread_attach");
    printf("mono_thread_attach:%p\n", mono_thread_attach);
    Invoke_method(mono_image_open_from_data_full,  void* (__cdecl*)(void*, int, bool, void*, bool), hMono, "mono_image_open_from_data_full");
    printf("mono_image_open_from_data_full:%p\n", mono_image_open_from_data_full);
    Invoke_method(mono_assembly_load_from_full,    void* (__cdecl*)(void*, void*, int*, bool), hMono, "mono_assembly_load_from_full");
    printf("mono_assembly_load_from_full:%p\n", mono_assembly_load_from_full);
    Invoke_method(mono_class_from_name,            void* (__cdecl*)(void* image, char* name_space, char* name), hMono, "mono_class_from_name");
    printf("mono_class_from_name:%p\n", mono_class_from_name);
    Invoke_method(mono_class_get_method_from_name, void* (__cdecl*)(void* klass, char* name, int param_count), hMono, "mono_class_get_method_from_name");
    printf("mono_class_get_method_from_name:%p\n", mono_class_get_method_from_name);
    Invoke_method(mono_runtime_invoke,             void* (__cdecl*)(void* klass, char* obj, void** params, void** exc), hMono, "mono_runtime_invoke");
    printf("mono_runtime_invoke:%p\n", mono_runtime_invoke);
    
    printf("mono_get_root_domain:%p\n", mono_thread_attach(mono_get_root_domain()));*/
    return true;
}

namespace MonoInjector
{

    bool InjectDll(InjectInfo dll) 
    {
		InitAPI();

        printf("domain =%p\n", mono_get_root_domain());

        int status;
        void* raw_image = mono_image_open_from_data_full(dll.data, dll.data_length, true, &status, false);//注册自己的mono到游戏里
        printf("[image] raw_image =    %p\n", raw_image);
        void* assembly = mono_assembly_load_from_full(raw_image, new BYTE[1], &status, false);
        printf("[assembly] assembly =  %p\n", assembly);
        void* klass = mono_class_from_name(raw_image, (char*)dll.namespase.c_str(), (char*)dll.class_name.c_str());
        printf("[class] klass =        %p\n", klass);
        void* loader = mono_class_get_method_from_name(klass, (char*)dll.namespase.c_str(), 0);
        printf("[method] loader =      %p\n", loader);

        mono_runtime_invoke(loader, nullptr, nullptr, nullptr);
        printf("[Inject] 注入!\n");
    
        return true;
    }


}

void CheckDLLInfo(InjectInfo dll) 
{
    printf("[DLLInfo] NameSpase:  %s\n", (char*)dll.namespase.c_str());
    printf("[DLLInfo] ClassName:  %s\n", (char*)dll.class_name.c_str());
    printf("[DLLInfo] MethodName: %s\n", (char*)dll.method_name.c_str());
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);

        printf("已加载\n");
        InjectInfo dll;
        dll.namespase = "U3D_Mono";
        dll.class_name = "Loader";
        dll.method_name = "Load";
        dll.data = DLL_Data;
        dll.data_length = sizeof(DLL_Data);
        CheckDLLInfo(dll);
        MonoInjector::InjectDll(dll);
    }
    return TRUE;
}