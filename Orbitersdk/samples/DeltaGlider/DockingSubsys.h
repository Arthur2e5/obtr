// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DockingSubsys.h
// Nosecone and undock controls
// ==============================================================

#ifndef __DOCKINGSUBSYS_H
#define __DOCKINGSUBSYS_H

#include "DeltaGlider.h"
#include "DGSubsys.h"

// ==============================================================
// Docking control subsystem
// ==============================================================

class NoseconeCtrl;

class DockingCtrlSubsystem: public DGSubsystem {
public:
	DockingCtrlSubsystem (DeltaGlider *v, int ident);
	~DockingCtrlSubsystem ();
	DeltaGlider::DoorStatus NoseconeStatus () const;
	double NoseconePosition () const;
	const double *NoseconePositionPtr() const;
	void ActivateNosecone (DeltaGlider::DoorStatus action);
	void RevertNosecone ();

private:
	NoseconeCtrl *noseconectrl;
};


// ==============================================================
// Nosecone control
// ==============================================================

class NoseconeCtrl: public DGSubsystemComponent {
	friend class NoseconeLever;
	friend class NoseconeIndicator;

public:
	NoseconeCtrl (DockingCtrlSubsystem *_subsys);
	inline DeltaGlider::DoorStatus Status () const { return nose_status; }
	inline double Position () const { return nose_proc; }
	inline const double *PositionPtr() const { return &nose_proc; }
	void Activate (DeltaGlider::DoorStatus action);
	void Revert ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	void clbkPostCreation ();

private:
	NoseconeLever *lever;
	NoseconeIndicator *indicator;

	int ELID_LEVER;
	int ELID_INDICATOR;

	UINT anim_nose;             // handle for nose cone animation
	UINT anim_noselever;        // handle for nose cone lever animation

	DeltaGlider::DoorStatus nose_status, noselever_status;
	double nose_proc, noselever_proc;
};

// ==============================================================

class NoseconeLever: public PanelElement {
public:
	NoseconeLever (NoseconeCtrl *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	NoseconeCtrl *component;
};

// ==============================================================

class NoseconeIndicator: public PanelElement {
public:
	NoseconeIndicator (NoseconeCtrl *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	NoseconeCtrl *component;
	double tofs;
	bool light;
};

#endif // !__DOCKINGSUBSYS_H