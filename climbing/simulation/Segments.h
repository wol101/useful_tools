// Segments.h - the various segment objects

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <dmMobileBaseLink.hpp>
#include <dmRevoluteLink.hpp>

class ParameterFile;

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
  void draw() const;
};
  
class RightLeg: public dmRevoluteLink
{
public:  
  
  RightLeg();
  ~RightLeg();
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

class LeftArm: public dmRevoluteLink
{
public:  
  
  LeftArm();
  ~LeftArm();
  void draw() const;
};
  
class RightArm: public dmRevoluteLink
{
public:  
  
  RightArm();
  ~RightArm();
  void draw() const;
};
  
class LeftForearm: public dmRevoluteLink
{
public:  
  
  LeftForearm();
  ~LeftForearm();
  void draw() const;
};
  
class RightForearm: public dmRevoluteLink
{
public:  
  
  RightForearm();
  ~RightForearm();
  void draw() const;
};
  
class LeftHand: public dmRevoluteLink
{
public:  
  
  LeftHand();
  ~LeftHand();
  void draw() const;
};
  
class RightHand: public dmRevoluteLink
{
public:  
  
  RightHand();
  ~RightHand();
  void draw() const;
};

void ReadSegmentGlobals(ParameterFile &file);
 
#endif // SEGMENTS_H
