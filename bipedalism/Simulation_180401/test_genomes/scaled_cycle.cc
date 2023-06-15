// produce a genome that will try to mimic the fairly standard shape
// of the joint angular acceleration

// starts with both feet on the ground and the left foot in front

// StepTime = 0.01
// duration should be approx 0.4s

/* GeneMapping.dat
LeftHipExtensorController       200     0
LeftHipFlexorController 200     200
LeftKneeExtensorController      200     400
LeftKneeFlexorController        200     600
LeftAnkleExtensorController     200     800
LeftAnkleFlexorController       200     1000
RightHipExtensorController      200     1200
RightHipFlexorController        200     1400
RightKneeExtensorController     200     1600
RightKneeFlexorController       200     1800
RightAnkleExtensorController    200     2000
RightAnkleFlexorController      200     2200
*/

const double kGaitCyclePeriod = 1.6; 
const double kAnkleSpikeDuration = 0.1;
const double kAnkleSpikeSize = 3.0;
const double kAnkleNegativeSpikeDuration = 0.2;
const double kAnkleNegativeSpikeSize = 1.5;
const double kKneeSpikeDuration = 0.4;
const double kKneeSpikeSize = 0.5;
const double kKneeNegativeSpikeDuration = 0.4;
const double kKneeNegativeSpikeSize = 0.5;
const double kHipFlexSize = 0.0;
const double kHipExtendSize = 0.4;
const double kKneeExtendSize = 0.5;

const double kStepTime = 0.01;
const int kSteps = 200;

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

double CalculateValue(double minTime, double maxTime, double maximum, double t);

int main(int argc, char *argv[])
{
  ofstream out("scaled_cycle.txt");
  int i;
  double t;
  double LeftHipExtensorController[kSteps];
  double RightHipExtensorController[kSteps];
  double LeftHipFlexorController[kSteps];
  double RightHipFlexorController[kSteps];
  double LeftKneeExtensorController[kSteps];
  double RightKneeExtensorController[kSteps];
  double LeftKneeFlexorController[kSteps];
  double RightKneeFlexorController[kSteps];
  double LeftAnkleExtensorController[kSteps];
  double RightAnkleExtensorController[kSteps];
  double LeftAnkleFlexorController[kSteps];
  double RightAnkleFlexorController[kSteps];

  // genome size
  out << "2400\n";
  
  for (i = 0; i < kSteps; i++)
  {
    t = (double)i *kStepTime;
    while (t > kGaitCyclePeriod) t -= kGaitCyclePeriod;

    // LeftHipExtensorController
    if (t < kGaitCyclePeriod / 2)
    {
      RightHipFlexorController[i] = kHipFlexSize;
      RightHipExtensorController[i] = 0;
      LeftHipExtensorController[i] = kHipExtendSize;
      LeftHipFlexorController[i] = 0;
    }
    else
    {
      RightHipFlexorController[i] = 0;
      RightHipExtensorController[i] = kHipExtendSize;
      LeftHipExtensorController[i] = 0;
      LeftHipFlexorController[i] = kHipFlexSize;
    }
      
    LeftKneeExtensorController[i] = 
        CalculateValue(kGaitCyclePeriod - kKneeNegativeSpikeDuration, 
        kGaitCyclePeriod, kKneeNegativeSpikeSize, t);
    LeftKneeFlexorController[i] =
        CalculateValue(kGaitCyclePeriod / 2, 
        kGaitCyclePeriod / 2 + kKneeSpikeDuration, kKneeSpikeSize, t);
    
    LeftAnkleExtensorController[i] = 
        CalculateValue(kGaitCyclePeriod - kAnkleNegativeSpikeDuration, 
          kGaitCyclePeriod, kAnkleNegativeSpikeSize, t);
    LeftAnkleFlexorController[i] =
        CalculateValue(kGaitCyclePeriod / 2, 
        kGaitCyclePeriod / 2 + kAnkleSpikeDuration, kAnkleSpikeSize, t);
    
    RightKneeExtensorController[i] =
        CalculateValue(kGaitCyclePeriod / 2 - kKneeNegativeSpikeDuration, 
        kGaitCyclePeriod / 2, kKneeNegativeSpikeSize, t);
    RightKneeFlexorController[i] =
        CalculateValue(0, 
        kKneeSpikeDuration, kKneeSpikeSize, t);
    
    RightAnkleExtensorController[i] =
        CalculateValue(kGaitCyclePeriod / 2 - kAnkleNegativeSpikeDuration, 
        kGaitCyclePeriod / 2, kAnkleNegativeSpikeSize, t);
    RightAnkleFlexorController[i] =
        CalculateValue(0, 
        kAnkleSpikeDuration, kAnkleSpikeSize, t);
    
    // do some juggling for the knee
    
    if (RightKneeFlexorController[i] == 0) RightKneeExtensorController[i] = kKneeExtendSize;
    if (LeftKneeFlexorController[i] == 0) LeftKneeExtensorController[i] = kKneeExtendSize;
  } 
     
  // LeftHipExtensorController
  
  for (i = 0; i < 200; i++)
    out << LeftHipExtensorController[i] << " ";
  out << "\n";
  
  // LeftHipFlexorController
  
  for (i = 0; i < 200; i++)
    out << LeftHipFlexorController[i] << " ";
  out << "\n";
  
  // LeftKneeExtensorController
  
  for (i = 0; i < 200; i++)
    out << LeftKneeExtensorController[i] << " ";
  out << "\n";
  
  // LeftKneeFlexorController
  
  for (i = 0; i < 200; i++)
    out << LeftKneeFlexorController[i] << " ";
  out << "\n";
  
  // LeftAnkleExtensorController
  
  for (i = 0; i < 200; i++)
    out << LeftAnkleExtensorController[i] << " ";
  out << "\n";
    
  // LeftAnkleFlexorController
  
  for (i = 0; i < 200; i++)
    out << LeftAnkleFlexorController[i] << " ";
  out << "\n";
  
  // RightHipExtensorController
  
  for (i = 0; i < 200; i++)
    out << RightHipExtensorController[i] << " ";
  out << "\n";
  
  // RightHipFlexorController
  
  for (i = 0; i < 200; i++)
    out << RightHipFlexorController[i] << " ";
  out << "\n";
  
  // RightKneeExtensorController
  
  for (i = 0; i < 200; i++)
    out << RightKneeExtensorController[i] << " ";
  out << "\n"; 
  
  // RightKneeFlexorController
  
  for (i = 0; i < 200; i++)
    out << RightKneeFlexorController[i] << " ";
  out << "\n";
  
  // RightAnkleExtensorController

  for (i = 0; i < 200; i++)
    out << RightAnkleExtensorController[i] << " ";
  out << "\n";
  
  // RightAnkleFlexorController
  
  for (i = 0; i < 200; i++)
    out << RightAnkleFlexorController[i] << " ";
  out << "\n";

  
}
  
double CalculateValue(double minTime, double maxTime, double maximum, double t)
{
  if (t <= minTime) return 0;
  if (t >= maxTime) return 0;
  
  // scale time from zero to one
  t = (t - minTime) / (maxTime - minTime);
  
  // use an inverted parabola with max pf 1 at 0.5 and zeros at 0 and 1
  
  return maximum * (t * 4 - t * t * 4);
}

