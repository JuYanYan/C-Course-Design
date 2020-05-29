/*
 | C课程设计
 | 文件名称: mwin.cpp
 | 文件作用: 主窗口交互逻辑
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
#include "pch.h"
#include "mwin.h"
#include "resource.h"
#define  WINDOW_WIDTH        640
#define  WINDOW_HEIGHT       400
#define  LISTVIEW_ID         40050
MainWin::MainWin(HINSTANCE _hInst)
{
    hInst = _hInst;
    LoadStringW(_hInst, IDC_CCDESIGN, winClass, ARRAY_SZ(winClass));
}
// 初始化实例
bool MainWin::InitInstant(const tchar *title, int nCmdShow)
{
    int x, y;
    RECT  rc;
    WNDCLASS wc = { 0 };
    bgBrush = CreateSolidBrush(0xf0f0f0);                                   // 窗口背景色刷子(纯色)
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);                            // 鼠标指针: 箭头
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);                            // 菜单
    wc.hbrBackground = bgBrush;                                             // 窗口背景
    wc.lpszClassName = winClass;
    if (!RegisterClass(&wc))
    {
        return false;
    }
    getWindowCenterPos(&x, &y);                                             // 屏幕居中
    hWnd = CreateWindow(
        winClass, 
        title, 
        WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        x,
        y,
        WINDOW_WIDTH,                                                       // 窗口宽度
        WINDOW_HEIGHT,                                                      // 窗口高度
        nullptr, 
        nullptr, 
        hInst, 
        nullptr
    );
    if (!hWnd)
    {
        return false;
    }
    GetClientRect(hWnd, &rc);
    SetVarValue(rc.right - rc.left, rc.bottom - rc.top);                    // 设置客户区大小
    initGridLayout(hWnd);                                                   // 创建窗口的所有gui控件
    ShowWindow(hWnd, nCmdShow);                                             // 显示并更新窗口
    UpdateWindow(hWnd);
    adminMode = false;                                                      // 默认是一般的显示模式
    curUpdateSQL = _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;");
    if (loadDataBank(_T("data.db")) == false)
    {
        assert(0);
    }
    return true;
}
// 主消息循环
LRESULT MainWin::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_GETMINMAXINFO:                                                  // 限制窗口大小
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 600;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 320;
        break;
    case WM_SIZE:                                                           // 窗口大小改变
        ResizeGrid(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_NOTIFY:
        if (wParam == LISTVIEW_ID)                                          // listview通知消息
        {
            assert(lstView.Handle == ((LPNMHDR)lParam)->hwndFrom);
            if (((LPNMHDR)lParam)->code == NM_DBLCLK)                       // 双击鼠标左键
            {
                if (lstView.SelectIndex != -1 && adminMode)
                {
                    DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_INFO), hWnd, InfoDlgproc, (LPARAM)(void*)this);
                }
            }
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDM_CHANGEMODE)                               // 更改权限
        {
            changeMode();
        }
        else if (LOWORD(wParam) == IDM_ABOUT)                               // 关于按钮
        {
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWnd, AboutDlgproc, (LPARAM)(void*)this);
        }
        else if (LOWORD(wParam) == IDM_LOGIN_OUT)                           // 打卡
        {
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN_OUT), hWnd, LogDlgproc, (LPARAM)(void*)this);
        }
        else if (adminMode)                                                 // 其余在非admin下不可使用
        {
            switch (LOWORD(wParam))                                         // 分析菜单或快捷键操作
            {
            case IDM_ADD:
                DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_EDIT), hWnd, InsertDlgproc, (LPARAM)(void*)this);
                break;
            case IDM_REMOVE:
                removeData();
                break;
            case IDM_CHANGE:
                DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_UPDATE), hWnd, UpdateDlgproc, (LPARAM)(void*)this);
                break;
            case IDM_SEARCH:
                DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_SEARCH), hWnd, CondDlgproc, (LPARAM)(void*)this);
                break;
            case IDM_SETEMPTY:
                clearDataBank();
                break;
            case IDM_EXPORT:
                exportData();
                break;
            case IDM_CLEARCOND:
                resetCond();
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        else {
            MessageBox(hWnd, _T("您没有权限进行这些操作"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;
    case WM_DESTROY:
        DeleteObject(bgBrush);
        CloseDatabank();                                                    // 关闭数据库
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
// 取得屏幕居中窗口出现的x, y
void MainWin::getWindowCenterPos(int *x, int *y)
{
    RECT rc;
    HWND hw;
    int  w, h, _x, _y;
    hw = GetDesktopWindow();
    GetClientRect(hw, &rc);
    w = rc.right - rc.left;
    h = rc.bottom - rc.top;
    _x = (w - WINDOW_WIDTH) / 2;
    _y = (h - WINDOW_HEIGHT) / 2;
    if (_x < 0)
    {
        _x = 0;
    }
    if (_y < 0)
    {
        _y = 0;
    }
    *x = _x;
    *y = _y;
}
// 主窗口的GUI布局
void MainWin::initGridLayout(HWND hWnd)
{
    toolbar.Create(hWnd,
    {
        { IDM_LOGIN_OUT, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_LOGIN)), _T("") },
        { IDM_ADD, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_ADD)), _T("") },
        { IDM_CHANGE, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_UPDATE)), _T("") },
        { IDM_REMOVE, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_REMOVE)), _T("") },
        { IDM_EXPORT, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_EXPORT)), _T("") },
        { IDM_SEARCH, LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_COND)), _T("") },
    });
    AddControl(_T("width=0; height=0; margin=0,0,0,0"), toolbar);
    lstView.Create(hWnd, LISTVIEW_ID,
    { 
        _T("工号"), _T("姓名"), _T("电话"), _T("岗位"), 
        _T("上班时间"), _T("下班时间"), _T("累计工时")
    });
    AddControl(_T("margin=32,0,0,0"), lstView);
}
// 载入数据库
bool MainWin::loadDataBank(const tchar *file)
{
    if (OpenDatabank(file) == false)
    {
        return false;
    }
    if (ExecuteSQLNoRet(_T("PRAGMA foreign_keys = ON;")) == false)              // 打开外键支持
    {                                                                           // 这个默认是关掉的...我们需要用外键完成两个表的关系
        return false;
    }
    if (ExistTable("WORKER") == false)                                          // 创建表
    {
        bool _ = ExecuteSQLNoRet(_T("CREATE TABLE WORKER( ")
                                        "ID INT PRIMARY KEY  NOT NULL, "
                                        "NAME           TEXT NOT NULL, "
                                        "PHONE          TEXT, "
                                        "OCCUPATION     TEXT NOT NULL "
                                    ");"
        );
        assert(_);

        adminMode = true;
        SetWindowText(hWnd, _T("建筑工地考勤管理系统 - 管理员"));
        MessageBox(hWnd, _T("数据库是空的, 您需要自己添加人员信息, 已为您切换模式为admin"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
    }
    if (ExistTable("WORKCHECK") == false)                                       // 创建表
    {
        bool _ = ExecuteSQLNoRet(_T("CREATE TABLE WORKCHECK( ")
                                        "ID             INT   NOT NULL, "
                                        "LOGINDATE      DATE  NOT NULL, "
                                        "LOGOUTDATE     DATE, "
                                        "WORKHOUR       FLOAT NOT NULL DEFAULT 0, "
                                        "FOREIGN KEY(ID) REFERENCES WORKER(ID)"
                                    ");"
        );
        assert(_);
    }
    showDataOnList();
    return true;
}
// 把data显示到listview上
void MainWin::showDataOnList()
{
    lstView.ClearItem();
    if (adminMode)
    {
        ExecuteSQLWithRet(curUpdateSQL, [&](std::vector<tstring> &lst) -> void
        {
            lstView.AppendItem(
            {
                lst[0].c_str(),
                lst[1].c_str(),
                lst[2].c_str(),
                lst[3].c_str(),
                lst[5].c_str(),
                lst[6].c_str(),
                lst[7].c_str(),
            });
        });
    }
    else {
        ExecuteSQLWithRet(_T("SELECT * FROM WORKCHECK;"), [&](std::vector<tstring> &lst) -> void
        {
            lstView.AppendItem(
            {
                lst[0].c_str(),
                _T(" "),
                _T(" "),
                _T(" "),
                lst[1].c_str(),
                lst[2].c_str(),
                lst[3].c_str(),
            });
        });
    }
}
// 清空数据库
void MainWin::clearDataBank()
{
    int ret;
    ret = MessageBox(hWnd, _T("此操作会删除所有数据, 并不可恢复, 是否继续?"), _T("考勤管理 - 提示"), MB_ICONINFORMATION | MB_YESNO);
    if (ret == 6)                                                               // 选择了Yes
    {
        bool _;
        _ = ExecuteSQLNoRet(_T("DELETE FROM WORKCHECK;"));
        assert(_);

        _ = ExecuteSQLNoRet(_T("DELETE FROM WORKER;"));
        assert(_);

        lstView.ClearItem();
        MessageBox(hWnd, _T("清空完成, 现在数据库是空的, 请添加数据"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
    }
}
// 移除数据
void MainWin::removeData()
{
    int selid = lstView.SelectIndex;
    if (selid == -1)
    {
        MessageBox(hWnd, _T("您必须选择一列才能进行删除操作, 若要清空, 请转到 菜单 '编辑' -> '清空'"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
        return;
    }
    tchar tmp[64], sql[256];
    lstView.GetItemData(tmp, 64, 0, selid);

    int ret;
    ret = MessageBox(hWnd, _T("您无法修改任何人员的考勤记录, 因此这会删掉该人的所有信息, 是否继续?"), _T("考勤管理 - 提示"), MB_ICONINFORMATION | MB_YESNO);
    if (ret == 6)
    {
        bool _;
        _stprintf_s(sql, _T("DELETE FROM WORKCHECK WHERE ID = %s;"), tmp);                  // 先从考勤表里面移除

        _ = ExecuteSQLNoRet(sql);
        assert(_);

        _stprintf_s(sql, _T("DELETE FROM WORKER WHERE ID = %s;"), tmp);                     // 再从职员表中移除, 这样就保证移除时不存在任何依赖

        _ = ExecuteSQLNoRet(sql);
        assert(_);
        // 刷新显示
        showDataOnList();
    }
}
// 导出报表
void MainWin::exportData()
{
    FILE *f;
    char *str;
    int len, maxlen;
    tchar fname[MAX_PATH] = { _T("考勤.csv") };
    OPENFILENAME ofn = { 0 };
retry:
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("csv(逗号分隔表格)\0*.csv\0\0");
    ofn.lpstrFile = fname;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = _T("考勤管理 - 导出数据");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    // TODO 该api在windows 8/8.1/10上存在未知bug
    // 更换为IDialog系列接口
    if (!GetSaveFileName(&ofn))
    {
        return;
    }
    _tfopen_s(&f, fname, _T("wb"));
    if (f == NULL)
    {
        MessageBox(hWnd, _T("保存文件失败, 请检查文件是否被占用"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
        goto retry;
    }
    str = NULL;
    maxlen = 0;
    // 以本地代码页保存
    fwrite("工号,姓名,联系方式,工作岗位,上班时间,下班时间,工时\r\n", 1, 52, f);
    ExecuteSQLWithRet(_T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;"), [&](std::vector<tstring> &lst) -> void
    {
        static const int order[] =
        {
            0, 1, 2, 3, 5, 6, 7
        };
        for (auto & j : order)
        {
            auto &i = lst[j];
            if (i.length() != 0)                                        // 保证存放的都是有效的数据
            {
                len = WideCharToMultiByte(CP_ACP, 0, i.c_str(), -1, NULL, 0, NULL, NULL);
                if (len > maxlen)                                       // 尽可能减少内存管理器的调用
                {
                    if (str != NULL)
                    {
                        memfree(str);
                    }
                    str = memalloc(char, len);
                    maxlen = len;
                }
                WideCharToMultiByte(CP_ACP, 0, i.c_str(), -1, str, len, NULL, NULL);
                fwrite(str, 1, len - 1, f);                             // len包括'\0', 它不应该出现在文件中
            }
            fwrite(",", 1, 1, f);
        }
        fwrite("\r\n", 1, 2, f);
    });
    if (str != NULL)
    {
        memfree(str);
    }
    fclose(f);
}
// 清除筛选
void MainWin::resetCond()
{
    curUpdateSQL = _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;");
    showDataOnList();
}
// 更改权限
void MainWin::changeMode()
{
    if (adminMode == false)
    {
        int ret;
        ret = MessageBox(hWnd, _T("此操作会将您的操作权限提升自管理员级别, 是否继续?"), _T("考勤管理 - 提示"), MB_ICONINFORMATION | MB_YESNO);
        // TODO 在这里可以加上密码啥的
        if (ret == 6)
        {
            adminMode = true;
            showDataOnList();
            SetWindowText(hWnd, _T("建筑工地考勤管理系统 - 管理员"));
        }
    }
    else {
        adminMode = false;
        showDataOnList();
        SetWindowText(hWnd, _T("建筑工地考勤管理系统"));
    }
}
// -------------------------------
// 对话框消息处理
// 插入对话框
INT_PTR CALLBACK MainWin::InsertDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    static MainWin *pthis;
    switch (msg)
    {
    case WM_INITDIALOG:
        pthis = (MainWin *)lParam;
        return (INT_PTR)TRUE;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            tchar name[128], id[128], phone[128], stat[128];
            tchar sql[764];
            getWindowText(hDlg, IDC_EDIT_WORKER_ID, id, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_NAME, name, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_STAT, stat, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_PHONENUM, phone, 128);
            if (standardStr(id) == false)
            {
                MessageBox(hDlg, _T("请输入一个有效的工号"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
                return (INT_PTR)TRUE;
            }
            standardStr(name);
            standardStr(stat);
            standardStr(phone);
            _stprintf_s(sql, _T("INSERT INTO WORKER (ID, NAME, PHONE, OCCUPATION) VALUES ('%s', '%s', '%s', '%s');"),
                id, name, phone, stat
            );                                                          // 因为ID是主键, 如果能插入那么工时就一定是0
            if (pthis->ExecuteSQLNoRet(sql) == false)
            {
                MessageBox(hDlg, _T("无法添加数据, 请检查工号是否唯一"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
                return (INT_PTR)TRUE;
            }
            else {                                                      // 刷新列表, 操作模式一定是admin
                pthis->lstView.AppendItem({ id, name, phone, stat, _T(" "), _T(" "), _T("0") });
            }
        }
        // IDOK和IDCANEL都在最后结束对话框
        case IDCANCEL:
        {
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            return (INT_PTR)TRUE;
        }
        break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// 修改对话框
INT_PTR CALLBACK MainWin::UpdateDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    static int selid;
    static MainWin *pthis;
    switch (msg)
    {
    case WM_INITDIALOG:
        pthis = (MainWin *)lParam;
        selid = pthis->lstView.SelectIndex;
        if (selid == -1)
        {
            MessageBox(hDlg, _T("您必须选择一列才能进行操作"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
            EndDialog(hDlg, LOWORD(wParam));
        }
        else {
            tchar name[128], id[128], phone[128], stat[128];
            pthis->lstView.GetItemData(id, 128, 0, selid);
            pthis->lstView.GetItemData(name, 128, 1, selid);
            pthis->lstView.GetItemData(stat, 128, 3, selid);
            pthis->lstView.GetItemData(phone, 128, 2, selid);

            SetDlgItemText(hDlg, IDC_EDIT_LOGIN_ID, id);
            SetDlgItemText(hDlg, IDC_EDIT_WORKER_NAME, name);
            SetDlgItemText(hDlg, IDC_EDIT_WORKER_STAT, stat);
            SetDlgItemText(hDlg, IDC_EDIT_WORKER_PHONENUM, phone);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            tchar name[128], id[128], phone[128], stat[128];
            tchar sql[764];
            getWindowText(hDlg, IDC_EDIT_WORKER_ID, id, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_NAME, name, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_STAT, stat, 128);
            getWindowText(hDlg, IDC_EDIT_WORKER_PHONENUM, phone, 128);
            int ret = MessageBox(hDlg, _T("该更改不可撤销, 是否继续?"), _T("考勤管理 - 提示"), MB_ICONINFORMATION | MB_YESNO);
            if (ret == 6)                                               // 工号不会被删, 所以不检查了
            {
                standardStr(name);
                standardStr(stat);
                standardStr(phone);
                _stprintf_s(sql, _T("UPDATE WORKER SET NAME='%s', PHONE='%s', OCCUPATION='%s' WHERE ID='%s';"),
                    name, phone, stat, id
                );
                bool _ = pthis->ExecuteSQLNoRet(sql);
                assert(_);

                // 刷新显示
                pthis->showDataOnList();

                EndDialog(hDlg, LOWORD(wParam));
            }
        }
        break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            break;
        }
        break;
    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)TRUE;
}
// 登录对话框
INT_PTR CALLBACK MainWin::LogDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    static MainWin *pthis;
    static std::map<int, time_t> logTable;
    switch (msg)
    {
    case WM_INITDIALOG:
        pthis = (MainWin *)lParam;
        return (INT_PTR)TRUE;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            tchar id[128];
            char  cond[148];
            getWindowText(hDlg, IDC_EDIT_LOGIN_ID, id, 128);

            auto iid = _ttoi(id);
            sprintf_s(cond, "ID=%d AND LOGINDATE = (SELECT LOGINDATE FROM WORKCHECK WHERE ID=%d ORDER BY LOGINDATE DESC LIMIT 1)", iid, iid);
            if (_tcslen(id) == 0)
            {
                MessageBox(hDlg, _T("你需要输入一个有效的工号"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
            }
            else if (pthis->ExistItemInTable("WORKCHECK", cond, "LOGINDATE") == false || pthis->ExistItemInTable("WORKCHECK", cond, "LOGOUTDATE"))
            {                                                                       // 是新的记录
                tchar sql[256];
                _stprintf_s(sql, _T("INSERT INTO WORKCHECK(ID, LOGINDATE) VALUES ('%d',  datetime('now','localtime'));"), iid);
                if (pthis->ExecuteSQLNoRet(sql) == false)                           // 由于外键, 不存在的id会导致失败
                {
                    MessageBox(hDlg, _T("打卡失败, 请检查工号"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
                }
                else {                                                              // 打卡成功
                    // 记录工时, 因为同一人只会同一时刻存在登入或登出的状态, 因此不会出现冲突
                    logTable[iid] = time(NULL);
                    // 刷新显示
                    pthis->showDataOnList();
                    // 结束对话框
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                }
            }
            else {                                                                  // 是null, 说明是登出操作, 不是登入
                tchar sql[384];
                double hour;
                if (logTable.count(iid) == 0)
                {
                    hour = 0;
                    // TODO 保存map记录
                    MessageBox(hDlg, _T("您已错过正确的打卡时间, 因此您的本次工时为0"), _T("考勤管理 - 提示"), MB_ICONINFORMATION);
                }
                else {
                    hour = (time(NULL) - logTable[iid]) / 3600.0;
                }
                _stprintf_s(
                    sql, 
                    _T("UPDATE WORKCHECK SET LOGOUTDATE = datetime('now','localtime'), WORKHOUR = '%lg' WHERE ID=%d AND LOGINDATE = ")
                       "(SELECT LOGINDATE FROM WORKCHECK WHERE ID=%d ORDER BY LOGINDATE DESC LIMIT 1);", 
                    hour,
                    iid, 
                    iid
                );
                bool _ = pthis->ExecuteSQLNoRet(sql);
                assert(_);
                // 刷新显示
                pthis->showDataOnList();
                EndDialog(hDlg, LOWORD(wParam));	                                // 结束对话框
                return (INT_PTR)TRUE;
            }
        }
        break;
        case IDCANCEL:
        {
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            return (INT_PTR)TRUE;
        }
        break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// 详细(统计信息)对话框
INT_PTR CALLBACK MainWin::InfoDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    static int id;
    static HWND hList;
    static MainWin *pthis;
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        int k = 0;
        double sum;
        tchar _id[128], sql[256], tmp[128];
        pthis = (MainWin *)lParam;

        hList = GetDlgItem(hDlg, IDC_INFO_LIST);

        WListView::SetColumn(hList, 128, { _T("上班时间"), _T("下班时间"), _T("累计工时") });
        ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

        pthis->lstView.GetItemData(_id, 128, 0);                        // 取得主窗口里面的列表选中的字串, 得到工号
        id = _ttoi(_id);
        // 填充列表
        sum = 0;
        _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK WHERE ID=%d;"), id);
        bool _ = pthis->ExecuteSQLWithRet(sql, [&](std::vector<tstring> &lst) -> void
        {
            WListView::AppendItem(hList,
            {
                lst[0].c_str(),
                lst[1].c_str(),
                lst[2].c_str(),
            }, k);
            sum += _ttof(lst[2].c_str());                               // 顺便计算总工时
        });
        assert(_);
        // 刷新那几个text, id是主键, 所以只会得到一个结果
        _stprintf_s(sql, _T("SELECT NAME,OCCUPATION FROM WORKER WHERE ID=%d;"), id);
        _ = pthis->ExecuteSQLWithRet(sql, [&](std::vector<tstring> &lst) -> void
        {
            _stprintf_s(tmp, _T("名字:  %s"), lst[0].c_str());
            SetDlgItemText(hDlg, IDC_STATIC_NAME, tmp);

            _stprintf_s(tmp, _T("岗位:  %s"), lst[1].c_str());
            SetDlgItemText(hDlg, IDC_STATIC_STATE, tmp);
        });
        assert(_);
        // 工时
        _stprintf_s(tmp, _T("工时:  %lg (总)"), sum);
        SetDlgItemText(hDlg, IDC_STATIC_WORKHOUR, tmp);
        // 组合框数据
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("所有时间"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("最近三月"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("最近一周"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("最近三天"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("仅今天"));
        // 设置组合框选中项是所有时间
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_SETCURSEL, 0, 0);
        return (INT_PTR)TRUE;
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            return (INT_PTR)TRUE;
            break;
        case IDC_COMBO_SHOWTIME:                                        // 组合框通知
            if (HIWORD(wParam) == CBN_SELCHANGE)                        // 选中项被改变
            {
                int k = 0;
                double sum;
                int selidx = SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_GETCURSEL, 0, 0);
                tchar sql[256], tmp[128];
                const tchar *format = NULL;
                assert(selidx != -1);
                // 选择合适的sql
                switch (selidx)
                {
                case 0:                                                 // 所有时间
                    format = _T("工时:  %lg (总)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK WHERE ID=%d;"), id);
                    break;
                case 1:                                                 // 最近3月
                    format = _T("工时:  %lg (三月)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-3 month');", id);
                    break;
                case 2:                                                 // 最近1周
                    format = _T("工时:  %lg (一周)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-7 day');", id);
                    break;
                case 3:                                                 // 最近三天
                    format = _T("工时:  %lg (三天)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-3 day');", id);
                    break;
                case 4:                                                 // 仅今天
                    format = _T("工时:  %lg (当日)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', 'start of day');", id);
                    break;
                default:
                    assert(0);
                    break;
                }
                // 刷新列表
                sum = 0;
                ListView_DeleteAllItems(hList);
                bool _ = pthis->ExecuteSQLWithRet(sql, [&](std::vector<tstring> &lst) -> void
                {
                    WListView::AppendItem(hList,
                        {
                            lst[0].c_str(),
                            lst[1].c_str(),
                            lst[2].c_str(),
                        }, k);
                    sum += _ttof(lst[2].c_str());                       // 顺便计算总工时
                });
                assert(_);
                // 更新工时
                _stprintf_s(tmp, format, sum);
                SetDlgItemText(hDlg, IDC_STATIC_WORKHOUR, tmp);
                return (INT_PTR)TRUE;
            }
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// 筛选对话框
INT_PTR CALLBACK MainWin::CondDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    static tchar id[128] = { 0 };
    static tchar sql[512] = { 0 };
    static tchar name[128] = { 0 };
    static tchar stat[128] = { 0 };
    static MainWin *pthis;
    switch (msg)
    {
    case WM_INITDIALOG:
        pthis = (MainWin *)lParam;
        SetDlgItemText(hDlg, IDC_EDIT_SEARCH_ID, id);
        SetDlgItemText(hDlg, IDC_EDIT_SEARCH_NAME, name);
        SetDlgItemText(hDlg, IDC_EDIT_SEARCH_STAT, stat);
        return (INT_PTR)TRUE;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            getWindowText(hDlg, IDC_EDIT_SEARCH_ID, id, 128);
            getWindowText(hDlg, IDC_EDIT_SEARCH_NAME, name, 128);
            getWindowText(hDlg, IDC_EDIT_SEARCH_STAT, stat, 128);
            if (id[0] == 0 && name[0] == 0 && stat[0] == 0)             // 三个都是空的
            {
                pthis->resetCond();
            }
            else {
                bool first = true;
                tchar tmp[256];
                pthis->curUpdateSQL = sql;
                // TODO SQL注入预防和正确性检查
                if (id[0] != 0)
                { 
                    first = false;
                    _stprintf_s(sql, _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID WHERE WORKER.ID LIKE '%s'"), id);
                }
                if (name[0] != 0)
                {
                    if (first)
                    {
                        _stprintf_s(sql, _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID WHERE WORKER.NAME LIKE '%s'"), name);
                    }
                    else {                                              // sql拼接
                        _stprintf_s(tmp, _T(" AND WORKER.NAME LIKE '%s'"), name);
                        _tcscat_s(sql, tmp);
                    }
                    first = false;
                }
                if (stat[0] != 0)
                {
                    if (first)
                    {
                        _stprintf_s(sql, _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID WHERE WORKER.OCCUPATION LIKE '%s'"), name);
                    }
                    else {                                              // sql拼接
                        _stprintf_s(tmp, _T(" AND WORKER.OCCUPATION LIKE '%s'"), stat);
                        _tcscat_s(sql, tmp);
                    }
                    first = false;
                }
                pthis->showDataOnList();                                // 刷新显示
            }
        }
        break;
        case IDC_BUTTON_CLEAR:
            pthis->resetCond();
            id[0] = 0;                                                  // 从起始处截断以清空
            name[0] = 0;
            stat[0] = 0;
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_ID, id);
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_NAME, name);
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_STAT, stat);
            break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            return (INT_PTR)TRUE;
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// 关于对话框
INT_PTR CALLBACK MainWin::AboutDlgproc(
    _In_		HWND		hDlg,
    _In_		UINT		msg,
    _In_		WPARAM		wParam,
    _In_		LPARAM		lParam
)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // 结束对话框
            return (INT_PTR)TRUE;
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// 取得文本
void MainWin::getWindowText(HWND win, tchar *str, int maxlen)
{
    int len = SendMessage(win, WM_GETTEXTLENGTH, 0, 0);
    if (len >= maxlen - 1)
    {
        len = maxlen - 2;
    }
    SendMessage(win, WM_GETTEXT, len + 1, (LPARAM)(void*)str);
    str[len + 1] = _T('\0');
}
void MainWin::getWindowText(HWND win, int id, tchar *str, int maxlen)
{
    getWindowText(GetDlgItem(win, id), str, maxlen);
}
// 规范化字符串, 如果处理完发现长度是0返回false
bool MainWin::standardStr(tchar *str)
{
    tchar *tmp;
    tchar *beg;
    int len = (int)_tcslen(str);
    if (len == 0)
    {
        return false;
    }
    tmp = (tchar*)alloca(sizeof(tchar) * (len + 1));                // 在栈上分配内存
    _tcscpy_s(tmp, len + 1, str);
    while (*tmp && *tmp == _T(' '))                                 // 前导空格
    {
        tmp++;
    }
    beg = str;
    while (*tmp)                                                    // 拷贝数据
    {
        *str = *tmp;
        str++;
        tmp++;
    }
    while (str >= beg && *str == _T(' '))                           // 后面的空格
    {
        str--;
    }
    *str = _T('\0');                                                // 封闭字符串
    if (str == beg)
    {
        return false;
    }
    return true;
}
