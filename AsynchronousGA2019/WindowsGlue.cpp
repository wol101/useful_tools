#include "WindowsGlue.h"

#include <stdlib.h>
#include <string.h>
#include <memory>

#ifdef _WIN32
#include <Windows.h>
#else
#include <wchar.h>
#endif

WindowsGlue::WindowsGlue()
{
}

WindowsGlue::~WindowsGlue()
{
}

char *WindowsGlue::ToUTF8(const wchar_t *input)
{
#ifdef _WIN32
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, nullptr, nullptr);
    if (sizeRequired == 0) return nullptr;
    char *output = new char[size_t(sizeRequired)];
    WideCharToMultiByte(CP_UTF8, 0, input, -1, output, sizeRequired, nullptr, nullptr);
#else
    int sizeRequired = wcslen(input) * 4;
    char *output = new char[sizeRequired + 1];
    wcstombs(output, input, sizeRequired);
#endif
    return output;
}

char *WindowsGlue::ToUTF8(const char *input)
{
    size_t sizeRequired = strlen(input);
    char *output = new char[sizeRequired + 1];
    strncpy(output, input, sizeRequired + 1);
    return output;
}

wchar_t *WindowsGlue::ToWide(const char *input)
{
#ifdef _WIN32
    int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
    if (sizeRequired == 0) return nullptr;
    wchar_t *output = new wchar_t[size_t(sizeRequired)];
    MultiByteToWideChar(CP_UTF8, 0, input, -1, output, sizeRequired);
#else
    int sizeRequired = strlen(input);
    wchar_t *output = new wchar_t[sizeRequired + 1];
    mbstowcs(output, input, sizeRequired);
#endif
    return output;
}

wchar_t *WindowsGlue::ToWide(const wchar_t *input)
{
    size_t sizeRequired = wcslen(input);
    wchar_t *output = new wchar_t[sizeRequired + 1];
    wcscpy(output, input);
    return output;
}

char *WindowsGlue::ToUTF8Tmp(const wchar_t *input)
{
#ifdef _WIN32
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, nullptr, nullptr);
    if (sizeRequired == 0) return nullptr;
    m_charBuffer.resize(size_t(sizeRequired));
    WideCharToMultiByte(CP_UTF8, 0, input, -1, m_charBuffer.data(), sizeRequired, nullptr, nullptr);
#else
    size_t sizeRequired = wcslen(input) * 4;
    m_charBuffer.resize(sizeRequired + 1);
    wcstombs(m_charBuffer.data(), input, sizeRequired);
#endif
    return m_charBuffer.data();
}

char *WindowsGlue::ToUTF8Tmp(const char *input)
{
    size_t sizeRequired = strlen(input);
    m_charBuffer.resize(sizeRequired + 1);
    strncpy(m_charBuffer.data(), input, sizeRequired);
    return m_charBuffer.data();
}

wchar_t *WindowsGlue::ToWideTmp(const char *input)
{
#ifdef _WIN32
    int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
    if (sizeRequired == 0) return nullptr;
    m_wcharBuffer.resize(size_t(sizeRequired));
    MultiByteToWideChar(CP_UTF8, 0, input, -1, m_wcharBuffer.data(), sizeRequired);
#else
    size_t sizeRequired = strlen(input);
    m_wcharBuffer.resize(sizeRequired + 1);
    mbstowcs(m_wcharBuffer.data(), input, sizeRequired);
#endif
    m_wcharBuffer.data()[sizeRequired] = 0;
    return m_wcharBuffer.data();
}

wchar_t *WindowsGlue::ToWideTmp(const wchar_t *input)
{
    size_t sizeRequired = wcslen(input);
    m_wcharBuffer.resize(sizeRequired + 1);
    wcsncpy(m_wcharBuffer.data(), input, sizeRequired);
    return m_wcharBuffer.data();
}

std::string WindowsGlue::ToUTF8(const std::wstring &input)
{
#ifdef _WIN32
    int len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (len == 0) return std::string();
    auto buf = std::make_unique<char[]>(size_t(len));
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, buf.get(), len, nullptr, nullptr);
    std::string r(buf.get());
    return r;
#else
    size_t sizeRequired = input.size() * 4;
    char *output = new char[sizeRequired + 1];
    sizeRequired = wcstombs(output, input.c_str(), sizeRequired);
    std::string returnValue(output, size_t(sizeRequired));
    delete [] output;
    return returnValue;
#endif
}

std::string WindowsGlue::ToUTF8(const std::string &input)
{
    return input;
}

std::wstring WindowsGlue::ToWide(const std::wstring &input)
{
    return input;
}

std::wstring WindowsGlue::ToWide(const std::string input)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, nullptr, 0);
    if (len == 0) return std::wstring();
    auto buf = std::make_unique<wchar_t[]>(size_t(len));
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, buf.get(), len);
    std::wstring r(buf.get());
    return r;
#else
    size_t sizeRequired = input.size();
    wchar_t *output = new wchar_t[sizeRequired + 1];
    sizeRequired = mbstowcs(output, input.c_str(), sizeRequired);
    std::wstring returnValue(output, size_t(sizeRequired));
    delete [] output;
    return returnValue;
#endif
}

