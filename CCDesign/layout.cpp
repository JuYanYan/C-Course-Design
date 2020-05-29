/*
 | C�γ����
 | �ļ�����: layout.cpp
 | �ļ�����: ���ֹ�����
 | ��������: 2020-05-06
 | ��������: 2020-05-18
 | ������Ա: JuYan
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
#include "layout.h"
// ������ť
void GUILayout::AddControl(const tchar *layout, IControl *ctrl)
{
    int strid;
    Region rgn;
    ControlInfo info;
    info.iCtrl = ctrl;
    _tcscpy_s(info.lay, layout);

    GetLayoutRegion(layout, &rgn, &strid);                              // ȡ�ò���

    ctrl->InitProperty(rgn, GetString(strid));                          // ˢ������

    ctrllst.emplace_back(info);
}
// ���Ĵ�С
void GUILayout::ResizeGrid(int new_w, int new_h)
{
    int strid;
    Region rgn;
    SetVarValue(new_w, new_h);
    for (auto & i : ctrllst)                                            // ˢ�����пؼ�������
    {
        GetLayoutRegion(i.lay, &rgn, &strid);
        i.iCtrl->SetProperty(rgn);
    }
}

