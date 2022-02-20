#include "LicenceUtils.h"

#include <vector>
#include <regex>
#include <ctime>
#include <algorithm>

#include "framework.h"
#include "licence.h"
#include "mac.h"
#include "AES.h"
#include "base64.h"
#include "md5.h"

#define SECRET_KEY "LEe4J1Qom9P3WV0v3SvmOQ=="

using std::vector;

void LicenceUtils::generateSerial(char* serial)
{
	unsigned char mac[MAX_SIZE];
	GetMacAddress(reinterpret_cast<char*>(mac));
	std::cout << "MAC address:" << mac << std::endl;
	// TODO ����mac�ַ�����Ϊ17��Ŀǰ������һλ����Ȼ�޷�����aes
	// ȷ��һ���Ƿ���ҪgetBytes����
	// ����ECB/CBC/CFB  AESKeyLength::AES_128/AES_192/AES_256 ?
	vector<unsigned char> mac_plain(mac, mac + strlen(reinterpret_cast<char*>(mac)) - 1); // strlen(reinterpret_cast<char*>(mac) - 1) == 16

	// ����Կ���н��룬��ȡԭʼ��Կ
	string base64_key = base64_encode(reinterpret_cast<const unsigned char*>(SECRET_KEY), sizeof(SECRET_KEY));
	vector<unsigned char> key(base64_key.begin(), base64_key.end());

	AES aes(AESKeyLength::AES_128);
	vector<unsigned char> aes_res = aes.EncryptECB(mac_plain, key);
	string aes_res_str(aes_res.begin(), aes_res.end());
	std::cout << "AES result: " << aes_res_str << std::endl;

	string md5_res = MD5(aes_res_str).toString();
	transform(md5_res.begin(), md5_res.end(), md5_res.begin(), ::toupper);

	for (int i = 1; i < 8; i++) {
		md5_res.insert(md5_res.begin() + 5 * i - 1, '-');
	}
	strcpy_s(serial, md5_res.length() + 1, md5_res.c_str());
}

bool LicenceUtils::regist(const char *regist, char* expireDate)
{
	Licence licence;

	// ��ע����ת��ΪLicence����
	regist2Licence(regist, &licence);

	// ����������
	char serial[SERIAL_LEN];
	generateSerial(serial);
	string shortSerial = generateShortSerial(string(serial));
	if (shortSerial == licence.getSerial())
	{
		licence.setSerial(serial);
		if (validateExpire(&licence))
		{
			// ������Ȩ�ļ�
			persistLicence(&licence);

			// ��ʶע��ɹ�
			return true;
		}
	}

	return false;
}

// ��ע����ת����licence����
void LicenceUtils::regist2Licence(string regist, Licence* licence)
{
	string base64_key = base64_encode(reinterpret_cast<const unsigned char*>(SECRET_KEY), sizeof(SECRET_KEY));
	vector<unsigned char> key(base64_key.begin(), base64_key.end());

	// ɾ����Ȩ���е� "-"
	size_t nPos = 0;
	nPos = regist.find("-", nPos);  // ���ҿո���str�е�һ�γ��ֵ�λ��
	while (nPos != string::npos)
	{
		regist.replace(nPos, 1, "");
		nPos = regist.find("-", nPos);
	}
	transform(regist.begin(), regist.end(), regist.begin(), tolower);

	vector<unsigned char> data(regist.begin(), regist.end());

	AES aes(AESKeyLength::AES_128);
	vector<unsigned char> decrypted = aes.DecryptECB(data, key);

	licence->setSerial(string(decrypted.begin(), decrypted.begin() + 7));
	string expire(decrypted.begin() + 7, decrypted.end());
	if (expire == "00000000")
	{
		licence->setExpire("000000000");
	}
	else
	{
		licence->setExpire("1" + expire);
	}
}

string LicenceUtils::generateShortSerial(string serial)
{
	// ʵ��split
	std::regex reg("-");
	std::vector<std::string> items(std::sregex_token_iterator(serial.begin(), serial.end(), reg, -1), std::sregex_token_iterator());

	string shortSerial;
	for (size_t i = 1; i < items.size(); i++)
	{
		string item = items.at(i);
		shortSerial.append(1, item.at(i % 4));
	}
	return shortSerial;
}

bool LicenceUtils::loadLicence(string path, Licence *licence)
{
	return false;
}

void LicenceUtils::persistLicence(Licence* pLicence)
{

}

bool LicenceUtils::validate()
{
	Licence licence;

	// ������Ȩ�ļ�
	if (!loadLicence("", &licence))
	{
		std::cout << "��Ȩ�ļ�������" << std::endl;
		return false;
	}

	// ��֤����ָ��
	if (!validateDigest(&licence))
	{
		std::cout << "��Ȩ�ļ���Ч" << std::endl;
		return false;
	}
	
	// ��֤ʧЧ��
	if (!validateExpire(&licence))
	{
		std::cout << "��Ȩ�ļ��ѹ���" << std::endl;
		return false;
	}

	return true;
}

time_t LicenceUtils::string2Date(string str)
{
	int year = std::stoi(str.substr(1, 4));
	int month = std::stoi(str.substr(5, 2));
	int day = std::stoi(str.substr(7, 2));
	tm tm_;                                    // ����tm�ṹ�塣
	tm_.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬����tm_yearΪint��ʱ������ȥ1900��
	tm_.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ʱ������ȥ1��
	tm_.tm_mday = day;                         // ��
	time_t t_ = mktime(&tm_);                  // ��tm�ṹ��ת����time_t��ʽ��
	return t_;                                 // ����ֵ��
}

bool LicenceUtils::validateExpire(Licence* pLicence)
{
	// ��������֤
	string expire = pLicence->getExpire();
	if (expire.at(0) == '1')
	{
		time_t expireTime = string2Date(expire);
		return time(0) < expireTime;
	}
	return true;
}

bool LicenceUtils::validateDigest(Licence* pLicence)
{
	char serial[SERIAL_LEN];
	generateSerial(serial);
	
	return strcmp(serial, pLicence->getSerial().c_str()) == 0;
}
