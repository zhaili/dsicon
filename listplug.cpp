// listplug.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "listplug.h"
#include "cunicode.h"

#include "dsheader.h"

#define parsefunction "ext=\"NDS\""

HINSTANCE hinst;
HMODULE FLibHandle=0;
char inifilename[MAX_PATH]="lsplugin.ini";  // Unused in this plugin, may be used to save data

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			hinst=(HINSTANCE)hModule;
			break;
		case DLL_PROCESS_DETACH:
			if (FLibHandle)
				FreeLibrary(FLibHandle);
			FLibHandle=NULL;
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "DSIcon";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	CDSImage image("test.nds");

    image.Load("xpa-msog.nds");

	const PBITMAPINFO pbi = image.GetBitmapInfo();
	const u8* imagedata = image.GetImageData();

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
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		//SetDIBitsToDevice(hdc, 0, 0, 32, 32, 0, 0, 0, 32, imagedata, pbi, DIB_RGB_COLORS);
		StretchDIBits(hdc, 0, 0, 64, 64, 0, 0, 32, 32, imagedata, pbi, DIB_RGB_COLORS, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//HWND __stdcall ListLoadW(HWND ParentWin,WCHAR* FileToLoad,int ShowFlags)
//{
//	HWND hwnd;
//	RECT r;
//	DWORD w2;
//	char *pdata;
//	WCHAR *p;
//	BOOL success=false;
//
//	if (ShowFlags & lcp_forceshow==0) {  // don't check extension in this case!
//		p=wcsrchr(FileToLoad,'\\');
//		if (!p)
//			return NULL;
//		p=wcsrchr(p,'.');
//		if (!p || (wcsicmp(p,supportedextension1)!=0))
//			return NULL;
//	}
//    
//	// Extension is supported -> load file
//
//
//	GetClientRect(ParentWin,&r);
//    
//	// Create window invisbile, only show when data fully loaded!
//	hwnd = CreateWindow("RichEdit20A","",WS_CHILD | ES_MULTILINE | ES_READONLY |
//                        WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL,
//                        r.left,r.top,r.right-r.left,
//                        r.bottom-r.top,ParentWin,NULL,hinst,NULL);
//
//	//hwnd = CreateWindow();
//    
//	if (hwnd) {
//		PostMessage(ParentWin,WM_COMMAND,MAKELONG(lcp_ansi,itm_fontstyle),(LPARAM)hwnd);
//
//		//int l=GetFileSize(f,NULL);
//		//pdata=(char*)malloc(l+1);
//		//if (pdata) {
//		//	ReadFile(f,pdata,l,&w2,NULL);
//		//	if (w2<0) w2=0;
//		//	if (w2>(DWORD)l) w2=l;
//		//	pdata[w2]=0;
//
//		//	free(pdata);
//		//}
//
//		if (!success) {
//			DestroyWindow(hwnd);
//			hwnd=NULL;
//		}
//	}
////	CloseHandle(f);
//
//	if (hwnd)
//		ShowWindow(hwnd,SW_SHOW);
//	return hwnd;
//}

// HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags)
// {
// 	//WCHAR FileToLoadW[wdirtypemax];
// 	//return ListLoadW(ParentWin,awfilenamecopy(FileToLoadW,FileToLoad),ShowFlags);
//     return (HWND)1;
// }

// HWND __stdcall ListLoadW(HWND ParentWin,WCHAR* FileToLoad,int ShowFlags)
// {
//     return (HWND)1;
// }

// int __stdcall ListLoadNextW(HWND ParentWin,HWND ListWin,WCHAR* FileToLoad,int ShowFlags)
// {
// 	RECT r;
// 	DWORD w2;
// 	char *pdata;
// 	WCHAR *p;
// 	BOOL success=false;

// 	if (ShowFlags & lcp_forceshow==0) {  // don't check extension in this case!
// 		p=wcsrchr(FileToLoad,'\\');
// 		if (!p)
// 			return LISTPLUGIN_ERROR;
// 		p=wcsrchr(p,'.');
// 		if (!p || (wcsicmp(p,supportedextension1)!=0 && wcsicmp(p,supportedextension2)!=0
// 			   && wcsicmp(p,supportedextension3)!=0 && wcsicmp(p,supportedextension4)!=0))
// 			return LISTPLUGIN_ERROR;
// 	}
// 	// Extension is supported -> load file
// 	HANDLE f=CreateFileT(FileToLoad,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
// 							FILE_FLAG_SEQUENTIAL_SCAN,NULL);
// 	if (f==INVALID_HANDLE_VALUE)
// 		return LISTPLUGIN_ERROR;

// 	lastloadtime=GetCurrentTime();

// 	GetClientRect(ParentWin,&r);
// 	// Create window invisbile, only show when data fully loaded!
// 	if (ListWin) {
// 		SetWindowText(ListWin,"");  // clear
// 		SendMessage(ListWin, EM_SETMARGINS, EC_LEFTMARGIN, 8);
// 		SendMessage(ListWin, EM_SETEVENTMASK, 0, ENM_UPDATE); //ENM_SCROLL doesn't work for thumb movements!

// 		PostMessage(ParentWin,WM_COMMAND,MAKELONG(lcp_ansi,itm_fontstyle),(LPARAM)ListWin);

// 		int l=GetFileSize(f,NULL);
// 		pdata=(char*)malloc(l+1);
// 		if (pdata) {
// 			ReadFile(f,pdata,l,&w2,NULL);
// 			if (w2<0) w2=0;
// 			if (w2>(DWORD)l) w2=l;
// 			pdata[w2]=0;
// 			if (strlen(pdata)==w2) {     // Make sure the file doesn't contain any 0x00 char!
// 				char *p2=InsertLineNumbers(pdata,w2);
// 				if (p2) {
// 					SetWindowText(ListWin,p2);
// 					free(p2);
// 					PostMessage(ParentWin,WM_COMMAND,MAKELONG(0,itm_percent),(LPARAM)ListWin);
// 					success=true;
// 				}
// 			}
// 			free(pdata);
// 		}
// 		if (!success) {
// 			return LISTPLUGIN_ERROR;
// 		}
// 	}
// 	CloseHandle(f);
// 	lastloadtime=GetCurrentTime();
// 	return LISTPLUGIN_OK;
// }

// int __stdcall ListLoadNext(HWND ParentWin,HWND ListWin,char* FileToLoad,int ShowFlags)
// {
// 	WCHAR FileToLoadW[wdirtypemax];
// 	return ListLoadNextW(ParentWin,ListWin,awfilenamecopy(FileToLoadW,FileToLoad),ShowFlags);
// }

// int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter)
// {
// 	switch (Command) {
// 	case lc_copy:
// 		SendMessage(ListWin,WM_COPY,0,0);
// 		return LISTPLUGIN_OK;
// 	case lc_newparams:
// 		PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(0,itm_next),(LPARAM)ListWin);
// 		return LISTPLUGIN_ERROR;
// 	case lc_selectall:
// 		SendMessage(ListWin,EM_SETSEL,0,-1);
// 		return LISTPLUGIN_OK;
// 	case lc_setpercent:
// 		int firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
// 		int linecount=SendMessage(ListWin,EM_GETLINECOUNT,0,0);
// 		if (linecount>0) {
// 			int pos=MulDiv(Parameter,linecount,100);
// 			SendMessage(ListWin,EM_LINESCROLL,0,pos-firstvisible);
// 			firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
// 			// Place caret on first visible line!
// 			int firstchar=SendMessage(ListWin,EM_LINEINDEX,firstvisible,0);
// 			SendMessage(ListWin,EM_SETSEL,firstchar,firstchar);
// 			pos=MulDiv(firstvisible,100,linecount);
// 			// Update percentage display
// 			PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(pos,itm_percent),(LPARAM)ListWin);
// 			return LISTPLUGIN_OK;
// 		}
// 		break;
// 	}
// 	return LISTPLUGIN_ERROR;
// }

void __stdcall ListCloseWindow(HWND ListWin)
{
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


// int __stdcall ListNotificationReceived(HWND ListWin,int Message,WPARAM wParam,LPARAM lParam)
// {
// 	switch (Message) {
// 	case WM_COMMAND:
// 		if (HIWORD(wParam)==EN_UPDATE && abs((int)GetCurrentTime()-lastloadtime)>1000) {
// 			int firstvisible=SendMessage(ListWin,EM_GETFIRSTVISIBLELINE,0,0);
// 			int linecount=SendMessage(ListWin,EM_GETLINECOUNT,0,0);
// 			if (linecount>0) {
// 				int percent=MulDiv(firstvisible,100,linecount);
// 				PostMessage(GetParent(ListWin),WM_COMMAND,MAKELONG(percent,itm_percent),(LPARAM)ListWin);
// 			}
// 			return 0;
// 		}
// 		break;
// 	case WM_NOTIFY:
// 		break;
// 	case WM_MEASUREITEM:
// 		break;
// 	case WM_DRAWITEM:
// 		break;
// 	}
// 	return 0;
// }
