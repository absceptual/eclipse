#pragma once
#include <Windows.h>

class IBaseClientDLL
{
public:
    // Called once when the client DLL is loaded
    virtual int				Init(void* appSystemFactory,
                                void* physicsFactory,
                                void* pGlobals) = 0;

    virtual void			PostInit() = 0;

    // Called once when the client DLL is being unloaded
    virtual void			Shutdown(void) = 0;

    // Called once the client is initialized to setup client-side replay interface pointers
    virtual bool			ReplayInit(void* replayFactory) = 0;
    virtual bool			ReplayPostInit() = 0;

    // Called at the start of each level change
    virtual void			LevelInitPreEntity(char const* pMapName) = 0;
    // Called at the start of a new level, after the entities have been received and created
    virtual void			LevelInitPostEntity() = 0;
    // Called at the end of a level
    virtual void			LevelShutdown(void) = 0;

    // Request a pointer to the list of client datatable classes
    virtual void* GetAllClasses(void) = 0;
};

class IClientEntityList
{
public:
    // Get IClientNetworkable interface for specified entity
    virtual void * GetClientNetworkable(int entnum) = 0;
    virtual void* GetClientNetworkableFromHandle(void* hEnt) = 0;
    virtual void* GetClientUnknownFromHandle(void* hEnt) = 0;

    // NOTE: This function is only a convenience wrapper.
    // It returns GetClientNetworkable( entnum )->GetIClientEntity().
    virtual void* GetClientEntity(int entnum) = 0;
    virtual void* GetClientEntityFromHandle(void* hEnt) = 0;

    // Returns number of entities currently in use
    virtual int                    NumberOfEntities(bool bIncludeNonNetworkable) = 0;

    // Returns highest index actually used
    virtual int                    GetHighestEntityIndex(void) = 0;

    // Sizes entity list to specified size
    virtual void                SetMaxEntities(int maxents) = 0;
    virtual int                    GetMaxEntities() = 0;
};

using createinterface_t = void* (__cdecl*)(const char* interface_name, int* return_value);

namespace interfaces
{
    template <typename type>
    auto get(const char* dll_name, const char* interface_name)
    {
#pragma warning(disable : 6387)
        auto ct_interface{ reinterpret_cast<createinterface_t>(GetProcAddress(GetModuleHandleA(dll_name), "CreateInterface")) };

        int return_value{ 0 };
        return reinterpret_cast<type*>(ct_interface(interface_name, &return_value));
    }
}