// produce a genome that will produce some sort of attempt at a
// left foot up

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

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>

int main(int argc, char *argv[])
{
  ofstream out("left_foot_up.txt");
  int i;
  
  out << "2400\n";
  
  // LeftHipExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.1 ";
  out << "\n";
  
  // LeftHipFlexorController
  
  for (i = 0; i < 200; i++)
    out << "0.9 ";
  out << "\n";
  
  // LeftKneeExtensorController
  
  for (i = 0; i < 10; i++)
    out << "0.1 ";
  out << "\n";
  for (i = 10; i < 200; i++)
    out << "5.0 ";
  out << "\n";
  
  // LeftKneeFlexorController
  
  for (i = 0; i < 10; i++)
    out << "0.5 ";
  out << "\n";
  for (i = 10; i < 200; i++)
    out << "0.0 ";
  out << "\n";
  
  // LeftAnkleExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.3 ";
  out << "\n";
  
  // LeftAnkleFlexorController
  
  for (i = 0; i < 200; i++)
    out << "0.1 ";
  out << "\n";
  
  // RightHipExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.4 ";
  out << "\n";
  
  // RightHipFlexorController
  
  for (i = 0; i < 200; i++)
    out << "0.1 ";
  out << "\n";
  
  // RightKneeExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.1 ";
  out << "\n";
  
  // RightKneeFlexorController
  
  for (i = 0; i < 200; i++)
    out << "0.2 ";
  out << "\n";
  
  // RightAnkleExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.3 ";
  out << "\n";
  
  // RightAnkleExtensorController
  
  for (i = 0; i < 200; i++)
    out << "0.1 ";
  out << "\n";
  
}
  
