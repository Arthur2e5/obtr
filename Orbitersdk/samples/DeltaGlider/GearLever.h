// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// GearLever.h
// Gear up/down lever interface
// ==============================================================

#ifndef __GEARLEVER_H
#define __GEARLEVER_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================

class GearLever: public PanelElement {
public:
	GearLever (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================

class GearIndicator: public PanelElement {
public:
	GearIndicator (VESSEL3 *v);
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	//void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	double tofs;
	bool light;
};

#endif // !__GEARLEVER_H