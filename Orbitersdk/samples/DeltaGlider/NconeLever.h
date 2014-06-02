// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// NconeLever.h
// Nose cone open/close lever interface
// ==============================================================

#ifndef __NCONELEVER_H
#define __NCONELEVER_H

#include "DeltaGlider.h"

// ==============================================================

class NoseconeLever: public PanelElement {
public:
	NoseconeLever (VESSEL3 *v);
	void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);
};

// ==============================================================

class NoseconeIndicator: public PanelElement {
public:
	NoseconeIndicator (VESSEL3 *v);
	void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	double tofs;
	bool light;
};

#endif // !__NCONELEVER_H