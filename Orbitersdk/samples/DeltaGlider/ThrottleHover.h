// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// ThrottleHover.h
// Hover throttle sliders
// ==============================================================

#ifndef __THROTTLEHOVER_H
#define __THROTTLEHOVER_H

#include "..\Common\Vessel\Instrument.h"

class DeltaGlider;

class ThrottleHover: public PanelElement {
public:
	ThrottleHover (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);

private:
	float ppos;
};

#endif // !__THROTTLEHOVER_H