/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：util.cpp
*   创 建 者：刘禹 finallyly liuyusi0121@sogou-inc.com(6141)
*   创建日期：2016年04月19日
*   描    述：
*   备    注: 用移位的方式实现utf8编码与unicode之间的转换
*   更新日志：unicode字节序有字节序分为大端(Big Endian)和小端(Little Endian)两种;
*             utf-8不区分字节序，本函数unicode用小端方式存储
*
================================================================*/


// please add your code here!
#include "util.h"
// #txt---
//       |  Unicode符号范围      |  UTF-8编码方式
//     n |  (十六进制)           | (二进制)
//     ---+-----------------------+------------------------------------------------------
//      1 | 0000 0000 - 0000 007F |                                              0xxxxxxx
//      2 | 0000 0080 - 0000 07FF |                                     110xxxxx 10xxxxxx
//      3 | 0000 0800 - 0000 FFFF |                            1110xxxx 10xxxxxx 10xxxxxx
//      4 | 0001 0000 - 0010 FFFF |                   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//      5 | 0020 0000 - 03FF FFFF |          111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//      6 | 0400 0000 - 7FFF FFFF | 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//
//      表 1. UTF-8的编码规则
// #txt---end
//

/*********************************************************************************
 *  params:
 *  c UTF-8字符串的首字节，根据首字节可以判断该UTF-8数组的实际长度
 *********************************************************************************/
int enc_get_utf8_size(uchar c)
{
    uchar t = 1 << 7;
    uchar r = c;
    int count = 0;
    while (r & t) {
    r = r << 1;
    count++;
    }
    return count;
}

/*****************************************************************************
 *  参数：
 *  unic unicode的编码值
 *  pOutput指向用于存储utf8编码的缓冲区的指针
 *  outSize pOutput缓冲区大小
 *  返回值:  返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 .
 *  注意:
 *   1. UTF8没有字节序问题, 但是Unicode有字节序要求;
 *      字节序分为大端(Big Endian)和小端(Little Endian)两种;
 *      我们这里采用小端表示
 *   2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小
 * **************************************************************************/

int enc_unicode_to_utf8_one(const UINT32 unic, uchar *pOutput, int outSize) {
    if (NULL == pOutput) {
        return 0;
    }
    if (unic <= 0x0000007F) {
        // * U-00000000 - U-0000007F:0xxxxxxx
        *pOutput = (unic & 0x7F);
        return 1;
    }
    else if (unic >= 0x00000080 and unic <= 0x000007FF) {
        *(pOutput+1) = (unic & 0x3F) | 0x80;
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    }
    else if (unic >= 0x00000800 && unic <= 0x0000FFFF) {
        *(pOutput+2) = (unic & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else if (unic >= 0x00010000 && unic <= 0x001FFFFF) {
        *(pOutput+3) = (unic & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    }
    else if (unic >= 0x00200000 && unic <= 0x03FFFFFF) {
        *(pOutput+4) = (unic & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    }
    else if (unic >= 0x04000000 && unic <= 0x7FFFFFFF ) {
        *(pOutput+5) = (unic & 0x3F) | 0x80;
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }
    return 0;
}


/************************************************************
 * 将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码.
 * 参数:
 *      pInput      指向输入缓冲区, 以UTF-8编码
 *      Unic        指向输出缓冲区, 其保存的数据即是Unicode编码值,
 *     类型为unsigned int
 *     返回值:
 *          成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0.
 *      注意:
 *          UTF8没有字节序问题, 但是Unicode有字节序要求;
 *          字节序分为大端(Big Endian)和小端(Little Endian)两种;
 ************************************************************/
int enc_utf8_to_unicode_one(const uchar* pInput, UINT32 *Unic) {
     if (NULL == pInput || NULL == Unic) {
         return 0;
     }
     // b1 表示UTF-8编码的pInput中的首字节, b2 表示二字节, ...
      char b1, b2, b3, b4, b5, b6;
      *Unic = 0x0; // 把 *Unic 初始化为全零
      int utfbytes = enc_get_utf8_size(*pInput);
      uchar *pOutput = (uchar *) Unic;
      int failure=0;
      switch ( utfbytes ) {
          case 0:
            *pOutput = *pInput;
            utfbytes += 1;
            break;
          case 2:
            b1 = *pInput;
            b2 = *(pInput + 1);
            if ( (b2 & 0xC0) != 0x80 ) {
                failure = 1;
            }
            else {
                *pOutput     = (b1 << 6) + (b2 & 0x3F);
                *(pOutput+1) = (b1 >> 2) & 0x07;
            }
            break;
          case 3:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) ) {
                failure = 1;
            }
            else {
                *pOutput = (b2 << 6) + (b3 & 0x3F);
                *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            }
            break;
          case 4:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                                    || ((b4 & 0xC0) != 0x80) ) {
                failure = 1;
            }
            else {
                 *pOutput = (b3 << 6) + (b4 & 0x3F);
                 *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
                 *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
            }
            break;
          case 5:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                        || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) ) {
                failure = 1;
            }
            else {
                *pOutput = (b3 << 6) + (b4 & 0x3F);
                *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
                *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
                *(pOutput+3) = (b1 << 6);
            }
            break;
          case 6:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            b6 = *(pInput + 5);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)|| ((b4 & 0xC0) != 0x80) 
                || ((b5 & 0xC0) != 0x80) || ((b6 & 0xC0) != 0x80) ) {
                failure = 1;
            }
            else {
                *pOutput     = (b5 << 6) + (b6 & 0x3F);
                *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
                *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
                *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F); 
            }
            break;
          default:
           failure=1; 
           break;
      }
      if (1==failure) {
          return 0;
      }
      else {
          return utfbytes;
      }
}
/******************************************************************
 *  参数：
 *      unicArray unicode字符数组
 *      unicArraySize unicode字符数组长度
 *  功能：
 *      在调用enc_unicode_to_utf8_batch 之前调用，
 *      预估unicode字符数组转换成utf8之后需要的字节数
 *  返回值:失败返回-1，否则返回utf8编码所需要的字节数
 *
 * ****************************************************************/
int enc_calc_utf8_outsize(const UINT32 *unicArray, int unicArraySize) {
    int count = 0;
    int failure = 0;
    for (int i = 0; i < unicArraySize;i++) {
        if (unicArray[i] <= 0x0000007F) {
            count += 1;
        }
        else if ( unicArray[i] >= 0x00000080 && unicArray[i] <= 0x000007FF ) {
            count += 2;
        }
        else if ( unicArray[i] >= 0x00000800 && unicArray[i] <= 0x0000FFFF ) {
            count += 3;
        }
        else if (unicArray[i] >= 0x00010000 && unicArray[i] <= 0x001FFFFF) {
            count += 4;
        }
        else if (unicArray[i] >= 0x00200000 && unicArray[i] <= 0x03FFFFFF) {
            count += 5;
        }
        else if (unicArray[i] >= 0x04000000 && unicArray[i] <= 0x7FFFFFFF) {
            count += 6;
        }
        else {
            failure = 1;
            break;
        }
    }
    if (0 == failure) {
        return count;
    }
    else {
        return -1;
    }
}

/***********************************************************
 * 参数
 *  pInput UTF-8字符串
 * 返回值
 *  UTF-8字符串转成unicode数组所需要的空间
 *  失败返回-1,空字符串返回0
 * 功能
 *  先于enc_utf8_to_unicode_batch调用，估算unicode数组大小
 *    
 * **********************************************************/
int enc_calc_unicode_outsize(const uchar *pInput) {
    int count = 0;
    int len = strlen((char *)pInput);
    int i = 0;
    int step = 0;
    while (i < len) {
        step = enc_get_utf8_size(*(pInput + i));
        if (0 == step) {
            step = 1;
        }
        count++;
        i += step;
    }
    if (i==len) {
        return count;
    }
    else {
        return -1;
    }

}


/**************************************************************************
 *  参数
 *      unicArray 需要转换的unicode数组
 *      unicArraySize unicode数组规模
 *      pOutput 存放转换后的utf8数组
 *      outSize 转换后的utf8数组大小，事先需要调用enc_calc_utf8_outsize计算得出
 *  返回值
 *      出错返回-1；
 *      正确返回 转换后的utf8数组大小
 *  注意 
 *      pOutput申请空间时，应该为outSize+1,最后一个用于存放字符串结尾符
 * 功能 
 *    unicode数组转成utf8
 * ***********************************************************************/
int enc_unicode_to_utf8_batch(const UINT32 *unicArray, int unicArraySize, 
    uchar *pOutput, int outSize) {
    int step = 0;
    int count = 0; 
    int failure = 0;
    int i = 0;
    int j = 0;
    int allcount = 0;
    while (i < unicArraySize) {
        count = enc_calc_utf8_outsize(unicArray+i, 1);
        allcount += count;
        step = enc_unicode_to_utf8_one(unicArray[i], pOutput+j, count);
        //如果转换失败或者是utf8所占字节数的二次校验不匹配，则失败
        if (0 == step ||(step != count)) {
            failure = 1;
            break;
        }
        i++;
        j += step;
    }
    if (i != unicArraySize) {
        failure = 1;
    }
    if (0==failure) {
        return allcount;
    }
    else {
        return -1;
    }
}
/*********************************************************************************
 *  参数 
 *      pInput, 输入的utf-8数组
 *      UnicSeq 存储转换后的unicode数组
 *      outSize UnicSeq数组大小，转换之前需要用enc_calc_unicode_outsize计算好
 *  返回值
 *      失败返回-1，成功返回 unicode数组个数
 *  备注
 *
 * ******************************************************************************/
int enc_utf8_to_unicode_batch(const uchar* pInput, UINT32 *UnicSeq, int outSize) {
    //不对结尾字符求unicode码
    int len = strlen((char *)pInput);
    int i = 0;
    int j = 0;
    int step = 0;
    int failure = 0;
    while (i < len) {
        step = enc_utf8_to_unicode_one(pInput + i, UnicSeq + j);
        if (0 == step) {
            failure =1 ;
            break;
        }
        i+=step;
        j++;
    }
    if (i!=len) {
        failure =1;
    }
    if ( 0 == failure) {
        return  j;
    }
    else {
        return -1;
    }
}

//int main(int argc, char ** argv) {
    //单子测试
    /*
    UINT32 acode = 0x5218;
    uchar *buf = new uchar[6];
    enc_unicode_to_utf8_one(acode,buf,6);
    fprintf(stdout,"%0x对应的汉字是%s\n",acode,buf);
    char *src = "禹";
    UINT32 bcode = 0;
    enc_utf8_to_unicode_one((uchar*)src, &bcode);
    fprintf(stdout,"%s对应的码字是%0x\n",src,bcode);
    delete buf;
    */
    /*  
    //数组测试
    UINT32 *codeseq_original = NULL;
    UINT32 *codeseq_convert = NULL;
    char *utf8str_original = "刘禹读liúyǔ,のた";
    uchar *utf8str_convert = NULL;
    //先进行utf8数组转unicode的操作
    //估计unicode数组size;
    int unic_size = enc_calc_unicode_outsize((uchar *)utf8str_original);
    codeseq_convert = new UINT32 [unic_size];
    int realsize=enc_utf8_to_unicode_batch((uchar *)utf8str_original, codeseq_convert,unic_size);
    fprintf(stdout, "unic_size=%d\n",unic_size);
    fprintf(stdout,"utf8str_original=%s\n",utf8str_original);
    //二次校验
    if (realsize==unic_size) {
        for (int i = 0; i < unic_size; i++) {
            fprintf(stdout,"%0x",codeseq_convert[i]);
            if (i < unic_size - 1 ) {
                fprintf(stdout," ");
            }
        }
        fprintf(stdout,"\n");
    }
    else {
        fprintf(stdout, "额，utf8转unicode失败了\n");
    }
    //unicode数组转utf8的操作
    codeseq_original= new UINT32 [unic_size];
    memcpy(codeseq_original,codeseq_convert, unic_size* sizeof(UINT32));
    fprintf(stdout,"pcodeseq_original\n");
    for (int i = 0; i < unic_size; i++) {
        fprintf(stdout,"%0x",codeseq_convert[i]);
        if (i < unic_size - 1 ) {
                fprintf(stdout," ");
        }
    }
    fprintf(stdout,"\n");
    fprintf(stdout,"%d\n",MAX_WORD_LEN);
    //估计需要的utf8字符数组的size
    int  utf8_size = enc_calc_utf8_outsize(codeseq_original,unic_size);
    fprintf(stdout,"utf8_size=%d\n",utf8_size);
    //多申请一个存放结尾字符
    utf8str_convert = new uchar [utf8_size + 1];
    realsize = enc_unicode_to_utf8_batch(codeseq_original,unic_size,utf8str_convert,utf8_size);
    fprintf(stdout,"realsize=%d\n",realsize);
    utf8str_convert[utf8_size] = 0;
    if (realsize == utf8_size) {
        fprintf(stdout,"%s\n",(char *)utf8str_convert);
    }
    else {
        fprintf(stdout, "额， unicode数组转utf-8失败了\n");
    }
    
    if (NULL != codeseq_convert) {
        delete [] codeseq_convert;
        codeseq_convert = NULL;
    }

    if (NULL != codeseq_original) {
        delete [] codeseq_original;
        codeseq_original = NULL;
    }
    if (NULL != utf8str_convert) {
        delete [] utf8str_convert;
        utf8str_convert = NULL;
    }
    return 0;
}
*/
