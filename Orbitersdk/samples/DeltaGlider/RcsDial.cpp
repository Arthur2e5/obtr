// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// RcsDial.cpp
// RCS selector dial user interface
// ==============================================================

#define STRICT 1
#include "RcsDial.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

RCSDial::RCSDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void RCSDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 0;
}

// ==============================================================

bool RCSDial::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float texw = (float)PANEL2D_TEXW; // texture width
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_x0 = 1160.5f;            // left edge of texture block
	static const float tx_y0 = texh-615.5f;        // top edge of texture block
	static const float tx_dx = 39.0f;              // texture block width
	static const float tx_dy = 43.0f;              // texture block height
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};

	float dtu = (float)(vessel->GetAttitudeMode()*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// ==============================================================

bool RCSDial::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	return (mx < 20 ? dg->DecAttMode() : dg->IncAttMode());
}

// ==============================================================

bool RCSDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;

	if (p.x < 0.5) return dg->DecAttMode();
	else           return dg->IncAttMode();
}