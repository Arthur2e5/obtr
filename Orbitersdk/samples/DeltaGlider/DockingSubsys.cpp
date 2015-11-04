// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DockingSubsys.cpp
// Nosecone and undock controls
// ==============================================================

#include "DockingSubsys.h"
#include "meshres.h"
#include "meshres_p0.h"
#include "meshres_vc.h"
#include "dg_vc_anim.h"

// ==============================================================
// Docking control subsystem
// ==============================================================

DockingCtrlSubsystem::DockingCtrlSubsystem (DeltaGlider *v, int ident)
: DGSubsystem (v, ident)
{
	// create component instances
	AddComponent (noseconectrl = new NoseconeCtrl (this));
	AddComponent (undockctrl = new UndockCtrl (this));
	AddComponent (eladderctrl = new EscapeLadderCtrl (this));
	AddComponent (dsealctrl = new DocksealCtrl (this));
}

// --------------------------------------------------------------

DockingCtrlSubsystem::~DockingCtrlSubsystem ()
{
	// delete components
	delete noseconectrl;
	delete undockctrl;
	delete eladderctrl;
	delete dsealctrl;
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus DockingCtrlSubsystem::NoseconeStatus () const
{
	return noseconectrl->Status();
}

// --------------------------------------------------------------

double DockingCtrlSubsystem::NoseconePosition () const
{
	return noseconectrl->Position();
}

// --------------------------------------------------------------

const double *DockingCtrlSubsystem::NoseconePositionPtr() const
{
	return noseconectrl->PositionPtr();
}

// --------------------------------------------------------------

void DockingCtrlSubsystem::ActivateNosecone (DeltaGlider::DoorStatus action)
{
	noseconectrl->Activate (action);
}

// --------------------------------------------------------------

void DockingCtrlSubsystem::RevertNosecone ()
{
	noseconectrl->Revert();
}

// --------------------------------------------------------------

void DockingCtrlSubsystem::ActivateLadder (DeltaGlider::DoorStatus action)
{
	eladderctrl->Activate (action);
}

// --------------------------------------------------------------

DeltaGlider::DoorStatus DockingCtrlSubsystem::LadderStatus () const
{
	return eladderctrl->Status();
}

// --------------------------------------------------------------

void DockingCtrlSubsystem::clbkDockEvent (int dock, OBJHANDLE mate)
{
	dsealctrl->SetDockStatus (mate != 0);
}

// ==============================================================
// Nosecone control
// ==============================================================

NoseconeCtrl::NoseconeCtrl (DockingCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	nose_status       = DeltaGlider::DOOR_CLOSED;
	nose_proc         = 0.0;
	noselever_status  = DeltaGlider::DOOR_CLOSED;
	noselever_proc    = 0.0;

	ELID_LEVER = AddElement (lever = new NoseconeLever (this));
	ELID_INDICATOR = AddElement (indicator = new NoseconeIndicator (this));

	// Nosecone animation
	static UINT NConeTLGrp[2] = {GRP_NConeTL1,GRP_NConeTL2};
	static MGROUP_ROTATE NConeTL (0, NConeTLGrp, 2,
		_V(-0.424,-0.066,9.838), _V(-0.707,-0.707,0), (float)(150*RAD));
	static UINT NConeTRGrp[2] = {GRP_NConeTR1,GRP_NConeTR2};
	static MGROUP_ROTATE NConeTR (0, NConeTRGrp, 2,
		_V( 0.424,-0.066,9.838), _V(-0.707, 0.707,0), (float)(150*RAD));
	static UINT NConeBLGrp[2] = {GRP_NConeBL1,GRP_NConeBL2};
	static MGROUP_ROTATE NConeBL (0, NConeBLGrp, 2,
		_V(-0.424,-0.914,9.838), _V( 0.707,-0.707,0), (float)(150*RAD));
	static UINT NConeBRGrp[2] = {GRP_NConeBR1,GRP_NConeBR2};
	static MGROUP_ROTATE NConeBR (0, NConeBRGrp, 2,
		_V( 0.424,-0.914,9.838), _V( 0.707, 0.707,0), (float)(150*RAD));
	static UINT NConeDockGrp[1] = {GRP_NConeDock};
	static MGROUP_TRANSLATE NConeDock (0, NConeDockGrp, 1, _V(0,0,0.06));
	// virtual cockpit mesh animation (nose cone visible from cockpit)
	static UINT VCNConeTLGrp[1] = {GRP_NOSECONE_L_VC};
	static MGROUP_ROTATE VCNConeTL (1, VCNConeTLGrp, 1,
		_V(-0.424,-0.066,9.838), _V(-0.707,-0.707,0), (float)(150*RAD));
	static UINT VCNConeTRGrp[1] = {GRP_NOSECONE_R_VC};
	static MGROUP_ROTATE VCNConeTR (1, VCNConeTRGrp, 1,
		_V( 0.424,-0.066,9.838), _V(-0.707, 0.707,0), (float)(150*RAD));
	anim_nose = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_nose, 0.01, 0.92, &NConeTL);
	DG()->AddAnimationComponent (anim_nose, 0.01, 0.92, &VCNConeTL);
	DG()->AddAnimationComponent (anim_nose, 0.02, 0.925, &NConeTR);
	DG()->AddAnimationComponent (anim_nose, 0.02, 0.925, &VCNConeTR);
	DG()->AddAnimationComponent (anim_nose, 0, 0.91, &NConeBL);
	DG()->AddAnimationComponent (anim_nose, 0.015, 0.915, &NConeBR);
	DG()->AddAnimationComponent (anim_nose, 0.8, 1, &NConeDock);

	// Nosecone lever animatuion
	static UINT NoseconeLeverGrp = GRP_NOSECONE_LEVER_VC;
	static MGROUP_ROTATE NoseconeLeverTransform (1, &NoseconeLeverGrp, 1,
		VC_NCONELEVER_ref, VC_NCONELEVER_axis, (float)(-70*RAD));
	anim_noselever = DG()->CreateAnimation (0.5);
	DG()->AddAnimationComponent (anim_noselever, 0, 1, &NoseconeLeverTransform);

}

// --------------------------------------------------------------

void NoseconeCtrl::Activate (DeltaGlider::DoorStatus action)
{
	extern void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd=0);

	bool close = (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING);
	nose_status = noselever_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		nose_proc = noselever_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_nose, nose_proc);
		DG()->SetAnimation (anim_noselever, noselever_proc);
		DG()->UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (0, GlobalElId(ELID_LEVER));
	oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));

	if (close && ((DockingCtrlSubsystem*)Subsys())->LadderStatus() != DeltaGlider::DOOR_CLOSED)
		((DockingCtrlSubsystem*)Subsys())->ActivateLadder (action); // retract ladder before closing the nose cone

	UpdateCtrlDialog (DG());
	DG()->RecordEvent ("NOSECONE", close ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

void NoseconeCtrl::Revert ()
{
	Activate (nose_status == DeltaGlider::DOOR_CLOSED || nose_status == DeltaGlider::DOOR_CLOSING ?
		DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
}

// --------------------------------------------------------------

void NoseconeCtrl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate nose cone
	if (nose_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * NOSE_OPERATING_SPEED;
		if (nose_status == DeltaGlider::DOOR_CLOSING) {
			if (nose_proc > 0.0)
				nose_proc = max (0.0, nose_proc-da);
			else
				nose_status = DeltaGlider::DOOR_CLOSED;
		} else { // door opening
			if (nose_proc < 1.0)
				nose_proc = min (1.0, nose_proc+da);
			else
				nose_status = DeltaGlider::DOOR_OPEN;
		}
		DG()->SetAnimation (anim_nose, nose_proc);
		oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));
		DG()->UpdateStatusIndicators();
	}

	// animate VC nosecone lever
	if (noselever_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * 4.0;
		if (noselever_status == DeltaGlider::DOOR_CLOSING) {
			if (noselever_proc > 0.0)
				noselever_proc = max (0.0, noselever_proc-da);
			else
				noselever_status = DeltaGlider::DOOR_CLOSED;
		} else  { // door opening
			if (noselever_proc < 1.0)
				noselever_proc = min (1.0, noselever_proc+da);
			else
				noselever_status = DeltaGlider::DOOR_OPEN;
		}
		DG()->SetAnimation (anim_noselever, noselever_proc);
	}
}

// --------------------------------------------------------------

bool NoseconeCtrl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_LEVER), _R(1221,347,1260,461), PANEL_REDRAW_USER,  PANEL_MOUSE_LBDOWN, panel2dtex, lever);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_INDICATOR), _R(0,0,0,0), PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, indicator);

	return true;
}

// --------------------------------------------------------------

bool NoseconeCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Nosecone lever
	oapiVCRegisterArea (GlobalElId(ELID_LEVER), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_LEVER), VC_NCONELEVER_mousearea[0], VC_NCONELEVER_mousearea[1], VC_NCONELEVER_mousearea[2], VC_NCONELEVER_mousearea[3]);

	// Nosecone indicator
	oapiVCRegisterArea (GlobalElId(ELID_INDICATOR), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);

	return true;
}

// --------------------------------------------------------------

void NoseconeCtrl::clbkSaveState (FILEHANDLE scn)
{
	if (nose_status) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", nose_status, nose_proc);
		oapiWriteScenario_string (scn, "NOSECONE", cbuf);
	}
}

// --------------------------------------------------------------

bool NoseconeCtrl::clbkParseScenarioLine (const char *line)
{
	if (!_strnicmp (line, "NOSECONE", 8)) {
		sscanf (line+8, "%d%lf", &nose_status, &nose_proc);
		if (nose_status == DeltaGlider::DOOR_OPEN || nose_status == DeltaGlider::DOOR_OPENING) {
			noselever_status = DeltaGlider::DOOR_OPEN; noselever_proc = 1.0;
		} else {
			noselever_status = DeltaGlider::DOOR_CLOSED; noselever_proc = 0.0;
		}
		return true;
	}
	return false;
}

// --------------------------------------------------------------

void NoseconeCtrl::clbkPostCreation ()
{
	DG()->SetAnimation (anim_nose, nose_proc);
	DG()->SetAnimation (anim_noselever, nose_status & 1);
}

// ==============================================================

NoseconeLever::NoseconeLever (NoseconeCtrl *comp)
: PanelElement(comp->DG()), component(comp)
{
}

// --------------------------------------------------------------

void NoseconeLever::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 84;
}

// --------------------------------------------------------------

bool NoseconeLever::Redraw2D (SURFHANDLE surf)
{
	static const float texh = (float)PANEL2D_TEXH; // texture height

	DeltaGlider::DoorStatus action = component->nose_status;
	bool leverdown = (action == DeltaGlider::DOOR_OPENING || action == DeltaGlider::DOOR_OPEN);

	float y0, dy, tv0;
	if (leverdown) y0 = 420.5f, dy = 21.0f, tv0 = texh-677.5f;
	else           y0 = 346.5f, dy = 19.0f, tv0 = texh-696.5f;
	int j;
	for (j = 0; j < 4; j++) {
		grp->Vtx[vtxofs+j].y = y0 + (j/2)*dy;
		grp->Vtx[vtxofs+j].tv = (tv0 + (j/2)*dy)/texh;
	}
	return false;
}

// --------------------------------------------------------------

bool NoseconeLever::ProcessMouse2D (int event, int mx, int my)
{
	DeltaGlider *dg = component->DG();
	DeltaGlider::DoorStatus action = component->nose_status;
	if (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING) {
		if (my < 58) component->Activate (DeltaGlider::DOOR_OPENING);
	} else {
		if (my > 36) component->Activate (DeltaGlider::DOOR_CLOSING);
	}
	return false;
}

// --------------------------------------------------------------

bool NoseconeLever::ProcessMouseVC (int event, VECTOR3 &p)
{
	component->Activate (p.y > 0.5 ? DeltaGlider::DOOR_CLOSING : DeltaGlider::DOOR_OPENING);
	return false;
}


// ==============================================================

NoseconeIndicator::NoseconeIndicator (NoseconeCtrl *comp)
: PanelElement(comp->DG()), component(comp)
{
	tofs = (double)rand()/(double)RAND_MAX;
	light = true;
}

// --------------------------------------------------------------

void NoseconeIndicator::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 88;
}

// --------------------------------------------------------------

bool NoseconeIndicator::Redraw2D (SURFHANDLE surf)
{
	static const float texw = (float)PANEL2D_TEXW; // texture width

	int i, j, xofs;
	double d;
	DeltaGlider::DoorStatus action = component->nose_status;
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: xofs = 1014; break;
		case DeltaGlider::DOOR_OPEN:   xofs = 1027; break;
		default: xofs = (modf (oapiGetSimTime()+tofs, &d) < 0.5 ? 1040 : 1014); break;
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 3; j++)
			grp->Vtx[vtxofs+i*3+j].tu = (xofs + (j%2)*12)/texw;
	}
	return false;
}

// --------------------------------------------------------------

bool NoseconeIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;

	DeltaGlider::DoorStatus action = component->nose_status;
	bool showlights;
	double d;
	switch (action) {
		case DeltaGlider::DOOR_CLOSED: showlights = false; break;
		case DeltaGlider::DOOR_OPEN:   showlights = true; break;
		default: showlights = (modf (oapiGetSimTime()+tofs, &d) < 0.5); break;
	}
	if (showlights != light) {
		GROUPEDITSPEC ges;
		static WORD vtxofs = VC_NCONE_INDICATOR_vofs;
		static const DWORD nvtx = 2;
		static WORD vidx[nvtx] = {vtxofs,vtxofs+1};
		static float v[2] = {0.2427f,0.3003f};
		NTVERTEX vtx[nvtx];
		for (DWORD i = 0; i < nvtx; i++)
			vtx[i].tv = v[showlights ? 1:0];
		ges.flags = GRPEDIT_VTXTEXV;
		ges.Vtx = vtx;
		ges.vIdx = vidx;
		ges.nVtx = nvtx;
		oapiEditMeshGroup (hMesh, GRP_VC4_LIT_VC, &ges);
		light = showlights;
	}
	return false;
}


// ==============================================================
// Undock control
// ==============================================================

UndockCtrl::UndockCtrl (DockingCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	undock_status     = DeltaGlider::DOOR_CLOSED;
	undock_proc       = 0.0;

	ELID_LEVER = AddElement (lever = new UndockLever (this));

	// Undock lever
	static UINT UndockLeverGrp = GRP_UNDOCK_LEVER_VC;
	static MGROUP_ROTATE UndockLeverTransform (1, &UndockLeverGrp, 1,
		VC_UNDOCKLEVER_ref, VC_UNDOCKLEVER_axis, (float)(-90*RAD));
	anim_undocklever = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_undocklever, 0, 1, &UndockLeverTransform);
}

// --------------------------------------------------------------

void UndockCtrl::Activate (DeltaGlider::DoorStatus action)
{
	undock_status = action;
	if (action == DeltaGlider::DOOR_OPENING) DG()->Undock(0);
}

// --------------------------------------------------------------

void UndockCtrl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate undock lever
	if (undock_status >= DeltaGlider::DOOR_CLOSING) {
		if (undock_status == DeltaGlider::DOOR_CLOSING) {
			double da = simdt * 10.0;
			if (undock_proc > 0.0)
				undock_proc = max (0.0, undock_proc-da);
			else
				undock_status = DeltaGlider::DOOR_CLOSED;
		} else { // door opening
			double da = simdt * 5.0;
			if (undock_proc < 1.0)
				undock_proc = min (1.0, undock_proc+da);
			else
				undock_status = DeltaGlider::DOOR_OPEN;
		}
		DG()->SetAnimation (anim_undocklever, undock_proc);
	}
}

// --------------------------------------------------------------

bool UndockCtrl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_LEVER), _R(1151,355,1193,436), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP, panel2dtex, lever);

	return true;
}

// --------------------------------------------------------------

bool UndockCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Undock lever
	oapiVCRegisterArea (GlobalElId(ELID_LEVER), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_LEVER), VC_UNDOCKLEVER_mousearea[0], VC_UNDOCKLEVER_mousearea[1], VC_UNDOCKLEVER_mousearea[2], VC_UNDOCKLEVER_mousearea[3]);

	return true;
}

// ==============================================================

UndockLever::UndockLever (UndockCtrl *comp)
: PanelElement(comp->DG()), component(comp)
{
	btndown = false;
}

// --------------------------------------------------------------

void UndockLever::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 100;
	btndown = false;
}

// --------------------------------------------------------------

bool UndockLever::Redraw2D (SURFHANDLE surf)
{
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float bb_y0 =  354.0f;     // top edge of button block
	static const float tx_dy = 103.0f;       // texture block height
	static const float tx_y0 = texh-356.0f; // top edge of texture block

	float y = (btndown ? bb_y0+tx_dy : bb_y0);
	float tv = (btndown ? tx_y0+tx_dy : tx_y0)/texh;
	grp->Vtx[vtxofs+2].y = grp->Vtx[vtxofs+3].y = y;
	grp->Vtx[vtxofs+2].tv = grp->Vtx[vtxofs+3].tv = tv;
	return false;
}

// --------------------------------------------------------------

bool UndockLever::ProcessMouse2D (int event, int mx, int my)
{
	if (event & PANEL_MOUSE_LBDOWN) vessel->Undock (0);
	btndown = (event == PANEL_MOUSE_LBDOWN);
	return true;
}

// --------------------------------------------------------------

bool UndockLever::ProcessMouseVC (int event, VECTOR3 &p)
{
	component->Activate (event & PANEL_MOUSE_LBDOWN ? DeltaGlider::DOOR_OPENING : DeltaGlider::DOOR_CLOSING);
	return false;
}


// ==============================================================
// Escape ladder control
// ==============================================================

EscapeLadderCtrl::EscapeLadderCtrl (DockingCtrlSubsystem *_subsys)
: DGSubsystemComponent(_subsys)
{
	ladder_status     = DeltaGlider::DOOR_CLOSED;
	ladder_proc       = 0.0;

	ELID_SWITCH = AddElement (sw = new LadderSwitch (this));

	// Escape ladder animation
	static UINT LadderGrp[2] = {GRP_Ladder1,GRP_Ladder2};
	static MGROUP_TRANSLATE Ladder1 (0, LadderGrp, 2, _V(0,0,1.1));
	static MGROUP_ROTATE Ladder2 (0, LadderGrp, 2,
		_V(0,-1.05,9.85), _V(1,0,0), (float)(80*RAD));
	anim_ladder = DG()->CreateAnimation (0);
	DG()->AddAnimationComponent (anim_ladder, 0, 0.5, &Ladder1);
	DG()->AddAnimationComponent (anim_ladder, 0.5, 1, &Ladder2);
}

// --------------------------------------------------------------

void EscapeLadderCtrl::Activate (DeltaGlider::DoorStatus action)
{
	extern void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd=0);

	bool close = (action == DeltaGlider::DOOR_CLOSED || action == DeltaGlider::DOOR_CLOSING);
	if (!close && ((DockingCtrlSubsystem*)Subsys())->NoseconeStatus() != DeltaGlider::DOOR_OPEN) return;
	// don't extend ladder if nosecone is closed

	ladder_status = action;
	if (action <= DeltaGlider::DOOR_OPEN) {
		ladder_proc = (action == DeltaGlider::DOOR_CLOSED ? 0.0 : 1.0);
		DG()->SetAnimation (anim_ladder, ladder_proc);
	}
	UpdateCtrlDialog(DG());
	DG()->RecordEvent ("LADDER", close ? "CLOSE" : "OPEN");
}

// --------------------------------------------------------------

void EscapeLadderCtrl::clbkPostCreation ()
{
	DG()->SetAnimation (anim_ladder, ladder_proc);
}

// --------------------------------------------------------------

void EscapeLadderCtrl::clbkPostStep (double simt, double simdt, double mjd)
{
	// animate escape ladder
	if (ladder_status >= DeltaGlider::DOOR_CLOSING) {
		double da = simdt * LADDER_OPERATING_SPEED;
		if (ladder_status == DeltaGlider::DOOR_CLOSING) {
			if (ladder_proc > 0.0)
				ladder_proc = max (0.0, ladder_proc-da);
			else {
				ladder_status = DeltaGlider::DOOR_CLOSED;
			}
		} else {
			if (ladder_proc < 1.0)
				ladder_proc = min (1.0, ladder_proc+da);
			else {
				ladder_status = DeltaGlider::DOOR_OPEN;
			}
		}
		DG()->SetAnimation (anim_ladder, ladder_proc);
	}
}

// --------------------------------------------------------------

void EscapeLadderCtrl::clbkSaveState (FILEHANDLE scn)
{
	if (ladder_status) {
		char cbuf[256];
		sprintf (cbuf, "%d %0.4f", ladder_status, ladder_proc);
		oapiWriteScenario_string (scn, "LADDER", cbuf);
	}
}

// --------------------------------------------------------------

bool EscapeLadderCtrl::clbkParseScenarioLine (const char *line)
{
	if (!_strnicmp (line, "LADDER", 6)) {
		sscanf (line+6, "%d%lf", &ladder_status, &ladder_proc);
		return true;
	}
	return false;
}

// --------------------------------------------------------------

bool EscapeLadderCtrl::clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	if (panelid != 0) return false;

	SURFHANDLE panel2dtex = oapiGetTextureHandle(DG()->panelmesh0,1);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_SWITCH), _R(1171,496,1197,548), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP, panel2dtex, sw);
	sw->DefineAnimation2D (DG()->panelmesh0, GRP_INSTRUMENTS_ABOVE_P0, 44);

	return true;
}

// --------------------------------------------------------------

bool EscapeLadderCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Ladder extend/retract switch
	oapiVCRegisterArea (GlobalElId(ELID_SWITCH), PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_SWITCH), VC_ELADDER_SWITCH_mousearea[0], VC_ELADDER_SWITCH_mousearea[1], VC_ELADDER_SWITCH_mousearea[2], VC_ELADDER_SWITCH_mousearea[3]);
	sw->DefineAnimationVC (VC_ELADDER_SWITCH_ref, VC_ELADDER_SWITCH_axis, GRP_SWITCH1_VC, VC_ELADDER_SWITCH_vofs);

	return true;
}

// ==============================================================

LadderSwitch::LadderSwitch (EscapeLadderCtrl *comp)
: DGSwitch1(comp->DG(), DGSwitch1::SPRING), component(comp)
{
}

// --------------------------------------------------------------

bool LadderSwitch::ProcessMouse2D (int event, int mx, int my)
{
	if (DGSwitch1::ProcessMouse2D (event, mx, my)) {
		DGSwitch1::State state = GetState();
		switch (state) {
			case DGSwitch1::UP: component->Activate (DeltaGlider::DOOR_CLOSING); break;
			case DGSwitch1::DOWN: component->Activate (DeltaGlider::DOOR_OPENING); break;
		}
		return true;
	}
	return false;
}

// --------------------------------------------------------------

bool LadderSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (DGSwitch1::ProcessMouseVC (event, p)) {
		DGSwitch1::State state = GetState();
		switch (state) {
			case DGSwitch1::UP:   component->Activate (DeltaGlider::DOOR_CLOSING); break;
			case DGSwitch1::DOWN: component->Activate (DeltaGlider::DOOR_OPENING); break;
		}
		return true;
	}
	return false;
}


// ==============================================================
// Dock seal control
// ==============================================================

DocksealCtrl::DocksealCtrl (DockingCtrlSubsystem *_subsys)
: DGSubsystemComponent (_subsys)
{
	isDocked = false;
	dockTime = -1e10;

	ELID_INDICATOR = AddElement (indicator = new DocksealIndicator (this));
}

// --------------------------------------------------------------

void DocksealCtrl::SetDockStatus (bool docked)
{
	isDocked = docked;
	dockTime = oapiGetSimTime();
	if (!docked || dockTime < 1.0) {
		dockTime -= 1e10;
		isSealing = false;
	} else isSealing = true;
	oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));
}

// --------------------------------------------------------------

void DocksealCtrl::clbkPostStep (double simt, double simdt, double mjd)
{
	if (isSealing) {
		oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));
		isSealing = (simt-simdt-dockTime <= 10.0);
	}
}

// --------------------------------------------------------------

void DocksealCtrl::clbkPostCreation ()
{
	DOCKHANDLE hDock = DG()->GetDockHandle (0);
	OBJHANDLE mate = DG()->GetDockStatus(hDock);
	if (mate) {
		isDocked = true;
		oapiTriggerRedrawArea (0, 0, GlobalElId(ELID_INDICATOR));
	}
}

// --------------------------------------------------------------

bool DocksealCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// dock seal indicator
	oapiVCRegisterArea (GlobalElId(ELID_INDICATOR), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);

	return false;
}

// ==============================================================

DocksealIndicator::DocksealIndicator (DocksealCtrl *comp)
: PanelElement(comp->DG()), component(comp)
{
	light = false;
}

// --------------------------------------------------------------

bool DocksealIndicator::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	if (!hMesh) return false;

	bool showlights = false;
	double d, dt;
	if (component->isDocked) {
		if ((dt = oapiGetSimTime()-component->dockTime) > 10.0)
			showlights = true;
		else
			showlights = (modf (dt, &d) < 0.5);
	}
	if (showlights != light) {
		GROUPEDITSPEC ges;
		static WORD vtxofs = VC_SEAL_INDICATOR_vofs;
		static const DWORD nvtx = 4;
		static WORD vidx[nvtx] = {vtxofs,vtxofs+1,vtxofs+2,vtxofs+3};
		static float u[2] = {0.0586f,0.0713f};
		NTVERTEX vtx[nvtx];
		for (DWORD i = 0; i < nvtx; i++)
			vtx[i].tu = u[showlights ? 1:0];
		ges.flags = GRPEDIT_VTXTEXU;
		ges.Vtx = vtx;
		ges.vIdx = vidx;
		ges.nVtx = nvtx;
		oapiEditMeshGroup (hMesh, GRP_VC4_LIT_VC, &ges);
		light = showlights;
	}
	return false;
}