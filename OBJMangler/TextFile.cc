/*
 *  TextFile.cpp
 *  GaitSymODE
 *
 *  Created by Bill Sellers on 24/08/2005.
 *  Copyright 2005 Bill Sellers. All rights reserved.
 *
 */

// TextFile.cc - utility class to read in various sorts of data files

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>

#include "TextFile.h"

const static int kStorageIncrement = 65536;

// default constructor
TextFile::TextFile()
{
    m_Size = 0;
    m_FileData = 0;
    m_Index = 0;
    m_ExitOnErrorFlag = false;
    m_RangeControl = 0;
}

// default destructor
TextFile::~TextFile()
{
    if (m_FileData) delete [] m_FileData;
}

void TextFile::SetRawData(const char *string)
{
    if (m_FileData) delete [] m_FileData;
    m_Size = strlen(string) + 1;
    m_FileData = new char [m_Size];
    strcpy(m_FileData, string);
    m_Index = m_FileData;
}

void TextFile::ClearData()
{
    if (m_FileData) delete [] m_FileData;
    m_Size = 0;
    m_FileData = 0;
    m_Index = m_FileData;
}

// preforms a global search and replace 
int TextFile::Replace(const char *oldString, const char *newString)
{
    int count = 0;
    int oldLen = strlen(oldString);
    int newLen = strlen(newString);
    char *startPtr = m_FileData;
    char *foundPtr;
    char *endPtr;
    ptrdiff_t size;
    char *newBuffer;
    int i;
    char **segment = new char *[1 + m_Size / oldLen]; // bound to be big enough
    
    do
    {
        foundPtr = strstr(startPtr, oldString);
        if (foundPtr)
        {
            size = foundPtr - startPtr;
            segment[count] = new char[size + 1];
            memcpy(segment[count], startPtr, size);
            segment[count][size] = 0;
            count ++;
            startPtr = foundPtr + oldLen;
            endPtr = startPtr;
        }
    } while (foundPtr);
    
    if (count) // safe but slow version - would be quicker with memcpy and lots of string length storage
    {
        newBuffer = new char[m_Size + (newLen - oldLen) * count + 1];        
        *newBuffer = 0;
        for (i = 0; i < count; i++)
        {
            strcat(newBuffer, segment[i]);
            delete segment[i];
            strcat(newBuffer, newString);
        }
        strcat(newBuffer, endPtr);
        delete [] m_FileData;
        m_Size = strlen(newBuffer) + 1;
        m_FileData = newBuffer;
        m_Index = m_FileData;
    }
    
    delete [] segment;
    return count;
}

// read the named file
// returns true on error
bool TextFile::ReadFile(const char * const name)
{
    struct stat fileStat;
    FILE *in;
    int count;
    int error;
    
    if (m_FileData) delete [] m_FileData;
    m_FileData = 0;
    
    error = stat(name, &fileStat);
    if (error && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadFile(" << name <<
        ") - Cannot stat file\n";
        exit(1);
    }
    if (error) return true;
    m_FileData = new char[fileStat.st_size + 1];
    if (m_FileData == 0 && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadFile(" << name <<
        ") - Cannot allocate m_FileData\n";
        exit(1);
    }
    if (m_FileData == 0) return true;
    m_Index = m_FileData;
    m_Size = fileStat.st_size;
    m_FileData[m_Size] = 0;
    
    in = fopen(name, "r");
    if (in == 0 && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadFile(" << name <<
        ") - Cannot open file\n";
        exit(1);
    }
    if (in == 0) return true;
    count = fread(m_FileData, fileStat.st_size, 1, in);
    fclose(in);
    if (count != 1 && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadFile(" << name <<
        ") - Cannot read file\n";
        exit(1);
    }
    if (count != 1) return true;
    
    return false;
}

// write the data to a file
// if binary is true, the whole of the buffer (including terminating zero) is written
// otherwise it is just the string until the terminating zero
bool TextFile::WriteFile(const char * const name, bool binary)
{
    FILE *out;
    size_t count;
    
    out = fopen(name, "w");
    
    if (out == 0)
    {
        if (m_ExitOnErrorFlag)
        {
            std::cerr << "Error: TextFile::WriteFile(" << name <<
            ") - Cannot open file\n";
            exit(1);
        }
        else return true;
    }
    
    // write file
    if (binary) count = fwrite(m_FileData, m_Size, 1, out);
    else count = fwrite(m_FileData, strlen(m_FileData), 1, out);
    
    if (count != 1)
    {
        if (m_ExitOnErrorFlag)
        {
            std::cerr << "Error: TextFile::WriteFile(" << name <<
            ") - Cannot write file\n";
            exit(1);
        }
        else return true;
    }
    
    if (fclose(out))
    {
        if (m_ExitOnErrorFlag)
        {
            std::cerr << "Error: TextFile::WriteFile(" << name <<
            ") - Cannot close file\n";
            exit(1);
        }
        else return true;
    }
    
    return false;
}

// read an integer parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, int *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    *val = (int)strtol(buffer, 0, 10);
    
    return false;
}

// read a long parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, long *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    *val = strtol(buffer, 0, 10);
    
    return false;
}

// read an unsigned integer parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, unsigned int *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    *val = (unsigned int)strtoul(buffer, 0, 10);
    
    return false;
}

// read an unsigned long parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, unsigned long *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    *val = strtoul(buffer, 0, 10);
    
    return false;
}


// read a double parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, double *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    *val = strtod(buffer, 0);
    
    return false;
}

// read a bool parameter
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, bool *val, bool searchFromStart)
{
    char buffer[64];
    
    if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
    
    if (strcmp(buffer, "true") == 0 || strcmp(buffer, "TRUE") == 0 || strcmp(buffer, "1") == 0)
    {
        *val = true;
        return false;
    }
    if (strcmp(buffer, "false") == 0 || strcmp(buffer, "FALSE") == 0 || strcmp(buffer, "0") == 0)
    {
        *val = false;
        return false;
    }
    
    return true;
}

// read a string parameter - up to (size - 1) bytes
// returns false on success
bool TextFile::RetrieveParameter(const char * const param, char *val, int size, bool searchFromStart)
{
    if (FindParameter(param, searchFromStart)) return true;
    
    return (ReadNext(val, size));
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool TextFile::RetrieveQuotedStringParameter(const char * const param, char *val, int size, bool searchFromStart)
{
    if (FindParameter(param, searchFromStart)) return true;
    
    return (ReadNextQuotedString(val, size));
}

// return a parameter selected from a range of values
bool TextFile::RetrieveRangedParameter(const char * const param,
                                       double *val, bool searchFromStart)
{
    if (FindParameter(param, searchFromStart)) return true;
    if (ReadNextRanged(val)) return true;
    return false;
}

// find a parameter and move index to just after parameter
// NB can't have whitespace in parameter (might work but not guaranteed)
// in fact there are lots of ways this can be confused
// I'm assuming that the system strstr function is more efficient
// than anything I might come up with
bool TextFile::FindParameter(const char * const param,
                             bool searchFromStart)
{
    char *p;
    int len = strlen(param);
    
    if (searchFromStart) p = m_FileData;
    else p = m_Index;
    
    while (1)
    {
        p = strstr(p, param);
        if (p == 0) break; // not found at all
        if (p == m_FileData) // at beginning of file
        {
            if (*(p + len) < 33) // ends with whitespace
            {
                m_Index = p + len;
                return false;
            }
        }
        else
        {
            if (*(p - 1) < 33) // character before is whitespace
            {
                if (*(p + len) < 33) // ends with whitespace
                {
                    m_Index = p + len;
                    return false;
                }
            }
        }
        p += len;
    }
    if (m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::FindParameter(" << param
        << " - could not find parameter\n";
        exit(1);
    }
    return true;
}

// read the next whitespace delimited token - up to (size - 1) characters
// automatically copes with quote delimited strings
bool TextFile::ReadNext(char *val, int size)
{
    int len = 0;
    
    // find non-whitespace
    while (*m_Index < 33)
    {
        if (*m_Index == 0 && m_ExitOnErrorFlag)
        {
            std::cerr << "Error: TextFile::ReadNext no non-whitespace found\n";
            exit(1);
        }
        if (*m_Index == 0) return true;
        m_Index++;
    }
    
    if (*m_Index == '\"') return ReadNextQuotedString(val, size);
    
    // copy until whitespace
    while (*m_Index > 32)
    {
        *val = *m_Index;
        val++;
        m_Index++;
        len++;
        if (len == size - 1) break;
    }
    *val = 0;
    return false;
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool TextFile::ReadNextQuotedString(char *val, int size)
{
    char *start;
    char *end;
    int len;
    
    start = strstr(m_Index, "\"");
    if (start == 0 && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadNextQuotedString could not find opening \"\n";
        exit(1);
    }
    if (start == 0) return true;
    
    end = strstr(start + 1, "\"");
    if (end == 0 && m_ExitOnErrorFlag)
    {
        std::cerr << "Error: TextFile::ReadNextQuotedString could not find closing \"\n";
        exit(1);
    }
    if (end == 0) return true;
    
    len = end - start - 1;
    if (len >= size) len = size - 1;
    m_Index = start + len + 2;
    memcpy(val, start + 1, len);
    val[len] = 0;
    
    return false;
}

// read the next integer
bool TextFile::ReadNext(int *val)
{
    char buffer[64];
    
    if (ReadNext(buffer, sizeof(buffer))) return true;
    
    *val = (int)strtol(buffer, 0, 10);
    
    return false;
}

// read the next double
bool TextFile::ReadNext(double *val)
{
    char buffer[64];
    
    if (ReadNext(buffer, sizeof(buffer))) return true;
    
    *val = strtod(buffer, 0);
    
    return false;
}

// return the next ranged parameter
bool TextFile::ReadNextRanged(double *val)
{
    double low, high;
    if (ReadNext(&low)) return true;
    if (ReadNext(&high)) return true;
    
    // m_RangeControl is normally from 0 to 1.0
    *val = low + m_RangeControl * (high - low);
    return false;
}

// read an array of ints
bool TextFile::RetrieveParameter(const char * const param, int n,
                                 int *val, bool searchFromStart)
{
    int i;
    if (FindParameter(param, searchFromStart)) return true;
    
    for (i = 0; i < n; i++)
    {
        if (ReadNext(&(val[i]))) return true;
    }
    
    return false;
}

// read an array of doubles
bool TextFile::RetrieveParameter(const char * const param, int n,
                                 double *val, bool searchFromStart)
{
    int i;
    if (FindParameter(param, searchFromStart)) return true;
    
    for (i = 0; i < n; i++)
    {
        if (ReadNext(&(val[i]))) return true;
    }
    
    return false;
}

// read an array of ranged doubles
bool TextFile::RetrieveRangedParameter(const char * const param, int n,
                                       double *val, bool searchFromStart)
{
    int i;
    if (FindParameter(param, searchFromStart)) return true;
    
    for (i = 0; i < n; i++)
    {
        if (ReadNextRanged(&(val[i]))) return true;
    }
    
    return false;
}

// read a line, optionally ignoring blank lines and comments
// (comment string to end of line)
// returns true on error
bool TextFile::ReadNextLine(char *line, int size, bool ignoreEmpty,
                            char commentChar, char continuationChar)
{
    char *c;
    bool loopFlag = true;
    bool openQuotes = false;
    
    while (loopFlag)
    {
        if (ReadNextLine(line, size)) return true;
        
        if (commentChar)
        {
            c = line;
            while (*c)
            {
                if (*c == '"') openQuotes = !openQuotes;
                if (*c == commentChar && openQuotes == false)
                {
                    *c = 0;
                    break;
                }
                c++;
            }
        }
        
        if (ignoreEmpty)
        {
            c = line;
            while (*c)
            {
                if (*c > 32) 
                {
                    loopFlag = false;
                    break;
                }
                c++;
            }
        }
        else loopFlag = false;
        
        if (continuationChar)
        {
            c = line;
            if (*c)
            {
                while (*c)
                {
                    c++;
                    size--;
                }
                c--;
                if (*c == continuationChar)
                {
                    loopFlag = true;
                    size++;
                    line = c;
                }
            }
        }
    }
    return false;
}

// read a line
// returns true on error
bool TextFile::ReadNextLine(char *line, int size)
{
    char *p = m_Index;
    char *c = line;
    int count = 0;
    size--; // needs to be shrunk to make room for the zero
    
    if (*p == 0) return true; // at end of file
    
    while (EndOfLineTest(&p) == false)
    {
        if (count >= size)
        {
            *c = 0;
            if (m_ExitOnErrorFlag)
            {
                std::cerr << "Error: TextFile::ReadNextLine line longer than string\n";
                exit(1);
            }
            else return true;
        }
        
        *c = *p;
        count++;
        c++;
        p++;
    }
    m_Index = p;
    *c = 0;
    return false;
}

// tests for end of line and bumps pointer
// note takes a pointer to a pointer
bool TextFile::EndOfLineTest(char **p)
{
    if (**p == 0) return true; // don't bump past end of string
    if (**p == 10) // must be Unix style linefeed
    {
        (*p)++;
        return true;
    }
    if (**p == 13) // Mac or DOS
    {
        (*p)++;
        if (**p == 10) (*p)++; // DOS
        return true;
    }
    return false;
}

// Count token utility
int TextFile::CountTokens(char *string)
{
    char *p = string;
    bool inToken = false;
    int count = 0;
    
    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            count++;
            if (*p == '"')
            {
                p++;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
        }
        p++;
    }
    return count;
}

// Return tokens utility
// note string is altered by this routine
// if returned count is >= size then there are still tokens
// (this is probably an error status)
// recommend that tokens are counted first
int TextFile::ReturnTokens(char *string, char *ptrs[], int size)
{
    char *p = string;
    bool inToken = false;
    int count = 0;
    
    while (*p != 0)
    {
        if (inToken == false && *p > 32)
        {
            inToken = true;
            if (count >= size) return count;
            ptrs[count] = p;
            count++;
            if (*p == '"')
            {
                p++;
                ptrs[count - 1] = p;
                while (*p != '"')
                {
                    p++;
                    if (*p == 0) return count;
                }
                *p = 0;
            }
        }
        else if (inToken == true && *p <= 32)
        {
            inToken = false;
            *p = 0;
        }
        p++;
    }
    return count;
}

// read the next integer
bool TextFile::ReadNextBinary(int *val)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(int)) return true;
    *val = *((int *)m_Index);
    m_Index += sizeof(int);
    return false;
}

// read the next float
bool TextFile::ReadNextBinary(float *val)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(float)) return true;
    *val = *((float *)m_Index);
    m_Index += sizeof(float);
    return false;
}

// read the next double
bool TextFile::ReadNextBinary(double *val)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(double)) return true;
    *val = *((double *)m_Index);
    m_Index += sizeof(double);
    return false;
}

// read the next char
bool TextFile::ReadNextBinary(char *val)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(char)) return true;
    *val = *((char *)m_Index);
    m_Index += sizeof(char);
    return false;
}

// read the next bool
bool TextFile::ReadNextBinary(bool *val)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(bool)) return true;
    *val = *((bool *)m_Index);
    m_Index += sizeof(bool);
    return false;
}

// read the next integer array
bool TextFile::ReadNextBinary(int *val, int n)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(int) * n) return true;
    
    int *p = (int *)m_Index;
    for (int i = 0; i < n; i++)
    {
        *val = *p;
        p++;
        val++;
    }
    m_Index = (char *)p;
    return false;
}

// read the next float array
bool TextFile::ReadNextBinary(float *val, int n)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(float) * n) return true;
    
    float *p = (float *)m_Index;
    for (int i = 0; i < n; i++)
    {
        *val = *p;
        p++;
        val++;
    }
    m_Index = (char *)p;
    return false;
}

// read the next double array
bool TextFile::ReadNextBinary(double *val, int n)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(double) * n) return true;
    
    double *p = (double *)m_Index;
    for (int i = 0; i < n; i++)
    {
        *val = *p;
        p++;
        val++;
    }
    m_Index = (char *)p;
    return false;
}

// read the next character array
bool TextFile::ReadNextBinary(char *val, int n)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(char) * n) return true;
    
    char *p = (char *)m_Index;
    for (int i = 0; i < n; i++)
    {
        *val = *p;
        p++;
        val++;
    }
    m_Index = (char *)p;
    return false;
}

// read the next bool array
bool TextFile::ReadNextBinary(bool *val, int n)
{
    if (((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData) > m_Size - (ptrdiff_t)sizeof(bool) * n) return true;
    
    bool *p = (bool *)m_Index;
    for (int i = 0; i < n; i++)
    {
        *val = *p;
        p++;
        val++;
    }
    m_Index = (char *)p;
    return false;
}

// write an integer parameter
// returns false on success
bool TextFile::WriteParameter(const char * const param, int val)
{
    char buffer[64];
    
    sprintf(buffer, "%d", val);
    if (WriteParameter(param, buffer)) return true;
    
    return false;
}

// write a long parameter
// returns false on success
bool TextFile::WriteParameter(const char * const param, long val)
{
    char buffer[64];
    
    sprintf(buffer, "%ld", val);
    if (WriteParameter(param, buffer)) return true;
    
    return false;
}


// write a double parameter
// returns false on success
bool TextFile::WriteParameter(const char * const param, double val)
{
    char buffer[64];
    
    sprintf(buffer, "%17f", val);
    if (WriteParameter(param, buffer)) return true;
    
    return false;
}

// write a bool parameter
// returns false on success
bool TextFile::WriteParameter(const char * const param, bool val)
{
    char buffer[64];
    
    if (val) strcpy(buffer, "true");
    else strcpy(buffer, "false");
    if (WriteParameter(param, buffer)) return true;
    
    return true;
}

// write a string parameter
// returns false on success
bool TextFile::WriteParameter(const char * const param, const char * const val)
{
    if (WriteNext(param, '\t')) return true;
    if (WriteNext(val, '\n')) return true;
    
    return false;
}

// write a quoted string parameter
// returns false on success
bool TextFile::WriteQuotedStringParameter(const char * const param, const char * const val)
{
    if (WriteNext(param, '\t')) return true;
    if (WriteNextQuotedString(val, '\n')) return true;
    
    return false;
}

// write an integer parameter array
// returns false on success
bool TextFile::WriteParameter(const char * const param, int n, int *val)
{
    int i;
    if (n <= 0) return true;
    
    if (WriteNext(param, '\t')) return true;
    
    for (i = 0; i < n - 1; i++)
        if (WriteNext(val[i], '\t')) return true;
    
    if (WriteNext(val[i], '\n')) return true;
    return false;
}

// write a long parameter array
// returns false on success
bool TextFile::WriteParameter(const char * const param, int n, long *val)
{
    int i;
    if (n <= 0) return true;
    
    if (WriteNext(param, '\t')) return true;
    
    for (i = 0; i < n - 1; i++)
        if (WriteNext(val[i], '\t')) return true;
    
    if (WriteNext(val[i], '\n')) return true;
    return false;
}


// write a double parameter array
// returns false on success
bool TextFile::WriteParameter(const char * const param, int n, double *val)
{
    int i;
    if (n <= 0) return true;
    
    if (WriteNext(param, '\t')) return true;
    
    for (i = 0; i < n - 1; i++)
        if (WriteNext(val[i], '\t')) return true;
    
    if (WriteNext(val[i], '\n')) return true;
    return false;
}

// write a bool parameter array
// returns false on success
bool TextFile::WriteParameter(const char * const param, int n, bool *val)
{
    int i;
    if (n <= 0) return true;
    
    if (WriteNext(param, '\t')) return true;
    
    for (i = 0; i < n - 1; i++)
        if (WriteNext(val[i], '\t')) return true;
    
    if (WriteNext(val[i], '\n')) return true;
    return false;
}

// write an integer
// returns false on success
bool TextFile::WriteNext(int val, char after)
{
    char buffer[64];
    
    sprintf(buffer, "%d", val);
    if (WriteNext(buffer, after)) return true;
    
    return false;
}

// write a long
// returns false on success
bool TextFile::WriteNext(long val, char after)
{
    char buffer[64];
    
    sprintf(buffer, "%ld", val);
    if (WriteNext(buffer, after)) return true;
    
    return false;
}

// write a double
// returns false on success
bool TextFile::WriteNext(double val, char after)
{
    char buffer[64];
    
    sprintf(buffer, "%.17f", val);
    if (WriteNext(buffer, after)) return true;
    
    return false;
}

// write a bool
// returns false on success
bool TextFile::WriteNext(bool val, char after)
{
    char buffer[64];
    
    if (val) strcpy(buffer, "true");
    else strcpy(buffer, "false");
    if (WriteNext(buffer, after)) return true;
    
    return true;
}

// write a string
// returns false on success
// note string must be shorter than kStorageIncrement
bool TextFile::WriteNext(const char * const val, char after)
{
    char *p;
    const char *cp;
    bool needQuotes = false;
    int size = 0;
    
    // check for whitespace and measure actual size
    cp = val;
    while (*cp)
    {
        if (*cp < 33) needQuotes = true;
        cp++;
        size++;
    }
    
    if ((ptrdiff_t)m_Index + (ptrdiff_t)size + 16 >= (ptrdiff_t)m_FileData + (ptrdiff_t)m_Size)
    {
        p = new char[m_Size + kStorageIncrement];
        if (p == 0)
        {
            if (m_ExitOnErrorFlag)
            {
                std::cerr << "Error: TextFile::WriteNext(" << val
                << ") - could not allocate memory\n";
                exit(1);
            }
            else
            {
                return true;
            }
        }
        memcpy(p, m_FileData, m_Size);
        m_Index = p + ((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData);
        delete [] m_FileData;
        m_FileData = p;
        m_Size += kStorageIncrement;
    }
    
    if (needQuotes) *m_Index++ = '"';
    memcpy(m_Index, val, size);
    m_Index += size;
    if (needQuotes) *m_Index++ = '"';
    if (after) *m_Index++ = after;
    *m_Index = 0;
    
    return false;
}

// write a string
// returns false on success
// note string must be shorter than kStorageIncrement
bool TextFile::WriteNextQuotedString(const char * const val, char after)
{
    char *p;
    const char *cp;
    int size = 0;
    
    // check for whitespace and measure actual size
    cp = val;
    while (*cp)
    {
        cp++;
        size++;
    }
    
    if ((ptrdiff_t)m_Index + (ptrdiff_t)size + 16 >= (ptrdiff_t)m_FileData + (ptrdiff_t)m_Size)
    {
        p = new char[m_Size + kStorageIncrement];
        if (p == 0)
        {
            if (m_ExitOnErrorFlag)
            {
                std::cerr << "Error: TextFile::WriteNext(" << val
                << ") - could not allocate memory\n";
                exit(1);
            }
            else
            {
                return true;
            }
        }
        memcpy(p, m_FileData, m_Size);
        m_Index = p + ((ptrdiff_t)m_Index - (ptrdiff_t)m_FileData);
        delete [] m_FileData;
        m_FileData = p;
        m_Size += kStorageIncrement;
    }
    
    *m_Index++ = '"';
    memcpy(m_Index, val, size);
    m_Index += size;
    *m_Index++ = '"';
    if (after) *m_Index++ = after;
    *m_Index = 0;
    
    return false;
}

// strip out beginning and ending whitespace
void TextFile::Strip(char *str)
{
    char *p1, *p2;
    
    if (*str == 0) return;
    
    // heading whitespace
    if (*str <= ' ')
    {
        p1 = str;
        while (*p1)
        {
            if (*p1 > ' ') break;
            p1++;
        }
        p2 = str;
        while (*p1)
        {
            *p2 = *p1;
            p1++;
            p2++;
        }
        *p2 = 0;
    }
    
    if (*str == 0) return;
    
    // tailing whitespace
    p1 = str;
    while (*p1)
    {
        p1++;
    }
    p1--;
    while (*p1 <= ' ')
    {
        p1--;
    }
    p1++;
    *p1 = 0;
    
    return;
}






