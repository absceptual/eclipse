#pragma once
#include <Windows.h>

#define MAXSTUDIOBONES		128

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100
#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

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

class IClientRenderable
{
public:
    // Gets at the containing class...
    virtual void* GetIClientUnknown() = 0;

    // Data accessors
    virtual void GetRenderOrigin(void) = 0;
    virtual void GetRenderAngles(void) = 0;
    virtual bool					ShouldDraw(void) = 0;
    virtual bool					IsTransparent(void) = 0;
    virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
    virtual bool					UsesFullFrameBufferTexture() = 0;

    virtual void	GetShadowHandle() const = 0;

    // Used by the leaf system to store its render handle.
    virtual void RenderHandle() = 0;

    // Render baby!
    virtual const void* GetModel() const = 0;
    virtual int						DrawModel(int flags) = 0;

    // Get the body parameter
    virtual int		GetBody() = 0;

    // Determine alpha and blend amount for transparent objects based on render state info
    virtual void	ComputeFxBlend() = 0;
    virtual int		GetFxBlend(void) = 0;

    // Determine the color modulation amount
    virtual void	GetColorModulation(float* color) = 0;

    // Returns false if the entity shouldn't be drawn due to LOD. 
    // (NOTE: This is no longer used/supported, but kept in the vtable for backwards compat)
    virtual bool	LODTest() = 0;

    // Call this to get the current bone transforms for the model.
    // currentTime parameter will affect interpolation
    // nMaxBones specifies how many matrices pBoneToWorldOut can hold. (Should be greater than or
    // equal to studiohdr_t::numbones. Use MAXSTUDIOBONES to be safe.)
    virtual bool	SetupBones(void* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
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