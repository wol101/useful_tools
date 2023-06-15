// ParameterFile.h - class to read in parameter files (parameter value)

#ifndef PARAMETERFILE_H
#define PARAMETERFILE_H

class ParameterFile
{
  public:
    ParameterFile();
    ~ParameterFile();

    bool ReadFile(const char * const name);
    bool RetrieveParameter(const char * const param, 
        int *val, bool searchFromStart = true);
    bool RetrieveParameter(const char * const param, 
        double *val, bool searchFromStart = true);
    bool RetrieveParameter(const char * const param, 
        char *val, int size, bool searchFromStart = true);
    bool RetrieveQuotedStringParameter(const char * const param, 
        char *val, int size, bool searchFromStart = true);
    bool FindParameter(const char * const param, bool searchFromStart = true);
    bool ReadNext(int *val);
    bool ReadNext(double *val);
    bool ReadNext(char *val, int size);
    bool RetrieveParameter(const char * const param, int n,
        int *val, bool searchFromStart = true);
    bool RetrieveParameter(const char * const param, int n,
        double *val, bool searchFromStart = true);

    void SetExitOnError(bool flag) { m_ExitOnErrorFlag = flag; };

    char * GetRawData() { return m_FileData; };

    int CountTokensToEOF(bool searchFromStart = true);
    int CountTokensToEOL(bool searchFromStart = true);
    
  protected:
    char * m_FileData;
    char * m_Index;
    bool m_ExitOnErrorFlag;
};

#endif // PARAMETERFILE_H

