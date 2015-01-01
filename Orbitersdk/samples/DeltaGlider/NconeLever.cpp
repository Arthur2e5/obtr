// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// NconeLever.cpp
// Nose cone open/close lever interface
// ==============================================================

#define STRICT 1
#include "NconeLever.h"
#include "meshres_p0.h"
#include "meshres_vc.h"

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width
static const float texh = (float)PANEL2D_TEXH; // texture height

// ==============================================================

NoseconeLever::NoseconeLever (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void NoseconeLever::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 84;
}

// ==============================================================

bool NoseconeLever::Redraw2D (SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	DeltaGlider::DoorStatus action = dg->nose_status;
	bool leverdown = (action == DeltaGlider::DOOR_OPENING || action == DeltaGlider::DOOR_OPEN);

	float y0, dy, tv0;
	if (leverdown) y0 = 400.5f, dy = 21.0f, tv0 = texh-677.5f;
	else           y0 = 326.5f, dy = 19.0f, tv0 = texh-696.5f;
	int j;
	for (j = 0; j < 4; j++) {
		grp->Vtx[vtxofs+j].y = y0 + (j/2)*dy;
		grp->Vtx[vtxofs+j].tv = (tv0 + (j/2)*dy)/texh;
	}
	return false;
}

// ==============================================================

bool NoseconeLever::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	DeltaGlider::DoorStatus action = dg->nose_status;
	if (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING) {
		if (my < 58) dg->ActivateDockingPort (DeltaGlider::DOOR_OPENING);
	} else {
		if (my > 36) dg->ActivateDockingPort (DeltaGlider::DOOR_CLOSING);
	}
	return false;
}

// ==============================================================

bool NoseconeLever::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ActivateDockingPort (p.y > 0.5 ? DeltaGlider::DOOR_CLOSING : DeltaGlider::DOOR_OPENING);
	return false;
}

// ==============================================================
// ==============================================================

NoseconeIndicator::NoseconeIndicator (VESSEL3 *v): PanelElement (v)
{
	tofs = (double)rand()/(double)RAND_MAX;
	light = true;
}

// ==============================================================

void NoseconeIndicator::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 88;
}

// ==============================================================

bool NoseconeIndicator::Redraw2D (SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int i, j, xofs;
	double d;
	DeltaGlider::DoorStatus action = dg->nose_status;
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: xofs = 1014; break;
		case DeltaGlider::DOOR_OPEN:   xofs = 1027; break;
		default: xofs = (modf (oapiGetSimTime()+tofs, &d) < 0.5 ? 1040 : 1014); break;
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 3; j++)
			grp->Vtx[vtxofs+i*3+j].tu = (xofs + (j%2)*12)/texw;
	}
	return false;
}

// ==============================================================

bool NoseconeIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;

	DeltaGlider::DoorStatus action = ((DeltaGlider*)vessel)->nose_status;
	bool showlights;
	double d;
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: showlights = false; break;
		case DeltaGlider::DOOR_OPEN:   showlights = true; break;
		default: showlights = (modf (oapiGetSimTime()+tofs, &d) < 0.5); break;
	}
	if (showlights != light) {
		GROUPEDITSPEC ges;
		static WORD vtxofs = 11;
		static const DWORD nvtx = 2;
		static WORD vidx[nvtx] = {vtxofs,vtxofs+1};
		static float v[2] = {0.2427f,0.3003f};
		NTVERTEX vtx[nvtx];
		for (DWORD i = 0; i < nvtx; i++)
			vtx[i].tv = v[(showlights ? 1:0)];
		ges.flags = GRPEDIT_VTXTEXV;
		ges.Vtx = vtx;
		ges.vIdx = vidx;
		ges.nVtx = nvtx;
		oapiEditMeshGroup (hMesh, GRP_LIT_SURFACES_VC, &ges);
		light = showlights;
	}
	return false;
}
