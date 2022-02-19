// licenceTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <string>

using std::cout;
using std::endl;
using std::string;

typedef void(*pGenerateSerial)(char* serial);
typedef bool (*pValidateRegistCode)(string regist);
int main()
{
    HMODULE hDLL = LoadLibrary(TEXT("licence.dll"));
    if (hDLL)
    {
        pGenerateSerial generateSerial = pGenerateSerial(GetProcAddress(hDLL, "generateSerial"));
        if (generateSerial != NULL)
        {
            char serial[40];
            generateSerial(serial);
            cout << "Serial: " << serial << endl;
        }
        else
        {
            cout << "Cannot Find Function " << "generateSerial" << endl;
        }

        pValidateRegistCode validateRegistCode = pValidateRegistCode(GetProcAddress(hDLL, "validateRegistCode"));
        if (validateRegistCode != NULL)
        {
            validateRegistCode("5B01-98DD-B50A-CA07-826D-BE41-3AA2-80F9");
            cout << "validateRegistCode run!" << endl;
        }
        else
        {
            cout << "Cannot Find Function " << "validateRegistCode" << endl;
        }

        FreeLibrary(hDLL);
    }
    else
    {
        DWORD dwError = 0;
        std::cout << "Cannot Find " << "licence.dll" << std::endl;
        dwError = GetLastError();
        cout << "Error Code:" << dwError << endl;
    }


    getchar();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件