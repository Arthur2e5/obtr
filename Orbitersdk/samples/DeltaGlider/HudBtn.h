// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// HudBtn.h
// HUD mode selector button interface
// ==============================================================

#ifndef __HUDBTN_H
#define __HUDBTN_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================
// HUD mode selector buttons
class HUDButton: public PanelElement {
public:
	HUDButton (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
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
class HUDColourButton: public PanelElement {
public:
	HUDColourButton (VESSEL3 *v);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	DWORD pending_action;
};

#endif // !__HUDBTN_H