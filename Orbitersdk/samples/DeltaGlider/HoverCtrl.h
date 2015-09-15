// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HoverCtrl.h
// Hover controls and displays
// ==============================================================

#ifndef __HOVERCTRL_H
#define __HOVERCTRL_H

#include "DeltaGlider.h"
#include "DGSwitches.h"

// ==============================================================

class HoverCtrlDial: public DGDial1 {
public:
	HoverCtrlDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================

class HoverDisp: public PanelElement {
public:
	HoverDisp (VESSEL3 *v);
	~HoverDisp ();
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	int pofs_cur, pofs_cmd;
	int rofs_cur, rofs_cmd;
	GROUPREQUESTSPEC vc_grp; ///< Buffered VC vertex data
	WORD vperm[8];
};

// ==============================================================

class PHoverCtrl: public DGSwitch2 {
public:
	PHoverCtrl (VESSEL3 *v);
	//void Reset2D (MESHHANDLE hMesh);
	//bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================

class RHoverCtrl: public DGSwitch2 {
public:
	RHoverCtrl (VESSEL3 *v);
	//void Reset2D (MESHHANDLE hMesh);
	//bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================
// ==============================================================
// Hover hold altitude control subsystem

class HoverHoldAltControl: public DGSubSystem {
	friend class HoverHoldAltIndicator;

public:
	HoverHoldAltControl (DeltaGlider *vessel, int ident);
	void Activate (bool ison);
	double TargetAlt() const { return holdalt; }
	void SetTargetAlt (double alt);
	void SetTargetAltCurrent ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadVC (int vcid);
	enum HoldMode { HOLDMODE_NONE, HOLDMODE_ALT, HOLDMODE_RATE };

private:
	double holdalt;   // current hold altitude
	bool active;      // hover hold altitude active?
	VESSEL::AltitudeMode altmode;
	HoldMode holdmode;
	static const int AID_HOLDALT_DISP;
	static const int AID_HOLDALT_BTN;
	static const int AID_HOLDALT_SELECT;
	static const int AID_HOLDALT_SETCUR;
};

// ==============================================================
// Hover hold altitude button

class HoverAltBtn: public DGButton3 {
public:
	HoverAltBtn (HoverHoldAltControl *hhac);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverHoldAltControl *ctrl;
};

// ==============================================================
// Hover altitude/rate selector switch

class HoverAltSwitch: public DGSwitch2 {
public:
	HoverAltSwitch (HoverHoldAltControl *hhac);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverHoldAltControl *ctrl;
};

// ==============================================================
// Hover "copy current" button

class HoverAltCurBtn: public DGButton2 {
public:
	HoverAltCurBtn (HoverHoldAltControl *hhac);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverHoldAltControl *ctrl;
};

// ==============================================================
// Hover hold altitude indicator displays

class HoverHoldAltIndicator: public PanelElement {
public:
	HoverHoldAltIndicator (HoverHoldAltControl *hhac, SURFHANDLE blitsrc);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE hSurf);

private:
	void UpdateReadout (const char *tgtstr, char *curstr);
	HoverHoldAltControl *ctrl;
	SURFHANDLE btgt, bsrc;
	HoverHoldAltControl::HoldMode holdmode_disp;
	bool hold_disp;
	char holdstr[10];   // current hold altitude readout
};

#endif // !__HOVERCTRL_H