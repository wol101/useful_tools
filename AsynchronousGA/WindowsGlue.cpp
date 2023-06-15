#include "WindowsGlue.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <wchar.h>
#endif

namespace AsynchronousGA
{

WindowsGlue::WindowsGlue()
{
    m_charBuffer = new char[k_maxStringLength + 1];
    m_wcharBuffer = new wchar_t[k_maxStringLength + 1];
}

WindowsGlue::~WindowsGlue()
{
    if (m_charBuffer) delete [] m_charBuffer;
    if (m_wcharBuffer) delete [] m_wcharBuffer;
}

char *WindowsGlue::ToUTF8(const wchar_t *input)
{
#ifdef _WIN32
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, input, -1, 0, 0, 0, 0);
    if (sizeRequired == 0) return nullptr;
    char *output = new char[sizeRequired];
    WideCharToMultiByte(CP_UTF8, 0, input, -1, output, sizeRequired, 0, 0);
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
    strcpy(output, input);
    return output;
}

wchar_t *WindowsGlue::ToWide(const char *input)
{
#ifdef _WIN32
    int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, input, -1, 0, 0);
    if (sizeRequired == 0) return nullptr;
    wchar_t *output = new wchar_t[sizeRequired];
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
    WideCharToMultiByte(CP_UTF8, 0, input, -1, m_charBuffer, k_maxStringLength, 0, 0);
#else
    wcstombs(m_charBuffer, input, k_maxStringLength);
#endif
    m_charBuffer[k_maxStringLength] = 0;
    return m_charBuffer;
}

char *WindowsGlue::ToUTF8Tmp(const char *input)
{
    strncpy(m_charBuffer, input, k_maxStringLength);
    m_charBuffer[k_maxStringLength] = 0;
    return m_charBuffer;
}

wchar_t *WindowsGlue::ToWideTmp(const char *input)
{
#ifdef _WIN32
    MultiByteToWideChar(CP_UTF8, 0, input, -1, m_wcharBuffer, k_maxStringLength);
#else
    mbstowcs(m_wcharBuffer, input, k_maxStringLength);
#endif
    m_wcharBuffer[k_maxStringLength] = 0;
    return m_wcharBuffer;
}

wchar_t *WindowsGlue::ToWideTmp(const wchar_t *input)
{
    wcsncpy(m_wcharBuffer, input, k_maxStringLength);
    m_wcharBuffer[k_maxStringLength] = 0;
    return m_wcharBuffer;
}

}

