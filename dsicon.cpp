// dsicon.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "..\vld\include\vld.h"

#include "dsicon.h"

#include "dsheader.h"

#define parsefunction "ext=\"NDS\""

#define DSICON_FRAME "DSIcon"

HINSTANCE ghinst;
HMODULE FLibHandle=0;
char inifilename[MAX_PATH]="lsplugin.ini";  // Unused in this plugin, may be used to save data

#define DISPPOS_NORMAL 0
#define DISPPOS_CENTER 1

//int g_disppos = DISPPOS_NORMAL;

//CDSImage g_dsImage;

struct WindowVariable
{
    int disppos;
    CDSImage dsImage;
};

std::map<HWND, WindowVariable> g_WindowVarMap;

BOOL InitInstance(HINSTANCE hInstance);
void Uninitialize();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            InitInstance((HINSTANCE)hModule);
            break;
        case DLL_PROCESS_DETACH:
            Uninitialize();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

char* strlcpy(char* p,char*p2,int maxlen)
{
    if ((int)strlen(p2)>=maxlen) {
        strncpy(p,p2,maxlen);
        p[maxlen]=0;
    } else
        strcpy(p,p2);
    return p;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOWFRAME);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = DSICON_FRAME;
    wcex.hIconSm        = NULL;

    return RegisterClassEx(&wcex);
}

void MyUnregisterClass(HINSTANCE hInstance)
{
    UnregisterClass(DSICON_FRAME, hInstance);
}

BOOL InitInstance(HINSTANCE hInstance)
{
   ghinst = hInstance; // Store instance handle in our global variable

   MyRegisterClass(hInstance);

   return TRUE;
}

void Uninitialize()
{
    MyUnregisterClass(ghinst);

    if (FLibHandle)
        FreeLibrary(FLibHandle);
    FLibHandle=NULL;
}

void OnDraw(HWND hwnd)
{
    const WindowVariable& winvar = g_WindowVarMap[hwnd];

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    const PBITMAPINFO pbi = winvar.dsImage.GetBitmapInfo();
    const u8* imagedata = winvar.dsImage.GetImageData();

    int srcw = abs(pbi->bmiHeader.biWidth);
    int srch = abs(pbi->bmiHeader.biHeight);

    int x=0, y=0;
    if (winvar.disppos == DISPPOS_CENTER)
    {
        RECT r;
        GetClientRect(hwnd, &r);
        int w = r.right-r.left;
        int h = r.bottom - r.top;

        x = (w-srcw)/2;
        y = (h-srch)/2;
    }

    SetDIBitsToDevice(hdc, x, y, srcw, srcw, 0, 0, 0, srch, imagedata, pbi, DIB_RGB_COLORS);

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        OnDraw(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND __stdcall ListLoad(HWND hwndParent,char* FileToLoad,int ShowFlags)
{
    RECT r;
    GetClientRect(hwndParent,&r);

    HWND hwnd = CreateWindow(
        DSICON_FRAME, "NDS Icon Viewer",
        WS_CHILD,
        r.left,r.top,r.right-r.left, r.bottom-r.top,
        hwndParent, NULL, ghinst, NULL);

    if (!hwnd) return NULL;

    ShowWindow(hwnd, SW_SHOW);

    CDSImage dsimg(FileToLoad);

    WindowVariable winvar;
    winvar.disppos = (ShowFlags & lcp_center) ? DISPPOS_CENTER : DISPPOS_NORMAL;
    winvar.dsImage = dsimg;

    g_WindowVarMap[hwnd] = winvar;

    return hwnd;
}

// int __stdcall ListLoadNextW(HWND ParentWin,HWND ListWin,WCHAR* FileToLoad,int ShowFlags)
// {
//  RECT r;
//  DWORD w2;
//  char *pdata;
//  WCHAR *p;
//  BOOL success=false;

//  if (ShowFlags & lcp_forceshow==0) {  // don't check extension in this case!
//      p=wcsrchr(FileToLoad,'\\');
//      if (!p)
//          return LISTPLUGIN_ERROR;
//      p=wcsrchr(p,'.');
//      if (!p || (wcsicmp(p,supportedextension1)!=0 && wcsicmp(p,supportedextension2)!=0
//             && wcsicmp(p,supportedextension3)!=0 && wcsicmp(p,supportedextension4)!=0))
//          return LISTPLUGIN_ERROR;
//  }
//  // Extension is supported -> load file
//  HANDLE f=CreateFileT(FileToLoad,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
//                          FILE_FLAG_SEQUENTIAL_SCAN,NULL);
//  if (f==INVALID_HANDLE_VALUE)
//      return LISTPLUGIN_ERROR;

//  lastloadtime=GetCurrentTime();

//  GetClientRect(ParentWin,&r);
//  // Create window invisbile, only show when data fully loaded!
//  if (ListWin) {
//      SetWindowText(ListWin,"");  // clear
//      SendMessage(ListWin, EM_SETMARGINS, EC_LEFTMARGIN, 8);
//      SendMessage(ListWin, EM_SETEVENTMASK, 0, ENM_UPDATE); //ENM_SCROLL doesn't work for thumb movements!

//      PostMessage(ParentWin,WM_COMMAND,MAKELONG(lcp_ansi,itm_fontstyle),(LPARAM)ListWin);

//      int l=GetFileSize(f,NULL);
//      pdata=(char*)malloc(l+1);
//      if (pdata) {
//          ReadFile(f,pdata,l,&w2,NULL);
//          if (w2<0) w2=0;
//          if (w2>(DWORD)l) w2=l;
//          pdata[w2]=0;
//          if (strlen(pdata)==w2) {     // Make sure the file doesn't contain any 0x00 char!
//              char *p2=InsertLineNumbers(pdata,w2);
//              if (p2) {
//                  SetWindowText(ListWin,p2);
//                  free(p2);
//                  PostMessage(ParentWin,WM_COMMAND,MAKELONG(0,itm_percent),(LPARAM)ListWin);
//                  success=true;
//              }
//          }
//          free(pdata);
//      }
//      if (!success) {
//          return LISTPLUGIN_ERROR;
//      }
//  }
//  CloseHandle(f);
//  lastloadtime=GetCurrentTime();
//  return LISTPLUGIN_OK;
// }

// int __stdcall ListLoadNext(HWND ParentWin,HWND ListWin,char* FileToLoad,int ShowFlags)
// {
//  WCHAR FileToLoadW[wdirtypemax];
//  return ListLoadNextW(ParentWin,ListWin,awfilenamecopy(FileToLoadW,FileToLoad),ShowFlags);
// }

int __stdcall ListSendCommand(HWND listwin,int command,int parameter)
{
    WindowVariable& winvar = g_WindowVarMap[listwin];

    switch (command)
    {
    case lc_newparams:
        winvar.disppos = (parameter & lcp_center) ? DISPPOS_CENTER : DISPPOS_NORMAL;
        InvalidateRect(listwin, NULL, TRUE);
        return LISTPLUGIN_OK;
        break;
    }
    return LISTPLUGIN_ERROR;
}

void __stdcall ListCloseWindow(HWND ListWin)
{
    //assert(g_WindowVarMap.);
    g_WindowVarMap.erase(ListWin);
    DestroyWindow(ListWin);
    return;
}

void __stdcall ListGetDetectString(char* DetectString,int maxlen)
{
    strlcpy(DetectString,parsefunction,maxlen);
}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps)
{
    if (!inifilename && dps)
    {
        strlcpy(inifilename,dps->DefaultIniName,MAX_PATH-1);
    }
}

HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad,int width,int height,
                                       char* contentbuf,int contentbuflen)
{
    CDSImage image(FileToLoad);
    //CDSImage image("e:\\test\\xpa-msog.nds");

    const PBITMAPINFO pbi = image.GetBitmapInfo();
    const u8* imagedata = image.GetImageData();

    HDC maindc = GetDC(GetDesktopWindow());
    HDC imgdc  = CreateCompatibleDC(maindc);
    HBITMAP bmp_image = CreateCompatibleBitmap(maindc,width,height);
    ReleaseDC(GetDesktopWindow(),maindc);

    SelectObject(imgdc, bmp_image);

    int srcw = abs(pbi->bmiHeader.biWidth);
    int srch = abs(pbi->bmiHeader.biHeight);

    StretchDIBits(imgdc, 0, 0, width, height, 0, 0, srcw, srch, imagedata, pbi, DIB_RGB_COLORS, SRCCOPY);
    //SetDIBitsToDevice(imgdc, 0, 0, width, height, 0, 0, 0, 32, imagedata, pbi, DIB_RGB_COLORS);

    DeleteDC(imgdc);

    return bmp_image;
}
