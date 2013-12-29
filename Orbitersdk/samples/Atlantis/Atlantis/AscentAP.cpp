// ==============================================================
//                 ORBITER MODULE: Atlantis
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2012 Martin Schweiger
//                   All rights reserved
//
// AscentAP.cpp
// Class implementation for Atlantis ascent autopilot
// Automatic control of ascent profile from liftoff to
// ET separation using engine gimballing of SSME and SRB engines
// ==============================================================

#include "Atlantis.h"
#include "AscentAP.h"
#include "resource.h"
//#include "Uxtheme.h"
#include "Common\Dialog\Graph.h"

extern GDIParams g_Param;

// ==============================================================
// class AscentAP: ascent autopilot
// ==============================================================

AscentAP::AscentAP (Atlantis *atlantis)
{
	vessel = atlantis;
	n_pitch_profile = 0;
	active = false;
	met = 0.0;
	SetDefaultProfiles();
	dlg = new AscentAPDlg (this);
}

// --------------------------------------------------------------

AscentAP::~AscentAP ()
{
	if (n_pitch_profile) delete []pitch_profile;
	delete dlg;
}

// --------------------------------------------------------------

void AscentAP::Update (double simt)
{
	if (active) {
		met = simt-t_launch;
		dlg->Update (simt);
		if (vessel->status == 0) {
			if (met < 0.0) {
				vessel->SetEngineLevel (ENGINE_MAIN, min (1.0, (5.0+met)*0.4));
			} else {
				if (vessel->status == 0) {
					t_launch = vessel->t0 = simt;
					vessel->pET->IgniteSRBs ();
					vessel->status = 1;
				}
			}
		}
		if (vessel->status < 3) {
			double apalt;
			OBJHANDLE hRef = vessel->GetApDist(apalt);
			apalt -= oapiGetSize (hRef);
			if (apalt >= tgt_alt)
				vessel->SetThrusterGroupLevel (THGROUP_MAIN, 0.0);
		}
	}
}

// --------------------------------------------------------------

void AscentAP::Launch (double azimuth, double alt)
{
	if (!active && vessel->status == 0 && vessel->pET) {
		double r;
		tgt_launch_azimuth = azimuth;
		tgt_alt = alt;
		vessel->GetEquPos(launch_lng, launch_lat, r);
		t_launch = oapiGetSimTime()+5.0;
		vessel->SetAttitudeMode (RCS_NONE);
		active = true;
	}
}

// --------------------------------------------------------------

void AscentAP::SetDefaultProfiles ()
{
	int i;
	const int n_pitch = 20;
	double p_met[n_pitch] = { 0,  5,   10,   20,   30,   40,   50,   60,   70,   80,   90, 100,  120,  140,  164, 195, 250, 300,  420,  530};
	double p_val[n_pitch] = {90, 90, 80.2, 69.8, 63.2, 57.4, 52.4, 46.8, 43.2, 38.6, 34.8,  32, 26.4, 19.9, 13.8,  10,   6,   3, -1.2, -5.2};

	if (n_pitch_profile) delete []pitch_profile;
	n_pitch_profile = n_pitch;
	pitch_profile = new ProfSample[n_pitch];
	for (i = 0; i < n_pitch; i++) {
		pitch_profile[i].t = p_met[i];
		pitch_profile[i].v = p_val[i]*RAD;
	}

	tgt_launch_azimuth = PI05;
	tgt_alt = 350e3;
	t_roll_upright = 345.0;
}

// --------------------------------------------------------------

double AscentAP::GetTargetAzimuth () const
{
	double lng, lat, r;
	vessel->GetEquPos(lng, lat, r);

	// calculate downrange distance
	double B = lng-launch_lng;
	double c = PI05-launch_lat;
	double a = PI05-lat;
	double b = acos(cos(a)*cos(c) + sin(a)*sin(c)*cos(B));

	// calculate target position for this downrange distance
	double a_tgt = acos(cos(b)*cos(c) + sin(c)*sin(b)*cos(tgt_launch_azimuth));
	double B_tgt = asin(sin(b)*sin(tgt_launch_azimuth)/sin(a_tgt));
	//double tgt_lng = B_tgt+launch_lng;
	//double tgt_lat = PI05-a_tgt;

	// calcuate azimuth at target position
	double C_tgt = PI-asin(sin(c)*sin(B_tgt)/sin(b));
	return C_tgt;
}

// --------------------------------------------------------------

void AscentAP::GetTargetDirection (double met, VECTOR3 &dir, double &tgt_hdg) const
{
	double tgt_pitch, xz;
	if (met > pitch_profile[n_pitch_profile-1].t) {
		tgt_pitch = pitch_profile[n_pitch_profile-1].v;
	} else {
		int i;
		for (i = 0; i < n_pitch_profile-1 && pitch_profile[i+1].t < met; i++);
		tgt_pitch = pitch_profile[i].v +
			(pitch_profile[i+1].v - pitch_profile[i].v) * (met-pitch_profile[i].t) / (pitch_profile[i+1].t - pitch_profile[i].t);
	}
	if (met >= t_roll_upright) {
		const double pitch_ofs = 15.1*RAD;
		double bank = vessel->GetBank();
		tgt_pitch += (cos(bank)+1)*pitch_ofs;
	}
	tgt_hdg = GetTargetAzimuth();
	xz = cos(tgt_pitch);

	vessel->HorizonInvRot(_V(xz*sin(tgt_hdg), sin(tgt_pitch), xz*cos(tgt_hdg)), dir);
}

// --------------------------------------------------------------

void AscentAP::GetTargetRate (double met, VECTOR3 &rate) const
{
	if (active) {
		const double pitch_ofs = 15.1*RAD;
		double tgt_hdg;

		rate.x = rate.y = rate.z = 0.0;
		if (met <= 5.0) return;

		VECTOR3 tgtdir, avel;
		GetTargetDirection (met, tgtdir, tgt_hdg);
		vessel->GetAngularVel (avel);

		double dpitch = -asin(tgtdir.y);
		double dyaw   = -atan2(tgtdir.x, tgtdir.z);
		rate.x = GetTargetPitchRate (dpitch, avel.x);
		rate.y = (met < 35.0 ? 0.0 : GetTargetYawRate (dyaw, avel.y));
		rate.z = (met <= 35.0 ? GetTargetRollRate (tgt_hdg, true) :
				                GetTargetRollRate (met <= t_roll_upright ? PI : 0, false));
	} else {
		rate.x = rate.y = rate.z = 0.0;
	}
}

// --------------------------------------------------------------

double AscentAP::GetTargetPitchRate (double dpitch, double vpitch) const
{
	const double a = -0.15;
	const double b =  0.15;
	if      (dpitch >= PI) dpitch -= PI2;
	else if (dpitch < -PI) dpitch += PI2;
	double bank = vessel->GetBank();
	return a*dpitch + b*vpitch;

}

// --------------------------------------------------------------

double AscentAP::GetTargetYawRate (double dyaw, double vyaw) const
{
	const double a = 0.10;
	const double b = 0.10;
	if      (dyaw >= PI) dyaw -= PI2;
	else if (dyaw < -PI) dyaw += PI2;
	return a*dyaw + b*vyaw;
}

// --------------------------------------------------------------

double AscentAP::GetTargetRollRate (double tgt, bool tgt_is_heading) const
{
	double a, b, maxrate;
	if (tgt_is_heading) { // launch roll
		a = 0.60;
		b = 0.20;
		maxrate = 0.25;
	} else {              // post launch roll
		a = 0.15;
		b = 0.05;
		maxrate = 0.15;
	}

	VECTOR3 avel, yh;
	vessel->GetAngularVel (avel);
	double dh, droll = avel.z;

	if (tgt_is_heading) {
		vessel->HorizonRot (_V(0,1,0), yh);
		double yhdg = atan2(yh.x, yh.z);
		dh = yhdg-tgt;
		if (dh > PI) dh -= PI2;
		else if (dh < -PI) dh += PI2;
	} else {
		double bank = vessel->GetBank();
		dh = bank-tgt;
		if (dh >= PI) dh -= PI2;
		else if (dh < -PI) dh += PI2;
	}

	double rate = min (maxrate, max (-maxrate, a*dh + b*droll));
	
	return rate;
}

// --------------------------------------------------------------

void AscentAP::OpenDialog ()
{
	dlg->Open (g_Param.hDLL);
}


// ==============================================================
// class AscentAPDlg: dialog interface for ascent autopilot
// ==============================================================

AscentAPDlg::AscentAPDlg(AscentAP *_ap): TabbedDialog (IDD_ASCENTAP, IDC_TAB1)
{
	ap = _ap;
}

// --------------------------------------------------------------

AscentAPDlg::~AscentAPDlg ()
{
}

// --------------------------------------------------------------

void AscentAPDlg::Update (double simt)
{
	if (DlgHandle()) {
		static char title[64] = "Atlantis Launch Autopilot | MET ";
		strcpy (title+32, MetStr (ap->met)); 
		SetWindowText (DlgHandle(), title);
		for (int i = 0; i < TabCount(); i++)
			Tab(i)->Update (simt);
	}
}

// --------------------------------------------------------------

const char *AscentAPDlg::MetStr (double met) const
{
	static char str[32];
	int h, m;
	if (met < 0.0) {
		str[0] = '-';
		met = -met;
	} else
		str[0] = ' ';
	h = (int)(met/3600.0);
	met -= h*3600;
	m = (int)(met/60.0);
	met -= m*60;
	int nh = (h < 100 ? 2:3);
	h = min(h,999);
	sprintf (str+1, "%0*d:%02d:%04.1f", nh, h, m, met);
	return str;
}

// --------------------------------------------------------------

int AscentAPDlg::OnInitDialog (WPARAM wParam)
{
	AddTab (new AscentAPDlgTabControl (this), "Control");
	AddTab (new AscentAPDlgTabGimbal (this), "Gimbal");
	AddTab (new AscentAPDlgTabThrust (this), "Thrust");
	AddTab (new AscentAPDlgTabAltitude (this), "Altitude");
	return TabbedDialog::OnInitDialog (wParam);
}


// ==============================================================
// class AscentAPDlgTab: base class for dialog tabs
// ==============================================================

AscentAPDlgTab::AscentAPDlgTab (AscentAPDlg *frame, int dlgId)
: TabPage (frame, dlgId)
{
	ap = frame->AP();
}


// ==============================================================
// class AscentAPDlgTabControl: AP control tab
// ==============================================================

AscentAPDlgTabControl::AscentAPDlgTabControl (AscentAPDlg *frame)
: AscentAPDlgTab (frame, IDD_ASCENTAP_CTRL)
{
}

// --------------------------------------------------------------

int AscentAPDlgTabControl::OnInitTab (WPARAM wParam)
{
	char cbuf[256];
	sprintf (cbuf, "%0.1f", ap->GetLaunchAzimuth()*DEG);
	SetWindowText (GetDlgItem (TabHandle(), IDC_AZIMUTH), cbuf);
	sprintf (cbuf, "%0.1f", ap->GetOrbitAltitude()*1e-3);
	SetWindowText (GetDlgItem (TabHandle(), IDC_ALT), cbuf);
	return TRUE;
}

// --------------------------------------------------------------

int AscentAPDlgTabControl::OnLaunch ()
{
	char cbuf[256];
	double azimuth, alt;
	GetWindowText (GetDlgItem (TabHandle(), IDC_AZIMUTH), cbuf, 256);
	sscanf (cbuf, "%lf", &azimuth);
	azimuth *= RAD;
	GetWindowText (GetDlgItem (TabHandle(), IDC_ALT), cbuf, 256);
	sscanf (cbuf, "%lf", &alt);
	alt *= 1e3;
	ap->Launch (azimuth, alt);
	return TRUE;
}

// --------------------------------------------------------------

int AscentAPDlgTabControl::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) {
	case IDC_LAUNCH:
		return OnLaunch();
	}
	return TabPage::OnCommand (wParam, lParam);
}


// ==============================================================
// class AscentAPDlgTabGimbal: AP gimbal tab
// ==============================================================

AscentAPDlgTabGimbal::AscentAPDlgTabGimbal (AscentAPDlg *frame)
: AscentAPDlgTab (frame, IDD_ASCENTAP_GIMBAL)
{
	pen1 = CreatePen (PS_SOLID, 0, 0xB0B0B0);
	pen2 = CreatePen (PS_SOLID, 0, 0x0000FF);
}

// --------------------------------------------------------------

AscentAPDlgTabGimbal::~AscentAPDlgTabGimbal ()
{
	DeleteObject (pen1);
	DeleteObject (pen2);
}

// --------------------------------------------------------------

BOOL AscentAPDlgTabGimbal::DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG: {
		RECT rect;
		GetClientRect (GetDlgItem (hWnd, IDC_SSME_L), &rect);
		rad = min (rect.right-rect.left, rect.bottom-rect.top)*0.5;
		} return TRUE;
	case WM_PAINT:
		RepaintAll (hWnd);
		break;
	}
	return FALSE;
}

// --------------------------------------------------------------

void AscentAPDlgTabGimbal::Update (double simt)
{
	const double range = 10.5*RAD;
	double pitch, yaw;
	int i, x, y;
	int DlgId[5] = {IDC_SSME_L, IDC_SSME_R, IDC_SSME_U, IDC_SRB_L, IDC_SRB_R};
	for (i = 0; i < 5; i++) {
		HWND hCtrl = GetDlgItem (TabHandle(), DlgId[i]);
		if (i < 3)
			ap->GetVessel()->GetSSMEGimbalPos (i, pitch, yaw);
		else
			ap->GetVessel()->GetSRBGimbalPos (i-3, pitch, yaw);
		UpdateGimbalCross (hCtrl, i, pitch, yaw);
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabGimbal::UpdateGimbalCross (HWND hCtrl, int idx, double pitch, double yaw)
{
	const double range = 10.5*RAD;
	int x, y;
	x = (int)(yaw/range * rad + 0.5);
	y = (int)(pitch/range * rad + 0.5);
	if (x != gimbalx[idx] || y != gimbaly[idx]) {
		HDC hDC = GetDC (hCtrl);
		RECT rect;
		GetClientRect (hCtrl, &rect);
		int cntx = (rect.left+rect.right)/2;
		int cnty = (rect.top+rect.bottom)/2;
		HPEN ppen = (HPEN)SelectObject (hDC, GetStockObject (WHITE_PEN));
		SelectObject (hDC, GetStockObject (NULL_BRUSH));
		PaintGimbalCross (hDC, rect, gimbalx[idx], gimbaly[idx]);
		SelectObject (hDC, pen1);
		MoveToEx (hDC, rect.left, cnty, NULL); LineTo (hDC, rect.right, cnty);
		MoveToEx (hDC, cntx, rect.top, NULL); LineTo (hDC, cntx, rect.bottom);
		Rectangle (hDC, (rect.left+cntx)/2, (rect.top+cnty)/2, (rect.right+cntx)/2, (rect.bottom+cnty)/2);
		SelectObject (hDC, GetStockObject (BLACK_PEN));
		Rectangle (hDC, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject (hDC, pen2);
		PaintGimbalCross (hDC, rect, gimbalx[idx]=x, gimbaly[idx]=y);
		SelectObject (hDC, ppen);
		ReleaseDC (hCtrl, hDC);
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabGimbal::PaintGimbalCross (HDC hDC, const RECT &rect, int x, int y)
{
	int xmin, xmax, ymin, ymax, cntx, cnty;
	xmin = rect.left, xmax = rect.right;
	ymin = rect.top, ymax = rect.bottom;
	cntx = (xmin+xmax)/2;
	cnty = (ymin+ymax)/2;
	x += cntx, y += cnty;
	if (x >= xmin && x < xmax) {
		MoveToEx (hDC, x, max(y-10, ymin), NULL);
		LineTo (hDC, x, min(y+11, ymax));
	}
	if (y >= ymin && y < ymax) {
		MoveToEx (hDC, max(x-10, xmin), y, NULL);
		LineTo (hDC, min(x+11,xmax), y);
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabGimbal::RepaintAll (HWND hWnd)
{
	int DlgId[5] = {IDC_SSME_L, IDC_SSME_R, IDC_SSME_U, IDC_SRB_L, IDC_SRB_R};
	for (int i = 0; i < 5; i++) {
		HWND hCtrl = GetDlgItem (hWnd, DlgId[i]);
		InvalidateRect (hCtrl, NULL, FALSE);
		UpdateWindow (hCtrl);
		PaintGimbalBox (hCtrl);
		gimbalx[i] = gimbaly[i] = 0;
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabGimbal::PaintGimbalBox (HWND hWnd)
{
	RECT rect;
	int cntx, cnty;
	HDC hDC = GetDC (hWnd);
	GetClientRect (hWnd, &rect);
	cntx = (rect.right+rect.left)/2;
	cnty = (rect.bottom+rect.top)/2;
	SelectObject (hDC, GetStockObject (WHITE_BRUSH));
	SelectObject (hDC, GetStockObject (BLACK_PEN));
	Rectangle (hDC, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject (hDC, pen1);
	MoveToEx (hDC, rect.left, cnty, NULL); LineTo (hDC, rect.right, cnty);
	MoveToEx (hDC, cntx, rect.top, NULL); LineTo (hDC, cntx, rect.bottom);
	SelectObject (hDC, GetStockObject (BLACK_PEN));
	ReleaseDC (hWnd, hDC);
}


// ==============================================================
// class AscentAPDlgTabThrust: AP thrust tab
// ==============================================================

AscentAPDlgTabThrust::AscentAPDlgTabThrust (AscentAPDlg *frame)
: AscentAPDlgTab (frame, IDD_ASCENTAP_THRUST)
{
	Graph::InitGDI ();
	ssmegraph = new Graph(1);
	ssmegraph->SetTitle ("SSME thrust");
	ssmegraph->SetYLabel ("Thrust [%]");
	srbgraph = new Graph(1);
	srbgraph->SetTitle ("SRB thrust");
	srbgraph->SetYLabel ("Thrust [%]");
	updt = oapiGetSimTime();
	dupdt = 1.0;
}

// --------------------------------------------------------------

AscentAPDlgTabThrust::~AscentAPDlgTabThrust ()
{
	delete ssmegraph;
	delete srbgraph;
	Graph::FreeGDI();
}

// --------------------------------------------------------------

void AscentAPDlgTabThrust::Update (double simt)
{
	if (ap->Active() && simt >= updt) {
		double lvl;
		lvl = ap->GetVessel()->GetThrusterGroupLevel(THGROUP_MAIN);
		ssmegraph->AppendDataPoint (lvl);
		lvl = ap->GetVessel()->GetSRBThrustLevel(0);
		srbgraph->AppendDataPoint (lvl);
		RefreshGraph (ssmegraph, IDC_SSMETHRUST);
		RefreshGraph (srbgraph, IDC_SRBTHRUST);
		updt += dupdt;
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabThrust::RefreshGraph (Graph *graph, int GraphId)
{

	HWND hCtrl = GetDlgItem (TabHandle(), GraphId);
	InvalidateRect (hCtrl, NULL, TRUE);
	UpdateWindow (hCtrl);
	RECT rect;
	HDC hDC = GetDC (hCtrl);
	GetClientRect (hCtrl, &rect);
	graph->Refresh (hDC, rect.right-rect.left, rect.bottom-rect.top);
	ReleaseDC (hCtrl, hDC);
}

// --------------------------------------------------------------

int AscentAPDlgTabThrust::OnPaint ()
{
	RefreshGraph (ssmegraph, IDC_SSMETHRUST);
	RefreshGraph (srbgraph, IDC_SRBTHRUST);
	return FALSE;
}

// --------------------------------------------------------------

BOOL AscentAPDlgTabThrust::DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_PAINT:
		return OnPaint ();
	}
	return FALSE;
}


// ==============================================================
// class AscentAPDlgTabAltitude: AP altitude tab
// ==============================================================

AscentAPDlgTabAltitude::AscentAPDlgTabAltitude (AscentAPDlg *frame)
: AscentAPDlgTab (frame, IDD_ASCENTAP_ALT)
{
	Graph::InitGDI ();
	altgraph = new Graph(1);
	altgraph->SetTitle ("Altitude");
	altgraph->SetYLabel ("alt [km]");
	updt = oapiGetSimTime();
	dupdt = 1.0;
}

// --------------------------------------------------------------

AscentAPDlgTabAltitude::~AscentAPDlgTabAltitude ()
{
	delete altgraph;
	Graph::FreeGDI();
}

// --------------------------------------------------------------

void AscentAPDlgTabAltitude::Update (double simt)
{
	if (ap->Active() && simt >= updt) {
		double alt;
		alt = ap->GetVessel()->GetAltitude()*1e-3;
		altgraph->AppendDataPoint (alt);
		RefreshGraph (altgraph, IDC_ALTITUDE);
		updt += dupdt;
	}
}

// --------------------------------------------------------------

void AscentAPDlgTabAltitude::RefreshGraph (Graph *graph, int GraphId)
{

	HWND hCtrl = GetDlgItem (TabHandle(), GraphId);
	InvalidateRect (hCtrl, NULL, TRUE);
	UpdateWindow (hCtrl);
	RECT rect;
	HDC hDC = GetDC (hCtrl);
	GetClientRect (hCtrl, &rect);
	graph->Refresh (hDC, rect.right-rect.left, rect.bottom-rect.top);
	ReleaseDC (hCtrl, hDC);
}

// --------------------------------------------------------------

int AscentAPDlgTabAltitude::OnPaint ()
{
	RefreshGraph (altgraph, IDC_ALTITUDE);
	return FALSE;
}

// --------------------------------------------------------------

BOOL AscentAPDlgTabAltitude::DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_PAINT:
		return OnPaint ();
	}
	return FALSE;
}

