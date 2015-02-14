// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DGSwitches.h
// Prototypes for DG-specific cockpit switches and dials
// ==============================================================

#ifndef __DGSWITCHES_H
#define __DGSWITCHES_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================
// Flip switch with two or three discrete states:
// up-down (TWOSTATE) or up-down-centered (THREESTATE)
// SPRING automatically returns to center position when released
// SPRING switches must be defined with PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP flag
// otherwise PANEL_MOUSE_LBDOWN is enough

class DGSwitch1: public PanelElement {
public:
	enum Mode { TWOSTATE, THREESTATE, SPRING };
	enum State { CENTER, UP, DOWN };
	DGSwitch1 (VESSEL3 *v, Mode m=TWOSTATE);
	void DefineAnimationVC (const VECTOR3 &ref, const VECTOR3 &axis,
		int meshgrp, int vtxofs);
	bool ProcessMouseVC (int event, VECTOR3 &p);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

	inline State GetState() const { return state; }
	bool SetState (State s);
	State Up (); // move to next state up and return new state
	State Down (); // move to next state down and return new state

private:
	Mode mode;         // switch mode
	State state;       // logical state
	State vstate;      // current visual state
	VECTOR3 rf, ax;    // rotation reference point, axis
	int grp, vofs;     // mesh group vertex offset
	const static int nvtx;   // number of vertices in switch lever
	static double travel; // switch lever angle from center to up position [rad]
};

#endif // !__DGSWITCHES_H