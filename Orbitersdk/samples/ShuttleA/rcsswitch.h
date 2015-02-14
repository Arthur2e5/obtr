// ==============================================================
//                 ORBITER MODULE: ShuttleA
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// rcsswitch.h
// User interface RCS switch
// ==============================================================

#ifndef __RCSSWITCH_H
#define __RCSSWITCH_H

#include "ShuttleA.h"
#include "..\Common\Vessel\Instrument.h"

// ==============================================================

class RCSSwitch: public PanelElement {
public:
	RCSSwitch (VESSEL3 *v);
	void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
};

#endif // !__RCSSWITCH_H