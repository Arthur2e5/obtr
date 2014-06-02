// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// AtCtrlDial.cpp
// Atmospheric control selector dial user interface
// ==============================================================

#define STRICT 1
#include "AtctrlDial.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

ATCtrlDial::ATCtrlDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void ATCtrlDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 4;
}

// ==============================================================

bool ATCtrlDial::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float texw = (float)PANEL2D_TEXW; // texture width
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_x0 = 1160.5f;            // left edge of texture block
	static const float tx_y0 = texh-615.5f;        // top edge of texture block
	static const float tx_dx = 39.0f;              // texture block width
	static const float tx_dy = 43.0f;              // texture block height
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};

	float dtu = (float)(min(vessel->GetADCtrlMode(),2)*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// ==============================================================

bool ATCtrlDial::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	DWORD mode = vessel->GetADCtrlMode();
	dg->SetAnimation (dg->anim_afdial, mode == 0 ? 0 : mode == 7 ? 0.5 : 1);
	return false;
}

// ==============================================================

bool ATCtrlDial::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	return (mx < 20 ? dg->DecADCMode() : dg->IncADCMode());
}

// ==============================================================

bool ATCtrlDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DWORD mode = vessel->GetADCtrlMode();

	if (p.x < 0.5) { // dial turn left
		if (mode > 0) {
			vessel->SetADCtrlMode (mode==1 ? 7 : 0);
			return true;
		}
	} else { // dial turn right
		if (mode != 1) {
			vessel->SetADCtrlMode (mode==0 ? 7 : 1);
			return true;
		}
	}
	return false;
}