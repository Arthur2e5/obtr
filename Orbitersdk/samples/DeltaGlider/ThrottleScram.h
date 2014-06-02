// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ThrottleScram.h
// Scram throttle sliders
// ==============================================================

#ifndef __THROTTLESCRAM_H
#define __THROTTLESCRAM_H

#include "..\Common\Vessel\Instrument.h"

class DeltaGlider;

class ThrottleScram: public PanelElement {
public:
	ThrottleScram (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);

private:
	float ppos[2];
};

#endif // !__THROTTLESCRAM_H