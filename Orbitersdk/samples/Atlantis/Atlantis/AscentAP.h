// ==============================================================
//                 ORBITER MODULE: Atlantis
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2012 Martin Schweiger
//                   All rights reserved
//
// AscentAP.h
// Class interface for Atlantis ascent autopilot
// Automatic control of ascent profile from liftoff to
// ET separation using engine gimballing of SSME and SRB engines
// ==============================================================

#ifndef __ATLANTIS_ASCENTAP
#define __ATLANTIS_ASCENTAP

#include "Common\Dialog\TabDlg.h"

class Atlantis;
class Graph;

struct ProfSample {
	double t;
	double v;
};

// ==============================================================
// class AscentAP: ascent autopilot
// ==============================================================

class AscentAP {
	friend class AscentAPDlg;

public:
	AscentAP (Atlantis *atlantis);
	~AscentAP ();

	Atlantis *GetVessel () { return vessel; }
	void Launch (double azimuth, double alt);
	void Update (double simt);
	bool Active() const { return active; }
	double GetMET () const { return oapiGetSimTime() - t_launch; }
	double GetLaunchAzimuth () const { return tgt_launch_azimuth; }
	double GetOrbitAltitude () const { return tgt_alt; }
	double GetTargetAzimuth () const;
	void GetTargetDirection (double met, VECTOR3 &dir, double &tgt_hdg) const;
	void GetTargetRate (double met, VECTOR3 &rate) const;

	void OpenDialog ();

protected:
	void SetDefaultProfiles ();

private:
	double GetTargetPitchRate (double dpitch, double vpitch) const;
	double GetTargetYawRate (double dyaw, double vyaw) const;
	double GetTargetRollRate (double tgt, bool tgt_is_heading) const;

	Atlantis *vessel;

	ProfSample *pitch_profile;
	int n_pitch_profile;
	double tgt_launch_azimuth;
	double tgt_alt;
	double t_roll_upright;
	double launch_lng, launch_lat;
	double t_launch;
	double met;
	bool active;

	AscentAPDlg *dlg;
};

// ==============================================================
// class AscentAPDlg: dialog interface for ascent autopilot
// ==============================================================

class AscentAPDlgTab;

class AscentAPDlg: public TabbedDialog {
public:
	AscentAPDlg (AscentAP *_ap);
	~AscentAPDlg ();
	void Update (double simt);
	AscentAP *AP() { return ap; }
	int OnInitDialog (WPARAM wParam);

private:
	const char *MetStr (double met) const;
	AscentAP *ap;
};

// ==============================================================
// class AscentAPDlgTab: base class for dialog tabs
// ==============================================================

class AscentAPDlgTab: public TabPage {
public:
	AscentAPDlgTab (AscentAPDlg *frame, int dlgId);

protected:
	AscentAP *ap;
};

// ==============================================================
// class AscentAPDlgTabControl: AP control tab
// ==============================================================

class AscentAPDlgTabControl: public AscentAPDlgTab {
public:
	AscentAPDlgTabControl (AscentAPDlg *frame);

protected:
	int OnInitTab (WPARAM wParam);
	int OnLaunch ();
	int OnCommand (WPARAM wParam, LPARAM lParam);
};

// ==============================================================
// class AscentAPDlgTabGimbal: AP gimbal tab
// ==============================================================

class AscentAPDlgTabGimbal: public AscentAPDlgTab {
public:
	AscentAPDlgTabGimbal (AscentAPDlg *frame);
	~AscentAPDlgTabGimbal();
	void Update (double simt);

protected:
	void RepaintAll (HWND hWnd);
	void PaintGimbalCross (HDC hDC, const RECT &rect, int x, int y);
	void UpdateGimbalCross (HWND hCtrl, int idx, double pitch, double yaw);
	void PaintGimbalBox (HWND hWnd);
	BOOL DlgProc (HWND, UINT, WPARAM, LPARAM);

private:
	int gimbalx[5], gimbaly[5];
	double rad;
	HPEN pen1, pen2;
};

// ==============================================================
// class AscentAPDlgTabThrust: AP thrust tab
// ==============================================================

class AscentAPDlgTabThrust: public AscentAPDlgTab {
public:
	AscentAPDlgTabThrust (AscentAPDlg *frame);
	~AscentAPDlgTabThrust ();
	void Update (double simt);

protected:
	int OnPaint ();
	void RefreshGraph (Graph *graph, int GraphId);
	BOOL DlgProc (HWND, UINT, WPARAM, LPARAM);

private:
	Graph *ssmegraph, *srbgraph;
	double updt;
	double dupdt;
};

// ==============================================================
// class AscentAPDlgTabAltitude: AP altitude tab
// ==============================================================

class AscentAPDlgTabAltitude: public AscentAPDlgTab {
public:
	AscentAPDlgTabAltitude (AscentAPDlg *frame);
	~AscentAPDlgTabAltitude ();
	void Update (double simt);

protected:
	int OnPaint ();
	void RefreshGraph (Graph *graph, int GraphId);
	BOOL DlgProc (HWND, UINT, WPARAM, LPARAM);

private:
	Graph *altgraph;
	double updt;
	double dupdt;
};

#endif // !__ATLANTIS_ASCENTAP