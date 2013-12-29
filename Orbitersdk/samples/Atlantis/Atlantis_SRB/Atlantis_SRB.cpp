// ==============================================================
//                 ORBITER MODULE: Atlantis
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2003 Martin Schweiger
//                   All rights reserved
//
// Atlantis_SRB.cpp
// Reference implementation of Atlantis SRB(Space Shuttle - Solid
// Rocket Booster) vessel class module
// Note: This module takes control of the SRB after separation
// from the Shuttle's main tank.
// ==============================================================

#define ORBITER_MODULE
#define ATLANTIS_SRB_MODULE

#include "Atlantis.h"
#include "math.h"
#include "stdio.h"

// ==============================================================
// Specialised vessel class Atlantis_SRB
// ==============================================================

static const int ntdvtx = 13;
static const TOUCHDOWNVTX tdvtx[ntdvtx] = {
	{_V( 0,      2.5, -21.18), 1e9, 1e7, 0.3 },
	{_V( 2.1651,-1.25,-21.18), 1e9, 1e7, 0.3 },
	{_V(-2.1651,-1.25,-21.18), 1e9, 1e7, 0.3 },
	{_V( 2.1651, 1.25,-21.18), 1e9, 1e7, 0.3 },
	{_V( 0,     -2.5 ,-21.18), 1e9, 1e7, 0.3 },
	{_V(-2.1651, 1.25,-21.18), 1e9, 1e7, 0.3 },
	{_V( 0,      1.85, 18.95), 1e9, 1e7, 0.3 },
	{_V( 1.6021,-0.925,18.95), 1e9, 1e7, 0.3 },
	{_V(-1.6021,-0.925,18.95), 1e9, 1e7, 0.3 },
	{_V( 1.6021, 0.925,18.95), 1e9, 1e7, 0.3 },
	{_V( 0,     -1.85, 18.95), 1e9, 1e7, 0.3 },
	{_V(-1.6021, 0.925,18.95), 1e9, 1e7, 0.3 },
	{_V( 0,      0,    23.78), 1e9, 1e7, 0.3 }
};

// Constructor
Atlantis_SRB::Atlantis_SRB (OBJHANDLE hObj)
: VESSEL2(hObj)
{
	// preload mesh
	hSRBMesh = oapiLoadMeshGlobal ("Atlantis_srb");
}

void Atlantis_SRB::SetLaunchElevation (double elev)
{
	if (launchelev = elev) {
		TOUCHDOWNVTX launch_tdvtx[ntdvtx];
		memcpy (launch_tdvtx, tdvtx, ntdvtx*sizeof(TOUCHDOWNVTX));
		for (int i = 0; i < 6; i++)
			launch_tdvtx[i].pos.z -= elev;
		SetTouchdownPoints (launch_tdvtx, ntdvtx);
		SetSize (23.0+elev);
	}
}

// reconstruct liftoff time from fuel level
void Atlantis_SRB::SetRefTime (void)
{
	extern int SRB_nt;
	extern double SRB_Seq[], SRB_Prop[], SRB_PrpSCL[];

	int i;
	double fuel = GetFuelMass()/GetMaxFuelMass();
	for (i = 1; i < SRB_nt; i++)
		if (fuel >= SRB_Prop[i]) break;
	double met = SRB_Seq[i] + (fuel-SRB_Prop[i])/SRB_PrpSCL[i-1];
	t0 = oapiGetSimTime()-met;
}

double Atlantis_SRB::ThrustProfile (double met)
{
	// This thrust profile is adapted from STS 107 Columbia Accident
	// Investigation Board Working Scenario report
	// http://caib.nasa.gov/news/working_scenario/pdf/sts107workingscenario.pdf

	const int nsample = 9;
	static const double ts[nsample] = {
		 0, 8, 22, 50, 78, 110, 117, 126, 135
	};
	static const double lvls[nsample] = {
		0.9153, 0.9772, 1.0000, 0.7329, 0.8306, 0.5375, 0.1954, 0.05, 0
	};
	int i;
	double lvl;
	if (met <= 0 || met >= ts[nsample-1])
		lvl = 0.0;
	else {
		for (i = nsample-1; i >= 0; i--)
			if (met >= ts[i]) break;
		lvl = (met-ts[i])/(ts[i+1]-ts[i]) * (lvls[i+1]-lvls[i]) + lvls[i];
	}
	return lvl;
}

double Atlantis_SRB::GetThrustLevel () const
{
	return GetThrusterLevel (th_main);
}

bool Atlantis_SRB::Ignite ()
{
	if (GetPropellantMass (ph_main) == SRB_MAX_PROPELLANT_MASS) {
		SetThrusterLevel (th_main, 1.0);
		t0 = oapiGetSimTime();
		bMainEngine = true;
		if (launchelev) {
			SetTouchdownPoints (tdvtx, ntdvtx); // reset touchdown  points
			SetSize (23.0);
		}
		return true;
	}
	return false;
}

void Atlantis_SRB::FireBolt ()
{
	SetThrusterLevel (th_bolt, 1.0);
	tsep = oapiGetSimTime();
	bSeparationEngine = true;
}

void Atlantis_SRB::SetThrustGimbal (const VECTOR3 &dir)
{
	SetThrusterDir (th_main, dir);
}

VECTOR3 Atlantis_SRB::GetThrustDir ()
{
	VECTOR3 dir;
	GetThrusterDir (th_main, dir);
	return dir;
}

// ==============================================================
// Callback functions
// ==============================================================

// Set SRB class specs
void Atlantis_SRB::clbkSetClassCaps (FILEHANDLE cfg)
{
	extern PARTICLESTREAMSPEC srb_contrail, srb_exhaust;
	PARTICLESTREAMSPEC srb_bolt = {
		0, 8.0, 20, 0.0, 0.1, 0.3, 16, 3.0, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_LIN, 0, 1,
		PARTICLESTREAMSPEC::ATM_FLAT, 1, 1
	};

	SetEnableFocus (false);
	// SRB cannot receive input focus

	// *********************** physical parameters *********************************

	SetSize (23.0);
	SetEmptyMass (SRB_EMPTY_MASS);
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetCrossSections (_V(162.1,162.1,26.6));
	SetRotDrag (_V(0.7,0.7,0.1));
	SetPMI (_V(154.3,154.3,1.83));
	//SetGravityGradientDamping (10.0);
	SetTouchdownPoints (tdvtx, ntdvtx);
	SetLiftCoeffFunc (0);

	// ************************* docking port **************************************

	CreateDock (_V(1.95,0,5),_V(1,0,0),_V(0,0,1)); // ET attachment

	// ************************* propellant specs **********************************

	ph_main = CreatePropellantResource (SRB_MAX_PROPELLANT_MASS);

	// *********************** thruster definitions ********************************

	// main engine
	th_main = CreateThruster (_V(0,0,-21), THRUSTGIMBAL_LAUNCH, SRB_THRUST_MAX, ph_main, SRB_ISP0, SRB_ISP1);
	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust2");
	srb_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");
	AddExhaust (th_main, 16.0, 2.0, tex);
	AddExhaustStream (th_main, _V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main, _V(0,0,-25), &srb_exhaust);

	// separation bolts
	th_bolt = CreateThruster (_V(0,0,1.0), _V(-1,0,0), 3e6, ph_main, 1e7);
	// for simplicity, the separation bolts directly use SRB propellant. We give
	// them an insanely high ISP to avoid significant propellant drainage

	AddExhaust (th_bolt, 0.7, 0.1, _V(2.1,0,-8), _V(-1,0,0));
	AddExhaust (th_bolt, 0.7, 0.1, _V(2.1,0,11), _V(-1,0,0));
	AddExhaustStream (th_bolt, _V(2.1,0,0), &srb_bolt);

	// ************************ visual parameters **********************************

	AddMesh (hSRBMesh);

	bMainEngine = false;
	bSeparationEngine = false;
}

// Finish setup
void Atlantis_SRB::clbkPostCreation ()
{
	//SetRefTime ();	// reconstruct ignition time from fuel level
}

// Simulation time step
void Atlantis_SRB::clbkPostStep (double simt, double simdt, double mjd)
{
	double lvl = 0.0;
	double met = 0.0;
	if (bMainEngine) {
		met = simt-t0;
		lvl = ThrustProfile (met);
		SetThrusterLevel (th_main, lvl);
	}
	if (bSeparationEngine) {
		if (simt-tsep > 0.5) {
			DelThruster (th_bolt);
			bSeparationEngine = false;
		}
	}

#ifdef UNDEF
	extern void GetSRB_State (double, double&, double&);
	//sprintf (oapiDebugString(), "SRB mass = %f", GetMass());
	if (bMainEngine) {
		double met = simt-t0;
		if (met >= SRB_CUTOUT_TIME) {
			SetThrusterLevel (th_main, 0);
			bMainEngine = false;
		} else {
			double thrust_level, prop_level;
			GetSRB_State (met, thrust_level, prop_level);
			SetThrusterLevel (th_main, thrust_level);
		}
		if (bSeparationEngine) {
			static double bolt_t = 0.5;
			double srb_dt = simt - srb_separation_time;
			if (srb_dt > bolt_t) {
				DelThruster (th_bolt);
				bSeparationEngine = false;
			} else {
				SetThrusterLevel (th_bolt, sqrt (1.0-srb_dt/bolt_t));
			}
		}
	}
	//if (GetAltitude() < 0.0) oapiDeleteVessel (GetHandle());
#endif
}

// ==============================================================
// API interface
// ==============================================================

// Initialisation
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	return new Atlantis_SRB (hvessel);
}

// Cleanup
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (Atlantis_SRB*)vessel;
}
