/*
 | C�γ����
 | �ļ�����: calc.cpp
 | �ļ�����: ���ʽ����
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
#include "calc.h"
#define EBIT             0x100
#define SBIT             0x200
#define RBIT             0x400
#define STATE_ACC        0x800

#define IS_NUMBER(c)     ((c) >= _T('0') && (c) <= _T('9'))
#define IS_IDENTIFITY(c) ( ((c) >= _T('a') && (c) <= _T('z')) || ((c) >= _T('A') && (c) <= _T('Z')) || ((c) == _T('_')) )
#define GET_STATE(a, t)  ((a) & ((t) - 1))
#define NTERM_ZEROID(x)  ((x) - _word_nt_beg_ - 1)

#define CALL_TRANS(name) ret = name ## (stk, dat)

#define ASSERT_SET(x, value)    do { if((x) == -1) { (x) = (value); } } while(0)
#define SING_WORD(_typ, _dtyp)  do { w.word = (_typ); w.dtyp = (_dtyp); pstr++; } while(0)
LayCalc::LayCalc()
{
    w = 0;
    h = 0;
}
// ���ñ���w��h�Ĵ�С
void LayCalc::SetVarValue(int _w, int _h)
{
    w = (fltdat)_w;
    h = (fltdat)_h;
}
// ȡ�ñ��ʽ��ֵ
void LayCalc::GetLayoutRegion(const tchar *expr, Region * rgn, int *strid)
{
    pstr = expr;
    resStrId = -1;
    resRgn.w = -1;
    resRgn.h = -1;
    resRgn.x = -1;
    resRgn.y = -1;
    if (lr1Parser() == false)
    {
        assert(0);
        abort();
    }
    resRgn.Normal();                                                        // �淶����
    *rgn = resRgn;
    *strid = (resStrId == -1 ? 0 : resStrId);
}
// �Ƶ�����
bool LayCalc::adHocSyntaxDirTrans(int rid, analyzeStk *stk, analyzeDat *dat)
{
    bool ret;
    switch (rid)                                                            // ����
    {                                                   //..................// ���չ�Լid�����Ƶ�
    case 4:                                             // Expr -> Term
    case 5:                                             // Term -> Onop
    case 6:                                             // Onop -> Num
    case 7:                                             //  Num -> num
        ret = true;
        dat->val = PSTACK_TOP(stk).val;                 // ��ֵ���Ų���ʽ��������
        break;
    case 11:                                            // Num -> ( Expr )
        ret = true;
        dat->val = PSTACK_INDEX(stk, 1).val;
        break;
    case 8:                                             // Onop -> addsub Num
        CALL_TRANS(calcUnaryOperator);
        break;
    case 9:                                             // Expr -> Expr addsub Term
    case 10:                                            // Term -> Term muldiv Onop
        CALL_TRANS(calcBinaryOperator);
        break;
    case 3:                                             // Pair -> prop = List
        CALL_TRANS(setPropertyValue);
        break;
    case 12:                                            // Pair -> prop = Expr , Expr , Expr , Expr
        CALL_TRANS(setMarginPropertyValue);
        break;
    case 1:                                             // Stat -> Pair
    case 2:                                             //       | Stat ; Pair
    default:                                            //..................// nothing to do
        ret = true;
        break;
    }
    return ret;
}
// ��������
// Pair -> prop = List
bool LayCalc::setPropertyValue(analyzeStk *stk, analyzeDat *dat)
{
    auto & p = PSTACK_INDEX(stk, 2).dtyp;
    auto & v = PSTACK_INDEX(stk, 0).val;
    switch (p)
    {
    case dirWord_p_txt:
        ASSERT_SET(resStrId, v.strid);
        break;
    case dirWord_p_w:
        ASSERT_SET(resRgn.w, (int)v.value);
        break;
    case dirWord_p_h:
        ASSERT_SET(resRgn.h, (int)v.value);
        break;
    default:
        assert(0);
        break;
    }
    return true;
}
// Pair -> prop = Expr , Expr , Expr , Expr
bool LayCalc::setMarginPropertyValue(analyzeStk *stk, analyzeDat *dat)
{
    auto & p = PSTACK_INDEX(stk, 8).dtyp;
    fltdat v[4] =
    {
        PSTACK_INDEX(stk, 6).val.value,
        PSTACK_INDEX(stk, 4).val.value,
        PSTACK_INDEX(stk, 2).val.value,
        PSTACK_INDEX(stk, 0).val.value,
    };
    assert(p == dirWord_p_marign);
    // left
    if (v[0] >= 0)
    {
        resRgn.x = (int)v[0];
    }
    // top
    if (v[1] >= 0)
    {
        resRgn.y = (int)v[1];
    }
    // right
    if (v[2] >= 0)                                                  // right����������, ��ô�������Grid������
    {
        if (v[0] >= 0)                                              // margin: 123, *, 123, *
        {
            ASSERT_SET(resRgn.w, (int)(w - v[2] - v[0]));
        }
        else {                                                      // margin: *, *, 123, *
            resRgn.x = (int)(w - resRgn.w - v[2]);
        }
    }                                                               // else: ����, ����width
    // bottom
    if (v[3] >= 0)
    {
        if (v[1] >= 0)
        {
            ASSERT_SET(resRgn.h, (int)(h - v[3] - v[1]));
        }
        else {
            resRgn.y = (int)(h - resRgn.h - v[3]);
        }
    }
    return true;
}
// ����һԪ�������ֵ
// Onop -> addsub Num
bool LayCalc::calcUnaryOperator(analyzeStk *stk, analyzeDat *dat)
{
    auto & o = PSTACK_INDEX(stk, 1).dtyp;
    auto & a = PSTACK_INDEX(stk, 0).val.value;
    auto & r = dat->val.value;
    switch (o)
    {
    case dirWord_add:
        break;
    case dirWord_sub:
        r = -a;
        break;
    default:
        assert(0);
        break;
    }
    return true;
    return true;
}
// �����Ԫ�������ֵ
// Expr -> Expr addsub Term
// Term -> Term muldiv Onop
bool LayCalc::calcBinaryOperator(analyzeStk *stk, analyzeDat *dat)
{
    auto & o = PSTACK_INDEX(stk, 1).dtyp;
    auto & a = PSTACK_INDEX(stk, 2).val.value;
    auto & b = PSTACK_INDEX(stk, 0).val.value;
    auto & r = dat->val.value;
    switch (o)
    {
    case dirWord_add:
        r = a + b;
        break;
    case dirWord_sub:
        r = a - b;
        break;
    case dirWord_mul:
        r = a * b;
        break;
    case dirWord_div:
        r = a / b;
        break;
    default:
        assert(0);
        break;
    }
    return true;
}
// ���  ����  ���ս��  ����ʽ����
//  1    1    Stat    Stat -> Pair                               Y
//  2    3    Stat    Stat -> Stat ; Pair                        Y
//  3    3    Pair    Pair -> prop = Expr                        Y
//  4    1    Expr    Expr -> Term                               Y
//  5    1    Term    Term -> Onop                               Y
//  6    1    Onop    Onop -> Num                                Y
//  7    1    Num      Num -> num                                Y
//  8    2    Onop    Onop -> addsub Num                         Y
//  9    3    Expr    Expr -> Expr addsub Term                   Y
// 10    3    Term    Term -> Term muldiv Onop                   Y
// 11    3    Num      Num -> ( Expr )                           Y
// 12    9    Pair    Pair -> prop = Expr , Expr , Expr , Expr   Y
// LR(1)�﷨������, ͬʱ�Ƶ�ת������, ��ɹ���
bool LayCalc::lr1Parser()
{
    bool ret;
    int state;
    int action;
    wordDat word;
    analyzeDat dat = { 0 };
    analyzeStk stk = { 0 };
    ret = true;
    dat.word = word_stack_bottom;
    dat.state = 0;
    STACK_INIT(stk);
    STACK_PUSH(stk, dat);
    if (getNextWord(&word) == false)
    {
        ret = false;
        goto err;
    }
    while (1)
    {
        state = STACK_TOP(stk).state;
        if (state < 0)                                                      // �������е��﷨�����������ܵ���stateС��0
        {
            ret = false;
            assert(0);
            break;
        }
        action = actionTable[state][word.word];
        if (action == STATE_ACC)                                            // ����״̬
        {
            break;
        }
        else if (BIT_TEST(action, RBIT))                                    // ��Լ����
        {
            int  rid, termlen;
            rid = GET_STATE(action, RBIT);

            ret = adHocSyntaxDirTrans(rid, &stk, &dat);                     // �Ƶ�����
            if (ret == false)
            {
                break;
            }

            termlen = lengthTable[rid - 1];                                 // rid��1��ʼ��, - 1 ������0��ʼ
            while (termlen-- > 0)
            {
                STACK_POP(stk);
            }
            state = STACK_TOP(stk).state;
            dat.dtyp = dirWord_unknow;                                      // ���ս������Ӧ�κ�����
            dat.word = id2TermTable[rid - 1];                               // rid��1��ʼ��, - 1 ������0��ʼ
            dat.state = getGotoValue(state, dat.word);
            STACK_PUSH(stk, dat);
        }
        else if (BIT_TEST(action, SBIT))                                    // ת�ƶ���
        {
            dat.dtyp = word.dtyp;
            dat.word = word.word;
            dat.state = GET_STATE(action, SBIT);
            dat.val.value = word.value;                                     // value������, �������ͻ´������union
            STACK_PUSH(stk, dat);
            if (getNextWord(&word) == false)
            {
                ret = false;
                break;
            }
        }
        else {                                                              // �﷨����
            // TODO: ��ϸ�Ĵ�������
            // error-id = GET_STATE(action, EBIT)
            ret = false;
            printf("������Ϣ: �޷�Ϊ����%d������״̬%d�µĶ���\n", word.word, state);
            break;
        }
    }
err:
    return ret;
}
// ȡ��goto���ֵ
int LayCalc::getGotoValue(int state, int non_terminal)
{
    int id = NTERM_ZEROID(non_terminal);
    assert(state >= 0);
    return (int)gotoTable[state][id];
}
// ȡ����һ������
bool LayCalc::getNextWord(wordDat *word)
{
    bool ret;
    wordDat w = { 0 };
    ret = true;
nextchar:
    switch (*pstr)
    {
    case _T('+'):
        SING_WORD(word_addsub, dirWord_add);
        break;
    case _T('-'):
        SING_WORD(word_addsub, dirWord_sub);
        break;
    case _T('*'):
        SING_WORD(word_muldiv, dirWord_mul);
        break;
    case _T('/'):
        SING_WORD(word_muldiv, dirWord_div);
        break;
    case _T('('):
        SING_WORD(word_lsarc, dirWord_unknow);
        break;
    case _T(')'):
        SING_WORD(word_rsarc, dirWord_unknow);
        break;
    case _T(';'):
        SING_WORD(word_cutsym, dirWord_unknow);
        break;
    case _T(','):
        SING_WORD(word_comma, dirWord_unknow);
        break;
    case _T('='):
        SING_WORD(word_set, dirWord_set);
        break;
    case _T('\0'):
        SING_WORD(word_eof, dirWord_unknow);
        break;
    case _T(' '):
        pstr++;                                                         // ��һ���ַ�
        goto nextchar;                                                  // Ȼ������ʶ��һ��
        break;
    default:
        if (IS_NUMBER(*pstr))                                           // ����
        {
            int n = 0;                                                  // ��������
            fltdat r = 0.1, v = 0;
            while (IS_NUMBER(*pstr))
            {
                n = n * 10 + *pstr - _T('0');
                pstr++;
            }
            if (*pstr && *pstr == _T('.'))                              // ת��С������(�����)
            {
                pstr++;
                while (IS_NUMBER(*pstr))
                {
                    v = v + (*pstr - _T('0')) * r;
                    r = r * 0.1;
                    pstr++;
                }
                w.value = n + v;                                        // �������� + С������
            }
            else {
                w.value = n;                                            // ֻ����������
            }
            w.word = word_num;
            w.dtyp = dirWord_val;
        }
        else if (IS_IDENTIFITY(*pstr))                                  // ����
        {
            tchar tmp[24], *ptmp;
            ptmp = tmp;
            while (IS_IDENTIFITY(*pstr))
            {
                *ptmp = *pstr;
                ptmp++;
                // TODO ���Խ��( ptmp )
                pstr++;
            }
            *ptmp = 0;                                                  // ����ַ���
            ret = setPropertyWord(tmp, &w);
        }
        else if (*pstr == _T('`'))                                      // �ַ���
        {
            tchar tmp[24], *ptmp;
            ptmp = tmp;
            pstr++;
            while (*pstr != _T('`'))
            {
                *ptmp = *pstr;
                ptmp++;
                // TODO BUGBUG ���Խ��( ptmp )
                pstr++;
            }
            pstr++;
            *ptmp = 0;
            w.word = word_num;
            w.dtyp = dirWord_str;
            w.strid = PushString(tmp);
        }
        else {
            ret = false;
        }
    }
    *word = w;
    return ret;
}
// ����name����������Ϣ��word
// TODO ����Ϊhash map
bool LayCalc::setPropertyWord(const tchar *name, wordDat *w)
{
    if (_tcscmp(name, _T("width")) == 0)                                // ѡ����ʵ�����Ȼ��set
    {
        w->word = word_prop;
        w->dtyp = dirWord_p_w;
    }
    else if (_tcscmp(name, _T("height")) == 0)
    {
        w->word = word_prop;
        w->dtyp = dirWord_p_h;
    }
    else if (_tcscmp(name, _T("margin")) == 0)
    {
        w->word = word_prop;
        w->dtyp = dirWord_p_marign;
    }
    else if (_tcscmp(name, _T("text")) == 0)
    {
        w->word = word_prop;
        w->dtyp = dirWord_p_txt;
    }
    else if (_tcscmp(name, _T("w")) == 0)
    {
        w->word = word_num;
        w->dtyp = dirWord_val;
        w->value = LayCalc::w;
    }
    else if (_tcscmp(name, _T("h")) == 0)
    {
        w->word = word_num;
        w->dtyp = dirWord_val;
        w->value = LayCalc::h;
    }
    else {
        return false;
    }
    return true;
}
const int LayCalc::lengthTable[12] =                                    // ����ʽ���򳤶ȱ�
{
    1, 3, 3, 1, 1, 1, 1, 2, 3, 3, 3, 9,
};
const int LayCalc::id2TermTable[12] =                                   // ����ʽ -> idӳ��
{
    word_ntStat, word_ntStat, word_ntPair, word_ntExpr, 
    word_ntTerm, word_ntOnop, word_ntNum,  word_ntOnop, 
    word_ntExpr, word_ntTerm, word_ntNum,  word_ntPair,
};
const short LayCalc::gotoTable[27][6] =                                 // �﷨��������ת��
{
     1,  2, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1,  6, -1, -1, -1, -1,
    -1, -1,  7,  8,  9, 11,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 17,
    -1, -1, -1, -1, -1, -1,
    -1, -1, 18,  8,  9, 11,
    -1, -1, -1, -1, -1, -1,
    -1, -1, 19,  8,  9, 11,
    -1, -1, -1, 20,  9, 11,
    -1, -1, -1, -1, 21, 11,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, 24,  8,  9, 11,
    -1, -1, -1, -1, -1, -1,
    -1, -1, 26,  8,  9, 11,
    -1, -1, -1, -1, -1, -1,
};
const short LayCalc::actionTable[27][10] =                              // �﷨������������
{
     3 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  4 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT, STATE_ACC,
     0 | EBIT,  1 | RBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  1 | RBIT,
     0 | EBIT,  0 | EBIT,  5 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     3 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  2 | RBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  2 | RBIT,
     0 | EBIT,  3 | RBIT,  0 | EBIT, 15 | SBIT,  0 | EBIT,  0 | EBIT, 14 | SBIT,  0 | EBIT,  0 | EBIT,  3 | RBIT,
     0 | EBIT,  4 | RBIT,  0 | EBIT,  4 | RBIT,  0 | EBIT,  0 | EBIT,  4 | RBIT, 16 | SBIT,  4 | RBIT,  4 | RBIT,
     0 | EBIT,  5 | RBIT,  0 | EBIT,  5 | RBIT,  0 | EBIT,  0 | EBIT,  5 | RBIT,  5 | RBIT,  5 | RBIT,  5 | RBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  6 | RBIT,  0 | EBIT,  6 | RBIT,  0 | EBIT,  0 | EBIT,  6 | RBIT,  6 | RBIT,  6 | RBIT,  6 | RBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  7 | RBIT,  0 | EBIT,  7 | RBIT,  0 | EBIT,  0 | EBIT,  7 | RBIT,  7 | RBIT,  7 | RBIT,  7 | RBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  8 | RBIT,  0 | EBIT,  8 | RBIT,  0 | EBIT,  0 | EBIT,  8 | RBIT,  8 | RBIT,  8 | RBIT,  8 | RBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 15 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT, 22 | SBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 15 | SBIT,  0 | EBIT,  0 | EBIT, 23 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  9 | RBIT,  0 | EBIT,  9 | RBIT,  0 | EBIT,  0 | EBIT,  9 | RBIT, 16 | SBIT,  9 | RBIT,  9 | RBIT,
     0 | EBIT, 10 | RBIT,  0 | EBIT, 10 | RBIT,  0 | EBIT,  0 | EBIT, 10 | RBIT, 10 | RBIT, 10 | RBIT, 10 | RBIT,
     0 | EBIT, 11 | RBIT,  0 | EBIT, 11 | RBIT,  0 | EBIT,  0 | EBIT, 11 | RBIT, 11 | RBIT, 11 | RBIT, 11 | RBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 15 | SBIT,  0 | EBIT,  0 | EBIT, 25 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT,  0 | EBIT,  0 | EBIT, 10 | SBIT, 12 | SBIT, 13 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,
     0 | EBIT, 12 | RBIT,  0 | EBIT, 15 | SBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT,  0 | EBIT, 12 | RBIT,
};
