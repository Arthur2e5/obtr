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
class RcsProgButtons;

class RcsSubsystem: public DGSubsystem {
public:
	RcsSubsystem (DeltaGlider *dg, int ident);
	void SetMode (int mode);
	void SetProg (int prog, bool active);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);

private:
	RcsModeDial *modedial;
	RcsProgButtons *progbuttons;
	int ELID_MODEDIAL;
	int ELID_PROGBUTTONS;
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


// ==============================================================
// RCS program buttons
// ==============================================================

class RcsProgButtons: public PanelElement {
public:
	RcsProgButtons (RcsSubsystem *_subsys);
	~RcsProgButtons ();
	void SetMode (int mode, bool active);
	void DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
		DWORD vofs[6], DWORD vofs_label[6]);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	RcsSubsystem *subsys;
	DGButton3 *btn[6]; // the list of navmode buttons
};

#endif // !__RCSSUBSYS_H