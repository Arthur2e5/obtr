// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// NavButton.cpp
// Navigation mode button user interface
// ==============================================================

#define STRICT 1
#include "NavButton.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"
#include "meshres_vc.h"

// ==============================================================

NavButtons::NavButtons (VESSEL3 *v): PanelElement (v)
{
	for (int i = 0; i < 6; i++)
		btn[i] = new DGButton3 (v);
}

// --------------------------------------------------------------

NavButtons::~NavButtons ()
{
	for (int i = 0; i < 6; i++)
		delete btn[i];
}

// --------------------------------------------------------------

void NavButtons::DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
	DWORD vofs[6], DWORD vofs_label[6])
{
	for (int i = 0; i < 6; i++) 
		btn[i]->DefineAnimationVC (axis, meshgrp, meshgrp_label, vofs[i], vofs_label[i]);
}

// --------------------------------------------------------------

void NavButtons::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 20;
}

// --------------------------------------------------------------

bool NavButtons::Redraw2D (SURFHANDLE)
{
	// constants for texture coordinates
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_y0 = texh-655.0f;        // top edge of texture block
	static const float tx_dy = 37.0f;              // texture block height
	static const float tv0_active = (tx_y0)/texh, tv1_active = (tx_y0+tx_dy)/texh;
	static const float tv0_idle = (tx_y0+tx_dy+0.5f)/texh, tv1_idle = (tx_y0+tx_dy+0.5f)/texh;
	float tv0, tv1;
	int vofs;

	for (DWORD i = NAVMODE_KILLROT; i <= NAVMODE_HOLDALT; i++) {
		if (vessel->GetNavmodeState (i)) tv0 = tv0_active, tv1 = tv1_active;
		else                             tv0 = tv0_idle,   tv1 = tv1_idle;
		vofs = vtxofs+(i-NAVMODE_KILLROT)*4;
		grp->Vtx[vofs+0].tv = grp->Vtx[vofs+1].tv = tv0;
		grp->Vtx[vofs+2].tv = grp->Vtx[vofs+3].tv = tv1;
	}
		
	return false;
}

// --------------------------------------------------------------

bool NavButtons::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	for (int i = 0; i < 6; i++)
		btn[i]->RedrawVC (hMesh, surf);
	return false;
}

// --------------------------------------------------------------

bool NavButtons::ProcessMouse2D (int event, int mx, int my)
{
	int mode = 0;
	if (my < 39) {
		if (mx >= 20 && mx < 59) mode = NAVMODE_KILLROT;
	} else {
		static int navmode[6] = {
			NAVMODE_PROGRADE, NAVMODE_RETROGRADE,
			NAVMODE_NORMAL, NAVMODE_ANTINORMAL,
			NAVMODE_HLEVEL, NAVMODE_HOLDALT
		};
		mode = navmode[mx/39 + ((my-39)/39)*2];
	}
	if (mode) vessel->ToggleNavmode (mode);
	return (mode != 0);
}

// --------------------------------------------------------------

bool NavButtons::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int modemap[2][4] = {{1,4,6,2},{0,3,5,0}};
	static int btnmode[6] = {1,4,3,6,5,2};
	static int modebtn[6] = {0,5,2,1,4,3};
	int ix = (int)(p.x*169.0);
	int iy = (int)(p.y*63);
	int br = ix/43;
	int bc = iy/33;
	if (ix-br*43 >= 40) return false;
	if (iy-bc*33 >= 30) return false;
	int mode = modemap[bc][br];
	if (!mode) return false;
	int b = modebtn[mode-1];
	int i;

	if (event & PANEL_MOUSE_LBDOWN) {
		for (i = 0; i < 6; i++) {
			if (i==b) {
				btn[i]->SetState (btn[i]->GetState() == DGButton3::OFF ? DGButton3::PRESSED_FROM_OFF : DGButton3::PRESSED_FROM_ON);
			} else {
				bool ison = vessel->GetNavmodeState (btnmode[i]);
				btn[i]->SetState (ison ? DGButton3::ON : DGButton3::OFF);
			}
		}
		vessel->ToggleNavmode (mode);
	} else if (event & PANEL_MOUSE_LBUP) {
		btn[b]->SetState (btn[b]->GetState() == DGButton3::PRESSED_FROM_OFF ? DGButton3::ON : DGButton3::OFF);
	}
	return true;
}

// --------------------------------------------------------------

void NavButtons::SetMode (int mode, bool active)
{
	static int modebtn[6] = {0,5,2,1,4,3};
	int b = modebtn[mode-1];
	if (active) {
		if (btn[b]->GetState () == DGButton3::OFF)
			btn[b]->SetState (DGButton3::ON);
	} else {
		btn[b]->SetState (DGButton3::OFF);
	}
}
