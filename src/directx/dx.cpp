
#include "dx.h"

BOOL CALLBACK enum_handler(HWND handle, LPARAM lp)
{
    DWORD id{ 0 };

    GetWindowThreadProcessId(handle, &id);
    if (GetCurrentProcessId() != id) // compare current pid to current handle in loop
        return TRUE;

    dx::window = handle;
    return FALSE;
}

auto dx::get_process_window()
{
    dx::window = NULL;
    EnumWindows(enum_handler, NULL);

    RECT size;
    GetWindowRect(dx::window, &size);
    
    // compensating for window top bar
    dx::window_height = (size.bottom - size.top) - 29; 
    dx::window_width = (size.right - size.left) - 5;
    return dx::window;
}

bool dx::get_d3d9_device(void** vmt, size_t size)
{
    if (!vmt)
        return false;

    // Create Direct3d Interface
    auto p_d3d{ Direct3DCreate9(D3D_SDK_VERSION) };
    if (!p_d3d)
        return false;

    // Setup D3D device/paramters to create our dummy objcect 
    IDirect3DDevice9* dummy{ nullptr };
    D3DPRESENT_PARAMETERS d3d_pp{ };
    d3d_pp.Windowed = false;
    d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3d_pp.hDeviceWindow = get_process_window();

    auto result = p_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        dx::window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3d_pp,
        &dummy
    );

    if (FAILED(result))
    {
        d3d_pp.Windowed = true;
        result = p_d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            dx::window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &d3d_pp,
            &dummy
        );

        p_d3d->Release();
        if (FAILED(result))
            return false;
    }

    memcpy(dx::vmt, *reinterpret_cast<void***>(dummy), size);
    dummy->Release();
    p_d3d->Release();

    return true;
}
