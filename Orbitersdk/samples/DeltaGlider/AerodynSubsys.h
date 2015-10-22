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
class Airbrake;

class AerodynCtrlSubsystem: public DGSubsystem {
public:
	AerodynCtrlSubsystem (DeltaGlider *v, int ident);
	~AerodynCtrlSubsystem ();
	void SetMode (DWORD mode);
	void ActivateAirbrake (DeltaGlider::DoorStatus action, bool half_step = false);
	DeltaGlider::DoorStatus AirbrakeStatus () const;
	const double *AirbrakePositionPtr () const;

private:
	AerodynSelector *selector;
	Airbrake *airbrake;
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

// ==============================================================
// Airbrake
// ==============================================================

class Airbrake: public DGSubsystemComponent {
	friend class AirbrakeLever;

public:
	Airbrake (AerodynCtrlSubsystem *_subsys);
	void Activate (DeltaGlider::DoorStatus action, bool half_step = false);
	DeltaGlider::DoorStatus Status() const;
	inline const double *PositionPtr() const { return &brake_proc; }
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	void clbkPostCreation ();

private:
	AirbrakeLever *lever;
	int ELID_LEVER;

	DeltaGlider::DoorStatus brake_status, airbrakelever_status;
	int airbrake_tgt;

	UINT anim_brake;            // handle for airbrake animation
	UINT anim_airbrakelever;    // VC airbrake lever
	double brake_proc, airbrakelever_proc;
};

// ==============================================================

class AirbrakeLever: public PanelElement {
public:
	AirbrakeLever (Airbrake *comp);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	Airbrake *component;
	int state;
};

#endif // ___AERODYNSUBSYS_H