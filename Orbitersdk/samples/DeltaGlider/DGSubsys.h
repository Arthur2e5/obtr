// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DGSubsys.h
// Base classes for DG subsystems, subsystem components and
// panel elements
// ==============================================================

#ifndef __DGSUBSYS_H
#define __DGSUBSYS_H

#include "DeltaGlider.h"
#include "..\Common\Vessel\Instrument.h"
#include <vector>

// ==============================================================

class DGSubSystemComponent;

class DGSubSystem: public SubSystem {
public:
	DGSubSystem (DeltaGlider *v, int ident): SubSystem (v, ident) {}
	inline DeltaGlider *DG() { return (DeltaGlider*)Vessel(); }
	void AddComponent (DGSubSystemComponent *comp);
	virtual void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);

protected:
	std::vector<DGSubSystemComponent*> component;
};

// ==============================================================

class DGSubSystemComponent {
public:
	DGSubSystemComponent (DGSubSystem *_subsys);
	virtual ~DGSubSystemComponent() {}
	inline DeltaGlider *DG() { return subsys->DG(); }
	virtual void clbkPostStep (double simt, double simdt, double mjd) {}
	virtual bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) { return false; }
	virtual bool clbkLoadVC (int vcid) { return false; }

protected:
	inline DGSubSystem *Subsys() { return subsys; }
	inline int AddElement (PanelElement *el) { return subsys->AddElement (el); }
	inline int GlobalElId (int localid) const { return subsys->GlobalElId (localid); }

private:
	DGSubSystem *subsys;
};

// ==============================================================

class DGPanelElement: public PanelElement {
public:
	DGPanelElement (DeltaGlider *_dg): PanelElement (dg), dg(_dg) {}

protected:
	DeltaGlider *dg;
};

#endif // !__DGSUBSYS_H