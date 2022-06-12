
// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

// https://github.com/microsoft/detours/wiki

// Add "$(DXSDK_DIR)include" and "$(DXSDK_DIR)LIB\x86"

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* pDevice);
endScene pEndScene;

LPD3DXFONT font;

void drawRect(int x1, int y1, int x2, int y2, IDirect3DDevice9* pDevice)
{
    D3DRECT rectangle = { x1, y1, x2, y2 };
    pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
}

HRESULT __stdcall hookedEndScene(IDirect3DDevice9* pDevice) // draw stuff in here
{
    drawRect(650, 350, 700, 500, pDevice);
    return pEndScene(pDevice); // call original endScene 
}


void hackMenu()
{

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION); // create device object to get address of vmt, from there get endscene address

    D3DPRESENT_PARAMETERS d3dparams = { 0 };
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.hDeviceWindow = GetForegroundWindow();
    d3dparams.Windowed = true;

    IDirect3DDevice9* pDevice = nullptr;

    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetForegroundWindow(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dparams, &pDevice);

    if (result != D3D_OK || pDevice == nullptr)
    {
        pD3D->Release();
        return;
    }

    uintptr_t* vTable = (uintptr_t*)(*(uintptr_t*)pDevice);

    uintptr_t* EndScene = vTable + 168; // 42 x 4 (Location of endscene x pointer offset)

    pEndScene = (endScene)*EndScene;

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)pEndScene, (PVOID)hookedEndScene);
    DetourTransactionCommit();

    pDevice->Release();
    pD3D->Release();

}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)hackMenu, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

