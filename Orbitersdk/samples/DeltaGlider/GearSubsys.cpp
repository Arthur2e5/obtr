// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// GearSubsys.cpp
// Subsystem for landing gear control
// ==============================================================

#define STRICT 1

#include "GearSubsys.h"
#include "meshres.h"
#include "meshres_vc.h"
#include "meshres_p0.h"
#include "dg_vc_anim.h"

// ==============================================================
// Landing gear subsystem
// ==============================================================

GearSubsystem::GearSubsystem (DeltaGlider *v, int ident)
: DGSubsystem (v, ident)
{
	// create component instances
	AddComponent (gearctrl = new GearControl (this));
}

// --------------------------------------------------------------

void GearSubsystem::ActivateGear (DeltaGlider::DoorStatus action)
{
	gearctrl->ActivateGear (action);
}

// --------------------------------------------------------------

void GearSubsystem::RevertGear ()
{
	gearctrl->RevertGear ();
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus GearSubsystem::GearStatus() const
{
	return gearctrl->GearStatus();
}

// --------------------------------------------------------------

const double *GearSubsystem::GearPositionPtr() const
{
	return gearctrl->GearPositionPtr();
}

// ==============================================================
// Gear control: lever+indicator
// ==============================================================

GearControl::GearControl (GearSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	gear_status       = DeltaGlider::DOOR_CLOSED;
	gear_proc         = 0.0;
	gearlever_status  = DeltaGlider::DOOR_CLOSED;
	gearlever_proc    = 0.0;

	ELID_LEVER = AddElement (lever = new GearLever (this));
	ELID_INDICATOR = AddElement (indicator = new GearIndicator (this));

	// Landing gear animation
	static UINT NWheelStrutGrp[2] = {GRP_NWheelStrut1,GRP_NWheelStrut2};
	static MGROUP_ROTATE NWheelStrut (0, NWheelStrutGrp, 2,
		_V(0,-1.048,8.561), _V(1,0,0), (float)(-95*RAD));
	static UINT NWheelFCoverGrp[2] = {GRP_NWheelFCover1,GRP_NWheelFCover2};
	static MGROUP_ROTATE NWheelFCover (0, NWheelFCoverGrp, 2,
		_V(0,-1.145,8.65), _V(1,0,0), (float)(-90*RAD));
	static UINT NWheelLCoverGrp[2] = {GRP_NWheelLCover1,GRP_NWheelLCover2};
	static MGROUP_ROTATE NWheelLCover1 (0, NWheelLCoverGrp, 2,
		_V(-0.3,-1.222,7.029), _V(0,0.052,0.999), (float)(-90*RAD));
	static MGROUP_ROTATE NWheelLCover2 (0, NWheelLCoverGrp, 2,
		_V(-0.3,-1.222,7.029), _V(0,0.052,0.999), (float)( 90*RAD));
	static UINT NWheelRCoverGrp[2] = {GRP_NWheelRCover1,GRP_NWheelRCover2};
	static MGROUP_ROTATE NWheelRCover1 (0, NWheelRCoverGrp, 2,
		_V( 0.3,-1.222,7.029), _V(0,0.052,0.999), (float)( 90*RAD));
	static MGROUP_ROTATE NWheelRCover2 (0, NWheelRCoverGrp, 2,
		_V( 0.3,-1.222,7.029), _V(0,0.052,0.999), (float)(-90*RAD));
	static UINT LWheelStrutGrp[2] = {GRP_LWheelStrut1,GRP_LWheelStrut2};
	static MGROUP_ROTATE LWheelStrut (0, LWheelStrutGrp, 2,
		_V(-3.607,-1.137,-3.08), _V(0,0,1), (float)(-90*RAD));
	static UINT RWheelStrutGrp[2] = {GRP_RWheelStrut1,GRP_RWheelStrut2};
	static MGROUP_ROTATE RWheelStrut (0, RWheelStrutGrp, 2,
		_V( 3.607,-1.137,-3.08), _V(0,0,1), (float)(90*RAD));
	static UINT LWheelOCoverGrp[4] = {GRP_LWheelOCover1,GRP_LWheelOCover2,GRP_LWheelOCover3,GRP_LWheelOCover4};
	static MGROUP_ROTATE LWheelOCover (0, LWheelOCoverGrp, 4,
		_V(-3.658,-1.239,-3.038), _V(0,0,1), (float)(-110*RAD));
	static UINT LWheelICoverGrp[2] = {GRP_LWheelICover1,GRP_LWheelICover2};
	static MGROUP_ROTATE LWheelICover1 (0, LWheelICoverGrp, 2,
		_V(-2.175,-1.178,-3.438), _V(0,0,1), (float)(90*RAD));
	static MGROUP_ROTATE LWheelICover2 (0, LWheelICoverGrp, 2,
		_V(-2.175,-1.178,-3.438), _V(0,0,1), (float)(-90*RAD));
	static UINT RWheelOCoverGrp[4] = {GRP_RWheelOCover1,GRP_RWheelOCover2,GRP_RWheelOCover3,GRP_RWheelOCover4};
	static MGROUP_ROTATE RWheelOCover (0, RWheelOCoverGrp, 4,
		_V( 3.658,-1.239,-3.038), _V(0,0,1), (float)( 110*RAD));
	static UINT RWheelICoverGrp[2] = {GRP_RWheelICover1,GRP_RWheelICover2};
	static MGROUP_ROTATE RWheelICover1 (0, RWheelICoverGrp, 2,
		_V( 2.175,-1.178,-3.438), _V(0,0,1), (float)(-90*RAD));
	static MGROUP_ROTATE RWheelICover2 (0, RWheelICoverGrp, 2,
		_V( 2.175,-1.178,-3.438), _V(0,0,1), (float)( 90*RAD));
	anim_gear = DG()->CreateAnimation (1);
	DG()->AddAnimationComponent (anim_gear, 0.3, 1, &NWheelStrut);
	DG()->AddAnimationComponent (anim_gear, 0.3, 0.9, &NWheelFCover);
	DG()->AddAnimationComponent (anim_gear, 0, 0.3, &NWheelLCover1);
	DG()->AddAnimationComponent (anim_gear, 0.7, 1.0, &NWheelLCover2);
	DG()->AddAnimationComponent (anim_gear, 0, 0.3, &NWheelRCover1);
	DG()->AddAnimationComponent (anim_gear, 0.7, 1.0, &NWheelRCover2);
	DG()->AddAnimationComponent (anim_gear, 0, 1, &LWheelStrut);
	DG()->AddAnimationComponent (anim_gear, 0, 1, &RWheelStrut);
	DG()->AddAnimationComponent (anim_gear, 0, 1, &LWheelOCover);
	DG()->AddAnimationComponent (anim_gear, 0, 0.3, &LWheelICover1);
	DG()->AddAnimationComponent (anim_gear, 0.7, 1, &LWheelICover2);
	DG()->AddAnimationComponent (anim_gear, 0, 1, &RWheelOCover);
	DG()->AddAnimationComponent (anim_gear, 0, 0.3, &RWheelICover1);
	DG()->AddAnimationComponent (anim_gear, 0.7, 1, &RWheelICover2);

	// VC gear lever animation
	static UINT GearLeverGrp = GRP_GEAR_LEVER_VC;
	static MGROUP_ROTATE GearLeverTransform (1, &GearLeverGrp, 1,
		vc_gearlever_ref, vc_gearlever_axis, (float)(-70*RAD));
	anim_gearlever = DG()->CreateAnimation (0.5);
	DG()->AddAnimationComponent (anim_gearlever, 0, 1, &GearLeverTransform);
}

// --------------------------------------------------------------

void GearControl::ActivateGear (DeltaGlider::DoorStatus action)
{
	if (action == DeltaGlider::DOOR_OPENING && DG()->GroundContact()) return;
	// we cannot deploy the landing gear if we are already sitting on the ground

	bool close = (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING);
	gear_status = gearlever_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		gear_proc = gearlever_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_gear, gear_proc);
		DG()->SetAnimation (anim_gearlever, gearlever_proc);
		DG()->UpdateStatusIndicators();
		DG()->SetGearParameters (gear_proc);
	}
	oapiTriggerPanelRedrawArea (0, GlobalElId(ELID_LEVER));
	oapiTriggerRedrawArea (2, 0, GlobalElId(ELID_INDICATOR));
	DG()->RecordEvent ("GEAR", close ? "UP" : "DOWN");
}

// --------------------------------------------------------------

void GearControl::RevertGear ()
{
	extern void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd=0);

	ActivateGear (gear_status == DeltaGlider::DOOR_CLOSED || gear_status == DeltaGlider::DOOR_CLOSING ?
				  DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
	UpdateCtrlDialog (DG());
}

// --------------------------------------------------------------

void GearControl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate landing gear
	if (gear_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * GEAR_OPERATING_SPEED;
		if (gear_status == DeltaGlider::DOOR_CLOSING) {
			if (gear_proc > 0.0)
				gear_proc = max (0.0, gear_proc-da);
			else {
				gear_status = DeltaGlider::DOOR_CLOSED;
			}
		} else  { // door opening
			if (gear_proc < 1.0)
				gear_proc = min (1.0, gear_proc+da);
			else {
				gear_status = DeltaGlider::DOOR_OPEN;
			}
		}
		DG()->SetAnimation (anim_gear, gear_proc);
		DG()->SetGearParameters (gear_proc);
		oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));
		DG()->UpdateStatusIndicators();
	}

	// animate gear lever
	if (gearlever_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * 4.0;
		if (gearlever_status == DeltaGlider::DOOR_CLOSING) {
			if (gearlever_proc > 0.0)
				gearlever_proc = max (0.0, gearlever_proc-da);
			else {
				gearlever_status = DeltaGlider::DOOR_CLOSED;
			}
		} else  { // door opening
			if (gearlever_proc < 1.0)
				gearlever_proc = min (1.0, gearlever_proc+da);
			else {
				gearlever_status = DeltaGlider::DOOR_OPEN;
			}
		}
		DG()->SetAnimation (anim_gearlever, gearlever_proc);
	}
}

// --------------------------------------------------------------

bool GearControl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_LEVER), _R(73,147,105,372), PANEL_REDRAW_USER,   PANEL_MOUSE_LBDOWN, panel2dtex, lever);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_INDICATOR), _R(0,0,0,0), PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, indicator);

	return true;
}

// --------------------------------------------------------------

bool GearControl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	SURFHANDLE tex1 = oapiGetTextureHandle (DG()->vcmesh_tpl, 16);

	// Gear lever
	oapiVCRegisterArea (GlobalElId(ELID_LEVER), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_LEVER), vc_gearlever_mousearea[0], vc_gearlever_mousearea[1], vc_gearlever_mousearea[2], vc_gearlever_mousearea[3]);

	// Gear indicator
	oapiVCRegisterArea (GlobalElId(ELID_INDICATOR), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea (GlobalElId(ELID_INDICATOR), _R(32,127,61,158), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, tex1);

	return true;
}

// --------------------------------------------------------------

void GearControl::clbkSaveState (FILEHANDLE scn)
{
	if (gear_status) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", gear_status, gear_proc);
		oapiWriteScenario_string (scn, "GEAR", cbuf);
	}
}

// --------------------------------------------------------------

bool GearControl::clbkParseScenarioLine (const char *line)
{
	if (!_strnicmp (line, "GEAR", 4)) {
		sscanf (line+4, "%d%lf", &gear_status, &gear_proc);
		if (gear_status == DeltaGlider::DOOR_OPEN || gear_status == DeltaGlider::DOOR_OPENING) {
			gearlever_status = DeltaGlider::DOOR_OPEN; gearlever_proc = 1.0;
		} else {
			gearlever_status = DeltaGlider::DOOR_CLOSED; gearlever_proc = 0.0;
		}
		DG()->SetGearParameters (gear_proc);
		return true;
	}
	return false;
}

// --------------------------------------------------------------

void GearControl::clbkPostCreation ()
{
	DG()->SetAnimation (anim_gear, gear_proc);
	DG()->SetAnimation (anim_gearlever, gear_status & 1);
}

// ==============================================================

GearLever::GearLever (GearControl *comp)
: PanelElement(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

void GearLever::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 68;
}

// --------------------------------------------------------------

bool GearLever::Redraw2D (SURFHANDLE surf)
{
	static const float tx_dx =  176.0f;
	static const float bb_y0 =  187.0f;
	DeltaGlider::DoorStatus action = component->GearStatus();
	bool leverdown = (action == DeltaGlider::DOOR_OPENING || action == DeltaGlider::DOOR_OPEN);
	float y = (leverdown ? bb_y0+tx_dx : bb_y0);
	grp->Vtx[vtxofs+2].y = grp->Vtx[vtxofs+3].y = y;
	return false;
}

// --------------------------------------------------------------

bool GearLever::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider::DoorStatus action = component->GearStatus();
	if (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING) {
		if (my < 151) component->ActivateGear (DeltaGlider::DOOR_OPENING);
	} else {
		if (my >  46) component->ActivateGear (DeltaGlider::DOOR_CLOSING);
	}
	return false;
}

// --------------------------------------------------------------

bool GearLever::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	component->ActivateGear (p.y > 0.5 ? DeltaGlider::DOOR_CLOSING : DeltaGlider::DOOR_OPENING);
	return false;
}

// ==============================================================

GearIndicator::GearIndicator (GearControl *comp)
: PanelElement(comp->DG()), component(comp)
{
	tofs = (double)rand()/(double)RAND_MAX;
	light = true;
}

// --------------------------------------------------------------

void GearIndicator::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 72;
}

// --------------------------------------------------------------

bool GearIndicator::Redraw2D (SURFHANDLE surf)
{
	static const float texw = (float)PANEL2D_TEXW; // texture width
	int i, j, xofs;
	double d;
	DeltaGlider::DoorStatus action = component->GearStatus();
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: xofs = 1018; break;
		case DeltaGlider::DOOR_OPEN:   xofs = 1030; break;
		default: xofs = (modf (oapiGetSimTime()+tofs, &d) < 0.5 ? 1042 : 1020); break;
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++)
			grp->Vtx[vtxofs+i*4+j].tu = (xofs + (j%2)*10)/texw;
	}
	return false;
}

// --------------------------------------------------------------

bool GearIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;

	DeltaGlider::DoorStatus action = component->GearStatus();
	bool showlights;
	double d;
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: showlights = false; break;
		case DeltaGlider::DOOR_OPEN:   showlights = true; break;
		default: showlights = (modf (oapiGetSimTime()+tofs, &d) < 0.5); break;
	}
	if (showlights != light) {
		GROUPEDITSPEC ges;
		static WORD vtxofs = 7;
		static const DWORD nvtx = 2;
		static WORD vidx[nvtx] = {vtxofs,vtxofs+1};
		static float v[2] = {0.2427f,0.3042f};
		NTVERTEX vtx[nvtx];
		for (DWORD i = 0; i < nvtx; i++)
			vtx[i].tv = v[(showlights ? 1:0)];
		ges.flags = GRPEDIT_VTXTEXV;
		ges.Vtx = vtx;
		ges.vIdx = vidx;
		ges.nVtx = nvtx;
		oapiEditMeshGroup (hMesh, GRP_LIT_SURFACES_VC, &ges);
		light = showlights;
	}
	return false;
}
