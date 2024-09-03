#include <iostream>
#include <string>
#include <conio.h>
#include <Windows.h> // для хука клавиатуры и блокирования
#include <thread>

using namespace std;

#define exit_key 101 // кнопка у (рус. раскладка)
#define exit_KEY_upperKey 69 // кнопка У (рус. раскладка)
#define print_key 48 // кнопка 0

// про typedef https://learn.microsoft.com/ru-ru/cpp/cpp/aliases-and-typedefs-cpp?view=msvc-170
HHOOK _hook; // создаёт хук. это типо перехватчик, из windows.h
// https://learn.microsoft.com/ru-ru/windows/win32/winmsg/using-hooks
KBDLLHOOKSTRUCT kbdstruct; // cодержит сведения о низкоуровневом событии ввода с клавиатуры
// https://learn.microsoft.com/ru-ru/windows/win32/api/winuser/ns-winuser-kbdllhookstruct
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM LParam)
{
	if (nCode >= 0) // если не мёртвый символ.
	{
		kbdstruct = *((KBDLLHOOKSTRUCT*)LParam);
		BYTE KeyboardS[256] = {}; // байт =) типо unsigned char. сохраняет данные о статусе клавиатуры
		wchar_t UnicodeChar[3] = {}; // массив символов размером 3
		GetKeyState(VK_SHIFT); // SHIFT
		GetKeyState(VK_MENU); // не работает лол
		GetKeyState(VK_CONTROL); // ctrl
		GetKeyboardState(KeyboardS);

		ToUnicodeEx((UINT)kbdstruct.vkCode, (UINT)kbdstruct.scanCode, KeyboardS, UnicodeChar, sizeof(UnicodeChar) / sizeof(*UnicodeChar) - 1, (UINT)kbdstruct.flags, GetKeyboardLayout(0));
		// ^ если я прав то это конвертер для клавиатуры в юникод. 
		// sizeof в этом случае просто считает сколько места программе нужно для выполнения


		// блокировать:
		/*
		ctrl + c
		ctrl + d
		ctrl + z
		alt + f4
		*/

		if (kbdstruct.flags & LLKHF_ALTDOWN && kbdstruct.vkCode == VK_F4)
		{
			system("CLS"); // очищает консоль
			cout << "ALT+F4 ЗАБЛОКИРОВАНО\n";
			return -1; // return -1 = ничего не делать
		}

		switch (wParam)
		{
		case WM_KEYDOWN: // при нажатии на кнопку
			if (kbdstruct.vkCode == VK_LCONTROL || kbdstruct.vkCode == VK_RCONTROL)
			{
				system("CLS");
				cout << "CTRL ЗАБЛОКИРОВАН\n";
				return -1;
			}
			break;
		}
	}
	return CallNextHookEx(_hook, nCode, wParam, LParam);
}
// https://learn.microsoft.com/ru-ru/windows/win32/learnwin32/writing-the-window-procedure

void PrintMenu()
{
	cout << "Тут должен быть текст меню.\nПроверьте PrintMenu()\n\nInsert menu text here.\nSee PrintMenu()\n";
	return;
}
void BlockKeyboard()
{
	_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0); // создаёт хук

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		PeekMessage(&msg, 0, 0, 0, 0x0001);
	}
}

void ChooseStuff()
{
	int c = 0;
	while (1)
	{
		switch (c = _getch())
		{
		case print_key:
			// сюда функцию для вывода чего-нить
			break;
		case exit_key:
		case exit_KEY_upperKey:
			exit(0);
		default:
			cout << "Не можем найти функцию\n";
			break;
		}
	}
	return;
}

int main()
{
	setlocale(0, "russian");
	PrintMenu();
	thread tBlockKey(BlockKeyboard); // две функции в один момент можно запустить через thread
	thread tChoose(ChooseStuff);

	tBlockKey.join();
	tChoose.join();
	return 0;
}

