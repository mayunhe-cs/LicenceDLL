// licence.cpp : 定义 DLL 的导出函数。
//
/*
aes: https://github.com/SergeyBel/AES
md5: https://www.cnblogs.com/flying_bat/archive/2007/09/25/905133.html
     https://www.cnblogs.com/ydxt/p/3582141.html (todo)
*/
#include <vector>
#include <regex>
#include <ctime>
#include <algorithm>

//#include "pch.h"
#include "framework.h"
#include "licence.h"
#include "mac.h"
#include "AES.h"
#include "base64.h"
#include "md5.h"


using std::vector;
//using std::transform;

// 将注册码转换成licence对象
Licence* regist2Licence(string regist);
string generalShortSerial(string serial);
void persistLicence(Licence* pLicence);
bool validateExpire(Licence* pLicence);

string LicenceConstants::secretKey = "LEe4J1Qom9P3WV0v3SvmOQ==";
string LicenceConstants::licencePath;

void LicenceConstants::init()
{
    char basePath[_MAX_PATH];
    _getcwd(basePath, _MAX_PATH);
    licencePath = string(basePath) + "\\.licence";
}

// 使用了 extern "C" ，若返回string会有警告
void generateSerial(char* serial)
{
    unsigned char mac[MAX_SIZE];
    GetMacAddress(reinterpret_cast<char*>(mac));
    std::cout << "MAC address:" << mac << std::endl;
    // TODO 这里mac字符串长为17，目前舍弃了一位，不然无法进行aes
    // 确定一下是否不需要getBytes操作
    // 调用ECB/CBC/CFB  AESKeyLength::AES_128/AES_192/AES_256 ?
    vector<unsigned char> mac_plain(mac, mac + strlen(reinterpret_cast<char*>(mac))-1); // strlen(reinterpret_cast<char*>(mac) - 1) == 16
    string base64_key = base64_encode(reinterpret_cast<const unsigned char*>(LicenceConstants::secretKey.c_str()), LicenceConstants::secretKey.length());
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

bool validateRegistCode(string regist)
{
    bool valid = false;
    Licence *pLicence = regist2Licence(regist);
    char serial[40];
    generateSerial(serial);
    string shortSerial = generalShortSerial(serial);
    if (shortSerial == pLicence->getSerial())
    {
        pLicence->setSerial(serial);
        // setDigest
        if (validateExpire(pLicence))
        {
            persistLicence(pLicence);
            valid = true;
            return true;
        }
    }
    

    if(pLicence) delete pLicence;
    return valid;
}

bool validateLicence()
{
    return false;
}


// 将注册码转换成licence对象
Licence* regist2Licence(string regist)
{
    Licence* licence = new Licence;
    string base64_key = base64_encode(reinterpret_cast<const unsigned char*>(LicenceConstants::secretKey.c_str()), LicenceConstants::secretKey.length());
    vector<unsigned char> key(base64_key.begin(), base64_key.end());

    // 替换'-'为空
    size_t nPos = 0;
    nPos = regist.find("-", nPos);  // 查找空格在str中第一次出现的位置
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
    return licence;

}

string generalShortSerial(string serial)
{
    // 实现split
    std::regex reg("-");
    std::vector<std::string> items(std::sregex_token_iterator(serial.begin(), serial.end(), reg, -1), std::sregex_token_iterator());

    string shortSerial;
    for (int i = 1; i < items.size(); i++)
    {
        string item = items.at(i);
        shortSerial.append(1, item.at(i % 4));
    }
    return shortSerial;
}

time_t StringToDatetime(string str)
{
    int year = std::stoi(str.substr(1, 4));
    int month = std::stoi(str.substr(5, 2));
    int day = std::stoi(str.substr(7, 2));
    tm tm_;                                    // 定义tm结构体。
    tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = day;                         // 日。
    //tm_.tm_hour = hour;                        // 时。
    //tm_.tm_min = minute;                       // 分。
    //tm_.tm_sec = second;                       // 秒。
    //tm_.tm_isdst = 0;                          // 非夏令时。
    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}

bool validateExpire(Licence* pLicence)
{
    // TODO 完整性验证
    string expire = pLicence->getExpire();
    if (expire.at(0) == '1')
    {
        time_t expireTime = StringToDatetime(expire);
        return time(0) < expireTime;
    }
    return true;
}

void persistLicence(Licence* pLicence)
{

}