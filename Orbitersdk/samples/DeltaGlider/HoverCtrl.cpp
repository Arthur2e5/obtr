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

// --------------------------------------------------------------

void HoverCtrlDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 180;
}

// --------------------------------------------------------------

void HoverCtrlDial::ResetVC (DEVMESHHANDLE hMesh)
{
	int mode = ((DeltaGlider*)vessel)->GetHoverMode();
	SetPosition (mode);
}

// --------------------------------------------------------------

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

// --------------------------------------------------------------

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

// --------------------------------------------------------------

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

HoverDisp::HoverDisp (VESSEL3 *v): PanelElement (v)
{
	pofs_cur = rofs_cur = 0;
	pofs_cmd = rofs_cmd = 0;
	memset (&vc_grp, 0, sizeof(GROUPREQUESTSPEC));
	for (int i = 0; i < 8; i++)
		vperm[i] = (WORD)(i+16);
}

// --------------------------------------------------------------

HoverDisp::~HoverDisp ()
{
	if (vc_grp.Vtx) delete []vc_grp.Vtx;
}

// --------------------------------------------------------------

void HoverDisp::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 184;
}

// --------------------------------------------------------------

void HoverDisp::ResetVC (DEVMESHHANDLE hMesh)
{
	vc_grp.nVtx = 8;
	if (!vc_grp.Vtx) vc_grp.Vtx = new NTVERTEX[vc_grp.nVtx];
	if (oapiGetMeshGroup (hMesh, GRP_VC_INSTR_VC, &vc_grp) != 0) { // problems
		delete []vc_grp.Vtx;
		vc_grp.Vtx = 0;
	}
}

// --------------------------------------------------------------

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

// --------------------------------------------------------------

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

PHoverCtrl::PHoverCtrl (VESSEL3 *v)
: DGSwitch2 (v)
{
}

// --------------------------------------------------------------

bool PHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncPHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// --------------------------------------------------------------

bool PHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncPHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================

RHoverCtrl::RHoverCtrl (VESSEL3 *v)
: DGSwitch2 (v)
{
}

// --------------------------------------------------------------

bool RHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncRHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// --------------------------------------------------------------

bool RHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int ctrl = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		ctrl = (int)GetState();
	((DeltaGlider*)vessel)->IncRHover (ctrl);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================
// ==============================================================

const int HoverHoldAltControl::AID_HOLDALT_DISP = 0;
const int HoverHoldAltControl::AID_HOLDALT_BTN = 1;
const int HoverHoldAltControl::AID_HOLDALT_SELECT = 2;
const int HoverHoldAltControl::AID_HOLDALT_SETCUR = 3;

// --------------------------------------------------------------

HoverHoldAltControl::HoverHoldAltControl (DeltaGlider *vessel, int ident)
: DGSubSystem(vessel, ident)
{
	extern GDIParams g_Param;

	holdalt   = 0.0;
	active = false;
	altmode = VESSEL::ALTMODE_MEANRAD;
	holdmode = HOLDMODE_ALT;

	nelement = 4;
	element = new PanelElement*[nelement];
	element[AID_HOLDALT_DISP] = new HoverHoldAltIndicator(this, g_Param.surf);
	element[AID_HOLDALT_BTN] = new HoverAltBtn (this);
	element[AID_HOLDALT_SELECT] = new HoverAltSwitch (this);
	element[AID_HOLDALT_SETCUR] = new HoverAltCurBtn (this);
}

// --------------------------------------------------------------

void HoverHoldAltControl::Activate (bool ison)
{
	if (ison != active) {
		active = ison;

		if (active) DG()->SetHoverHoldAltitude (holdalt, altmode);
		else        DG()->DeactivateNavmode (NAVMODE_HOLDALT);

		((HoverAltBtn*)element[AID_HOLDALT_BTN])->SetState(active ? DGButton3::ON : DGButton3::OFF);
		oapiTriggerRedrawArea (0, 0, (Id()+1)*1000+AID_HOLDALT_BTN);
	}
}

// --------------------------------------------------------------

void HoverHoldAltControl::SetTargetAlt (double alt)
{
	holdalt = alt;
	if (active) DG()->SetHoverHoldAltitude (holdalt, altmode);
}

// --------------------------------------------------------------

void HoverHoldAltControl::SetTargetAltCurrent ()
{
	SetTargetAlt (DG()->GetAltitude (altmode));
}

// --------------------------------------------------------------

void HoverHoldAltControl::clbkPostStep (double simt, double simdt, double mjd)
{
}

// --------------------------------------------------------------

bool HoverHoldAltControl::clbkLoadVC (int vcid)
{
	switch (vcid) {
	case 0: // VC pilot position
		// readouts
		oapiVCRegisterArea (Global(AID_HOLDALT_DISP), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// Hover hold alt button
		oapiVCRegisterArea (Global(AID_HOLDALT_BTN), PANEL_REDRAW_MOUSE | PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Spherical (Global(AID_HOLDALT_BTN), VC_BTN_HOVER_HOLDALT_ref, VC_BTN_HOVER_HOLDALT_mouserad);
		((DGButton3*)element[AID_HOLDALT_BTN])->DefineAnimationVC (VC_BTN_HOVER_HOLDALT_axis, GRP_BUTTON3_VC, GRP_LIT_SURF_VC, VC_BTN_HOVER_HOLDALT_vofs, VC_BTN_HOVER_HOLDALT_LABEL_vofs);

		// Hover hold alt select switch
		oapiVCRegisterArea (Global(AID_HOLDALT_SELECT), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (Global(AID_HOLDALT_SELECT), VC_HOVER_HOLDALT_SWITCH_mousearea[0], VC_HOVER_HOLDALT_SWITCH_mousearea[1], VC_HOVER_HOLDALT_SWITCH_mousearea[2], VC_HOVER_HOLDALT_SWITCH_mousearea[3]);
		((DGSwitch2*)element[AID_HOLDALT_SELECT])->DefineAnimationVC (VC_HOVER_HOLDALT_SWITCH_ref, VC_HOVER_HOLDALT_SWITCH_axis, GRP_SWITCH2_VC, VC_HOVER_HOLDALT_SWITCH_vofs);

		// Hover hold alt set current button
		oapiVCRegisterArea (Global(AID_HOLDALT_SETCUR), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Spherical (Global(AID_HOLDALT_SETCUR), VC_BTN_HOVER_HOLDALT_CUR_ref, VC_BTN_HOVER_HOLDALT_CUR_mouserad);
		((DGButton2*)element[AID_HOLDALT_SETCUR])->DefineAnimationVC (VC_BTN_HOVER_HOLDALT_CUR_axis, GRP_BUTTON2_VC, VC_BTN_HOVER_HOLDALT_CUR_vofs);

		break;
	}
	return true;
}

// ==============================================================

HoverAltBtn::HoverAltBtn (HoverHoldAltControl *hhac)
: DGButton3 (hhac->DG()), ctrl(hhac)
{
}

// --------------------------------------------------------------

bool HoverAltBtn::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGButton3::ProcessMouseVC (event, p)) {
		DGButton3::State state = GetState();
		if      (state == DGButton3::OFF) ctrl->Activate (false);
		else if (state == DGButton3::ON)  ctrl->Activate (true);
	}
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================

HoverAltSwitch::HoverAltSwitch (HoverHoldAltControl *hhac)
: DGSwitch2(hhac->DG()), ctrl(hhac)
{
}

// --------------------------------------------------------------

bool HoverAltSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int state = 0;
	static double refT = 0.0;
	if (event & PANEL_MOUSE_LBDOWN)
		refT = oapiGetSysTime();

	if (DGSwitch2::ProcessMouseVC (event, p))
		state = (int)GetState();

	if (state) {
		double alt = ctrl->TargetAlt();
		double t = oapiGetSysTime();
		double dt = oapiGetSysStep();
		double downt = t-refT;
		double dalt = dt * max(fabs(alt),1.0);
		if (downt < 10.0) dalt *= 1e-6 + downt*(1.0-1e-6)/10.0;
		if (state == 1) dalt = -dalt;
		ctrl->SetTargetAlt (alt + dalt);
	}

	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================

HoverAltCurBtn::HoverAltCurBtn (HoverHoldAltControl *hhac)
: DGButton2(hhac->DG()), ctrl(hhac)
{
}

// --------------------------------------------------------------

bool HoverAltCurBtn::ProcessMouseVC (int event, VECTOR3 &p)
{
	DGButton2::ProcessMouseVC (event, p);
	if (event & PANEL_MOUSE_LBDOWN)
		ctrl->SetTargetAltCurrent ();
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================

HoverHoldAltIndicator::HoverHoldAltIndicator (HoverHoldAltControl *hhac, SURFHANDLE blitsrc)
: PanelElement (hhac->DG()), ctrl(hhac), bsrc(blitsrc)
{
	btgt = 0;
}

// --------------------------------------------------------------

void HoverHoldAltIndicator::ResetVC (DEVMESHHANDLE hMesh)
{
	btgt = oapiGetTextureHandle (ctrl->DG()->vcmesh_tpl, 14);
	strcpy (holdstr, "         ");
	holdmode_disp = HoverHoldAltControl::HOLDMODE_NONE;
	hold_disp = false;
}

// --------------------------------------------------------------

bool HoverHoldAltIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE hSurf)
{
	bool refresh = false;
	if (!btgt) return refresh;

	char cbuf[10];
	FormatValue (cbuf, 10, ctrl->holdalt, 4);
	if (strncmp (cbuf, holdstr, 10)) {
		UpdateReadout (cbuf, holdstr);
		refresh = true;
	}

	if (holdmode_disp != ctrl->holdmode) {
		oapiBlt (btgt, bsrc, 314, 2, (ctrl->holdmode-1)*25, 14, 25, 8);
		holdmode_disp = ctrl->holdmode;
		refresh = true;
	}

	if (hold_disp != ctrl->active) {
		oapiBlt (btgt, bsrc, 348, 2, ctrl->active ? 51:78, 14, 27, 8);
		hold_disp = ctrl->active;
		refresh = true;
	}
	return refresh;
}

// --------------------------------------------------------------

void HoverHoldAltIndicator::UpdateReadout (const char *tgtstr, char *curstr)
{
	int tgtx = 314;
	int tgty = 12;
	const int srcy = 0;
	int srcx;
	const int w = 8;
	const int h = 11;
	char c;
	int i;
	for (i = 0; i < 10 && tgtstr[i]; i++) {
		if ((c=tgtstr[i]) != curstr[i]) {
			if (c >= '0' && c <= '9') srcx = (c-'0')*8;
			else switch(c) {
				case '.': srcx = 10*8; break;
				case '+': srcx = 11*8; break;
				case '-': srcx = 12*8; break;
				case 'k': srcx = 13*8; break;
				case 'M': srcx = 14*8; break;
				case 'G': srcx = 15*8; break;
				default:  srcx = 16*8; break;
			}
			oapiBlt (btgt, bsrc, tgtx, tgty, srcx, srcy, w, h);
			curstr[i] = c;
		}
		tgtx += w;
	}
	if (i < 10) curstr[i] = '\0';
}
