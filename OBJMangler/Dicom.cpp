/*
 *  Dicom.cpp
 *  OBJMangler
 *
 *  Created by Bill Sellers on 10/05/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#include "Dicom.h"

#include <string.h>

void write_basic_dicom(std::ostream &s, uint8_t *data, int x, int y, int z, double xx, double yy, double zz)
{
    write_preamble(s);
    write_attribute(s, 2, 16, "UI", "1.2.840.10008.1.2.1"); // 1.2.840.10008.1.2.1 little endian; 1.2.840.10008.1.2.2 big endian
    write_attribute(s, 24, 80, "DS", zz); // slice thickness
    write_attribute(s, 40, 2, "US", 1); // samples per pixel
    write_attribute(s, 40, 8, "IS", z); // number of frames
    write_attribute(s, 40, 16, "US", y); // rows
    write_attribute(s, 40, 17, "US", x); // cols
    write_attribute(s, 40, 48, "DS", xx); // pixel spacing
    write_attribute(s, 40, 52, "DS", xx/yy); // aspect ratio
    write_attribute(s, 40, 256, "US", 8); // bits allocated
    write_attribute(s, 40, 257, "US", 8); // bits stored
    write_attribute(s, 40, 258, "US", 7); // high bit
    write_attribute(s, 0x7fe0, 0, "UL", x * y * z); // Pixel Data Group Length
    write_attribute(s, 0x7fe0, 16, "OB", data, x * y * z); // Pixel Data
}

void write_preamble(std::ostream &s)
{
    char preamble[128];
    memset(preamble, 0, 128);
    s.write(preamble, 128);
    s.write("DICM", 4);
}

void write_attribute(std::ostream &s, int Group, int Element, char *VR, char *str)
{
    // Group and Element
    s.put(static_cast<char>(Group % 256));
    s.put(static_cast<char>(Group / 256));
    s.put(static_cast<char>(Element % 256));
    s.put(static_cast<char>(Element / 256)); 
    
    s << VR;
    
    int len = strlen(str);
    bool odd = false;
    if (len % 2)
    {
        len++;
        odd = true;
    }
    s.put(len % 256);
    s.put(len / 256); 
    
    s << str;
    if (odd) s.put(0); // pad to make length even
}

void write_attribute(std::ostream &s, int Group, int Element, char *VR, int v)
{
    // Group and Element
    s.put(static_cast<char>(Group % 256));
    s.put(static_cast<char>(Group / 256));
    s.put(static_cast<char>(Element % 256));
    s.put(static_cast<char>(Element / 256)); 
    
    s << VR;
    
    if (VR[0] == 'I')
    {
        int len = 2;
        s.put(len % 256);
        s.put(len / 256); 
        
        // assume little endian already
        char *ptr = (char *)(&v);
        s.put(*ptr++);
        s.put(*ptr++);
    }
    else
    {
        int len = 4;
        s.put(len % 256);
        s.put(len / 256); 
        
        // assume little endian already
        char *ptr = (char *)(&v);
        s.put(*ptr++);
        s.put(*ptr++);
        s.put(*ptr++);
        s.put(*ptr++);
    }
}

void write_attribute(std::ostream &s, int Group, int Element, char *VR, double v)
{
    // Group and Element
    s.put(static_cast<char>(Group % 256));
    s.put(static_cast<char>(Group / 256));
    s.put(static_cast<char>(Element % 256));
    s.put(static_cast<char>(Element / 256)); 
    
    s << VR;
    
    int len = 8;
    s.put(len % 256);
    s.put(len / 256); 
    
    // assume little endian already
    char *ptr = (char *)(&v);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
    s.put(*ptr++);
}

void write_attribute(std::ostream &s, int Group, int Element, char *VR, uint8_t *str, uint32_t len)
{
    // Group and Element
    s.put(static_cast<char>(Group % 256));
    s.put(static_cast<char>(Group / 256));
    s.put(static_cast<char>(Element % 256));
    s.put(static_cast<char>(Element / 256)); 
    
    s << VR;
    
    bool odd = false;
    if (len % 2)
    {
        len++;
        odd = true;
    }
    s.put(0);
    s.put(0);
    s.put((len & 0x000000ff));
    s.put((len & 0xffff00ff) / 0xff);
    s.put((len & 0xff00ffff) / 0xffff);
    s.put((len & 0x00ffffff) / 0xffffff);
    
    s.write((char *)str, len);
    if (odd) s.put(0); // pad to make length even
}

