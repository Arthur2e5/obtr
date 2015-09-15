// ==============================================================
//             ORBITER MODULE: Common vessel tools
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// Instrument.h
// Interface for class PanelElement:
//   Base class for panel and VC instrument visualisations
// Interface for class SubSystem:
//   Base class for a vessel subsystem: acts as a container for
//   a group of panel elements and underlying system logic
// ==============================================================

#ifndef __INSTRUMENT_H
#define __INSTRUMENT_H

#include "Orbitersdk.h"

class VESSEL3;

// ==============================================================

class PanelElement {
public:
	PanelElement (VESSEL3 *v);
	virtual ~PanelElement ();

	virtual void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void AddMeshDataVC (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void Reset2D ();
	virtual void Reset2D (MESHHANDLE hMesh);
	virtual void ResetVC (DEVMESHHANDLE hMesh);
	virtual bool Redraw2D (SURFHANDLE surf);
	virtual bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	virtual bool ProcessMouse2D (int event, int mx, int my);
	virtual bool ProcessMouseVC (int event, VECTOR3 &p);

protected:
	void AddGeometry (MESHHANDLE hMesh, DWORD grpidx, const NTVERTEX *vtx, DWORD nvtx, const WORD *idx, DWORD nidx);

	char *DispStr (double dist, int precision=4);

	VESSEL3 *vessel;
	MESHHANDLE mesh;
	DWORD gidx;
	MESHGROUP *grp; // panel mesh group representing the instrument
	DWORD vtxofs;   // vertex offset in mesh group
};

// ==============================================================

class SubSystem {
public:
	SubSystem (VESSEL3 *v, int ident);
	// create a new subsystem for vessel 'v' with identifier 'ident'

	virtual ~SubSystem ();
	inline VESSEL3 *Vessel() const { return vessel; }
	inline int Id() const { return id; }
	inline int Global(int elementid) const { return elementid + (id+1)*1000; } // map a local element id to global id
	virtual void clbkPostStep (double simt, double simdt, double mjd) {}
	virtual bool clbkLoadVC (int vcid);  // create the VC elements for the subsystem
	virtual void clbkResetVC (DEVMESHHANDLE hMesh);
	virtual bool clbkVCRedrawEvent (int id, int event, DEVMESHHANDLE hMesh, SURFHANDLE hSurf);
	virtual bool clbkVCMouseEvent (int id, int event, VECTOR3 &p);

protected:
	PanelElement **element;
	DWORD nelement;

private:
	VESSEL3 *vessel;
	int id;
};

#endif // !__INSTRUMENT_H