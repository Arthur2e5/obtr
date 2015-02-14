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

HoverCtrlDial::HoverCtrlDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void HoverCtrlDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 180;
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

bool HoverCtrlDial::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int mode = dg->GetHoverMode();
	dg->SetAnimation (dg->anim_hoverdial, mode*0.5);
	return false;
}

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
	DeltaGlider *dg = (DeltaGlider*)vessel;
	DWORD mode = dg->GetHoverMode();

	if (p.x < 0.5) { // dial turn left
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
	int i, j, ofs;
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
	const VECTOR3 &cnt = vc_hvrind_cnt;
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
			z = i < 4 ? -0.0005f : -0.001f;
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

PHoverCtrl::PHoverCtrl (VESSEL3 *v): PanelElement (v)
{
	vc_state = 0;
	for (int i = 0; i < nvtx_per_switch; i++)
		vperm[i] = (WORD)(i+nvtx_per_switch*4);
}

// ==============================================================

void PHoverCtrl::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 192;
}

// ==============================================================

void PHoverCtrl::ResetVC (DEVMESHHANDLE hMesh)
{
	GROUPREQUESTSPEC grs;
	memset (&grs, 0, sizeof(GROUPREQUESTSPEC));
	grs.Vtx = vtx0;
	grs.nVtx = nvtx_per_switch;
	grs.VtxPerm = vperm;
	oapiGetMeshGroup (hMesh, GRP_SWITCHES1_VC, &grs);
}

// ==============================================================

bool PHoverCtrl::Redraw2D (SURFHANDLE surf)
{
	int j, state;
	state = ((DeltaGlider*)vessel)->hpswitch;
	for (j = 0; j < 4; j++)
		grp->Vtx[vtxofs+j].tu = (1053.5f+state*16+(j%2)*15)/texw;
	return false;
}

// ==============================================================

bool PHoverCtrl::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const VECTOR3 &ref = vc_hpswitch_ref;
	static const double tilt[3] = {0,15*RAD,-15*RAD};

	int j, state;
	state = ((DeltaGlider*)vessel)->hpswitch;
	if (state == vc_state) return false; // nothing to do
	vc_state = state;

	NTVERTEX vtx[nvtx_per_switch];
	memcpy (vtx, vtx0, nvtx_per_switch*sizeof(NTVERTEX));

	if (state) {
		MATRIX3 R = rotm(vc_hpswitch_axis,tilt[state]);
		for (j = 0; j < nvtx_per_switch; j++) {
			VECTOR3 v = {vtx[j].x-ref.x, vtx[j].y-ref.y, vtx[j].z-ref.z};
			VECTOR3 vr = mul(R,v);
			vtx[j].x = (float)(vr.x + ref.x);
			vtx[j].y = (float)(vr.y + ref.y);
			vtx[j].z = (float)(vr.z + ref.z);
			VECTOR3 n = {vtx[j].nx, vtx[j].ny, vtx[j].nz};
			VECTOR3 nr = mul(R,n);
			vtx[j].nx = (float)nr.x;
			vtx[j].ny = (float)nr.y;
			vtx[j].nz = (float)nr.z;
		}
	}

	static const int grpid = GRP_SWITCHES1_VC;
	GROUPEDITSPEC ges = {GRPEDIT_VTXCRD|GRPEDIT_VTXNML,0,vtx,nvtx_per_switch,vperm};
	oapiEditMeshGroup (hMesh, grpid, &ges);
	return false;
}

// ==============================================================

bool PHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (my <  22) mode = 2;
		else               mode = 1;
		ctrl = 1;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	return ((DeltaGlider*)vessel)->IncPHover (ctrl, mode);
}

// ==============================================================

bool PHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (p.y < 0.5 ) mode = 1;
		else                 mode = 2;
		ctrl = 1;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	((DeltaGlider*)vessel)->IncPHover (ctrl, mode);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================
// ==============================================================

RHoverCtrl::RHoverCtrl (VESSEL3 *v): PanelElement (v)
{
	vc_state = 0;
	for (int i = 0; i < nvtx_per_switch; i++)
		vperm[i] = (WORD)(i+nvtx_per_switch*5);
}

// ==============================================================

void RHoverCtrl::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 196;
}

// ==============================================================

void RHoverCtrl::ResetVC (DEVMESHHANDLE hMesh)
{
	GROUPREQUESTSPEC grs;
	memset (&grs, 0, sizeof(GROUPREQUESTSPEC));
	grs.Vtx = vtx0;
	grs.nVtx = nvtx_per_switch;
	grs.VtxPerm = vperm;
	oapiGetMeshGroup (hMesh, GRP_SWITCHES1_VC, &grs);
}

// ==============================================================

bool RHoverCtrl::Redraw2D (SURFHANDLE surf)
{
	int j, state;
	state = ((DeltaGlider*)vessel)->hrswitch;
	for (j = 0; j < 4; j++)
		grp->Vtx[vtxofs+j].tu = (1053.5+state*16+(j%2)*15)/texw;
	return false;
}

// ==============================================================

bool RHoverCtrl::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const VECTOR3 &ref = vc_hrswitch_ref;
	static const double tilt[3] = {0,15*RAD,-15*RAD};

	int j, state;
	state = ((DeltaGlider*)vessel)->hrswitch;
	if (state == vc_state) return false; // nothing to do
	vc_state = state;

	NTVERTEX vtx[nvtx_per_switch];
	memcpy (vtx, vtx0, nvtx_per_switch*sizeof(NTVERTEX));

	if (state) {
		MATRIX3 R = rotm(vc_hrswitch_axis,tilt[state]);
		for (j = 0; j < nvtx_per_switch; j++) {
			VECTOR3 v = {vtx[j].x-ref.x, vtx[j].y-ref.y, vtx[j].z-ref.z};
			VECTOR3 vr = mul(R,v);
			vtx[j].x = (float)(vr.x + ref.x);
			vtx[j].y = (float)(vr.y + ref.y);
			vtx[j].z = (float)(vr.z + ref.z);
			VECTOR3 n = {vtx[j].nx, vtx[j].ny, vtx[j].nz};
			VECTOR3 nr = mul(R,n);
			vtx[j].nx = (float)nr.x;
			vtx[j].ny = (float)nr.y;
			vtx[j].nz = (float)nr.z;
		}
	}

	static const int grpid = GRP_SWITCHES1_VC;
	GROUPEDITSPEC ges = {GRPEDIT_VTXCRD|GRPEDIT_VTXNML,0,vtx,nvtx_per_switch,vperm};
	oapiEditMeshGroup (hMesh, grpid, &ges);
	return false;
}

// ==============================================================

bool RHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (mx <  22) mode = 1;
		else               mode = 2;
		ctrl = 1;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	return ((DeltaGlider*)vessel)->IncRHover (ctrl, mode);
}

// ==============================================================

bool RHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (p.y < 0.5 ) mode = 1;
		else                 mode = 2;
		ctrl = 1;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	((DeltaGlider*)vessel)->IncRHover (ctrl, mode);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

