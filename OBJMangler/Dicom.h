/*
 *  Dicom.h
 *  OBJMangler
 *
 *  Created by Bill Sellers on 10/05/2009.
 *  Copyright 2009 Bill Sellers. All rights reserved.
 *
 */

#ifndef Dicom_h
#define Dicom_h

#include <iostream>
#include <stdlib.h>
#include <stdint.h>

void write_preamble(std::ostream &s);
void write_basic_dicom(std::ostream &s, uint8_t *data, int x, int y, int z, double xx, double yy, double zz);
void write_attribute(std::ostream &s, int Group, int Element, char *VR, char *str);
void write_attribute(std::ostream &s, int Group, int Element, char *VR, int v);
void write_attribute(std::ostream &s, int Group, int Element, char *VR, double v);
void write_attribute(std::ostream &s, int Group, int Element, char *VR, uint8_t *str, uint32_t len);

#endif

