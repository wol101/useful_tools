#!/usr/bin/perl -w

while ( 1 )
{
  # run objective socket for 1 hour
  system "/home/wis/cvs/GaitGen/bin/objective_socket --port 8086 --hostname 158.125.169.30 --runTimeLimit 3600"
}
