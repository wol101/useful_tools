// LoggedForce - a virtual subclass of dmForce that allows logging by creating a log file
// when the force name is set

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <assert.h>
#include <string.h>

#include <dm.h>
#include <dmForce.h>

#include "LoggedForce.h"

#define NAME_SIZE           256
#define FILENAME_LENGTH     31

LoggedForce::LoggedForce(void)
{
  mOutput = 0;
  mLoggingFlag = false;
  m_power = 0;
  m_force = 0;
}

LoggedForce::~LoggedForce(void)
{
  if (mOutput)
    delete mOutput;
}

bool LoggedForce::startLog(void)
{
  char theString[NAME_SIZE];

  if (getName() == 0)
    return true;
  strncpy(theString, getName(), FILENAME_LENGTH);
  theString[FILENAME_LENGTH] = 0;

  try
  {
    mOutput = new ofstream(theString);
    mLoggingFlag = true;
    return false;
  }

  catch(...)
  {
    return true;
  }
}

bool LoggedForce::stopLog(void)
{
  if (mOutput == 0)
    return true;

  try
  {
    delete mOutput;
    mLoggingFlag = false;
    return false;
  }

  catch(...)
  {
    return true;
  }
}
