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
}

// --------------------------------------------------------------

AerodynCtrlSubsystem::~AerodynCtrlSubsystem ()
{
	// delete components
	delete selector;
}

// --------------------------------------------------------------

void AerodynCtrlSubsystem::SetMode (DWORD mode)
{
	selector->SetMode (mode);
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