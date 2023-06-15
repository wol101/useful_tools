// Cohort.cc - holds genome information

#include <assert.h>
#include "Cohort.h"

Cohort::Cohort()
{
  m_Size = 0;
  m_GenomeInfo = 0;
}

Cohort::~Cohort()
{
  if (m_GenomeInfo) delete [] m_GenomeInfo;
}

void 
Cohort::SetSize(int size)
{
  if (size != m_Size)
  {
    if (m_GenomeInfo) delete [] m_GenomeInfo;
    m_Size = size;
    m_GenomeInfo = new GenomeInfo[m_Size];
  }
}

GenomeInfo *
Cohort::GetGenomeInfoPtr(int index)
{
  assert(index >= 0 && index < m_Size);
  
  return &m_GenomeInfo[index];
}

