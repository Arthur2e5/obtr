// ==============================================================
//                 ORBITER MODULE: ShuttleA
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// rcsswitch.cpp
// User interface RCS switch
// ==============================================================

#define STRICT 1
#include "rcsswitch.h"

static const float texw = (float)PANELEL_TEXW;
static const float texh = (float)PANELEL_TEXH;
static const float tx_x0 =   0.5f;
static const float tx_y0 = texh-43.5f;
static const float tx_dx =  19.0f;
static const float tx_dy =  43.0f;
static const float bb_x0 = 390.5f;
static const float bb_y0 =  52.5f;

// ==============================================================

RCSSwitch::RCSSwitch (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void RCSSwitch::AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx)
{
	static const DWORD nvtx = 4;
	static const DWORD nidx = 6;
	static const NTVERTEX vtx[nvtx] = {
		{bb_x0,      bb_y0,      0,  0,0,0,  tx_x0/texw,        tx_y0/texh        },
		{bb_x0+tx_dx,bb_y0,      0,  0,0,0,  (tx_x0+tx_dx)/texw,tx_y0/texh        },
		{bb_x0,      bb_y0+tx_dy,0,  0,0,0,  tx_x0/texw,        (tx_y0+tx_dy)/texh},
		{bb_x0+tx_dx,bb_y0+tx_dy,0,  0,0,0,  (tx_x0+tx_dx)/texw,(tx_y0+tx_dy)/texh}
	};
	static const WORD idx[nidx] = {
		0,1,2,  3,2,1
	};
	AddGeometry (hMesh, grpidx, vtx, nvtx, idx, nidx);
}

// ==============================================================

bool RCSSwitch::Redraw2D (SURFHANDLE surf)
{
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};
	float dtu = (float)(vessel->GetAttitudeMode()*20.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// ==============================================================

bool RCSSwitch::ProcessMouse2D (int event, int mx, int my)
{
	int mode = vessel->GetAttitudeMode();
	if (my < 21) {
		switch (mode) {
		case RCS_ROT:  vessel->SetAttitudeMode (RCS_LIN); return true;
		case RCS_NONE: vessel->SetAttitudeMode (RCS_ROT); return true;
		}
	} else {
		switch (mode) {
		case RCS_LIN:  vessel->SetAttitudeMode (RCS_ROT); return true;
		case RCS_ROT:  vessel->SetAttitudeMode (RCS_NONE); return true;
		}
	}
	return false;
}