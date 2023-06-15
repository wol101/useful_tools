// ForceList object - maintains an independent list of drawable forces

#ifndef __FORCELIST_H__
#define __FORCELIST_H__

#include <vector>
class dmForce;

class ForceList 
{
  public:
    ForceList();
    ~ForceList();

    void AddForce(dmForce * force);
    dmForce *FindByName(const char * const name);
    void draw(void);
    int GetSize(void) { return forceList->size(); };
    dmForce *GetEntry(int i) { return (*forceList)[i]; };

  protected:
    std::vector <dmForce *> *forceList;
};



#endif // __FORCELIST_H__
