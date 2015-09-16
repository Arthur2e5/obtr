// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HoverCtrl.cpp
// Classes for Hover balance control and Hover altitude subsystems
// ==============================================================

#define STRICT 1
#include "HoverCtrl.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// constants for texture coordinates
static const float texw = (float)PANEL2D_TEXW; // texture width

// ==============================================================
// ==============================================================

HoverBalanceControl::HoverBalanceControl (DeltaGlider *vessel, int ident)
: DGSubSystem (vessel, ident)
{
	mode = 0;
	phover = phover_cmd = 0.0;
	rhover = rhover_cmd = 0.0;

	ELID_MODEDIAL     = AddElement (modedial     = new HoverCtrlDial (this));
	ELID_PHOVERSWITCH = AddElement (phoverswitch = new PHoverCtrl (this));
	ELID_RHOVERSWITCH = AddElement (rhoverswitch = new RHoverCtrl (this));
	ELID_DISPLAY      = AddElement (hoverdisp    = new HoverDisp (this));
}

// --------------------------------------------------------------

bool HoverBalanceControl::IncPHover (int dir)
{
	if (dir && mode == 2) {
		const double cmd_speed = 0.5;
		double dcmd = oapiGetSimStep() * cmd_speed * PHOVER_RANGE * (dir == 1 ? -1.0:1.0);
		phover_cmd = min (PHOVER_RANGE, max (-PHOVER_RANGE, phover_cmd+dcmd));
	}
	return true;
}

// --------------------------------------------------------------

bool HoverBalanceControl::IncRHover (int dir)
{
	if (dir && mode == 2) {
		const double cmd_speed = 0.5;
		double dcmd = oapiGetSimStep() * cmd_speed * RHOVER_RANGE * (dir == 1 ? -1.0:1.0);
		rhover_cmd = min (RHOVER_RANGE, max (-RHOVER_RANGE, rhover_cmd+dcmd));
	}
	return true;
}

// --------------------------------------------------------------

void HoverBalanceControl::AutoHoverAtt ()
{
	double lvl;

	// Pitch command
	lvl = -DG()->GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ROTMODE);
	if (!lvl) lvl = DG()->GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ROTMODE);
	phover_cmd = lvl*PHOVER_RANGE;

	// Roll command
	lvl = -DG()->GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ROTMODE);
	if (!lvl) lvl = DG()->GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ROTMODE);
	rhover_cmd = lvl*RHOVER_RANGE;
}

// --------------------------------------------------------------

void HoverBalanceControl::TrackHoverAtt ()
{
	if (mode) {
		phover = DG()->GetPitch();
		rhover = DG()->GetBank();
		const double fb_scale = 3.0/4.55; // scaling between front and back hovers (distance from CG)
		double Lf = DG()->GetHoverThrusterLevel(0);
		double Ll = DG()->GetHoverThrusterLevel(1);
		double Lr = DG()->GetHoverThrusterLevel(2);
		double Lb = (Ll+Lr)*0.5;
		double Lm = (Lf+Lb)*0.5;
		double Tf = Lf;
		double Tb = Lb*fb_scale;
		double Tm = (Tf+Tb)*0.5;
		if (fabs(phover) <= MAX_AUTO_HOVER_ATT && fabs(rhover) <= MAX_AUTO_HOVER_ATT) { // within control range
			const double p_alpha = 0.2;
			const double p_beta = 0.6;
			const double r_alpha = 0.2;
			const double r_beta = 0.6;
			double dp = phover - phover_cmd;
			double dr = rhover - rhover_cmd;
			VECTOR3 avel;
			DG()->GetAngularVel(avel);
			double dpv =  avel.x;
			double drv = -avel.z;
			double balance_fb = -p_alpha*dp - p_beta*dpv;
			double balance_lr = -r_alpha*dr - r_beta*drv;
			if (Lf || Lb) {
				// front/back balance
				double Lf_cmd = Lm+balance_fb;
				double Lb_cmd = Lm-balance_fb;
				double D = (Lf_cmd-Lf + (Lb_cmd-Lb)*fb_scale)/(1.0+fb_scale);
				Lf_cmd -= D;
				Lb_cmd -= D;

				double Lmin = min (Lf_cmd, Lb_cmd);
				double Lmax = max (Lf_cmd, Lb_cmd);
				if (Lmin < 0.0) {
					if (Lf_cmd < 0.0) Lf_cmd = 0.0, Lb_cmd += Lmin/fb_scale;
					else              Lb_cmd = 0.0, Lf_cmd += Lmin*fb_scale;
				}
				if (Lmax > 1.0) {
					if (Lf_cmd > 1.0) Lf_cmd = 1.0, Lb_cmd += (Lmax-1.0)/fb_scale;
					else              Lb_cmd = 1.0, Lf_cmd += (Lmax-1.0)*fb_scale;
				}
				// left/right balance
				double Ll_cmd = Lb_cmd-balance_lr;
				double Lr_cmd = Lb_cmd+balance_lr;
				Lmin = min (Ll_cmd, Lr_cmd);
				Lmax = max (Ll_cmd, Lr_cmd);
				if (Lmin < 0.0) {
					if (Ll_cmd < 0.0) Ll_cmd = 0.0, Lr_cmd += Lmin;
					else              Lr_cmd = 0.0, Ll_cmd += Lmin;
				}
				if (Lmax > 1.0) {
					if (Ll_cmd > 1.0) Ll_cmd = 1.0, Lr_cmd += Lmax-1.0;
					else              Lr_cmd = 1.0, Ll_cmd += Lmax-1.0;
				}
				DG()->SetHoverThrusterLevel (0, Lf_cmd);
				DG()->SetHoverThrusterLevel (1, Ll_cmd);
				DG()->SetHoverThrusterLevel (2, Lr_cmd);
			}
		} else {
			double L_cmd = 2.0*Tm / (1.0 + fb_scale);
			for (int i = 0; i < 3; i++)
				DG()->SetHoverThrusterLevel (i, L_cmd);
		}
	} else {
		phover = rhover = phover_cmd = rhover_cmd = 0.0;
	}
	oapiTriggerRedrawArea (0, 0, GlobalElId (ELID_DISPLAY));
}

// --------------------------------------------------------------

void HoverBalanceControl::clbkPostStep (double simt, double simdt, double mjd)
{
	if (mode == 1) AutoHoverAtt();
	TrackHoverAtt();
}

// --------------------------------------------------------------

bool HoverBalanceControl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);

	// Hover control dial
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_MODEDIAL),     _R(32,280,72,324), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN, panel2dtex, modedial);

	// Hover manual switches
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_PHOVERSWITCH), _R(69,402,85,446), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP, panel2dtex, phoverswitch);
	phoverswitch->DefineAnimation2D (DGSwitch2::VERT, GRP_INSTRUMENTS_ABOVE_P0, 192);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_RHOVERSWITCH), _R( 9,415,53,431), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP, panel2dtex, rhoverswitch);
	rhoverswitch->DefineAnimation2D (DGSwitch2::HORZ, GRP_INSTRUMENTS_ABOVE_P0, 196);

	// Hover balance display
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_DISPLAY),      _R( 0,  0, 0,  0), PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, hoverdisp);

	return true;
}

// --------------------------------------------------------------

bool HoverBalanceControl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Hover control dial
	oapiVCRegisterArea (GlobalElId(ELID_MODEDIAL), PANEL_REDRAW_USER | PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_MODEDIAL), VC_HOVER_DIAL_mousearea[0], VC_HOVER_DIAL_mousearea[1], VC_HOVER_DIAL_mousearea[2], VC_HOVER_DIAL_mousearea[3]);
	modedial->DefineAnimationVC (VC_HOVER_DIAL_ref, VC_HOVER_DIAL_axis, GRP_DIAL1_VC, VC_HOVER_DIAL_vofs);

	// Hover manual switches
	oapiVCRegisterArea (GlobalElId(ELID_PHOVERSWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PHOVERSWITCH), VC_HOVER_PSWITCH_mousearea[0], VC_HOVER_PSWITCH_mousearea[1], VC_HOVER_PSWITCH_mousearea[2], VC_HOVER_PSWITCH_mousearea[3]);
	phoverswitch->DefineAnimationVC (VC_HOVER_PSWITCH_ref, VC_HOVER_PSWITCH_axis, GRP_SWITCH2_VC, VC_HOVER_PSWITCH_vofs);
	oapiVCRegisterArea (GlobalElId(ELID_RHOVERSWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_RHOVERSWITCH), VC_HOVER_RSWITCH_mousearea[0], VC_HOVER_RSWITCH_mousearea[1], VC_HOVER_RSWITCH_mousearea[2], VC_HOVER_RSWITCH_mousearea[3]);
	rhoverswitch->DefineAnimationVC (VC_HOVER_RSWITCH_ref, VC_HOVER_RSWITCH_axis, GRP_SWITCH2_VC, VC_HOVER_RSWITCH_vofs);

	// Hover balance display
	oapiVCRegisterArea (GlobalElId(ELID_DISPLAY), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	return true;
}

// --------------------------------------------------------------

void HoverBalanceControl::clbkReset2D (int panelid, MESHHANDLE hMesh)
{
	if (panelid != 0) return;
	DGSubSystem::clbkReset2D (panelid, hMesh);
}

// --------------------------------------------------------------

void HoverBalanceControl::clbkResetVC (int vcid, DEVMESHHANDLE hMesh)
{
	if (vcid != 0) return;
	DGSubSystem::clbkResetVC (vcid, hMesh);
}

// ==============================================================

HoverCtrlDial::HoverCtrlDial (HoverBalanceControl *hbc)
: DGDial1 (hbc->DG(), 3, -50*RAD, 50*RAD), ctrl(hbc)
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
	int mode = ctrl->Mode();
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

	float dtu = (float)(ctrl->Mode()*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// --------------------------------------------------------------

bool HoverCtrlDial::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int mode = ctrl->Mode();

	if (mx < 20) { // dial turn left
		if (mode > 0) {
			ctrl->SetMode (mode-1);
			return true;
		}
	} else { // dial turn right
		if (mode < 2) {
			ctrl->SetMode (mode+1);
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
		ctrl->SetMode (pos);
		return true;
	}
	return false;
}


// ==============================================================

HoverDisp::HoverDisp (HoverBalanceControl *hbc)
: PanelElement(hbc->DG()), ctrl(hbc)
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

	g = max (-PHOVER_RANGE, min(PHOVER_RANGE, ctrl->PHover()));
	ofs = (int)floor((g/PHOVER_RANGE)*18+0.5);
	if (ofs != pofs_cur) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+j].y = y0 + dy*(j/2) + ofs;
		pofs_cur = ofs;
	}
	g = max (-RHOVER_RANGE, min(RHOVER_RANGE, ctrl->RHover()));
	ofs = (int)floor((g/RHOVER_RANGE)*18+0.5);
	if (ofs != rofs_cur) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+j].x = x0 + dx*(j%2) - ofs;
		rofs_cur = ofs;
	}

	g = max (-PHOVER_RANGE, min(PHOVER_RANGE, ctrl->PHover(false)));
	ofs = (int)floor((g/PHOVER_RANGE)*18+0.5);
	if (ofs != pofs_cmd) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+4+j].y = y0 + dy*(j/2) + ofs;
		pofs_cmd = ofs;
	}
	g = max (-RHOVER_RANGE, min(RHOVER_RANGE, ctrl->RHover(false)));
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
		dx = -max (-RHOVER_RANGE, min(RHOVER_RANGE, ctrl->RHover()))*xrange;
		dy = -max (-PHOVER_RANGE, min(PHOVER_RANGE, ctrl->PHover()))*yrange;
		for (j = 0; j < 4; j++) {
			Vtx[4+j].x = cnt.x + dx + indsize*(j%2 ? 1:-1);
			Vtx[4+j].y = dy + indsize*(j/2 ? 1:-1);
		}
		dx = -ctrl->RHover(false)*xrange;
		dy = -ctrl->PHover(false)*yrange;
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

PHoverCtrl::PHoverCtrl (HoverBalanceControl *hbc)
: DGSwitch2(hbc->DG()), ctrl(hbc)
{
}

// --------------------------------------------------------------

bool PHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int state = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		state = (int)GetState();
	ctrl->IncPHover (state);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// --------------------------------------------------------------

bool PHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int state = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		state = (int)GetState();
	ctrl->IncPHover (state);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================

RHoverCtrl::RHoverCtrl (HoverBalanceControl *hbc)
: DGSwitch2(hbc->DG()), ctrl(hbc)
{
}

// --------------------------------------------------------------

bool RHoverCtrl::ProcessMouse2D (int event, int mx, int my)
{
	static int state = 0;
	if (DGSwitch2::ProcessMouse2D (event, mx, my))
		state = (int)GetState();
	ctrl->IncRHover (state);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// --------------------------------------------------------------

bool RHoverCtrl::ProcessMouseVC (int event, VECTOR3 &p)
{
	static int state = 0;
	if (DGSwitch2::ProcessMouseVC (event, p))
		state = (int)GetState();
	ctrl->IncRHover (state);
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}


// ==============================================================
// ==============================================================

HoverHoldAltControl::HoverHoldAltControl (DeltaGlider *vessel, int ident)
: DGSubSystem(vessel, ident)
{
	extern GDIParams g_Param;

	holdalt   = 0.0;
	active = false;
	altmode = VESSEL::ALTMODE_MEANRAD;
	holdmode = HOLDMODE_ALT;

	ELID_DISPLAY    = AddElement (new HoverHoldAltIndicator(this, g_Param.surf));
	ELID_HOLDBTN    = AddElement (holdbtn = new HoverAltBtn (this));
	ELID_ALTSET     = AddElement (altset  = new HoverAltSwitch (this));
	ELID_ALTCURRENT = AddElement (altcur  = new HoverAltCurBtn (this));
}

// --------------------------------------------------------------

void HoverHoldAltControl::Activate (bool ison)
{
	if (ison != active) {
		active = ison;

		if (active) DG()->SetHoverHoldAltitude (holdalt, altmode);
		else        DG()->DeactivateNavmode (NAVMODE_HOLDALT);

		holdbtn->SetState(active ? DGButton3::ON : DGButton3::OFF);
		oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_HOLDBTN));
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
		oapiVCRegisterArea (GlobalElId(ELID_DISPLAY), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// Hover hold alt button
		oapiVCRegisterArea (GlobalElId(ELID_HOLDBTN), PANEL_REDRAW_MOUSE | PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Spherical (GlobalElId(ELID_HOLDBTN), VC_BTN_HOVER_HOLDALT_ref, VC_BTN_HOVER_HOLDALT_mouserad);
		holdbtn->DefineAnimationVC (VC_BTN_HOVER_HOLDALT_axis, GRP_BUTTON3_VC, GRP_LIT_SURF_VC, VC_BTN_HOVER_HOLDALT_vofs, VC_BTN_HOVER_HOLDALT_LABEL_vofs);

		// Hover hold alt select switch
		oapiVCRegisterArea (GlobalElId(ELID_ALTSET), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_ALTSET), VC_HOVER_HOLDALT_SWITCH_mousearea[0], VC_HOVER_HOLDALT_SWITCH_mousearea[1], VC_HOVER_HOLDALT_SWITCH_mousearea[2], VC_HOVER_HOLDALT_SWITCH_mousearea[3]);
		altset->DefineAnimationVC (VC_HOVER_HOLDALT_SWITCH_ref, VC_HOVER_HOLDALT_SWITCH_axis, GRP_SWITCH2_VC, VC_HOVER_HOLDALT_SWITCH_vofs);

		// Hover hold alt set current button
		oapiVCRegisterArea (GlobalElId(ELID_ALTCURRENT), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Spherical (GlobalElId(ELID_ALTCURRENT), VC_BTN_HOVER_HOLDALT_CUR_ref, VC_BTN_HOVER_HOLDALT_CUR_mouserad);
		altcur->DefineAnimationVC (VC_BTN_HOVER_HOLDALT_CUR_axis, GRP_BUTTON2_VC, VC_BTN_HOVER_HOLDALT_CUR_vofs);

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
