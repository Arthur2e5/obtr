// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// RcsDial.h
// RCS selector dial user interface
// ==============================================================

#ifndef __RCSDIAL_H
#define __RCSDIAL_H

#include "DGSwitches.h"

// ==============================================================

class RCSDial: public DGDial1 {
public:
	RCSDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__RCSDIAL_H