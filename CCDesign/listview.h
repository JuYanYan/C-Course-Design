/*
 | C课程设计
 | 文件名称: listview.h
 | 文件作用: 列表框(ListView)
 | 创建日期: 2020-05-17
 | 更新日期: 2020-05-20
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
class WListView : public IControl
{
public:
	WListView();
	~WListView();
    void Create(HWND hWnd, int id, const std::vector<const tchar *> &colName);
    void AppendItem(const std::vector<const tchar *> &data);
    void AppendItem(const std::vector<const tchar *> &data, int pos);
    void SetRowData(const tchar * data, int colpos, int rowpos);
    void SetRowData(const std::vector<const tchar *> &data, const std::vector<int> &colpos, int rowpos);
    void GetItemData(tchar *res, int buffsz, int colpos);
    void GetItemData(tchar *res, int buffsz, int colpos, int rowpos);
    void DeleteItem(int rowidx);
    void ClearItem();
    property_r(HWND, Handle);                       // 窗口句柄
    get(Handle)
    {
        return hWnd;
    }
    property_r(int, SelectIndex);                  // 选中的行
    get(SelectIndex)
    {
        return SendMessage(hWnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    }
    static void SetColumn(HWND hList, int colw, const std::vector<const tchar *> &colName);
    static void AppendItem(HWND hWnd, const std::vector<const tchar *> &data, int pos);
private:
    HWND  hWnd;
    HFONT font;
    int colCount;
    void SetProperty(const Region &rgn);
    void InitProperty(const Region &rgn, const tchar *text);
};

