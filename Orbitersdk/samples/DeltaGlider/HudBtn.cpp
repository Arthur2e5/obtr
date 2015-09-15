// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// HudBtn.cpp
// HUD mode selector button interface
// ==============================================================

#define STRICT 1
#include "HudBtn.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"
#include "meshres_vc.h"

// ==============================================================

HUDModeButtons::HUDModeButtons (VESSEL3 *v)
: PanelElement (v)
{
	vmode = 0;
	for (int i = 0; i < 3; i++)
		btn[i] = new DGButton3 (v);
}

// --------------------------------------------------------------

HUDModeButtons::~HUDModeButtons ()
{
	for (int i = 0; i < 3; i++)
		delete btn[i];
}

// --------------------------------------------------------------

void HUDModeButtons::DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
	DWORD vofs[3], DWORD vofs_label[3])
{
	for (int i = 0; i < 3; i++) 
		btn[i]->DefineAnimationVC (axis, meshgrp, meshgrp_label, vofs[i], vofs_label[i]);
}

// --------------------------------------------------------------

void HUDModeButtons::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 8;
}

// --------------------------------------------------------------

bool HUDModeButtons::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float tx_dy =  4.0f;       // texture block height
	static const float bb_y0 = 19.5f;       // top edge of button block

	float y, y0 = bb_y0, y1 = bb_y0+tx_dy;
	int i, j, mode = oapiGetHUDMode();
	for (i = 0; i < 3; i++) {
		y = (i+1 == mode ? y1 : y0);
		for (j = 2; j < 4; j++)
			grp->Vtx[vtxofs+i*4+j].y = y;
	}
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	for (int i = 0; i < 3; i++)
		btn[i]->RedrawVC (hMesh, surf);
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	if (mx%29 < 20) {
		int mode = HUD_NONE+(mx/29);
		if (mode == HUD_NONE)
			dg->ModHUDBrightness(my < 8);
		else if (event & PANEL_MOUSE_LBDOWN)
			dg->SetHUDMode (mode);
	}
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::ProcessMouseVC (int event, VECTOR3 &p)
{
	int i;
	int ix = (int)(p.x*126.0);
	int b = ix/43;
	if (ix-b*43 >= 40) return false;

	if (event & PANEL_MOUSE_LBDOWN) {
		for (i = 0; i < 3; i++)
			btn[i]->SetState (i==b ? DGButton3::PRESSED_FROM_OFF : DGButton3::OFF);
		DeltaGlider *dg = (DeltaGlider*)vessel;
		static const int mode[3] = {HUD_ORBIT,HUD_SURFACE,HUD_DOCKING};
		vmode = mode[b];
		dg->SetHUDMode (vmode);
	} else if (event & PANEL_MOUSE_LBUP) {
		btn[b]->SetState (DGButton3::ON);
	}
	return true;
}

// --------------------------------------------------------------

void HUDModeButtons::SetMode (int mode)
{
	if (mode != vmode) {
		int b = mode-HUD_ORBIT;
		for (int i = 0; i < 3; i++)
			btn[i]->SetState (i==b ? DGButton3::ON : DGButton3::OFF);
		vmode = mode;
	}
}

// ==============================================================
// ==============================================================

HUDBrightnessDial::HUDBrightnessDial (VESSEL3 *v): PanelElement (v)
{
}

// --------------------------------------------------------------

bool HUDBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ModHUDBrightness (p.x > 0.5);
	return true;
}

// ==============================================================
// ==============================================================

HUDColourButton::HUDColourButton (VESSEL3 *v): DGButton2(v)
{
}

// --------------------------------------------------------------

bool HUDColourButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	DGButton2::ProcessMouseVC (event, p);
	if (event & PANEL_MOUSE_LBDOWN)
		oapiToggleHUDColour ();
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================
// ==============================================================

HUDUpDownSwitch::HUDUpDownSwitch (VESSEL3 *v): DGSwitch1(v, DGSwitch1::SPRING)
{
}

// --------------------------------------------------------------

bool HUDUpDownSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		switch (state) {
			case DGSwitch1::UP: ((DeltaGlider*)vessel)->ActivateHud (DeltaGlider::DOOR_OPENING); break;
			case DGSwitch1::DOWN: ((DeltaGlider*)vessel)->ActivateHud (DeltaGlider::DOOR_CLOSING); break;
		}
		return true;
	}
	return false;
}