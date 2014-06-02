// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// Airbrake.h
// Airbrake control
// ==============================================================

#ifndef __AIRBRAKE_H
#define __AIRBRAKE_H

#include "..\Common\Vessel\Instrument.h"

class Airbrake: public PanelElement {
public:
	Airbrake (VESSEL3 *v);
	//void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int state;
};

#endif // !__AIRBAKE_H