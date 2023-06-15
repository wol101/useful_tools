/*
 *  DebugControl.h
 *  PrimateAgent
 *
 *  Created by Bill Sellers on Fri Mar 04 2005.
 *  Copyright (c) 2005 Bill Sellers. All rights reserved.
 *
 */


#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

const int NoDebug = 0;
const int AgentDebug = 1;
const int DTDValidateDebug = 2;

const char * const gDebugLabels[] =
{
    "NoDebug", "AgentDebug", "DTDValidateDebug"
};

#ifndef DEBUG_MAIN
// cope with declaring globals extern
extern int g_Debug;
extern std::ostream *g_DebugStream;
#else
int g_Debug = NoDebug;
std::ostream *g_DebugStream = &std::cerr;
#endif

#endif // DEBUG_H




