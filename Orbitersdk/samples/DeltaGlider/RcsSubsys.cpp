// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// RcsSubsys.cpp
// Reaction control subsystem: lin/rot selection, attitude programs
// ==============================================================

#define STRICT 1
#include "RcsSubsys.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================
// Reaction control subsystem
// ==============================================================

RcsSubsystem::RcsSubsystem (DeltaGlider *dg, int ident)
: DGSubsystem (dg, ident)
{
	ELID_MODEDIAL = AddElement (modedial = new RcsModeDial(this));
}

// --------------------------------------------------------------

void RcsSubsystem::SetMode (int mode)
{
	oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_MODEDIAL));
}

// --------------------------------------------------------------

bool RcsSubsystem::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_MODEDIAL), _R(1136, 69,1176,113), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN, 0, modedial);

	return true;
}

// --------------------------------------------------------------

bool RcsSubsystem::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// RCS mode dial
	oapiVCRegisterArea (GlobalElId(ELID_MODEDIAL), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_MODEDIAL), VC_RCS_DIAL_mousearea[0], VC_RCS_DIAL_mousearea[1], VC_RCS_DIAL_mousearea[2], VC_RCS_DIAL_mousearea[3]);
	modedial->DefineAnimationVC (VC_RCS_DIAL_ref, VC_RCS_DIAL_axis, GRP_DIAL1_VC, VC_RCS_DIAL_vofs);

	return true;
}


// ==============================================================
// Mode dial
// ==============================================================

RcsModeDial::RcsModeDial (RcsSubsystem *_subsys)
: DGDial1(_subsys->DG(), 3, -50*RAD, 50*RAD), subsys(_subsys)
{
}

// --------------------------------------------------------------

void RcsModeDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 0;
}

// --------------------------------------------------------------

void RcsModeDial::ResetVC (DEVMESHHANDLE hMesh)
{
	DWORD mode = subsys->DG()->GetAttitudeMode();
	SetPosition (mode);
}

// --------------------------------------------------------------

bool RcsModeDial::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float texw = (float)PANEL2D_TEXW; // texture width
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_x0 = 1160.5f;            // left edge of texture block
	static const float tx_y0 = texh-615.5f;        // top edge of texture block
	static const float tx_dx = 39.0f;              // texture block width
	static const float tx_dy = 43.0f;              // texture block height
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};

	float dtu = (float)(subsys->DG()->GetAttitudeMode()*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// --------------------------------------------------------------

bool RcsModeDial::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	int pos = subsys->DG()->GetAttitudeMode();
	SetPosition(pos);
	return DGDial1::RedrawVC (hMesh, surf);
}

// --------------------------------------------------------------

bool RcsModeDial::ProcessMouse2D (int event, int mx, int my)
{
	return (mx < 20 ? subsys->DG()->DecAttMode() : subsys->DG()->IncAttMode());
}

// --------------------------------------------------------------

bool RcsModeDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGDial1::ProcessMouseVC (event, p)) {
		int pos = GetPosition();
		subsys->DG()->SetAttitudeMode (pos);
		return true;
	}
	return false;
}
