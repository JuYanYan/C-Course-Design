/*
 | C课程设计
 | 文件名称: calc.h
 | 文件作用: 表达式计算
 | 创建日期: 2020-05-06
 | 更新日期: 2020-05-06
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
#include "strtable.h"
class LayCalc : public StrTable
{
public:
    LayCalc();
    ~LayCalc() = default;
    void SetVarValue(int w, int h);
    void GetLayoutRegion(const tchar *expr, Region * rgn, int *strid);
private:
    enum Word                                           // 终结符和非终结符
    {
        word_prop,   word_cutsym, word_set,    word_addsub, word_lsarc, 
        word_num,    word_comma,  word_muldiv, word_rsarc,  word_eof,
        word_stack_bottom,
        _word_nt_beg_,
        word_ntStat, word_ntPair, word_ntExpr, word_ntTerm, word_ntOnop, word_ntNum,

        dirWord_add, dirWord_sub, dirWord_mul, dirWord_div, dirWord_set,
        dirWord_val, dirWord_p_w, dirWord_p_h, dirWord_p_txt, dirWord_p_marign,
        dirWord_str,
        dirWord_unknow,
    };
    struct wordDat
    {
        int    word;
        int    dtyp;
        union
        {
            int strid;
            fltdat value;
        };
    };
    struct analyzeDat
    {
        int state;
        int word;
        int dtyp;
        union
        {
            int strid;
            fltdat value;
        } val;
    };
    struct analyzeStk
    {
        analyzeDat dat[24];
        int        top;                             // 栈顶
    };
    fltdat w, h;
    int resStrId;
    Region resRgn;                                  // 结果放在这
    const tchar *pstr;
    static const int lengthTable[12];
    static const int id2TermTable[12];
    static const short gotoTable[27][6];
    static const short actionTable[27][10];
    bool adHocSyntaxDirTrans(int rid, analyzeStk *stk, analyzeDat *dat);
    bool setPropertyValue(analyzeStk *stk, analyzeDat *dat);
    bool setMarginPropertyValue(analyzeStk *stk, analyzeDat *dat);
    bool calcUnaryOperator(analyzeStk *stk, analyzeDat *dat);
    bool calcBinaryOperator(analyzeStk *stk, analyzeDat *dat);
    bool lr1Parser();
    int  getGotoValue(int state, int non_terminal);
    bool getNextWord(wordDat *word);
    bool setPropertyWord(const tchar *name, wordDat *will_set);
};

