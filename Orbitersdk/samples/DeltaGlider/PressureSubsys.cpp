// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// PressureSubsys.cpp
// Cabin and airlock pressure control subsystem
// ==============================================================

#include "PressureSubsys.h"
#include "DockingSubsys.h"
#include "DeltaGlider.h"
#include "meshres.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================

double PressureSubsystem::v_cabin = 24.0;
double PressureSubsystem::v_airlock = 4.0;
double PressureSubsystem::p_target = 100e3;

// --------------------------------------------------------------

PressureSubsystem::PressureSubsystem (DeltaGlider *vessel, int ident)
: DGSubsystem(vessel, ident)
{
	extern GDIParams g_Param;

	p_cabin = p_airlock = 100e3;
	p_ext_hatch = p_ext_lock = 0.0;
	docked = false;

	AddComponent (airlockctrl = new AirlockCtrl (this));

	for (int i = 0; i < 5; i++) {
		ELID_PVALVESWITCH[i] = AddElement (valve_switch[i] = new PValveSwitch (this, i));
		valve_status[i] = 0;
	}
	ELID_DISPLAY = AddElement (pind = new PressureIndicator (this, g_Param.surf));
}

// --------------------------------------------------------------

PressureSubsystem::~PressureSubsystem ()
{
	delete airlockctrl;
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus PressureSubsystem::OLockStatus () const
{
	return airlockctrl->olock_status;
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus PressureSubsystem::ILockStatus () const
{
	return airlockctrl->ilock_status;
}

// --------------------------------------------------------------

void PressureSubsystem::ActivateOuterAirlock (DeltaGlider::DoorStatus action)
{
	airlockctrl->ActivateOuterLock (action);
}

// --------------------------------------------------------------

void PressureSubsystem::ActivateInnerAirlock (DeltaGlider::DoorStatus action)
{
	airlockctrl->ActivateInnerLock (action);
}

// --------------------------------------------------------------

void PressureSubsystem::RevertOuterAirlock ()
{
	airlockctrl->RevertOuterLock();
}

// --------------------------------------------------------------

void PressureSubsystem::clbkPostStep (double simt, double simdt, double mjd)
{
	DGSubsystem::clbkPostStep (simt, simdt, mjd);

	docked = DG()->DockingStatus(0) != 0;
	double p_static = DG()->GetAtmPressure();
	p_ext_hatch = p_static;
	if (!docked) {
		p_ext_lock = p_static;
		if (DG()->SubsysDocking()->NoseconeStatus() != DeltaGlider::DOOR_CLOSED)
			p_ext_lock += DG()->GetDynPressure() * DG()->SubsysDocking()->NoseconePosition();
	}
	else v_extdock = 2.0; // for now

	double cs, pdiff, dvol, dpc, dpa, pc, pa;

	// exchange cabin - ext.hatch
	cs = (valve_status[1] ? 2e-4:0.0);
	if (DG()->hatch_status != DeltaGlider::DOOR_CLOSED) {
		cs += 0.1*DG()->hatch_proc;
	}
	if (cs) {
		pdiff = p_ext_hatch-p_cabin;
		dvol = pdiff*cs*simdt*1e3;
		dpc = dvol/v_cabin;
		pc = p_cabin + dpc;
		if (p_cabin > p_ext_hatch) pc = max(pc,p_ext_hatch);
		else                       pc = min(pc,p_ext_hatch);
		p_cabin = pc;
	}

	// exchange airlock - ext.lock
	cs = (valve_status[3] ? 2e-4:0.0);
	if (airlockctrl->olock_status != DeltaGlider::DOOR_CLOSED) {
		cs += 1.0*airlockctrl->olock_proc;
	}
	if (cs) {
		pdiff = p_ext_lock-p_airlock;
		dvol = pdiff*cs*simdt*1e3;
		dpa = dvol/v_airlock;
		pa = p_airlock + dpa;
		if (docked) {
			dpc = -dvol/v_extdock;
			pc = p_ext_lock + dpc;
			if ((p_airlock-p_ext_lock)*(pa-pc) < 0.0)
				pa = pc = (pa*v_airlock + pc*v_extdock)/(v_airlock+v_extdock);
			p_ext_lock = pc;
		} else {
			if (p_airlock > p_ext_lock) pa = max(pa,p_ext_lock);
			else                        pa = min(pa,p_ext_lock);
		}
		p_airlock = pa;
	}

	// exchange cabin - airlock
	cs = (valve_status[2] ? 2e-4:0.0);
	if (airlockctrl->ilock_status != DeltaGlider::DOOR_CLOSED) {
		cs += 1.0*airlockctrl->ilock_proc;
	}
	if (cs) {
		pdiff = p_cabin-p_airlock;
		dvol = pdiff*cs*simdt*1e3;
		dpc = -dvol/v_cabin;
		dpa = dvol/v_airlock;
		pc = p_cabin + dpc;
		pa = p_airlock + dpa;
		if ((p_airlock-p_cabin)*(pa-pc) < 0.0)
			pa = pc = (pa*v_airlock + pc*v_cabin)/(v_airlock+v_cabin);
		p_cabin = pc;
		p_airlock = pa;
	}

	// supply cabin
	cs = (valve_status[0] ? 5e-5:0.0);
	if (cs) {
		pdiff = 400e3-p_cabin;
		dvol = pdiff*cs*simdt*1e3;
		dpc = dvol/v_cabin;
		pc = p_cabin + dpc;
		pc = min (pc, p_target);
		p_cabin = pc;
	}

	// supply airlock
	cs = (valve_status[4] ? 5e-5:0.0);
	if (cs) {
		pdiff = 400e3-p_airlock;
		dvol = pdiff*cs*simdt*1e3;
		dpa = dvol/v_airlock;
		pa = p_airlock + dpa;
		pa = min (pa, p_target);
		p_airlock = pa;
	}
}

// --------------------------------------------------------------

bool PressureSubsystem::clbkLoadVC (int vcid)
{
	DGSubsystem::clbkLoadVC (vcid);

	switch (vcid) {
	case 0:
		// Pressure indicator display
		oapiVCRegisterArea (GlobalElId(ELID_DISPLAY), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// Pressure valve switches
		oapiVCRegisterArea (GlobalElId(ELID_PVALVESWITCH[0]), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PVALVESWITCH[0]), VC_CABIN_O2_SWITCH_mousearea[0], VC_CABIN_O2_SWITCH_mousearea[1], VC_CABIN_O2_SWITCH_mousearea[2], VC_CABIN_O2_SWITCH_mousearea[3]);
		valve_switch[0]->DefineAnimationVC (VC_CABIN_O2_SWITCH_ref, VC_CABIN_O2_SWITCH_axis, GRP_SWITCH1_VC, VC_CABIN_O2_SWITCH_vofs);

		oapiVCRegisterArea (GlobalElId(ELID_PVALVESWITCH[1]), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PVALVESWITCH[1]), VC_VALVE1_SWITCH_mousearea[0], VC_VALVE1_SWITCH_mousearea[1], VC_VALVE1_SWITCH_mousearea[2], VC_VALVE1_SWITCH_mousearea[3]);
		valve_switch[1]->DefineAnimationVC (VC_VALVE1_SWITCH_ref, VC_VALVE1_SWITCH_axis, GRP_SWITCH1_VC, VC_VALVE1_SWITCH_vofs);

		oapiVCRegisterArea (GlobalElId(ELID_PVALVESWITCH[2]), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PVALVESWITCH[2]), VC_VALVE2_SWITCH_mousearea[0], VC_VALVE2_SWITCH_mousearea[1], VC_VALVE2_SWITCH_mousearea[2], VC_VALVE2_SWITCH_mousearea[3]);
		valve_switch[2]->DefineAnimationVC (VC_VALVE2_SWITCH_ref, VC_VALVE2_SWITCH_axis, GRP_SWITCH1_VC, VC_VALVE2_SWITCH_vofs);

		oapiVCRegisterArea (GlobalElId(ELID_PVALVESWITCH[3]), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PVALVESWITCH[3]), VC_VALVE3_SWITCH_mousearea[0], VC_VALVE3_SWITCH_mousearea[1], VC_VALVE3_SWITCH_mousearea[2], VC_VALVE3_SWITCH_mousearea[3]);
		valve_switch[3]->DefineAnimationVC (VC_VALVE3_SWITCH_ref, VC_VALVE3_SWITCH_axis, GRP_SWITCH1_VC, VC_VALVE3_SWITCH_vofs);

		oapiVCRegisterArea (GlobalElId(ELID_PVALVESWITCH[4]), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_PVALVESWITCH[4]), VC_LOCK_O2_SWITCH_mousearea[0], VC_LOCK_O2_SWITCH_mousearea[1], VC_LOCK_O2_SWITCH_mousearea[2], VC_LOCK_O2_SWITCH_mousearea[3]);
		valve_switch[4]->DefineAnimationVC (VC_LOCK_O2_SWITCH_ref, VC_LOCK_O2_SWITCH_axis, GRP_SWITCH1_VC, VC_LOCK_O2_SWITCH_vofs);
		break;
	}
	return true;
}

// ==============================================================
// Airlock controls
// ==============================================================

AirlockCtrl::AirlockCtrl (PressureSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	olock_status = DeltaGlider::DOOR_CLOSED;
	olock_proc   = 0.0;
	ilock_status = DeltaGlider::DOOR_CLOSED;
	ilock_proc   = 0.0;

	ELID_OSWITCH = AddElement (osw = new OuterLockSwitch (this));
	ELID_ISWITCH = AddElement (isw = new InnerLockSwitch (this));

	// Outer airlock animation
	static UINT OLockGrp[2] = {GRP_OLock1,GRP_OLock2};
	static MGROUP_ROTATE OLock (0, OLockGrp, 2,
		_V(0,-0.080,9.851), _V(1,0,0), (float)(110*RAD));
	static UINT VCOLockGrp[1] = {13};
	static MGROUP_ROTATE VCOLock (1, VCOLockGrp, 1,
		_V(0,-0.080,9.851), _V(1,0,0), (float)(110*RAD));
	anim_olock = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_olock, 0, 1, &OLock);
	DG()->AddAnimationComponent (anim_olock, 0, 1, &VCOLock);

	// Inner airlock animation
	static UINT ILockGrp[2] = {GRP_ILock1,GRP_ILock2};
	static MGROUP_ROTATE ILock (0, ILockGrp, 2,
		_V(0,-0.573,7.800), _V(1,0,0), (float)(85*RAD));
	// virtual cockpit mesh animation (inner airlock visible from cockpit)
	static UINT VCILockGrp[4] = {GRP_ILOCK1_VC,GRP_ILOCK2_VC,GRP_ILOCK3_VC,GRP_ILOCK_GLASS_VC};
	static MGROUP_ROTATE VCILock (1, VCILockGrp, 4,
		_V(0,-0.573,7.800), _V(1,0,0), (float)(85*RAD));
	anim_ilock = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_ilock, 0, 1, &ILock);
	DG()->AddAnimationComponent (anim_ilock, 0, 1, &VCILock);
}

// --------------------------------------------------------------

void AirlockCtrl::ActivateOuterLock (DeltaGlider::DoorStatus action)
{
	extern void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd=0);

	bool close = (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING);
	olock_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		olock_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_olock, olock_proc);
		DG()->UpdateStatusIndicators();
	}
	//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKSWITCH);
	UpdateCtrlDialog (DG());
	DG()->RecordEvent ("OLOCK", close ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

void AirlockCtrl::ActivateInnerLock (DeltaGlider::DoorStatus action)
{
	extern void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd=0);

	bool close = (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING);
	ilock_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		ilock_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_ilock, ilock_proc);
		DG()->UpdateStatusIndicators();
	}
	//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKSWITCH);
	UpdateCtrlDialog (DG());
	DG()->RecordEvent ("ILOCK", close ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

void AirlockCtrl::RevertOuterLock ()
{
	ActivateOuterLock (olock_status == DeltaGlider::DOOR_CLOSED || olock_status == DeltaGlider::DOOR_CLOSING ?
		                  DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
}

// --------------------------------------------------------------

void AirlockCtrl::RevertInnerLock ()
{
	ActivateInnerLock (ilock_status == DeltaGlider::DOOR_CLOSED || ilock_status == DeltaGlider::DOOR_CLOSING ?
		                  DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
}

// --------------------------------------------------------------

void AirlockCtrl::clbkSaveState (FILEHANDLE scn)
{
	if (olock_status) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", olock_status, olock_proc);
		oapiWriteScenario_string (scn, "AIRLOCK", cbuf);
	}
	if (ilock_status) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", ilock_status, ilock_proc);
		oapiWriteScenario_string (scn, "IAIRLOCK", cbuf);
	}
}

// --------------------------------------------------------------

bool AirlockCtrl::clbkParseScenarioLine (const char *line)
{
	if (!_strnicmp (line, "AIRLOCK", 7)) {
		sscanf (line+7, "%d%lf", &olock_status, &olock_proc);
		return true;
	} else if (!_strnicmp (line, "IAIRLOCK", 8)) {
		sscanf (line+8, "%d%lf", &ilock_status, &ilock_proc);
		return true;
	}
	return false;
}

// --------------------------------------------------------------

void AirlockCtrl::clbkPostCreation ()
{
	DG()->SetAnimation (anim_olock, olock_proc);
	DG()->SetAnimation (anim_ilock, ilock_proc);	
}

// --------------------------------------------------------------

void AirlockCtrl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate outer airlock
	if (olock_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (olock_status == DeltaGlider::DOOR_CLOSING) {
			if (olock_proc > 0.0)
				olock_proc = max (0.0, olock_proc-da);
			else {
				olock_status = DeltaGlider::DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		} else { // door opening
			if (olock_proc < 1.0)
				olock_proc = min (1.0, olock_proc+da);
			else {
				olock_status = DeltaGlider::DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		}
		DG()->SetAnimation (anim_olock, olock_proc);
		DG()->UpdateStatusIndicators();
	}

	// animate inner airlock
	if (ilock_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (ilock_status == DeltaGlider::DOOR_CLOSING) {
			if (ilock_proc > 0.0)
				ilock_proc = max (0.0, ilock_proc-da);
			else {
				ilock_status = DeltaGlider::DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		} else { // door opening
			if (ilock_proc < 1.0)
				ilock_proc = min (1.0, ilock_proc+da);
			else {
				ilock_status = DeltaGlider::DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		}
		DG()->SetAnimation (anim_ilock, ilock_proc);
		DG()->UpdateStatusIndicators();
	}
}

// --------------------------------------------------------------

bool AirlockCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Outer airlock open/close switch
	oapiVCRegisterArea (GlobalElId(ELID_OSWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_OSWITCH), VC_OLOCK_SWITCH_mousearea[0], VC_OLOCK_SWITCH_mousearea[1], VC_OLOCK_SWITCH_mousearea[2], VC_OLOCK_SWITCH_mousearea[3]);
	osw->DefineAnimationVC (VC_OLOCK_SWITCH_ref, VC_OLOCK_SWITCH_axis, GRP_SWITCH1_VC, VC_OLOCK_SWITCH_vofs);

	// Inner airlock open/close switch
	oapiVCRegisterArea (GlobalElId(ELID_ISWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_ISWITCH), VC_ILOCK_SWITCH_mousearea[0], VC_ILOCK_SWITCH_mousearea[1], VC_ILOCK_SWITCH_mousearea[2], VC_ILOCK_SWITCH_mousearea[3]);
	isw->DefineAnimationVC (VC_ILOCK_SWITCH_ref, VC_ILOCK_SWITCH_axis, GRP_SWITCH1_VC, VC_ILOCK_SWITCH_vofs);

	return true;
}

// ==============================================================

OuterLockSwitch::OuterLockSwitch (AirlockCtrl *comp)
: DGSwitch1(comp->DG(), DGSwitch1::TWOSTATE), component(comp)
{
}

// --------------------------------------------------------------

void OuterLockSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (component->olock_status == DeltaGlider::DOOR_CLOSED ||
			  component->olock_status == DeltaGlider::DOOR_CLOSING ? DOWN:UP);
}

// --------------------------------------------------------------

bool OuterLockSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		component->ActivateOuterLock (state==UP ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
		return true;
	}
	return false;
}

// ==============================================================

InnerLockSwitch::InnerLockSwitch (AirlockCtrl *comp)
: DGSwitch1(comp->DG(), DGSwitch1::TWOSTATE), component(comp)
{
}

// --------------------------------------------------------------

void InnerLockSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (component->ilock_status == DeltaGlider::DOOR_CLOSED ||
			  component->ilock_status == DeltaGlider::DOOR_CLOSING ? DOWN:UP);
}

// --------------------------------------------------------------

bool InnerLockSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		component->ActivateInnerLock (state==UP ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
		return true;
	}
	return false;
}

// ==============================================================

PValveSwitch::PValveSwitch (PressureSubsystem *_subsys, int id)
: DGSwitch1(_subsys->DG(), DGSwitch1::TWOSTATE), subsys(_subsys), vid(id)
{
}

// --------------------------------------------------------------

void PValveSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (subsys->GetPValve(vid) ? UP:DOWN);
}

// --------------------------------------------------------------

bool PValveSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		subsys->SetPValve (vid, state==UP ? 1:0);
		return true;
	}
	return false;
}

// ==============================================================

PressureIndicator::PressureIndicator (PressureSubsystem *_subsys, SURFHANDLE blitsrc)
: PanelElement(_subsys->DG()), subsys(_subsys), bsrc(blitsrc)
{
	btgt = 0;
}

// --------------------------------------------------------------

void PressureIndicator::ResetVC (DEVMESHHANDLE hMesh)
{
	upt = 0.0;
	btgt = oapiGetTextureHandle (subsys->DG()->vcmesh_tpl, 14);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 8; j++)
			label[i][j] = ' ';
}

// --------------------------------------------------------------

bool PressureIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!btgt) return false;

	double t = oapiGetSimTime();
	if (t < upt && t > upt-1.0) return false;
	upt = t + 0.5;

	char cbuf[16];
	ValStr (subsys->PExtHatch(), cbuf);
	BlitReadout (0, cbuf);
	ValStr (subsys->PCabin(), cbuf);
	BlitReadout (1, cbuf);
	ValStr (subsys->PAirlock(), cbuf);
	BlitReadout (2, cbuf);
	ValStr (subsys->PExtLock(), cbuf);
	BlitReadout (3, cbuf);

	return false;
}

// --------------------------------------------------------------

void PressureIndicator::ValStr (double p, char *cbuf)
{
	p *= 1e-3; // convert to kPa
	if (p > 300.0) {
		strcpy (cbuf, "-----");
	} else {
		sprintf (cbuf, "%5.1lf", p);
	}
}

// --------------------------------------------------------------

void PressureIndicator::BlitReadout (int which, const char *str)
{
	int tgtx = 65 + which*61;
	int tgty = 17;
	int srcx, srcy = 0;
	int w = 8;
	int h = 11;
	char c, *tgtstr = label[which];
	for (int i = 0; i < 5; i++) {
		if ((c=str[i]) != tgtstr[i]) {
			if (c >= '0' && c <= '9') srcx = (c-'0')*8;
			else switch (c) {
				case '.': srcx = 10*8; break;
				case '+': srcx = 11*8; break;
				case '-': srcx = 12*8; break;
				case 'k': srcx = 13*8; break;
				case 'M': srcx = 14*8; break;
				case 'G': srcx = 15*8; break;
				default:  srcx = 16*8; break;
			}
			oapiBlt (btgt, bsrc, tgtx, tgty, srcx, srcy, w, h);
			tgtstr[i] = c;
		}
		tgtx += w;
	}
}
