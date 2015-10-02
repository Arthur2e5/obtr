// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DGSubsys.cpp
// Base classes for DG subsystems, subsystem components and
// panel elements
// ==============================================================

#include "DGSubsys.h"

// ==============================================================
// DGSubsystem
// ==============================================================

void DGSubsystem::AddComponent (DGSubsystemComponent *comp)
{
	component.push_back (comp);
}

// --------------------------------------------------------------

void DGSubsystem::clbkPostCreation ()
{
	for (int i = 0; i < component.size(); i++)
		component[i]->clbkPostCreation ();
}

// --------------------------------------------------------------

void DGSubsystem::clbkSaveState (FILEHANDLE scn)
{
	for (int i = 0; i < component.size(); i++)
		component[i]->clbkSaveState (scn);
}

// --------------------------------------------------------------

bool DGSubsystem::clbkParseScenarioLine (const char *line)
{
	for (int i = 0; i < component.size(); i++)
		if (component[i]->clbkParseScenarioLine (line))
			return true;
	return false;
}

// --------------------------------------------------------------

void DGSubsystem::clbkPostStep (double simt, double simdt, double mjd)
{
	for (int i = 0; i < component.size(); i++)
		component[i]->clbkPostStep (simt, simdt, mjd);
}

// --------------------------------------------------------------

bool DGSubsystem::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	bool b = false;
	for (int i = 0; i < component.size(); i++) {
		bool bi = component[i]->clbkLoadPanel2D (panelid, hPanel, viewW, viewH);
		b = b || bi;
	}
	return b;
}

// --------------------------------------------------------------

bool DGSubsystem::clbkLoadVC (int vcid)
{
	bool b = false;
	for (int i = 0; i < component.size(); i++) {
		bool bi = component[i]->clbkLoadVC (vcid);
		b = b || bi;
	}
	return b;
}

// --------------------------------------------------------------

void DGSubsystem::clbkReset2D (int panelid, MESHHANDLE hMesh)
{
	Subsystem::clbkReset2D (panelid, hMesh);

	for (int i = 0; i < component.size(); i++)
		component[i]->clbkReset2D (panelid, hMesh);
}

// --------------------------------------------------------------

void DGSubsystem::clbkResetVC (int vcid, DEVMESHHANDLE hMesh)
{
	Subsystem::clbkResetVC (vcid, hMesh);

	for (int i = 0; i < component.size(); i++)
		component[i]->clbkResetVC (vcid, hMesh);
}

// ==============================================================

DGSubsystemComponent::DGSubsystemComponent (DGSubsystem *_subsys)
: subsys(_subsys)
{}

