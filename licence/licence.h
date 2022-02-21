#pragma once
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LICENCE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LICENCE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef LICENCE_EXPORTS
#define LICENCE_API __declspec(dllexport)
#else
#define LICENCE_API __declspec(dllimport)
#endif

#include <string>
#include <direct.h>

using std::string;

class Licence
{
private:
    string serial;
    string digest;
    string expire;

public:
    Licence(string _serial, string _digest, string _expire) :serial(_serial), digest(_digest), expire(_expire) {}
    Licence(string _serial, string _digest) :Licence(_serial, _digest, "") {}
    Licence(string _serial) :Licence(_serial, "", "") {}
    Licence() {}

    string getSerial() const { return serial; }
    void setSerial(string _serial) { serial = _serial; }
    string getExpire() const { return expire; }
    void setExpire(string _expire) { expire = _expire; }
};

class LicenceConstants
{
public:
    static string licencePath;

    static void init();
};


#ifdef __cplusplus         // if used by C++ code
extern "C" {                  // we need to export the C interface
#endif

// TODO:
// 1.没有找到动态链接类的例子 
//   https://blog.csdn.net/wu_lian_nan/article/details/89028047
//   https://stackoverflow.com/questions/431533/c-dynamically-loading-classes-from-dlls
// 2.以下导出的函数中参数/返回值若有dll中定义的类，如何做？
// 

// serial分配的空间应大于40
LICENCE_API void generateSerial(char* serial);
LICENCE_API bool validateRegistCode(string regist);
LICENCE_API bool validateLicence();


// 此类是从 dll 导出的
class LICENCE_API Clicence
{
public:
    Clicence(void);
};

extern LICENCE_API int nlicence;

LICENCE_API int fnlicence(void);
#ifdef __cplusplus
}
#endif