// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// CockpitLight.h
// Cockpit lighting switches
// ==============================================================

#ifndef __COCKPITLIGHT_H
#define __COCKPITLIGHT_H

#include "DGSwitches.h"

// ==============================================================
// Instrument lights on/off switch

class InstrLightSwitch: public DGSwitch1 {
public:
	InstrLightSwitch (VESSEL3 *v);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Instrument light brightness dial

class InstrLightBrightnessDial: public PanelElement {
public:
	InstrLightBrightnessDial (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Flood lights off/white/red switch

class FloodLightSwitch: public DGSwitch1 {
public:
	FloodLightSwitch (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Flood light brightness dial

class FloodLightBrightnessDial: public PanelElement {
public:
	FloodLightBrightnessDial (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Landing/docking light switch

class LandDockLightSwitch: public DGSwitch1 {
public:
	LandDockLightSwitch (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Strobe light switch

class StrobeLightSwitch: public DGSwitch1 {
public:
	StrobeLightSwitch (VESSEL3 *v);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// Nav light switch

class NavLightSwitch: public DGSwitch1 {
public:
	NavLightSwitch (VESSEL3 *v);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__COCKPITLIGHT_H