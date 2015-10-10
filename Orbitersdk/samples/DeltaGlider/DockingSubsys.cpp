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
}

// --------------------------------------------------------------

DockingCtrlSubsystem::~DockingCtrlSubsystem ()
{
	// delete components
	delete noseconectrl;
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
		vc_nconelever_ref, vc_nconelever_axis, (float)(-70*RAD));
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

	// TODO
	//if (close && ladder_status != DeltaGlider::DOOR_CLOSED)
	//	Subsys()->ActivateLadder (action); // retract ladder before closing the nose cone

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
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_LEVER), _R(1141,327,1180,421), PANEL_REDRAW_USER,  PANEL_MOUSE_LBDOWN, panel2dtex, lever);
	DG()->RegisterPanelArea (hPanel, GlobalElId(ELID_INDICATOR), _R(0,0,0,0), PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, indicator);

	return true;
}

// --------------------------------------------------------------

bool NoseconeCtrl::clbkLoadVC (int vcid)
{
	if (vcid != 0) return false;

	// Nosecone lever
	oapiVCRegisterArea (GlobalElId(ELID_LEVER), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	oapiVCSetAreaClickmode_Quadrilateral (GlobalElId(ELID_LEVER), vc_nconelever_mousearea[0], vc_nconelever_mousearea[1], vc_nconelever_mousearea[2], vc_nconelever_mousearea[3]);

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
	if (leverdown) y0 = 400.5f, dy = 21.0f, tv0 = texh-677.5f;
	else           y0 = 326.5f, dy = 19.0f, tv0 = texh-696.5f;
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
		static WORD vtxofs = 11;
		static const DWORD nvtx = 2;
		static WORD vidx[nvtx] = {vtxofs,vtxofs+1};
		static float v[2] = {0.2427f,0.3003f};
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
