// produce a genome that will try to mimic the fairly standard shape
// of the joint angular acceleration

// starts with both feet on the ground and moves left leg first
// generates the required GeneMapping file

enum Side
{
  left,
  right
};

const int kNumControllers = 12;
const char * const kControllerNames[kNumControllers] = {
"LeftHipExtensorController",
"LeftHipFlexorController",
"LeftKneeExtensorController",   
"LeftKneeFlexorController",     
"LeftAnkleExtensorController",   
"LeftAnkleFlexorController",     
"RightHipExtensorController",    
"RightHipFlexorController",       
"RightKneeExtensorController",  
"RightKneeFlexorController",     
"RightAnkleExtensorController",  
"RightAnkleFlexorController"
};

const double kStepTime = 0.01;
const int kSteps = 500;

double LeftHipExtensorController[kSteps] = {};
double RightHipExtensorController[kSteps] = {};
double LeftHipFlexorController[kSteps] = {};
double RightHipFlexorController[kSteps] = {};
double LeftKneeExtensorController[kSteps] = {};
double RightKneeExtensorController[kSteps] = {};
double LeftKneeFlexorController[kSteps] = {};
double RightKneeFlexorController[kSteps] = {};
double LeftAnkleExtensorController[kSteps] = {};
double RightAnkleExtensorController[kSteps] = {};
double LeftAnkleFlexorController[kSteps] = {};
double RightAnkleFlexorController[kSteps] = {};

void HipExtend(Side s, int i, double v);
void HipFlex(Side s, int i, double v);
void KneeExtend(Side s, int i, double v);
void KneeFlex(Side s, int i, double v);
void AnkleExtend(Side s, int i, double v);
void AnkleFlex(Side s, int i, double v);
void GaitHalfCycle(Side side, double t, int i, double startToeOff, 
  double startFlexForward, double startExtendForward, double finish);

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

double CalculateValue(double minTime, double maxTime, double maximum, double t);

int main(int argc, char *argv[])
{
  ofstream out("gait_gen1.txt");
  int i;
  double t;

  // create the required GeneMapping.dat file
  ofstream geneMappingFile("GeneMapping.dat");
  for (i = 0; i < kNumControllers; i++)
  {
    geneMappingFile << kControllerNames[i] << "\t" 
      << kSteps << "\t" << i * kSteps << "\n";
  }

  // genome size
      out << kSteps * kNumControllers << "\n";
  
  for (i = 0; i < kSteps; i++)
  {
    t = (double)i *kStepTime;

    // start off phase
    if (t  < 0.1)
    {
      HipExtend(right, i, 0.1); // push backwards at right hip
      KneeExtend(right, i, 0.2); // keep knee extended
      AnkleExtend(right, i, 0.05); // tilt leg forwards at right ankle

      HipFlex(left, i, 0.15); // start to move left leg forwards
      KneeFlex(left, i, 0.5); // bend knee to get foot off the ground
      AnkleExtend(left, i, 0.01); // needed to stop toe droop
    }
    else
      
    // extend left leg forwards
    if (t < 0.9)
    {
      HipExtend(right, i, 0.1); // push backwards at right hip
      KneeExtend(right, i, 0.3); // keep knee extended
      AnkleExtend(right, i, 0.05); // tilt leg forwards at right ankle

      HipFlex(left, i, 0.15); // move left leg forwards
      KneeExtend(left, i, 0.5); // extend knee to stride forward
      AnkleExtend(left, i, 0.05); // needed to stop toe droop
    }
    else 
      
    if (t < 4.75)
    {
      GaitHalfCycle(right, t, i, 0.9, 0.95, 1.2, 1.55);
      GaitHalfCycle(left, t, i, 1.55, 1.6, 1.85, 2.1);
      GaitHalfCycle(right, t, i, 2.1, 2.15, 2.4, 2.65);
      GaitHalfCycle(left, t, i, 2.65, 2.7, 2.95, 3.2);
      GaitHalfCycle(right, t, i, 3.2, 3.25, 3.5, 3.7);
      GaitHalfCycle(left, t, i, 3.7, 3.75, 4.0, 4.2);
      GaitHalfCycle(right, t, i, 4.2, 4.25, 4.5, 4.75);
    }
    else
    {
      LeftHipExtensorController[i] = LeftHipExtensorController[i - 1];
      RightHipExtensorController[i] = RightHipExtensorController[i - 1];
      LeftHipFlexorController[i] = LeftHipFlexorController[i - 1];
      RightHipFlexorController[i] = RightHipFlexorController[i - 1];
      LeftKneeExtensorController[i] = LeftKneeExtensorController[i - 1];
      RightKneeExtensorController[i] = RightKneeExtensorController[i - 1];
      LeftKneeFlexorController[i] = LeftKneeFlexorController[i - 1];
      RightKneeFlexorController[i] = RightKneeFlexorController[i - 1];
      LeftAnkleExtensorController[i] = LeftAnkleExtensorController[i - 1];
      RightAnkleExtensorController[i] = RightAnkleExtensorController[i - 1];
      LeftAnkleFlexorController[i] = LeftAnkleFlexorController[i - 1];
      RightAnkleFlexorController[i] = RightAnkleFlexorController[i - 1];
    }    
  } 
     
  // LeftHipExtensorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftHipExtensorController[i] << " ";
  out << "\n";
  
  // LeftHipFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftHipFlexorController[i] << " ";
  out << "\n";
  
  // LeftKneeExtensorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftKneeExtensorController[i] << " ";
  out << "\n";
  
  // LeftKneeFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftKneeFlexorController[i] << " ";
  out << "\n";
  
  // LeftAnkleExtensorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftAnkleExtensorController[i] << " ";
  out << "\n";
    
  // LeftAnkleFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << LeftAnkleFlexorController[i] << " ";
  out << "\n";
  
  // RightHipExtensorController
  
  for (i = 0; i < kSteps; i++)
    out << RightHipExtensorController[i] << " ";
  out << "\n";
  
  // RightHipFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << RightHipFlexorController[i] << " ";
  out << "\n";
  
  // RightKneeExtensorController
  
  for (i = 0; i < kSteps; i++)
    out << RightKneeExtensorController[i] << " ";
  out << "\n"; 
  
  // RightKneeFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << RightKneeFlexorController[i] << " ";
  out << "\n";
  
  // RightAnkleExtensorController

  for (i = 0; i < kSteps; i++)
    out << RightAnkleExtensorController[i] << " ";
  out << "\n";
  
  // RightAnkleFlexorController
  
  for (i = 0; i < kSteps; i++)
    out << RightAnkleFlexorController[i] << " ";
  out << "\n";

  // output the low bound, high bound and a zero fitness
  out << "0 10 0\n";
  
}

void GaitHalfCycle(Side side, double t, int i, double startToeOff, 
  double startFlexForward, double startExtendForward, double finish)
{
  Side offside;
  if (t < startToeOff || t >= finish) return;

  if (side == right) offside = left;
  else offside = right;

  // right toe off
  if (t < startFlexForward)
  {
    HipExtend(side, i, 0.05); // push backwards at right hip (weakly)
    KneeExtend(side, i, 0.05); // keep knee extended (weakly)
    AnkleFlex(side, i, 3.0); // powerful push off at ankle

    HipExtend(offside, i, 0.3); // start extending
    KneeExtend(offside, i, 0.3); // keep knee extended
    AnkleExtend(offside, i, 0.05); // start to put foot flat
  }
  else 
      
  // right leg forward swing
  if (t < startExtendForward)
  {      
    HipFlex(side, i, 0.5); // start to flex right hip (strongly)
    KneeFlex(side, i, 1.5); // bend knee to get foot off the ground (strongly)
    AnkleExtend(side, i, 0.8); // get foot back to horizontal(strongly)
      
    HipExtend(offside, i, 0.3); // start to extend left hip
    KneeExtend(offside, i, 0.3); // keep knee extended
    AnkleExtend(offside, i, 0.05); // extend ankle
  }
  else 
      
  // extend right leg forwards
  if (t < finish)
  {
    HipFlex(side, i, 0.15); // move right leg forwards
    KneeExtend(side, i, 0.5); // extend knee to stride forward
    AnkleExtend(side, i, 0.05); // needed to stop toe droop

    HipExtend(offside, i, 0.1); // push backwards at left hip
    KneeExtend(offside, i, 0.3); // keep knee extended
    AnkleExtend(offside, i, 0.05); // tilt leg forwards at left ankle
  }
}
  
void HipExtend(Side s, int i, double v)
{
  if (s == left)
  {
    LeftHipExtensorController[i] = v;
    LeftHipFlexorController[i] = 0;
  }
  if (s == right)
  {
    RightHipExtensorController[i] = v;
    RightHipFlexorController[i] = 0;
  }
}

void HipFlex(Side s, int i, double v)
{
  if (s == left)
  {
    LeftHipFlexorController[i] = v;
    LeftHipExtensorController[i] = 0;
  }
  if (s == right)
  {
    RightHipFlexorController[i] = v;
    RightHipExtensorController[i] = 0;
  }
}

void KneeExtend(Side s, int i, double v)
{
  if (s == left)
  {
    LeftKneeExtensorController[i] = v;
    LeftKneeFlexorController[i] = 0;
  }
  if (s == right)
  {
    RightKneeExtensorController[i] = v;
    RightKneeFlexorController[i] = 0;
  }
}

void KneeFlex(Side s, int i, double v)
{
  if (s == left)
  {
    LeftKneeFlexorController[i] = v;
    LeftKneeExtensorController[i] = 0;
  }
  if (s == right)
  {
    RightKneeFlexorController[i] = v;
    RightKneeExtensorController[i] = 0;
  }
}

void AnkleExtend(Side s, int i, double v)
{
  if (s == left)
  {
    LeftAnkleExtensorController[i] = v;
    LeftAnkleFlexorController[i] = 0;
  }
  if (s == right)
  {
    RightAnkleExtensorController[i] = v;
    RightAnkleFlexorController[i] = 0;
  }
}

void AnkleFlex(Side s, int i, double v)
{
  if (s == left)
  {
    LeftAnkleFlexorController[i] = v;
    LeftAnkleExtensorController[i] = 0;
  }
  if (s == right)
  {
    RightAnkleFlexorController[i] = v;
    RightAnkleExtensorController[i] = 0;
  }
}



