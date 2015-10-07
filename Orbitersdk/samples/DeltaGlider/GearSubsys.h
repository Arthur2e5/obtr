// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// GearSubsys.h
// Subsystem for landing gear control
// ==============================================================

#ifndef __GEARSUBSYS_H
#define __GEARSUBSYS_H

#include "DeltaGlider.h"
#include "DGSubsys.h"
#include "DGSwitches.h"
#include <vector>

// ==============================================================
// Landing gear subsystem
// ==============================================================

class GearControl;
class Wheelbrake;

class GearSubsystem: public DGSubsystem {
public:
	GearSubsystem (DeltaGlider *v, int ident);
	void ActivateGear (DeltaGlider::DoorStatus action);
	void RevertGear ();
	DeltaGlider::DoorStatus GearStatus() const;
	const double *GearPositionPtr() const;

private:
	GearControl *gearctrl;
	Wheelbrake *wheelbrake;
};

// ==============================================================
// Gear control: lever+indicator
// ==============================================================

class GearLever;
class GearIndicator;

class GearControl: public DGSubsystemComponent {
public:
	GearControl (GearSubsystem *_subsys);
	void ActivateGear (DeltaGlider::DoorStatus action);
	void RevertGear ();
	inline DeltaGlider::DoorStatus GearStatus() const { return gear_status; }
	inline const double *GearPositionPtr() const { return &gear_proc; }
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	void clbkPostCreation ();

private:
	DeltaGlider::DoorStatus gear_status, gearlever_status;
	double gear_proc, gearlever_proc;
	UINT anim_gear;             // handle for landing gear animation
	UINT anim_gearlever;        // VC gear lever

	GearLever *lever;
	GearIndicator *indicator;

	int ELID_LEVER;
	int ELID_INDICATOR;
};

// ==============================================================

class GearLever: public PanelElement {
public:
	GearLever (GearControl *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	GearControl *component;
};

// ==============================================================

class GearIndicator: public PanelElement {
public:
	GearIndicator (GearControl *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	GearControl *component;
	double tofs;
	bool light;
};

// ==============================================================
// Wheelbrake
// ==============================================================

class WheelbrakeLever;

class Wheelbrake: public DGSubsystemComponent {
public:
	Wheelbrake (GearSubsystem *_subsys);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);

private:
	WheelbrakeLever *lever;
	int ELID_LEVER;
};

// ==============================================================

class WheelbrakeLever: public PanelElement {
public:
	WheelbrakeLever (Wheelbrake *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);

private:
	Wheelbrake *component;
	bool isdown[2];
};

#endif // !__GEARSUBSYS_H