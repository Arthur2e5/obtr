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
// DGSubSystem
// ==============================================================

void DGSubSystem::AddComponent (DGSubSystemComponent *comp)
{
	component.push_back (comp);
}

// --------------------------------------------------------------

void DGSubSystem::clbkPostStep (double simt, double simdt, double mjd)
{
	for (int i = 0; i < component.size(); i++)
		component[i]->clbkPostStep (simt, simdt, mjd);
}

// --------------------------------------------------------------

bool DGSubSystem::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	bool b = false;
	for (int i = 0; i < component.size(); i++) {
		bool bi = component[i]->clbkLoadPanel2D (panelid, hPanel, viewW, viewH);
		b = b || bi;
	}
	return b;
}

// --------------------------------------------------------------

bool DGSubSystem::clbkLoadVC (int vcid)
{
	bool b = false;
	for (int i = 0; i < component.size(); i++) {
		bool bi = component[i]->clbkLoadVC (vcid);
		b = b || bi;
	}
	return b;
}

// ==============================================================

DGSubSystemComponent::DGSubSystemComponent (DGSubSystem *_subsys)
: subsys(_subsys)
{}

