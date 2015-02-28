// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// GimbalCtrl.h
// Gimbal controls and displays
// ==============================================================

#ifndef __GIMBALCTRL_H
#define __GIMBALCTRL_H

#include "DeltaGlider.h"
#include "DGSwitches.h"

// ==============================================================

class MainGimbalDial: public DGDial1 {
public:
	MainGimbalDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================

class MainGimbalDisp: public PanelElement {
public:
	MainGimbalDisp (VESSEL3 *v);
	~MainGimbalDisp ();
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	int pofs_cur[2], pofs_cmd[2];
	int yofs_cur[2], yofs_cmd[2];
	GROUPREQUESTSPEC vc_grp; ///< Buffered VC vertex data
};

// ==============================================================

class PMainGimbalCtrl: public PanelElement {
public:
	PMainGimbalCtrl (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int vc_state[2];
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch*2];
};

// ==============================================================

class YMainGimbalCtrl: public PanelElement {
public:
	YMainGimbalCtrl (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int vc_state[2];
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch*2];
	WORD vperm[nvtx_per_switch*2];
};

#endif // !__GIMBALCTRL_H