// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// SwitchArray.h
// User interface for row of switches on main panel
// ==============================================================

#ifndef __SWITCHARRAY_H
#define __SWITCHARRAY_H

#include "DGSwitches.h"

// ==============================================================

class SwitchArray: public PanelElement {
public:
	SwitchArray (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);

private:
	int btnstate[8]; // 0=up, 1=down
};

#endif // !__SWITCHARRAY_H