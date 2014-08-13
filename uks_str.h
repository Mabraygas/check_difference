/*
 * uks_str.h
 *
 *  Created on: 2011-5-9
 *      Author: wei.liu
 */

#ifndef UKS_STR_H_
#define UKS_STR_H_

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>


// 将 $value 的高位置在 $szBuf[0] 处，即看上去 $szBuf 打印出来是 $value 的 bit 位的直接展示
inline void  U64To01Txt(uint64_t value, char * szBuf) {
	for (int i = 0; i < 64; i++) {
		szBuf[63-i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[64] = 0;
}
// 将
inline void  U16To01Txt(uint16_t value, char * szBuf) {
	for (int i = 0; i < 16; i++) {
		szBuf[15 - i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[16] = 0;
}
// 将
inline void  U8To01Txt(uint8_t value, char * szBuf) {
	for (int i = 0; i < 8; i++) {
		szBuf[7-i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[8] = 0;
}

// 将 "00010110100" 形式的字符串转换为 u64， $szBuf 至少要有 64B 大小
inline void  TxtToU64(const char * sBuf, uint64_t * pddwVal) {
	uint64_t  ddwVal = 0;
	for (int i = 0; i < 64; i++) {
		ddwVal |= ((uint64_t)('1' == sBuf[i])) << (63 - i);
	}
	*pddwVal = ddwVal;
}

// 将 "00010110100" 形式的字符串转换为 u16， $szBuf 至少要有 16B 大小
inline void  TxtToU16(const char * sBuf, uint16_t * pwVal) {
	uint16_t  wVal = 0;
	for (int i = 0; i < 16; i++) {
		wVal |= ('1' == sBuf[i]) << (15 - i);
	}
	*pwVal = wVal;
}

// 将 "00010110100" 形式的字符串转换为 u16， $szBuf 至少要有 16B 大小
inline void  TxtToU8(const char * sBuf, uint8_t * pcVal) {
	uint8_t  cVal = 0;
	for (int i = 0; i < 8; i++) {
		cVal |= ('1' == sBuf[i]) << (7 - i);
	}
	*pcVal = cVal;
}

// 查找 $szIn 中最后一个 $szNeedle
// 如果 $szNeedle 为 0 长字符串，则直接返回指向 $szIn 末尾 0 字节处的指针
// $szIn 和 $szNeedle 不能为 NULL
inline char *  Strrstr(const char * szIn, const char * szNeedle) {
	if (0 == szNeedle[0]) {
		return  (char *)szIn + strlen(szIn);
	}
	char * pstr = NULL;
    char * szIn2 = new char[strlen(szIn) + 10];
    char* szNeedle2 = new char[strlen(szNeedle) + 10];
    strcpy(szIn2, szIn);
    strcpy(szNeedle2, szNeedle);
    char * plast = strstr(szIn2, szNeedle2);
	
    if (! plast) {
		return  NULL;
	}
	while (1) {
		pstr = strstr(plast + 1, szNeedle2);
		if (pstr != NULL) {
			plast = pstr;
		}
		else {
			break;
		}
	}
	return plast;
}


inline int uks_gbk_trunk(char * szGbk, int iLimit) {
	char * pr = szGbk;
	int  iStrLen = 0;
	while (*pr) {
		if (*pr < 0) { // gbk字符
			iStrLen ++;
			pr ++;
			if (*pr < 0) { // 应当也是 gbk 字符
				if (iStrLen >= iLimit) {
					iStrLen --;
					pr --; // 双字节字符中的一个字节越位，则需要回退一个字节
					break;
				}
				iStrLen ++;
				pr ++;
			} else { // gbk两个字节都是 < 0 的编码，这里检测半个字
				return  -1;
			}
		} else {
			pr ++;
			iStrLen ++;
		}
		if (iStrLen >= iLimit) {
			break;
		}
	}
	szGbk[iStrLen] = 0;
	return  pr - szGbk;
}


inline int uks_str_explode(const char * szStr, const char ch, char * pSubStr[], const int iAryLimt, const int iStrLimt) {
	long i = 0;
	long lInput = strlen(szStr);
	long iLast = -1;//上一个分隔字符 $ch 的位置
	int  iSub = 0;
	if (lInput < 1) {
		return  0;
	}
	if (iAryLimt < 1) {
		return  0;
	}
	if (iStrLimt < 2) {
		return  0;
	}
	for (i = 0; i < lInput; i++) {
		if (ch == szStr[i]) {
			long iSubLen = i - iLast - 1;
			if (iSubLen > 0) {
				if (iSubLen > iStrLimt - 1) {
					iSubLen = iStrLimt - 1;
				}
				memcpy(pSubStr[iSub], &szStr[iLast + 1], iSubLen);
				pSubStr[iSub][iSubLen] = 0;
				if (++iSub >= iAryLimt) {
					return  iSub;
				}
			}
			iLast = i;
		}
	}

	long iSubLen = i - iLast - 1;
	if (iSubLen > 0) {
		if (iSubLen > iStrLimt - 1) {
			iSubLen = iStrLimt - 1;
		}
		memcpy(pSubStr[iSub], &szStr[iLast + 1], iSubLen);
		pSubStr[iSub][iSubLen] = 0;
		++iSub;
	}

	return  iSub;
}

/* 用于包含中文的字符串的 explode
 * $cSep : 分隔符;
 * $iAryLimt : 子字符串个数限制;
 * $iStrLimt : 子字符串大小限制（包括结尾的 0）;
 * 如果分隔符连续出现，则认为是 0 长子字符串;
 * return  >= 0 : 分隔出的子字符串
 */
inline int uks_wstr_explode(const char * szStr, const char cSep, char * apSubStr[], const int iAryLimt, const int iStrLimt) {
	long lInput = strlen(szStr);
	long iLast = -1;//上一个分隔字符 $ch 的位置
	int  iSub = 0; // 子字符串的个数
	if (lInput < 1) {
		return  0;
	}
	if (iAryLimt < 1) {
		return  0;
	}
	if (iStrLimt < 2) {
		return  0;
	}
	const char * pr = szStr;
	int w = 0;
	while (*pr && iSub < iAryLimt) {
		const char c0 = *pr;
		if (w >= iStrLimt) {
			// 本子字符串直接结束，跳过剩下字符，直到结尾或者 tag 结果
			while (*pr && cSep != *pr) {
				pr ++;
			}
			apSubStr[iSub][w++] = 0;
			iSub++;
			w = 0;
			continue;
		}

		if (c0 < 0) {
			apSubStr[iSub][w++] = *pr ++;
			apSubStr[iSub][w++] = *pr ++;
		}
		else if (cSep == *pr) {
			// 遇到一个分隔符
			apSubStr[iSub][w++] = 0;
			w = 0;
			iSub ++;
			pr++;
		}
		else {
			apSubStr[iSub][w++] = *pr ++;
		}
	}
	if (w) {
		apSubStr[iSub][w++] = 0;
		w = 0;
		++iSub;
	}

	return  iSub;
}

// 判断 $ch 是否为英文字母  a-zA-Z
static int IsLetter(const char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		return  1;
	}
	return  0;
}

// 判断 $ch 是否为数字  a-zA-Z
static int IsDigit(const char ch) {
	if (ch >= '0' && ch <= '9') {
		return  1;
	}
	return  0;
}

// 判断 $ch 是否为空白字符 ' ', '\t', '\r', '\n'
static int IsWightSpace(const char ch) {
	if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
		return  1;
	}
	return  0;
}


// 判断 $szWord 是否为英文单词
// 必须全为英文字母或者可带上 '\''
static int IsEngWord(const char * szWord) {
	int i = 0;
	while (szWord[i]) {
		switch (szWord[i]) {
		case ' ' :
		case '\t':
		case '-' : // dining-room, good-bye
		case '\'' : // Let's
			break;
		default:
			if ((szWord[i] < 'a' || szWord[i] > 'z') && (szWord[i] < 'A' || szWord[i] > 'Z')) {
				return  0;
			}
		}
		i++;
	}
	return  1;
}


// 判断 $szWord 是否为英文单词
// 必须全为英文字母或者可带上 '\''、'.'
static int IsEngWord2(const char * szWord) {
	int i = 0;
	while (szWord[i]) {
		switch (szWord[i]) {
		case ' ' :
		case '.':
		case '\t':
		case '\'' : // Let's
			break;
		default:
			if ((szWord[i] < 'a' || szWord[i] > 'z') && (szWord[i] < 'A' || szWord[i] > 'Z')) {
				return  0;
			}
		}
		i++;
	}
	return  1;
}

/* 判断 $szWord 是否为数字序列
 * 认为 0-9  1.1  .1.1  1-1.1 等等也都是数字
 * 0 长字符串不是数字；
 *
 */
static int IsNumber(const char * szNum) {
	int i = 0;
	while (szNum[i]) {
		switch(szNum[i]) {
		case '-':
		case '.':
			break;
		default:
			if (szNum[i] < '0' || szNum[i] > '9') {
				return  0;
			}
		}
		i++;
	}
	if (0 == i) {
		return  0;
	}
	return  1;
}

/* 判断 $szWord 是否全为中文数字序列（一九四九）不考虑夹杂阿拉伯数字的情况
 * 0 长字符串不是数字；
 *
 */
static int IsCnNumber(const char * szNum) {
	//char  szChNum[] = "零一二三四五六七八九十百千万亿兆";
	char  szChNum[] = "零一二三四五六七八九十百千万亿零壹贰叁肆伍陆柒捌玖拾佰仟萬两几半数〇多"; //没有 少
	int i = 0;
	while (szNum[i]) {
		if (szNum[i] < 0) {
			char cn[3] = {0};
			memcpy(cn, szNum + i, 2);
			if (! strstr(szChNum, cn)) {
				return  0;
			}
			i += 2;
		}
		else {
			return 0;
		}
	}
	if (0 == i) {
		return  0;
	}
	return  1;
}

static int ToLower(char * ps) {
	char * pw = ps;
	while (*pw) {
		if (*pw < 0) {
			pw += 2;
		}
		else {
			if ('A' <= *pw && *pw <= 'Z') {
				*pw = *pw - 'A' + 'a';
			}
			pw ++;
		}
	}
	return  0;
}

// 在 $szStr 中查找 $ch 出现的各个位置
inline int uks_str_chr_pos(const char * szStr, const char ch, int * aiPos, const int iAryLimt) {
	long i = 0;
	long lInput = strlen(szStr);
	int  iSub = 0;
	if (lInput < 1) {
		return  0;
	}
	if (iAryLimt < 1) {
		return  0;
	}

	for (i = 0; i < lInput; i++) {
		if (ch == szStr[i]) {
			aiPos[iSub] = i;
			if (++iSub >= iAryLimt) {
				return  iSub;
			}
		}
	}

	return  iSub;
}

// 将英文字母串 $szEng 中大写字母原地转小写
// 返回改变的字母个数
inline int Str2Lower(char * szEng, int iLen = 0) {
	if (0 == iLen) {
		iLen = strlen(szEng);
	}
	int i = 0;
	int iCvt = 0;
	for (i = 0; i < iLen; i++) {
		if ('A' <= szEng[i] && szEng[i] <= 'Z') {
			szEng[i] = szEng[i] - 'A' + 'a';
			iCvt ++;
		}
	}
	return  iCvt;
}

/* 判断 $str 是否以 $endStr 结尾
 * return   1 : 以 $endStr 结尾，或者 $endStr 为 0 长字符串;
 *          0 :
 */
inline int StrEndWith(const char * str, const char * endStr) {
	const int iLen = strlen(endStr);
	if (iLen < 1) {
		return  1;
	}
	const char * pr = str;
	while (*pr) {
		pr ++;
	}
	if (pr - str < iLen) {
		// $str 没有 $endStr 长
		return  0;
	}
	pr -= iLen;
	if (memcmp(pr, endStr, iLen) == 0) {
		return  1;
	}

	return  0;
}

/* 在 $szBuf 的基础上截断右侧空白字符
 */
int  LocalRTrim(char * szBuf);

/* 在 $szBuf 的基础上删除左侧空白字符，将剩余字符移动到 $szBuf[0] 处
 */
int  LocalLTrim(char * szBuf);

/* 从字符串中解析出整数数组；
 * 方法：逐字节向后查找数字，遇到数字后开始转换为整数，然后继续向后查找；
 * return  >= 0 : 转换出来的整数个数
 */
int  StoAi(const char * szBuf, int * aiOut, const int items);



// 十六进制字符转成二进制数
inline unsigned char HexChar2Bin(const char c) {
	// 48 - '0', 65 - 'A',  97 - 'a'
	if ('0' <= c && c <= '9') {
		return  c - '0';
	}
	else if ('A' <= c && c <= 'F') {
		return  c - 'A' + 10;
	}
	else if ('a' <= c && c <= 'f') {
		return  c - 'a' + 10;
	}
	return  0xFF;
}

// 二进制数转成十六进制字符
inline char Bin2HexChar(const char c) {
	if (0 <= c && c <= 9) {
		return  c - 0 + '0';
	}
	else if (0xA <= c && c <= 0xF) {
		return  c - 0xA + 'A';
	}
	return  '.';
}

// 将十六进制字符串转换成二进制格式值写入 $psBin，$psBin 应当足够大
inline int HexStr2Bin(const char * psIn, const int iLen, char * psBin) {
	int i = 0;
	for (i = 0; i < iLen - 1; i += 2) {
		psBin[i/2] = (HexChar2Bin(psIn[i]) << 4) | (HexChar2Bin(psIn[i+1]) & 0xF);
	}
	return  0;
}

// 将二进制值转换成十六进制字符串
// 要求 $psStr 指向大小应足够大(>=2*iLen)
inline int Bin2HexStr(const char * psIn, const int iLen, char * psStr) {
	int i = 0;
	for (i = 0; i < iLen; i ++) {
		psStr[2 * i] = Bin2HexChar((psIn[i] >> 4) & 0xF);
		psStr[2 * i + 1] = Bin2HexChar(psIn[i] & 0xF);
	}
	return  0;
}


char * KmpStrstr(const char * psIn, const char * psNeedle);
char * KmpStrstr2(const char * psIn, const char * psNeedle);

/*
 *
 */
unsigned int BM(const char* text, size_t text_length, const char* pattern, size_t pattern_length, unsigned int* matches);

/* 移动指针 *$ps 直到遇到 $stopCh 或者字符串结束，$ps 停止在 $stopCh 或者字符串结束的 0 处
 */
void  MoveUntile(const char ** ps, const char stopCh);


#endif /* UKS_STR_H_ */
