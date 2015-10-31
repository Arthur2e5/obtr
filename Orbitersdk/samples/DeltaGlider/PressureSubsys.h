// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// PressureSubsys.h
// Cabin and airlock pressure control subsystem
// ==============================================================

#ifndef __PRESSURESUBSYS_H
#define __PRESSURESUBSYS_H

#include "DeltaGlider.h"
#include "DGSubsys.h"
#include "DGSwitches.h"

// ==============================================================

class AirlockCtrl;
class TophatchCtrl;
class PValveSwitch;
class PressureIndicator;

// ==============================================================
// Pressure control module
// ==============================================================

class PressureSubsystem: public DGSubsystem {
public:
	PressureSubsystem (DeltaGlider *vessel, int ident);
	~PressureSubsystem ();
	inline double PCabin() const { return p_cabin; }
	inline double PAirlock() const { return p_airlock; }
	inline double PExtHatch() const { return p_ext_hatch; }
	inline double PExtLock() const { return p_ext_lock; }
	inline int GetPValve (int i) const { return valve_status[i]; }
	inline void SetPValve (int i, int status) { valve_status[i] = status; }
	DeltaGlider::DoorStatus OLockStatus () const;
	DeltaGlider::DoorStatus ILockStatus () const;
	void ActivateOuterAirlock (DeltaGlider::DoorStatus action);
	void ActivateInnerAirlock (DeltaGlider::DoorStatus action);
	void RevertOuterAirlock ();
	void ActivateHatch (DeltaGlider::DoorStatus action);
	DeltaGlider::DoorStatus HatchStatus () const;
	void RepairDamage ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadVC (int vcid);     // create the VC elements for this module

private:
	bool docked;
	double p_cabin, p_airlock;      // current pressure in cabin and airlock [Pa]
	double p_ext_hatch, p_ext_lock; // current pressure outside hatch and airlock [Pa]
	static double v_cabin;          // cabin volume [m^3]
	static double v_airlock;        // airlock volume [m^3]
	double v_extdock;               // volume of compartment outside airlock
	static double p_target;         // target pressure in cabin and airlock when supply valves are open
	AirlockCtrl *airlockctrl;       // airlock controls
	TophatchCtrl *hatchctrl;        // top hatch controls
	PValveSwitch *valve_switch[5];  // the switches controlling supply and relief valves
	int valve_status[5];            // 0=closed, 1=open
	PressureIndicator *pind;

	// local panel element identifiers
	int ELID_PVALVESWITCH[5];
	int ELID_DISPLAY;
};

// ==============================================================
// Airlock controls
// ==============================================================

class AirlockCtrl: public DGSubsystemComponent {
	friend class PressureSubsystem;
	friend class OuterLockSwitch;
	friend class InnerLockSwitch;

public:
	AirlockCtrl (PressureSubsystem *_subsys);
	void ActivateOuterLock (DeltaGlider::DoorStatus action);
	void ActivateInnerLock (DeltaGlider::DoorStatus action);
	void RevertOuterLock ();
	void RevertInnerLock ();
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	void clbkPostCreation ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadVC (int vcid);

private:
	DeltaGlider::DoorStatus olock_status, ilock_status;
	double olock_proc, ilock_proc;

	OuterLockSwitch *osw;
	InnerLockSwitch *isw;

	int ELID_OSWITCH;
	int ELID_ISWITCH;

	UINT anim_olock;            // handle for outer airlock animation
	UINT anim_ilock;            // handle for inner airlock animation
};

// ==============================================================

class OuterLockSwitch: public DGSwitch1 {
public:
	OuterLockSwitch (AirlockCtrl *comp);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	AirlockCtrl *component;
};

// ==============================================================

class InnerLockSwitch: public DGSwitch1 {
public:
	InnerLockSwitch (AirlockCtrl *comp);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	AirlockCtrl *component;
};

// ==============================================================
// Top hatch controls
// ==============================================================

class TophatchCtrl: public DGSubsystemComponent {
	friend class PressureSubsystem;
	friend class HatchCtrlSwitch;

public:
	TophatchCtrl (PressureSubsystem *_subsys);
	~TophatchCtrl ();
	void Activate (DeltaGlider::DoorStatus action);
	void Revert ();
	void RepairDamage();
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	void clbkPostCreation ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadVC (int vcid);     // create the VC elements for this module

private:
	DeltaGlider::DoorStatus hatch_status;
	double hatch_proc;
	HatchCtrlSwitch *sw;
	int ELID_SWITCH;
	PSTREAM_HANDLE hatch_vent;
	double hatch_vent_t;
	UINT anim_hatch;            // handle for top hatch animation
	int hatchfail;
};

// ==============================================================

class HatchCtrlSwitch: public DGSwitch1 {
public:
	HatchCtrlSwitch (TophatchCtrl *comp);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	TophatchCtrl *component;
};

// ==============================================================
// Switch for pressure valve operation

class PValveSwitch: public DGSwitch1 {
public:
	PValveSwitch (PressureSubsystem *_subsys, int id);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	PressureSubsystem *subsys;
	int vid; // valve identifier
};

// ==============================================================
// Pressure indicator display

class PressureIndicator: public PanelElement {
public:
	PressureIndicator (PressureSubsystem *_subsys, SURFHANDLE blitsrc);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	void ValStr (double p, char *cbuf);
	void BlitReadout (int which, const char *str);
	PressureSubsystem *subsys;
	double upt;
	SURFHANDLE bsrc, btgt;
	char label[4][8];
};

#endif // !___PRESSURESUBSYS_H