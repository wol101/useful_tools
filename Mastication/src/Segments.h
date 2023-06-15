// Segments.h - the various segment objects

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <dmMobileBaseLink.hpp>

class DataFile;

class Mandible: public dmMobileBaseLink
{
public:

    Mandible();
    ~Mandible();
    void draw() const;
};

class Skull: public dmMobileBaseLink
{
public:

    Skull();
    ~Skull();
    void draw() const;
};


void ReadSegmentGlobals(DataFile &file);
 
#endif // SEGMENTS_H
