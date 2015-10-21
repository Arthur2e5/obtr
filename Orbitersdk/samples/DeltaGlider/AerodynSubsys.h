// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// AerodynSubsys.h
// Subsystem for aerodynamic controls (selector dial, elevator
// trim, airbrake)
// ==============================================================

#ifndef __AERODYNSUBSYS_H
#define __AERODYNSUBSYS_H

#include "DGSwitches.h"
#include "DGSubsys.h"

// ==============================================================
// Aerodynamic control subsystem
// ==============================================================

class AerodynSelector;

class AerodynCtrlSubsystem: public DGSubsystem {
public:
	AerodynCtrlSubsystem (DeltaGlider *v, int ident);
	~AerodynCtrlSubsystem ();
	void SetMode (DWORD mode);
private:
	AerodynSelector *selector;
};

// ==============================================================
// Control selector dial
// ==============================================================

class AerodynSelector: public DGSubsystemComponent {
	friend class AerodynSelectorDial;

public:
	AerodynSelector (AerodynCtrlSubsystem *_subsys);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);
	void SetMode (DWORD mode);

protected:
	bool IncMode();
	bool DecMode();

private:
	AerodynSelectorDial *dial;
	int ELID_DIAL;
};

// ==============================================================

class AerodynSelectorDial: public DGDial1 {
public:
	AerodynSelectorDial (AerodynSelector *comp);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	AerodynSelector *component;
};

#endif // ___AERODYNSUBSYS_H