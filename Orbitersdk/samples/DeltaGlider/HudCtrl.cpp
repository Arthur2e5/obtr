// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HudCtrl.cpp
// Class for HUD control subsystem
// ==============================================================

#define STRICT 1
#include "HudCtrl.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================
// HUD control subsystem
// ==============================================================

HUDControl::HUDControl (DeltaGlider *vessel, int ident)
: DGSubsystem (vessel, ident)
{
	last_mode  = HUD_NONE;
	hud_status = DeltaGlider::DOOR_CLOSED;
	hud_proc   = 0.0;

	ELID_MODEBUTTONS   = AddElement (modebuttons = new HUDModeButtons (this));
	ELID_HUDBRIGHTNESS = AddElement (brightdial = new HUDBrightnessDial (this));
	ELID_HUDCOLOUR     = AddElement (colbutton = new HUDColourButton (this));
	ELID_HUDRETRACT    = AddElement (updownswitch = new HUDUpDownSwitch (this));

	// HUD brightness dial animation
	static UINT HudBDialGrp = GRP_HUD_BRIGHTNESS_VC;
	static MGROUP_ROTATE HudBDialTransform (1, &HudBDialGrp, 1,
		VC_HUD_BRIGHTNESS_ref, VC_HUD_BRIGHTNESS_axis, (float)(-280*RAD));
	anim_vc_hudbdial = DG()->CreateAnimation (0.5);
	DG()->AddAnimationComponent (anim_vc_hudbdial, 0, 1, &HudBDialTransform);

	// Fold up HUD animation
	static UINT HudGrp1[2] = {GRP_HUD_FRAME_VC, GRP_HUD_PANE_VC};
	static UINT HudGrp2[3] = {GRP_HUD_FRAME_VC, GRP_HUD_PANE_VC, GRP_HUD_RAIL_VC};
	static MGROUP_ROTATE HudTransform1 (1, HudGrp1, 2,
		_V(0,1.5836,7.1280), _V(1,0,0), (float)(-62*RAD));
	static MGROUP_ROTATE HudTransform2 (1, HudGrp2, 3,
		_V(0,0.99,6.53), _V(1,0,0), (float)(-26*RAD));
	anim_vc_hud = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_vc_hud, 0, 0.4, &HudTransform1);
	DG()->AddAnimationComponent (anim_vc_hud, 0.4, 1, &HudTransform2);

}

// --------------------------------------------------------------

int HUDControl::GetHUDMode () const {
	return last_mode;
}

// --------------------------------------------------------------

void HUDControl::SetHUDMode (int mode)
{
	if (mode != HUD_NONE) {
		last_mode = mode;
		if (oapiCockpitMode() != COCKPIT_VIRTUAL || hud_status == DeltaGlider::DOOR_CLOSED)
			oapiSetHUDMode (mode);
		oapiTriggerRedrawArea (0, 0, GlobalElId (ELID_MODEBUTTONS));
		modebuttons->SetMode (mode);
	}
}

// --------------------------------------------------------------

void HUDControl::ToggleHUDMode ()
{
	SetHUDMode (last_mode == 3 ? 1 : last_mode+1);
}

// --------------------------------------------------------------

void HUDControl::ActivateHud (DeltaGlider::DoorStatus action)
{
	hud_status = action;
	if (action == DeltaGlider::DOOR_OPENING) {
		int hudmode = oapiGetHUDMode();
		if (hudmode != HUD_NONE) {
			last_mode = hudmode;
			oapiSetHUDMode (HUD_NONE);
		}
	}
	DG()->RecordEvent ("HUD", action == DeltaGlider::DOOR_CLOSING ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

void HUDControl::RevertHud (void)
{
	ActivateHud (hud_status == DeltaGlider::DOOR_CLOSED || hud_status == DeltaGlider::DOOR_CLOSING ?
		DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
}

// --------------------------------------------------------------

void HUDControl::ModHUDBrightness (bool increase)
{
	if (increase) oapiIncHUDIntensity();
	else          oapiDecHUDIntensity();

	double brt = oapiGetHUDIntensity();
	int mode = oapiGetHUDMode();
	if (brt == 0) {
		if (mode != HUD_NONE) {
			last_mode = mode;
			oapiSetHUDMode (HUD_NONE);
		}
	} else {
		if (mode == HUD_NONE && hud_status == DeltaGlider::DOOR_CLOSED)
			oapiSetHUDMode (last_mode);
	}
	if (oapiCockpitMode() == COCKPIT_VIRTUAL)
		DG()->SetAnimation (anim_vc_hudbdial, brt);
}

// --------------------------------------------------------------

void HUDControl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate HUD
	if (hud_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * HUD_OPERATING_SPEED;
		if (hud_status == DeltaGlider::DOOR_CLOSING) { // fold up HUD
			if (hud_proc > 0.0) hud_proc = max (0.0, hud_proc-da);
			else {
				hud_status = DeltaGlider::DOOR_CLOSED;
				oapiSetHUDMode (last_mode);
			}
		} else {
			if (hud_proc < 1.0) hud_proc = min (1.0, hud_proc+da);
			else                hud_status = DeltaGlider::DOOR_OPEN;
		}
		DG()->SetAnimation (anim_vc_hud, hud_proc);
	}

}

// --------------------------------------------------------------

bool HUDControl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);

	DG()->RegisterPanelArea (hPanel, GlobalElId (ELID_MODEBUTTONS), _R(  15, 18, 122, 33), PANEL_REDRAW_USER,   PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY, panel2dtex, modebuttons);

	return true;
}

// --------------------------------------------------------------

bool HUDControl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// HUD mode indicator/selector buttons on the dash panel
	oapiVCRegisterArea (GlobalElId(ELID_MODEBUTTONS), PANEL_REDRAW_USER | PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_MODEBUTTONS), VC_HUD_BUTTONS_mousearea[0], VC_HUD_BUTTONS_mousearea[1], VC_HUD_BUTTONS_mousearea[2], VC_HUD_BUTTONS_mousearea[3]);
	{
		static DWORD hudbtn_vofs[3] = {VC_BTN_HUDMODE_1_vofs,VC_BTN_HUDMODE_2_vofs,VC_BTN_HUDMODE_3_vofs};
		static DWORD hudbtn_label_vofs[3] = {VC_BTN_HUDMODE_1_LABEL_vofs, VC_BTN_HUDMODE_2_LABEL_vofs, VC_BTN_HUDMODE_3_LABEL_vofs};
		modebuttons->DefineAnimationsVC (VC_BTN_HUDMODE_1_axis, GRP_BUTTON3_VC, GRP_LIT_SURF_VC, hudbtn_vofs, hudbtn_label_vofs);
	}

	// HUD brightness dial
	oapiVCRegisterArea (GlobalElId(ELID_HUDBRIGHTNESS), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_HUDBRIGHTNESS), VC_HUD_BRIGHTNESS_mousearea[0], VC_HUD_BRIGHTNESS_mousearea[1], VC_HUD_BRIGHTNESS_mousearea[2], VC_HUD_BRIGHTNESS_mousearea[3]);

	// HUD colour selector button
	oapiVCRegisterArea (GlobalElId(ELID_HUDCOLOUR), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Spherical (GlobalElId(ELID_HUDCOLOUR), VC_HUD_COLBUTTON_ref, VC_HUD_COLBUTTON_mouserad);
	colbutton->DefineAnimationVC (VC_HUD_COLBUTTON_axis, GRP_BUTTON2_VC, VC_HUD_COLBUTTON_vofs);

	// HUD extend/retract switch
	oapiVCRegisterArea (GlobalElId(ELID_HUDRETRACT), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_HUDRETRACT), VC_HUDRETRACT_SWITCH_mousearea[0], VC_HUDRETRACT_SWITCH_mousearea[1], VC_HUDRETRACT_SWITCH_mousearea[2], VC_HUDRETRACT_SWITCH_mousearea[3]);
	updownswitch->DefineAnimationVC (VC_HUDRETRACT_SWITCH_ref, VC_HUDRETRACT_SWITCH_axis, GRP_SWITCH1_VC, VC_HUDRETRACT_SWITCH_vofs);

	return true;
}

// --------------------------------------------------------------

void HUDControl::clbkResetVC (int vcid, DEVMESHHANDLE hMesh)
{
	DG()->SetAnimation (anim_vc_hudbdial, oapiGetHUDIntensity());
	int hudmode = oapiGetHUDMode();
	if (hudmode != HUD_NONE && hud_status != DeltaGlider::DOOR_CLOSED)
		hud_status = DeltaGlider::DOOR_CLOSING, hud_proc = 0.0;
}

// ==============================================================

HUDModeButtons::HUDModeButtons (HUDControl *hc)
: PanelElement (hc->DG()), ctrl(hc)
{
	vmode = 0;
	for (int i = 0; i < 3; i++)
		btn[i] = new DGButton3 (vessel);
}

// --------------------------------------------------------------

HUDModeButtons::~HUDModeButtons ()
{
	for (int i = 0; i < 3; i++)
		delete btn[i];
}

// --------------------------------------------------------------

void HUDModeButtons::DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
	DWORD vofs[3], DWORD vofs_label[3])
{
	for (int i = 0; i < 3; i++) 
		btn[i]->DefineAnimationVC (axis, meshgrp, meshgrp_label, vofs[i], vofs_label[i]);
}

// --------------------------------------------------------------

void HUDModeButtons::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 8;
}

// --------------------------------------------------------------

bool HUDModeButtons::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float tx_dy =  4.0f;       // texture block height
	static const float bb_y0 = 19.5f;       // top edge of button block

	float y, y0 = bb_y0, y1 = bb_y0+tx_dy;
	int i, j, mode = oapiGetHUDMode();
	for (i = 0; i < 3; i++) {
		y = (i+1 == mode ? y1 : y0);
		for (j = 2; j < 4; j++)
			grp->Vtx[vtxofs+i*4+j].y = y;
	}
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	for (int i = 0; i < 3; i++)
		btn[i]->RedrawVC (hMesh, surf);
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	if (mx%29 < 20) {
		int mode = HUD_NONE+(mx/29);
		if (mode == HUD_NONE)
			ctrl->ModHUDBrightness(my < 8);
		else if (event & PANEL_MOUSE_LBDOWN)
			ctrl->SetHUDMode (mode);
	}
	return false;
}

// --------------------------------------------------------------

bool HUDModeButtons::ProcessMouseVC (int event, VECTOR3 &p)
{
	int i;
	int ix = (int)(p.x*126.0);
	int b = ix/43;
	if (ix-b*43 >= 40) return false;

	if (event & PANEL_MOUSE_LBDOWN) {
		for (i = 0; i < 3; i++)
			btn[i]->SetState (i==b ? DGButton3::PRESSED_FROM_OFF : DGButton3::OFF);
		DeltaGlider *dg = (DeltaGlider*)vessel;
		static const int mode[3] = {HUD_ORBIT,HUD_SURFACE,HUD_DOCKING};
		vmode = mode[b];
		ctrl->SetHUDMode (vmode);
	} else if (event & PANEL_MOUSE_LBUP) {
		btn[b]->SetState (DGButton3::ON);
	}
	return true;
}

// --------------------------------------------------------------

void HUDModeButtons::SetMode (int mode)
{
	if (mode != vmode) {
		int b = mode-HUD_ORBIT;
		for (int i = 0; i < 3; i++)
			btn[i]->SetState (i==b ? DGButton3::ON : DGButton3::OFF);
		vmode = mode;
	}
}

// ==============================================================
// ==============================================================

HUDBrightnessDial::HUDBrightnessDial (HUDControl *hc)
: PanelElement (hc->DG()), ctrl(hc)
{
}

// --------------------------------------------------------------

bool HUDBrightnessDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	ctrl->ModHUDBrightness (p.x > 0.5);
	return true;
}

// ==============================================================
// ==============================================================

HUDColourButton::HUDColourButton (HUDControl *hc)
: DGButton2(hc->DG()), ctrl(hc)
{
}

// --------------------------------------------------------------

bool HUDColourButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	DGButton2::ProcessMouseVC (event, p);
	if (event & PANEL_MOUSE_LBDOWN)
		oapiToggleHUDColour ();
	return (event & (PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP));
}

// ==============================================================
// ==============================================================

HUDUpDownSwitch::HUDUpDownSwitch (HUDControl *hc)
: DGSwitch1(hc->DG(), DGSwitch1::SPRING), ctrl(hc)
{
}

// --------------------------------------------------------------

bool HUDUpDownSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		switch (state) {
			case DGSwitch1::UP:   ctrl->ActivateHud (DeltaGlider::DOOR_OPENING); break;
			case DGSwitch1::DOWN: ctrl->ActivateHud (DeltaGlider::DOOR_CLOSING); break;
		}
		return true;
	}
	return false;
}