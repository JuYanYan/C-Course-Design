/*
 | C课程设计
 | 文件名称: strtable.cpp
 | 文件作用: 字符串表
 | 创建日期: 2020-05-14
 | 更新日期: 2020-05-14
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
#include "strtable.h"
StrTable::StrTable()
{
    count = 0;
    PushString(_T("unused"));                                                       // 缺省设置
}
// 往全局字符串表中插入一个字符串
int StrTable::PushString(const tchar *str)
{
    if (stringTable.count(str) == 0)                                                // 只插入新的字符串
    {
        int id = GetGlobalStrID();
        stringTable[str] = id;
        stringIndexTable[id] = str;
        return id;
    }
    return stringTable[str];
}
// 取得一个字符串
const tchar * StrTable::GetString(int id)
{
    assert(id >= 0);
    if (stringIndexTable.count(id) == 0)
    {
        return _T("(unknow)");
    }
    return stringIndexTable[id].c_str();
}
// 取得唯一的id
int StrTable::GetGlobalStrID()
{
    return count++;
}


