// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// NavButton.h
// Navigation mode button user interface
// ==============================================================

#ifndef __NAVBUTTON_H
#define __NAVBUTTON_H

#include "DGSwitches.h"

// ==============================================================
// Container for 6 "Navmode" buttons: Kill-Rot, Prograde, Retrograde,
// Normal, Antinormal, Horizon-level

class NavButtons: public PanelElement {
public:
	NavButtons (VESSEL3 *v);
	~NavButtons ();
	void DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
		DWORD vofs[6], DWORD vofs_label[6]);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
	void SetMode (int mode, bool active);

private:
	DGButton3 *btn[6]; // the list of navmode buttons
};

#endif // !__NAVBUTTON_H