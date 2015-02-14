// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// GimbalCtrl.cpp
// Gimbal controls and displays
// ==============================================================

#define STRICT 1
#include "GimbalCtrl.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width
static const float texh = (float)PANEL2D_TEXH; // texture height
static const float tx_x0 = 1147.5f;            // left edge of texture block
static const float tx_y0 = texh-614.5f;        // top edge of texture block
static const float tx_dx = 7.0f;               // texture block width
static const float tx_dy = 6.0f;               // texture block height
// constants for panel coordinates
static const float bb_dx =  7.0f;
static const float bb_dy =  3.0f;
static const float pm_x0 = 27.0f;       // left edge of button block
static const float pm_y0 = 103.5f;      // top edge of button block
static const float sc_y0 = 431.5f;


// ==============================================================

MainGimbalDial::MainGimbalDial (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

void MainGimbalDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 144;
}

// ==============================================================

bool MainGimbalDial::Redraw2D (SURFHANDLE surf)
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
	float dtu = (float)(dg->GetMainGimbalMode()*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// ==============================================================

bool MainGimbalDial::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int mode = dg->GetMainGimbalMode();
	dg->SetAnimation (dg->anim_gimbaldial, mode*0.5);
	return false;
}

// ==============================================================

bool MainGimbalDial::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int mode = dg->GetMainGimbalMode();

	if (mx < 20) { // dial turn left
		if (mode > 0) {
			dg->SetMainGimbalMode (mode-1);
			return true;
		}
	} else { // dial turn right
		if (mode < 2) {
			dg->SetMainGimbalMode (mode+1);
			return true;
		}
	}
	return false;
}

// ==============================================================

bool MainGimbalDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	DWORD mode = dg->GetMainGimbalMode();

	if (p.x < 0.5) { // dial turn left
		if (mode > 0) {
			dg->SetMainGimbalMode (mode-1);
			return true;
		}
	} else { // dial turn right
		if (mode < 2) {
			dg->SetMainGimbalMode (mode+1);
			return true;
		}
	}
	return false;
}

// ==============================================================
// ==============================================================

MainGimbalDisp::MainGimbalDisp (VESSEL3 *v): PanelElement (v)
{
	for (int i = 0; i < 2; i++) {
		pofs_cur[i] = yofs_cur[i] = 0;
		pofs_cmd[i] = yofs_cmd[i] = 0;
	}
	memset (&vc_grp, 0, sizeof(GROUPREQUESTSPEC));
}

// ==============================================================

MainGimbalDisp::~MainGimbalDisp ()
{
	if (vc_grp.Vtx) delete []vc_grp.Vtx;
}

// ==============================================================

void MainGimbalDisp::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 148;
}

// ==============================================================

void MainGimbalDisp::ResetVC (DEVMESHHANDLE hMesh)
{
	vc_grp.nVtx = 16;
	if (!vc_grp.Vtx) vc_grp.Vtx = new NTVERTEX[vc_grp.nVtx];
	if (oapiGetMeshGroup (hMesh, GRP_VC_INSTR_VC, &vc_grp) != 0) { // problems
		delete []vc_grp.Vtx;
		vc_grp.Vtx = 0;
	}
}

// ==============================================================

bool MainGimbalDisp::Redraw2D (SURFHANDLE surf)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int i, j, ofs;
	double g;
	const float x0 =  28.5f;
	const float xx =  42.0f;
	const float y0 = 135.5f;
	const float dx =  10.0f;
	const float dy =  10.0f;

	for (i = 0; i < 2; i++) {
		g = dg->MainPGimbal(i);
		ofs = (int)floor((g/MAIN_PGIMBAL_RANGE)*18+0.5);
		if (ofs != pofs_cur[i]) {
			for (j = 0; j < 4; j++)
				grp->Vtx[vtxofs+4*i+j].y = y0 + dy*(j/2) + ofs;
			pofs_cur[i] = ofs;
		}
		g = dg->MainYGimbal(i);
		ofs = (int)floor((g/MAIN_YGIMBAL_RANGE)*18+0.5);
		if (ofs != yofs_cur[i]) {
			for (j = 0; j < 4; j++)
				grp->Vtx[vtxofs+4*i+j].x = x0 + i*xx + dx*(j%2) - ofs;
			yofs_cur[i] = ofs;
		}
	}

	for (i = 0; i < 2; i++) {
		g = dg->MainPGimbal(i, false);
		ofs = (int)floor((g/MAIN_PGIMBAL_RANGE)*18+0.5);
		if (ofs != pofs_cmd[i]) {
			for (j = 0; j < 4; j++)
				grp->Vtx[vtxofs+8+4*i+j].y = y0 + dy*(j/2) + ofs;
			pofs_cmd[i] = ofs;
		}
		g = dg->MainYGimbal(i, false);
		ofs = (int)floor((g/MAIN_YGIMBAL_RANGE)*18+0.5);
		if (ofs != yofs_cmd[i]) {
			for (j = 0; j < 4; j++)
				grp->Vtx[vtxofs+8+4*i+j].x = x0 + i*xx + dx*(j%2) - ofs;
			yofs_cmd[i] = ofs;
		}
	}

	return false;
}

// ==============================================================

bool MainGimbalDisp::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const double &slope = vc_lpanel_tilt;
	const VECTOR3 (&cnt)[2] = vc_gimind_cnt;
	static const double cosa = cos(slope), sina = sin(slope);
	static const double indsize = 0.002586;
	static const double xrange = 0.0103/MAIN_YGIMBAL_RANGE;
	static const double yrange = 0.0103/MAIN_PGIMBAL_RANGE;

	DeltaGlider *dg = (DeltaGlider*)vessel;
	NTVERTEX *Vtx = vc_grp.Vtx;
	if (hMesh && Vtx) {
		int i, j;
		double dx, dy;
		float y, z;
		for (i = 0; i < 2; i++) {
			dx = -dg->MainYGimbal(i)*xrange;
			dy = -dg->MainPGimbal(i)*yrange;
			for (j = 0; j < 4; j++) {
				Vtx[4+i*8+j].x = cnt[i].x + dx + indsize*(j%2 ? 1:-1);
				Vtx[4+i*8+j].y = dy + indsize*(j/2 ? 1:-1);
			}
			dx = -dg->MainYGimbal(i,false)*xrange;
			dy = -dg->MainPGimbal(i,false)*yrange;
			for (j = 0; j < 4; j++) {
				Vtx[i*8+j].x = cnt[i].x + dx + indsize*(j%2 ? 1:-1);
				Vtx[i*8+j].y = dy + indsize*(j/2 ? 1:-1);
			}
		}
		for (i = 0; i < 16; i++) {
			y = Vtx[i].y;
			z = (i%8) < 4 ? -0.0005f : -0.001f;
			Vtx[i].y = (float)(cnt[0].y + y*cosa - z*sina);
			Vtx[i].z = (float)(cnt[0].z + y*sina + z*cosa);
		}
		GROUPEDITSPEC ges;
		ges.flags = GRPEDIT_VTXCRD;
		ges.nVtx = vc_grp.nVtx;
		ges.Vtx  = vc_grp.Vtx;
		ges.vIdx = 0;
		oapiEditMeshGroup (hMesh, GRP_VC_INSTR_VC, &ges);

	}
	return false;
}

// ==============================================================
// ==============================================================

PMainGimbalCtrl::PMainGimbalCtrl (VESSEL3 *v): PanelElement (v)
{
	for (int i = 0; i < 2; i++)
		vc_state[i] = 0;
}

// ==============================================================

void PMainGimbalCtrl::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 164;
}

// ==============================================================

void PMainGimbalCtrl::ResetVC (DEVMESHHANDLE hMesh)
{
	GROUPREQUESTSPEC grs;
	memset (&grs, 0, sizeof(GROUPREQUESTSPEC));
	grs.Vtx = vtx0;
	grs.nVtx = nvtx_per_switch*2;
	oapiGetMeshGroup (hMesh, GRP_SWITCHES1_VC, &grs);
}

// ==============================================================

bool PMainGimbalCtrl::Redraw2D (SURFHANDLE surf)
{
	int i, j, state;
	for (i = 0; i < 2; i++) {
		state = ((DeltaGlider*)vessel)->mpswitch[i];
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+i*4+j].tu = (1053.5f+state*16+(j%2)*15)/texw;
	}
	return false;
}

// ==============================================================

bool PMainGimbalCtrl::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const VECTOR3 &ref = vc_gpswitch_ref;
	static const double tilt[3] = {0,15*RAD,-15*RAD};

	int i, j, ofs, state;
	bool redraw = false;
	for (i = 0; i < 2; i++) {
		state = ((DeltaGlider*)vessel)->mpswitch[i];
		if (state != vc_state[i]) {
			vc_state[i] = state;
			redraw = true;
		}
	}
	if (!redraw) return false;

	NTVERTEX vtx[nvtx_per_switch*2];
	memcpy (vtx, vtx0, nvtx_per_switch*2*sizeof(NTVERTEX));

	for (i = 0; i < 2; i++) {
		ofs = i*nvtx_per_switch;
		state = vc_state[i];
		if (!state) continue;
		MATRIX3 R = rotm(vc_gpswitch_axis,tilt[state]);
		for (j = 0; j < nvtx_per_switch; j++) {
			VECTOR3 v = {vtx[ofs+j].x-ref.x, vtx[ofs+j].y-ref.y, vtx[ofs+j].z-ref.z};
			VECTOR3 vr = mul(R,v);
			vtx[ofs+j].x = (float)(vr.x + ref.x);
			vtx[ofs+j].y = (float)(vr.y + ref.y);
			vtx[ofs+j].z = (float)(vr.z + ref.z);
			VECTOR3 n = {vtx[ofs+j].nx, vtx[ofs+j].ny, vtx[ofs+j].nz};
			VECTOR3 nr = mul(R,n);
			vtx[ofs+j].nx = (float)nr.x;
			vtx[ofs+j].ny = (float)nr.y;
			vtx[ofs+j].nz = (float)nr.z;
		}
	}

	static const int grpid = GRP_SWITCHES1_VC;
	GROUPEDITSPEC ges = {GRPEDIT_VTXCRD|GRPEDIT_VTXNML,0,vtx,nvtx_per_switch*2,0};
	oapiEditMeshGroup (hMesh, grpid, &ges);
	return false;
}

// ==============================================================

bool PMainGimbalCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (mx <  10) ctrl = 1;
		else if (mx >= 25) ctrl = 2;
		else               ctrl = 3;
		if      (my <  22) mode = 2;
		else               mode = 1;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	return ((DeltaGlider*)vessel)->IncMainPGimbal (ctrl, mode);
}

// ==============================================================

bool PMainGimbalCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (p.x < 0.25) ctrl = 1;
		else if (p.x > 0.75) ctrl = 2;
		else                 ctrl = 3;
		if      (p.y < 0.5 ) mode = 1;
		else                 mode = 2;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	((DeltaGlider*)vessel)->IncMainPGimbal (ctrl, mode);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================
// ==============================================================

YMainGimbalCtrl::YMainGimbalCtrl (VESSEL3 *v): PanelElement (v)
{
	int i;
	for (i = 0; i < 2; i++)
		vc_state[i] = 0;
	for (i = 0; i < nvtx_per_switch*2; i++)
		vperm[i] = (WORD)(i+nvtx_per_switch*2);
}

// ==============================================================

void YMainGimbalCtrl::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 172;
}

// ==============================================================

void YMainGimbalCtrl::ResetVC (DEVMESHHANDLE hMesh)
{
	GROUPREQUESTSPEC grs;
	memset (&grs, 0, sizeof(GROUPREQUESTSPEC));
	grs.Vtx = vtx0;
	grs.nVtx = nvtx_per_switch*2;
	grs.VtxPerm = vperm;
	oapiGetMeshGroup (hMesh, GRP_SWITCHES1_VC, &grs);
}

// ==============================================================

bool YMainGimbalCtrl::Redraw2D (SURFHANDLE surf)
{
	int i, j, state;
	for (i = 0; i < 2; i++) {
		state = ((DeltaGlider*)vessel)->myswitch[i];
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+i*4+j].tu = (1053.5+state*16+(j%2)*15)/texw;
	}
	return false;
}

// ==============================================================

bool YMainGimbalCtrl::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	const VECTOR3 &ref = vc_gyswitch_ref;
	static const double tilt[3] = {0,15*RAD,-15*RAD};

	int i, j, ofs, state;
	bool redraw = false;
	for (i = 0; i < 2; i++) {
		state = ((DeltaGlider*)vessel)->myswitch[i];
		if (state != vc_state[i]) {
			vc_state[i] = state;
			redraw = true;
		}
	}
	if (!redraw) return false;

	NTVERTEX vtx[nvtx_per_switch*2];
	memcpy (vtx, vtx0, nvtx_per_switch*2*sizeof(NTVERTEX));

	for (i = 0; i < 2; i++) {
		ofs = i*nvtx_per_switch;
		state = vc_state[i];
		if (!state) continue;
		MATRIX3 R = rotm(vc_gyswitch_axis,tilt[state]);
		for (j = 0; j < nvtx_per_switch; j++) {
			VECTOR3 v = {vtx[ofs+j].x-ref.x, vtx[ofs+j].y-ref.y, vtx[ofs+j].z-ref.z};
			VECTOR3 vr = mul(R,v);
			vtx[ofs+j].x = (float)(vr.x + ref.x);
			vtx[ofs+j].y = (float)(vr.y + ref.y);
			vtx[ofs+j].z = (float)(vr.z + ref.z);
			VECTOR3 n = {vtx[ofs+j].nx, vtx[ofs+j].ny, vtx[ofs+j].nz};
			VECTOR3 nr = mul(R,n);
			vtx[ofs+j].nx = (float)nr.x;
			vtx[ofs+j].ny = (float)nr.y;
			vtx[ofs+j].nz = (float)nr.z;
		}
	}

	static const int grpid = GRP_SWITCHES1_VC;
	GROUPEDITSPEC ges = {GRPEDIT_VTXCRD|GRPEDIT_VTXNML,0,vtx,nvtx_per_switch*2,vperm};
	oapiEditMeshGroup (hMesh, grpid, &ges);
	return false;
}

// ==============================================================

bool YMainGimbalCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (my <  10) ctrl = 1;
		else if (my >= 25) ctrl = 2;
		else               ctrl = 3;
		if      (mx <  22) mode = 1;
		else               mode = 2;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	return ((DeltaGlider*)vessel)->IncMainYGimbal (ctrl, mode);
}

// ==============================================================

bool YMainGimbalCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0, mode = 0;
	if (event & PANEL_MOUSE_LBDOWN) {
		if      (p.x < 0.25) ctrl = 1;
		else if (p.x > 0.75) ctrl = 2;
		else                 ctrl = 3;
		if      (p.y < 0.5 ) mode = 1;
		else                 mode = 2;
	} else if (event & PANEL_MOUSE_LBUP) {
		ctrl = 0;
	}
	((DeltaGlider*)vessel)->IncMainYGimbal (ctrl, mode);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}
