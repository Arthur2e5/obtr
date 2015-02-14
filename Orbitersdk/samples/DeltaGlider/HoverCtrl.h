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

// ==============================================================

class HoverCtrlDial: public PanelElement {
public:
	HoverCtrlDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
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

class PHoverCtrl: public PanelElement {
public:
	PHoverCtrl (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int vc_state;
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch];
	WORD vperm[nvtx_per_switch];
};

// ==============================================================

class RHoverCtrl: public PanelElement {
public:
	RHoverCtrl (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int vc_state;
	static const int nvtx_per_switch = 28;
	NTVERTEX vtx0[nvtx_per_switch];
	WORD vperm[nvtx_per_switch];
};

#endif // !__HOVERCTRL_H