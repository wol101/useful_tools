// Cohort.h - holds genome information

#ifndef COHORT_H
#define COHORT_H

struct Parent
{
  int generationNumber;
  int rank;
};

struct GenomeInfo
{
  Parent parent1;
  Parent parent2;
  double fitness;
};

class Cohort
{
  public:
      Cohort();
      ~Cohort();
      
      void SetSize(int size);
      int GetSize() { return m_Size; };
      GenomeInfo * GetGenomeInfoPtr(int index);
      
  protected:
      
    GenomeInfo *m_GenomeInfo;
    int m_Size;
};

#endif // COHORT_H
