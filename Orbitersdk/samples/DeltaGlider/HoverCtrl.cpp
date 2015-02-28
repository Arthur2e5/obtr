// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HoverCtrl.cpp
// Hover controls and displays
// ==============================================================

#define STRICT 1
#include "HoverCtrl.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width

// ==============================================================

HoverCtrlDial::HoverCtrlDial (VESSEL3 *v)
: DGDial1 (v, 3, -50*RAD, 50*RAD)
{
}

// ==============================================================

void HoverCtrlDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 180;
}

// ==============================================================

void HoverCtrlDial::ResetVC (DEVMESHHANDLE hMesh)
{
	int mode = ((DeltaGlider*)vessel)->GetHoverMode();
	SetPosition (mode);
}

// ==============================================================

bool HoverCtrlDial::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float texw = (float)PANEL2D_TEXW; // texture width
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_x0 = 1160.5f;            // left edge of texture block
	static const float tx_y0 = texh-615.5f;        // top edge of texture block
	static const float tx_dx = 39.0f;              // texture block width
	static const float tx_dy = 43.0f;              // texture block height
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};

	DeltaGlider *dg = (DeltaGlider*)vessel;
	float dtu = (float)(dg->GetHoverMode()*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// ==============================================================

//bool HoverCtrlDial::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
//{
//	DeltaGlider *dg = (DeltaGlider*)vessel;
//	int mode = dg->GetHoverMode();
//	dg->SetAnimation (dg->anim_hoverdial, mode*0.5);
//	return false;
//}

// ==============================================================

bool HoverCtrlDial::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int mode = dg->GetHoverMode();

	if (mx < 20) { // dial turn left
		if (mode > 0) {
			dg->SetHoverMode (mode-1);
			return true;
		}
	} else { // dial turn right
		if (mode < 2) {
			dg->SetHoverMode (mode+1);
			return true;
		}
	}
	return false;
}

// ==============================================================

bool HoverCtrlDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGDial1::ProcessMouseVC (event, p)) {
		int pos = GetPosition();
		((DeltaGlider*)vessel)->SetHoverMode (pos);
		return true;
	}
	return false;
}


// ==============================================================
// ==============================================================

HoverDisp::HoverDisp (VESSEL3 *v): PanelElement (v)
{
	pofs_cur = rofs_cur = 0;
	pofs_cmd = rofs_cmd = 0;
	memset (&vc_grp, 0, sizeof(GROUPREQUESTSPEC));
	for (int i = 0; i < 8; i++)
		vperm[i] = (WORD)(i+16);
}

// ==============================================================

HoverDisp::~HoverDisp ()
{
	if (vc_grp.Vtx) delete []vc_grp.Vtx;
}

// ==============================================================

void HoverDisp::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 184;
}

// ==============================================================

void HoverDisp::ResetVC (DEVMESHHANDLE hMesh)
{
	vc_grp.nVtx = 8;
	if (!vc_grp.Vtx) vc_grp.Vtx = new NTVERTEX[vc_grp.nVtx];
	if (oapiGetMeshGroup (hMesh, GRP_VC_INSTR_VC, &vc_grp) != 0) { // problems
		delete []vc_grp.Vtx;
		vc_grp.Vtx = 0;
	}
}

// ==============================================================

bool HoverDisp::Redraw2D (SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int j, ofs;
	double g;
	const float x0 =  47.5f;
	const float y0 = 346.5f;
	const float dx =  10.0f;
	const float dy =  10.0f;

	g = max(-PHOVER_RANGE, min(PHOVER_RANGE,dg->PHover()));
	ofs = (int)floor((g/PHOVER_RANGE)*18+0.5);
	if (ofs != pofs_cur) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+j].y = y0 + dy*(j/2) + ofs;
		pofs_cur = ofs;
	}
	g = max(-RHOVER_RANGE, min(RHOVER_RANGE,dg->RHover()));
	ofs = (int)floor((g/RHOVER_RANGE)*18+0.5);
	if (ofs != rofs_cur) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+j].x = x0 + dx*(j%2) - ofs;
		rofs_cur = ofs;
	}

	g = max(-PHOVER_RANGE, min(PHOVER_RANGE,dg->PHover(false)));
	ofs = (int)floor((g/PHOVER_RANGE)*18+0.5);
	if (ofs != pofs_cmd) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+4+j].y = y0 + dy*(j/2) + ofs;
		pofs_cmd = ofs;
	}
	g = max(-RHOVER_RANGE, min(RHOVER_RANGE,dg->RHover(false)));
	ofs = (int)floor((g/RHOVER_RANGE)*18+0.5);
	if (ofs != rofs_cmd) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+4+j].x = x0 + dx*(j%2) - ofs;
		rofs_cmd = ofs;
	}

	return false;
}

// ==============================================================

bool HoverDisp::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const double &slope = vc_lpanel_tilt;
	const VECTOR3 &cnt = VC_HOVER_INDICATOR_ref;
	static const double cosa = cos(slope), sina = sin(slope);
	static const double indsize = 0.002586;
	static const double xrange = 0.0103/RHOVER_RANGE;
	static const double yrange = 0.0103/PHOVER_RANGE;

	DeltaGlider *dg = (DeltaGlider*)vessel;
	NTVERTEX *Vtx = vc_grp.Vtx;
	if (hMesh && Vtx) {
		int i, j;
		double dx, dy;
		float y, z;
		dx = -max(-RHOVER_RANGE, min(RHOVER_RANGE,dg->RHover()))*xrange;
		dy = -max(-PHOVER_RANGE, min(PHOVER_RANGE,dg->PHover()))*yrange;
		for (j = 0; j < 4; j++) {
			Vtx[4+j].x = cnt.x + dx + indsize*(j%2 ? 1:-1);
			Vtx[4+j].y = dy + indsize*(j/2 ? 1:-1);
		}
		dx = -dg->RHover(false)*xrange;
		dy = -dg->PHover(false)*yrange;
		for (j = 0; j < 4; j++) {
			Vtx[j].x = cnt.x + dx + indsize*(j%2 ? 1:-1);
			Vtx[j].y = dy + indsize*(j/2 ? 1:-1);
		}
		for (i = 0; i < 8; i++) {
			y = Vtx[i].y;
			z = i < 4 ? -0.0002f : -0.0004f;
			Vtx[i].y = (float)(cnt.y + y*cosa - z*sina);
			Vtx[i].z = (float)(cnt.z + y*sina + z*cosa);
		}
		GROUPEDITSPEC ges = {GRPEDIT_VTXCRD,0,vc_grp.Vtx,vc_grp.nVtx,vperm};
		oapiEditMeshGroup (hMesh, GRP_VC_INSTR_VC, &ges);

	}
	return false;
}


// ==============================================================
// ==============================================================

PHoverCtrl::PHoverCtrl (VESSEL3 *v)
: DGSwitch2 (v)
{
}

// ==============================================================

bool PHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncPHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================

bool PHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncPHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================
// ==============================================================

RHoverCtrl::RHoverCtrl (VESSEL3 *v)
: DGSwitch2 (v)
{
}

// ==============================================================

bool RHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncRHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================

bool RHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncRHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

