// Kurs_Project.cpp: определяет точку входа для приложения.
//

#include "resource.h"
#include "stdafx.h"
#include "Kurs_Project.h"
#include <TlHelp32.h>
#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <crtversion.h>

#define MAX_LOADSTRING 100
#define UPDATE_TIMER_ID 3001
#define UPDATE_TIMER_ID1 3002
#define UPDATE_TIMER_ID2 3003
#define UPDATE_TIMER_ID3 3004
#define ARRAY_SIZE 1024
#define INFO_BUFFER_SIZE 32767

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
HWND hDlg;

// Отправить объявления функций, включенных в этот модуль кода:
INT_PTR CALLBACK	SystemMonitor(HWND, UINT, WPARAM, LPARAM);
VOID CALLBACK UpdateTimerFunctionP(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK UpdateTimerFunctionS(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK UpdateTimerFunctionD(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK UpdateTimerFunctionM(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void PrintINF_PC(HWND hWnd);
void PrintMemoryInfo(HWND hWnd, DWORD processID );
void DrawGraph(HWND hWnd, int percent);


wchar_t* buf;
wchar_t* wbuf;
wchar_t * ItoT(int i)
{
	if (!buf) free(buf);
	buf = (wchar_t*) calloc(10,sizeof(wchar_t));
	wsprintf(LPSTR(buf),"%d",i);
	return buf;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	MSG msg;
	hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SYSTEM_MONITOR), 0, SystemMonitor);
	ShowWindow(hDlg, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, 0, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}
#pragma warning(push)
#pragma warning(disable:4996)
INT_PTR CALLBACK SystemMonitor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		PrintINF_PC(hDlg);
		UpdateTimerFunctionS(hDlg, 0, 0, 0);
		SetTimer(hDlg, UPDATE_TIMER_ID1, 1000, UpdateTimerFunctionS);
		UpdateTimerFunctionP(hDlg, 0, 0, 0);
		SetTimer(hDlg, UPDATE_TIMER_ID, 10000, UpdateTimerFunctionP);
		UpdateTimerFunctionD(hDlg, 0, 0, 0);
		SetTimer(hDlg, UPDATE_TIMER_ID2, 10000, UpdateTimerFunctionD);
		UpdateTimerFunctionM(hDlg, 0, 0, 0);
		SetTimer(hDlg, UPDATE_TIMER_ID3, 30000, UpdateTimerFunctionM);
		break;

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDCANCEL:
			KillTimer(hDlg, UPDATE_TIMER_ID1);
			KillTimer(hDlg, UPDATE_TIMER_ID);
			KillTimer(hDlg, UPDATE_TIMER_ID2);
			KillTimer(hDlg, UPDATE_TIMER_ID3);
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void PrintINF_PC(HWND hWnd)
{
	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	char buf[1024];
	SYSTEM_INFO si;
	OSVERSIONINFOEX osviex;

	GetSystemInfo(&si); 
	//версия
	ZeroMemory(&osviex, sizeof(OSVERSIONINFOEX));
	osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((LPOSVERSIONINFO)&osviex);
	SendMessage(GetDlgItem(hWnd, IDC_EDIT2), WM_SETTEXT, NULL, (LPARAM)osviex.szCSDVersion);
	//ОС
	if (osviex.dwMajorVersion==10 && osviex.dwMinorVersion==0)
		sprintf(buf,"Windows 10");
	else if (osviex.dwMajorVersion==6 && osviex.dwMinorVersion==3)
		sprintf(buf,"Windows 8.1");
	else if (osviex.dwMajorVersion==6 && osviex.dwMinorVersion==2)
		sprintf(buf,"Windows 8");
	else if (osviex.dwMajorVersion==6 && osviex.dwMinorVersion==1)
		sprintf(buf,"Windows 7");
	else if (osviex.dwMajorVersion==6 && osviex.dwMinorVersion==0)
		sprintf(buf,"Windows Vista");
	else if (osviex.dwMajorVersion==5 && osviex.dwMinorVersion==3)
		sprintf(buf,"Windows Server 2003");
	else if(osviex.dwMajorVersion==5 && osviex.dwMinorVersion==0)
		sprintf(buf,"Windows 2000");
	else if(osviex.dwMajorVersion==5 &&   osviex.dwMinorVersion==1)
		sprintf(buf,"Windows XP");
	else if(osviex.dwMajorVersion<=4)
		sprintf(buf,"Windows NT");		
	SendMessage(GetDlgItem(hWnd, IDC_EDIT1), WM_SETTEXT, NULL, (LPARAM)buf);
	//тип ОС
	if(si.wProcessorArchitecture==9)
	{
		sprintf(buf,"64-разрядная");
	}
	else if(si.wProcessorArchitecture==0)
	{
		sprintf(buf,"32-разрядная");
	}
	SendMessage(GetDlgItem(hWnd, IDC_EDIT3), WM_SETTEXT, NULL, (LPARAM)buf);
	//имя пользователя
	bufCharCount = INFO_BUFFER_SIZE;
	if( GetUserName( infoBuf, &bufCharCount ) )
	{
		SendMessage(GetDlgItem(hWnd, IDC_EDIT4), WM_SETTEXT, NULL, (LPARAM)infoBuf);
	}
	//имя пк
	bufCharCount = INFO_BUFFER_SIZE;
	if( GetComputerName( infoBuf, &bufCharCount ) )
	{
		SendMessage(GetDlgItem(hWnd, IDC_EDIT5), WM_SETTEXT, NULL, (LPARAM)infoBuf);
	}

}

VOID CALLBACK UpdateTimerFunctionS(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	PERFORMACE_INFORMATION pi;
	char buf[1024];
	int days, hours, minutes, seconds, percent;
	double PhysicalUsage;
	if(GetPerformanceInfo(&pi, sizeof(pi)))
	{
		sprintf(buf, "%i %s %i",((pi.CommitTotal)*4/1024), "/", ((pi.CommitLimit)*4/1024));
		SendMessage(GetDlgItem(hWnd, IDC_EDIT12), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", ((pi.PhysicalTotal)*4/1024));
		SendMessage(GetDlgItem(hWnd, IDC_EDIT6), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf( buf, "%i", ((pi.PhysicalAvailable)*4/1024));
		SendMessage(GetDlgItem(hWnd, IDC_EDIT7), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(  buf, "%i", ((pi.KernelTotal)*4/1024)); 
		SendMessage(GetDlgItem(hWnd, IDC_EDIT13), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", ((pi.KernelPaged)*4/1024));
		SendMessage(GetDlgItem(hWnd, IDC_EDIT14), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", ((pi.KernelNonpaged)*4/1024));
		SendMessage(GetDlgItem(hWnd, IDC_EDIT15), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", pi.HandleCount );
		SendMessage(GetDlgItem(hWnd, IDC_EDIT8), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", pi.ProcessCount );
		SendMessage(GetDlgItem(hWnd, IDC_EDIT10), WM_SETTEXT, NULL, (LPARAM)buf);
		sprintf(buf, "%i", pi.ThreadCount );
		SendMessage(GetDlgItem(hWnd, IDC_EDIT9), WM_SETTEXT, NULL, (LPARAM)buf);
		PhysicalUsage=(((double)pi.PhysicalTotal-(double)pi.PhysicalAvailable)*4/1024/1024);
		sprintf(buf, "%.2lf%s", PhysicalUsage , "ГБ" );
		SendMessage(GetDlgItem(hWnd, IDC_EDIT18), WM_SETTEXT, NULL, (LPARAM)buf);
		percent=pi.PhysicalAvailable*100/pi.PhysicalTotal;
		DrawGraph(hDlg, percent);
	}
	days=(GetTickCount())/(1000*60*60*24);
	hours=(GetTickCount())/(1000*60*60) - days*24;
	minutes=(GetTickCount())/(1000*60) - hours*60 - days*24*60;
	seconds=(GetTickCount())/(1000) - minutes*60 - hours*60*60 - days*24*60*60;
	sprintf(buf, "%i %s %i %s %i %s %i", days, ":", hours, ":", minutes, ":", seconds);
	SendMessage(GetDlgItem(hWnd, IDC_EDIT11), WM_SETTEXT, NULL, (LPARAM)buf);
}

void DrawGraph(HWND hWnd, int percent)
{
	HDC hdc = GetDC(hWnd); // Получаем DC(контекст устройства) для рисования
	HGDIOBJ hpenOld,hbrushOld; 
	HBRUSH hbrush;

	hbrush = CreateSolidBrush(RGB(0, 255, 0));
	hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);
	Rectangle(hdc, 705, 70, 755, 170);
	
	hbrush = CreateSolidBrush(RGB(0, 0, 0));
	hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);
	Rectangle(hdc, 705, 70, 755, percent+70);
	ReleaseDC(hWnd, hdc);
}

VOID CALLBACK UpdateTimerFunctionP(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HWND hListBox = GetDlgItem(hWnd, IDC_LIST1);
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(PROCESSENTRY32);
	char *buffer = new char[1024];
	int kolvoproc=0;
	if (Process32First(hSnapshot, &process))
	{
		do
		{
			sprintf_s(buffer, 1024, "%u %s", process.th32ProcessID, process.szExeFile);
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
			kolvoproc++;
		} while (Process32Next(hSnapshot, &process));
	}
	SendMessage(GetDlgItem(hWnd,IDC_EDIT16),WM_SETTEXT,NULL,(LPARAM)ItoT(kolvoproc));
	delete buffer;
	CloseHandle(hSnapshot);
}

VOID CALLBACK UpdateTimerFunctionD(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HWND hListBox = GetDlgItem(hWnd, IDC_LIST2);
	LPVOID drivers[ARRAY_SIZE];
	DWORD cbNeeded;
	int cDrivers, i;
	char *buffer = new char[1024];

	SendMessage(hListBox, LB_RESETCONTENT, NULL, NULL);
	if( EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
	{ 
		TCHAR szDriver[ARRAY_SIZE];

		cDrivers = cbNeeded / sizeof(drivers[0]);

		SendMessage(GetDlgItem(hWnd,IDC_EDIT17),WM_SETTEXT,NULL,(LPARAM)ItoT(cDrivers));
		for (i=0; i < cDrivers; i++ )
		{
			if(GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
			{
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szDriver);
			}
		}
	}
	else 
	{
		sprintf_s(buffer, 1024, "%s %d", "Ошибка вывода драйверов; необходимый размер массива", cbNeeded / sizeof(LPVOID));
		MessageBox(hWnd, buffer, "Ошибка", NULL);
	}
	delete buffer;
}

VOID CALLBACK UpdateTimerFunctionM(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	SendMessage(GetDlgItem(hWnd, IDC_LIST3), LB_RESETCONTENT, NULL, NULL);
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
		MessageBox(hWnd, "Невозможно получить список процессов", "Ошибка", NULL);
	}

	cProcesses = cbNeeded / sizeof(DWORD);

	for ( i = 0; i < cProcesses; i++ )
	{
		PrintMemoryInfo( hWnd, aProcesses[i] );
	}
}

void PrintMemoryInfo( HWND hWnd, DWORD processID )
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;
	char buff[1024];
	int i = 0;
	int s = 0;

	i+=sprintf(buff+i, "%d", processID);
	while(i<9)
	{
		i+=sprintf(buff+i, "%s", " ");
	}

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID );
	if (NULL == hProcess)
		return;

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		i+=sprintf( buff+i, "%s %d", " ", pmc.PageFaultCount );
		while(i<22)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i", " ", pmc.PeakWorkingSetSize/(1024) );
		while(i<33)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i", " ", pmc.WorkingSetSize/(1024) );
		while(i<46)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i", " ", pmc.QuotaPeakPagedPoolUsage/(1024) );
		while(i<62)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i"," ", pmc.QuotaPagedPoolUsage/(1024) );
		while(i<77)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i"," ", pmc.QuotaPeakNonPagedPoolUsage/(1024) );
		while(i<93)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i"," ", pmc.QuotaNonPagedPoolUsage/(1024) );
		while(i<104)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i"," ", pmc.PagefileUsage/(1024) ); 
		while(i<114)
		{
			i+=sprintf(buff+i, "%s", " ");
		}
		i+=sprintf( buff+i, "%s %i"," ", pmc.PeakPagefileUsage/(1024) );
	}
	SendMessage(GetDlgItem(hWnd, IDC_LIST3), WM_SETFONT, (WPARAM) GetStockObject(SYSTEM_FIXED_FONT), NULL);
	SendMessage(GetDlgItem(hWnd, IDC_LIST3), LB_ADDSTRING, 0, (LPARAM)buff);
	CloseHandle( hProcess );
}
#pragma warning(pop)
