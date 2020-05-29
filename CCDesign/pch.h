/*
 | C课程设计
 | 文件名称: pch.h
 | 文件作用: 预编译头
 | 创建日期: 2020-05-06
 | 更新日期: 2020-05-13
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
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include <map>
#include <vector>
#include <time.h>
#include <math.h>
#include <string>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include <commctrl.h>
#include <functional>
#include <commdlg.h>
#include "sqlite3.h"
#pragma comment(lib, "comctl32.lib")
#define STACK_INIT(stk)             (stk).top = 0 
#define STACK_CLEAR(stk)            (stk).top = 0 
#define STACK_PUSH(stk, d)          (stk).dat[(stk).top++] = d
#define STACK_POP(stk)              (stk).dat[--(stk).top]
#define PSTACK_POP(stk)             (stk)->dat[--(stk)->top]
#define STACK_TOP(stk)              (stk).dat[(stk).top - 1]
#define PSTACK_TOP(stk)             (stk)->dat[(stk)->top - 1]
#define STACK_INDEX(stk, i)         (stk).dat[(stk).top - (i) - 1]
#define PSTACK_INDEX(pstk, i)       ((pstk)->dat [ (pstk)->top - (i) - 1])
#define STACK_SIZE(stk)             ((stk).top)
#define STACK_EMPTY(stk)            ((stk).top == 0)
#define BIT_TEST(n, t)            (((n) & (t)) == (t))
#define ARRAY_SZ(n)                 (sizeof(n) / sizeof((n)[0]))
// 下面的宏用于实现get/set
#define  property_r(typ, name)   __declspec(property(get = __property_get_ ## name)) typ name;   \
                                 typedef typ __property_typ_ ## name
#define  property_w(typ, name)   __declspec(property(put = __property_set_ ## name)) typ name;   \
                                 typedef typ __property_typ_ ## name
#define  property_rw(typ, name)  __declspec(property(get = __property_get_ ## name, put = __property_set_ ## name)) typ name;   \
                                 typedef typ __property_typ_ ## name
#define  set(name)               void        __property_set_ ## name (__property_typ_ ## name value)
#define  get(name)               __property_typ_ ## name __property_get_ ## name ()
typedef TCHAR tchar;                // tchar, 提供不同版本的char(ANSI: char, Unicode: wchar_t)
#ifdef _UNICODE
typedef std::wstring tstring;       // tstring, 提供不同版本的std::string
#else
typedef std::string  tstring;
#endif
typedef double fltdat;
// 分配内存空间
extern void* __memalloc(size_t sz_count);
#define memalloc(typ, count)     (typ*)__memalloc((count) * sizeof(typ))
// 释放
extern void memfree(void *p);


