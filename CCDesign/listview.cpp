/*
 | C课程设计
 | 文件名称: listview.cpp
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
#include "pch.h"
#include "listview.h"
WListView::WListView()
{
    font = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
}
WListView::~WListView()
{
    if (font != NULL)
    {
        DeleteObject(font);
    }
}
void WListView::Create(HWND father, int id, const std::vector<const tchar *> &colName)
{
    colCount = (int)colName.size();
    if (colCount <= 0)
    {
        assert(0);
        abort();
    }
    hWnd = CreateWindow(
        WC_LISTVIEW,
        _T(""),
        WS_VISIBLE | WS_TABSTOP | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, 0,
        100, 100,                                               // 缺省尺寸
        father,
        (HMENU)id,
        NULL,
        NULL
    );
    if (!hWnd)
    {
        assert(0);
        abort();
    }
    // 设置拓展样式和字体
    constexpr DWORD extStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_FLATSB | LVS_EX_SINGLEROW;
    ListView_SetExtendedListViewStyle(hWnd, extStyle);
    if (font != NULL)
    {
        SendMessage(hWnd, WM_SETFONT, (WPARAM)font, TRUE);
    }
    // 预留128个
    ListView_SetItemCount(hWnd, 128);
    // 最后创建列
    SetColumn(hWnd, 84, colName);
}
// 添加项
void WListView::AppendItem(const std::vector<const tchar *> &data)
{
    int pos = ListView_GetItemCount(hWnd);                      // 插入到最后
    AppendItem(data, pos);
}
void WListView::AppendItem(const std::vector<const tchar *> &data, int pos)
{
    if (colCount != data.size())
    {
        assert(0);
        abort();
    }
    AppendItem(hWnd, data, pos);
}
void WListView::AppendItem(HWND hWnd, const std::vector<const tchar *> &data, int pos)
{
    int count;
    LVITEM vitem;
    tchar tmp[96];
    count = 0;
    vitem.mask = LVIF_TEXT;
    vitem.iItem = pos;
    vitem.pszText = tmp;
    for (auto &i : data)
    {
        _tcscpy_s(tmp, i);
        vitem.iSubItem = count;
        if (count == 0)                                         // 先添加项, 再设置其余子项的内容
        {
            ListView_InsertItem(hWnd, &vitem);
        }
        else {
            ListView_SetItem(hWnd, &vitem);
        }
        count += 1;
    }
}
// 更改一行的内容
void WListView::SetRowData(const tchar * data, int colpos, int rowpos)
{
    LVITEM vitem;
    tchar tmp[96];
    _tcscpy_s(tmp, data);
    vitem.mask = LVIF_TEXT;
    vitem.iItem = rowpos;                                       // 哪一行
    vitem.pszText = tmp;
    vitem.iSubItem = colpos;                                    // 对应是哪一列

    ListView_SetItem(hWnd, &vitem);
}
void WListView::SetRowData(const std::vector<const tchar *> &data, const std::vector<int> &colpos, int rowpos)
{
    int k;
    LVITEM vitem;
    tchar tmp[96];
    k = 0;
    vitem.mask = LVIF_TEXT;
    vitem.iItem = rowpos;                                       // 哪一行
    vitem.pszText = tmp;
    assert(data.size() == colpos.size());
    for (auto &i : data)
    {
        _tcscpy_s(tmp, i);
        vitem.iSubItem = colpos[k];                             // 对应是哪一列
        ListView_SetItem(hWnd, &vitem);
        k += 1;
    }
}
// 取得item
void WListView::GetItemData(tchar *res, int buffsz, int colpos)
{
    int idx = SendMessage(hWnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    assert(idx != -1);
    GetItemData(res, buffsz, colpos, idx);
}
void WListView::GetItemData(tchar *res, int buffsz, int colpos, int rowpos)
{
    LVITEM item;
    item.pszText = res;
    item.iSubItem = colpos;                                     // 在哪一列
    item.cchTextMax = buffsz;
    int _ = SendMessage(hWnd, LVM_GETITEMTEXT, rowpos, (LPARAM)(void*)&item);
    assert(_ > 1);
}
// 删除一行
void WListView::DeleteItem(int rowidx)
{
    SendMessage(hWnd, LVM_DELETEITEM, rowidx, 0);
}
// 清空
void WListView::ClearItem()
{
    ListView_DeleteAllItems(hWnd);
}
// 设置列的数据
void WListView::SetColumn(HWND hList, int colw, const std::vector<const tchar *> &colName)
{
    int count;
    tchar tmp[96];
    LV_COLUMN lvc;
    count = 0;
    lvc.cx = colw;
    lvc.fmt = LVCFMT_LEFT;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.pszText = tmp;
    for (auto &i : colName)                                     // 创建列
    {
        _tcscpy_s(tmp, i);
        SendMessage(hList, LVM_INSERTCOLUMN, count, (LPARAM)(void*)&lvc);
        count += 1;
    }
}
// 设置属性
void WListView::SetProperty(const Region &rgn)
{
    SetWindowPos(hWnd, NULL, rgn.x, rgn.y, rgn.w, rgn.h, SWP_NOZORDER);
}
void WListView::InitProperty(const Region &rgn, const tchar *text)
{
    SetProperty(rgn);
    // text属性无效
}
