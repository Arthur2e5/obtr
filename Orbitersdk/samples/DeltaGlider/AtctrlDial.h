// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// AtctrlDial.h
// Atmospheric control selector dial user interface
// ==============================================================

#ifndef __ATCTRLDIAL_H
#define __ATCTRLDIAL_H

#include "DGSwitches.h"

// ==============================================================

class ATCtrlDial: public DGDial1 {
public:
	ATCtrlDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__ATCTRLDIAL_H