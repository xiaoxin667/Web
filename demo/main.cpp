#include <iostream>
#include <Windows.h>

// 传入输入框的编号和按钮的编号
#define IDC_EDIT 1001
#define IDC_BUTTON 1002

using namespace std;

HWND g_hEdit;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_CREATE:  // 主窗口正在创建，此刻创建子控件
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lp;

		// 输入框
		g_hEdit = CreateWindow(L"EDIT", nullptr,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			20, 20, 400, 28,              // x, y, 宽, 高（相对主窗口客户区）
			hwnd, (HMENU)IDC_EDIT,        // 父窗口 + 控件ID
			pcs->hInstance, nullptr);

		// 按钮
		CreateWindow(L"BUTTON", L"发送",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			440, 20, 80, 28,
			hwnd, (HMENU)IDC_BUTTON,
			pcs->hInstance, nullptr);
		return 0;
	}
	case WM_COMMAND:  // 子控件有动静时，消息发到这里
		if (LOWORD(wp) == IDC_BUTTON && HIWORD(wp) == BN_CLICKED) {
			wchar_t text[512] = {};
			GetWindowText(g_hEdit, text, 512);   // 取输入框内容
			// TODO: 以后在这里调 send() 把 text 发出去
			MessageBox(hwnd, text, L"即将发送", MB_OK);  // 先用弹窗验证
			SetWindowText(g_hEdit, L"");          // 清空输入框
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
	// 注册窗口
	const wchar_t CLASS_NAME[] = L"MyWindow"; // 直接将窗口名称设置为变量，后续直接传递变量名称
	WNDCLASS wc = {};

	wc.lpfnWndProc = WndProc; // 传递消息处理函数
	wc.hInstance = hInst;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // 设置光标样式
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 窗口背景色
	RegisterClass(&wc);

	// 创建窗口
	HWND hwnd = CreateWindow(CLASS_NAME, L"空白窗口",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, nullptr, nullptr, hInst, nullptr

	);


	// 显示窗口
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// 消息循环：没有它进程会立刻退出
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}