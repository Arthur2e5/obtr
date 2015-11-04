// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// AvionicsSubsys.h
// Subsystem for avionics components:
// - angular rate indicator
// ==============================================================

#define STRICT 1

#include "AvionicsSubsys.h"
#include "MomentInd.h"

// ==============================================================
// Avionics subsystem
// ==============================================================

AvionicsSubsystem::AvionicsSubsystem (DeltaGlider *v, int ident)
: DGSubsystem (v, ident)
{
	extern GDIParams g_Param;

	// create component instances
	ELID_ANGRATEIND = AddElement (angrateind = new AngRateIndicator (v, g_Param.surf));
}

// --------------------------------------------------------------

bool AvionicsSubsystem::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	// angular rate indicators
	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_ANGRATEIND), _R(0,0,0,0), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, angrateind);

	return true;
}

// --------------------------------------------------------------

bool AvionicsSubsystem::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// angular rate indicators
	oapiVCRegisterArea (GlobalElId(ELID_ANGRATEIND), _R(0,0,1,1), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_DIRECT, oapiGetTextureHandle (DG()->vcmesh_tpl, 14));

	return true;
}

