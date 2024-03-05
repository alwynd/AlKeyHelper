#include "alkeyhelper.h"
/**
	Author: alwyn.j.dippenaar@gmail.com
	The impl for AlKeyHelper.
**/


using namespace std;

// Format date
void FormatDate(char* buffer)
{
	buffer[0] = '\0';
	time_t timer;
	tm tm_info;

	timer = time(NULL);
	localtime_s(&tm_info, &timer);

	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\0", &tm_info);
}



// Debug string to console
void debug(const char* msg)
{
	char buffer[27];
	ZeroMemory(buffer, 27);
	FormatDate(buffer);

	printf("%s - %s\n", buffer, msg); fflush(stdout);
	
}


//==================================================================
//Globals
HHOOK g_hKeyboardHook;
char tmpbuf[2048];
char tmpbuf2[2048];
bool quit = false;

bool ctl = false;
bool shift = false;
bool esc = false;


double elapsed = 0.0;

bool pulse = false;
bool combat = false;
bool ecm = false;
bool chaf = false;
bool collector = false;
bool mine = false;
bool minedown = false;
int combatmode = 0;


char key[128];
char keyval[2048];
char title[8000];

int pmode = 1;

HWND elite = NULL;


//==================================================================



BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param)
{
	::GetWindowTextA(hwnd, title, sizeof(title));

	if (strstr(title, "Elite - Dangerous (CLIENT)") != NULL) {
		elite = hwnd;
	}
	return TRUE;
}


/**
	Send, and optionally repeat a key.
**/
void sendkey(WORD key, int cmd, int repeat=1)
{
	INPUT inp;
	ZeroMemory(&inp, sizeof(INPUT));

	for (int i = 0; i < repeat; i++)
	{
		inp.type = INPUT_KEYBOARD;
		inp.ki.time = 200;
		inp.ki.dwFlags = KEYEVENTF_SCANCODE;
		inp.ki.wScan = key;

		if (cmd == 0 || cmd == 1)
		{
			SendInput(1, &inp, sizeof(INPUT));
			Sleep(200);
		} //if

		if (cmd == 0 || cmd == 2)
		{
			inp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			SendInput(1, &inp, sizeof(INPUT));
			//Sleep(200);
		} //if
	} //for
}

void DisableAllModes()
{
	combat = false;
	mine = false;
	chaf = false;
	pulse = false;
	collector = false;
	ecm = false;
	
	sendkey(DIK_NUMPADENTER, 2);
}

void PowerToShieldAndEng()
{
	debug("WinMain ... DEFAULT PWR SETTING");	
	combat = false;
	sendkey(DIK_DOWN, 0);
	sendkey(DIK_UP, 0);
	sendkey(DIK_UP, 0);
	sendkey(DIK_LEFT, 0);
	sendkey(DIK_LEFT, 0);
}

/**
This function handles the key events.
**/
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	bool process = false;

	ZeroMemory(key, sizeof(char) * 128);
	ZeroMemory(keyval, sizeof(char) * 2048);

	LPKBDLLHOOKSTRUCT str = NULL;
	str = (LPKBDLLHOOKSTRUCT)lParam;

	if (str->vkCode == VK_ESCAPE)
	{
		esc = (wParam == WM_KEYDOWN);
	}
	if (str->vkCode == VK_LCONTROL)
	{
		ctl = (wParam == WM_KEYDOWN);
	}
	if (str->vkCode == VK_LSHIFT)
	{
		shift = (wParam == WM_KEYDOWN);
	}

	if (!shift && esc && ctl)
	{
		debug("WinMain:-- quit detected");
		quit = true;
		return 1;
	}

	if (quit)
	{
		if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
		g_hKeyboardHook = NULL;

		debug("WinMain:-- DONE (LowLevelKeyboardProc)");

		exit(0);
		return 1;
	} //if


	if (wParam == WM_KEYUP)
	{
		switch (str->vkCode)
		{
		case VK_F5:				// ON F5
			combat = !combat;	// toggle combat mode, set power.
			combatmode = 0;
			if (combat)
			{
				debug("WinMain ... COMBAT .. ON");
				sendkey(DIK_DOWN, 0);
			}
			else
			{
				debug("WinMain ... COMBAT .. OFF");
				PowerToShieldAndEng();
			}
			
			break;
		case VK_F6:				// ON F6
			pulse = !pulse;		// pulse wave and boost	
			if (pulse)
			{
				debug("WinMain ... PULSE .. ON");
			}
			else
			{
				debug("WinMain ... PULSE .. OFF");
			}
			
			break;
		case VK_F7:				// ON F6
			chaf = !chaf;		// chaf toggle	
			if (chaf)
			{
				debug("WinMain ... chaf .. ON");
			}
			else
			{
				debug("WinMain ... chaf .. OFF");
			}
			
			break;
		case VK_F3:				// ON F3 
			ecm = !ecm;			// toggle ecm
			if (ecm)
			{
				debug("WinMain ... ECM .. ON");
			}
			else
			{
				debug("WinMain ... ECM .. OFF");
			}
			
			break;
		case VK_F9:				// ON F9 
			collector = !collector; // toggle collector
			if (collector)
			{
				debug("WinMain ... collector .. ON");
			}
			else
			{
				debug("WinMain ... collector .. OFF");
			}
			
			break;
		case VK_F10:			// ON F10
			if (!mine)
			{
				sendkey(DIK_LCONTROL, 0);	// swop weapons
				sendkey(DIK_RIGHTARROW, 0);	// pwr to wep
				sendkey(DIK_RIGHTARROW, 0);	// pwr to wep
				sendkey(DIK_V, 0);	// pwr to wep
				sendkey(DIK_RIGHTARROW, 0);	// pwr to wep
				sendkey(DIK_HOME, 0);	// deploy cargo
				Sleep(500);
			}
			mine = !mine; // toggle mine
			minedown = false;
			sendkey(DIK_NUMPADENTER, 0);	// pulse wave
			if (mine)
			{
				debug("WinMain ... mine .. ON");
			}
			else
			{
				sendkey(DIK_NUMPADENTER, 2);
				debug("WinMain ... mine .. OFF");
			}
			
			break;
		case VK_F2:				// ON F2
								// power to shield and eng ( -1 )
			DisableAllModes();			
			PowerToShieldAndEng();
			break;

		default:
			break;
		} //switch
	} //if


	return (process ? 1 : 0);
}

void PulseWave(void *parm)
{
	while (!quit)
	{
		if (pulse)
		{
			debug("WinMain ... PULSE");
			sendkey(DIK_TAB, 0);			// boost
			sendkey(DIK_NUMPADENTER, 0);	// pulse wave
			Sleep(6000);

			// elite = NULL;
			// EnumWindows(EnumWindowsProc, NULL);

		} //if
		Sleep(10);
		//Yield();

	} //while
	_endthread();
}


void ChargeECM(void *parm)
{
	while (!quit)
	{
		if (ecm)
		{
			sendkey(DIK_X, 1);
			Sleep(3200);
			sendkey(DIK_X, 2);
			Sleep(10200);
		} //if
		Sleep(10);
		//Yield();
	} //while
}


void ChafMode(void *parm)
{
	while (!quit)
	{
		if (chaf)
		{
			sendkey(DIK_C, 0);
			Sleep(40000);
		} //if
		Sleep(10);
		//Yield();
	} //while
}


void CollectorMode(void *parm)
{
	while (!quit)
	{
		if (collector)
		{
			sendkey(DIK_NUMPAD0, 0);
			Sleep(10000);
		} //if
		Sleep(10);
		//Yield();
	} //while
}

void MineMode(void *parm)
{
	while (!quit)
	{
		if (mine)
		{
			minedown = !minedown;
			if (minedown)
			{
				sendkey(DIK_NUMPADENTER, 1);
			}
			else
			{
				sendkey(DIK_NUMPADENTER, 2);
			}
			Sleep(6200);

			if (!mine)
			{
				Sleep(6200);
				sendkey(DIK_LCONTROL, 0);	// swop weapons
				PowerToShieldAndEng();
			}
			
		} //if
		Sleep(10);
		//Yield();
	} //while
}


void TogglePower(void *parm)
{
	while (!quit)
	{
		if (combat)
		{
			// toggle power control
			sendkey(DIK_DOWNARROW, 0);			// reset power
			switch (combatmode)
			{
			case 0:
				debug("WinMain ... COMBAT .. PWR TO ENG");
				sendkey(DIK_UPARROW, 0);		// pwr to eng 
				combatmode += 1;                // 
				break;
			case 1:
			case 3:
				combatmode += 1;                // 
				debug("WinMain ... COMBAT .. PWR DFLT");
				break;
			case 2:
				sendkey(DIK_RIGHTARROW, 0);		// pwr to sys 
				debug("WinMain ... COMBAT .. PWR SYS");
				combatmode += 1;                // 
				break;
			case 4:
				sendkey(DIK_LEFTARROW, 0);		// pwr to wep 
				debug("WinMain ... COMBAT .. PWR WEP");
				combatmode += 1;                // 
				break;
				
			case 5:
				debug("WinMain ... COMBAT .. DFLT");
				combatmode = 0;                // 
				break;
			default:
				break;					
			}
			Sleep(8000);
		} //if

		Sleep(10);
		//Yield();
	} //while
	_endthread();
}



/**
	Main entry point.
**/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	debug("WinMain:-- START");
	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	//SetForegroundWindow(elite);

	Sleep(1000);

	_beginthread(PulseWave, 0, NULL);
	_beginthread(TogglePower, 0, NULL);
	_beginthread(ChargeECM, 0, NULL);
	_beginthread(ChafMode, 0, NULL);
	_beginthread(CollectorMode, 0, NULL);
	_beginthread(MineMode, 0, NULL);

	MSG msg;
	int bRet = 0;
	while (!quit && (bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		Sleep(1);
		//Yield();

		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//ShowCursor(TRUE);

	if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);

	debug("WinMain:-- DONE (winmain)");
	return 0;
}

