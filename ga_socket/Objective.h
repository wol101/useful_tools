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
