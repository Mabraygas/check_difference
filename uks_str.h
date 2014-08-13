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


// �� $value �ĸ�λ���� $szBuf[0] ����������ȥ $szBuf ��ӡ������ $value �� bit λ��ֱ��չʾ
inline void  U64To01Txt(uint64_t value, char * szBuf) {
	for (int i = 0; i < 64; i++) {
		szBuf[63-i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[64] = 0;
}
// ��
inline void  U16To01Txt(uint16_t value, char * szBuf) {
	for (int i = 0; i < 16; i++) {
		szBuf[15 - i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[16] = 0;
}
// ��
inline void  U8To01Txt(uint8_t value, char * szBuf) {
	for (int i = 0; i < 8; i++) {
		szBuf[7-i] = '0' + (value & 0x1);
		value >>= 1;
	}
	szBuf[8] = 0;
}

// �� "00010110100" ��ʽ���ַ���ת��Ϊ u64�� $szBuf ����Ҫ�� 64B ��С
inline void  TxtToU64(const char * sBuf, uint64_t * pddwVal) {
	uint64_t  ddwVal = 0;
	for (int i = 0; i < 64; i++) {
		ddwVal |= ((uint64_t)('1' == sBuf[i])) << (63 - i);
	}
	*pddwVal = ddwVal;
}

// �� "00010110100" ��ʽ���ַ���ת��Ϊ u16�� $szBuf ����Ҫ�� 16B ��С
inline void  TxtToU16(const char * sBuf, uint16_t * pwVal) {
	uint16_t  wVal = 0;
	for (int i = 0; i < 16; i++) {
		wVal |= ('1' == sBuf[i]) << (15 - i);
	}
	*pwVal = wVal;
}

// �� "00010110100" ��ʽ���ַ���ת��Ϊ u16�� $szBuf ����Ҫ�� 16B ��С
inline void  TxtToU8(const char * sBuf, uint8_t * pcVal) {
	uint8_t  cVal = 0;
	for (int i = 0; i < 8; i++) {
		cVal |= ('1' == sBuf[i]) << (7 - i);
	}
	*pcVal = cVal;
}

// ���� $szIn �����һ�� $szNeedle
// ��� $szNeedle Ϊ 0 ���ַ�������ֱ�ӷ���ָ�� $szIn ĩβ 0 �ֽڴ���ָ��
// $szIn �� $szNeedle ����Ϊ NULL
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
		if (*pr < 0) { // gbk�ַ�
			iStrLen ++;
			pr ++;
			if (*pr < 0) { // Ӧ��Ҳ�� gbk �ַ�
				if (iStrLen >= iLimit) {
					iStrLen --;
					pr --; // ˫�ֽ��ַ��е�һ���ֽ�Խλ������Ҫ����һ���ֽ�
					break;
				}
				iStrLen ++;
				pr ++;
			} else { // gbk�����ֽڶ��� < 0 �ı��룬����������
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
	long iLast = -1;//��һ���ָ��ַ� $ch ��λ��
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

/* ���ڰ������ĵ��ַ����� explode
 * $cSep : �ָ���;
 * $iAryLimt : ���ַ�����������;
 * $iStrLimt : ���ַ�����С���ƣ�������β�� 0��;
 * ����ָ����������֣�����Ϊ�� 0 �����ַ���;
 * return  >= 0 : �ָ��������ַ���
 */
inline int uks_wstr_explode(const char * szStr, const char cSep, char * apSubStr[], const int iAryLimt, const int iStrLimt) {
	long lInput = strlen(szStr);
	long iLast = -1;//��һ���ָ��ַ� $ch ��λ��
	int  iSub = 0; // ���ַ����ĸ���
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
			// �����ַ���ֱ�ӽ���������ʣ���ַ���ֱ����β���� tag ���
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
			// ����һ���ָ���
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

// �ж� $ch �Ƿ�ΪӢ����ĸ  a-zA-Z
static int IsLetter(const char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		return  1;
	}
	return  0;
}

// �ж� $ch �Ƿ�Ϊ����  a-zA-Z
static int IsDigit(const char ch) {
	if (ch >= '0' && ch <= '9') {
		return  1;
	}
	return  0;
}

// �ж� $ch �Ƿ�Ϊ�հ��ַ� ' ', '\t', '\r', '\n'
static int IsWightSpace(const char ch) {
	if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
		return  1;
	}
	return  0;
}


// �ж� $szWord �Ƿ�ΪӢ�ĵ���
// ����ȫΪӢ����ĸ���߿ɴ��� '\''
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


// �ж� $szWord �Ƿ�ΪӢ�ĵ���
// ����ȫΪӢ����ĸ���߿ɴ��� '\''��'.'
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

/* �ж� $szWord �Ƿ�Ϊ��������
 * ��Ϊ 0-9  1.1  .1.1  1-1.1 �ȵ�Ҳ��������
 * 0 ���ַ����������֣�
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

/* �ж� $szWord �Ƿ�ȫΪ�����������У�һ���ľţ������Ǽ��Ӱ��������ֵ����
 * 0 ���ַ����������֣�
 *
 */
static int IsCnNumber(const char * szNum) {
	//char  szChNum[] = "��һ�����������߰˾�ʮ��ǧ������";
	char  szChNum[] = "��һ�����������߰˾�ʮ��ǧ������Ҽ��������½��ƾ�ʰ��Ǫ�f������������"; //û�� ��
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

// �� $szStr �в��� $ch ���ֵĸ���λ��
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

// ��Ӣ����ĸ�� $szEng �д�д��ĸԭ��תСд
// ���ظı����ĸ����
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

/* �ж� $str �Ƿ��� $endStr ��β
 * return   1 : �� $endStr ��β������ $endStr Ϊ 0 ���ַ���;
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
		// $str û�� $endStr ��
		return  0;
	}
	pr -= iLen;
	if (memcmp(pr, endStr, iLen) == 0) {
		return  1;
	}

	return  0;
}

/* �� $szBuf �Ļ����Ͻض��Ҳ�հ��ַ�
 */
int  LocalRTrim(char * szBuf);

/* �� $szBuf �Ļ�����ɾ�����հ��ַ�����ʣ���ַ��ƶ��� $szBuf[0] ��
 */
int  LocalLTrim(char * szBuf);

/* ���ַ����н������������飻
 * ���������ֽ����������֣��������ֺ�ʼת��Ϊ������Ȼ����������ң�
 * return  >= 0 : ת����������������
 */
int  StoAi(const char * szBuf, int * aiOut, const int items);



// ʮ�������ַ�ת�ɶ�������
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

// ��������ת��ʮ�������ַ�
inline char Bin2HexChar(const char c) {
	if (0 <= c && c <= 9) {
		return  c - 0 + '0';
	}
	else if (0xA <= c && c <= 0xF) {
		return  c - 0xA + 'A';
	}
	return  '.';
}

// ��ʮ�������ַ���ת���ɶ����Ƹ�ʽֵд�� $psBin��$psBin Ӧ���㹻��
inline int HexStr2Bin(const char * psIn, const int iLen, char * psBin) {
	int i = 0;
	for (i = 0; i < iLen - 1; i += 2) {
		psBin[i/2] = (HexChar2Bin(psIn[i]) << 4) | (HexChar2Bin(psIn[i+1]) & 0xF);
	}
	return  0;
}

// ��������ֵת����ʮ�������ַ���
// Ҫ�� $psStr ָ���СӦ�㹻��(>=2*iLen)
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

/* �ƶ�ָ�� *$ps ֱ������ $stopCh �����ַ���������$ps ֹͣ�� $stopCh �����ַ��������� 0 ��
 */
void  MoveUntile(const char ** ps, const char stopCh);


#endif /* UKS_STR_H_ */
