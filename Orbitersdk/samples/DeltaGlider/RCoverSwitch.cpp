// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2014 Martin Schweiger
//                   All rights reserved
//
// RCoverSwitch.cpp
// User interface for retro engine switch
// ==============================================================

#define STRICT 1
#include "RCoverSwitch.h"
#include "DeltaGlider.h"

// ==============================================================

RCoverSwitch::RCoverSwitch (VESSEL3 *v): PanelElement (v)
{
}

// ==============================================================

bool RCoverSwitch::ProcessMouseVC (int event, VECTOR3 &p)
{
	DeltaGlider *dg = (DeltaGlider*)vessel;
	int pos = max(0, min (1, (int)(p.y*2.0)));
	dg->ActivateRCover (pos ? DeltaGlider::DOOR_CLOSING:DeltaGlider::DOOR_OPENING);
	return false;
}
