#!/usr/bin/python -u

import sys
import string
import array
import os

# set the genome size
genomeSize = 9 * 256;
print genomeSize;

# set the duration
duration = 1;

# set max force values in Newtons

temporalisMaxForce = 410;
masseterMaxForce = 330;
medialPterygoidMaxForce = 250;
lateralPterygoidMaxForce = 37;

for leftTemporalisForce in [0, temporalisMaxForce]:
  for leftMasseterForce in [0, masseterMaxForce]:
    for leftMedialPterygoidForce in [0, medialPterygoidMaxForce]:
      for leftLateralPterygoid in [0, lateralPterygoidMaxForce]:
        for rightTemporalisForce in [0, temporalisMaxForce]:
          for rightMasseterForce in [0, masseterMaxForce]:
            for rightMedialPterygoidForce in [0, medialPterygoidMaxForce]:
              for rightLateralPterygoid in [0, lateralPterygoidMaxForce]:
                print;
                print duration;
                print leftTemporalisForce,leftMasseterForce,leftMedialPterygoidForce,leftLateralPterygoid;
                print rightTemporalisForce,rightMasseterForce,rightMedialPterygoidForce,rightLateralPterygoid;
      
