// Routines for writing DXF files from
// AutoCAD Programming 2nd Ed D. N. Jump McGraw Hill 1991

#ifndef WRITE_DXF_H
#define WRITE_DXF_H

class ostream;

void generate_header(ostream &dxf, double xmin, double xmax,
    double ymin, double ymax); // optional!
void start_entities(ostream &dxf);
void finish_entities(ostream &dxf);
void generate_eof(ostream &dxf);

void write_line(ostream &dxf, double from_x, double from_y,
    double to_x, double to_y, char *layer = "LAYER_1");
void write_text(ostream &dxf, double x1, double y1, 
    double xscale /*scale factor */, double ht, 
    double angle, double oblique /* oblique angle */, char *txt,
    char *style, char *layer = "LAYER_1");




#endif // WRITE_DXF_H
