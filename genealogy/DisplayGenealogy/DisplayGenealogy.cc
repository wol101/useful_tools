// program to read in the genealogy file and produce a tree
// representing the genealogy

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <string.h>
#include <vector>
#include <math.h>

#include <glut.h>

#include "ParameterFile.h"
#include "Cohort.h"
#include "Axis.h"
#include "write_dxf.h"

#define gDebugDeclare
#include "Debug.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ODD(n) ((n) & 1)

static void ParseGenealogyFile(const char *inputFileName, 
    std::vector<Cohort *> &cohortList);
static void DrawTreeForwards(std::vector<Cohort *> &cohortList, 
      std::vector<int> &rankList);
static void DrawBranchForwards(std::vector<Cohort *> &cohortList, 
    int currentRank, int currentGeneration);
static void DrawTreeBackwards(std::vector<Cohort *> &cohortList, 
      std::vector<int> &rankList);
static void DrawBranchBackwards(std::vector<Cohort *> &cohortList, 
    int currentRank, int currentGeneration);

static void ExtractRange(const char *in, int *start, int *end);

static void StartOpenGLLoop();           // start the OpenGL loop
static void Display(void);        // the OpenGL display loop
static void Reshape(int w, int h);      // reshape the OpenGL view
static void ProcessKeyboard(unsigned char key, int, int); // the OpenGL keyboard loop
static void ProcessSpecialKeys(int key, int, int);  // the OpenGL special key loop
static void Idle(void);           // called by Open GL idle routine

static double g_Height = 1;
static double g_Width = 1;
static double g_XScale;
static double g_YScale;

static std::vector<int> g_TerminationRank;

struct LineSegment
{
  double x1;
  double y1;
  double x2;
  double y2;
  double red;
  double green;
  double blue;
};

std::vector<LineSegment> g_LineSegmentList;

int g_NGenerations;
int g_PopulationSize;
    
int main(int argc, char *argv[])
{
  int i, j;
  char *inFileName = 0;
  int windowWidth = 512;
  int windowHeight = 512;
  char *DXFFile = 0;
  std::vector<int> rankList;
  bool forwardsFlag = false;
  bool notFirstFlag;
  int maxGeneration = -1;
        
  // pass arguments to OpenGL
  glutInit(&argc, argv);
    
  // parse the command line
  try
  {
    for (i = 1; i < argc; i++)
    {
      // do something with arguments

      if (strcmp(argv[i], "--input") == 0 ||
            strcmp(argv[i], "-i") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        inFileName = argv[i];
      }

      else
      if (strcmp(argv[i], "--windowHeight") == 0||
               strcmp(argv[i], "-h") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        sscanf(argv[i], "%d", &windowHeight);
      }
      
      else
      if (strcmp(argv[i], "--windowWidth") == 0 ||
               strcmp(argv[i], "-w") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        sscanf(argv[i], "%d", &windowWidth);
      }
      
      else
      if (strcmp(argv[i], "--maxGeneration") == 0 ||
               strcmp(argv[i], "-g") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        sscanf(argv[i], "%d", &maxGeneration);
      }
      
      else
      if (strcmp(argv[i], "--rank") == 0 ||
               strcmp(argv[i], "-r") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        int start, end;
        ExtractRange(argv[i], &start, &end);
        for (int j = start; j <= end; j++) rankList.push_back(j);
      }
      
      else
      if (strcmp(argv[i], "--DXFFile") == 0 ||
               strcmp(argv[i], "-d") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        DXFFile = argv[i];
      }

      else
      if (strcmp(argv[i], "--forwards") == 0 ||
               strcmp(argv[i], "-f") == 0)
      {
        forwardsFlag = true;
      }

      else
      if (strcmp(argv[i], "--debug") == 0 ||
               strcmp(argv[i], "-e") == 0)
      {
        i++;
        if (i >= argc) throw(1);
        gDebug = atoi(argv[i]);
      }
      
      else
       {
          throw(1);
      }
    }
    
    if (inFileName == 0) throw(2);
  }
  
  // catch argument errors
  catch (...)
  {
    cerr << "Argument Error\n";
    cerr << "(-i) --input inputFileName\n";
    cerr << "(-h) --windowHeight y\n";
    cerr << "(-w) --windowWidth x\n";
    cerr << "(-r) --rank n\n";
    cerr << "(-g) --maxGeneration n\n";
    cerr << "(-d) --DXFFile outputFileName\n";
    cerr << "(-f) --forwards\n";
    cerr << "(-e) --debug n\n";
    exit(1);
  }
  
  // create the required window
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Display Genealogy");
  
  // Set the clear color to black
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  // Set the shading model
  glShadeModel(GL_FLAT);  
        

  std::vector<Cohort *> cohortList;
  ParseGenealogyFile(inFileName, cohortList);
  
  if (maxGeneration > 0 && maxGeneration + 1 <= (int)cohortList.size())
     g_NGenerations = maxGeneration + 1;
  else
     g_NGenerations = (int)cohortList.size();
  g_PopulationSize = cohortList[0]->GetSize();
  
  // calculate scales
  g_XScale = g_Width / (double)g_PopulationSize;
  g_YScale = g_Height / (double)g_NGenerations;

  if (forwardsFlag)
     DrawTreeForwards(cohortList, rankList);
  else
     DrawTreeBackwards(cohortList, rankList);
  
  for (i = 0; i < (int)g_TerminationRank.size(); i++)
  {
     notFirstFlag = false;
     for (j = 0; j < i; j++)
     {
        if (g_TerminationRank[i] == g_TerminationRank[j]) 
        {
           notFirstFlag = true;
           break;
        }
     }
     if (notFirstFlag == false) 
        cout << "Termination Rank: " << g_TerminationRank[i] << "\n";
  }
  cout.flush();
  
  if (DXFFile)
  {
    ofstream dxf(DXFFile);
  
    // since we know the extents of the drawing lets specify them
    generate_header(dxf, 0, 0, g_Width, g_Height); 

    // outline drawing area
    write_line(dxf, 0, 0, g_Width, 0);
    write_line(dxf, g_Width, 0, g_Width, g_Height);
    write_line(dxf, g_Width, g_Height, 0, g_Height);
    write_line(dxf, 0, g_Height, 0, 0);
    
    for (i = 0; i < (int)g_LineSegmentList.size(); i++)
    {
      write_line(dxf, g_LineSegmentList[i].x1, g_LineSegmentList[i].y1,
        g_LineSegmentList[i].x2, g_LineSegmentList[i].y2);
    }

    // finish basic DXF file
    finish_entities(dxf);
    generate_eof(dxf);
    dxf.close();
  }
  StartOpenGLLoop();
    
  return 0;
}

// routine to read in the genealogy file
void ParseGenealogyFile(const char *inputFileName, 
    std::vector<Cohort *> &cohortList)
{
  ParameterFile input;
  int i, j;
  GenomeInfo *info;
  Cohort *cohort;
      
  if (gDebug == Debug_DisplayGenealogy)
     cerr << "ParseGenealogyFile:\tinputFileName\t" << inputFileName << "\n";
  input.ReadFile(inputFileName);
  int tokensPerLine = input.CountTokensToEOL();
  if (tokensPerLine == 0)
  {
    cerr << "Zero length first line\n";
    exit(1);
  }
  int tokensPerFile = input.CountTokensToEOF();
  if (gDebug == Debug_DisplayGenealogy)
  {
    cerr << "ParseGenealogyFile:\ttokensPerLine\t" << tokensPerLine << "\n";
    cerr << "ParseGenealogyFile:\ttokensPerFile\t" << tokensPerFile << "\n";
  }
  int nGenerations = tokensPerFile / tokensPerLine;
  int populationSize = (tokensPerLine - 1) / 5;
  if ((tokensPerLine - 1) % 5 != 0)
  {
    cerr << "Line length error\n";
    exit(1);
  }
  
  int generationNumber;
  for (i = 0; i < nGenerations; i++)
  {
    cohort = new Cohort();
    cohort->SetSize(populationSize);
    input.ReadNext(&generationNumber);
    if (generationNumber != i)
    {
      cerr << "Generation number error\n";
      exit(1);
    }
    for (j = 0; j < populationSize; j++)
    {
      info = cohort->GetGenomeInfoPtr(j);
      input.ReadNext(&info->parent1.generationNumber);
      input.ReadNext(&info->parent1.rank);
      input.ReadNext(&info->parent2.generationNumber);
      input.ReadNext(&info->parent2.rank);
      input.ReadNext(&info->fitness);
    }
    cohortList.push_back(cohort);
  }
  if (gDebug == Debug_DisplayGenealogy)
      cerr << "ParseGenealogyFile:\tpopulationSize\t" << populationSize << 
           "\tnGenerations\t" << nGenerations << "\n";
}   
 
// draw tree forwards entry point 
// start from the rank list of individuals from the last generation
// no ranks, use the best individual
void DrawTreeForwards(std::vector<Cohort *> &cohortList, 
      std::vector<int> &rankList)
{
  if (rankList.size() == 0) 
  {
     cout << "Using rank " << g_PopulationSize - 1 << "\n";
     cout.flush();
     rankList.push_back(g_PopulationSize - 1);
  }
  
  for (int i = 0; i < (int)rankList.size(); i++)
     DrawBranchForwards(cohortList, rankList[i], 0);
}

// recursive tree drawing function
void DrawBranchForwards(std::vector<Cohort *> &cohortList, 
    int currentRank, int currentGeneration)
{
  GenomeInfo *info;
  GenomeInfo *info2;
  static LineSegment line;
  
  //static int count = 0;
  //count++;
  //if (count > 100) return;
  if (currentGeneration >= g_NGenerations - 1) 
  {
     g_TerminationRank.push_back(currentRank);
     return;
  }
  
  if (gDebug == Debug_DisplayGenealogy)
        cerr << "DrawBranchForwards:\tcurrentRank\t" << currentRank << 
        "\tcurrentGeneration\t" << currentGeneration << "\n";
  
  info2 = cohortList[currentGeneration]->GetGenomeInfoPtr(currentRank);
  for (int i = 0; i < g_PopulationSize; i++)
  {
     info = cohortList[currentGeneration + 1]->GetGenomeInfoPtr(i);
     if ((info->parent1.rank == currentRank && 
           info->parent1.generationNumber == currentGeneration) 
           || (info->parent2.rank == currentRank &&
           info->parent2.generationNumber == currentGeneration))
     {
        line.x1 = currentRank * g_XScale;
        line.y1 = currentGeneration * g_YScale;
        line.x2 = i * g_XScale;
        line.y2 = (currentGeneration + 1) * g_YScale;
        line.red = 0.0;
        line.green = 1.0;
        line.blue = 0.0;
        g_LineSegmentList.push_back(line);
        
        DrawBranchForwards(cohortList,
          i, currentGeneration + 1); 
     }
     else
     {
        if (info->fitness == info2->fitness && 
              info->parent1.rank == info2->parent1.rank &&
              info->parent1.generationNumber == info2->parent1.generationNumber &&
              info->parent2.rank == info2->parent2.rank &&
              info->parent2.generationNumber == info2->parent2.generationNumber)
        {
           line.x1 = currentRank * g_XScale;
           line.y1 = currentGeneration * g_YScale;
           line.x2 = i * g_XScale;
           line.y2 = (currentGeneration + 1) * g_YScale;
           line.red = 0.0;
           line.green = 0.0;
           line.blue = 1.0;
            g_LineSegmentList.push_back(line);

           DrawBranchForwards(cohortList,
             i, currentGeneration + 1); 
        }
     }
  }
}  

// draw tree backwards entry point 
// start from the rank list of individuals from the last generation
// no ranks, use the best individual
static void DrawTreeBackwards(std::vector<Cohort *> &cohortList, std::vector<int> &rankList)
{
  if (rankList.size() == 0) 
  {
     cout << "Using rank " << g_PopulationSize - 1 << "\n";
     cout.flush();
     rankList.push_back(g_PopulationSize - 1);
  }
     
  for (int i = 0; i < (int)rankList.size(); i++)
     DrawBranchBackwards(cohortList, rankList[i], g_NGenerations - 1);
}

// recursive tree drawing function
void DrawBranchBackwards(std::vector<Cohort *> &cohortList, 
    int currentRank, int currentGeneration)
{
  GenomeInfo *info;
  static LineSegment line;
  int i;
  
  if (currentGeneration <= 0) 
  {
     if (gDebug == Debug_DisplayGenealogy)
         cerr << "DrawBranchBackwards:\tcurrentRank" << currentRank << "\n";
     g_TerminationRank.push_back(currentRank);
     return;
  }
      
  info = cohortList[currentGeneration]->GetGenomeInfoPtr(currentRank);
  
  // check for end of branch 1
  if (info->parent1.rank == -1)
  {
     for (i = g_PopulationSize - 1; i >= 0; i--)
     {
        if (info->fitness == cohortList[0]->GetGenomeInfoPtr(i)->fitness)
        {
           if (gDebug == Debug_DisplayGenealogy)
               cerr << "DrawBranchBackwards:\tbranch1\tcurrentRank\t" << currentRank << 
               "\tcurrentGeneration\t" << currentGeneration << 
               "\ti\t" << i << "\n";
           g_TerminationRank.push_back(i);
           // draw branch 1
           line.x1 = currentRank * g_XScale;
           line.y1 = currentGeneration * g_YScale;
           line.x2 = i * g_XScale;
           line.y2 = 0 * g_YScale;
           line.red = 0.0;
           line.green = 1.0;
           line.blue = 0.0;
           g_LineSegmentList.push_back(line);
           break;
        }
     }
  }
  else
  {
    if (gDebug == Debug_DisplayGenealogy)
        cerr << "DrawBranchBackwards:\tbranch1\tcurrentRank\t" << currentRank << 
        "\tcurrentGeneration\t" << currentGeneration << 
        "\tparent1.rank\t" << info->parent1.rank << 
        "\tparent1.generationNumber\t" << info->parent1.generationNumber << "\n";

    // draw branch 1
    line.x1 = currentRank * g_XScale;
    line.y1 = currentGeneration * g_YScale;
    line.x2 = info->parent1.rank * g_XScale;
    line.y2 = info->parent1.generationNumber * g_YScale;
    line.red = 0.0;
    line.green = 1.0;
    line.blue = 0.0;
    g_LineSegmentList.push_back(line);
  
    DrawBranchBackwards(cohortList,
      info->parent1.rank, info->parent1.generationNumber); 
  }
  
  // check for end of branch 2
  if (info->parent2.rank != -1)
  {
    if (gDebug == Debug_DisplayGenealogy)
        cerr << "DrawBranchBackwards:\tbranch1\tcurrentRank\t" << currentRank << 
        "\tcurrentGeneration\t" << currentGeneration << 
        "\tparent2.rank\t" << info->parent2.rank << 
        "\tparent2.generationNumber\t" << info->parent2.generationNumber << "\n";
     
    // draw branch 2
    line.x1 = currentRank * g_XScale;
    line.y1 = currentGeneration * g_YScale;
    line.x2 = info->parent2.rank * g_XScale;
    line.y2 = info->parent2.generationNumber * g_YScale;
    line.red = 0.0;
    line.green = 0.0;
    line.blue = 1.0;
    g_LineSegmentList.push_back(line);
  
    DrawBranchBackwards(cohortList,
      info->parent2.rank, info->parent2.generationNumber); 
  }
}  

// parse command line range in the form start-end
static void ExtractRange(const char *in, int *start, int *end)
{
   char buffer[256];
   char *ptr;
   
   strcpy(buffer, in);
   ptr = strstr(buffer, "-");
   if (ptr == 0) // no range found
   {
      *start = atoi(buffer);
      *end = *start;
      return;
   }
   
   *ptr = 0;
   ptr++;
   
   *start = atoi(buffer); 
   *end = atoi(ptr);
   return;
}

//----------------------------------------------------------------------------
void StartOpenGLLoop()
{

  glutReshapeFunc(Reshape);
  glutKeyboardFunc(ProcessKeyboard);
  glutSpecialFunc(ProcessSpecialKeys);
  glutDisplayFunc(Display);
  glutIdleFunc(Idle);

  glutMainLoop();

}

//----------------------------------------------------------------------------
void Display(void)
{
  unsigned int i;
  unsigned int nSegs = g_LineSegmentList.size();
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(1.0, 0.0, 0.0);
  Rect theRect = {0.0, g_Width, 0.0, g_Height};
  Axis::Draw(theRect, 0, (double)g_PopulationSize, 
    bottom, g_Width / 100, g_Width / 50, g_Width / 40, "Rank");
  Axis::Draw(theRect, 0, (double)g_PopulationSize, 
    top, g_Width / 100, g_Width / 50, g_Width / 40, 0, false);
  Axis::Draw(theRect, 0, (double)g_NGenerations, 
    left, g_Height / 100, g_Height / 50, g_Height / 40, "Generation");
  Axis::Draw(theRect, 0, (double)g_NGenerations, 
    right, g_Height / 100, g_Height / 50, g_Height / 40, 0, false);
    
  for (i = 0; i < nSegs; i++)
  {
    glBegin(GL_LINES);
    glColor3f(g_LineSegmentList[i].red, g_LineSegmentList[i].green, g_LineSegmentList[i].blue);
    glVertex2f(g_LineSegmentList[i].x1, g_LineSegmentList[i].y1);
    glVertex2f(g_LineSegmentList[i].x2, g_LineSegmentList[i].y2);
    glEnd();
  }

  glFlush();
}

//----------------------------------------------------------------------------
void Reshape(int width, int height)
{
  // Set the new viewport size
  glViewport(0, 0, (GLint)width, (GLint)height);

  // Choose the projection matrix to be the matrix 
  // manipulated by the following calls
  glMatrixMode(GL_PROJECTION);

  // Set the projection matrix to be the identity matrix
  glLoadIdentity();

  // Define the dimensions of the Orthographic Viewing Volume
  double vertMargin = 0.15 * g_Height;
  double horizMargin = 0.15 * g_Width;
  glOrtho(-horizMargin, g_Width + horizMargin, 
      -vertMargin, g_Height + vertMargin, 
      -1.0, 1.0);

  // Choose the modelview matrix to be the matrix
  // manipulated by further calls
  glMatrixMode(GL_MODELVIEW);

  // Clear the window
  glClear(GL_COLOR_BUFFER_BIT);
}

//----------------------------------------------------------------------------
void ProcessKeyboard(unsigned char key, int, int)
{
  switch (key)
  {
  case 27:
    glutDestroyWindow(glutGetWindow());
    exit(1);
    break;

  }
}


//----------------------------------------------------------------------------
void ProcessSpecialKeys(int key, int, int)
{
  switch (key)
  {
  case GLUT_KEY_LEFT:
    break;
  case GLUT_KEY_RIGHT:
    break;
  case GLUT_KEY_UP:
    break;
  case GLUT_KEY_DOWN:
    break;
  }
}


//----------------------------------------------------------------------------
void Idle(void)
{
}


