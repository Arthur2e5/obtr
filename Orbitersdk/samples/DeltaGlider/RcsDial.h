// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// RcsDial.h
// RCS selector dial user interface
// ==============================================================

#ifndef __RCSDIAL_H
#define __RCSDIAL_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================

class RCSDial: public PanelElement {
public:
	RCSDial (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__RCSDIAL_H