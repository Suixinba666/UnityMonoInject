#pragma once

typedef void* (__cdecl* MONO_GET_ROOT_DOMAIN)(void);
typedef void* (__cdecl* MONO_THREAD_ATTACH)(void* domain);
typedef void* (__cdecl* MONO_IMAGE_OPEN_FROM_DATA_FULL)(void*, int, bool, void*, bool);
typedef void* (__cdecl* MONO_ASSEMBLY_LOAD_FROM_FULL)(void*, void*, int*, bool);
typedef void* (__cdecl* MONO_CLASS_FROM_NAME_CASE)(void* image, char* name_space, char* name);
typedef void* (__cdecl* MONO_CLASS_FROM_NAME)(void* image, char* name_space, char* name);
typedef void* (__cdecl* MONO_CLASS_GET_METHOD_FROM_NAME)(void* klass, char* name, int param_count);
typedef void* (__cdecl* MONO_RUNTIME_INVOKE)(void* klass, char* obj, void** params, void** exc);

MONO_GET_ROOT_DOMAIN              mono_get_root_domain;
MONO_THREAD_ATTACH                mono_thread_attach;
MONO_IMAGE_OPEN_FROM_DATA_FULL    mono_image_open_from_data_full;
MONO_ASSEMBLY_LOAD_FROM_FULL      mono_assembly_load_from_full;
MONO_CLASS_FROM_NAME_CASE         mono_class_from_name_case;
MONO_CLASS_FROM_NAME              mono_class_from_name;
MONO_CLASS_GET_METHOD_FROM_NAME   mono_class_get_method_from_name;
MONO_RUNTIME_INVOKE               mono_runtime_invoke;