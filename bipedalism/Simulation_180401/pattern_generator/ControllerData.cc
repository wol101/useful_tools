// ControllerData.cc - block to contain the controller data

#include "ControllerData.h"

// Constructor - set default values
ControllerData::ControllerData()
{
  m_LeftHipExtensorController = 0;
  m_RightHipExtensorController = 0;
  m_LeftHipFlexorController = 0;
  m_RightHipFlexorController = 0;
  m_LeftKneeExtensorController = 0;
  m_RightKneeExtensorController = 0;
  m_LeftKneeFlexorController = 0;
  m_RightKneeFlexorController = 0;
  m_LeftAnkleExtensorController = 0;
  m_RightAnkleExtensorController = 0;
  m_LeftAnkleFlexorController = 0;
  m_RightAnkleFlexorController = 0;

  m_Size = 0;
}

// Destructor - deallocate any allocated memory
ControllerData::~ControllerData()
{
  if (m_Size)
  {
    delete [] m_LeftHipExtensorController;
    delete [] m_RightHipExtensorController;
    delete [] m_LeftHipFlexorController;
    delete [] m_RightHipFlexorController;
    delete [] m_LeftKneeExtensorController;
    delete [] m_RightKneeExtensorController;
    delete [] m_LeftKneeFlexorController;
    delete [] m_RightKneeFlexorController;
    delete [] m_LeftAnkleExtensorController;
    delete [] m_RightAnkleExtensorController;
    delete [] m_LeftAnkleFlexorController;
    delete [] m_RightAnkleFlexorController;
  }
}

// Set the storage size
void ControllerData::SetSize(int size)
{
  if (size == m_Size) return;
  
  if (m_Size)
  {
    delete [] m_LeftHipExtensorController;
    delete [] m_RightHipExtensorController;
    delete [] m_LeftHipFlexorController;
    delete [] m_RightHipFlexorController;
    delete [] m_LeftKneeExtensorController;
    delete [] m_RightKneeExtensorController;
    delete [] m_LeftKneeFlexorController;
    delete [] m_RightKneeFlexorController;
    delete [] m_LeftAnkleExtensorController;
    delete [] m_RightAnkleExtensorController;
    delete [] m_LeftAnkleFlexorController;
    delete [] m_RightAnkleFlexorController;
  }
  
  m_Size = size;
  
  m_LeftHipExtensorController = new double[m_Size];
  m_RightHipExtensorController = new double[m_Size];
  m_LeftHipFlexorController = new double[m_Size];
  m_RightHipFlexorController = new double[m_Size];
  m_LeftKneeExtensorController = new double[m_Size];
  m_RightKneeExtensorController = new double[m_Size];
  m_LeftKneeFlexorController = new double[m_Size];
  m_RightKneeFlexorController = new double[m_Size];
  m_LeftAnkleExtensorController = new double[m_Size];
  m_RightAnkleExtensorController = new double[m_Size];
  m_LeftAnkleFlexorController = new double[m_Size];
  m_RightAnkleFlexorController = new double[m_Size];
    
}

