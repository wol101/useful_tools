// Routines for writing DXF files from
// AutoCAD Programming 2nd Ed D. N. Jump McGraw Hill 1991

// converted to C++ streams

#include <iostream.h>
#include <fstream.h>

#include "write_dxf.h"

static void write_DXF(ostream &dxf, int tag, const void *value);

// this is actually optional but it's useful if you
// want to set the limits and extents
void generate_header(ostream &dxf, double xmin, double xmax,
    double ymin, double ymax)
{
  write_DXF(dxf, 0, "SECTION");
  write_DXF(dxf, 2, "HEADER");
  write_DXF(dxf, 9, "$LIMMIN");
  write_DXF(dxf, 10, &xmin);
  write_DXF(dxf, 20, &ymin);
  write_DXF(dxf, 9, "$LIMMAX");
  write_DXF(dxf, 10, &xmax);
  write_DXF(dxf, 20, &ymax);
  write_DXF(dxf, 9, "$EXTMIN");
  write_DXF(dxf, 10, &xmin);
  write_DXF(dxf, 20, &ymin);
  write_DXF(dxf, 9, "$EXTMAX");
  write_DXF(dxf, 10, &xmax);
  write_DXF(dxf, 20, &ymax);
  write_DXF(dxf, 0, "ENDSEC");
}

void generate_eof(ostream &dxf)
{
  dxf << "  0\nEOF\n";
}

void write_DXF(ostream &dxf, int tag, const void *value)
{
  dxf << tag << "\n";
  if (tag <= 9)
    dxf <<  (char *)value << "\n";
  else
    if (tag <= 59)
      dxf << *(double *)value << "\n";
    else
      dxf << *(int *)value << "\n";
}

void start_entities(ostream &dxf)
{
  write_DXF(dxf, 0, "SECTION");
  write_DXF(dxf, 2, "ENTITIES");
}


void finish_entities(ostream &dxf) 
{
  write_DXF(dxf, 0, "ENDSEC");
}

void write_line(ostream &dxf, double from_x, double from_y,
    double to_x, double to_y, char *layer)
{ 
  write_DXF(dxf, 0, "LINE");
  write_DXF(dxf, 8, layer);
  write_DXF(dxf, 10, &from_x);
  write_DXF(dxf, 20, &from_y);
  write_DXF(dxf, 11, &to_x);
  write_DXF(dxf, 21, &to_y);
}
  
void write_text(ostream &dxf, double x1, double y1, 
    double xscale /*scale factor */, double ht, 
    double angle, double oblique /* oblique angle */, char *txt,
    char *style, char *layer)
{
  write_DXF(dxf, 0, "TEXT");
  write_DXF(dxf, 8, layer);
  write_DXF(dxf, 10, &x1);
  write_DXF(dxf, 20, &y1);
  write_DXF(dxf, 40, &ht);
  write_DXF(dxf, 1, txt);
  if (angle) write_DXF(dxf, 50, &angle);
  if (xscale != 1.0) write_DXF(dxf, 41, &xscale);
  if (oblique) write_DXF(dxf, 51, &oblique);
  if (style) write_DXF(dxf, 7, style);
}

     
  
