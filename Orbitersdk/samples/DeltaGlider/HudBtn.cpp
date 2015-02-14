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

HUDButton::HUDButton (VESSEL3 *v): PanelElement (v)
{
}

// --------------------------------------------------------------

void HUDButton::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 8;
}

// --------------------------------------------------------------

void HUDButton::ResetVC (DEVMESHHANDLE hMesh)
{
}

// --------------------------------------------------------------

bool HUDButton::Redraw2D (SURFHANDLE surf)
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

bool HUDButton::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;
	DeltaGlider *dg = (DeltaGlider*)vessel;

	static const int nbutton = 3;
	static const int nvtx_per_button = 16;
	static const int nvtx = nbutton * nvtx_per_button;
	int i, j;
	NTVERTEX vtx[nvtx];
	GROUPEDITSPEC ges;
	ges.flags = GRPEDIT_VTXCRDZ | GRPEDIT_VTXTEXV;
	ges.Vtx = vtx;
	ges.nVtx = nvtx;
	ges.vIdx = NULL;
	static float z0_base[nvtx_per_button] = {
		7.2630f,7.2630f,7.2630f,7.2630f,7.2630f,7.2630f,7.2680f,7.2680f,
		7.2630f,7.2630f,7.2680f,7.2680f,7.2630f,7.2630f,7.2680f,7.2680f
	};
	static float z0_shift = 0.004f;
	static float v0_base[nvtx_per_button] = {
		0.2002f,0.2002f,0.1602f,0.1602f,0.1602f,0.1602f,0.1602f,0.1602f,
		0.2002f,0.1602f,0.2002f,0.1602f,0.2002f,0.1602f,0.2002f,0.1602f
	};
	static float v0_shift = (float)(41.0/1024.0);
	static const int mode[3] = {HUD_ORBIT,HUD_SURFACE,HUD_DOCKING};
	for (i = 0; i < nbutton; i++) {
		int vofs = i*nvtx_per_button;
		bool hilight = (dg->GetHUDMode() == mode[i]);
		for (j = 0; j < nvtx_per_button; j++) {
			vtx[vofs+j].z = z0_base[j] + (hilight ? z0_shift : 0);
			vtx[vofs+j].tv = v0_base[j] + (hilight && i<3 ? v0_shift : 0);
		}
	}
	oapiEditMeshGroup (hMesh, GRP_HUD_BUTTONS_VC, &ges);
	return false;
}

// --------------------------------------------------------------

bool HUDButton::ProcessMouse2D (int event, int mx, int my)
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

bool HUDButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	static const int mode[3] = {HUD_ORBIT,HUD_SURFACE,HUD_DOCKING};
	int btn = max(0, min (2, (int)(p.x*3.0)));
	dg->SetHUDMode (mode[btn]);
	return true;
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

HUDColourButton::HUDColourButton (VESSEL3 *v): PanelElement (v)
{
	pending_action = 0;
}

bool HUDColourButton::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (pending_action) {
		static const int nvtx_per_button = 20;
		static const double depth = 0.004;
		static int meshgrp = GRP_BUTTON2_VC;
		NTVERTEX dvtx[nvtx_per_button];
		WORD vofs[nvtx_per_button];
		float dz = (float)(pending_action == 1 ? depth : -depth);
		static int vofs0 = 0;
		for (int i = 0; i < nvtx_per_button; i++) {
			dvtx[i].z = dz;
			vofs[i] = vofs0 + i;
		}
		GROUPEDITSPEC ges = {GRPEDIT_VTXCRDADDZ, 0, dvtx, nvtx_per_button, vofs};
		oapiEditMeshGroup (hMesh, meshgrp, &ges);

		pending_action = 0;
	}
	return false;
}

// --------------------------------------------------------------

bool HUDColourButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (event & PANEL_MOUSE_LBDOWN) {
		oapiToggleHUDColour ();
		pending_action = 1;
	} else {
		pending_action = 2;
	}
	return true;
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