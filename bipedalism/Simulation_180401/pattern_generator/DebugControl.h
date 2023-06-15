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
  ControllerDebug = 1,
  MuscleDebug = 2,
  ContactDebug = 3,
  FitnessDebug = 4,
  MainDebug = 5,
  StrapForceDebug = 6,
  StrapForceAnchorDebug = 7
};

DEBUG_EXTERN DebugControl gDebug;

#endif // DEBUG_H
