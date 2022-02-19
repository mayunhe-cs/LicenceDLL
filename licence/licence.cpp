// licence.cpp : 定义 DLL 的导出函数。
//
/*
aes: https://github.com/SergeyBel/AES
md5: https://www.cnblogs.com/flying_bat/archive/2007/09/25/905133.html
     https://www.cnblogs.com/ydxt/p/3582141.html (todo)
*/
#include <vector>
#include <algorithm>

//#include "pch.h"
#include "framework.h"
#include "licence.h"
#include "mac.h"
#include "AES.h"
#include "base64.h"
#include "md5.h"



using std::vector;

// 这是已导出类的构造函数。
Clicence::Clicence()
{
    LicenceConstants::init();
    return;
}

// 这是导出变量的一个示例
LICENCE_API int nlicence=0;

// 这是导出函数的一个示例。
LICENCE_API int fnlicence(void)
{
    return 0;
}


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
    transform(md5_res.begin(), md5_res.end(), md5_res.begin(), std::toupper);

    for (int i = 1; i < 8; i++) {
        md5_res.insert(md5_res.begin() + 5 * i - 1, '-');
    }
    strcpy_s(serial, md5_res.length() + 1, md5_res.c_str());
}

bool validateRegistCode(string regist)
{
    regist2Licence(regist);
    char serial[40];
    generateSerial(serial);
    // TODO
    return false;
}

bool validateLicence()
{
    return false;
}



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
        nPos = regist.find(" ", nPos);
    }

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