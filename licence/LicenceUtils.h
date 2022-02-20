#pragma once

#include "licence.h"

#ifdef LICENCE_EXPORTS
#define LICENCE_API __declspec(dllexport)
#else
#define LICENCE_API __declspec(dllimport)
#endif

#define SERIAL_LEN	40		// �����볤��
#define REGIST_LEN	40		// ע���볤��
#define DATE_LEN	8		// �����ַ�������

using std::string;

class LICENCE_API LicenceUtils
{
public:
	// ����������
	void generateSerial(char* serial);

	// ע�ᣬ������Ȩ��Ϣ
	bool regist(const char *regist, char *expireDate);

	// ��֤����Ƿ���Ȩ�����ȶ�ȡָ��·���µ�.licence�ļ���Ȼ��У��
	// ��Ȩ�ļ�����Ч��
	bool validate();

private:
	// ��Ȩ��ת����Licence����
	void regist2Licence(string regist, Licence *licence);

	// ���ɶ̸�ʽ��������
	string generateShortSerial(string serial);

	// ������Ȩ�ļ�
	bool loadLicence(string path, Licence *licence);

	// ��Licence���󱣴�Ϊ .licence�ļ�
	void persistLicence(Licence* pLicence);

	// ��֤��Ȩ�ļ��뵱ǰ������ƥ���ϵ
	bool validateDigest(Licence* pLicence);

	// ��֤��ǰʱ���Ƿ�����Ч����
	bool validateExpire(Licence* pLicence);

	// �ַ�����ʽ������ʱ��ת����Datetime����
	time_t string2Date(string str);
};

