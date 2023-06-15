// Debug.h - a set of constants used for debug control

#ifndef Debug_h
#define Debug_h

#define Debug_Axis 1
#define Debug_Cohort 2
#define Debug_DisplayGenealogy 3
#define Debug_ParameterFile 4
#define Debug_StrokeFont 5
#define Debug_write_dxf 6

#ifdef gDebugDeclare
int gDebug = 0;
#else
extern int gDebug;
#endif

#endif
