// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// AvionicsSubsys.h
// Subsystem for avionics components:
// - angular rate indicator
// ==============================================================

#ifndef __AVIONICSSUBSYS_H
#define __AVIONICSSUBSYS_H

#include "DGSubsys.h"

// ==============================================================
// Avionics subsystem
// ==============================================================

class AngRateIndicator;

class AvionicsSubsystem: public DGSubsystem {
public:
	AvionicsSubsystem (DeltaGlider *v, int ident);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);

private:
	AngRateIndicator *angrateind;
	int ELID_ANGRATEIND;
};

#endif // !__AVIONICSSUBSYS_H