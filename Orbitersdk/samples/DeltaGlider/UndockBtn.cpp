// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// UndockBtn.cpp
// "Dock release" button interface
// ==============================================================

#define STRICT 1
#include "UndockBtn.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"

// constants for texture coordinates
static const float texh = (float)PANEL2D_TEXH; // texture height
static const float tx_y0 = texh-614.5f; // top edge of texture block
static const float tx_dy = 39.0f;       // texture block height
static const float bb_y0 =  471.5f;     // top edge of button block

// ==============================================================

UndockButton::UndockButton (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void UndockButton::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 100;
	btndown = false;
}

// ==============================================================

bool UndockButton::Redraw2D (SURFHANDLE surf)
{
	float y = (btndown ? bb_y0+tx_dy : bb_y0);
	float tv = (btndown ? tx_y0+tx_dy : tx_y0)/texh;
	grp->Vtx[vtxofs+2].y = grp->Vtx[vtxofs+3].y = y;
	grp->Vtx[vtxofs+2].tv = grp->Vtx[vtxofs+3].tv = tv;
	return false;
}

// ==============================================================

bool UndockButton::ProcessMouse2D (int event, int mx, int my)
{
	if (event & PANEL_MOUSE_LBDOWN) vessel->Undock (0);
	btndown = (event == PANEL_MOUSE_LBDOWN);
	return true;
}

// ==============================================================

bool UndockButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	dg->ActivateUndocking (event & PANEL_MOUSE_LBDOWN ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
	return false;
}