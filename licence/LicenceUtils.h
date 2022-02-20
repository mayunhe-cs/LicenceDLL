#pragma once

#include "licence.h"

#ifdef LICENCE_EXPORTS
#define LICENCE_API __declspec(dllexport)
#else
#define LICENCE_API __declspec(dllimport)
#endif

#define SERIAL_LEN	40		// 申请码长度
#define REGIST_LEN	40		// 注册码长度
#define DATE_LEN	8		// 日期字符串长度

using std::string;

class LICENCE_API LicenceUtils
{
public:
	// 生成申请码
	void generateSerial(char* serial);

	// 注册，生成授权信息
	bool regist(const char *regist, char *expireDate);

	// 验证软件是否授权。首先读取指定路径下的.licence文件，然后校验
	// 授权文件的有效性
	bool validate();

private:
	// 授权码转换成Licence对象
	void regist2Licence(string regist, Licence *licence);

	// 生成短格式的申请码
	string generateShortSerial(string serial);

	// 加载授权文件
	bool loadLicence(string path, Licence *licence);

	// 把Licence对象保存为 .licence文件
	void persistLicence(Licence* pLicence);

	// 验证授权文件与当前机器的匹配关系
	bool validateDigest(Licence* pLicence);

	// 验证当前时刻是否在有效期内
	bool validateExpire(Licence* pLicence);

	// 字符串格式的日期时间转换成Datetime对象
	time_t string2Date(string str);
};

