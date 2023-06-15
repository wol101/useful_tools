#!/usr/bin/python -u

import sys
import string
import array
import os

# set the number of ramp values
ramp = 10.0;

# set the genome size
genomeSize = int(9 * 256 * ramp);
print genomeSize;

# set the duration
duration = 1 / ramp;

# set max force values in Newtons

temporalisMaxForce = 410.0;
masseterMaxForce = 330.0;
medialPterygoidMaxForce = 250.0;
lateralPterygoidMaxForce = 37.0;

for leftTemporalisForce in [0, temporalisMaxForce]:
  for leftMasseterForce in [0, masseterMaxForce]:
    for leftMedialPterygoidForce in [0, medialPterygoidMaxForce]:
      for leftLateralPterygoidForce in [0, lateralPterygoidMaxForce]:
        for rightTemporalisForce in [0, temporalisMaxForce]:
          for rightMasseterForce in [0, masseterMaxForce]:
            for rightMedialPterygoidForce in [0, medialPterygoidMaxForce]:
              for rightLateralPterygoidForce in [0, lateralPterygoidMaxForce]:
                for r in range(1, ramp + 1):
                  scale = r / ramp;
                  print;
                  print duration;
                  print leftTemporalisForce*scale,leftMasseterForce*scale,leftMedialPterygoidForce*scale,leftLateralPterygoidForce*scale;
                  print rightTemporalisForce*scale,rightMasseterForce*scale,rightMedialPterygoidForce*scale,rightLateralPterygoidForce*scale;
      
