/*
 | C课程设计
 | 文件名称: main.cpp
 | 文件作用: 主程序入口点
 | 创建日期: 2020-05-06
 | 更新日期: 2020-05-06
 | 开发人员: JuYan
 +----------------------------
 MIT License

 Copyright (C) JuYan

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/
#include "pch.h"
#include "mwin.h"
#include "resource.h"
static MainWin *win;
int APIENTRY wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow
)
{
    MSG msg;
    HACCEL hAccelTable;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
#if _DEBUG                                                          // 分配一个控制台方便debug
    FILE *stream, *ins;
    AllocConsole();
    setlocale(LC_CTYPE, "chs");                                     // chs = Chinese Simple, 设置LCP

    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&ins, "CONIN$", "r", stdin);
#endif
    win = new MainWin(hInstance);
    if (win->InitInstant(_T("建筑工地考勤管理系统"), nCmdShow) == false)
    {
        MessageBox(0, _T("\"建筑工地考勤管理系统\"无法完成实例化, 可以尝试重新打开它, 但不一定有效"), _T("建筑工地考勤管理系统 - 错误"), MB_ICONINFORMATION);
        goto err;
    }
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CCDESIGN));
    while (GetMessage(&msg, nullptr, 0, 0))                         // 主消息循环
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
err:
    delete win;
    return (int)msg.wParam;
}
extern LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return win->MsgProc(hWnd, message, wParam, lParam);
}

