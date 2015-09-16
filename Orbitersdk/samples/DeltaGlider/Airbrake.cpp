// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// Airbrake.cpp
// Airbrake control
// ==============================================================

#define STRICT 1
#include "Airbrake.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// ==============================================================

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width
static const float texh = (float)PANEL2D_TEXH; // texture height
static const float tx_x0 = 1138.0f;
static const float tx_y0 = texh-580.0f;
// constants for panel coordinates
static const float bb_x0 = 1242.5f;
static const float bb_y0 =  217.5f;
static const float bb_dx =   21.0f;
static const float bb_dy =    7.0f;

// ==============================================================

Airbrake::Airbrake (VESSEL3 *v): PanelElement (v)
{
//	Reset2D();
}

// ==============================================================
#ifdef UNDEF
void Airbrake::AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx)
{
	static const DWORD NVTX = 4;
	static const DWORD NIDX = 6;
	static const NTVERTEX VTX[NVTX] = {
		{bb_x0,      bb_y0,      0,  0,0,0,  tx_x0/texw,         tx_y0/texh},
		{bb_x0+bb_dx,bb_y0,      0,  0,0,0,  (tx_x0+bb_dx)/texw, tx_y0/texh},
		{bb_x0,      bb_y0+bb_dy,0,  0,0,0,  tx_x0/texw,         (tx_y0+bb_dy)/texh},
		{bb_x0+bb_dx,bb_y0+bb_dy,0,  0,0,0,  (tx_x0+bb_dx)/texw, (tx_y0+bb_dy)/texh}
	};
	static const WORD IDX[NIDX] = {
		0,1,2, 3,2,1
	};

	AddGeometry (hMesh, grpidx, VTX, NVTX, IDX, NIDX);
}
#endif
// ==============================================================

void Airbrake::Reset2D (MESHHANDLE hMesh)
{
	state = -1;
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 64;
}

// ==============================================================

void Airbrake::ResetVC (DEVMESHHANDLE hMesh)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->SetAnimation (dg->anim_airbrakelever, dg->airbrakelever_proc);
}

// ==============================================================

bool Airbrake::Redraw2D (SURFHANDLE surf)
{
	DeltaGlider* dg = (DeltaGlider*)vessel;
	DeltaGlider::DoorStatus ds = dg->brake_status;
	int newstate = dg->airbrake_tgt; //(ds == DeltaGlider::DOOR_CLOSED || ds == DeltaGlider::DOOR_CLOSING ? 0 : 1);
	if (newstate != state) {
		state = newstate;
		static const float yp[4] = {bb_y0, bb_y0, bb_y0+bb_dy, bb_y0+bb_dy};
		float yshift = state*24.0f;
		for (int i = 0; i < 4; i++)
			grp->Vtx[vtxofs+i].y = yp[i]+yshift;
	}
	return false;
}

// ==============================================================

bool Airbrake::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ActivateAirbrake (my > 30 ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
	return false;
}

// ==============================================================

bool Airbrake::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ActivateAirbrake (p.y > 0.5 ? DeltaGlider::DOOR_CLOSING : DeltaGlider::DOOR_OPENING);
	return false;
}

