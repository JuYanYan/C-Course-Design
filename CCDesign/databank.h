/*
 | C课程设计
 | 文件名称: databank.h
 | 文件作用: sqlite3数据库的封装
 | 创建日期: 2020-05-18
 | 更新日期: 2020-05-30
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
class SQLDatabank
{
public:
	SQLDatabank();
	~SQLDatabank();
    bool OpenDatabank(const tchar *file);
    bool ExistTable(const char *name);
    bool ExistItemInTable(const char *table, const char *cond, const char *itemname);
    bool ExecuteSQLNoRet(const tchar *sql);
    bool ExecuteSQLWithRet(const tchar *sql, const std::function<void(std::vector<tstring> &lst)> &call);
    void CloseDatabank();
private:
    sqlite3 *db;
    static int  callback_exist_table(void *param, int argc, char **argv, char **azColName);
    static int  callback_item_is_null(void *param, int argc, char **argv, char **azColName);
    static int  callback_select_col(const void *param, int argc, char **argv, char **azColName);
    static void UTF16toUTF8(const wchar_t *utf16, char **utf8);
    static void UTF8toUTF16(const char *utf8, wchar_t **utf16);
};

