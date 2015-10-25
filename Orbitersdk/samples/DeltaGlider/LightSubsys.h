// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// LightSubsys.h
// Cockpit and external light control: instrument/overhead lights,
// landing/docking/navigation/strobe lights
// ==============================================================

#ifndef __LIGHTSUBSYS_H
#define __LIGHTSUBSYS_H

#include "DGSwitches.h"
#include "DGSubsys.h"

// ==============================================================
// Light control subsystem
// ==============================================================

class InstrumentLight;
class CockpitLight;

class LightCtrlSubsystem: public DGSubsystem {
public:
	LightCtrlSubsystem (DeltaGlider *v, int ident);
	~LightCtrlSubsystem ();

private:
	InstrumentLight *instrlight;
	CockpitLight *cockpitlight;
};

// ==============================================================
// Instrument lights
// ==============================================================

class InstrumentLight: public DGSubsystemComponent {
	friend class InstrumentLightSwitch;
	friend class InstrumentBrightnessDial;

public:
	InstrumentLight (LightCtrlSubsystem *_subsys);
	void SetLight (bool on, bool force=false);
	inline bool GetLight () const { return light_on; }
	void ModBrightness (bool up);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	bool clbkLoadVC (int vcid);
	void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);

private:
	bool light_on;                     // instrument illumination switch status
	int light_col;                     // instr. light colour index, 0=default
	double brightness;                 // instrument illumination brightness setting

	InstrumentLightSwitch *sw;
	InstrumentBrightnessDial *dial;
	int ELID_SWITCH;
	int ELID_DIAL;
	UINT anim_dial;                    // VC instrument brightness dial
};

// ==============================================================

class InstrumentLightSwitch: public DGSwitch1 {
public:
	InstrumentLightSwitch (InstrumentLight *comp);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	InstrumentLight *component;
};

// ==============================================================

class InstrumentBrightnessDial: public PanelElement {
public:
	InstrumentBrightnessDial (InstrumentLight *comp);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	InstrumentLight *component;
};

// ==============================================================
// Cockpit floodlights
// ==============================================================

class CockpitLight: public DGSubsystemComponent {
	friend class CockpitLightSwitch;
	friend class CockpitBrightnessDial;

public:
	CockpitLight (LightCtrlSubsystem *_subsys);
	void SetLight (int mode, bool force=false);
	inline int GetLight () const { return light_mode; }
	void ModBrightness (bool up);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	bool clbkLoadVC (int vcid);
	void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);

private:
	int light_mode;                  // 0=off, 1=red, 2=white
	double brightness;               // floodlight brightness
	PointLight *light;               // local light object

	CockpitLightSwitch *sw;
	CockpitBrightnessDial *dial;
	int ELID_SWITCH;
	int ELID_DIAL;
	UINT anim_dial;                  // VC floodlight brightness dial
};

// ==============================================================

class CockpitLightSwitch: public DGSwitch1 {
public:
	CockpitLightSwitch (CockpitLight *comp);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	CockpitLight *component;
};

// ==============================================================

class CockpitBrightnessDial: public PanelElement {
public:
	CockpitBrightnessDial (CockpitLight *comp);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	CockpitLight *component;
};

#endif // !__LIGHTSUBSYS_H