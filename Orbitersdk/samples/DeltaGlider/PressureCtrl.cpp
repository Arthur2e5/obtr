// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// PressureCtrl.cpp
// Cabin and airlock pressure control subsystem
// ==============================================================

#include "PressureCtrl.h"
#include "DeltaGlider.h"
#include "dg_vc_anim.h"
#include "meshres_vc.h"

// ==============================================================

double PressureControl::v_cabin = 24.0;
double PressureControl::v_airlock = 4.0;
double PressureControl::p_target = 100e3;

// --------------------------------------------------------------

PressureControl::PressureControl (DeltaGlider *vessel, int ident)
: DGSubSystem(vessel, ident)
{
	extern GDIParams g_Param;

	p_cabin = p_airlock = 100e3;
	p_ext_hatch = p_ext_lock = 0.0;
	docked = false;

	for (int i = 0; i < 5; i++) {
		ELID_PVALVESWITCH[i] = AddElement (valve_switch[i] = new PValveSwitch (this, i));
		valve_status[i] = 0;
	}
	ELID_DISPLAY = AddElement (pind = new PressureIndicator (this, g_Param.surf));
}

// --------------------------------------------------------------

void PressureControl::clbkPostStep (double simt, double simdt, double mjd)
{
	docked = DG()->DockingStatus(0) != 0;
	double p_static = DG()->GetAtmPressure();
	p_ext_hatch = p_static;
	if (!docked) {
		p_ext_lock = p_static;
		if (DG()->nose_status != DeltaGlider::DOOR_CLOSED)
			p_ext_lock += DG()->GetDynPressure() * DG()->nose_proc;
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
	if (DG()->olock_status != DeltaGlider::DOOR_CLOSED) {
		cs += 1.0*DG()->olock_proc;
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
	if (DG()->ilock_status != DeltaGlider::DOOR_CLOSED) {
		cs += 1.0*DG()->ilock_proc;
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

bool PressureControl::clbkLoadVC (int id)
{
	switch (id) {
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

PValveSwitch::PValveSwitch (PressureControl *pc, int id)
: DGSwitch1 (pc->Vessel(), DGSwitch1::TWOSTATE), pctrl(pc), vid(id)
{
}

// --------------------------------------------------------------

void PValveSwitch::ResetVC (DEVMESHHANDLE hMesh)
{
	SetState (pctrl->GetPValve(vid) ? UP:DOWN);
}

// --------------------------------------------------------------

bool PValveSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		pctrl->SetPValve (vid, state==UP ? 1:0);
		return true;
	}
	return false;
}

// ==============================================================

PressureIndicator::PressureIndicator (PressureControl *pc, SURFHANDLE blitsrc)
: PanelElement (pc->Vessel()), pctrl(pc), bsrc(blitsrc)
{
	btgt = 0;
}

// --------------------------------------------------------------

void PressureIndicator::ResetVC (DEVMESHHANDLE hMesh)
{
	upt = 0.0;
	btgt = oapiGetTextureHandle (pctrl->DG()->vcmesh_tpl, 14);
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
	ValStr (pctrl->PExtHatch(), cbuf);
	BlitReadout (0, cbuf);
	ValStr (pctrl->PCabin(), cbuf);
	BlitReadout (1, cbuf);
	ValStr (pctrl->PAirlock(), cbuf);
	BlitReadout (2, cbuf);
	ValStr (pctrl->PExtLock(), cbuf);
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
