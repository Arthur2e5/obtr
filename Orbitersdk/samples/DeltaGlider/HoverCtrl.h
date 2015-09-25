// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// HoverCtrl.h
// Classes for Hover balance control and Hover altitude subsystems
// ==============================================================

#ifndef __HOVERCTRL_H
#define __HOVERCTRL_H

#include "DeltaGlider.h"
#include "DGSwitches.h"

// ==============================================================
// Hover balance control subsystem
// ==============================================================

class HoverCtrlDial;
class PHoverCtrl;
class RHoverCtrl;
class HoverDisp;

class HoverBalanceControl: public DGSubSystem {
public:
	HoverBalanceControl (DeltaGlider *vessel, int ident);
	inline int Mode() const { return mode; }
	inline void SetMode (int newmode) { mode = newmode; }
	inline double PHover (bool actual=true) const { return actual ? phover : phover_cmd; }
	inline double RHover (bool actual=true) const { return actual ? rhover : rhover_cmd; }
	bool IncPHover (int dir);     // manually change hover pitch command
	bool IncRHover (int dir);     // manually change hover roll command
	void AutoHoverAtt ();         // set hover pitch/roll commands from user input
	void TrackHoverAtt ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	void clbkReset2D (int panelid, MESHHANDLE hMesh);
	bool clbkLoadVC (int vcid);
	void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);

private:
	int mode;                     // balance mode: 0=off, 1=auto, 2=manual
	double phover, phover_cmd;    // current/commanded hover pitch angle (tan)
	double rhover, rhover_cmd;    // current/commanded hover roll angle (tan)

	HoverCtrlDial *modedial;      // mode dial object
	PHoverCtrl    *phoverswitch;  // pitch hover balance switch object
	RHoverCtrl    *rhoverswitch;  // roll hover balance switch object
	HoverDisp     *hoverdisp;     // hover balance display object

	int ELID_MODEDIAL;            // element ID: mode dial
	int ELID_PHOVERSWITCH;        // element ID: pitch hover balance switch
	int ELID_RHOVERSWITCH;        // element ID: roll hover balance switch
	int ELID_DISPLAY;             // element ID: balance display
};

// ==============================================================

class HoverCtrlDial: public DGDial1 {
public:
	HoverCtrlDial (HoverBalanceControl *hbc);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverBalanceControl *ctrl;
};

// ==============================================================

class HoverDisp: public PanelElement {
public:
	HoverDisp (HoverBalanceControl *hbc);
	~HoverDisp ();
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	HoverBalanceControl *ctrl;
	int pofs_cur, pofs_cmd;
	int rofs_cur, rofs_cmd;
	GROUPREQUESTSPEC vc_grp; ///< Buffered VC vertex data
	WORD vperm[8];
};

// ==============================================================

class PHoverCtrl: public DGSwitch2 {
public:
	PHoverCtrl (HoverBalanceControl *hbc);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverBalanceControl *ctrl;
};

// ==============================================================

class RHoverCtrl: public DGSwitch2 {
public:
	RHoverCtrl (HoverBalanceControl *hbc);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	HoverBalanceControl *ctrl;
};


// ==============================================================
// Hover hold altitude control subsystem
// ==============================================================

class HoverAltBtn;
class HoverAltSwitch;
class HoverAltCurBtn;
class HoverAltModeButtons;

class HoverHoldAltControl: public DGSubSystem {
	friend class HoverHoldAltIndicator;

public:
	enum HoverMode { HOLD_NONE, HOLD_ALT, HOLD_VSPD };

	HoverHoldAltControl (DeltaGlider *vessel, int ident);
	void Activate (bool ison);
	double TargetAlt() const { return holdalt; }
	void SetTargetAlt (double alt);
	void SetTargetAltCurrent ();
	double TargetVspd() const { return holdvspd; }
	void SetTargetVspd (double vspd);
	double TargetPrm() const { return (hovermode == HOLD_ALT ? holdalt : holdvspd); }
	void SetTargetPrm (double prm);
	void SetHoverMode (HoverMode mode);
	HoverMode GetHoverMode () const { return hovermode; }
	void HoverHoldVspd (double vspd);
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadVC (int vcid);

private:
	double holdalt;   // current hold altitude
	double holdvspd;  // current hold vertical speed
	double holdT;     // for time interval
	double pvh;       // previous vertical speed
	bool active;      // hover hold altitude active?
	VESSEL::AltitudeMode altmode;
	HoverMode hovermode;

	HoverAltBtn *holdbtn;
	HoverAltSwitch *altset;
	HoverAltCurBtn *altcur;
	HoverAltModeButtons *modebuttons;

	int ELID_DISPLAY;
	int ELID_HOLDBTN;
	int ELID_ALTSET;
	int ELID_ALTCURRENT;
	int ELID_MODEBUTTONS;
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
// Hover mode buttons

class HoverAltModeButtons: public PanelElement {
public:
	HoverAltModeButtons (HoverHoldAltControl *hhac);
	~HoverAltModeButtons();
	void DefineAnimationsVC (const VECTOR3 &axis, DWORD meshgrp, DWORD meshgrp_label,
		DWORD vofs[2], DWORD vofs_label[2]);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouseVC (int event, VECTOR3 &p);
	void ResetVC (DEVMESHHANDLE hMesh);

private:
	HoverHoldAltControl *ctrl;
	DGButton3 *btn[2];
	HoverHoldAltControl::HoverMode vmode; // currently displayed hover mode
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
	HoverHoldAltControl::HoverMode holdmode_disp;
	bool hold_disp;
	char holdstr[10];   // current hold altitude readout
};

#endif // !__HOVERCTRL_H