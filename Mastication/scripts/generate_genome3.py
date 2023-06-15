#!/usr/bin/python -u

import sys
import string
import array
import os

# set the number of ramp values
ramp = 100;

# set the genome size
genomeSize = int(9 * 256 * ramp);
print genomeSize;

# set the duration
duration = 1.0 / float(ramp);

# set max force values in Newtons

temporalisMaxForce = 410.0;
masseterMaxForce = 330.0;
medialPterygoidMaxForce = 250.0;
lateralPterygoidMaxForce = 37.0;

# create a list of desired force values

leftTemporalisForceList = []
leftMasseterForceList = []
leftMedialPterygoidForceList = []
leftLateralPterygoidForceList = []
rightTemporalisForceList = []
rightMasseterForceList = []
rightMedialPterygoidForceList = []
rightLateralPterygoidForceList = []

for leftTemporalisForce in [0.0, temporalisMaxForce]:
  for leftMasseterForce in [0.0, masseterMaxForce]:
    for leftMedialPterygoidForce in [0.0, medialPterygoidMaxForce]:
      for leftLateralPterygoidForce in [0.0, lateralPterygoidMaxForce]:
        for rightTemporalisForce in [0.0, temporalisMaxForce]:
          for rightMasseterForce in [0.0, masseterMaxForce]:
            for rightMedialPterygoidForce in [0.0, medialPterygoidMaxForce]:
              for rightLateralPterygoidForce in [0.0, lateralPterygoidMaxForce]:
                leftTemporalisForceList.append(leftTemporalisForce)
                leftMasseterForceList.append(leftMasseterForce)
                leftMedialPterygoidForceList.append(leftMedialPterygoidForce)
                leftLateralPterygoidForceList.append(leftLateralPterygoidForce)
                rightTemporalisForceList.append(rightTemporalisForce)
                rightMasseterForceList.append(rightMasseterForce)
                rightMedialPterygoidForceList.append(rightMedialPterygoidForce)
                rightLateralPterygoidForceList.append(rightLateralPterygoidForce)
      
i = 0
for r in range(0, ramp):
  scale = float(r) / float(ramp - 1)
  leftTemporalisForce = scale * leftTemporalisForceList[i]
  leftMasseterForce = scale * leftMasseterForceList[i]
  leftMedialPterygoidForce = scale * leftMedialPterygoidForceList[i]
  leftLateralPterygoidForce = scale * leftLateralPterygoidForceList[i]
  rightTemporalisForce = scale * rightTemporalisForceList[i]
  rightMasseterForce = scale * rightMasseterForceList[i]
  rightMedialPterygoidForce = scale * rightMedialPterygoidForceList[i]
  rightLateralPterygoidForce = scale * rightLateralPterygoidForceList[i]

  print
  print duration
  print leftTemporalisForce,leftMasseterForce,leftMedialPterygoidForce,leftLateralPterygoidForce
  print rightTemporalisForce,rightMasseterForce,rightMedialPterygoidForce,rightLateralPterygoidForce

i = 1
while i < 256:
  for r in range(0, ramp):
    scale = float(r) / float(ramp - 1)
    leftTemporalisForce = leftTemporalisForceList[i - 1] + scale * (leftTemporalisForceList[i] - leftTemporalisForceList[i - 1])
    leftMasseterForce = leftMasseterForceList[i - 1] + scale * (leftMasseterForceList[i] - leftMasseterForceList[i - 1])
    leftMedialPterygoidForce = leftMedialPterygoidForceList[i - 1] + scale * (leftMedialPterygoidForceList[i] - leftMedialPterygoidForceList[i - 1])
    leftLateralPterygoidForce = leftLateralPterygoidForceList[i - 1] + scale * (leftLateralPterygoidForceList[i] - leftLateralPterygoidForceList[i - 1])
    rightTemporalisForce = rightTemporalisForceList[i - 1] + scale * (rightTemporalisForceList[i] - rightTemporalisForceList[i - 1])
    rightMasseterForce = rightMasseterForceList[i - 1] + scale * (rightMasseterForceList[i] - rightMasseterForceList[i - 1])
    rightMedialPterygoidForce = rightMedialPterygoidForceList[i - 1] + scale * (rightMedialPterygoidForceList[i] - rightMedialPterygoidForceList[i - 1])
    rightLateralPterygoidForce = rightLateralPterygoidForceList[i - 1] + scale * (rightLateralPterygoidForceList[i] - rightLateralPterygoidForceList[i - 1])
    
    print
    print duration
    print leftTemporalisForce,leftMasseterForce,leftMedialPterygoidForce,leftLateralPterygoidForce
    print rightTemporalisForce,rightMasseterForce,rightMedialPterygoidForce,rightLateralPterygoidForce
    
  i = i + 1
