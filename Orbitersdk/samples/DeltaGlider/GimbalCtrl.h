// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// GimbalCtrl.h
// Class for main engine gimbal control subsystem
// ==============================================================

#ifndef __GIMBALCTRL_H
#define __GIMBALCTRL_H

#include "DeltaGlider.h"
#include "DGSwitches.h"

// ==============================================================
// Main engine gimbal control subsystem
// ==============================================================

class MainGimbalDial;
class PMainGimbalCtrl;
class YMainGimbalCtrl;
class MainGimbalDisp;

class GimbalControl: public DGSubSystem {
	friend class PMainGimbalCtrl;
	friend class YMainGimbalCtrl;

public:
	GimbalControl (DeltaGlider *vessel, int ident);
	int Mode() const { return mode; }
	void SetMode (int newmode) { mode = newmode; }
	inline double MainPGimbal (int which, bool actual=true) const
	{ return actual ? mpgimbal[which]: mpgimbal_cmd[which]; }  // return main engine gimbal pitch
	inline double MainYGimbal (int which, bool actual=true) const
	{ return actual ? mygimbal[which] : mygimbal_cmd[which]; } // return main engine gimbal yaw
	void SetMainPGimbal (int which, double lvl);               // command a pitch gimbal value
	void SetMainYGimbal (int which, double lvl);               // command a yaw gimbal value
	bool IncMainPGimbal (int which, int mode);                 // manually change gimbal pitch command
	bool IncMainYGimbal (int which, int mode);                 // manually change gimbal yaw command
	void AutoMainGimbal ();                                    // apply automatic main engine gimbal setting
	void TrackMainGimbal ();                                   // follow gimbals to commanded values
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	void clbkReset2D (int panelid, MESHHANDLE hMesh);
	bool clbkLoadVC (int vcid);
	void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);

private:
	int mode;                     // gimbal mode: 0=off, 1=auto, 2=manual
	double mpgimbal[2], mpgimbal_cmd[2]; // current/commanded main engine pitch gimbal angle (tan)
	double mygimbal[2], mygimbal_cmd[2]; // current/commanded main engine yaw gimbal angle (tan)
	int mpswitch[2], mpmode; // main gimbal pitch button states
	int myswitch[2], mymode; // main gimbal yaw button states

	MainGimbalDial *modedial;       // mode dial object
	PMainGimbalCtrl *pgimbalswitch; // pitch gimbal switch object
	YMainGimbalCtrl *ygimbalswitch; // yaw gimbal switch object
	MainGimbalDisp *gimbaldisp;     // gimbal display object

	int ELID_MODEDIAL;            // element ID: mode dial
	int ELID_PGIMBALSWITCH;       // element ID: pitch gimbal switches
	int ELID_YGIMBALSWITCH;       // element ID: yaw gimbal switches
	int ELID_DISPLAY;             // element ID: gimbal display
};

// ==============================================================

class MainGimbalDial: public DGDial1 {
public:
	MainGimbalDial (GimbalControl *gc);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	GimbalControl *ctrl;
};

// ==============================================================

class MainGimbalDisp: public PanelElement {
public:
	MainGimbalDisp (GimbalControl *gc);
	~MainGimbalDisp ();
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	GimbalControl *ctrl;
	int pofs_cur[2], pofs_cmd[2];
	int yofs_cur[2], yofs_cmd[2];
	GROUPREQUESTSPEC vc_grp; ///< Buffered VC vertex data
};

// ==============================================================

class PMainGimbalCtrl: public PanelElement {
public:
	PMainGimbalCtrl (GimbalControl *gc);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	GimbalControl *ctrl;
	int vc_state[2];
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch*2];
};

// ==============================================================

class YMainGimbalCtrl: public PanelElement {
public:
	YMainGimbalCtrl (GimbalControl *gc);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	GimbalControl *ctrl;
	int vc_state[2];
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch*2];
	WORD vperm[nvtx_per_switch*2];
};

#endif // !__GIMBALCTRL_H