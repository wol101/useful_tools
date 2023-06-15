// TestObjective.cc - routine to calculate the fitness

// this routine give a score of 100 if all values in the genome are 0.5
// if they aren't then the value is lower...

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>

#include <pvm3.h>

#include "PVMMessages.h"

int main(int argc, char *argv[])
{
  int i;
  double score = 0;
  double difference;
  int genomeLength;
  double genomeData[256];

   // check PVM
  int mytid = pvm_mytid();
  if (mytid < 0)
  {
     cerr << "PVM error: pvm_mytid()\n";
     return -1;
  }
  
  // get parent ID
  int myparent = pvm_parent();
  if (myparent < 0)
  {
     cerr << "PVM error: pvm_parent()\n";
     pvm_exit();
     return -1;
  }
  
  // get genome from parent
  int  bufid = pvm_recv( myparent, kMessageDoubleArray );
  int info = pvm_upkint( &genomeLength, 1, 1 );
  info = pvm_upkdouble( genomeData, genomeLength, 1 );
  
   // very simple fitness function (make value close to 0.5)  
  for (i = 0; i < genomeLength; i++)
  {
    difference = genomeData[i] - 0.5;
    score -= (difference * difference);
  }

  // make score positive by adding 100
  score += 100;
  
  // send fitness back to parent
  bufid = pvm_initsend( PvmDataDefault );
  info = pvm_pkdouble( &score, 1, 1 );
  info = pvm_send( myparent, kMessageDouble );
  
  // wait for acknowledge before quitting
  bufid = pvm_recv( myparent, kMessageAcknowledge );
  
  info = pvm_exit();
}

