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
