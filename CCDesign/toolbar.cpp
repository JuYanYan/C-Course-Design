/*
 | C课程设计
 | 文件名称: toolbar.cpp
 | 文件作用: 纵向工具条
 | 创建日期: 2020-05-18
 | 更新日期: 2020-05-18
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
#include "toolbar.h"
WToolBar::WToolBar()
{
    font = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
    imglist = NULL;
}
WToolBar::~WToolBar()
{
    if (font != NULL)
    {
        DeleteObject(font);
    }
    if (imglist != NULL)
    {
        ImageList_Destroy(imglist);
    }
}
// 创建, 垂直的工具条
void WToolBar::Create(HWND father, const std::vector<ToolbarBtn> &button)
{
    hWnd = CreateWindow(
        TOOLBARCLASSNAME, 
        NULL,
        WS_CHILD | WS_VISIBLE | CCS_VERT | TBSTYLE_FLAT,
        0, 0, 0, 0,
        father, 
        NULL,
        NULL,
        NULL
    );
    if (!hWnd)
    {
        assert(0);
        abort();
    }
    if (font != NULL)
    {
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
    }
    int k, bmpid;
    TBBUTTON *btn;
    k = 0;
    btn = memalloc(TBBUTTON, button.size());
    memset(btn, 0, sizeof(TBBUTTON) * button.size());
    imglist = ImageList_Create(24, 24, ILC_COLOR24, button.size(), 0);
    for (auto & i : button)                                                 // 构造tbbutton
    {
        bmpid = ImageList_Add(imglist, i.bmp, 0);
        btn[k].iBitmap = MAKELONG(bmpid, 0);
        btn[k].idCommand = i.id;
        btn[k].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
        btn[k].fsStyle = BTNS_BUTTON;
        btn[k].iString = (INT_PTR)i.str;
        k += 1;
    }
    SendMessage(hWnd, TB_SETIMAGELIST, 0, (LPARAM)imglist);                 // 设置image list

    SendMessage(hWnd, TB_LOADIMAGES, (WPARAM)IDB_STD_LARGE_COLOR, (LPARAM)HINST_COMMCTRL);

    SendMessage(hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWnd, TB_ADDBUTTONS, button.size(), (LPARAM)(void*)btn);


    memfree(btn);
}
// 设置属性
void WToolBar::SetProperty(const Region &rgn)
{
    //SetWindowPos(hWnd, NULL, rgn.x, rgn.y, rgn.w, rgn.h, SWP_NOZORDER);
}
void WToolBar::InitProperty(const Region &rgn, const tchar *text)
{
    SetProperty(rgn);
    // text属性无效
}
