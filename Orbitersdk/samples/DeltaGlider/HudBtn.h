// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HudBtn.h
// HUD mode selector button interface
// ==============================================================

#ifndef __HUDBTN_H
#define __HUDBTN_H

#include "DGSwitches.h"

// ==============================================================
// HUD mode selector buttons
class HUDModeButtons: public PanelElement {
public:
	HUDModeButtons (VESSEL3 *v);
	~HUDModeButtons ();
	void DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
		DWORD vofs[3], DWORD vofs_label[3]);
	void SetMode (int mode);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	DGButton3 *btn[3];
	int vmode; // currently displayed HUD mode
};

// ==============================================================
// HUD brightness dial
class HUDBrightnessDial: public PanelElement {
public:
	HUDBrightnessDial (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// HUD colour selector button
class HUDColourButton: public DGButton2 {
public:
	HUDColourButton (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// HUD retract/extend switch
class HUDUpDownSwitch: public DGSwitch1 {
public:
	HUDUpDownSwitch (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__HUDBTN_H