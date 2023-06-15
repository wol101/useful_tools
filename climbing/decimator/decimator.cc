#include <string.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkOBJReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkOutlineFilter.h"
#include "vtkCamera.h"
#include "vtkStripper.h"
#include "vtkOBJExporter.h"
#include "vtkDecimatePro.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"

int main( int argc, char *argv[] )
{
  char *in = 0;
  char *out = 0;
  int i;
  double decimateLevel = 0.8;
  bool debugFlag = false;
  bool smoothFlag = false;
  
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--in") == 0)
    {
      i++;
      if (i > argc) return 1;
      in = argv[i];
    }
    else
    if (strcmp(argv[i], "--out") == 0)
    {
      i++;
      if (i > argc) return 1;
      out = argv[i];
    }
    else
    if (strcmp(argv[i], "--decimateLevel") == 0)
    {
      i++;
      if (i > argc) return 1;
      sscanf(argv[i], "%lf", &decimateLevel);
    }
    else
    if (strcmp(argv[i], "--help") == 0 || 
      strcmp(argv[i], "-h") == 0 ||
      strcmp(argv[i], "-?") == 0)
    {
      cerr << "\nOBJ viewer and decimator\n\n";
      cerr << "Command line arguments:\n\n";
      cerr << "--in filename\n";
      cerr << "--out filename.obj\n";
      cerr << "--decimateLevel n\n";
      cerr << "--help\n";
      cerr << "\n";
    }
    else
    {
      cerr << "Unrecognised argument: use --help for help\n";
      return 1;
    }
  }
  
  if (in == 0)
  {
    cerr << "Must specify --in filename\n";
    return 1;
  }
    
  // create the renderer stuff
  vtkRenderer *aRenderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(aRenderer);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  
  // read the obj file
  vtkOBJReader *obj = vtkOBJReader::New();
  obj->SetFileName(in);

  // make sure the mesh is triangular
  vtkTriangleFilter *objTriangle = vtkTriangleFilter::New();
  objTriangle->SetInput(obj->GetOutput());
  if (debugFlag) objTriangle->DebugOn(); 
  
  // make sure we have surfaces rather than isolated triangles
  vtkCleanPolyData *objCleaner = vtkCleanPolyData::New();
  objCleaner->SetInput(objTriangle->GetOutput());
   if (debugFlag) objCleaner->DebugOn(); 
  
  // decimate the mesh
  vtkDecimatePro *objDecimator = vtkDecimatePro::New();
  objDecimator->SetInput(objCleaner->GetOutput());
  objDecimator->SetTargetReduction(decimateLevel);
  objDecimator->PreserveTopologyOff();
  if (debugFlag) objDecimator->DebugOn(); 
     
  // smoothing might be useful
  vtkSmoothPolyDataFilter *objSmoother = vtkSmoothPolyDataFilter::New();
  objSmoother->SetInput(objDecimator->GetOutput());
  objSmoother->SetRelaxationFactor(0.01);
  objSmoother->SetNumberOfIterations(20);
  if (debugFlag) objSmoother->DebugOn(); 
                   
  // convert polygons to strips
  // note - doesn't preserve counter-clockwise vertex numbering
  //vtkStripper *objStripper = vtkStripper::New();
  // smoothing optional
  //if (smoothFlag) objStripper->SetInput(objSmoother->GetOutput());
  //else objStripper->SetInput(objDecimator->GetOutput());
  //if (debugFlag) objStripper->DebugOn();
  
  vtkPolyDataMapper *objMapper = vtkPolyDataMapper::New();
  // smoothing optional
  if (smoothFlag) objMapper->SetInput(objSmoother->GetOutput());
  else objMapper->SetInput(objDecimator->GetOutput());
  objMapper->ScalarVisibilityOff();
  
  vtkActor *objActor = vtkActor::New();
  objActor->SetMapper(objMapper);
  objActor->GetProperty()->SetDiffuseColor(1, 1, .9412);

  // get an outline
  vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
  outlineData->SetInput(obj->GetOutput());
  
  vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
  mapOutline->SetInput(outlineData->GetOutput());
  
  vtkActor *outline = vtkActor::New();
  outline->SetMapper(mapOutline);
  outline->GetProperty()->SetColor(0,0,0);

  // create a camera with the correct view up
  vtkCamera *aCamera = vtkCamera::New();
  aCamera->SetViewUp (0, 0, -1);
  aCamera->SetPosition (0, 1, 0);
  aCamera->SetFocalPoint (0, 0, 0);

  // now, tell the renderer our actors
  aRenderer->AddActor(outline);
  aRenderer->AddActor(objActor);

  aRenderer->SetActiveCamera(aCamera);
  aRenderer->ResetCamera ();
  aCamera->Dolly(1.5);
  aRenderer->SetBackground(1,1,1);
  aRenderer->ResetCameraClippingRange();
    
  // interact with data
  renWin->SetSize(512, 512);
  renWin->Render();
  
  if (out)
  {
    // glue to get rid of .obj at the end of the filename
    // otherwise it gets put in twice
    int l = strlen(out);
    if (strcmp(out + l - 4, ".obj") == 0) out[l - 4] = 0; 
      
    // export to OBJ
    vtkOBJExporter *objExport = vtkOBJExporter::New();
    if (debugFlag) objExport->DebugOn();
    objExport->SetInput(renWin);
    objExport->SetFilePrefix(out);
    objExport->Write();
  }
  
  iren->Start(); 
  
  return 0;
}
