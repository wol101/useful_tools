// Segments.h - the various segment objects

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <dmMobileBaseLink.hpp>
#include <dmRevoluteLink.hpp>

class DataFile;

class Torso: public dmMobileBaseLink
{
public:  
  
  Torso();
  ~Torso();  
  void draw() const;
};
  
class LeftThigh: public dmRevoluteLink
{
public:  
  
  LeftThigh();
  ~LeftThigh();
  void UpdateMidpoint();
  void draw() const;
};
  
class RightThigh: public dmRevoluteLink
{
public:  
  
  RightThigh();
  ~RightThigh();
  void UpdateMidpoint();
  void draw() const;
};
  
class LeftLeg: public dmRevoluteLink
{
public:  
  
  LeftLeg();
  ~LeftLeg();
  void UpdateMidpoint();
  void draw() const;
};
  
class RightLeg: public dmRevoluteLink
{
public:  
  
  RightLeg();
  ~RightLeg();
  void UpdateMidpoint();
  void draw() const;
};
  
class LeftFoot: public dmRevoluteLink
{
public:  
  
  LeftFoot();
  ~LeftFoot();
  void draw() const;
};
  
class RightFoot: public dmRevoluteLink
{
public:  
  
  RightFoot();
  ~RightFoot();
  void draw() const;
};

void ReadSegmentGlobals(DataFile &file);
 
#endif // SEGMENTS_H
