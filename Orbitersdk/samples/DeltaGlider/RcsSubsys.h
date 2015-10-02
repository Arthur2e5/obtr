// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// RcsSubsys.h
// Reaction control subsystem: lin/rot selection, attitude programs
// ==============================================================

#ifndef __RCSSUBSYS_H
#define __RCSSUBSYS_H

#include "DGSwitches.h"
#include "DGSubsys.h"

// ==============================================================
// Reaction control subsystem
// ==============================================================

class RcsModeDial;

class RcsSubsystem: public DGSubsystem {
public:
	RcsSubsystem (DeltaGlider *dg, int ident);
	void SetMode (int mode);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);

private:
	RcsModeDial *modedial;
	int ELID_MODEDIAL;
};

// ==============================================================
// Mode dial
// ==============================================================

class RcsModeDial: public DGDial1 {
public:
	RcsModeDial (RcsSubsystem *_subsys);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	RcsSubsystem *subsys;
};

#endif // !__RCSSUBSYS_H