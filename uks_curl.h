/*
 * uks_curl.h
 *
 *  Created on: 2012-8-14
 *      Author:  wei.liu
 */

#ifndef UKS_CURL_H_
#define UKS_CURL_H_

#include  <string>
#include  "curl/curl.h"


// Refer : http://blog.csdn.net/huyiyang2010/article/details/7664201


class CHttpClient
{
public:
	const static int  E_OK    = CURLE_OK; // ����
	const static int  E_CONN_FAIL = CURLE_COULDNT_CONNECT; // ����ʧ��
	const static int  E_TMOUT = CURLE_OPERATION_TIMEDOUT; // ��ʱ


public:
	CHttpClient(void);
	~CHttpClient(void);

public:
	/**
	* @brief HTTP POST����
	* @param strUrl �������,�����Url��ַ,��:http://www.baidu.com
	* @param strPost �������,ʹ�����¸�ʽpara1=val1?2=val2&��
	* @param strResponse �������,���ص�����
	* @return �����Ƿ�Post�ɹ�
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET����
	* @param strUrl �������,�����Url��ַ,��:http://www.baidu.com
	* @param strResponse �������,���ص�����
	* @return �����Ƿ�Post�ɹ�
	*/
	int Get(const std::string & strUrl, std::string & strResponse);

	/**
	* @brief HTTPS POST����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strPost �������,ʹ�����¸�ʽpara1=val1?2=val2&��
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

public:
	void SetDebug(bool bDebug);

private:
	bool m_bDebug;
};



#endif /* UKS_CURL_H_ */
