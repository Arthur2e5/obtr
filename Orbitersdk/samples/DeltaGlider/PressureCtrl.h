// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// PressureCtrl.h
// Cabin and airlock pressure control subsystem
// ==============================================================

#ifndef __PRESSURECTRL_H
#define __PRESSURECTRL_H

#include "DeltaGlider.h"
#include "DGSwitches.h"

// ==============================================================

class DeltaGlider;
class PValveSwitch;
class PressureIndicator;

// ==============================================================
// Pressure control module
// ==============================================================

class PressureControl: public DGSubSystem {
public:
	PressureControl (DeltaGlider *vessel, int ident);
	void clbkPostStep (double simt, double simdt, double mjd);
	inline double PCabin() const { return p_cabin; }
	inline double PAirlock() const { return p_airlock; }
	inline double PExtHatch() const { return p_ext_hatch; }
	inline double PExtLock() const { return p_ext_lock; }
	inline int GetPValve (int i) const { return valve_status[i]; }
	inline void SetPValve (int i, int status) { valve_status[i] = status; }
	
	bool clbkLoadVC (int id);       // create the VC elements for this module

private:
	bool docked;
	double p_cabin, p_airlock;      // current pressure in cabin and airlock [Pa]
	double p_ext_hatch, p_ext_lock; // current pressure outside hatch and airlock [Pa]
	static double v_cabin;          // cabin volume [m^3]
	static double v_airlock;        // airlock volume [m^3]
	double v_extdock;               // volume of compartment outside airlock
	static double p_target;         // target pressure in cabin and airlock when supply valves are open
	PValveSwitch *valve_switch[5];  // the switches controlling supply and relief valves
	int valve_status[5];            // 0=closed, 1=open
	PressureIndicator *pind;

	// local panel element identifiers
	int ELID_PVALVESWITCH[5];
	int ELID_DISPLAY;
};

// ==============================================================
// Switch for pressure valve operation

class PValveSwitch: public DGSwitch1 {
public:
	PValveSwitch (PressureControl *pc, int id);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	PressureControl *pctrl;
	int vid; // valve identifier
};

// ==============================================================
// Pressure indicator display

class PressureIndicator: public PanelElement {
public:
	PressureIndicator (PressureControl *pc, SURFHANDLE blitsrc);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	void ValStr (double p, char *cbuf);
	void BlitReadout (int which, const char *str);
	PressureControl *pctrl;
	double upt;
	SURFHANDLE bsrc, btgt;
	char label[4][8];
};

#endif // !___PRESSURECTRL_H