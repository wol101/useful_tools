/*
 *  DebugControl.h
 *  AsynchronousGA
 *
 *  Created by Bill Sellers on 21/10/2010.
 *  Copyright 2010 Bill Sellers. All rights reserved.
 *
 */

#ifndef DEBUGCONTROL_H
#define DEBUGCONTROL_H

namespace AsynchronousGA
{

enum DebugControl
{
    NoDebug = 0,
    DataFileDebug,
    GenomeDebug,
    MainDebug,
    MatingDebug,
    PopulationDebug,
    RandomDebug,
    StatisticsDebug,
    PreferencesDebug
};

const char * const gDebugLabels[] =
{
    "NoDebug",
    "DataFileDebug",
    "GenomeDebug",
    "MainDebug",
    "MatingDebug",
    "PopulationDebug",
    "RandomDebug",
    "StatisticsDebug",
    "PreferencesDebug"
};

#ifndef DEBUG_MAIN
// cope with declaring globals extern
extern DebugControl gDebugControl;
#else
DebugControl gDebugControl = NoDebug;
#endif

// some handy defines for debugging
#define PRINT_LINE std::cerr << __LINE__ << "\n";

}

#endif // DEBUGCONTROL_H
