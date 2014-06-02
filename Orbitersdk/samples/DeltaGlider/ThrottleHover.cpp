// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ThrottleHover.cpp
// Hover throttle sliders
// ==============================================================

#define STRICT 1
#include "ThrottleHover.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

ThrottleHover::ThrottleHover (VESSEL3 *v): PanelElement (v)
{
	ppos = 0.0f;
}

// ==============================================================

void ThrottleHover::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 56;
}

// ==============================================================

bool ThrottleHover::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float tx_dy = 18.0f;
	static const float bb_y0 = 358.5f;

	int j;
	float pos;
	static const float sy[4] = {bb_y0,bb_y0,bb_y0+tx_dy,bb_y0+tx_dy};

	DeltaGlider *dg = (DeltaGlider*)vessel;
	double level = dg->GetThrusterLevel (dg->th_hover[0]);
	pos = (float)(-level*116.0);
	if (pos != ppos) {
		for (j = 0; j < 4; j++) grp->Vtx[vtxofs+j].y = sy[j]+pos;
		ppos = pos;
	}
	return false;
}

// ==============================================================

bool ThrottleHover::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	my = max (0, min (116, my-9));
	dg->SetThrusterGroupLevel (dg->thg_hover, 1.0-my/116.0);
	return true;
}
