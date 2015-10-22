// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// AerodynSubsys.cpp
// Subsystem for aerodynamic controls (selector dial, elevator
// trim, airbrake)
// ==============================================================

#define STRICT 1

#include "AerodynSubsys.h"
#include "meshres.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================
// Aerodynamic control subsystem
// ==============================================================

AerodynCtrlSubsystem::AerodynCtrlSubsystem (DeltaGlider *v, int ident)
: DGSubsystem (v, ident)
{
	// create component instances
	AddComponent (selector = new AerodynSelector (this));
	AddComponent (airbrake = new Airbrake (this));
}

// --------------------------------------------------------------

AerodynCtrlSubsystem::~AerodynCtrlSubsystem ()
{
	// delete components
	delete selector;
	delete airbrake;
}

// --------------------------------------------------------------

void AerodynCtrlSubsystem::SetMode (DWORD mode)
{
	selector->SetMode (mode);
}

// --------------------------------------------------------------

void AerodynCtrlSubsystem::ActivateAirbrake (DeltaGlider::DoorStatus action, bool half_step)
{
	airbrake->Activate (action, half_step);
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus AerodynCtrlSubsystem::AirbrakeStatus () const
{
	return airbrake->Status();
}

// --------------------------------------------------------------

const double *AerodynCtrlSubsystem::AirbrakePositionPtr () const
{
	return airbrake->PositionPtr();
}

// ==============================================================
// Control selector dial
// ==============================================================

AerodynSelector::AerodynSelector (AerodynCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	ELID_DIAL = AddElement (dial = new AerodynSelectorDial (this));
}

// --------------------------------------------------------------

void AerodynSelector::SetMode (DWORD mode)
{
	DWORD curmode = DG()->GetADCtrlMode();
	if (curmode != mode) DG()->SetADCtrlMode (mode);
	oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_DIAL));

}

// --------------------------------------------------------------

bool AerodynSelector::IncMode ()
{
	DWORD mode = DG()->GetADCtrlMode();
	if (mode <= 1) {
		DG()->SetADCtrlMode (mode ? 7 : 1);
		return true;
	} else return false;
}

// --------------------------------------------------------------

bool AerodynSelector::DecMode ()
{
	DWORD mode = min (DG()->GetADCtrlMode(),2);
	if (mode) {
		DG()->SetADCtrlMode (mode-1);
		return true;
	} else return false;
}

// --------------------------------------------------------------

bool AerodynSelector::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	// mode dial
	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_DIAL), _R(99,69,139,113), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN, panel2dtex, dial);

	return true;
}

// --------------------------------------------------------------

bool AerodynSelector::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// mode dial
	oapiVCRegisterArea (GlobalElId(ELID_DIAL), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_DIAL), VC_AF_DIAL_mousearea[0], VC_AF_DIAL_mousearea[1], VC_AF_DIAL_mousearea[2], VC_AF_DIAL_mousearea[3]);
	dial->DefineAnimationVC (VC_AF_DIAL_ref, VC_AF_DIAL_axis, GRP_DIAL1_VC, VC_AF_DIAL_vofs);

	return true;
}

// ==============================================================

AerodynSelectorDial::AerodynSelectorDial (AerodynSelector *comp)
: DGDial1(comp->DG(), 3, -50*RAD, 50*RAD), component(comp)
{
}

// --------------------------------------------------------------

void AerodynSelectorDial::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 4;
}

// --------------------------------------------------------------

void AerodynSelectorDial::ResetVC (DEVMESHHANDLE hMesh)
{
	DWORD mode = vessel->GetADCtrlMode();
	SetPosition (mode == 0 ? 0 : mode == 7 ? 1 : 2);
}

// --------------------------------------------------------------

bool AerodynSelectorDial::Redraw2D (SURFHANDLE surf)
{
	// constants for texture coordinates
	static const float texw = (float)PANEL2D_TEXW; // texture width
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_x0 = 1160.5f;            // left edge of texture block
	static const float tx_y0 = texh-615.5f;        // top edge of texture block
	static const float tx_dx = 39.0f;              // texture block width
	static const float tx_dy = 43.0f;              // texture block height
	static float tu[4] = {tx_x0/texw,(tx_x0+tx_dx)/texw,tx_x0/texw,(tx_x0+tx_dx)/texw};

	float dtu = (float)(min(vessel->GetADCtrlMode(),2)*40.0)/texw;
	for (int i = 0; i < 4; i++)
		grp->Vtx[vtxofs+i].tu = tu[i]+dtu;
	return false;
}

// --------------------------------------------------------------

bool AerodynSelectorDial::ProcessMouse2D (int event, int mx, int my)
{
	return (mx < 20 ? component->DecMode() : component->IncMode());
}

// --------------------------------------------------------------

bool AerodynSelectorDial::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGDial1::ProcessMouseVC (event, p)) {
		int pos = GetPosition();
		component->SetMode (pos == 0 ? 0 : pos == 1 ? 7 : 1);
		return true;
	}
	return false;
}

// ==============================================================
// Airbrake
// ==============================================================

Airbrake::Airbrake (AerodynCtrlSubsystem *_subsys)
: DGSubsystemComponent (_subsys)
{
	ELID_LEVER = AddElement (lever = new AirbrakeLever (this));
	brake_status         = DeltaGlider::DOOR_CLOSED;
	brake_proc           = 0.0;
	airbrakelever_status = DeltaGlider::DOOR_CLOSED;
	airbrakelever_proc   = 0.0;
	airbrake_tgt         = 0;

	// Airbrake animation
	static UINT RRudderGrp[2] = {GRP_RRudder1,GRP_RRudder2};
	static UINT LRudderGrp[2] = {GRP_LRudder1,GRP_LRudder2};
	static UINT UpperBrakeGrp[4] = {35,30,52,55};
	static MGROUP_ROTATE UpperBrake (0, UpperBrakeGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(50*RAD));
	static UINT LowerBrakeGrp[4] = {29,36,51,54};
	static MGROUP_ROTATE LowerBrake (0, LowerBrakeGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(-50*RAD));
	static MGROUP_ROTATE RRudderBrake (0, RRudderGrp, 2,
		_V( 8.668,0.958,-6.204), _V( 0.143,0.975,-0.172), (float)( 25*RAD));
	static MGROUP_ROTATE LRudderBrake (0, LRudderGrp, 2,
		_V(-8.668,0.958,-6.204), _V(-0.143,0.975,-0.172), (float)(-25*RAD));

	anim_brake = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_brake, 0, 1, &UpperBrake);
	DG()->AddAnimationComponent (anim_brake, 0, 1, &LowerBrake);
	DG()->AddAnimationComponent (anim_brake, 0, 1, &RRudderBrake);
	DG()->AddAnimationComponent (anim_brake, 0, 1, &LRudderBrake);

	// Airbrake lever animation
	static UINT AirbrakeLeverGrp = GRP_AIRBRAKE_LEVER_VC;
	static MGROUP_ROTATE AirbrakeLeverTransform (1, &AirbrakeLeverGrp, 1,
		VC_AIRBRAKELEVER_ref, VC_AIRBRAKELEVER_axis, (float)(-40*RAD));
	anim_airbrakelever = DG()->CreateAnimation(0.8);
	DG()->AddAnimationComponent (anim_airbrakelever, 0, 1, &AirbrakeLeverTransform);
}

// --------------------------------------------------------------

void Airbrake::Activate (DeltaGlider::DoorStatus action, bool half_step)
{
	const double eps = 1e-8;
	brake_status = airbrakelever_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		brake_proc = airbrakelever_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_brake, brake_proc);
		DG()->SetAnimation (anim_airbrakelever, airbrakelever_proc);
	} else if (action == DeltaGlider::DOOR_OPENING) {
		airbrake_tgt = (airbrakelever_proc < 0.5-eps ? 1:2);
	} else {
		airbrake_tgt = (airbrakelever_proc > 0.5+eps ? 1:0);
	}
	oapiTriggerPanelRedrawArea (0, GlobalElId(ELID_LEVER));
	DG()->RecordEvent ("AIRBRAKE", action == DeltaGlider::DOOR_CLOSING ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus Airbrake::Status () const
{
	return brake_status;
}

// --------------------------------------------------------------

void Airbrake::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate airbrake
	if (brake_status >= DeltaGlider::DOOR_CLOSING) {
		double tgt, da = simdt * AIRBRAKE_OPERATING_SPEED;
		if (brake_status == DeltaGlider::DOOR_CLOSING) { // retract brake
			tgt = (airbrake_tgt == 1 ? 0.5:0.0);
			if (brake_proc > tgt) brake_proc = max (tgt, brake_proc-da);
			else                  brake_status = DeltaGlider::DOOR_CLOSED;
		} else {                            // deploy brake
			tgt = (airbrake_tgt == 1 ? 0.5:1.0);
			if (brake_proc < tgt) brake_proc = min (tgt, brake_proc+da);
			else                  brake_status = DeltaGlider::DOOR_OPEN;
		}
		DG()->SetAnimation (anim_brake, brake_proc);
		DG()->UpdateStatusIndicators();
	}

	// animate airbrake lever
	if (airbrakelever_status >= DeltaGlider::DOOR_CLOSING) {
		double tgt, da = simdt * 4.0;
		if (airbrakelever_status == DeltaGlider::DOOR_CLOSING) {
			tgt = (airbrake_tgt == 1 ? 0.5:0.0);
			if (airbrakelever_proc > tgt)
				airbrakelever_proc = max (tgt, airbrakelever_proc-da);
			else {
				airbrakelever_status = DeltaGlider::DOOR_CLOSED;
			}
		} else  { // door opening
			tgt = (airbrake_tgt == 1 ? 0.5:1.0);
			if (airbrakelever_proc < tgt)
				airbrakelever_proc = min (tgt, airbrakelever_proc+da);
			else {
				airbrakelever_status = DeltaGlider::DOOR_OPEN;
			}
		}
		DG()->SetAnimation (anim_airbrakelever, airbrakelever_proc);
	}
}

// --------------------------------------------------------------

bool Airbrake::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	// airbrake lever
	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_LEVER), _R( 141,153, 161,213), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN, panel2dtex, lever);

	return true;
}

// --------------------------------------------------------------

bool Airbrake::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Airbrake lever
	oapiVCRegisterArea (GlobalElId(ELID_LEVER), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_LEVER), VC_AIRBRAKELEVER_mousearea[0], VC_AIRBRAKELEVER_mousearea[1], VC_AIRBRAKELEVER_mousearea[2], VC_AIRBRAKELEVER_mousearea[3]);

	return true;
}

// --------------------------------------------------------------

void Airbrake::clbkSaveState (FILEHANDLE scn)
{
	if (brake_status || brake_proc) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", brake_status, brake_proc);
		oapiWriteScenario_string (scn, "AIRBRAKE", cbuf);
	}
}

// --------------------------------------------------------------

bool Airbrake::clbkParseScenarioLine (const char *line)
{
	if (!_strnicmp (line, "AIRBRAKE", 8)) {
		sscanf (line+8, "%d%lf", &brake_status, &brake_proc);
		if (fabs (brake_proc-0.5) < 0.1 && brake_status <= DeltaGlider::DOOR_OPEN) {
			airbrake_tgt = 1;
			airbrakelever_status = DeltaGlider::DOOR_CLOSED; airbrakelever_proc = 0.5;
		} else if (brake_status == DeltaGlider::DOOR_OPEN || brake_status == DeltaGlider::DOOR_OPENING) {
			airbrake_tgt = 2;
			airbrakelever_status = DeltaGlider::DOOR_OPEN; airbrakelever_proc = 1.0;
		} else {
			airbrake_tgt = 0;
			airbrakelever_status = DeltaGlider::DOOR_CLOSED; airbrakelever_proc = 0.0;
		}
		return true;
	}
	return false;
}

// --------------------------------------------------------------

void Airbrake::clbkPostCreation ()
{
	DG()->SetAnimation (anim_brake, brake_proc);
	DG()->SetAnimation (anim_airbrakelever, airbrakelever_status & 1);
}

// ==============================================================

AirbrakeLever::AirbrakeLever (Airbrake *comp)
: PanelElement(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

void AirbrakeLever::Reset2D (MESHHANDLE hMesh)
{
	state = -1;
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 64;
}

// --------------------------------------------------------------

void AirbrakeLever::ResetVC (DEVMESHHANDLE hMesh)
{
	DeltaGlider *dg = component->DG();
	dg->SetAnimation (component->anim_airbrakelever, component->airbrakelever_proc);
}

// --------------------------------------------------------------

bool AirbrakeLever::Redraw2D (SURFHANDLE surf)
{
	// constants for panel coordinates
	static const float bb_y0 =  155.5f;
	static const float bb_dy =    7.0f;

	DeltaGlider* dg = component->DG();
	DeltaGlider::DoorStatus ds = component->brake_status;
	int newstate = component->airbrake_tgt;
	if (newstate != state) {
		state = newstate;
		static const float yp[4] = {bb_y0, bb_y0, bb_y0+bb_dy, bb_y0+bb_dy};
		float yshift = state*24.0f;
		for (int i = 0; i < 4; i++)
			grp->Vtx[vtxofs+i].y = yp[i]+yshift;
	}
	return false;
}

// --------------------------------------------------------------

bool AirbrakeLever::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = component->DG();
	component->Activate (my > 30 ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
	return false;
}

// --------------------------------------------------------------

bool AirbrakeLever::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = component->DG();
	component->Activate (p.y > 0.5 ? DeltaGlider::DOOR_CLOSING : DeltaGlider::DOOR_OPENING);
	return false;
}
