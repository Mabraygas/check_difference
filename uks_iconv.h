/*
 * uks_iconv.h
 *
 *  Created on: 2012-1-30
 *      Author:  wei.liu
 */

#ifndef UKS_ICONV_H_
#define UKS_ICONV_H_

#include  "Eps_WIN32_LINUX.h"
#include  <iconv.h>


struct  UksConv {
	static const int	T_GBK	 = 1;
	static const int	T_GB2312 = 2;
	static const int	T_BIG5   = 3;
	static const int	T_UTF8   = 4;
	static const int	T_UTF16_LE  = 5;
	static const int	T_UTF16_BE  = 6;

	iconv_t		icv_;
	int			iFromEnc_;
	int			iToEnc_;

	UksConv(const int FromEnc, const int ToEnc)
		: icv_((void *)-1), iFromEnc_(FromEnc), iToEnc_(ToEnc) {

	}

	~UksConv() {
		if (icv_ >= 0) {
			iconv_close(icv_);
		}
	}

	int  IConv(const char * szIn, int * piInLeft, char * szOut, int * piOutLeft);

	/* 提供 C 方式的转换，如果没有提供 $iOutSize ，则认为 $szOut 有 1000B 大小；
	 * return  0  :  OK
	 */
	int  IConv(const char * szIn, char * szOut, const int iOutSize = 0);

	int  IConv(const char * szIn, int * iConvLen, char * szOut, const int iOutSize, int * piOutLen);

	/* 相当于 iconv -c
	 */
	int  IConvC(const char * szIn, int * iSkipLen, char * szOut, const int iOutSize, int * piOutLen);

private:
	int  Init();

	UksConv(const UksConv &); // 必须显式声明该函数

};

/* 返回 Utf8 编码的一个字符的长度
 * return  0 : 非 utf8 编码
 */
inline int  Utf8ChLen(const char * szStr) {
	// 按照 Utf8 读取一个字符
	// 2 字节 110xxxxx 10xxxxxx
	// 3 字节 1110xxxx 10xxxxxx 10xxxxxx
	// 4 字节 11110yyy 10xxxxxx 10xxxxxx 10xxxxxx
	// 5 字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	// 6 字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	unsigned char ch = szStr[0];
	if (ch < 128) {
		return  1;
	}

	if ((ch & 0xFC) == 0xFC) {
		return  6;
	}
	else if ((ch & 0xF8) == 0xF8) {
		return  5;
	}
	else if ((ch & 0xF0) == 0xF0) {
		return  4;
	}
	else if ((ch & 0xE0) == 0xE0) {
		return  3;
	}
	else if ((ch & 0xC0) == 0xC0) {
		return  2;
	}
	return  0;
}



#endif /* UKS_ICONV_H_ */
