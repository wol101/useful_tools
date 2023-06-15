// Objective.h - class to hold details of running objectives

#if defined (USE_FS)

#ifndef OBJECTIVE_H
#define OBJECTIVE_H

class Genome;

class Objective
{
  public:
      
    Objective();
    ~Objective();

    bool Valid() { return m_ValidFlag; };
    int GetID() { return m_RunID; };
    double GetScore() { return m_Fitness; };

    void SetSynchronousFlag(bool flag) { m_SynchronousFlag = flag; };
    void SetProgramName(const char * name);
    void SetExtraArguments(const char * name);
    void SetReadGenomeFlag(bool flag) { m_ReadGenomeFlag = flag; };
        
    void Run(int id, Genome *genome);
    bool CheckFinished();
    void CleanUp();

  protected:

    char m_GenomeName[256];
    char m_ScoreName[256];
    char m_ProgramName[256];
    char m_ExtraArguments[256];
    
    int m_RunID;
    bool m_ValidFlag;
    double m_Fitness;
    bool m_SynchronousFlag;
    bool m_ReadGenomeFlag;
    
    Genome *m_Genome;
};  


#endif // OBJECTIVE_H

#elif defined (USE_PVM)

// Objective.h - class to hold details of running objectives

#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <time.h>
class Genome;

class Objective
{
  public:
      
    Objective();
    ~Objective();

    int GetID() { return m_RunID; };
    double GetScore() { return m_Fitness; };

    void SetProgramName(const char * name);
    void SetExtraArguments(const char * args);
    void SetExtraData(const char * data);
    void SetID(int id) { m_RunID = id; };
    void SetTimeLimit(time_t timeLimit) { m_TimeLimit = timeLimit; };
    void SetRestartOnError(int restartOnError) { m_RestartOnError = restartOnError; };
    void SetGenome(Genome *genome) { m_Genome = genome; };

    char * GetProgramName() { return m_ProgramName; };
                      
    void Run(char *preferredHost);
    bool CheckRunning();

    static char * GetPreferredHost(char *command);
    static int CountTasks(char *command);

  protected:

    char m_ProgramName[256];
    char m_ExtraArguments[1024];
    char *m_ExtraArgumentsList[256];
    int m_ExtraArgumentsCount;
    char *m_ExtraData;
    int m_ExtraDataLength;
    Genome *m_Genome;
    int m_RunID;
    double m_Fitness;
    int m_Child;
    time_t m_StartTime;
    time_t m_TimeLimit;
    int m_RestartOnError;
    bool m_RunningFlag;
};  


#endif // OBJECTIVE_H

#elif defined (USE_SOCKETS)

// Objective.h - class to hold details of running objectives

#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <time.h>
#include <pinet.h>

class Genome;

class Objective
{
  public:
      
    Objective();
    ~Objective();

    int GetID() { return m_RunID; };
    double GetScore() { return m_Fitness; };

    void SetProgramName(const char * name);
    void SetExtraArguments(const char * args);
    void SetExtraData(const char * data);
    void SetID(int id) { m_RunID = id; };
    void SetTimeLimit(time_t timeLimit) { m_TimeLimit = timeLimit; };
    void SetGenome(Genome *genome) { m_Genome = genome; };

    char * GetProgramName() { return m_ProgramName; };
                      
    bool Run();
    bool CheckRunning();

  protected:

    char m_ProgramName[256];
    char *m_ExtraArguments;
    int m_ExtraArgumentsLength;
    char *m_ExtraData;
    int m_ExtraDataLength;
    Genome *m_Genome;
    int m_RunID;
    double m_Fitness;
    int m_Child;
    time_t m_StartTime;
    time_t m_TimeLimit;
    bool m_RunningFlag;
    pt::ipstream *m_Client;
    
};  


#endif // OBJECTIVE_H

#endif
