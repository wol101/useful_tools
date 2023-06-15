// DebugControl.h - holds some values useful for debugging

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

const int NoDebug = 0;
const int ContactDebug = 1;
const int FitnessDebug = 2;
const int MainDebug = 3;
const int StrapForceDebug = 4;
const int StrapForceAnchorDebug = 5;
const int CPGDebug = 6;
const int SegmentsDebug = 7;
const int MAMuscleDebug = 8;
const int MemoryDebug = 9;
const int SideStabilizerDebug = 10;
const int SocketDebug = 11;
const int SimulationDebug = 12;
const int DampedSpringDebug = 13;
const int GLUIDebug = 14;

const char * const gDebugLabels[] =
{
    "NoDebug", 
    "ContactDebug",
    "FitnessDebug", 
    "MainDebug", 
    "StrapForceDebug",
    "StrapForceAnchorDebug", 
    "CPGDebug",
    "SegmentsDebug", 
    "MAMuscleDebug", 
    "MemoryDebug", 
    "SideStabilizerDebug",
    "SocketDebug",
    "SimulationDebug",
    "DampedSpringDebug",
    "GLUIDebug" 
};

#ifndef DEBUG_MAIN
// cope with declaring globals extern
extern int gDebug;
extern ostream *gDebugStream;
#else
int gDebug = NoDebug;
ostream *gDebugStream = &cerr;
#endif

#endif // DEBUG_H
