#ifndef WINDOWSGLUE_H
#define WINDOWSGLUE_H

/*
 *  WindowsGlue.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 31/12/2017.
 *  Copyright 2017 Bill Sellers. All rights reserved.
 *
 */

// WindowsGlue.h - utility routines to help dealing with Windows idiosynchracies

namespace AsynchronousGA
{

class WindowsGlue
{
public:
    WindowsGlue();
    ~WindowsGlue();

    // these versions the called needs to delete the returned string
    static char *ToUTF8(const wchar_t *input);
    static char *ToUTF8(const char *input);
    static wchar_t *ToWide(const wchar_t *input);
    static wchar_t *ToWide(const char *input);
    // these versions used shared internal storage so the string is only available until the next call
    char *ToUTF8Tmp(const wchar_t *input);
    char *ToUTF8Tmp(const char *input);
    wchar_t *ToWideTmp(const wchar_t *input);
    wchar_t *ToWideTmp(const char *input);

private:
    char *m_charBuffer;
    wchar_t *m_wcharBuffer;
    const int k_maxStringLength = 1024 * 1024;
};

}

#endif // WINDOWSGLUE_H
