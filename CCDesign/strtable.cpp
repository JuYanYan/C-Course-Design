/*
 | C�γ����
 | �ļ�����: strtable.cpp
 | �ļ�����: �ַ�����
 | ��������: 2020-05-14
 | ��������: 2020-05-14
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
#include "strtable.h"
StrTable::StrTable()
{
    count = 0;
    PushString(_T("unused"));                                                       // ȱʡ����
}
// ��ȫ���ַ������в���һ���ַ���
int StrTable::PushString(const tchar *str)
{
    if (stringTable.count(str) == 0)                                                // ֻ�����µ��ַ���
    {
        int id = GetGlobalStrID();
        stringTable[str] = id;
        stringIndexTable[id] = str;
        return id;
    }
    return stringTable[str];
}
// ȡ��һ���ַ���
const tchar * StrTable::GetString(int id)
{
    assert(id >= 0);
    if (stringIndexTable.count(id) == 0)
    {
        return _T("(unknow)");
    }
    return stringIndexTable[id].c_str();
}
// ȡ��Ψһ��id
int StrTable::GetGlobalStrID()
{
    return count++;
}


