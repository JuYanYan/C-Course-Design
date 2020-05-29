/*
 | C�γ����
 | �ļ�����: mwin.cpp
 | �ļ�����: �����ڽ����߼�
 | ��������: 2020-05-06
 | ��������: 2020-05-23
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
// ��ʼ��ʵ��
bool MainWin::InitInstant(const tchar *title, int nCmdShow)
{
    int x, y;
    RECT  rc;
    WNDCLASS wc = { 0 };
    bgBrush = CreateSolidBrush(0xf0f0f0);                                   // ���ڱ���ɫˢ��(��ɫ)
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);                            // ���ָ��: ��ͷ
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);                            // �˵�
    wc.hbrBackground = bgBrush;                                             // ���ڱ���
    wc.lpszClassName = winClass;
    if (!RegisterClass(&wc))
    {
        return false;
    }
    getWindowCenterPos(&x, &y);                                             // ��Ļ����
    hWnd = CreateWindow(
        winClass, 
        title, 
        WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        x,
        y,
        WINDOW_WIDTH,                                                       // ���ڿ��
        WINDOW_HEIGHT,                                                      // ���ڸ߶�
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
    SetVarValue(rc.right - rc.left, rc.bottom - rc.top);                    // ���ÿͻ�����С
    initGridLayout(hWnd);                                                   // �������ڵ�����gui�ؼ�
    ShowWindow(hWnd, nCmdShow);                                             // ��ʾ�����´���
    UpdateWindow(hWnd);
    adminMode = false;                                                      // Ĭ����һ�����ʾģʽ
    curUpdateSQL = _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;");
    if (loadDataBank(_T("data.db")) == false)
    {
        assert(0);
    }
    return true;
}
// ����Ϣѭ��
LRESULT MainWin::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_GETMINMAXINFO:                                                  // ���ƴ��ڴ�С
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 600;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 320;
        break;
    case WM_SIZE:                                                           // ���ڴ�С�ı�
        ResizeGrid(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_NOTIFY:
        if (wParam == LISTVIEW_ID)                                          // listview֪ͨ��Ϣ
        {
            assert(lstView.Handle == ((LPNMHDR)lParam)->hwndFrom);
            if (((LPNMHDR)lParam)->code == NM_DBLCLK)                       // ˫��������
            {
                if (lstView.SelectIndex != -1 && adminMode)
                {
                    DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_INFO), hWnd, InfoDlgproc, (LPARAM)(void*)this);
                }
            }
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDM_CHANGEMODE)                               // ����Ȩ��
        {
            changeMode();
        }
        else if (LOWORD(wParam) == IDM_ABOUT)                               // ���ڰ�ť
        {
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWnd, AboutDlgproc, (LPARAM)(void*)this);
        }
        else if (LOWORD(wParam) == IDM_LOGIN_OUT)                           // ��
        {
            DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN_OUT), hWnd, LogDlgproc, (LPARAM)(void*)this);
        }
        else if (adminMode)                                                 // �����ڷ�admin�²���ʹ��
        {
            switch (LOWORD(wParam))                                         // �����˵����ݼ�����
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
            MessageBox(hWnd, _T("��û��Ȩ�޽�����Щ����"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;
    case WM_DESTROY:
        DeleteObject(bgBrush);
        CloseDatabank();                                                    // �ر����ݿ�
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
// ȡ����Ļ���д��ڳ��ֵ�x, y
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
// �����ڵ�GUI����
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
        _T("����"), _T("����"), _T("�绰"), _T("��λ"), 
        _T("�ϰ�ʱ��"), _T("�°�ʱ��"), _T("�ۼƹ�ʱ")
    });
    AddControl(_T("margin=32,0,0,0"), lstView);
}
// �������ݿ�
bool MainWin::loadDataBank(const tchar *file)
{
    if (OpenDatabank(file) == false)
    {
        return false;
    }
    if (ExecuteSQLNoRet(_T("PRAGMA foreign_keys = ON;")) == false)              // �����֧��
    {                                                                           // ���Ĭ���ǹص���...������Ҫ��������������Ĺ�ϵ
        return false;
    }
    if (ExistTable("WORKER") == false)                                          // ������
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
        SetWindowText(hWnd, _T("�������ؿ��ڹ���ϵͳ - ����Ա"));
        MessageBox(hWnd, _T("���ݿ��ǿյ�, ����Ҫ�Լ������Ա��Ϣ, ��Ϊ���л�ģʽΪadmin"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
    }
    if (ExistTable("WORKCHECK") == false)                                       // ������
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
// ��data��ʾ��listview��
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
// ������ݿ�
void MainWin::clearDataBank()
{
    int ret;
    ret = MessageBox(hWnd, _T("�˲�����ɾ����������, �����ɻָ�, �Ƿ����?"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION | MB_YESNO);
    if (ret == 6)                                                               // ѡ����Yes
    {
        bool _;
        _ = ExecuteSQLNoRet(_T("DELETE FROM WORKCHECK;"));
        assert(_);

        _ = ExecuteSQLNoRet(_T("DELETE FROM WORKER;"));
        assert(_);

        lstView.ClearItem();
        MessageBox(hWnd, _T("������, �������ݿ��ǿյ�, ���������"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
    }
}
// �Ƴ�����
void MainWin::removeData()
{
    int selid = lstView.SelectIndex;
    if (selid == -1)
    {
        MessageBox(hWnd, _T("������ѡ��һ�в��ܽ���ɾ������, ��Ҫ���, ��ת�� �˵� '�༭' -> '���'"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
        return;
    }
    tchar tmp[64], sql[256];
    lstView.GetItemData(tmp, 64, 0, selid);

    int ret;
    ret = MessageBox(hWnd, _T("���޷��޸��κ���Ա�Ŀ��ڼ�¼, ������ɾ�����˵�������Ϣ, �Ƿ����?"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION | MB_YESNO);
    if (ret == 6)
    {
        bool _;
        _stprintf_s(sql, _T("DELETE FROM WORKCHECK WHERE ID = %s;"), tmp);                  // �ȴӿ��ڱ������Ƴ�

        _ = ExecuteSQLNoRet(sql);
        assert(_);

        _stprintf_s(sql, _T("DELETE FROM WORKER WHERE ID = %s;"), tmp);                     // �ٴ�ְԱ�����Ƴ�, �����ͱ�֤�Ƴ�ʱ�������κ�����

        _ = ExecuteSQLNoRet(sql);
        assert(_);
        // ˢ����ʾ
        showDataOnList();
    }
}
// ��������
void MainWin::exportData()
{
    FILE *f;
    char *str;
    int len, maxlen;
    tchar fname[MAX_PATH] = { _T("����.csv") };
    OPENFILENAME ofn = { 0 };
retry:
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = _T("csv(���ŷָ����)\0*.csv\0\0");
    ofn.lpstrFile = fname;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = _T("���ڹ��� - ��������");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    // TODO ��api��windows 8/8.1/10�ϴ���δ֪bug
    // ����ΪIDialogϵ�нӿ�
    if (!GetSaveFileName(&ofn))
    {
        return;
    }
    _tfopen_s(&f, fname, _T("wb"));
    if (f == NULL)
    {
        MessageBox(hWnd, _T("�����ļ�ʧ��, �����ļ��Ƿ�ռ��"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
        goto retry;
    }
    str = NULL;
    maxlen = 0;
    // �Ա��ش���ҳ����
    fwrite("����,����,��ϵ��ʽ,������λ,�ϰ�ʱ��,�°�ʱ��,��ʱ\r\n", 1, 52, f);
    ExecuteSQLWithRet(_T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;"), [&](std::vector<tstring> &lst) -> void
    {
        static const int order[] =
        {
            0, 1, 2, 3, 5, 6, 7
        };
        for (auto & j : order)
        {
            auto &i = lst[j];
            if (i.length() != 0)                                        // ��֤��ŵĶ�����Ч������
            {
                len = WideCharToMultiByte(CP_ACP, 0, i.c_str(), -1, NULL, 0, NULL, NULL);
                if (len > maxlen)                                       // �����ܼ����ڴ�������ĵ���
                {
                    if (str != NULL)
                    {
                        memfree(str);
                    }
                    str = memalloc(char, len);
                    maxlen = len;
                }
                WideCharToMultiByte(CP_ACP, 0, i.c_str(), -1, str, len, NULL, NULL);
                fwrite(str, 1, len - 1, f);                             // len����'\0', ����Ӧ�ó������ļ���
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
// ���ɸѡ
void MainWin::resetCond()
{
    curUpdateSQL = _T("SELECT * FROM WORKER LEFT OUTER JOIN WORKCHECK ON WORKER.ID = WORKCHECK.ID;");
    showDataOnList();
}
// ����Ȩ��
void MainWin::changeMode()
{
    if (adminMode == false)
    {
        int ret;
        ret = MessageBox(hWnd, _T("�˲����Ὣ���Ĳ���Ȩ�������Թ���Ա����, �Ƿ����?"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION | MB_YESNO);
        // TODO ��������Լ�������ɶ��
        if (ret == 6)
        {
            adminMode = true;
            showDataOnList();
            SetWindowText(hWnd, _T("�������ؿ��ڹ���ϵͳ - ����Ա"));
        }
    }
    else {
        adminMode = false;
        showDataOnList();
        SetWindowText(hWnd, _T("�������ؿ��ڹ���ϵͳ"));
    }
}
// -------------------------------
// �Ի�����Ϣ����
// ����Ի���
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
                MessageBox(hDlg, _T("������һ����Ч�Ĺ���"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
                return (INT_PTR)TRUE;
            }
            standardStr(name);
            standardStr(stat);
            standardStr(phone);
            _stprintf_s(sql, _T("INSERT INTO WORKER (ID, NAME, PHONE, OCCUPATION) VALUES ('%s', '%s', '%s', '%s');"),
                id, name, phone, stat
            );                                                          // ��ΪID������, ����ܲ�����ô��ʱ��һ����0
            if (pthis->ExecuteSQLNoRet(sql) == false)
            {
                MessageBox(hDlg, _T("�޷��������, ���鹤���Ƿ�Ψһ"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
                return (INT_PTR)TRUE;
            }
            else {                                                      // ˢ���б�, ����ģʽһ����admin
                pthis->lstView.AppendItem({ id, name, phone, stat, _T(" "), _T(" "), _T("0") });
            }
        }
        // IDOK��IDCANEL�����������Ի���
        case IDCANCEL:
        {
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            return (INT_PTR)TRUE;
        }
        break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// �޸ĶԻ���
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
            MessageBox(hDlg, _T("������ѡ��һ�в��ܽ��в���"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
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
            int ret = MessageBox(hDlg, _T("�ø��Ĳ��ɳ���, �Ƿ����?"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION | MB_YESNO);
            if (ret == 6)                                               // ���Ų��ᱻɾ, ���Բ������
            {
                standardStr(name);
                standardStr(stat);
                standardStr(phone);
                _stprintf_s(sql, _T("UPDATE WORKER SET NAME='%s', PHONE='%s', OCCUPATION='%s' WHERE ID='%s';"),
                    name, phone, stat, id
                );
                bool _ = pthis->ExecuteSQLNoRet(sql);
                assert(_);

                // ˢ����ʾ
                pthis->showDataOnList();

                EndDialog(hDlg, LOWORD(wParam));
            }
        }
        break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            break;
        }
        break;
    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)TRUE;
}
// ��¼�Ի���
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
                MessageBox(hDlg, _T("����Ҫ����һ����Ч�Ĺ���"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
            }
            else if (pthis->ExistItemInTable("WORKCHECK", cond, "LOGINDATE") == false || pthis->ExistItemInTable("WORKCHECK", cond, "LOGOUTDATE"))
            {                                                                       // ���µļ�¼
                tchar sql[256];
                _stprintf_s(sql, _T("INSERT INTO WORKCHECK(ID, LOGINDATE) VALUES ('%d',  datetime('now','localtime'));"), iid);
                if (pthis->ExecuteSQLNoRet(sql) == false)                           // �������, �����ڵ�id�ᵼ��ʧ��
                {
                    MessageBox(hDlg, _T("��ʧ��, ���鹤��"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
                }
                else {                                                              // �򿨳ɹ�
                    // ��¼��ʱ, ��Ϊͬһ��ֻ��ͬһʱ�̴��ڵ����ǳ���״̬, ��˲�����ֳ�ͻ
                    logTable[iid] = time(NULL);
                    // ˢ����ʾ
                    pthis->showDataOnList();
                    // �����Ի���
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                }
            }
            else {                                                                  // ��null, ˵���ǵǳ�����, ���ǵ���
                tchar sql[384];
                double hour;
                if (logTable.count(iid) == 0)
                {
                    hour = 0;
                    // TODO ����map��¼
                    MessageBox(hDlg, _T("���Ѵ����ȷ�Ĵ�ʱ��, ������ı��ι�ʱΪ0"), _T("���ڹ��� - ��ʾ"), MB_ICONINFORMATION);
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
                // ˢ����ʾ
                pthis->showDataOnList();
                EndDialog(hDlg, LOWORD(wParam));	                                // �����Ի���
                return (INT_PTR)TRUE;
            }
        }
        break;
        case IDCANCEL:
        {
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            return (INT_PTR)TRUE;
        }
        break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// ��ϸ(ͳ����Ϣ)�Ի���
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

        WListView::SetColumn(hList, 128, { _T("�ϰ�ʱ��"), _T("�°�ʱ��"), _T("�ۼƹ�ʱ") });
        ListView_SetExtendedListViewStyle(hList, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

        pthis->lstView.GetItemData(_id, 128, 0);                        // ȡ��������������б�ѡ�е��ִ�, �õ�����
        id = _ttoi(_id);
        // ����б�
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
            sum += _ttof(lst[2].c_str());                               // ˳������ܹ�ʱ
        });
        assert(_);
        // ˢ���Ǽ���text, id������, ����ֻ��õ�һ�����
        _stprintf_s(sql, _T("SELECT NAME,OCCUPATION FROM WORKER WHERE ID=%d;"), id);
        _ = pthis->ExecuteSQLWithRet(sql, [&](std::vector<tstring> &lst) -> void
        {
            _stprintf_s(tmp, _T("����:  %s"), lst[0].c_str());
            SetDlgItemText(hDlg, IDC_STATIC_NAME, tmp);

            _stprintf_s(tmp, _T("��λ:  %s"), lst[1].c_str());
            SetDlgItemText(hDlg, IDC_STATIC_STATE, tmp);
        });
        assert(_);
        // ��ʱ
        _stprintf_s(tmp, _T("��ʱ:  %lg (��)"), sum);
        SetDlgItemText(hDlg, IDC_STATIC_WORKHOUR, tmp);
        // ��Ͽ�����
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("����ʱ��"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("�������"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("���һ��"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("�������"));
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_ADDSTRING, 0, (LPARAM)(void*)_T("������"));
        // ������Ͽ�ѡ����������ʱ��
        SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_SETCURSEL, 0, 0);
        return (INT_PTR)TRUE;
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            return (INT_PTR)TRUE;
            break;
        case IDC_COMBO_SHOWTIME:                                        // ��Ͽ�֪ͨ
            if (HIWORD(wParam) == CBN_SELCHANGE)                        // ѡ����ı�
            {
                int k = 0;
                double sum;
                int selidx = SendDlgItemMessage(hDlg, IDC_COMBO_SHOWTIME, CB_GETCURSEL, 0, 0);
                tchar sql[256], tmp[128];
                const tchar *format = NULL;
                assert(selidx != -1);
                // ѡ����ʵ�sql
                switch (selidx)
                {
                case 0:                                                 // ����ʱ��
                    format = _T("��ʱ:  %lg (��)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK WHERE ID=%d;"), id);
                    break;
                case 1:                                                 // ���3��
                    format = _T("��ʱ:  %lg (����)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-3 month');", id);
                    break;
                case 2:                                                 // ���1��
                    format = _T("��ʱ:  %lg (һ��)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-7 day');", id);
                    break;
                case 3:                                                 // �������
                    format = _T("��ʱ:  %lg (����)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', '-3 day');", id);
                    break;
                case 4:                                                 // ������
                    format = _T("��ʱ:  %lg (����)");
                    _stprintf_s(sql, _T("SELECT LOGINDATE,LOGOUTDATE,WORKHOUR FROM WORKCHECK ")
                                        "WHERE ID=%d AND LOGINDATE >= datetime('now', 'localtime', 'start of day');", id);
                    break;
                default:
                    assert(0);
                    break;
                }
                // ˢ���б�
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
                    sum += _ttof(lst[2].c_str());                       // ˳������ܹ�ʱ
                });
                assert(_);
                // ���¹�ʱ
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
// ɸѡ�Ի���
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
            if (id[0] == 0 && name[0] == 0 && stat[0] == 0)             // �������ǿյ�
            {
                pthis->resetCond();
            }
            else {
                bool first = true;
                tchar tmp[256];
                pthis->curUpdateSQL = sql;
                // TODO SQLע��Ԥ������ȷ�Լ��
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
                    else {                                              // sqlƴ��
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
                    else {                                              // sqlƴ��
                        _stprintf_s(tmp, _T(" AND WORKER.OCCUPATION LIKE '%s'"), stat);
                        _tcscat_s(sql, tmp);
                    }
                    first = false;
                }
                pthis->showDataOnList();                                // ˢ����ʾ
            }
        }
        break;
        case IDC_BUTTON_CLEAR:
            pthis->resetCond();
            id[0] = 0;                                                  // ����ʼ���ض������
            name[0] = 0;
            stat[0] = 0;
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_ID, id);
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_NAME, name);
            SetDlgItemText(hDlg, IDC_EDIT_SEARCH_STAT, stat);
            break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            return (INT_PTR)TRUE;
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// ���ڶԻ���
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
            EndDialog(hDlg, LOWORD(wParam));	                        // �����Ի���
            return (INT_PTR)TRUE;
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// ȡ���ı�
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
// �淶���ַ���, ��������귢�ֳ�����0����false
bool MainWin::standardStr(tchar *str)
{
    tchar *tmp;
    tchar *beg;
    int len = (int)_tcslen(str);
    if (len == 0)
    {
        return false;
    }
    tmp = (tchar*)alloca(sizeof(tchar) * (len + 1));                // ��ջ�Ϸ����ڴ�
    _tcscpy_s(tmp, len + 1, str);
    while (*tmp && *tmp == _T(' '))                                 // ǰ���ո�
    {
        tmp++;
    }
    beg = str;
    while (*tmp)                                                    // ��������
    {
        *str = *tmp;
        str++;
        tmp++;
    }
    while (str >= beg && *str == _T(' '))                           // ����Ŀո�
    {
        str--;
    }
    *str = _T('\0');                                                // ����ַ���
    if (str == beg)
    {
        return false;
    }
    return true;
}
