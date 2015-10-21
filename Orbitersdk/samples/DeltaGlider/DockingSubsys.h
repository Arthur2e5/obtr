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
#include "DGSwitches.h"

// ==============================================================
// Docking control subsystem
// ==============================================================

class NoseconeCtrl;
class UndockCtrl;
class EscapeLadderCtrl;
class DocksealCtrl;

class DockingCtrlSubsystem: public DGSubsystem {
public:
	DockingCtrlSubsystem (DeltaGlider *v, int ident);
	~DockingCtrlSubsystem ();

	DeltaGlider::DoorStatus NoseconeStatus () const;
	double NoseconePosition () const;
	const double *NoseconePositionPtr() const;
	void ActivateNosecone (DeltaGlider::DoorStatus action);
	void RevertNosecone ();

	DeltaGlider::DoorStatus LadderStatus () const;
	void ActivateLadder (DeltaGlider::DoorStatus action);

	void clbkDockEvent (int dock, OBJHANDLE mate);

private:
	NoseconeCtrl *noseconectrl;
	UndockCtrl *undockctrl;
	EscapeLadderCtrl *eladderctrl;
	DocksealCtrl *dsealctrl;
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


// ==============================================================
// Undock control
// ==============================================================

class UndockCtrl: public DGSubsystemComponent {
	friend class UndockLever;

public:
	UndockCtrl (DockingCtrlSubsystem *_subsys);
	void Activate (DeltaGlider::DoorStatus action);
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkLoadVC (int vcid);

private:
	UndockLever *lever;

	int ELID_LEVER;

	UINT anim_undocklever;      // handle for undock lever animation

	DeltaGlider::DoorStatus undock_status;
	double undock_proc;
};

// ==============================================================

class UndockLever: public PanelElement {
public:
	UndockLever (UndockCtrl *comp);
	void Reset2D (MESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	UndockCtrl *component;
	bool btndown;
};


// ==============================================================
// Escape ladder control
// ==============================================================

class EscapeLadderCtrl: public DGSubsystemComponent {
	friend class LadderSwitch;

public:
	EscapeLadderCtrl (DockingCtrlSubsystem *_subsys);
	void Activate (DeltaGlider::DoorStatus action);
	inline DeltaGlider::DoorStatus Status () const { return ladder_status; }
	void clbkPostCreation ();
	void clbkPostStep (double simt, double simdt, double mjd);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkParseScenarioLine (const char *line);
	bool clbkLoadVC (int vcid);

private:
	LadderSwitch *sw;
	int ELID_SWITCH;
	DeltaGlider::DoorStatus ladder_status;
	double ladder_proc;
	UINT anim_ladder;           // handle for front escape ladder animation
};

// ==============================================================

class LadderSwitch: public DGSwitch1 {
public:
	LadderSwitch (EscapeLadderCtrl *comp);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	EscapeLadderCtrl *component;
	bool light;
};


// ==============================================================
// Dock seal control
// ==============================================================

class DocksealCtrl: public DGSubsystemComponent {
	friend class DocksealIndicator;

public:
	DocksealCtrl (DockingCtrlSubsystem *_subsys);
	void SetDockStatus (bool docked);
	bool clbkLoadVC (int vcid);
	void clbkPostStep (double simt, double simdt, double mjd);
	void clbkPostCreation ();

private:
	DocksealIndicator *indicator;
	int ELID_INDICATOR;
	bool isDocked;
	bool isSealing;
	double dockTime;
};

// ==============================================================

class DocksealIndicator: public PanelElement {
public:
	DocksealIndicator (DocksealCtrl *comp);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

private:
	DocksealCtrl *component;
	bool light;
};

#endif // !__DOCKINGSUBSYS_H