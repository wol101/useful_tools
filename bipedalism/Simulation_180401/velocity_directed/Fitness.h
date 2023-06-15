// Fitness.h

// holds the objective function for GA use

#ifndef FITNESS_H
#define FITNESS_H

#include <vector>
#include "Util.h"
#include <dm.h>

class ModifiedContactModel;

class Fitness
{
  public:
      
  Fitness();  // default constructor
  ~Fitness();  // destructor
  
  void ReadInitialisationData(bool graphicsFlag);
  double CalculateFitness();
  
  void SetGenomeFileName(char *name) { m_GenomeFileName = name; };
  void SetKineticsFileName(char *name) { m_KineticsFileName = name; };

  static void ConvertLocalToWorldP(const dmABForKinStruct *m, const CartesianVector local, CartesianVector world);
  static void ConvertLocalToWorldV(const dmABForKinStruct *m, const SpatialVector local, SpatialVector world);
  static void GetContactLimits(ModifiedContactModel *contactModel, const dmABForKinStruct *m,
    double &xMin, double &yMin, double &zMin, 
    double &xMax, double &yMax, double &zMax);
  static bool SanityCheck(const SpatialVector v);
  static void GoalXVelocity(double theTime, 
    double *leftFootXV, bool *leftFootShouldContact,
    double *rightFootXV, bool *rightFootShouldContact,
    double *torsoV);

  protected:

  void OutputKinetics();
      
  double      *m_Genome;
  int         m_GenomeLength;
  
  char        *m_GenomeFileName;
  char        *m_ControlFileName;
  char        *m_GeneMappingFileName;
  char        *m_KineticsFileName;
  char        *m_ConfigFileName;

  ofstream     m_OutputKinetics;
};

#endif // FITNESS_H
