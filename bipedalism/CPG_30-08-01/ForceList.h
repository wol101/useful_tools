// ForceList object - maintains an independent list of drawable forces

#ifndef __FORCELIST_H__
#define __FORCELIST_H__

#include <vector>
class ExtendedForce;

class ForceList 
{
  public:
    ForceList();
    ~ForceList();

    void AddForce(ExtendedForce * force);
    ExtendedForce *FindByName(const char * const name);
    void draw(void);
    int GetSize(void) { return forceList->size(); };
    ExtendedForce *GetEntry(int i) { return (*forceList)[i]; };

  protected:
    std::vector <ExtendedForce *> *forceList;
};



#endif // __FORCELIST_H__
