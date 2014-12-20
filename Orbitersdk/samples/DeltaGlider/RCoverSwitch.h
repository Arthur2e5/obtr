// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2014 Martin Schweiger
//                   All rights reserved
//
// RCoverSwitch.h
// User interface for retro engine switch
// ==============================================================

#ifndef __RCOVERSWITCH_H
#define __RCOVERSWITCH_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================

class RCoverSwitch: public PanelElement {
public:
	RCoverSwitch (VESSEL3 *v);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

#endif // !__RCOVERSWITCH_H