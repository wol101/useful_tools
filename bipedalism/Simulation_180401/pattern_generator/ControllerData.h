// ControllerData.h - block to contain the controller data

#ifndef CONTROLLERDATA_H
#define CONTROLLERDATA_H

class ControllerData
{
  public:
  
  ControllerData();
  ~ControllerData();
    
  void SetSize(int size);
  int GetSize() { return m_Size; };    
  
  double *m_LeftHipExtensorController;
  double *m_RightHipExtensorController;
  double *m_LeftHipFlexorController;
  double *m_RightHipFlexorController;
  double *m_LeftKneeExtensorController;
  double *m_RightKneeExtensorController;
  double *m_LeftKneeFlexorController;
  double *m_RightKneeFlexorController;
  double *m_LeftAnkleExtensorController;
  double *m_RightAnkleExtensorController;
  double *m_LeftAnkleFlexorController;
  double *m_RightAnkleFlexorController;

protected:
    
  int m_Size;

};

#endif // CONTROLLERDATA_H
