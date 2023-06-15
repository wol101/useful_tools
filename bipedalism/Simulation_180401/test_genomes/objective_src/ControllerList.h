// ControllerList object - maintains an independent list of drawable controllers

#ifndef __CONTROLLERLIST_H__
#define __CONTROLLERLIST_H__

class Controller;
#include <vector>

class ControllerList { public:
	ControllerList();
	~ControllerList();

	void AddController(Controller * controller);
	Controller *FindByName(const char * const name);
	int GetSize(void) {
		return controllerList->size();
	};
	Controller *GetEntry(int i) {
		return (*controllerList)[i];
	};

      protected:

	 std::vector < Controller * >*controllerList;
};



#endif						 // __CONTROLLERLIST_H__
