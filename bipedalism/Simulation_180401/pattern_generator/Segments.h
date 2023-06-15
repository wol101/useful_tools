// Segments.h - the various segment objects

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <dmMobileBaseLink.h>
#include <dmRevoluteLink.h>

class ParameterFile;

class Torso: public dmMobileBaseLink
{
public:  
  
  Torso();
  ~Torso();
};
  
class LeftThigh: public dmRevoluteLink
{
public:  
  
  LeftThigh();
  ~LeftThigh();
};
  
class RightThigh: public dmRevoluteLink
{
public:  
  
  RightThigh();
  ~RightThigh();
};
  
class LeftLeg: public dmRevoluteLink
{
public:  
  
  LeftLeg();
  ~LeftLeg();
};
  
class RightLeg: public dmRevoluteLink
{
public:  
  
  RightLeg();
  ~RightLeg();
};
  
class LeftFoot: public dmRevoluteLink
{
public:  
  
  LeftFoot();
  ~LeftFoot();
};
  
class RightFoot: public dmRevoluteLink
{
public:  
  
  RightFoot();
  ~RightFoot();
};

void ReadSegmentGlobals(ParameterFile &file);
 
#endif // SEGMENTS_H
