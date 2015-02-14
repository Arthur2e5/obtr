// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// CockpitLight.cpp
// Cockpit lighting switches
// ==============================================================

#define STRICT 1
#include "CockpitLight.h"
#include "DeltaGlider.h"

// ==============================================================

InstrLightSwitch::InstrLightSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::TWOSTATE)
{
}

// ==============================================================

void InstrLightSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (((DeltaGlider*)vessel)->GetInstrLight() ? UP:DOWN);
	DGSwitch1::ResetVC (hMesh);
}

// ==============================================================

bool InstrLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		((DeltaGlider*)vessel)->SetInstrLight (state==DGSwitch1::UP ? 1:0);
		return true;
	}
	return false;
}

// ==============================================================
// ==============================================================

FloodLightSwitch::FloodLightSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::THREESTATE)
{
}

// ==============================================================

bool FloodLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		((DeltaGlider*)vessel)->SetFloodLight ((int)state);
		return true;
	}
	return false;
}

// ==============================================================
// ==============================================================

InstrLightBrightnessDial::InstrLightBrightnessDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

bool InstrLightBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ModInstrBrightness (p.x > 0.5);
	return true;
}

// ==============================================================
// ==============================================================

FloodLightBrightnessDial::FloodLightBrightnessDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

bool FloodLightBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ModFloodBrightness (p.x > 0.5);
	return true;
}

// ==============================================================
// ==============================================================

LandDockLightSwitch::LandDockLightSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::THREESTATE)
{
}

// ==============================================================

bool LandDockLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		((DeltaGlider*)vessel)->SetLandDockLight ((int)state);
		return true;
	}
	return false;
}

// ==============================================================
// ==============================================================

StrobeLightSwitch::StrobeLightSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::TWOSTATE)
{
}

// ==============================================================

void StrobeLightSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (((DeltaGlider*)vessel)->GetStrobeLight() ? UP:DOWN);
	DGSwitch1::ResetVC (hMesh);
}

// ==============================================================

bool StrobeLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		((DeltaGlider*)vessel)->SetStrobeLight (state==UP);
		return true;
	}
	return false;
}

// ==============================================================
// ==============================================================

NavLightSwitch::NavLightSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::TWOSTATE)
{
}

// ==============================================================

void NavLightSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (((DeltaGlider*)vessel)->GetNavLight() ? UP:DOWN);
	DGSwitch1::ResetVC (hMesh);
}

// ==============================================================

bool NavLightSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		((DeltaGlider*)vessel)->SetNavLight (state==UP);
		return true;
	}
	return false;
}
