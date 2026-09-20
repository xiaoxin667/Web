# 画图作业：Win32 GDI 画图程序学习笔记

从零创建窗口 → GDI 画图 → 鼠标交互画图程序的完整流程记录。
配套代码：`demo/main.cpp`（窗口 + 子控件入门）。

---

## 一、准备：需要什么库

| 内容 | 说明 |
|---|---|
| `<Windows.h>` | 必备头文件，窗口 + GDI 全在里面 |
| `<windowsx.h>` | 提供 `GET_X_LPARAM` / `GET_Y_LPARAM`，从鼠标消息里取坐标 |
| `User32.lib` | 窗口、鼠标消息（VS 桌面项目默认已链接） |
| `Gdi32.lib` | 所有画图函数（同上，默认已链接） |

**不需要**额外初始化 GDI / 字体引擎，也不需要 .rc 资源文件（菜单可以纯代码建）。

---

## 二、创建窗口的六步流程

整体比喻：**窗口类是图纸 → 注册是备案 → CreateWindow 是施工 → ShowWindow 是揭幕 → 消息循环是大门**。

### 第 1 步：入口函数 WinMain

```cpp
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
```

- 界面程序入口不是 `main` 而是它，链接器按此约定找启动点
- 参数由**系统传入**：`hInst` 是程序实例句柄（填窗口类和 CreateWindow 要用），
  `nCmdShow` 表示窗口亮相方式（正常/最大化），原样传给 `ShowWindow` 即可

### 第 2 步：填写 WNDCLASS（画图纸）

```cpp
WNDCLASS wc = {};                    // 整体清零，防止垃圾值
wc.lpfnWndProc   = WinProc;          // ★ 最重要：这个类的窗口消息由谁处理
wc.hInstance     = hInst;            // 模板属于哪个程序
wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);   // 背景刷，负责擦残影
wc.lpszClassName = L"MyWindow";      // 模板名，注册和创建全靠它对号
```

原因：Windows 把"一类窗口"的共性做成**模板**（窗口类），窗口只是模板的实例。
`lpfnWndProc` 是灵魂——消息能进你写的 `WinProc`，就是因为这里登记了它。

**必填三件套**：`lpfnWndProc`、`hInstance`、`lpszClassName`，少一个 `CreateWindow` 都会失败。

### 第 3 步：RegisterClass(&wc)（备案）

把栈上的模板**拷贝进系统登记表**。之后创建窗口按类名查表；一次注册可以创建多个同类窗口。

### 第 4 步：CreateWindow（施工）

```cpp
HWND hwnd = CreateWindow(L"MyWindow",          // 类名：与注册的一字不差
                         L"标题",               // 标题栏文字
                         WS_OVERLAPPEDWINDOW,   // 标准可缩放窗口样式
                         CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // x, y, 宽, 高
                         nullptr,               // 父窗口（顶层窗口没有）
                         nullptr,               // 菜单
                         hInst,                 // 实例句柄
                         nullptr);              // 附加数据
```

- 这一步才真正分配窗口对象，返回的 `HWND` 是它的"门牌号"，以后所有窗口操作凭句柄办事
- 此刻窗口**已存在但不可见**
- 创建过程中系统立刻发一条 `WM_CREATE`——想建子控件（输入框、按钮）就在这时建

### 第 5 步：ShowWindow + UpdateWindow（揭幕）

```cpp
ShowWindow(hwnd, nCmdShow);   // 按系统要求的方式显示
UpdateWindow(hwnd);           // 立刻发一条 WM_PAINT，马上画第一帧
```

原因：先配置好再亮相，避免用户看到画一半的窗口。

### 第 6 步：消息循环（大门常开）

```cpp
MSG msg;
while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);   // 把按键翻译成字符消息
    DispatchMessage(&msg);    // ★ 派发给窗口类登记的 WinProc
}
return (int)msg.wParam;
```

原因：Windows 程序是**事件驱动**——点鼠标、拖窗口都变成一条消息进队列，
循环负责取出并派发。`GetMessage` 只有取到 `WM_QUIT` 才返回 0，循环才结束。

### 配套：WinProc 必须尽的两份义务

```cpp
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);    // 义务1：投递 WM_QUIT，消息循环才能退出
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);  // 义务2：不处理的消息还给系统
}
```

- 忘 `DefWindowProc` → 窗口拖不动、关不掉（系统默认行为被吞了）
- 忘 `PostQuitMessage` → 窗口关了进程还挂在后台

### 全流程图

```
WinMain: 入口 → 填WNDCLASS → RegisterClass备案 → CreateWindow施工
         → ShowWindow揭幕 → 消息循环待命
系统:    事件发生 → 消息入队 → 循环取出 → DispatchMessage → WinProc处理
退出:    点× → WM_DESTROY → PostQuitMessage → WM_QUIT → 循环结束 → WinMain返回
```

---

## 三、代码组织：把初始化抽成函数

```cpp
// 文件顶部：函数声明（先声明后使用，否则报"未定义标识符"）
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
void InitGraph(WNDCLASS& wnc, HINSTANCE hInst);

void InitGraph(WNDCLASS& wnc, HINSTANCE hInst) {
    wnc.lpfnWndProc   = WinProc;
    wnc.hInstance     = hInst;
    wnc.lpszClassName = L"MyWindow";
    wnc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wnc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    InitGraph(wc, hInst);   // 调用写法和传值一模一样
    RegisterClass(&wc);
    // 创建 → 显示 → 消息循环...
}
```

**为什么形参必须带 `&`（引用）**：C/C++ 函数参数默认是**复制**。
传值时函数里改的是拷贝，函数一返回拷贝销毁，WinMain 里的 `wc` 还是全零，
`RegisterClass` 注册的就是空类 → 窗口创建失败。
`WNDCLASS& wnc` 表示别名，函数内改的就是调用者的本体。

规则总结：

| 意图 | 写法 |
|---|---|
| 函数要**修改**调用者的变量 | 传引用 `T&`（或指针 `T*`） |
| 函数只**读**，小类型 | 直接传值 |
| 函数只**读**，大结构体 | `const T&`（省复制开销） |

---

## 四、GDI 绘图：画笔、图形

### DC（设备描述表）

一切绘图都通过 DC 句柄 `HDC`：

- `WM_PAINT` 里：`HDC hdc = BeginPaint(hwnd, &ps);` … `EndPaint(hwnd, &ps);`
- 消息处理中即时画：`HDC hdc = GetDC(hwnd);` … `ReleaseDC(hwnd, hdc);`

### 画笔三步走：创建 → 选入 → 删除

```cpp
HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));  // 线型, 线宽, 颜色
HPEN hOld = (HPEN)SelectObject(hdc, hPen);           // 选入 DC，记住旧的
/* ...画图... */
SelectObject(hdc, hOld);   // 换回旧画笔
DeleteObject(hPen);        // 删掉自己创建的，否则 GDI 对象泄漏
```

- 线型：`PS_SOLID` 实线、`PS_DASH` 虚线、`PS_DOT` 点线、`PS_NULL` 看不见
- 虚线/点线线宽固定 1；只有实线能任意宽度
- 颜色用 `RGB(红, 绿, 蓝)`，各分量 0~255
- 不想创建可用库存对象：`SelectObject(hdc, GetStockObject(BLACK_PEN))`（不用删）
- **画笔只管线条**；`Rectangle`/`Ellipse` 内部填充由**画刷**（Brush）决定

### 基本图形函数

| 函数 | 用途 | 参数说明 |
|---|---|---|
| `MoveToEx(hdc, x, y, nullptr)` + `LineTo(hdc, x, y)` | 直线/连续折线 | 从上一点连到新点 |
| `Rectangle(hdc, l, t, r, b)` | 矩形 | 左、上、右、下 |
| `Ellipse(hdc, l, t, r, b)` | 椭圆 | **外接矩形**四边；正方形外接框 = 正圆 |

---

## 五、画图程序设计：鼠标拖拽画图形

### 核心思想：保存 + 重绘

`WM_PAINT` 随时可能触发（被遮挡、最小化、拉伸），所以**每个画好的图形必须存进
自己的数据结构**，`WM_PAINT` 里遍历重画：

```cpp
enum ShapeType { SHAPE_PEN, SHAPE_LINE, SHAPE_RECT, SHAPE_ELLIPSE, SHAPE_TEXT };

struct Shape {
    ShapeType type;
    std::vector<POINT> pts;   // 画笔存整条轨迹；其他图形用 pts[0]起点 pts[1]终点
    COLORREF color;
    int width;
    // 文字工具专用字段：内容、字体名、字高...
};
std::vector<Shape> g_shapes;  // 已完成
Shape g_cur;                  // 正在拖拽的
bool g_dragging = false;
```

### 鼠标三消息

| 消息 | 时机 | 做什么 |
|---|---|---|
| `WM_LBUTTONDOWN` | 按下 | 记起点，构造 `g_cur`，`SetCapture(hwnd)` 防拖出窗口丢事件 |
| `WM_MOUSEMOVE` | 按住移动 | 更新 `g_cur`（画笔追加点 / 其他改终点），实时预览 |
| `WM_LBUTTONUP` | 松开 | 图形定型，`ReleaseCapture()`，push 进 `g_shapes`，`InvalidateRect` |

坐标取法：`POINT p = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };`（需 `<windowsx.h>`）

### 两种预览策略

1. **自由画笔**：每次 mousemove 直接从上一点 `LineTo` 到新点（增量画，无需擦除、不闪）
2. **直线/矩形/圆（橡皮筋）**：用反色模式画两遍实现"拖影"

```cpp
HDC hdc = GetDC(hwnd);
SetROP2(hdc, R2_NOTXORPEN);  // 反色异或：同一路线画两次 = 屏幕恢复原样
DrawShape(hdc, g_cur);       // 在旧位置再画一遍 → 擦掉上一帧预览
g_cur.pts[1] = p;            // 更新终点
DrawShape(hdc, g_cur);       // 画新位置的预览
ReleaseDC(hwnd, hdc);
```

松开时同样用 XOR 再画一遍擦掉预览，然后 `InvalidateRect(hwnd, nullptr, FALSE)`
交给 `WM_PAINT` 正式重画全部图形。

### 小技巧

- **Shift 画正圆/正方形**：mousemove 里检查 `GetKeyState(VK_SHIFT) & 0x8000`，
  把终点约束成 `|dx| == |dy|`
- **撤销**：`g_shapes.pop_back(); InvalidateRect(...);`
- **切换工具/颜色**：按钮（`CreateWindow(L"BUTTON",...)`）或纯代码建菜单
  （`CreateMenu`/`AppendMenu`），都走 `WM_COMMAND`

---

## 六、中文文字：不同字体、大小、颜色

```cpp
// 1. 创建字体：重点三个参数——字高、字体名、字符集
HFONT hFont = CreateFontW(
    48,                    // 字高（约等于像素字号）
    0, 0, 0, FW_NORMAL,    // 宽度、倾斜、方位、粗细
    FALSE, FALSE, FALSE,   // 斜体、下划线、删除线
    DEFAULT_CHARSET,       // 字符集
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE,
    L"楷体");              // ★ 字体名：宋体/黑体/楷体/微软雅黑...

// 2. 选入 DC + 设置文字专属属性
HFONT hOld = (HFONT)SelectObject(hdc, hFont);
SetTextColor(hdc, RGB(200, 0, 0));   // 文字颜色（不是画笔颜色！）
SetBkMode(hdc, TRANSPARENT);         // 去掉文字背后的白底块
TextOutW(hdc, x, y, L"你好，世界", (int)wcslen(L"你好，世界"));

// 3. 换回并删除
SelectObject(hdc, hOld);
DeleteObject(hFont);
```

想展示多种字体：准备一组预设（文字、字体名、字高、颜色），
每次点击轮换一条，即可得到"宋体大字、黑体红字、楷体蓝字……"的效果。

---

## 七、常见坑清单

| 症状 | 原因 | 解决 |
|---|---|---|
| 窗口弹不出来 | 类名不一致 / 忘 RegisterClass / 顺序反了 | 检查 `lpszClassName` 与 `CreateWindow` 第一个参数 |
| 窗口拖不动、关不掉 | 吞掉了默认消息 | `switch` 末尾 `return DefWindowProc(...)` |
| 窗口关了进程还挂着 | 忘了投递退出消息 | `WM_DESTROY` 里 `PostQuitMessage(0)` |
| InitGraph 填的白填 | 形参没带 `&`，改的是拷贝 | `WNDCLASS& wnc` |
| `WinProc` 红色波浪线"未定义标识符" | 定义在使用之后 | 文件顶部加函数声明 |
| 画出来是黑的/不变色 | GDI 对象没 `SelectObject` 进 DC | 创建后必须选入 |
| 程序越跑越卡 | `CreatePen`/`CreateFont` 创建后没删 | 用完 `DeleteObject`（先选回旧对象） |
| 窗口拉伸后图形消失 | 只画不存 | 图形入库 + `WM_PAINT` 全量重绘 |
| 拖拽预览疯狂闪烁 | 每次 move 都 `InvalidateRect` 全屏重画 | 改用 XOR 橡皮筋 / 画笔增量画 |
| 文字背后有白底块 | DC 默认不透明背景 | `SetBkMode(hdc, TRANSPARENT)` |

---

## 八、VS 使用备忘

- **补全**：随时 `Ctrl + Space` 手动召唤；打开
  `工具 → 选项 → 文本编辑器 → C/C++` 里"**删除字符后显示完成列表**"，
  退格回单词内也会重新弹列表；IntelliCode 开启后列表顶部有 ★ 智能推荐
- **补全失灵**：先 Ctrl+S 触发重解析；不行就关 VS 删项目下的 `.vs` 文件夹重启
- **布局**：面板位置全靠拖拽 + 吸附箭头；`窗口 → 重置窗口布局` 一键回默认；
  布局存在 `.vs` 里跟解决方案走
- **终端**：`Ctrl + ` ` 打开，点图钉钉住即常驻底部
- **行距/字体**：`工具 → 选项 → 文本编辑器 → 常规 → 行距`（1.15/1.5 更舒展）；
  `环境 → 字体和颜色` 改字号（"环境"管 IDE 框架字体，"文本编辑器"管代码字体）
