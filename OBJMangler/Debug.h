// Debug.h - a set of constants used for debug control

#ifndef Debug_h
#define Debug_h

#define DebugFacetedObject 1
#define DebugGLUIRoutines 2
#define DebugMyFace 3
#define DebugOBJMangler 4
#define DebugTextFile 5

#ifdef gDebugDeclare
int gDebug = 0;
#else
extern int gDebug;
#endif

#endif
