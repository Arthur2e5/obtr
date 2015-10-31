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

class DGSubsystemComponent;

class DGSubsystem: public Subsystem {
public:
	DGSubsystem (DeltaGlider *v, int ident): Subsystem (v, ident) {}
	inline DeltaGlider *DG() { return (DeltaGlider*)Vessel(); }
	inline const DeltaGlider *DG() const { return (DeltaGlider*)Vessel(); }
	void AddComponent (DGSubsystemComponent *comp);
	virtual void clbkPostCreation ();
	virtual void clbkSaveState (FILEHANDLE scn);
	virtual bool clbkParseScenarioLine (const char *line);
	virtual void clbkPostStep (double simt, double simdt, double mjd);
	virtual bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	virtual bool clbkLoadVC (int vcid);
	virtual void clbkReset2D (int panelid, MESHHANDLE hMesh);
	virtual void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);
	virtual bool clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp);
	virtual void clbkRenderHUD (int mode, const HUDPAINTSPEC *hps, SURFHANDLE hTex);

protected:
	std::vector<DGSubsystemComponent*> component;
};

// ==============================================================

class DGSubsystemComponent {
public:
	DGSubsystemComponent (DGSubsystem *_subsys);
	virtual ~DGSubsystemComponent() {}
	inline DeltaGlider *DG() { return subsys->DG(); }
	virtual void clbkPostCreation () {}
	virtual void clbkSaveState (FILEHANDLE scn) {}
	virtual bool clbkParseScenarioLine (const char *line) { return false; }
	virtual void clbkPostStep (double simt, double simdt, double mjd) {}
	virtual bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) { return false; }
	virtual bool clbkLoadVC (int vcid) { return false; }
	virtual void clbkReset2D (int panelid, MESHHANDLE hMesh) {}
	virtual void clbkResetVC (int vcid, DEVMESHHANDLE hMesh) {}
	virtual bool clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp) { return false; }
	virtual void clbkRenderHUD (int mode, const HUDPAINTSPEC *hps, SURFHANDLE hTex) {}

protected:
	inline DGSubsystem *Subsys() { return subsys; }
	inline int AddElement (PanelElement *el) { return subsys->AddElement (el); }
	inline int GlobalElId (int localid) const { return subsys->GlobalElId (localid); }

private:
	DGSubsystem *subsys;
};

// ==============================================================

class DGPanelElement: public PanelElement {
public:
	DGPanelElement (DeltaGlider *_dg): PanelElement (dg), dg(_dg) {}

protected:
	DeltaGlider *dg;
};

#endif // !__DGSUBSYS_H