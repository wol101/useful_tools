#!/bin/tcsh

limit coredumpsize 0
limit cputime 3600
limit memoryuse 100000
while ( 1 )
  # run objective socket for 1 hour
  $HOME/cvs/GaitGen/bin/objective_socket --runTimeLimit 3600 --hostname ulgbc1.liv.ac.uk
end

