// 整个项目使用UTF-8编码，如果出现乱码在其他编译器当中，需更改为使用utf-8编码打开
#include <Windows.h>
#include <Windowsx.h> // 用于获取鼠标的坐标信息
#include "tool.h"

// 定义全局变量
POINT g_last = {}; 
bool g_drawing = false;

// 声明函数
void InitGraph(WNDCLASS& wnc, HINSTANCE hInst);
HWND CreateGraph(WNDCLASS& wnc, HINSTANCE hInst);

// 窗口消息函数
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	// 创建鼠标点击事件
	case WM_LBUTTONDOWN:
	{
		g_last.x = GET_X_LPARAM(lp);
		g_last.y = GET_Y_LPARAM(lp);
		g_drawing = true;
		SetCapture(hwnd);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (!g_drawing) return 0; // 没有点击鼠标
		// 存放鼠标坐标
		POINT p = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
		HDC hdc = GetDC(hwnd);

		// 根据当前工具创建画笔
		HPEN hPen;
		if (g_tool == TOOL_ERASER)
			hPen = CreatePen(PS_SOLID, 20, g_bgCOLOR);
		else
			hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		HPEN hOld = (HPEN)SelectObject(hdc, hPen);

		MoveToEx(hdc, g_last.x, g_last.y, nullptr);
		LineTo(hdc, p.x, p.y);
		ReleaseDC(hwnd, hdc);
		g_last = p;
		return 0;
	}
	// 松开鼠标事件
	case WM_LBUTTONUP:
	{
		g_drawing = false;
		ReleaseCapture();
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wp == 'E') g_tool = (g_tool == TOOL_PEN) ? TOOL_ERASER : TOOL_PEN;
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0; // 退出窗口逻辑
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

// 创建窗口主函数(画布)
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
	// 主要运行窗口函数
	// 创建一个对象用于接收初始化的值
	WNDCLASS wnc = {};
	InitGraph(wnc, hInst);

	// 注册窗口
	RegisterClass(&wnc);

	// 创建窗口
	HWND hwnd = CreateGraph(wnc, hInst);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// 消息循环
	MSG msg;
	while (GetMessage(&msg ,nullptr , 0, 0)) {
		TranslateMessage(&msg);
		// 将事件发送到相关的处理函数
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

void InitGraph(WNDCLASS& wnc, HINSTANCE hInst) {
	wnc.lpfnWndProc = WinProc; // 消息处理函数
	wnc.hInstance = hInst;
	wnc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wnc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	// 注册名称
	wnc.lpszClassName = L"222024321072036饶家瑞";
}

HWND CreateGraph(WNDCLASS& wnc, HINSTANCE hInst) {
	// 创建窗口
	HWND hwnd = CreateWindow(
		wnc.lpszClassName, // 窗口名称
		L"绘画工具", // 标题栏文字
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 600, // 默认高度和宽度值
		nullptr,
		nullptr,
		hInst, // 实例句柄
		nullptr
	);
	return hwnd;
}
// 初始化参数函数，传递目标类型
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
