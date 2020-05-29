/*
 | C课程设计
 | 文件名称: mwin.h
 | 文件作用: 主窗口
 | 创建日期: 2020-05-06
 | 更新日期: 2020-05-23
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
#pragma once
#include "layout.h"
#include "toolbar.h"
#include "listview.h"
#include "databank.h"
class MainWin : 
    public GUILayout,
    public SQLDatabank
{
public:
    MainWin(HINSTANCE _hInst);
    ~MainWin() = default;
    bool InitInstant(const tchar *title, int nCmdShow);
    LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    HWND hWnd;
    HBRUSH bgBrush;
    HINSTANCE hInst;
    tchar winClass[20];
    // start -- 控件
    WToolBar toolbar;
    WListView lstView;
    // end -- 控件
    // 当前的刷新列表用的sql(admin模式)
    const tchar *curUpdateSQL;
    // 显示模式, false是一般模式, true会显示所有数据
    bool adminMode;
    //
    void getWindowCenterPos(int *x, int *y);
    void initGridLayout(HWND hWnd);
    bool loadDataBank(const tchar *file);
    void showDataOnList();
    void clearDataBank();
    void removeData();
    void exportData();
    void resetCond();
    //
    void changeMode();
    //-- 对话框消息处理
    static INT_PTR CALLBACK InsertDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		message,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    static INT_PTR CALLBACK UpdateDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		msg,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    static INT_PTR CALLBACK LogDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		msg,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    static INT_PTR CALLBACK InfoDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		msg,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    static INT_PTR CALLBACK CondDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		msg,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    static INT_PTR CALLBACK AboutDlgproc(
        _In_		HWND		hDlg,
        _In_		UINT		msg,
        _In_		WPARAM		wParam,
        _In_		LPARAM		lParam
    );
    //-- utils
    static void getWindowText(HWND win, tchar *str, int maxlen);
    static void getWindowText(HWND win, int id, tchar *str, int maxlen);
    static bool standardStr(tchar *str);
};
// FROM: main.cpp
// 消息处理回调
extern LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
