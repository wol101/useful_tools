// ParameterFile.cc - class to read in parameter files (parameter value)

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "ParameterFile.h"

// default constructor
ParameterFile::ParameterFile()
{
  m_FileData = 0;
  m_Index = 0;
}

// default destructor
ParameterFile::~ParameterFile()
{
  if (m_FileData) delete [] m_FileData;
}

// read the named file
// returns true on error
bool ParameterFile::ReadFile(const char * const name)
{
  struct stat fileStat;
  FILE *in;
  int count;
  int error;
  
  if (m_FileData) delete [] m_FileData;
  m_FileData = 0;
  
  error = stat(name, &fileStat);
  if (error) return true;
  m_FileData = new char[fileStat.st_size + 1];
  if (m_FileData == 0) return true;
  m_Index = m_FileData;
  m_FileData[fileStat.st_size] = 0;
  
  in = fopen(name, "r");
  if (in == 0) return true;
  count = fread(m_FileData, fileStat.st_size, 1, in);
  fclose(in);
  if (count != 1) return true;
  
  return false;
}

// read an integer parameter
// returns false on success
bool ParameterFile::RetrieveParameter(const char * const param, int *val, bool searchFromStart)
{
  char buffer[64];
  
  if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
  
  *val = atoi(buffer);
  
  return false;
}


// read a double parameter
// returns false on success
bool ParameterFile::RetrieveParameter(const char * const param, double *val, bool searchFromStart)
{
  char buffer[64];
  
  if (RetrieveParameter(param, buffer, sizeof(buffer), searchFromStart)) return true;
  
  *val = atof(buffer);
  
  return false;
}

// read a string parameter - up to (size - 1) bytes
// returns false on success
bool ParameterFile::RetrieveParameter(const char * const param, char *val, int size, bool searchFromStart)
{
  char *start;
  int len = 0;

  if (FindParameter(param, searchFromStart)) return true;
  start = m_Index;
  
  while (*start < 33) // whitespace
  {
    if (*start == 0) return true; // reached end of string
    start++;
  }
  while (*start > 32)
  {
    *val = *start;
    val++;
    start++;
    len++;
    if (len == size - 1) break;
  }
  *val = 0;
  m_Index = start;
  return false;
}

// read a quoted string parameter - up to (size - 1) bytes
// returns false on success
bool ParameterFile::RetrieveQuotedStringParameter(const char * const param, char *val, int size, bool searchFromStart)
{
  char *start;
  char *end;
  int len;
  
  if (FindParameter(param, searchFromStart)) return true;
  
  start = strstr(m_Index, "\"");
  if (start == 0) return true;
  
  end = strstr(start + 1, "\"");
  if (end == 0) return true;
      
  len = end - start - 1;
  if (len >= size) len = size - 1;
  m_Index = start + len;
  memcpy(val, start + 1, len);
  val[len] = 0;
  
  return false;
}

// find a parameter and move index to just after parameter
// NB can't have whitespace in parameter (might work but not guaranteed)
// in fact there are lots of ways this can be confused
bool ParameterFile::FindParameter(const char * const param, 
    bool searchFromStart)
{
  char *p;
  int len = strlen(param);
  
  if (searchFromStart) m_Index = m_FileData;
    
  while (1)
  {
    p = strstr(m_Index, param);
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
  return true;
}

// read the next whitespace delimited token - up to (size - 1) characters
bool ParameterFile::ReadNext(char *val, int size)
{
  int len = 0;
  
  // find non-whitespace
  while (*m_Index < 33)
  {
    if (*m_Index == 0) return true;
    m_Index++;
  }
  
  // copy until whitespace
  while (*m_Index > 32)
  {
    *val = *m_Index;
    val++;
    m_Index++;
    len++;
    if (len == size - 1) break;
  }
  return false;
}

// read the next integer
bool ParameterFile::ReadNext(int *val)
{
  char buffer[64];
  
  if (ReadNext(buffer, sizeof(buffer))) return true;
  
  *val = atoi(buffer);    
  
  return false;
}

// read the next double
bool ParameterFile::ReadNext(double *val)
{
  char buffer[64];
  
  if (ReadNext(buffer, sizeof(buffer))) return true;
  
  *val = atof(buffer);    
  
  return false;
}

