// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ThrottleScram.cpp
// Scram throttle sliders
// ==============================================================

#define STRICT 1
#include "ThrottleScram.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

ThrottleScram::ThrottleScram (VESSEL3 *v): PanelElement (v)
{
	for (int i = 0; i < 2; i++) ppos[i] = 0.0f;
}

// ==============================================================

void ThrottleScram::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_SCRAM_INSTRUMENTS_P0);
	vtxofs = 0;
}

// ==============================================================

bool ThrottleScram::Redraw2D (SURFHANDLE surf)
{
	static const float tx_dy = 18.0f;
	static const float bb_y0 = 471.5f;

	int i, j, vofs;
	float pos;
	static const float sy[4] = {bb_y0,bb_y0,bb_y0+tx_dy,bb_y0+tx_dy};

	DeltaGlider *dg = (DeltaGlider*)vessel;
	for (i = 0; i < 2; i++) {
		double level = dg->GetThrusterLevel (dg->th_scram[i]);
		pos = (float)(-level*84.0);
		if (pos != ppos[i]) {
			vofs = vtxofs+i*4;
			for (j = 0; j < 4; j++) grp->Vtx[vofs+j].y = sy[j]+pos;
			ppos[i] = pos;
		}
	}
	return false;
}

// ==============================================================

bool ThrottleScram::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	static int ctrl = 0;
	if (event & PANEL_MOUSE_LBDOWN) { // record which slider to operate
		if      (mx <  12) ctrl = 0; // left engine
		else if (mx >= 37) ctrl = 1; // right engine
		else               ctrl = 2; // both
	}
	dg->SetScramLevel (ctrl, max (0.0, min (1.0, 1.0-my/84.0)));
	return true;
}