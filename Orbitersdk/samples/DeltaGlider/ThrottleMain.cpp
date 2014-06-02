// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ThrottleMain.cpp
// Main throttle sliders
// ==============================================================

#define STRICT 1
#include "ThrottleMain.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

ThrottleMain::ThrottleMain (VESSEL3 *v): PanelElement (v)
{
	for (int i = 0; i < 2; i++) ppos[i] = 0.0f;
}

// ==============================================================

void ThrottleMain::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 48;
}

// ==============================================================

bool ThrottleMain::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float tx_dy = 18.0f;
	static const float bb_y0 = 171.5f;

	int i, j, vofs;
	float pos;
	static const float sy[4] = {bb_y0,bb_y0,bb_y0+tx_dy,bb_y0+tx_dy};

	DeltaGlider *dg = (DeltaGlider*)vessel;
	for (i = 0; i < 2; i++) {
		double level = dg->GetThrusterLevel (dg->th_main[i]);
		if (level > 0) pos = (float)(-8.0-level*108.0);
		else {
			level = dg->GetThrusterLevel (dg->th_retro[i]);
			if (level > 0) pos = (float)(8.0+level*30.0);
			else           pos = 0.0f;
		}
		if (pos != ppos[i]) {
			vofs = vtxofs+i*4;
			for (j = 0; j < 4; j++) grp->Vtx[vofs+j].y = sy[j]+pos;
			ppos[i] = pos;
		}
	}
	return false;
}

// ==============================================================

bool ThrottleMain::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	static int ctrl = 0;
	if (event & PANEL_MOUSE_LBDOWN) { // record which slider to operate
		if      (mx <  12) ctrl = 0; // left engine
		else if (mx >= 37) ctrl = 1; // right engine
		else               ctrl = 2; // both
	}
	if ((my -= 9) < 0) my = 0;
	else if (my > 157) my = 157;
	dg->SetMainRetroLevel (ctrl, my <= 108 ? 1.0-my/108.0  : 0.0,   // main thruster level
			                     my >= 125 ? (my-125)/32.0 : 0.0);  // retro thruster level
	return true;
}