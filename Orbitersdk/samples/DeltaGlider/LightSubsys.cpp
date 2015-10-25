// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// LightSubsys.cpp
// Cockpit and external light control: instrument/overhead lights,
// landing/docking/navigation/strobe lights
// ==============================================================

#define STRICT 1

#include "LightSubsys.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================
// Light control subsystem
// ==============================================================

LightCtrlSubsystem::LightCtrlSubsystem (DeltaGlider *v, int ident)
: DGSubsystem (v, ident)
{
	// create component instances
	AddComponent (instrlight = new InstrumentLight (this));
	AddComponent (cockpitlight = new CockpitLight (this));
}

// --------------------------------------------------------------

LightCtrlSubsystem::~LightCtrlSubsystem ()
{
	// delete components
	delete instrlight;
	delete cockpitlight;
}

// ==============================================================
// Instrument lights
// ==============================================================

InstrumentLight::InstrumentLight (LightCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	light_on   = false;
	brightness = 0.5;
	light_col  = 0;

	ELID_SWITCH = AddElement (sw = new InstrumentLightSwitch (this));
	ELID_DIAL = AddElement (dial = new InstrumentBrightnessDial (this));

	// Instrument brightness dial animation
	static UINT InstrBDialGrp = GRP_INSTR_BRIGHTNESS_VC;
	static MGROUP_ROTATE InstrBDialTransform (1, &InstrBDialGrp, 1,
		VC_INSTR_BRIGHTNESS_ref, VC_INSTR_BRIGHTNESS_axis, (float)(-280*RAD));
	anim_dial = DG()->CreateAnimation (0.5);
	DG()->AddAnimationComponent (anim_dial, 0, 1, &InstrBDialTransform);
}

// --------------------------------------------------------------

void InstrumentLight::SetLight (bool on, bool force)
{
	if (on == light_on && !force) return; // nothing to do
	light_on = on;

	if (DG()->vcmesh) {
		static MATERIAL norm = {{1,1,1,1},{0.8,0.8,0.8,1},{0.1,0.1,0.1,0},{0.15,0.15,0.15,1},5};
		static MATERIAL label_glow[3] = {
			{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0.35,1,0.35,1},0},
		    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,0.7,0.15,1},0},
			{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0.6,0.6,1.0,1},0}};
		static MATERIAL btn_glow = {{1,1,1,1},{1,1,1,1},{0.1,0.1,0.1,1},{0.6,0.6,0.6,1},5};
		oapiSetMaterial (DG()->vcmesh, 0, on ? &btn_glow : &norm);
		int idx = max(0, min(2, light_col));
		MATERIAL mat;
		if (on) {
			float scale = (float)(0.2 + brightness*0.8);
			memcpy(&mat, label_glow+light_col, sizeof(MATERIAL));
			mat.emissive.r *= scale;
			mat.emissive.g *= scale;
			mat.emissive.b *= scale;
		}
		oapiSetMaterial (DG()->vcmesh, 11, on ? &mat : &norm);
		GROUPEDITSPEC ges = {on ? GRPEDIT_ADDUSERFLAG : GRPEDIT_DELUSERFLAG, 0x18, 0,0,0};
		oapiEditMeshGroup (DG()->vcmesh, GRP_LIT_LABELS_VC, &ges);
	}
}

// --------------------------------------------------------------

void InstrumentLight::ModBrightness (bool up)
{
	double dt = oapiGetSimStep();
	double db = dt * (up ? 0.3 : -0.3);
	brightness = max(0.0, min (1.0, brightness + db));
	DG()->SetAnimation (anim_dial, brightness);
	if (light_on) SetLight (true, true);
}

// --------------------------------------------------------------

void InstrumentLight::clbkSaveState (FILEHANDLE scn)
{
	if (light_on || light_col || brightness != 0.5) {
		char cbuf[256];
		sprintf (cbuf, "%d %d %0.2lf", (int)light_on, light_col, brightness);
		oapiWriteScenario_string (scn, "INSTRLIGHT", cbuf);
	}
}

// --------------------------------------------------------------

bool InstrumentLight::clbkParseScenarioLine (const char *line)
{
	if (!strnicmp (line, "INSTRLIGHT", 10)) {
		int lon;
		sscanf (line+10, "%d%d%lf", &lon, &light_col, &brightness);
		light_on = (lon != 0);
		return true;
	}
	return false;
}

// --------------------------------------------------------------

bool InstrumentLight::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Instrument light switch
	oapiVCRegisterArea (GlobalElId(ELID_SWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_SWITCH), VC_INSTRLIGHT_SWITCH_mousearea[0], VC_INSTRLIGHT_SWITCH_mousearea[1], VC_INSTRLIGHT_SWITCH_mousearea[2], VC_INSTRLIGHT_SWITCH_mousearea[3]);
	sw->DefineAnimationVC (VC_INSTRLIGHT_SWITCH_ref, VC_INSTRLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_INSTRLIGHT_SWITCH_vofs);

	// Instrument brightness dial
	oapiVCRegisterArea (GlobalElId(ELID_DIAL), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_DIAL), VC_INSTR_BRIGHTNESS_mousearea[0], VC_INSTR_BRIGHTNESS_mousearea[1], VC_INSTR_BRIGHTNESS_mousearea[2], VC_INSTR_BRIGHTNESS_mousearea[3]);

	return true;
}

// --------------------------------------------------------------

void InstrumentLight::clbkResetVC (int vcid, DEVMESHHANDLE hMesh)
{
	SetLight (light_on, true);
	DG()->SetAnimation (anim_dial, brightness);
}

// ==============================================================

InstrumentLightSwitch::InstrumentLightSwitch (InstrumentLight *comp)
: DGSwitch1(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

void InstrumentLightSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (component->GetLight() ? UP:DOWN);
	DGSwitch1::ResetVC (hMesh);
}

// --------------------------------------------------------------

bool InstrumentLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		component->SetLight (state==DGSwitch1::UP ? 1:0);
		return true;
	}
	return false;
}

// ==============================================================

InstrumentBrightnessDial::InstrumentBrightnessDial (InstrumentLight *comp)
: PanelElement(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

bool InstrumentBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	component->ModBrightness (p.x > 0.5);
	return true;
}

// ==============================================================
// Cockpit floodlights
// ==============================================================

CockpitLight::CockpitLight (LightCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	light = NULL;
	light_mode = 0;
	brightness = 0.7;

	ELID_SWITCH = AddElement (sw = new CockpitLightSwitch (this));
	ELID_DIAL = AddElement (dial = new CockpitBrightnessDial (this));

	// Floodlight brightness dial animation
	static UINT FloodBDialGrp = GRP_FLOOD_BRIGHTNESS_VC;
	static MGROUP_ROTATE FloodBDialTransform (1, &FloodBDialGrp, 1,
		VC_FLOOD_BRIGHTNESS_ref, VC_FLOOD_BRIGHTNESS_axis, (float)(-280*RAD));
	anim_dial = DG()->CreateAnimation (0.5);
	DG()->AddAnimationComponent (anim_dial, 0, 1, &FloodBDialTransform);
}

// --------------------------------------------------------------

void CockpitLight::SetLight (int mode, bool force)
{
	if (mode == light_mode && !force) return; // nothing to do

	static const COLOUR4 zero = {0.0f,0.0f,0.0f,0.0f};
	static const COLOUR4 wcol = {1.0f,1.0f,1.0f,0.0f};
	static const COLOUR4 rcol = {0.6f,0.05f,0.0f,0.0f};

	COLOUR4 col;

	light_mode = mode;
	if (mode) {
		col = (mode == 1 ? wcol : rcol);
		if (!light) {
			light = (PointLight*)DG()->AddPointLight(_V(0,1.65,6.68), 3, 0, 0, 3, col, col, zero);
			light->SetVisibility (LightEmitter::VIS_COCKPIT);
			light->Activate(true);
		}
		double intens = (float)(0.2 + brightness*0.8);
		light->SetIntensity (intens);
	} else {
		DG()->DelLightEmitter (light);
		light = NULL;
	}
}

// --------------------------------------------------------------

void CockpitLight::ModBrightness (bool up)
{
	double dt = oapiGetSimStep();
	double db = dt * (up ? 0.3 : -0.3);
	brightness = max(0.0, min (1.0, brightness + db));
	DG()->SetAnimation (anim_dial, brightness);
	if (light_mode) SetLight (light_mode, true);
}

// --------------------------------------------------------------

void CockpitLight::clbkSaveState (FILEHANDLE scn)
{
	if (light_mode || brightness != 0.7) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.2lf", light_mode, brightness);
		oapiWriteScenario_string (scn, "FLOODLIGHT", cbuf);
	}
}

// --------------------------------------------------------------

bool CockpitLight::clbkParseScenarioLine (const char *line)
{
	if (!strnicmp (line, "FLOODLIGHT", 10)) {
		sscanf (line+10, "%d%lf", &light_mode, &brightness);
		light_mode = max (0, min (2, light_mode));
		brightness = max (0.0, min (1.0, brightness));
		return true;
	}
	return false;
}

// --------------------------------------------------------------

bool CockpitLight::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Floodlight switch
	oapiVCRegisterArea (GlobalElId(ELID_SWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_SWITCH), VC_FLOODLIGHT_SWITCH_mousearea[0], VC_FLOODLIGHT_SWITCH_mousearea[1], VC_FLOODLIGHT_SWITCH_mousearea[2], VC_FLOODLIGHT_SWITCH_mousearea[3]);
	sw->DefineAnimationVC (VC_FLOODLIGHT_SWITCH_ref, VC_FLOODLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_FLOODLIGHT_SWITCH_vofs);

	// Floodlight brightness dial
	oapiVCRegisterArea (GlobalElId(ELID_DIAL), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_DIAL), VC_FLOOD_BRIGHTNESS_mousearea[0], VC_FLOOD_BRIGHTNESS_mousearea[1], VC_FLOOD_BRIGHTNESS_mousearea[2], VC_FLOOD_BRIGHTNESS_mousearea[3]);

	return true;
}

// --------------------------------------------------------------

void CockpitLight::clbkResetVC (int vcid, DEVMESHHANDLE hMesh)
{
	SetLight (light_mode, true);
	DG()->SetAnimation (anim_dial, brightness);
}

// ==============================================================

CockpitLightSwitch::CockpitLightSwitch (CockpitLight *comp)
: DGSwitch1(comp->DG(), DGSwitch1::THREESTATE), component(comp)
{
}

// --------------------------------------------------------------

void CockpitLightSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState ((DGSwitch1::State)component->GetLight());
	DGSwitch1::ResetVC (hMesh);
}

// --------------------------------------------------------------

bool CockpitLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		component->SetLight ((int)state);
		return true;
	}
	return false;
}

// ==============================================================

CockpitBrightnessDial::CockpitBrightnessDial (CockpitLight *comp)
: PanelElement(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

bool CockpitBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	component->ModBrightness (p.x > 0.5);
	return true;
}

