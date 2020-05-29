/*
 | C课程设计
 | 文件名称: toolbar.h
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
#pragma once
#include "icontrol.h"
class WToolBar : public IControl
{
public:
	WToolBar();
	~WToolBar();
    struct ToolbarBtn
    {
        int  id;
        HBITMAP bmp;
        const tchar *str;
    };
    void Create(HWND father, const std::vector<ToolbarBtn> &button);
    property_r(HWND, Handle);                       // 窗口句柄
    get(Handle)
    {
        return hWnd;
    }
private:
    HWND  hWnd;
    HFONT font;
    HIMAGELIST imglist;
    void SetProperty(const Region &rgn);
    void InitProperty(const Region &rgn, const tchar *text);
};
