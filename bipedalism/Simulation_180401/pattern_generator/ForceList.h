// ForceList object - maintains an independent list of drawable forces

#ifndef __FORCELIST_H__
#define __FORCELIST_H__

#include <vector>
class LoggedForce;

class ForceList 
{
  public:
    ForceList();
    ~ForceList();

    void AddForce(LoggedForce * force);
    LoggedForce *FindByName(const char * const name);
    void draw(void);
    int GetSize(void) { return forceList->size(); };
    LoggedForce *GetEntry(int i) { return (*forceList)[i]; };

  protected:
    std::vector <LoggedForce *> *forceList;
};



#endif // __FORCELIST_H__
