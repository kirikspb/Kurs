#include "stdafx.h"
#include <winsock2.h>
#include "Krest_nol_Kursovai.h"
#include <math.h>


#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;						
TCHAR szTitle[MAX_LOADSTRING];				
TCHAR szWindowClass[MAX_LOADSTRING];		

static char ForSend[3];
static bool Sending = false,Hod;
static char A[3][3] = {/////0-Пусто    1-Крестик   2-Нолик
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 }
};
// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KREST_NOL_KURSOVAI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KREST_NOL_KURSOVAI));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KREST_NOL_KURSOVAI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KREST_NOL_KURSOVAI);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 620, 663, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



WORD sockVer;
WSADATA wsaData;
int retVal;
SOCKET servSock;
SOCKET clientSock;

void Server(int vers){
	if (vers == 0){
		sockVer = MAKEWORD(2, 2);
		WSAStartup(sockVer, &wsaData);
		//Создаем сокет
		servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (servSock == INVALID_SOCKET)
		{
			WSACleanup();
			return;
		}
		SOCKADDR_IN sin;
		sin.sin_family = PF_INET;
		sin.sin_port = htons(1111);
		sin.sin_addr.s_addr = INADDR_ANY;
		retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}
		//Пытаемся начать слушать сокет
		retVal = listen(servSock, 10);
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}
		clientSock = accept(servSock, NULL, NULL);

		if (clientSock == INVALID_SOCKET)
		{
			WSACleanup();
			return;
		}
	}
	if (vers == 1){//отправляем
		if (SOCKET_ERROR == (retVal = send(clientSock, ForSend, sizeof (ForSend), 0)))
		{
			int res = WSAGetLastError();
			return;
		}

		Sending = true;
	}
	if (vers == 2){//принимаем
		char            buff[7];
		retVal = recv(clientSock, (char *)&buff, 7, 0);
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}
		A[buff[0]][buff[1]] = 2;
		Hod = true;
		bool z=false;
		for (int i = 0; i < 3; ++i){
			for (int j = 0; j < 3; ++j){
				if (A[j][i] == 0) z = true;
			}
		}
		if (z == false) {
			MessageBox(0, L"Конец игры", L"Жмяк", MB_OK);
		}
	}
	return;
}

WORD ver = MAKEWORD(2, 2);
LPHOSTENT hostEnt;
void Client(int vers){
	if (vers == 0){
		retVal = 0;
		WSAStartup(ver, (LPWSADATA)&wsaData);
		//	LPHOSTENT hostEnt;
		hostEnt = gethostbyname("localhost");
		if (!hostEnt)
		{
			WSACleanup();
			return;
		}
		//Создаем сокет
		clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSock == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}

		SOCKADDR_IN serverInfo;

		serverInfo.sin_family = PF_INET;
		serverInfo.sin_addr = *((LPIN_ADDR)*hostEnt->h_addr_list);
		serverInfo.sin_port = htons(1111);

		retVal = connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}

	}
	if (vers == 1) {
		if (SOCKET_ERROR == (retVal = send(clientSock, ForSend, sizeof (ForSend), 0)))
		{
			int res = WSAGetLastError();
			return;
		}
		Hod = false;
		Sending = true;
	}
	if (vers == 2) {
		char szResponse[9];
		char            buff[7];
		retVal = recv(clientSock, (char *)&buff, 7, 0);
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			return;
		}
		A[buff[0]][buff[1]] = 1;
		Hod = true;
		bool z = false;
		for (int i = 0; i < 3; ++i){
			for (int j = 0; j < 3; ++j){
				if (A[j][i] == 0) z = true;
			}
		}
		if (z == false) MessageBox(0, L"Конец игры", L"Жмяк", MB_OK);
	}
	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static bool Serv = true, ok = false;
	static int cxblock = 200, cyblock = 200,k,m;
	static double x,x1, y,y1;
	static HWND hWndButton[2];
	switch (message)
	{
	case WM_CREATE:
			hWndButton[0] = CreateWindow(_T("BUTTON"), _T("Создать сервер"),
				WS_CHILD | WS_VISIBLE | WS_BORDER,
				300, 250,
				130, 30,
				hWnd, (HMENU)1,
				hInst, 0);
			hWndButton[1] = CreateWindow(_T("BUTTON"), _T("Соединение"),
				WS_CHILD | WS_VISIBLE | WS_BORDER,
				300, 281,
				130, 30,
				hWnd, (HMENU)2,
				hInst, 0);

		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 1:
			Server(0);
			Serv = true;
			Hod = true;
			DestroyWindow(hWndButton[0]);
			DestroyWindow(hWndButton[1]);
			break;
		case 2:
			Client(0);
			DestroyWindow(hWndButton[0]);
			DestroyWindow(hWndButton[1]);
			Serv = false;
			Hod = false;
			Client(2);	
			InvalidateRect(hWnd, 0, true);
			break;
		}
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		
		break;
	case WM_LBUTTONUP:
		if (Hod == true){
			x = LOWORD(lParam) / cxblock;
			y = HIWORD(lParam) / cyblock;
			x = floor(x);
			y = floor(y);
			if (x < 3 && y < 3){
			if (Serv == true){//крестики	
					k = (int)x;
					m = (int)y;
					if (A[m][k] == 0){
						A[m][k] = 1;
						ok = true;
						Hod = false;						
					}
			}
			if (Serv == false){///Нолики
					k = (int)x;
					m = (int)y;
					if (A[m][k] == 0){
						A[m][k] = 2;					
						ok = true;
						Hod = false;
					}
				}
			}
			ForSend[0] = m;
			ForSend[1] = k;
			InvalidateRect(hWnd, 0, true);
		}	
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		MoveToEx(hdc, 200, 0, NULL);
		LineTo(hdc,200, 600);
		MoveToEx(hdc, 400,0, NULL);
		LineTo(hdc, 400, 600);
		MoveToEx(hdc,  0, 200 ,NULL);
		LineTo(hdc, 600, 200 );
		MoveToEx(hdc, 0, 400, NULL);
		LineTo(hdc, 600, 400);
		MoveToEx(hdc, 600, 0, NULL);
		LineTo(hdc, 600, 600);
		LineTo(hdc, 0, 600);

		for (int i = 0; i < 3; ++i){
			for (int j = 0; j < 3; ++j){
				if (A[i][j] == 1){
					x = j*cxblock;
					y = i*cyblock;
					MoveToEx(hdc, x, y, NULL);
					x1 = (j + 1)*cxblock;
					y1 = (i + 1)*cyblock;
					LineTo(hdc, x1, y1);
					MoveToEx(hdc, x1, y, NULL);
					LineTo(hdc, x, y1);
				}
				if (A[i][j] == 2){
					x = j*cxblock;
					y = i*cyblock;
					Ellipse(hdc, x+15, y, x + cxblock-15, y + cyblock);
				}
			}
		}
		
		if (Serv == true && ok == true){
			Server(1);
			bool z;
			z = false;
			for (int i = 0; i < 3; ++i){
				for (int j = 0; j < 3; ++j){
					if (A[j][i] == 0) z = true;
				}
			}
			if (z == false){
				MessageBox(0, L"Конец игры", L"Жмяк", MB_OK);
			}
			if (z == false) DestroyWindow(hWnd);
			Server(2);
			ok = false;
			InvalidateRect(hWnd, 0, true);
		}
		if (Serv == false && ok == true){
			Client(1);		
			Client(2);
			ok = false;
			InvalidateRect(hWnd, 0, true);
		}
		EndPaint(hWnd, &ps);
		
		break;
	case WM_DESTROY:
		closesocket(clientSock);
		closesocket(servSock);
		WSACleanup();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
