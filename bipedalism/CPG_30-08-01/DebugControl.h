// DebugControl.h - holds some values useful for debugging

#ifndef DEBUG_H
#define DEBUG_H

// cope with declaring globals extern
#ifndef DEBUG_EXTERN
#define DEBUG_EXTERN extern
#endif // DEBUG_EXTERN
                                                                          
enum DebugControl
{
  NoDebug = 0,
  ContactDebug = 1,
  FitnessDebug = 2,
  MainDebug = 3,
  StrapForceDebug = 4,
  StrapForceAnchorDebug = 5,
  CPGDebug = 6,
  SegmentsDebug = 7,
  MAMuscleDebug = 8
};

DEBUG_EXTERN DebugControl gDebug;

#endif // DEBUG_H
