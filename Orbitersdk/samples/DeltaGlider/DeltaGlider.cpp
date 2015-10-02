// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DeltaGlider.cpp
// Reference implementation of delta glider vessel class module
// ==============================================================

#define ORBITER_MODULE

#include "DeltaGlider.h"
#include "AAP.h"
#include "MFDButton.h"
#include "Horizon.h"
#include "InstrHsi.h"
#include "InstrAoa.h"
#include "InstrVs.h"
#include "AtctrlDial.h"
#include "NavButton.h"
#include "ElevTrim.h"
#include "Airbrake.h"
#include "UndockBtn.h"
#include "HudCtrl.h"
#include "GearLever.h"
#include "NconeLever.h"
#include "FuelMfd.h"
#include "ThrottleScram.h"
#include "MainRetroSubsys.h"
#include "HoverSubsys.h"
#include "RcsSubsys.h"
#include "SwitchArray.h"
#include "AirlockSwitch.h"
#include "Wheelbrake.h"
#include "MwsButton.h"
#include "MomentInd.h"
#include "CockpitLight.h"
#include "ScnEditorAPI.h"
#include "PressureCtrl.h"
#include "DlgCtrl.h"
#include "dg_vc_anim.h"
#include "meshres.h"
#include "meshres_vc.h"
#include "meshres_p0.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static HELPCONTEXT g_hc = {
	"html/vessels/deltaglider.chm",
	0,
	"html/vessels/deltaglider.chm::/deltaglider.hhc",
	"html/vessels/deltaglider.chm::/deltaglider.hhk"
};

static const DWORD ntdvtx_geardown = 13;
static TOUCHDOWNVTX tdvtx_geardown[ntdvtx_geardown] = {
	{_V( 0   ,-2.57,10   ), 1e6, 1e5, 1.6, 0.1},
	{_V(-3.5 ,-2.57,-1   ), 1e6, 1e5, 3.0, 0.2},
	{_V( 3.5 ,-2.57,-1   ), 1e6, 1e5, 3.0, 0.2},
	{_V(-8.5 ,-0.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 8.5 ,-0.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V(-8.5 ,-0.4 ,-3   ), 1e7, 1e5, 3.0},
	{_V( 8.5 ,-0.4 ,-3   ), 1e7, 1e5, 3.0},
	{_V(-8.85, 2.3 ,-5.05), 1e7, 1e5, 3.0},
	{_V( 8.85, 2.3 ,-5.05), 1e7, 1e5, 3.0},
	{_V(-8.85, 2.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 8.85, 2.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 0   , 2   , 6.2 ), 1e7, 1e5, 3.0},
	{_V( 0   ,-0.6 ,10.65), 1e7, 1e5, 3.0}
};

static const DWORD ntdvtx_gearup = 13;
static TOUCHDOWNVTX tdvtx_gearup[ntdvtx_gearup] = {
	{_V( 0   ,-1.5 ,9),     1e7, 1e5, 3.0, 3.0},
	{_V(-6   ,-0.8 ,-5),    1e7, 1e5, 3.0, 3.0},
	{_V( 3   ,-1.2 ,-5),    1e7, 1e5, 3.0, 3.0},
	{_V(-8.5 ,-0.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 8.5 ,-0.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V(-8.5 ,-0.4 ,-3   ), 1e7, 1e5, 3.0},
	{_V( 8.5 ,-0.4 ,-3   ), 1e7, 1e5, 3.0},
	{_V(-8.85, 2.3 ,-5.05), 1e7, 1e5, 3.0},
	{_V( 8.85, 2.3 ,-5.05), 1e7, 1e5, 3.0},
	{_V(-8.85, 2.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 8.85, 2.3 ,-7.05), 1e7, 1e5, 3.0},
	{_V( 0   , 2   , 6.2 ), 1e7, 1e5, 3.0},
	{_V( 0   ,-0.6 ,10.65), 1e7, 1e5, 3.0}
};

// ==============================================================
// Local prototypes

BOOL CALLBACK Ctrl_DlgProc (HWND, UINT, WPARAM, LPARAM);
void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd = 0);

//BOOL CALLBACK Damage_DlgProc (HWND, UINT, WPARAM, LPARAM);
//void UpdateDamageDialog (DeltaGlider *dg, HWND hWnd = 0);

// ==============================================================
// Airfoil coefficient functions
// Return lift, moment and zero-lift drag coefficients as a
// function of angle of attack (alpha or beta)
// ==============================================================

// 1. vertical lift component (wings and body)

void VLiftCoeff (VESSEL *v, double aoa, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	int i;
	const int nabsc = 9;
	static const double AOA[nabsc] = {-180*RAD,-60*RAD,-30*RAD, -2*RAD, 15*RAD,20*RAD,25*RAD,60*RAD,180*RAD};
	static const double CL[nabsc]  = {       0,      0,   -0.4,      0,    0.7,     1,   0.8,     0,      0};
	static const double CM[nabsc]  = {       0,      0,  0.014, 0.0039, -0.006,-0.008,-0.010,     0,      0};
	for (i = 0; i < nabsc-1 && AOA[i+1] < aoa; i++);
	double f = (aoa-AOA[i]) / (AOA[i+1]-AOA[i]);
	*cl = CL[i] + (CL[i+1]-CL[i]) * f;  // aoa-dependent lift coefficient
	*cm = CM[i] + (CM[i+1]-CM[i]) * f;  // aoa-dependent moment coefficient
	double saoa = sin(aoa);
	double pd = 0.015 + 0.4*saoa*saoa;  // profile drag
	*cd = pd + oapiGetInducedDrag (*cl, 1.5, 0.7) + oapiGetWaveDrag (M, 0.75, 1.0, 1.1, 0.04);
	// profile drag + (lift-)induced drag + transonic/supersonic wave (compressibility) drag
}

// 2. horizontal lift component (vertical stabilisers and body)

void HLiftCoeff (VESSEL *v, double beta, double M, double Re, void *context, double *cl, double *cm, double *cd)
{
	int i;
	const int nabsc = 8;
	static const double BETA[nabsc] = {-180*RAD,-135*RAD,-90*RAD,-45*RAD,45*RAD,90*RAD,135*RAD,180*RAD};
	static const double CL[nabsc]   = {       0,    +0.3,      0,   -0.3,  +0.3,     0,   -0.3,      0};
	for (i = 0; i < nabsc-1 && BETA[i+1] < beta; i++);
	*cl = CL[i] + (CL[i+1]-CL[i]) * (beta-BETA[i]) / (BETA[i+1]-BETA[i]);
	*cm = 0.0;
	*cd = 0.015 + oapiGetInducedDrag (*cl, 1.5, 0.6) + oapiGetWaveDrag (M, 0.75, 1.0, 1.1, 0.04);
}

// ==============================================================
// Specialised vessel class DeltaGlider
// ==============================================================

// --------------------------------------------------------------
// Constructor
// --------------------------------------------------------------
DeltaGlider::DeltaGlider (OBJHANDLE hObj, int fmodel)
: VESSEL4 (hObj, fmodel)
{
	int i, j;

	modelidx = (fmodel ? 1 : 0);

	// Subsystem definitions
	int ssys_id = 0;
	ssys.push_back (ssys_mainretro = new MainRetroSubsystem (this, ssys_id++));
	ssys.push_back (ssys_hoverctrl = new HoverSubsystem (this, ssys_id++));
	ssys.push_back (ssys_rcs       = new RcsSubsystem (this, ssys_id++));
	ssys.push_back (ssys_hud       = new HUDControl (this, ssys_id++));
	ssys.push_back (ssys_pressurectrl = new PressureControl (this, ssys_id++));

	aoa_ind = PI;
	slip_ind = PI*0.5;
	load_ind = PI;
	gear_status       = DOOR_CLOSED;
	gear_proc         = 0.0;
	gearlever_status  = DOOR_CLOSED;
	gearlever_proc    = 0.0;
	nose_status       = DOOR_CLOSED;
	nose_proc         = 0.0;
	noselever_status  = DOOR_CLOSED;
	noselever_proc    = 0.0;
	undock_status     = DOOR_CLOSED;
	undock_proc       = 0.0;
	ladder_status     = DOOR_CLOSED;
	ladder_proc       = 0.0;
	olock_status      = DOOR_CLOSED;
	olock_proc        = 0.0;
	ilock_status      = DOOR_CLOSED;
	ilock_proc        = 0.0;
	hatch_status      = DOOR_CLOSED;
	hatch_proc        = 0.0;
	brake_status      = DOOR_CLOSED;
	brake_proc        = 0.0;
	airbrakelever_status = DOOR_CLOSED;
	airbrakelever_proc   = 0.0;
	airbrake_tgt         = 0;
	radiator_status   = DOOR_CLOSED;
	radiator_proc     = 0.0;
	visual            = NULL;
	exmesh            = NULL;
	vcmesh            = NULL;
	vcmesh_tpl        = NULL;
	scramjet          = NULL;
	hatch_vent        = NULL;
	insignia_tex      = NULL;
	contrail_tex      = NULL;
	hPanelMesh        = NULL;
	campos            = CAM_GENERIC;
	th_main_level     = 0.0;
	cockpit_light     = NULL;
	docking_light     = NULL;
	instr_light_on    = false;
	flood_light_mode  = 0;
	landdock_light_mode = 0;
	strobe_light_on   = false;
	nav_light_on      = false;
	instr_brightness  = 0.5;
	flood_brightness  = 0.7;
	for (i = 0; i < 2; i++)
		panelcol[i]   = 0;
	skinpath[0] = '\0';
	for (i = 0; i < 3; i++)
		skin[i] = 0;
	for (i = 0; i < 4; i++)
		psngr[i] = false;
	for (i = 0; i < 2; i++) {
		scram_max[i] = 0.0;
		scram_intensity[i] = 0.0;
	}
	for (i = 0; i < 2; i++) {
		scflowidx[i] = 0;
		mainflowidx[i] = retroflowidx[i] = -1;
		scTSFCidx[i] = -1;
		mainpropidx[i] = rcspropidx[i] = scrampropidx[i] = -1;
	}
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++) rotidx[i][j] = 0;

	hbmode = hbswitch = 0;
	mainpropmass = rcspropmass = scrampropmass = -1;
	mainTSFCidx = hoverflowidx = -1;
	hbalanceidx = 28;
	
	ninstr = 0;

	// damage parameters
	bDamageEnabled = (GetDamageModel() != 0);
	bMWSActive = false;
	lwingstatus = rwingstatus = 1.0;
	hatchfail = 0;
	for (i = 0; i < 4; i++) aileronfail[i] = false;

	DefineAnimations();
	for (i = 0; i < nsurf; i++) srf[i] = 0;
}

// --------------------------------------------------------------
// Destructor
// --------------------------------------------------------------
DeltaGlider::~DeltaGlider ()
{
	DWORD i;

	if (scramjet) delete scramjet;

	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		delete *it;

	for (i = 0; i < ninstr; i++)
		if (instr[i]) delete instr[i];
	delete []instr;

	delete aap;

	if (insignia_tex) oapiDestroySurface(insignia_tex);

	if (contrail_tex) ReleaseSurfaces();

	for (i = 0; i < 3; i++)
		if (skin[i]) oapiReleaseTexture (skin[i]);
}

// --------------------------------------------------------------
// Set vessel mass excluding propellants
// --------------------------------------------------------------
void DeltaGlider::SetEmptyMass () const
{
	double emass = (scramjet ? EMPTY_MASS_SC : EMPTY_MASS);
	// add passengers+life support to empty vessel mass
	for (int i = 0; i < 4; i++)
		if (psngr[i]) emass += PSNGR_MASS;

	VESSEL4::SetEmptyMass (emass);
}

// --------------------------------------------------------------
// Create the instrument panel objects for the cockpit interface
// --------------------------------------------------------------
void DeltaGlider::CreatePanelElements ()
{
	int i, j;

	ninstr_main = 50;
	instr_scram0 = ninstr_main;
	ninstr_main += (ScramVersion() ? 1 : 0);

	instr_ovhd0 = ninstr_main;
	ninstr_ovhd = 10;

	ninstr = ninstr_main + ninstr_ovhd;

	instr = new PanelElement*[ninstr];
	instr[0]  = new InstrAtt (this);
	instr[1]  = new InstrHSI (this);
	instr[2]  = new InstrAOA (this);
	instr[3]  = new InstrVS (this);
	instr[4]  = new FuelMFD (this);
	instr[5]  = new NavButtons (this);
	instr[6]  = new ElevatorTrim (this);
	instr[7]  = new Airbrake (this);
	instr[8]  = 0;
	instr[9]  = 0;
	instr[10] = 0;
	instr[11] = new ATCtrlDial (this);
	instr[12] = new UndockButton (this);
	instr[13] = 0;
	instr[14] = new GearLever (this);
	instr[15] = 0;
	instr[16] = 0;
	instr[17] = 0;
	instr[18] = 0;
	instr[19] = 0;
	instr[20] = 0;
	instr[21] = 0;
	instr[22] = 0;
	instr[23] = new NoseconeLever (this);
	instr[24] = new NoseconeIndicator (this);
	instr[25] = new SwitchArray (this);
	instr[26] = new WheelBrakeLever (this);
	instr[27] = new MWSButton (this);
	for (i = MFD_LEFT; i <= MFD_RIGHT; i++) {
		instr[28+i*3] = new MFDButtonRow (this, i);
		for (j = 0; j < 2; j++)
			instr[29+i*3+j] = new MFDButtonCol (this, i, j);
	}
	instr[34] = 0;
	instr[35] = 0;
	instr[36] = 0;
	instr[37] = new GearIndicator (this);
	instr[38] = new InstrLightSwitch (this);
	instr[39] = new InstrLightBrightnessDial (this);
	instr[40] = new FloodLightSwitch (this);
	instr[41] = new FloodLightBrightnessDial (this);
	instr[42] = new AngRateIndicator (this, g_Param.surf);
	instr[43] = 0;
	instr[44] = new LandDockLightSwitch (this);
	instr[45] = new StrobeLightSwitch (this);
	instr[46] = new NavLightSwitch (this);
	instr[47] = new HatchSwitch (this);
	instr[48] = new ILockSwitch (this);
	instr[49] = new OLockSwitch (this);

	aap = new AAP (this);   aap->AttachHSI ((InstrHSI*)instr[1]);

	if (ScramVersion()) {
		instr[instr_scram0+0] = new ThrottleScram (this);
	}

	instr[instr_ovhd0+0] = new AirlockSwitch (this);
	for (i = 0; i < 3; i++) {
		instr[instr_ovhd0+1+i] = new AngularVelocityIndicator (this, i);
		instr[instr_ovhd0+4+i] = new AngularAccelerationIndicator (this, i);
		instr[instr_ovhd0+7+i] = new AngularMomentIndicator (this, i);
	}
}

// --------------------------------------------------------------
// Define animation sequences for moving parts
// --------------------------------------------------------------
void DeltaGlider::DefineAnimations ()
{
	// ***** Landing gear animation *****
	static UINT NWheelStrutGrp[2] = {GRP_NWheelStrut1,GRP_NWheelStrut2};
	static MGROUP_ROTATE NWheelStrut (0, NWheelStrutGrp, 2,
		_V(0,-1.048,8.561), _V(1,0,0), (float)(-95*RAD));
	static UINT NWheelFCoverGrp[2] = {GRP_NWheelFCover1,GRP_NWheelFCover2};
	static MGROUP_ROTATE NWheelFCover (0, NWheelFCoverGrp, 2,
		_V(0,-1.145,8.65), _V(1,0,0), (float)(-90*RAD));
	static UINT NWheelLCoverGrp[2] = {GRP_NWheelLCover1,GRP_NWheelLCover2};
	static MGROUP_ROTATE NWheelLCover1 (0, NWheelLCoverGrp, 2,
		_V(-0.3,-1.222,7.029), _V(0,0.052,0.999), (float)(-90*RAD));
	static MGROUP_ROTATE NWheelLCover2 (0, NWheelLCoverGrp, 2,
		_V(-0.3,-1.222,7.029), _V(0,0.052,0.999), (float)( 90*RAD));
	static UINT NWheelRCoverGrp[2] = {GRP_NWheelRCover1,GRP_NWheelRCover2};
	static MGROUP_ROTATE NWheelRCover1 (0, NWheelRCoverGrp, 2,
		_V( 0.3,-1.222,7.029), _V(0,0.052,0.999), (float)( 90*RAD));
	static MGROUP_ROTATE NWheelRCover2 (0, NWheelRCoverGrp, 2,
		_V( 0.3,-1.222,7.029), _V(0,0.052,0.999), (float)(-90*RAD));
	static UINT LWheelStrutGrp[2] = {GRP_LWheelStrut1,GRP_LWheelStrut2};
	static MGROUP_ROTATE LWheelStrut (0, LWheelStrutGrp, 2,
		_V(-3.607,-1.137,-3.08), _V(0,0,1), (float)(-90*RAD));
	static UINT RWheelStrutGrp[2] = {GRP_RWheelStrut1,GRP_RWheelStrut2};
	static MGROUP_ROTATE RWheelStrut (0, RWheelStrutGrp, 2,
		_V( 3.607,-1.137,-3.08), _V(0,0,1), (float)(90*RAD));
	static UINT LWheelOCoverGrp[4] = {GRP_LWheelOCover1,GRP_LWheelOCover2,GRP_LWheelOCover3,GRP_LWheelOCover4};
	static MGROUP_ROTATE LWheelOCover (0, LWheelOCoverGrp, 4,
		_V(-3.658,-1.239,-3.038), _V(0,0,1), (float)(-110*RAD));
	static UINT LWheelICoverGrp[2] = {GRP_LWheelICover1,GRP_LWheelICover2};
	static MGROUP_ROTATE LWheelICover1 (0, LWheelICoverGrp, 2,
		_V(-2.175,-1.178,-3.438), _V(0,0,1), (float)(90*RAD));
	static MGROUP_ROTATE LWheelICover2 (0, LWheelICoverGrp, 2,
		_V(-2.175,-1.178,-3.438), _V(0,0,1), (float)(-90*RAD));
	static UINT RWheelOCoverGrp[4] = {GRP_RWheelOCover1,GRP_RWheelOCover2,GRP_RWheelOCover3,GRP_RWheelOCover4};
	static MGROUP_ROTATE RWheelOCover (0, RWheelOCoverGrp, 4,
		_V( 3.658,-1.239,-3.038), _V(0,0,1), (float)( 110*RAD));
	static UINT RWheelICoverGrp[2] = {GRP_RWheelICover1,GRP_RWheelICover2};
	static MGROUP_ROTATE RWheelICover1 (0, RWheelICoverGrp, 2,
		_V( 2.175,-1.178,-3.438), _V(0,0,1), (float)(-90*RAD));
	static MGROUP_ROTATE RWheelICover2 (0, RWheelICoverGrp, 2,
		_V( 2.175,-1.178,-3.438), _V(0,0,1), (float)( 90*RAD));
	anim_gear = CreateAnimation (1);
	AddAnimationComponent (anim_gear, 0.3, 1, &NWheelStrut);
	AddAnimationComponent (anim_gear, 0.3, 0.9, &NWheelFCover);
	AddAnimationComponent (anim_gear, 0, 0.3, &NWheelLCover1);
	AddAnimationComponent (anim_gear, 0.7, 1.0, &NWheelLCover2);
	AddAnimationComponent (anim_gear, 0, 0.3, &NWheelRCover1);
	AddAnimationComponent (anim_gear, 0.7, 1.0, &NWheelRCover2);
	AddAnimationComponent (anim_gear, 0, 1, &LWheelStrut);
	AddAnimationComponent (anim_gear, 0, 1, &RWheelStrut);
	AddAnimationComponent (anim_gear, 0, 1, &LWheelOCover);
	AddAnimationComponent (anim_gear, 0, 0.3, &LWheelICover1);
	AddAnimationComponent (anim_gear, 0.7, 1, &LWheelICover2);
	AddAnimationComponent (anim_gear, 0, 1, &RWheelOCover);
	AddAnimationComponent (anim_gear, 0, 0.3, &RWheelICover1);
	AddAnimationComponent (anim_gear, 0.7, 1, &RWheelICover2);

	// ***** Nose cone animation *****
	static UINT NConeTLGrp[2] = {GRP_NConeTL1,GRP_NConeTL2};
	static MGROUP_ROTATE NConeTL (0, NConeTLGrp, 2,
		_V(-0.424,-0.066,9.838), _V(-0.707,-0.707,0), (float)(150*RAD));
	static UINT NConeTRGrp[2] = {GRP_NConeTR1,GRP_NConeTR2};
	static MGROUP_ROTATE NConeTR (0, NConeTRGrp, 2,
		_V( 0.424,-0.066,9.838), _V(-0.707, 0.707,0), (float)(150*RAD));
	static UINT NConeBLGrp[2] = {GRP_NConeBL1,GRP_NConeBL2};
	static MGROUP_ROTATE NConeBL (0, NConeBLGrp, 2,
		_V(-0.424,-0.914,9.838), _V( 0.707,-0.707,0), (float)(150*RAD));
	static UINT NConeBRGrp[2] = {GRP_NConeBR1,GRP_NConeBR2};
	static MGROUP_ROTATE NConeBR (0, NConeBRGrp, 2,
		_V( 0.424,-0.914,9.838), _V( 0.707, 0.707,0), (float)(150*RAD));
	static UINT NConeDockGrp[1] = {GRP_NConeDock};
	static MGROUP_TRANSLATE NConeDock (0, NConeDockGrp, 1, _V(0,0,0.06));
	// virtual cockpit mesh animation (nose cone visible from cockpit)
	static UINT VCNConeTLGrp[1] = {GRP_NOSECONE_L_VC};
	static MGROUP_ROTATE VCNConeTL (1, VCNConeTLGrp, 1,
		_V(-0.424,-0.066,9.838), _V(-0.707,-0.707,0), (float)(150*RAD));
	static UINT VCNConeTRGrp[1] = {GRP_NOSECONE_R_VC};
	static MGROUP_ROTATE VCNConeTR (1, VCNConeTRGrp, 1,
		_V( 0.424,-0.066,9.838), _V(-0.707, 0.707,0), (float)(150*RAD));
	anim_nose = CreateAnimation (0);
	AddAnimationComponent (anim_nose, 0.01, 0.92, &NConeTL);
	AddAnimationComponent (anim_nose, 0.01, 0.92, &VCNConeTL);
	AddAnimationComponent (anim_nose, 0.02, 0.925, &NConeTR);
	AddAnimationComponent (anim_nose, 0.02, 0.925, &VCNConeTR);
	AddAnimationComponent (anim_nose, 0, 0.91, &NConeBL);
	AddAnimationComponent (anim_nose, 0.015, 0.915, &NConeBR);
	AddAnimationComponent (anim_nose, 0.8, 1, &NConeDock);

	// ***** Outer airlock animation *****
	static UINT OLockGrp[2] = {GRP_OLock1,GRP_OLock2};
	static MGROUP_ROTATE OLock (0, OLockGrp, 2,
		_V(0,-0.080,9.851), _V(1,0,0), (float)(110*RAD));
	static UINT VCOLockGrp[1] = {13};
	static MGROUP_ROTATE VCOLock (1, VCOLockGrp, 1,
		_V(0,-0.080,9.851), _V(1,0,0), (float)(110*RAD));
	anim_olock = CreateAnimation (0);
	AddAnimationComponent (anim_olock, 0, 1, &OLock);
	AddAnimationComponent (anim_olock, 0, 1, &VCOLock);

	// ***** Inner airlock animation *****
	static UINT ILockGrp[2] = {GRP_ILock1,GRP_ILock2};
	static MGROUP_ROTATE ILock (0, ILockGrp, 2,
		_V(0,-0.573,7.800), _V(1,0,0), (float)(85*RAD));
	// virtual cockpit mesh animation (inner airlock visible from cockpit)
	static UINT VCILockGrp[4] = {GRP_ILOCK1_VC,GRP_ILOCK2_VC,GRP_ILOCK3_VC,GRP_ILOCK_GLASS_VC};
	static MGROUP_ROTATE VCILock (1, VCILockGrp, 4,
		_V(0,-0.573,7.800), _V(1,0,0), (float)(85*RAD));
	anim_ilock = CreateAnimation (0);
	AddAnimationComponent (anim_ilock, 0, 1, &ILock);
	AddAnimationComponent (anim_ilock, 0, 1, &VCILock);

	// ***** Escape ladder animation *****
	static UINT LadderGrp[2] = {GRP_Ladder1,GRP_Ladder2};
	static MGROUP_TRANSLATE Ladder1 (0, LadderGrp, 2, _V(0,0,1.1));
	static MGROUP_ROTATE Ladder2 (0, LadderGrp, 2,
		_V(0,-1.05,9.85), _V(1,0,0), (float)(80*RAD));
	anim_ladder = CreateAnimation (0);
	AddAnimationComponent (anim_ladder, 0, 0.5, &Ladder1);
	AddAnimationComponent (anim_ladder, 0.5, 1, &Ladder2);

	// ***** Top hatch animation *****
	static UINT HatchGrp[2] = {GRP_Hatch1,GRP_Hatch2};
	static MGROUP_ROTATE Hatch (0, HatchGrp, 2,
		_V(0,2.069,5.038), _V(1,0,0), (float)(110*RAD));
	static UINT VCHatchGrp[1] = {GRP_HATCH_VC};
	static MGROUP_ROTATE VCHatch (1, VCHatchGrp, 1,
		_V(0,2.069,5.038), _V(1,0,0), (float)(110*RAD));
	static UINT RearLadderGrp[2] = {GRP_RearLadder1,GRP_RearLadder2};
	static MGROUP_ROTATE RearLadder1 (0, RearLadderGrp, 2,
		_V(0,1.7621,4.0959), _V(1,0,0), (float)(-20*RAD));
	static MGROUP_ROTATE RearLadder2 (0, RearLadderGrp+1, 1,
		_V(0,1.1173,4.1894), _V(1,0,0), (float)(180*RAD));
	// virtual cockpit ladder animation
	static UINT VCRearLadderGrp[2] = {GRP_LADDER1_VC,GRP_LADDER2_VC};
	static MGROUP_ROTATE VCRearLadder1 (1, VCRearLadderGrp, 2,
		_V(0,1.7621,4.0959), _V(1,0,0), (float)(-20*RAD));
	static MGROUP_ROTATE VCRearLadder2 (1, VCRearLadderGrp+1, 1,
		_V(0,1.1173,4.1894), _V(1,0,0), (float)(180*RAD));
	anim_hatch = CreateAnimation (0);
	AddAnimationComponent (anim_hatch, 0, 1, &Hatch);
	AddAnimationComponent (anim_hatch, 0, 1, &VCHatch);
	AddAnimationComponent (anim_hatch, 0, 0.25, &RearLadder1);
	AddAnimationComponent (anim_hatch, 0.25, 0.8, &RearLadder2);
	AddAnimationComponent (anim_hatch, 0, 0.25, &VCRearLadder1);
	AddAnimationComponent (anim_hatch, 0.25, 0.8, &VCRearLadder2);

	// ***** Radiator animation *****
	static UINT RaddoorGrp[2] = {GRP_Raddoor1,GRP_Raddoor2};
	static MGROUP_ROTATE Raddoor (0, RaddoorGrp, 2,
		_V(0,1.481,-3.986), _V(1,0,0), (float)(170*RAD));
	static UINT RadiatorGrp[3] = {GRP_Radiator1,GRP_Radiator2,GRP_Radiator3};
	static MGROUP_TRANSLATE Radiator (0, RadiatorGrp, 3,
		_V(0,0.584,-0.157));
	static UINT LRadiatorGrp[1] = {GRP_Radiator1};
	static MGROUP_ROTATE LRadiator (0, LRadiatorGrp, 1,
		_V(-0.88,1.94,-4.211), _V(0,0.260,0.966), (float)(135*RAD));
	static UINT RRadiatorGrp[1] = {GRP_Radiator2};
	static MGROUP_ROTATE RRadiator (0, RRadiatorGrp, 1,
		_V(0.93,1.91,-4.211), _V(0,0.260,0.966), (float)(-135*RAD));
	anim_radiator = CreateAnimation (0);
	AddAnimationComponent (anim_radiator, 0, 0.33, &Raddoor);
	AddAnimationComponent (anim_radiator, 0.25, 0.5, &Radiator);
	AddAnimationComponent (anim_radiator, 0.5, 0.75, &RRadiator);
	AddAnimationComponent (anim_radiator, 0.75, 1, &LRadiator);

	// ***** Rudder animation *****
	static UINT RRudderGrp[2] = {GRP_RRudder1,GRP_RRudder2};
	static MGROUP_ROTATE RRudder (0, RRudderGrp, 2,
		_V( 8.668,0.958,-6.204), _V( 0.143,0.975,-0.172), (float)(-60*RAD));
	static UINT LRudderGrp[2] = {GRP_LRudder1,GRP_LRudder2};
	static MGROUP_ROTATE LRudder (0, LRudderGrp, 2,
		_V(-8.668,0.958,-6.204), _V(-0.143,0.975,-0.172), (float)(-60*RAD));
	anim_rudder = CreateAnimation (0.5);
	AddAnimationComponent (anim_rudder, 0, 1, &RRudder);
	AddAnimationComponent (anim_rudder, 0, 1, &LRudder);

	// ***** Elevator animation *****
	static UINT ElevatorGrp[8] = {29,30,35,36,51,52,54,55};
	static MGROUP_ROTATE Elevator (0, ElevatorGrp, 8,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(40*RAD));
	anim_elevator = CreateAnimation (0.5);
	AddAnimationComponent (anim_elevator, 0, 1, &Elevator);

	// ***** Elevator trim animation *****
	static MGROUP_ROTATE ElevatorTrim (0, ElevatorGrp, 8,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(10*RAD));
	anim_elevatortrim = CreateAnimation (0.5);
	AddAnimationComponent (anim_elevatortrim, 0, 1, &ElevatorTrim);

	// ***** Aileron animation *****
	static UINT LAileronGrp[4] = {29,30,51,52};
	static MGROUP_ROTATE LAileron (0, LAileronGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(-20*RAD));
	anim_laileron = CreateAnimation (0.5);
	AddAnimationComponent (anim_laileron, 0, 1, &LAileron);

	static UINT RAileronGrp[4] = {35,36,54,55};
	static MGROUP_ROTATE RAileron (0, RAileronGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(20*RAD));
	anim_raileron = CreateAnimation (0.5);
	AddAnimationComponent (anim_raileron, 0, 1, &RAileron);

	// ***** Airbrake animation *****
	static UINT UpperBrakeGrp[4] = {35,30,52,55};
	static MGROUP_ROTATE UpperBrake (0, UpperBrakeGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(50*RAD));
	static UINT LowerBrakeGrp[4] = {29,36,51,54};
	static MGROUP_ROTATE LowerBrake (0, LowerBrakeGrp, 4,
		_V(0,-0.4,-6.0), _V(1,0,0), (float)(-50*RAD));
	static MGROUP_ROTATE RRudderBrake (0, RRudderGrp, 2,
		_V( 8.668,0.958,-6.204), _V( 0.143,0.975,-0.172), (float)( 25*RAD));
	static MGROUP_ROTATE LRudderBrake (0, LRudderGrp, 2,
		_V(-8.668,0.958,-6.204), _V(-0.143,0.975,-0.172), (float)(-25*RAD));

	anim_brake = CreateAnimation (0);
	AddAnimationComponent (anim_brake, 0, 1, &UpperBrake);
	AddAnimationComponent (anim_brake, 0, 1, &LowerBrake);
	AddAnimationComponent (anim_brake, 0, 1, &RRudderBrake);
	AddAnimationComponent (anim_brake, 0, 1, &LRudderBrake);

	// ======================================================
	// VC animation definitions
	// ======================================================

	// Elevator trim wheel
	static UINT TrimWheelGrp = GRP_ETRIM_WHEEL_VC;
	static MGROUP_ROTATE TrimWheelTransform (1, &TrimWheelGrp, 1,
		etrim_wheel_ref, etrim_wheel_axis, (float)(PI*0.06));
	anim_vc_trimwheel = CreateAnimation (0.5);
	AddAnimationComponent (anim_vc_trimwheel, 0, 1, &TrimWheelTransform);

	// Scram throttle left engine
	static UINT ScramThrottleLGrp[2] = {GRP_THROTTLE_SCRAM_L1_VC,GRP_THROTTLE_SCRAM_L2_VC};
	static MGROUP_ROTATE ScramThrottleL (1, ScramThrottleLGrp, 2,
		_V(0,0.7849,6.96), _V(1,0,0), (float)(30*RAD));
	anim_scramthrottle[0] =  CreateAnimation (0);
	AddAnimationComponent (anim_scramthrottle[0], 0, 1, &ScramThrottleL);

	// Scram throttle right engine
	static UINT ScramThrottleRGrp[2] = {GRP_THROTTLE_SCRAM_R1_VC,GRP_THROTTLE_SCRAM_R2_VC};
	static MGROUP_ROTATE ScramThrottleR (1, ScramThrottleRGrp, 2,
		_V(0,0.7849,6.96), _V(1,0,0), (float)(30*RAD));
	anim_scramthrottle[1] =  CreateAnimation (0);
	AddAnimationComponent (anim_scramthrottle[1], 0, 1, &ScramThrottleR);

	// Gear lever
	static UINT GearLeverGrp = GRP_GEAR_LEVER_VC;
	static MGROUP_ROTATE GearLeverTransform (1, &GearLeverGrp, 1,
		vc_gearlever_ref, vc_gearlever_axis, (float)(-70*RAD));
	anim_gearlever = CreateAnimation (0.5);
	AddAnimationComponent (anim_gearlever, 0, 1, &GearLeverTransform);

	// Airbrake lever
	static UINT AirbrakeLeverGrp = GRP_AIRBRAKE_LEVER_VC;
	static MGROUP_ROTATE AirbrakeLeverTransform (1, &AirbrakeLeverGrp, 1,
		vc_abrakelever_ref, vc_abrakelever_axis, (float)(-40*RAD));
	anim_airbrakelever = CreateAnimation(0.8);
	AddAnimationComponent (anim_airbrakelever, 0, 1, &AirbrakeLeverTransform);

	// Nosecone lever
	static UINT NoseconeLeverGrp = GRP_NOSECONE_LEVER_VC;
	static MGROUP_ROTATE NoseconeLeverTransform (1, &NoseconeLeverGrp, 1,
		vc_nconelever_ref, vc_nconelever_axis, (float)(-70*RAD));
	anim_noselever = CreateAnimation (0.5);
	AddAnimationComponent (anim_noselever, 0, 1, &NoseconeLeverTransform);

	// Undock lever
	static UINT UndockLeverGrp = GRP_UNDOCK_LEVER_VC;
	static MGROUP_ROTATE UndockLeverTransform (1, &UndockLeverGrp, 1,
		vc_undocklever_ref, vc_undocklever_axis, (float)(-90*RAD));
	anim_undocklever = CreateAnimation (0);
	AddAnimationComponent (anim_undocklever, 0, 1, &UndockLeverTransform);

	// Instrument illumination brightness dial
	static UINT InstrBDialGrp = GRP_INSTR_BRIGHTNESS_VC;
	static MGROUP_ROTATE InstrBDialTransform (1, &InstrBDialGrp, 1,
		VC_INSTR_BRIGHTNESS_ref, VC_INSTR_BRIGHTNESS_axis, (float)(-280*RAD));
	anim_instrbdial = CreateAnimation (0.5);
	AddAnimationComponent (anim_instrbdial, 0, 1, &InstrBDialTransform);

	// Floodlight brightness dial
	static UINT FloodBDialGrp = GRP_FLOOD_BRIGHTNESS_VC;
	static MGROUP_ROTATE FloodBDialTransform (1, &FloodBDialGrp, 1,
		VC_FLOOD_BRIGHTNESS_ref, VC_FLOOD_BRIGHTNESS_axis, (float)(-280*RAD));
	anim_floodbdial = CreateAnimation (0.5);
	AddAnimationComponent (anim_floodbdial, 0, 1, &FloodBDialTransform);

	static UINT LadderSwitchGrp = GRP_LADDER_SWITCH_VC;
	static MGROUP_ROTATE LadderSwitch (1, &LadderSwitchGrp, 1,
		_V(0.2889,1.0622,7.2388), _V(-0.7590,-0.231,0.6087), (float)(31*RAD));
	anim_ladderswitch = CreateAnimation (1);
	AddAnimationComponent (anim_ladderswitch, 0, 1, &LadderSwitch);

	static UINT RadiatorSwitchGrp = GRP_RADIATOR_SWITCH_VC;
	static MGROUP_ROTATE RadiatorSwitch (1, &RadiatorSwitchGrp, 1,
		_V(0.2592,0.9517,7.2252), _V(-0.7590,-0.231,0.6087), (float)(31*RAD));
	anim_radiatorswitch = CreateAnimation (1);
	AddAnimationComponent (anim_radiatorswitch, 0, 1, &RadiatorSwitch);
}

// --------------------------------------------------------------
// Apply custom skin to the current mesh instance
// --------------------------------------------------------------
void DeltaGlider::ApplySkin ()
{
	if (!exmesh) return;
	if (skin[0]) oapiSetTexture (exmesh, 2, skin[0]);
	if (skin[1]) oapiSetTexture (exmesh, 3, skin[1]);
	oapiSetTexture (exmesh, 5, insignia_tex);

	static const MATERIAL panelmat[5] = {
		{{0.5,0.6,0.6,1},{0.5,0.6,0.6,1},{0.2,0.2,0.2,1},{0.15,0.15,0.15,1},10}, // default
		{{0.6,0.55,0.45,1},{0.6,0.55,0.45,1},{0.1,0.1,0.1,1},{0.15,0.15,0.15,1},5}, // brown
		{{0.2,0.2,0.15,1},{0.5,0.5,0.5,1},{0.6,0.6,0.6,1},{0.15,0.15,0.15,1},20}, // shiny anthrazit
		{{0.75,0.75,0.65,1},{0.75,0.75,0.65,1},{0.3,0.3,0.3,1},{0.15,0.15,0.15,1},20}, // shiny anthrazit
		{{0.5,0.1,0.1,1},{0.5,0.1,0.1,1},{0.2,0.2,0.2,1},{0.15,0.15,0.15,1},10} // DG-red
	};
	if (panelcol[0] > 0 && panelcol[0] <= 4)
		oapiSetMaterial (vcmesh, 14, panelmat+panelcol[0]);

	//time_t lt; time(&lt); struct tm *st = localtime(&lt);
	//if (vcmesh && st->tm_mon==3 && st->tm_mday==1) {
	//	SURFHANDLE t = oapiLoadTexture ("generic\\noisep.dds");
	//	if (t) oapiSetTexture (vcmesh, 17, t);
	//}
}

// --------------------------------------------------------------
// Paint individual vessel markings
// --------------------------------------------------------------
void DeltaGlider::PaintMarkings (SURFHANDLE tex)
{
	oapi::Font *font = oapiCreateFont (38, true, "Sans", FONT_BOLD);
	oapi::Sketchpad *skp = oapiGetSketchpad (tex);
	if (skp) {
		skp->SetFont (font);
		skp->SetTextColor (0xD0D0D0);
		skp->SetTextAlign (oapi::Sketchpad::CENTER);
		char cbuf[32];
		strncpy (cbuf, GetName(), 10);
		int len = min(strlen(GetName()), 10);
		skp->Text (196, 10, cbuf, len);
		skp->Text (196, 74, cbuf, len);
		skp->SetTextColor (0x808080);
		skp->SetTextAlign (oapi::Sketchpad::RIGHT);
		skp->Text (120, 158, cbuf, len);
		skp->SetTextAlign (oapi::Sketchpad::LEFT);
		skp->Text (133, 158, cbuf, len);
		oapiReleaseSketchpad (skp);
	}
	oapiReleaseFont (font);
}

// --------------------------------------------------------------
// Release bitmaps used for panel animations
// --------------------------------------------------------------
void DeltaGlider::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++)
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
}

// --------------------------------------------------------------
// Load panel animation bitmaps and initialise panel state
// --------------------------------------------------------------
void DeltaGlider::InitPanel (int panel)
{
	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkReset2D (panel, hPanelMesh);

	DWORD i;

	switch (panel) {
	case 0: // main panel
		// load bitmaps used by the panel
		srf[0] = oapiCreateSurface (LOADBMP (IDB_SWITCH1));
		srf[1] = oapiCreateSurface (LOADBMP (IDB_NAVBUTTON));
		srf[2] = oapiCreateSurface (LOADBMP (IDB_LIGHT1));
		srf[4] = oapiCreateSurface (LOADBMP (IDB_SLIDER1));
		srf[5] = oapiCreateSurface (LOADBMP (IDB_LIGHT2));
		srf[6] = oapiCreateSurface (LOADBMP (IDB_SWITCH4));
		srf[7] = oapiCreateSurface (LOADBMP (IDB_SWITCH4R));
		srf[8] = oapiCreateSurface (LOADBMP (IDB_INDICATOR2));  oapiSetSurfaceColourKey (srf[8], 0);
		srf[9] = oapiCreateSurface (LOADBMP (IDB_DIAL1));
		srf[10] = oapiCreateSurface (LOADBMP (IDB_FONT2));
		srf[11] = oapiCreateSurface (LOADBMP (IDB_WARN));

		// reset state flags for panel instruments
		for (i = 0; i < ninstr; i++) if (instr[i]) instr[i]->Reset2D (hPanelMesh);

		for (i = 0; i < 5; i++) engsliderpos[i] = (UINT)-1;
		for (i = 0; i < 2; i++)
			mainflowidx[i] = retroflowidx[i] = scTSFCidx[i] = scflowidx[i] =
			mainpropidx[i] = rcspropidx[i] = scrampropidx[i] = -1;
		mainpropmass = rcspropmass = scrampropmass = -1;
		hoverflowidx = mainTSFCidx = -1;

		memset (&p_prpdisp, 0, sizeof(p_prpdisp));
		memset (&p_engdisp, 0, sizeof(p_engdisp));
		memset (&p_rngdisp, 0, sizeof(p_rngdisp));
		break;
	case 1: // overhead panel
		srf[0] = oapiCreateSurface (LOADBMP (IDB_SWITCH1));
		srf[1] = oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[4] = oapiCreateSurface (LOADBMP (IDB_VPITCH));
		srf[5] = oapiCreateSurface (LOADBMP (IDB_VBANK));
		srf[6] = oapiCreateSurface (LOADBMP (IDB_VYAW));
		break;
	case 2: // bottom panel
		srf[0] = oapiCreateSurface (LOADBMP (IDB_SWITCH2));
		srf[1] = oapiCreateSurface (LOADBMP (IDB_LEVER1));
		srf[2] = oapiCreateSurface (LOADBMP (IDB_SWITCH3));
		srf[9] = oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		for (i = 0; i < 2; i++) wbrake_pos[i] = (UINT)-1;
		//dockreleasedown = false;
		break;
	}
}

// --------------------------------------------------------------
// Load VC animation bitmaps and initialise VC state
// --------------------------------------------------------------
void DeltaGlider::InitVC (int vc)
{
	int i;

	switch (vc) {
	case 0:
		srf[2] = oapiCreateSurface (LOADBMP (IDB_LIGHT1));
		srf[4] = oapiCreateSurface (LOADBMP (IDB_VPITCH));
		srf[5] = oapiCreateSurface (LOADBMP (IDB_VBANK));
		srf[6] = oapiCreateSurface (LOADBMP (IDB_VYAW));
		srf[8] = oapiCreateSurface (LOADBMP (IDB_INDICATOR2));  oapiSetSurfaceColourKey (srf[8], 0);
		srf[9] = oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[10] = oapiCreateSurface (LOADBMP (IDB_FONT2));

		// reset state flags for panel instruments
		for (i = 0; i < 5; i++) engsliderpos[i] = (UINT)-1;
		for (i = 0; i < 2; i++)
			mainflowidx[i] = retroflowidx[i] = scTSFCidx[i] = scflowidx[i] =
			mainpropidx[i] = rcspropidx[i] = scrampropidx[i] = -1;
		hoverflowidx = mainTSFCidx = -1;
		mainpropmass = rcspropmass = scrampropmass = -1;

		memset (&p_prpdisp, 0, sizeof(p_prpdisp));
		memset (&p_engdisp, 0, sizeof(p_engdisp));
		memset (&p_rngdisp, 0, sizeof(p_rngdisp));

		break;
	}
}

void DeltaGlider::ScramjetThrust ()
{
	int i;
	const double eps = 1e-8;
	const double Fnominal = 2.5*MAX_MAIN_THRUST[modelidx];

	double Fscram[2];
	scramjet->Thrust (Fscram);

	for (i = 0; i < 2; i++) {
		double level = GetThrusterLevel (th_scram[i]);
		double Fmax  = Fscram[i]/(level+eps);
		SetThrusterMax0 (th_scram[i], Fmax);
		SetThrusterIsp (th_scram[i], max (1.0, Fscram[i]/(scramjet->DMF(i)+eps))); // don't allow ISP=0

		// the following are used for calculating exhaust density
		scram_max[i] = min (Fmax/Fnominal, 1.0);
		scram_intensity[i] = level * scram_max[i];
	}
}

bool DeltaGlider::clbkDrawHUD (int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad *skp)
{
	if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;

	// draw the default HUD
	VESSEL4::clbkDrawHUD (mode, hps, skp);
	int cx = hps->CX, cy = hps->CY;

	// show gear deployment status
	if (gear_status == DOOR_OPEN || (gear_status >= DOOR_CLOSING && fmod (oapiGetSimTime(), 1.0) < 0.5)) {
		int d = hps->Markersize/2;
		if (cx >= -d*3 && cx < hps->W+d*3 && cy >= d && cy < hps->H+d*5) {
			skp->Rectangle (cx-d/2, cy-d*5, cx+d/2, cy-d*4);
			skp->Rectangle (cx-d*3, cy-d*2, cx-d*2, cy-d);
			skp->Rectangle (cx+d*2, cy-d*2, cx+d*3, cy-d);
		}
	}

	// show RCS mode
	switch (GetAttitudeMode()) {
	case RCS_ROT:
		skp->Text (0, hps->H-20, "RCS ROT", 7);
		break;
	case RCS_LIN:
		skp->Text (0, hps->H-20, "RCS_LIN", 7);
		break;
	}

	if (oapiGetHUDMode() == HUD_DOCKING) {
		if (nose_status != DOOR_OPEN) {
			int d = hps->Markersize*5;
			double tmp;
			if (nose_status == DOOR_CLOSED || modf (oapiGetSimTime(), &tmp) < 0.5) {
				skp->Line (cx-d,cy-d,cx+d,cy+d);
				skp->Line (cx-d,cy+d,cx+d,cy-d);
			}
			char *str = "NOSECONE";
			int w = skp->GetTextWidth (str);
			skp->Text (cx-w/2, cy-d, str, 8);
		}
	}
	return true;
}

void DeltaGlider::clbkRenderHUD (int mode, const HUDPAINTSPEC *hps, SURFHANDLE hTex)
{
	VESSEL4::clbkRenderHUD (mode, hps, hTex);

	static float texw = 512.0f, texh = 256.0f;
	float cx = (float)hps->CX, cy = (float)hps->CY;
	int i, nvtx = 0, nidx = 0;
	static NTVERTEX vtx[12+16+4];
	static WORD idx[18+36+6];
	static float scl = 0;
	static NTVERTEX vgear[12];
	static NTVERTEX vnose[16];
	static NTVERTEX vbrk[4];
	static WORD igear[18] = {
		0,3,1, 3,0,2,
		4,7,5, 7,4,6,
		8,11,9, 11,8,10
	};
	static WORD inose[36] = {
		0,1,2, 2,3,0,
		0,6,1, 6,7,1,
		1,8,2, 8,9,2,
		2,10,3, 10,11,3,
		3,4,0, 0,4,5,
		12,15,13, 12,14,15
	};
	static WORD ibrk[6] = {
		0,3,1, 0,2,3
	};

	if (scl != hps->Markersize*0.25f) { // resize
		scl = hps->Markersize*0.25f;
		memset (vgear, 0, 12*sizeof(NTVERTEX));
		float x[12] = {-4,-2,-4,-2,2,4,2,4,-1,1,-1,1};
		float y[12] = {-2,-2,-4,-4,-2,-2,-4,-4,-6,-6,-8,-8};
		for (i = 0; i < 12; i++) {
			vgear[i].x = cx + x[i]*scl;
			vgear[i].y = cy + y[i]*scl;
			vgear[i].tu = (405.0f + (18.0f * (i%2)))/texw;
			vgear[i].tv = (104.0f - (18.0f * ((i%4)/2)))/texh;
		}
		memset (vnose, 0, 16*sizeof(NTVERTEX));
		float xn[16] = {0,1,0,-1,-31,-30,30,31,31,30,-30,-31,  -13,13,-13,13};
		float yn[16] = {-1,0,1,0,-30,-31,-31,-30,30,31,31,30,  -25,-25,-28.9,-28.9};
		float un[16] = {392.5f, 397.0f, 392.5f, 388.0f, 388.0f, 392.5f, 392.5f, 397.0f, 397.0f, 392.5f, 392.5f, 388.0f,    124.0f, 204.0f, 124.0f, 204.0f};
		float vn[16] = {92.0f, 96.5f, 101.0f, 96.5f, 96.5f, 92.0f, 92.0f, 96.5f, 96.5f, 101.0f, 101.0f, 96.5f,             118.0f, 118.0f, 106.0f, 106.0f};
		for (i = 0; i < 16; i++) {
			vnose[i].x = cx + xn[i]*scl*0.4f;
			vnose[i].y = cy + yn[i]*scl*0.4f;
			vnose[i].tu = un[i]/texw;
			vnose[i].tv = vn[i]/texh;
		}
		memset (vbrk, 0, 4*sizeof(NTVERTEX));
		float xb[4] = {-9.1f, 9.1f, -9.1f, 9.1f};
		float yb[4] = {-30.0f, -30.0f, -33.9f, -33.9f};
		float ub[4] = {205.0f, 261.0f, 205.0f, 261.0f};
		float vb[4] = {118.0f, 118.0f, 106.0f, 106.0f};
		for (i = 0; i < 4; i++) {
			vbrk[i].x = cx + xb[i]*scl*0.4f;
			vbrk[i].y = cy + yb[i]*scl*0.4f;
			vbrk[i].tu = ub[i]/texw;
			vbrk[i].tv = vb[i]/texh;
		}
	}

	// show gear deployment status
	if (gear_status == DOOR_OPEN || (gear_status >= DOOR_CLOSING && fmod (oapiGetSimTime(), 1.0) < 0.5)) {
		memcpy (vtx+nvtx, vgear, 12*sizeof(NTVERTEX));
		for (i = 0; i < 18; i++) idx[nidx+i] = igear[i]+nvtx;
		nvtx += 12;
		nidx += 18;
	}

	// show nosecone status
	if (oapiGetHUDMode() == HUD_DOCKING && nose_status != DOOR_OPEN) {
		double tmp;
		if (nose_status == DOOR_CLOSED || modf (oapiGetSimTime(), &tmp) < 0.5) {
			memcpy (vtx+nvtx, vnose, 16*sizeof(NTVERTEX));
			for (i = 0; i < 36; i++) idx[nidx+i] = inose[i]+nvtx;
			nvtx += 16;
			nidx += 36;
		}
	}

	// show airbrake status
	if (brake_status != DOOR_CLOSED) {
		double tmp;
		if (brake_status == DOOR_OPEN || modf (oapiGetSimTime(), &tmp) < 0.5) {
			memcpy (vtx+nvtx, vbrk, 4*sizeof(NTVERTEX));
			for (i = 0; i < 6; i++) idx[nidx+i] = ibrk[i]+nvtx;
			nvtx += 4;
			nidx += 6;
		}
	}

	if (nvtx) {
		MESHGROUP grp = {vtx, idx, nvtx, nidx, 0, 0, 0, 0, 0};
		MESHHANDLE hmesh = oapiCreateMesh (1, &grp);
		oapiRenderHUD (hmesh, &hTex);
		oapiDeleteMesh (hmesh);
	}
}

void DeltaGlider::ActivateLandingGear (DoorStatus action)
{
	if (action == DOOR_OPENING && GroundContact()) return;
	// we cannot deploy the landing gear if we are already sitting on the ground

	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	gear_status = gearlever_status = action;
	if (action <= DOOR_OPEN) {
		gear_proc = gearlever_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_gear, gear_proc);
		SetAnimation (anim_gearlever, gearlever_proc);
		UpdateStatusIndicators();
		SetGearParameters (gear_proc);
	}
	oapiTriggerPanelRedrawArea (0, AID_GEARLEVER);
	oapiTriggerRedrawArea (2, 0, AID_GEARINDICATOR);
	RecordEvent ("GEAR", close ? "UP" : "DOWN");
}

void DeltaGlider::RevertLandingGear ()
{
	ActivateLandingGear (gear_status == DOOR_CLOSED || gear_status == DOOR_CLOSING ?
						 DOOR_OPENING : DOOR_CLOSING);
	UpdateCtrlDialog (this);
}

void DeltaGlider::SetGearParameters (double state)
{
	if (state == 1.0) {
		if (!bGearIsDown) {
			SetTouchdownPoints (tdvtx_geardown, ntdvtx_geardown);
			SetNosewheelSteering (true);
			bGearIsDown = true;
		}
	} else {
		if (bGearIsDown) {
			SetTouchdownPoints (tdvtx_gearup, ntdvtx_gearup);
			SetNosewheelSteering (false);
			bGearIsDown = false;
		}
	}
}

void DeltaGlider::ActivateDockingPort (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	nose_status = noselever_status = action;
	if (action <= DOOR_OPEN) {
		nose_proc = noselever_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_nose, nose_proc);
		SetAnimation (anim_noselever, noselever_proc);
		UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (0, AID_NOSECONELEVER);
	oapiTriggerRedrawArea (0, 0, AID_NOSECONEINDICATOR);

	if (close && ladder_status != DOOR_CLOSED)
		ActivateLadder (action); // retract ladder before closing the nose cone

	UpdateCtrlDialog (this);
	RecordEvent ("NOSECONE", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::ActivateUndocking (DoorStatus action)
{
	undock_status = action;
	if (action == DOOR_OPENING) Undock(0);
}

void DeltaGlider::RevertDockingPort ()
{
	ActivateDockingPort (nose_status == DOOR_CLOSED || nose_status == DOOR_CLOSING ?
						 DOOR_OPENING : DOOR_CLOSING);
}

void DeltaGlider::ActivateHatch (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	if (hatch_status == DOOR_CLOSED && !close && !hatch_vent && GetAtmPressure() < 10e3) {
		static PARTICLESTREAMSPEC airvent = {
			0, 1.0, 15, 0.5, 0.3, 2, 0.3, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
			PARTICLESTREAMSPEC::LVL_LIN, 0.1, 0.1,
			PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
		};
		static VECTOR3 pos = {0,2,4};
		static VECTOR3 dir = {0,1,0};
		static double lvl = 0.1;
		hatch_vent = AddParticleStream (&airvent, pos, dir, &lvl);
		hatch_vent_t = oapiGetSimTime();
	}

	hatch_status = action;
	if (action <= DOOR_OPEN) {
		hatch_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_hatch, hatch_proc);
		UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (1, AID_SWITCHARRAY);
	UpdateCtrlDialog (this);
	RecordEvent ("HATCH", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::RevertHatch ()
{
	ActivateHatch (hatch_status == DOOR_CLOSED || hatch_status == DOOR_CLOSING ?
				   DOOR_OPENING : DOOR_CLOSING);
}

void DeltaGlider::ActivateLadder (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	if (!close && nose_status != DOOR_OPEN) return;
	// don't extend ladder if nose cone is closed

	ladder_status = action;
	if (action <= DOOR_OPEN) {
		ladder_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_ladder, ladder_proc);
	}
	oapiTriggerPanelRedrawArea (1, AID_SWITCHARRAY);
	SetAnimation (anim_ladderswitch, close ? 0:1);
	UpdateCtrlDialog (this);
	RecordEvent ("LADDER", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::RevertLadder ()
{
	ActivateLadder (ladder_status == DOOR_CLOSED || ladder_status == DOOR_CLOSING ?
					DOOR_OPENING : DOOR_CLOSING);
}

void DeltaGlider::ActivateOuterAirlock (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	olock_status = action;
	if (action <= DOOR_OPEN) {
		olock_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_olock, olock_proc);
		UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (1, AID_AIRLOCKSWITCH);
	UpdateCtrlDialog (this);
	RecordEvent ("OLOCK", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::RevertOuterAirlock ()
{
	ActivateOuterAirlock (olock_status == DOOR_CLOSED || olock_status == DOOR_CLOSING ?
		                  DOOR_OPENING : DOOR_CLOSING);
}

void DeltaGlider::ActivateInnerAirlock (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	ilock_status = action;
	if (action <= DOOR_OPEN) {
		ilock_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_ilock, ilock_proc);
		UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (1, AID_AIRLOCKSWITCH);
	UpdateCtrlDialog (this);
	RecordEvent ("ILOCK", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::RevertInnerAirlock ()
{
	ActivateInnerAirlock (ilock_status == DOOR_CLOSED || ilock_status == DOOR_CLOSING ?
		                  DOOR_OPENING : DOOR_CLOSING);
}

void DeltaGlider::ActivateAirbrake (DoorStatus action, bool half_step)
{
	const double eps = 1e-8;
	brake_status = airbrakelever_status = action;
	if (action <= DOOR_OPEN) {
		brake_proc = airbrakelever_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_brake, brake_proc);
		SetAnimation (anim_airbrakelever, airbrakelever_proc);
	} else if (action == DOOR_OPENING) {
		airbrake_tgt = (airbrakelever_proc < 0.5-eps ? 1:2);
	} else {
		airbrake_tgt = (airbrakelever_proc > 0.5+eps ? 1:0);
	}
	oapiTriggerPanelRedrawArea (0, AID_AIRBRAKE);
	RecordEvent ("AIRBRAKE", action == DOOR_CLOSING ? "CLOSE" : "OPEN");
}

void DeltaGlider::SetInstrLight (bool on, bool force)
{
	if (on == instr_light_on && !force) return; // nothing to do
	instr_light_on = on;

	if (vcmesh) {
		static MATERIAL norm = {{1,1,1,1},{0.8,0.8,0.8,1},{0.1,0.1,0.1,0},{0.15,0.15,0.15,1},5};
		static MATERIAL label_glow[3] = {
			{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0.35,1,0.35,1},0},
		    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,0.7,0.15,1},0},
			{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0.6,0.6,1.0,1},0}};
		static MATERIAL btn_glow = {{1,1,1,1},{1,1,1,1},{0.1,0.1,0.1,1},{0.6,0.6,0.6,1},5};
		oapiSetMaterial (vcmesh, 0, on ? &btn_glow : &norm);
		int idx = max(0, min(2, panelcol[1]));
		MATERIAL mat;
		if (on) {
			float scale = (float)(0.2 + instr_brightness*0.8);
			memcpy(&mat, label_glow+panelcol[1], sizeof(MATERIAL));
			mat.emissive.r *= scale;
			mat.emissive.g *= scale;
			mat.emissive.b *= scale;
		}
		oapiSetMaterial (vcmesh, 11, on ? &mat : &norm);
		GROUPEDITSPEC ges = {on ? GRPEDIT_ADDUSERFLAG : GRPEDIT_DELUSERFLAG, 0x18, 0,0,0};
		oapiEditMeshGroup (vcmesh, GRP_LIT_LABELS_VC, &ges);
		oapiEditMeshGroup (vcmesh, GRP_GLOWLABELS_VC, &ges); // obsolete
	}
}


void DeltaGlider::SetFloodLight (int mode)
{
	static const COLOUR4 zero = {0.0f,0.0f,0.0f,0.0f};
	static const COLOUR4 wcol = {1.0f,1.0f,1.0f,0.0f};
	static const COLOUR4 rcol = {0.6f,0.05f,0.0f,0.0f};

	COLOUR4 col;

	flood_light_mode = mode;
	if (mode) {
		col = (mode == 1 ? wcol : rcol);
		if (!cockpit_light) {
			cockpit_light = (PointLight*)AddPointLight(_V(0,1.65,6.68), 3, 0, 0, 3, col, col, zero);
			cockpit_light->SetVisibility (LightEmitter::VIS_COCKPIT);
			cockpit_light->Activate(true);
		}
		double intens = (float)(0.2 + flood_brightness*0.8);
		cockpit_light->SetIntensity (intens);
	} else {
		DelLightEmitter (cockpit_light);
		cockpit_light = NULL;
	}
}

void DeltaGlider::SetLandDockLight (int mode)
{
	landdock_light_mode = mode;
	if (docking_light) {
		DelLightEmitter (docking_light);
		docking_light = NULL;
	}
	if (mode) {
		COLOUR4 col_a = {0,0,0,0};
		COLOUR4 col_white = {1,1,1,0};
		if (mode == 1) {
			docking_light = (SpotLight*)AddSpotLight(_V(0.3,0.3,8.5), _V(0,0,1), 150, 1e-3, 0, 1e-3, RAD*30, RAD*60, col_white, col_white, col_a);
		} else {
			double tilt = -10.0*RAD;
			docking_light = (SpotLight*)AddSpotLight(_V(0.1,-0.3,7.5), _V(0,sin(tilt),cos(tilt)), 5000, 1e-3, 1e-5, 2e-7, RAD*25, RAD*40, col_white, col_white, col_a);
		}
		docking_light->SetVisibility (LightEmitter::VIS_EXTERNAL);
	}
}

void DeltaGlider::SetStrobeLight (bool on)
{
	strobe_light_on = on;
	for (int i = 3; i <=6; i++) beacon[i].active = on;
	oapiTriggerPanelRedrawArea (1, AID_SWITCHARRAY);
	UpdateCtrlDialog (this);
}

void DeltaGlider::SetNavLight (bool on)
{
	nav_light_on = on;
	for (int i = 0; i <= 2; i++) beacon[i].active = on;
	oapiTriggerPanelRedrawArea (1, AID_SWITCHARRAY);
	UpdateCtrlDialog (this);
}

void DeltaGlider::ModInstrBrightness (bool up)
{
	double dt = oapiGetSimStep();
	double db = dt * (up ? 0.3 : -0.3);
	instr_brightness = max(0.0, min (1.0, instr_brightness + db));
	SetAnimation (anim_instrbdial, instr_brightness);
	if (instr_light_on) SetInstrLight (true, true);
}

void DeltaGlider::ModFloodBrightness (bool up)
{
	double dt = oapiGetSimStep();
	double db = dt * (up ? 0.3 : -0.3);
	flood_brightness = max(0.0, min (1.0, flood_brightness + db));
	SetAnimation (anim_floodbdial, flood_brightness);
	if (flood_light_mode) SetFloodLight (flood_light_mode);
}

void DeltaGlider::ActivateRadiator (DoorStatus action)
{
	bool close = (action == DOOR_CLOSED || action == DOOR_CLOSING);
	radiator_status = action;
	if (action <= DOOR_OPEN) {
		radiator_proc = (action == DOOR_CLOSED ? 0.0 : 1.0);
		SetAnimation (anim_radiator, radiator_proc);
		UpdateStatusIndicators();
	}
	oapiTriggerPanelRedrawArea (1, AID_SWITCHARRAY);
	SetAnimation (anim_radiatorswitch, close ? 0:1);
	UpdateCtrlDialog (this);
	RecordEvent ("RADIATOR", close ? "CLOSE" : "OPEN");
}

void DeltaGlider::RevertRadiator (void)
{
	ActivateRadiator (radiator_status == DOOR_CLOSED || radiator_status == DOOR_CLOSING ?
		DOOR_OPENING : DOOR_CLOSING);
}

bool DeltaGlider::DecAttMode ()
{
	int mode = GetAttitudeMode();
	if (mode) {
		SetAttitudeMode (mode-1);
		return true;
	} else return false;
}

bool DeltaGlider::IncAttMode ()
{
	int mode = GetAttitudeMode();
	if (mode < 2) {
		SetAttitudeMode (mode+1);
		return true;
	} else return false;
}

bool DeltaGlider::DecADCMode ()
{
	DWORD mode = min (GetADCtrlMode(),2);
	if (mode) {
		SetADCtrlMode (mode-1);
		return true;
	} else return false;
}

bool DeltaGlider::IncADCMode ()
{
	DWORD mode = GetADCtrlMode();
	if (mode <= 1) {
		SetADCtrlMode (mode ? 7 : 1);
		return true;
	} else return false;
}

void DeltaGlider::SetMainRetroLevel (int which, double lmain, double lretro)
{
	if (which == 2) {   // set main thruster group
		SetThrusterGroupLevel (THGROUP_MAIN,  lmain);
		SetThrusterGroupLevel (THGROUP_RETRO, lretro);
	} else {            // set individual engine
		SetThrusterLevel (th_main [which], lmain);
		SetThrusterLevel (th_retro[which], lretro);
	}
}

void DeltaGlider::SetScramLevel (int which, double level)
{
	for (int i = 0; i < 2; i++) {
		if (which != 1-i) {
			SetThrusterLevel (th_scram[i], level);
			scram_intensity[i] = level * scram_max[i];
		}
	}
}

void DeltaGlider::EnableRetroThrusters (bool state)
{
	for (int i = 0; i < 2; i++)
		SetThrusterResource (th_retro[i], state ? ph_main : NULL);
}

double DeltaGlider::GetMaxHoverThrust () const
{
	double th0 = 0.0;
	for (int i = 0; i < 3; i++)
		th0 += GetThrusterMax (th_hover[i]);
	return th0;
}

bool DeltaGlider::GetBeaconState (int which)
{
	switch (which) {
		case 0: return beacon[0].active;
		case 1: return beacon[3].active;
		case 2: return beacon[5].active;
		case 3: return beacon[7].active;
		default: return false;
	}
}

void DeltaGlider::TestDamage ()
{
	bool newdamage = false;
	double dt = oapiGetSimStep();

	// airframe damage as a result of wingload stress
	// or excessive dynamic pressure

	double load = GetLift() / 190.0; // L/S
	double dynp = GetDynPressure();  // dynamic pressure
	if (load > WINGLOAD_MAX || load < WINGLOAD_MIN || dynp > DYNP_MAX) {
		double alpha = max ((dynp-DYNP_MAX) * 1e-5,
			(load > 0 ? load-WINGLOAD_MAX : WINGLOAD_MIN-load) * 5e-5);
		double p = 1.0 - exp (-alpha*dt); // probability of failure
		if (oapiRand() < p) {
			// simulate structural failure by distorting the airfoil definition
			int rfail = rand();
			switch (rfail & 3) {
			case 0: // fail left wing
				lwingstatus *= exp (-alpha*oapiRand());
				break;
			case 1: // fail right wing
				rwingstatus *= exp (-alpha*oapiRand());
				break;
			case 2: { // fail left aileron
				if (hlaileron) {
					DelControlSurface (hlaileron);
					hlaileron = NULL;
				}
				aileronfail[rfail&4?0:1] = true;
				} break;
			case 3: { // fail right aileron
				if (hraileron) {
					DelControlSurface (hraileron);
					hraileron = NULL;
				}
				aileronfail[rfail&4?2:3] = true;
				} break;
			}
			newdamage = true;
		}
	}

	// top hatch damage
	if (hatch_proc > 0.05 && hatchfail < 2 && dynp > 30e3) {
		if (oapiRand() < 1.0 - exp(-dt*0.2)) {
			hatchfail++;
			newdamage = true;
		}
	}

	if (newdamage) {
		bMWSActive = true;
		ApplyDamage ();
		//UpdateDamageDialog (this);
	}
}

static UINT HatchGrp[2] = {12,88};

void DeltaGlider::ApplyDamage ()
{
	double balance = (rwingstatus-lwingstatus)*3.0;
	double surf    = (rwingstatus+lwingstatus)*35.0 + 20.0;
	EditAirfoil (hwing, 0x09, _V(balance,0,-0.3), 0, 0, surf, 0);

	if (rwingstatus < 1 || lwingstatus < 1) bMWSActive = true;

	SetDamageVisuals();
}

void DeltaGlider::RepairDamage ()
{
	int i;
	lwingstatus = rwingstatus = 1.0;
	EditAirfoil (hwing, 0x09, _V(0,0,-0.3), 0, 0, 90.0, 0);
	if (!hlaileron)
		hlaileron = CreateControlSurface2 (AIRCTRL_AILERON, 0.3, 1.5, _V( 7.5,0,-7.2), AIRCTRL_AXIS_XPOS, anim_raileron);
	if (!hraileron)
		hraileron = CreateControlSurface2 (AIRCTRL_AILERON, 0.3, 1.5, _V(-7.5,0,-7.2), AIRCTRL_AXIS_XNEG, anim_laileron);
	for (i = 0; i < 4; i++)
		aileronfail[i] = false;
	hatchfail = 0;
	bMWSActive = false;
	oapiTriggerRedrawArea (0,0, AID_MWS);
	//UpdateDamageDialog (this);
	SetDamageVisuals();
}

bool DeltaGlider::RedrawPanel_AOA (SURFHANDLE surf, bool force)
{
	static const double dial_max = RAD*165.0;
	static const double eps = 1e-2;

	double aoa = GetAOA();
	double dial_angle = PI - min (dial_max, max (-dial_max, aoa*7.7));
	if (force || fabs(dial_angle-aoa_ind) > eps) {
		oapiBltPanelAreaBackground (AID_AOAINSTR, surf);
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 28, 28, 26.0, dial_angle, &aoa_ind);
		oapiReleaseDC (surf, hDC);
		return true;
	} else return false;
}

bool DeltaGlider::RedrawPanel_Slip (SURFHANDLE surf, bool force)
{
	static const double dial_max = RAD*165.0;
	static const double eps = 1e-2;

	double slip = GetSlipAngle();
	double dial_angle = PI05 - min (dial_max, max (-dial_max, slip*7.7));
	if (force || fabs(dial_angle-slip_ind) > eps) {
		oapiBltPanelAreaBackground (AID_SLIPINSTR, surf);
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 28, 28, 26.0, dial_angle, &slip_ind);
		oapiReleaseDC (surf, hDC);
		return true;
	} else return false;
}

bool DeltaGlider::RedrawPanel_Wingload (SURFHANDLE surf, bool force)
{
	static const double dial_min = -123.0*RAD;
	static const double dial_max =  217.0*RAD;
	static const double eps = 1e-2;

	double load = GetLift() / 190.0; // L/S
	double dial_angle = PI - min (dial_max, max (dial_min, load/15.429e3*PI));
	if (force || fabs (dial_angle-load_ind) > eps) {
		oapiBltPanelAreaBackground (AID_LOADINSTR, surf);
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 28, 28, 26.0, dial_angle, &load_ind);
		oapiReleaseDC (surf, hDC);
		return true;
	} else return false;
}

void DeltaGlider::RedrawVC_ThScram ()
{
	for (int i = 0; i < 2; i++) {
		double level = GetThrusterLevel (th_scram[i]);
		UINT pos = (UINT)(level*500.0);
		if (pos != engsliderpos[i+3]) {
			SetAnimation (anim_scramthrottle[i], level);
			engsliderpos[i+3] = pos;
		}
	}
}

bool DeltaGlider::RedrawPanel_ScramTSFC (SURFHANDLE surf)
{
	bool redraw = false;
	for (int i = 0; i < 2; i++) {
		int p = min (66, (int)(scramjet->TSFC(i)*(1e3*66.0/0.03)));
		if (p != scTSFCidx[i])
			scTSFCidx[i] = p, redraw = true;
	}
	if (redraw) {
		oapiBltPanelAreaBackground (AID_SCRAMDISP3, surf);
		return RedrawPanel_IndicatorPair (surf, scTSFCidx, 66);
	} else return false;
}

bool DeltaGlider::RedrawPanel_ScramFlow (SURFHANDLE surf)
{
	bool redraw = false;
	for (int i = 0; i < 2; i++) {
		int p = min (66, (int)(scramjet->DMF(i)/3.0*67.0));
		if (p != scflowidx[i])
			scflowidx[i] = p, redraw = true;
	}
	if (redraw) {
		oapiBltPanelAreaBackground (AID_SCRAMDISP2, surf);
		return RedrawPanel_IndicatorPair (surf, scflowidx, 66);
	} else return false;
}

#ifdef UNDEF
bool DeltaGlider::RedrawPanel_MainProp (SURFHANDLE surf)
{
	double m = GetPropellantMass (ph_main);
	double lvl = m / max (1.0, max_rocketfuel);
	int p = min (88, (int)(lvl*89.0));
	if (p != *mainpropidx) {
		mainpropidx[0] = mainpropidx[1] = p;
		oapiBltPanelAreaBackground (AID_MAINPROP, surf);
		return RedrawPanel_IndicatorPair (surf, mainpropidx, 88);
	} else return false;
}

bool DeltaGlider::RedrawPanel_MainPropMass (SURFHANDLE surf)
{
	int m = (int)(GetPropellantMass (ph_main)+0.5);
	if (m != mainpropmass) {
		char cbuf[8];
		mainpropmass = m;
		sprintf (cbuf, "%05d", m);
		return RedrawPanel_Number (surf, 0, 0, cbuf);
	} else return false;
}
#endif

bool DeltaGlider::RedrawPanel_RCSProp (SURFHANDLE surf)
{
	double m = GetPropellantMass (ph_rcs);
	double lvl = m / RCS_FUEL_CAPACITY;
	int p = min (88, (int)(lvl*89.0));
	if (p != *rcspropidx) {
		rcspropidx[0] = rcspropidx[1] = p;
		oapiBltPanelAreaBackground (AID_RCSPROP, surf);
		return RedrawPanel_IndicatorPair (surf, rcspropidx, 88);
	} else return false;
}

bool DeltaGlider::RedrawPanel_RCSPropMass (SURFHANDLE surf)
{
	int m = (int)(GetPropellantMass (ph_rcs)+0.5);
	if (m != rcspropmass) {
		char cbuf[8];
		rcspropmass = m;
		sprintf (cbuf, "%03d", m);
		return RedrawPanel_Number (surf, 0, 0, cbuf);
	} else return false;
}

bool DeltaGlider::RedrawPanel_ScramProp (SURFHANDLE surf)
{
	double m = GetPropellantMass (ph_scram);
	double lvl = m / max (1.0, max_scramfuel);
	int p = min (88, (int)(lvl*89.0));
	if (p != *scrampropidx) {
		scrampropidx[0] = scrampropidx[1] = p;
		oapiBltPanelAreaBackground (AID_SCRAMPROP, surf);
		return RedrawPanel_IndicatorPair (surf, scrampropidx, 88);
	} else return false;
}

bool DeltaGlider::RedrawPanel_ScramPropMass (SURFHANDLE surf)
{
	int m = (int)(GetPropellantMass (ph_scram)+0.5);
	if (m != scrampropmass) {
		char cbuf[8];
		scrampropmass = m;
		sprintf (cbuf, "%05d", m);
		return RedrawPanel_Number (surf, 0, 0, cbuf);
	} else return false;
}

bool DeltaGlider::RedrawPanel_ScramTempDisp (SURFHANDLE surf)
{
	double T, phi;
	static const double rad = 19.0;
	int i, j, x0, y0, dx, dy;
	bool isVC = (oapiCockpitMode() == COCKPIT_VIRTUAL);

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.pen[0]);
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 2; i++) {
			T = scramjet->Temp (i, j);
			phi = PI * min (T,3900.0)/2000.0;
			dx = (int)(rad*sin(phi)), dy = (int)(rad*cos(phi));
			x0 = (isVC ? 20 : 22-j) + i*43;
			y0 = 19+j*46;
			MoveToEx (hDC, x0, y0, NULL); LineTo (hDC, x0+dx, y0-dy);
		}
	}
	SelectObject (hDC, GetStockObject (BLACK_PEN));
	oapiReleaseDC (surf, hDC);
	return true;
}

bool DeltaGlider::RedrawPanel_IndicatorPair (SURFHANDLE surf, int *p, int range)
{
	oapiBlt (surf, srf[8], 0, range-p[0], 0, 0, 6, 7, SURF_PREDEF_CK);
	oapiBlt (surf, srf[8], 7, range-p[1], 6, 0, 6, 7, SURF_PREDEF_CK);
	return true;
}

bool DeltaGlider::RedrawPanel_Number (SURFHANDLE surf, int x, int y, char *num)
{
	for (; *num; num++) {
		oapiBlt (surf, srf[10], x, y, (*num-'0')*7, 0, 6, 9);
		x += 6;
	}
	return true;
}
// D. Beachy: begin refactored section to fix flow rate panels
bool DeltaGlider::RedrawPanel_MainFlow (SURFHANDLE surf)
{
	double gaugeSize = 66.99;  // pointer can move 66 pixels; also round up to next pixel
	bool redraw = false;
	for (int i = 0; i < 2; i++) {
		double flowRate = GetThrusterFlowRate(th_main[i]);
        int p = (int)min(flowRate*gaugeSize/5.1, gaugeSize);  // gauge maxes out at 5.1
		if (p != mainflowidx[i])
			mainflowidx[i] = p, redraw = true;
	}
	if (redraw) {
		oapiBltPanelAreaBackground (AID_MAINDISP1, surf);
		return RedrawPanel_IndicatorPair (surf, mainflowidx, 66);
	} else return false;
}

bool DeltaGlider::RedrawPanel_RetroFlow (SURFHANDLE surf)
{
    double gaugeSize = 66.99;  // pointer can move 66 pixels; also round up to next pixel
	bool redraw = false;
	for (int i = 0; i < 2; i++) {
		double flowRate = GetThrusterFlowRate(th_retro[i]); 
		int p = (int)min(flowRate*gaugeSize/0.9,gaugeSize); // gauge maxes out at 0.9
		if (p != retroflowidx[i])
			retroflowidx[i] = p, redraw = true;
	}
	if (redraw) {
		oapiBltPanelAreaBackground (AID_MAINDISP2, surf);
		return RedrawPanel_IndicatorPair (surf, retroflowidx, 66);
	} else return false;
}

bool DeltaGlider::RedrawPanel_HoverFlow (SURFHANDLE surf)
{
    double gaugeSize = 66.99;  // pointer can move 66 pixels; also round up to next pixel
    // since hover flow rates are always locked we can assume the second hover thruster has the same flow as the first
    double flowRate = GetThrusterFlowRate(th_hover[0]); 
	int p = (int)min(flowRate*gaugeSize/3.6,gaugeSize); // gauge maxes out at 3.6
	if (p != hoverflowidx) {
		hoverflowidx = p;
		oapiBltPanelAreaBackground (AID_MAINDISP3, surf);
		// draw a pair of matching indicators
		oapiBlt (surf, srf[8], 0, 66-hoverflowidx, 0, 0, 6, 7, SURF_PREDEF_CK);
		oapiBlt (surf, srf[8], 7, 66-hoverflowidx, 6, 0, 6, 7, SURF_PREDEF_CK);
		return true;
	} else return false;
}

double DeltaGlider::GetThrusterFlowRate(THRUSTER_HANDLE th)
{
    double level  = GetThrusterLevel(th); // throttle level
    double isp    = GetThrusterIsp0(th);
    double thrust = GetThrusterMax0(th);
    double flow   = thrust*level/isp;
    
    return flow;
}
// D. Beachy: end refactored section

bool DeltaGlider::RedrawPanel_MainTSFC (SURFHANDLE surf)
{
	static const double scalemin = 0.02, scalemax = 0.035; // display range
	double tsfc = 1e3/GetThrusterIsp (th_main[0]);
	// we assume that all DG rocket engines have the same Isp
	int p = max (0, min (66, (int)((tsfc-scalemin)/(scalemax-scalemin)*67.0)));
	if (p != mainTSFCidx) {
		mainTSFCidx = p;
		oapiBltPanelAreaBackground (AID_MAINDISP4, surf);
		oapiBlt (surf, srf[8], 0, 66-mainTSFCidx, 0, 0, 6, 7, SURF_PREDEF_CK);
		return true;
	} else return false;
}

void DeltaGlider::UpdateStatusIndicators ()
{
	if (!vcmesh) return;
	float x, xon = 0.845f, xoff = 0.998f;
	double d;

	static NTVERTEX vtx[16];
	static WORD vidx[16] = {0,1,4,5,20,21,8,9,24,25,16,17,12,13,28,29};
	GROUPEDITSPEC ges;
	ges.flags = GRPEDIT_VTXTEXU;
	ges.nVtx = 16;
	ges.vIdx = vidx;
	ges.Vtx = vtx;

	// gear indicator
	x = (gear_status == DOOR_CLOSED ? xoff : gear_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[0].tu = vtx[1].tu = x;

	// retro cover indicator
	x = (ssys_mainretro->RCoverStatus() == DOOR_CLOSED ? xoff : ssys_mainretro->RCoverStatus() == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[2].tu = vtx[3].tu = x;

	// airbrake indicator
	x = (brake_status == DOOR_CLOSED ? xoff : brake_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[4].tu = vtx[5].tu = x;

	// nose cone indicator
	x = (nose_status == DOOR_CLOSED ? xoff : nose_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[6].tu = vtx[7].tu = x;

	// top hatch indicator
	x = (hatch_status == DOOR_CLOSED ? xoff : hatch_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[8].tu = vtx[9].tu = x;

	// radiator indicator
	x = (radiator_status == DOOR_CLOSED ? xoff : radiator_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[10].tu = vtx[11].tu = x;

	// outer airlock indicator
	x = (olock_status == DOOR_CLOSED ? xoff : olock_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[12].tu = vtx[13].tu = x;

	// inner airlock indicator
	x = (ilock_status == DOOR_CLOSED ? xoff : ilock_status == DOOR_OPEN ? xon : modf (oapiGetSimTime(), &d) < 0.5 ? xon : xoff);
	vtx[14].tu = vtx[15].tu = x;

	oapiEditMeshGroup (vcmesh, GRP_STATUS_INDICATOR_VC, &ges);
}

void DeltaGlider::SetPassengerVisuals ()
{
	if (!(vcmesh && exmesh)) return;
	GROUPEDITSPEC ges;

	static int expsngridx[4] = {106, 107, 108, 109};
	static int exvisoridx[4] = {111, 112, 113, 114};
	static int vcpsngridx[4] = {GRP_PASSENGER1_VC, GRP_PASSENGER2_VC, GRP_PASSENGER3_VC, GRP_PASSENGER4_VC};
	static int vcvisoridx[4] = {GRP_PASSENGER1_VISOR_VC, GRP_PASSENGER2_VISOR_VC, GRP_PASSENGER3_VISOR_VC, GRP_PASSENGER4_VISOR_VC};

	for (DWORD i = 0; i < 4; i++) {
		if (psngr[i]) {
			ges.flags = GRPEDIT_SETUSERFLAG;
			ges.UsrFlag = 1;
		} else {
			ges.flags = GRPEDIT_ADDUSERFLAG;
			ges.UsrFlag = 3;
		}
		oapiEditMeshGroup (exmesh, expsngridx[i], &ges);
		oapiEditMeshGroup (exmesh, exvisoridx[i], &ges);
		oapiEditMeshGroup (vcmesh, vcpsngridx[i], &ges);
		oapiEditMeshGroup (vcmesh, vcvisoridx[i], &ges);
	}
}

static UINT AileronGrp[8] = {29,51,30,52,35,55,36,54};

void DeltaGlider::SetDamageVisuals ()
{
	if (!exmesh) return;
	GROUPEDITSPEC ges;

	int i, j;

	// ailerons
	static UINT LAileronGrp[4] = {29,51,30,52};
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 2; j++)
			if (aileronfail[i]) {
				ges.flags = GRPEDIT_ADDUSERFLAG;
				ges.UsrFlag = 3;
				oapiEditMeshGroup (exmesh, AileronGrp[i*2+j], &ges);
			} else {
				ges.flags = GRPEDIT_SETUSERFLAG;
				ges.UsrFlag = 0;
				oapiEditMeshGroup (exmesh, AileronGrp[i*2+j], &ges);
			}
	}

	// top hatch
	for (i = 0; i < 2; i++) {
		ges.flags = GRPEDIT_SETUSERFLAG;
		ges.UsrFlag = (hatchfail < 2 ? 0:3);
		oapiEditMeshGroup (exmesh, HatchGrp[i], &ges);
	}
	if (hatchfail == 1)
		SetAnimation (anim_hatch, hatch_proc = 0.2);
}

void DeltaGlider::DrawNeedle (HDC hDC, int x, int y, double rad, double angle, double *pangle, double vdial)
{
	if (pangle) { // needle response delay
		double dt = oapiGetSimStep();
		if (fabs (angle - *pangle)/dt >= vdial)
			angle = (angle > *pangle ? *pangle+vdial*dt : *pangle-vdial*dt);
		*pangle = angle;
	}
	double dx = rad * cos(angle), dy = rad * sin(angle);
	SelectObject (hDC, g_Param.pen[1]);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, g_Param.pen[0]);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
}

void DeltaGlider::InitVCMesh()
{
	SetInstrLight (instr_light_on, true);
	SetFloodLight (flood_light_mode);
	SetLandDockLight (landdock_light_mode);
	SetStrobeLight (strobe_light_on);
	SetNavLight (nav_light_on);
	SetAnimation (anim_instrbdial, instr_brightness);
	SetAnimation (anim_floodbdial, flood_brightness);

	if (vcmesh) {
		// hide pilot head in VCPILOT position
		GROUPEDITSPEC ges;
		ges.flags = (campos < CAM_VCPSNGR1 ? GRPEDIT_ADDUSERFLAG : GRPEDIT_DELUSERFLAG);
		ges.UsrFlag = 3;
		oapiEditMeshGroup (vcmesh, GRP_PILOT_HEAD_VC, &ges);
		oapiEditMeshGroup (vcmesh, GRP_PILOT_VISOR_VC, &ges);

		for (DWORD i = 0; i < ninstr; i++) if (instr[i]) instr[i]->ResetVC (vcmesh);

		for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
			(*it)->clbkResetVC (0, vcmesh);
	}
}

// ==============================================================
// Overloaded callback functions
// ==============================================================

// --------------------------------------------------------------
// Set vessel class parameters
// --------------------------------------------------------------
void DeltaGlider::clbkSetClassCaps (FILEHANDLE cfg)
{
	// *************** physical parameters **********************

	bool b;
	int i;
	if (oapiReadItem_bool (cfg, "SCRAMJET", b) && b) // set up scramjet configuration
		scramjet = new Ramjet (this);

	VESSEL4::SetEmptyMass (scramjet ? EMPTY_MASS_SC : EMPTY_MASS);
	VECTOR3 r[2] = {{0,0,6}, {0,0,-4}};
	SetSize (10.0);
	SetVisibilityLimit (7.5e-4, 1.5e-3);
	SetAlbedoRGB (_V(0.77,0.20,0.13));
	SetGravityGradientDamping (20.0);
	SetCW (0.09, 0.09, 2, 1.4);
	SetWingAspect (0.7);
	SetWingEffectiveness (2.5);
	SetCrossSections (_V(53.0,186.9,25.9));
	SetMaxWheelbrakeForce (2e5);
	SetPMI (_V(15.5,22.1,7.7));

	SetDockParams (_V(0,-0.49,10.076), _V(0,0,1), _V(0,1,0));
	SetTouchdownPoints (tdvtx_geardown, ntdvtx_geardown);
	SetNosewheelSteering (true);
	bGearIsDown = true;
	EnableTransponder (true);
	bool render_cockpit = true;

	// ******************** NAV radios **************************

	InitNavRadios (4);

	// ****************** propellant specs **********************

	tankconfig = (scramjet ? 1:0);
	switch (tankconfig) {
	case 0:
		max_rocketfuel = TANK1_CAPACITY + TANK2_CAPACITY;
		max_scramfuel = 0.0;
		break;
	case 1:
		max_rocketfuel = TANK1_CAPACITY;
		max_scramfuel = TANK2_CAPACITY;
		break;
	}

	ph_main  = CreatePropellantResource (max_rocketfuel);    // main tank (fuel + oxydant)
	ph_rcs   = CreatePropellantResource (RCS_FUEL_CAPACITY); // RCS tank  (fuel + oxydant)
	if (max_scramfuel)
		ph_scram = CreatePropellantResource (max_scramfuel); // scramjet fuel

	// **************** thruster definitions ********************

	double ispscale = (modelidx ? 0.8 : 1.0);
	// Reduction of thrust efficiency at normal pressure

	contrail_tex = oapiRegisterParticleTexture ("Contrail1a");

	PARTICLESTREAMSPEC contrail = {
		0, 8.0, 4, 150, 0.25, 3.0, 4, 2.0, PARTICLESTREAMSPEC::DIFFUSE,
		PARTICLESTREAMSPEC::LVL_PSQRT, 0, 2,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-4, 1,
		contrail_tex
	};
	PARTICLESTREAMSPEC exhaust_main = {
		0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
	};
	PARTICLESTREAMSPEC exhaust_hover = {
		0, 1.5, 30, 150, 0.1, 0.1, 12, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
	};
	PARTICLESTREAMSPEC exhaust_scram = {
		0, 2.0, 10, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
	};

	// main thrusters
	th_main[0] = CreateThruster (_V(-1,0.0,-7.7), _V(0,0,1), MAX_MAIN_THRUST[modelidx], ph_main, ISP, ISP*ispscale);
	th_main[1] = CreateThruster (_V( 1,0.0,-7.7), _V(0,0,1), MAX_MAIN_THRUST[modelidx], ph_main, ISP, ISP*ispscale);
	thg_main = CreateThrusterGroup (th_main, 2, THGROUP_MAIN);
	EXHAUSTSPEC es_main[2] = {
		{th_main[0], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL},
		{th_main[1], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL}
	};
	for (i = 0; i < 2; i++) AddExhaust (es_main+i);
	AddExhaustStream (th_main[0], _V(-1,0,-15), &contrail);
	AddExhaustStream (th_main[1], _V( 1,0,-15), &contrail);
	AddExhaustStream (th_main[0], _V(-1,0,-10), &exhaust_main);
	AddExhaustStream (th_main[1], _V( 1,0,-10), &exhaust_main);
	//DWORD i = GetGroupThrusterCount (THGROUP_MAIN);

	// retro thrusters
	// note that we have to tilt retros slightly downwards to avoid inducing
	// an angular momentum, since they are mounted below the level of CG.
	// This also means that retros will induce an upward linear component.
	th_retro[0] = CreateThruster (_V(-3,-0.236,5.6), _V(0,0.04210548,-0.99911317), MAX_RETRO_THRUST, ph_main, ISP, ISP*ispscale);
	th_retro[1] = CreateThruster (_V( 3,-0.236,5.6), _V(0,0.04210548,-0.99911317), MAX_RETRO_THRUST, ph_main, ISP, ISP*ispscale);
	thg_retro = CreateThrusterGroup (th_retro, 2, THGROUP_RETRO);
	EXHAUSTSPEC es_retro[2] = {{th_retro[0], NULL, NULL, NULL, 3, 0.4, 0, 0.1, NULL}, {th_retro[1], NULL, NULL, NULL, 3, 0.4, 0, 0.1, NULL}};
	for (i = 0; i < 2; i++) AddExhaust (es_retro+i);

	// hover thrusters
	th_hover[0] = CreateThruster (_V(0,0,3), _V(0,1,0), MAX_HOVER_THRUST[modelidx], ph_main, ISP, ISP*ispscale);
	th_hover[1] = CreateThruster (_V(-3,0,-4.55), _V(0,1,0), 3.0/4.55*0.5*MAX_HOVER_THRUST[modelidx], ph_main, ISP, ISP*ispscale);
	th_hover[2] = CreateThruster (_V( 3,0,-4.55), _V(0,1,0), 3.0/4.55*0.5*MAX_HOVER_THRUST[modelidx], ph_main, ISP, ISP*ispscale);
	thg_hover = CreateThrusterGroup (th_hover, 3, THGROUP_HOVER);
	VECTOR3 hoverp0 = {0,-1.5, 3}, hoverp1 = {-3,-1.3,-4.55}, hoverp2 = {3,-1.3,-4.55}, hoverd = {0,1,0};
	EXHAUSTSPEC es_hover[3] = {
		{th_hover[0], NULL, &hoverp0, &hoverd, 6, 0.5, 0, 0.1, NULL, EXHAUST_CONSTANTPOS|EXHAUST_CONSTANTDIR},
		{th_hover[1], NULL, &hoverp1, &hoverd, 6, 0.5, 0, 0.1, NULL, EXHAUST_CONSTANTPOS|EXHAUST_CONSTANTDIR},
		{th_hover[2], NULL, &hoverp2, &hoverd, 6, 0.5, 0, 0.1, NULL, EXHAUST_CONSTANTPOS|EXHAUST_CONSTANTDIR}
	};
	for (i = 0; i < 3; i++) AddExhaust (es_hover+i);
	AddExhaustStream (th_hover[0], _V(0,-4,0), &contrail);
	AddExhaustStream (th_hover[0], _V(0,-2,3), &exhaust_hover);
	AddExhaustStream (th_hover[1], _V(-3,-2,-4.55), &exhaust_hover);
	AddExhaustStream (th_hover[2], _V( 3,-2,-4.55), &exhaust_hover);

	// set of attitude thrusters (idealised). The arrangement is such that no angular
	// momentum is created in linear mode, and no linear momentum is created in rotational mode.
	THRUSTER_HANDLE th_att_rot[4], th_att_lin[4];
	th_att_rot[0] = th_att_lin[0] = CreateThruster (_V(0,0, 8), _V(0, 1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[1] = th_att_lin[3] = CreateThruster (_V(0,0,-8), _V(0,-1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[2] = th_att_lin[2] = CreateThruster (_V(0,0, 8), _V(0,-1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[3] = th_att_lin[1] = CreateThruster (_V(0,0,-8), _V(0, 1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	CreateThrusterGroup (th_att_rot,   2, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+2, 2, THGROUP_ATT_PITCHDOWN);
	CreateThrusterGroup (th_att_lin,   2, THGROUP_ATT_UP);
	CreateThrusterGroup (th_att_lin+2, 2, THGROUP_ATT_DOWN);
	AddExhaust (th_att_rot[0], 0.6,  0.078, _V(-0.75,-0.7,  9.65), _V(0,-1,0));
	AddExhaust (th_att_rot[0], 0.6,  0.078, _V( 0.75,-0.7,  9.65), _V(0,-1,0));
	AddExhaust (th_att_rot[1], 0.79, 0.103, _V(-0.1 , 0.55,-7.3 ), _V(0, 1,0));
	AddExhaust (th_att_rot[1], 0.79, 0.103, _V( 0.1 , 0.55,-7.3 ), _V(0, 1,0));
	AddExhaust (th_att_rot[2], 0.6,  0.078, _V(-0.8,-0.25, 9.6), _V(0, 1,0));
	AddExhaust (th_att_rot[2], 0.6,  0.078, _V( 0.8,-0.25, 9.6), _V(0, 1,0));
	AddExhaust (th_att_rot[3], 0.79, 0.103, _V(-0.1, -0.55,-7.3 ), _V(0,-1,0));
	AddExhaust (th_att_rot[3], 0.79, 0.103, _V( 0.1, -0.55,-7.3 ), _V(0,-1,0));

	th_att_rot[0] = th_att_lin[0] = CreateThruster (_V(0,0, 6), _V(-1,0,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[1] = th_att_lin[3] = CreateThruster (_V(0,0,-6), _V( 1,0,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[2] = th_att_lin[2] = CreateThruster (_V(0,0, 6), _V( 1,0,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[3] = th_att_lin[1] = CreateThruster (_V(0,0,-6), _V(-1,0,0), MAX_RCS_THRUST, ph_rcs, ISP);
	CreateThrusterGroup (th_att_rot,   2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+2, 2, THGROUP_ATT_YAWRIGHT);
	CreateThrusterGroup (th_att_lin,   2, THGROUP_ATT_LEFT);
	CreateThrusterGroup (th_att_lin+2, 2, THGROUP_ATT_RIGHT);
	AddExhaust (th_att_rot[0], 0.6,  0.078, _V(1.0,-0.48,9.35), _V(1,0,0));
	AddExhaust (th_att_rot[1], 0.94, 0.122, _V(-2.2,0.2,-6.0), _V(-1,0,0));
	AddExhaust (th_att_rot[2], 0.6,  0.078, _V(-1.0,-0.48,9.35), _V(-1,0,0));
	AddExhaust (th_att_rot[3], 0.94, 0.122, _V(2.2,0.2,-6.0), _V(1,0,0));

	th_att_rot[0] = CreateThruster (_V( 6,0,0), _V(0, 1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[1] = CreateThruster (_V(-6,0,0), _V(0,-1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[2] = CreateThruster (_V(-6,0,0), _V(0, 1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_rot[3] = CreateThruster (_V( 6,0,0), _V(0,-1,0), MAX_RCS_THRUST, ph_rcs, ISP);
	CreateThrusterGroup (th_att_rot, 2, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+2, 2, THGROUP_ATT_BANKRIGHT);
	AddExhaust (th_att_rot[0], 1.03, 0.134, _V(-5.1, 0.2,0.4), _V(0, 1,0));
	AddExhaust (th_att_rot[1], 1.03, 0.134, _V( 5.1,-0.8,0.4), _V(0,-1,0));
	AddExhaust (th_att_rot[2], 1.03, 0.134, _V( 5.1, 0.2,0.4), _V(0, 1,0));
	AddExhaust (th_att_rot[3], 1.03, 0.134, _V(-5.1,-0.8,0.4), _V(0,-1,0));

	th_att_lin[0] = CreateThruster (_V(0,0,-7), _V(0,0, 1), 2*MAX_RCS_THRUST, ph_rcs, ISP);
	th_att_lin[1] = CreateThruster (_V(0,0, 7), _V(0,0,-1), 2*MAX_RCS_THRUST, ph_rcs, ISP);
	CreateThrusterGroup (th_att_lin,   1, THGROUP_ATT_FORWARD);
	CreateThrusterGroup (th_att_lin+1, 1, THGROUP_ATT_BACK);
	AddExhaust (th_att_lin[0], 0.6, 0.078, _V(0,-0.2,-7.6), _V(0,0,-1));
	AddExhaust (th_att_lin[0], 0.6, 0.078, _V(0,0.22,-7.6), _V(0,0,-1));
	AddExhaust (th_att_lin[1], 0.6, 0.078, _V(-0.82,-0.49,9.8), _V(0,0,1));
	AddExhaust (th_att_lin[1], 0.6, 0.078, _V( 0.82,-0.49,9.8), _V(0,0,1));

	COLOUR4 col_d = {0.9,0.8,1,0};
	COLOUR4 col_s = {1.9,0.8,1,0};
	COLOUR4 col_a = {0,0,0,0};
	COLOUR4 col_white = {1,1,1,0};
	LightEmitter *le = AddPointLight (_V(0,0,-10), 200, 1e-3, 0, 2e-3, col_d, col_s, col_a);
	le->SetIntensityRef (&th_main_level);

	// **************** scramjet definitions ********************

	if (scramjet) {
		VECTOR3 dir = {0.0, sin(SCRAM_DEFAULT_DIR), cos(SCRAM_DEFAULT_DIR)};

		for (int i = 0; i < 2; i++) {
			th_scram[i] = CreateThruster (_V(i?0.9:-0.9, -0.8, -5.6), dir, 0, ph_scram, 0);
			scramjet->AddThrusterDefinition (th_scram[i], SCRAM_FHV[modelidx],
				SCRAM_INTAKE_AREA, SCRAM_TEMAX[modelidx], SCRAM_MAX_DMF[modelidx]);
		}

		// thrust rating and ISP for scramjet engines are updated continuously
		//AddExhaust (th_scram[0], 10.0, 0.5);
		//AddExhaust (th_scram[1], 10.0, 0.5);
		PSTREAM_HANDLE ph;
		ph = AddExhaustStream (th_scram[0], _V(-1,-1.1,-5.4), &exhaust_scram);
		if (ph) oapiParticleSetLevelRef (ph, scram_intensity+0);
		ph = AddExhaustStream (th_scram[1], _V( 1,-1.1,-5.4), &exhaust_scram);
		if (ph) oapiParticleSetLevelRef (ph, scram_intensity+1);
	}

	// ********************* aerodynamics ***********************

	hwing = CreateAirfoil3 (LIFT_VERTICAL, _V(0,0,-0.3), VLiftCoeff, 0, 5, 90, 1.5);
	// wing and body lift+drag components

	CreateAirfoil3 (LIFT_HORIZONTAL, _V(0,0,-4), HLiftCoeff, 0, 5, 15, 1.5);
	// vertical stabiliser and body lift and drag components

	CreateControlSurface3 (AIRCTRL_ELEVATOR,     1.4, 1.7, _V(   0,0,-7.2), AIRCTRL_AXIS_XPOS, 1.0, anim_elevator);
	CreateControlSurface3 (AIRCTRL_RUDDER,       0.8, 1.7, _V(   0,0,-7.2), AIRCTRL_AXIS_YPOS, 1.0, anim_rudder);
	hlaileron = CreateControlSurface3 (AIRCTRL_AILERON, 0.3, 1.7, _V( 7.5,0,-7.2), AIRCTRL_AXIS_XPOS, 1.0, anim_raileron);
	hraileron = CreateControlSurface3 (AIRCTRL_AILERON, 0.3, 1.7, _V(-7.5,0,-7.2), AIRCTRL_AXIS_XNEG, 1.0, anim_laileron);
	CreateControlSurface3 (AIRCTRL_ELEVATORTRIM, 0.3, 1.7, _V(   0,0,-7.2), AIRCTRL_AXIS_XPOS, 1.0, anim_elevatortrim);

	CreateVariableDragElement (&gear_proc, 0.8, _V(0, -1, 0));     // landing gear
	CreateVariableDragElement (ssys_mainretro->RCoverPosition(), 0.2, _V(0,-0.5,6.5)); // retro covers
	CreateVariableDragElement (&nose_proc, 3, _V(0, 0, 8));        // nose cone
	CreateVariableDragElement (&radiator_proc, 1, _V(0,1.5,-4));   // radiator
	CreateVariableDragElement (&brake_proc, 4, _V(0,0,-8));        // airbrake

	SetRotDrag (_V(0.10,0.13,0.04));
	// angular damping

	// ************************* mesh ***************************

	// ********************* beacon lights **********************
	static VECTOR3 beaconpos[8] = {{-8.6,0,-3.3}, {8.6,0,-3.3}, {0,0.5,-7.5}, {0,2.2,2}, {0,-1.4,2}, {-8.9,2.5,-5.4}, {8.9,2.5,-5.4}, {2.5,-0.5,6.5}};
	static VECTOR3 beaconpos_scram = {0,-1.8,2};
	static VECTOR3 beaconcol[8] = {{1.0,0.5,0.5}, {0.5,1.0,0.5}, {1,1,1}, {1,0.6,0.6}, {1,0.6,0.6}, {1,1,1}, {1,1,1} , {1,1,1}};
	for (i = 0; i < 8; i++) {
		beacon[i].shape = (i < 3 ? BEACONSHAPE_DIFFUSE : BEACONSHAPE_STAR);
		beacon[i].pos = beaconpos+i;
		beacon[i].col = beaconcol+i;
		beacon[i].size = (i < 3 || i == 7 ? 0.3 : 0.55);
		beacon[i].falloff = (i < 3 ? 0.4 : 0.6);
		beacon[i].period = (i < 3 ? 0 : i < 5 ? 2 : i < 7 ? 1.13 : 0);
		beacon[i].duration = (i < 5 ? 0.1 : 0.05);
		beacon[i].tofs = (6-i)*0.2;
		beacon[i].active = false;
		AddBeacon (beacon+i);
	}
	if (scramjet) beacon[4].pos = &beaconpos_scram;

	//docking_light = (SpotLight*)AddSpotLight(_V(2.5,-0.5,6.5), _V(0,0,1), 150, 1e-3, 0, 1e-3, RAD*25, RAD*60, col_white, col_white, col_a);
	//docking_light->Activate(false);

	//static const COLOUR4 col_cockpit_light = {0.6,0.05,0,0}; //{1,1,1,0};
	//cockpit_light = (PointLight*)AddPointLight(_V(0,1.65,6.68), 3, 0, 0, 3, col_cockpit_light, col_cockpit_light, col_a);
	//cockpit_light->SetVisibility (LightEmitter::VIS_COCKPIT);
	//cockpit_light->Activate(false);

	SetMeshVisibilityMode (AddMesh (exmesh_tpl = oapiLoadMeshGlobal (ScramVersion() ? "DG\\deltaglider" : "DG\\deltaglider_ns")), MESHVIS_EXTERNAL);
	//SetMeshVisibilityMode (AddMesh (vcmesh_tpl = oapiLoadMeshGlobal ("DG\\deltaglider_vc")), MESHVIS_VC);
	panelmesh0 = oapiLoadMeshGlobal ("DG\\dg_2dpanel0");
	panelmesh1 = NULL; // we create this one inline when needed

	vcmesh_tpl = oapiLoadMeshGlobal ("DG\\deltaglider_vc");
	SetMeshVisibilityMode (AddMesh (vcmesh_tpl), MESHVIS_VC);

	// **************** vessel-specific insignia ****************

	insignia_tex = oapiCreateTextureSurface (256, 256);
	SURFHANDLE hTex = oapiGetTextureHandle (exmesh_tpl, 5);
	if (hTex) oapiBlt (insignia_tex, hTex, 0, 0, 0, 0, 256, 256);

	// **************** create cockpit elements *****************

	CreatePanelElements();
}

// --------------------------------------------------------------
// Read status from scenario file
// --------------------------------------------------------------
void DeltaGlider::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
    char *line;

	while (oapiReadScenario_nextline (scn, line)) {
        if (!_strnicmp (line, "NOSECONE", 8)) {
			sscanf (line+8, "%d%lf", &nose_status, &nose_proc);
			if (nose_status == DOOR_OPEN || nose_status == DOOR_OPENING) {
				noselever_status = DOOR_OPEN; noselever_proc = 1.0;
			} else {
				noselever_status = DOOR_CLOSED; noselever_proc = 0.0;
			}
		} else if (!_strnicmp (line, "GEAR", 4)) {
			sscanf (line+4, "%d%lf", &gear_status, &gear_proc);
			if (gear_status == DOOR_OPEN || gear_status == DOOR_OPENING) {
				gearlever_status = DOOR_OPEN; gearlever_proc = 1.0;
			} else {
				gearlever_status = DOOR_CLOSED; gearlever_proc = 0.0;
			}
		} else if (!_strnicmp (line, "AIRLOCK", 7)) {
			sscanf (line+7, "%d%lf", &olock_status, &olock_proc);
		} else if (!_strnicmp (line, "IAIRLOCK", 8)) {
			sscanf (line+8, "%d%lf", &ilock_status, &ilock_proc);
		} else if (!_strnicmp (line, "AIRBRAKE", 8)) {
			sscanf (line+8, "%d%lf", &brake_status, &brake_proc);
			if (fabs (brake_proc-0.5) < 0.1 && brake_status <= DOOR_OPEN) {
				airbrake_tgt = 1;
				airbrakelever_status = DOOR_CLOSED; airbrakelever_proc = 0.5;
			} else if (brake_status == DOOR_OPEN || brake_status == DOOR_OPENING) {
				airbrake_tgt = 2;
				airbrakelever_status = DOOR_OPEN; airbrakelever_proc = 1.0;
			} else {
				airbrake_tgt = 0;
				airbrakelever_status = DOOR_CLOSED; airbrakelever_proc = 0.0;
			}
		} else if (!_strnicmp (line, "RADIATOR", 8)) {
			sscanf (line+8, "%d%lf", &radiator_status, &radiator_proc);
		} else if (!_strnicmp (line, "LADDER", 6)) {
			sscanf (line+6, "%d%lf", &ladder_status, &ladder_proc);
		} else if (!_strnicmp (line, "HATCH", 5)) {
			sscanf (line+5, "%d%lf", &hatch_status, &hatch_proc);
		} else if (!_strnicmp (line, "TRIM", 4)) {
			double trim;
			sscanf (line+4, "%lf", &trim);
			SetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM, trim, true);
		} else if (!_strnicmp (line, "TANKCONFIG", 10)) {
			if (scramjet) sscanf (line+10, "%d", &tankconfig);
		} else if (!_strnicmp (line, "PSNGR", 5)) {
			DWORD i, res, pi[4];
			res = sscanf (line+5, "%d%d%d%d", pi+0, pi+1, pi+2, pi+3);
			for (i = 0; i < res; i++)
				if (pi[i]-1 < 4) psngr[pi[i]-1] = true;
		} else if (!_strnicmp (line, "SKIN", 4)) {
			sscanf (line+4, "%s", skinpath);
			char fname[256];
			strcpy (fname, "DG\\Skins\\");
			strcat (fname, skinpath);
			int n = strlen(fname); fname[n++] = '\\';
			strcpy (fname+n, "dgmk4_1.dds");  skin[0] = oapiLoadTexture (fname);
			strcpy (fname+n, scramjet ? "dgmk4_2.dds" : "dgmk4_2_ns.dds");  skin[1] = oapiLoadTexture (fname);
			strcpy (fname+n, "idpanel1.dds"); skin[2] = oapiLoadTexture (fname);
			if (skin[2]) {
				oapiBlt (insignia_tex, skin[2], 0, 0, 0, 0, 256, 256);
				oapiReleaseTexture (skin[2]);
				skin[2] = NULL;
			}
		} else if (!_strnicmp (line, "PANELCOL", 8)) {
			sscanf (line+8, "%d%d", panelcol+0, panelcol+1);
		} else if (!_strnicmp (line, "LIGHTS", 6)) {
			int lgt[4];
			sscanf (line+6, "%d%d%d%d", lgt+0, lgt+1, lgt+2, lgt+3);
			SetNavLight (lgt[0] != 0);
			//SetBeacon (lgt[1] != 0);
			SetStrobeLight (lgt[2] != 0);
			//SetDockingLight (lgt[3] != 0);
		} else if (!_strnicmp (line, "AAP", 3)) {
			aap->SetState (line);
        } else {
			int i;
			bool found = false;

			// offer the line to all subsystems
			for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
				if (found = (*it)->clbkParseScenarioLine (line))
					break;

			// unrecognised option - pass to Orbiter's generic parser
			if (!found)
	            ParseScenarioLineEx (line, vs);
        }
    }

	// modify tank configuration (DG-S only)
	if (tankconfig != 0) {
		switch (tankconfig) {
		case 1:
			max_rocketfuel = TANK1_CAPACITY;
			max_scramfuel  = TANK2_CAPACITY;
			break;
		case 2:
			max_rocketfuel = TANK2_CAPACITY;
			max_scramfuel  = TANK1_CAPACITY;
			break;
		}
		SetPropellantMaxMass (ph_main, max_rocketfuel);
		SetPropellantMaxMass (ph_scram, max_scramfuel);
	}
	SetGearParameters (gear_proc);
}

// --------------------------------------------------------------
// Write status to scenario file
// --------------------------------------------------------------
void DeltaGlider::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];
	int i;

	// Write default vessel parameters
	VESSEL4::clbkSaveState (scn);

	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkSaveState (scn);

	// Write custom parameters
	if (gear_status) {
		sprintf (cbuf, "%d %0.4f", gear_status, gear_proc);
		oapiWriteScenario_string (scn, "GEAR", cbuf);
	}
	if (nose_status) {
		sprintf (cbuf, "%d %0.4f", nose_status, nose_proc);
		oapiWriteScenario_string (scn, "NOSECONE", cbuf);
	}
	if (olock_status) {
		sprintf (cbuf, "%d %0.4f", olock_status, olock_proc);
		oapiWriteScenario_string (scn, "AIRLOCK", cbuf);
	}
	if (ilock_status) {
		sprintf (cbuf, "%d %0.4f", ilock_status, ilock_proc);
		oapiWriteScenario_string (scn, "IAIRLOCK", cbuf);
	}
	if (brake_status || brake_proc) {
		sprintf (cbuf, "%d %0.4f", brake_status, brake_proc);
		oapiWriteScenario_string (scn, "AIRBRAKE", cbuf);
	}
	if (radiator_status) {
		sprintf (cbuf, "%d %0.4f", radiator_status, radiator_proc);
		oapiWriteScenario_string (scn, "RADIATOR", cbuf);
	}
	if (ladder_status) {
		sprintf (cbuf, "%d %0.4f", ladder_status, ladder_proc);
		oapiWriteScenario_string (scn, "LADDER", cbuf);
	}
	if (hatch_status) {
		sprintf (cbuf, "%d %0.4lf", hatch_status, hatch_proc);
		oapiWriteScenario_string (scn, "HATCH", cbuf);
	}
	for (i = 0; i < 4; i++)
		if (psngr[i]) {
			sprintf (cbuf, "%d", i+1);
			for (++i; i < 4; i++)
				if (psngr[i]) sprintf (cbuf+strlen(cbuf), " %d", i+1);
			oapiWriteScenario_string (scn, "PSNGR", cbuf);
			break;
		}
	if (skinpath[0])
		oapiWriteScenario_string (scn, "SKIN", skinpath);
	if (panelcol[0] || panelcol[1]) {
		sprintf (cbuf, "%d %d", panelcol[0], panelcol[1]);
		oapiWriteScenario_string (scn, "PANELCOL", cbuf);
	}
	for (i = 0; i < 8; i++)
		if (beacon[i].active) {
			sprintf (cbuf, "%d %d %d %d", beacon[0].active, beacon[3].active, beacon[5].active, beacon[7].active);
			oapiWriteScenario_string (scn, "LIGHTS", cbuf);
			break;
		}

	double trim = GetControlSurfaceLevel (AIRCTRL_ELEVATORTRIM);
	if (trim) oapiWriteScenario_float (scn, "TRIM", trim);

	if (tankconfig)
		oapiWriteScenario_int (scn, "TANKCONFIG", tankconfig);

	// write out AAP settings
	aap->WriteScenario (scn);
}

// --------------------------------------------------------------
// Finalise vessel creation
// --------------------------------------------------------------
void DeltaGlider::clbkPostCreation ()
{
	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkPostCreation ();

	SetEmptyMass ();

	// update animation states
	SetAnimation (anim_gear, gear_proc);
	SetAnimation (anim_nose, nose_proc);
	SetAnimation (anim_ladder, ladder_proc);
	SetAnimation (anim_olock, olock_proc);
	SetAnimation (anim_ilock, ilock_proc);
	SetAnimation (anim_hatch, hatch_proc);
	SetAnimation (anim_radiator, radiator_proc);
	SetAnimation (anim_brake, brake_proc);
	SetAnimation (anim_gearlever, gear_status & 1);
	SetAnimation (anim_airbrakelever, airbrakelever_status & 1);
	SetAnimation (anim_noselever, nose_status & 1);
	SetAnimation (anim_radiatorswitch, radiator_status & 1);
	SetAnimation (anim_ladderswitch, ladder_status & 1);

	if (insignia_tex)
		PaintMarkings (insignia_tex);
}

// --------------------------------------------------------------
// Respond to playback event
// --------------------------------------------------------------
bool DeltaGlider::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!_stricmp (event_type, "GEAR")) {
		ActivateLandingGear (!_stricmp (event, "UP") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "NOSECONE")) {
		ActivateDockingPort (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "RCOVER")) {
		ssys_mainretro->ActivateRCover (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "RADIATOR")) {
		ActivateRadiator (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "AIRBRAKE")) {
		ActivateAirbrake(!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "HATCH")) {
		ActivateHatch (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "OLOCK")) {
		ActivateOuterAirlock (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "ILOCK")) {
		ActivateInnerAirlock (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	} else if (!_stricmp (event_type, "LADDER")) {
		ActivateLadder (!_stricmp (event, "CLOSE") ? DOOR_CLOSING : DOOR_OPENING);
		return true;
	}
	return false;
}

// --------------------------------------------------------------
// Create DG visual
// --------------------------------------------------------------
void DeltaGlider::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	visual = vis;
	exmesh = GetDevMesh (vis, 0);
	vcmesh = GetDevMesh (vis, 1);
	SetPassengerVisuals();
	SetDamageVisuals();

	if (vcmesh && !ScramVersion()) { // disable scram-specific components
		GROUPEDITSPEC ges;
		ges.flags = GRPEDIT_ADDUSERFLAG;
		ges.UsrFlag = 3;
		static int vcscramidx[11] = {
			GRP_SCRAMGIMBAL_L_VC,GRP_SCRAMGIMBAL_R_VC,
			GRP_SCRAM_GIMBAL_FRAME_VC,GRP_SCRAM_INDICATOR_LABEL_VC,GRP_SCRAM_STATUS_VC,
			GRP_SCRAM_TEMP_VC,GRP_SCRAM_GIMBAL_INDICATOR_VC,
			GRP_THROTTLE_SCRAM_L1_VC,GRP_THROTTLE_SCRAM_R1_VC,
			GRP_THROTTLE_SCRAM_L2_VC,GRP_THROTTLE_SCRAM_R2_VC};
		for (DWORD i = 0; i < 11; i++)
			oapiEditMeshGroup (vcmesh, vcscramidx[i], &ges);
	}

	ApplySkin();

	//if (oapiCameraInternal()) {
		UpdateStatusIndicators();
		//if (oapiCockpitMode() == COCKPIT_VIRTUAL)
			InitVCMesh();
	//}
}

// --------------------------------------------------------------
// Destroy DG visual
// --------------------------------------------------------------
void DeltaGlider::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	visual = NULL;
	exmesh = NULL;
	vcmesh = NULL;
}

// --------------------------------------------------------------
// Respond to MFD mode change
// --------------------------------------------------------------
void DeltaGlider::clbkMFDMode (int mfd, int mode)
{
	switch (mfd) {
	case MFD_LEFT:
		oapiTriggerRedrawArea (0, 0, AID_MFD1_LBUTTONS);
		oapiTriggerRedrawArea (0, 0, AID_MFD1_RBUTTONS);
		break;
	case MFD_RIGHT:
		oapiTriggerRedrawArea (0, 0, AID_MFD2_LBUTTONS);
		oapiTriggerRedrawArea (0, 0, AID_MFD2_RBUTTONS);
		break;
	}
}

// --------------------------------------------------------------
// Respond to RCS mode change
// --------------------------------------------------------------
void DeltaGlider::clbkRCSMode (int mode)
{
	ssys_rcs->SetMode (mode);
}

// --------------------------------------------------------------
// Respond to control surface mode change
// --------------------------------------------------------------
void DeltaGlider::clbkADCtrlMode (DWORD mode)
{
	oapiTriggerRedrawArea (0, 0, AID_ADCTRLMODE);
}

// --------------------------------------------------------------
// Respond to HUD mode change
// --------------------------------------------------------------
void DeltaGlider::clbkHUDMode (int mode)
{
	ssys_hud->SetHUDMode (mode);
}

// --------------------------------------------------------------
// Respond to navmode change
// --------------------------------------------------------------
void DeltaGlider::clbkNavMode (int mode, bool active)
{
	if (mode == NAVMODE_HOLDALT) {
		ssys_hoverctrl->ActivateHold (active);
	} else {
		((NavButtons*)instr[5])->SetMode (mode, active);
		oapiTriggerRedrawArea (0, 0, AID_NAVMODE);
	}
}

// --------------------------------------------------------------
// Respond to navmode processing request
// --------------------------------------------------------------
int DeltaGlider::clbkNavProcess (int mode)
{
	if (mode & NAVBIT_HOLDALT) {
		//mode ^= NAVBIT_HOLDALT;
		//ProcessHoverHoldalt();
	}
	return mode;
}

// --------------------------------------------------------------
// Frame update
// --------------------------------------------------------------
void DeltaGlider::clbkPostStep (double simt, double simdt, double mjd)
{
	// calculate max scramjet thrust
	if (scramjet) ScramjetThrust ();

	th_main_level = GetThrusterGroupLevel (THGROUP_MAIN);

	// animate landing gear
	if (gear_status >= DOOR_CLOSING) {
		double da = simdt * GEAR_OPERATING_SPEED;
		if (gear_status == DOOR_CLOSING) {
			if (gear_proc > 0.0)
				gear_proc = max (0.0, gear_proc-da);
			else {
				gear_status = DOOR_CLOSED;
				//oapiTriggerRedrawArea (2, 0, AID_GEARINDICATOR);
			}
		} else  { // door opening
			if (gear_proc < 1.0)
				gear_proc = min (1.0, gear_proc+da);
			else {
				gear_status = DOOR_OPEN;
				//oapiTriggerRedrawArea (2, 0, AID_GEARINDICATOR);
			}
		}
		SetAnimation (anim_gear, gear_proc);
		SetGearParameters (gear_proc);
		oapiTriggerRedrawArea (0, 0, AID_GEARINDICATOR);
		UpdateStatusIndicators();
	}
	if (gearlever_status >= DOOR_CLOSING) {
		double da = simdt * 4.0;
		if (gearlever_status == DOOR_CLOSING) {
			if (gearlever_proc > 0.0)
				gearlever_proc = max (0.0, gearlever_proc-da);
			else {
				gearlever_status = DOOR_CLOSED;
			}
		} else  { // door opening
			if (gearlever_proc < 1.0)
				gearlever_proc = min (1.0, gearlever_proc+da);
			else {
				gearlever_status = DOOR_OPEN;
			}
		}
		SetAnimation (anim_gearlever, gearlever_proc);
	}

	// animate nose cone
	if (nose_status >= DOOR_CLOSING) {
		double da = simdt * NOSE_OPERATING_SPEED;
		if (nose_status == DOOR_CLOSING) {
			if (nose_proc > 0.0)
				nose_proc = max (0.0, nose_proc-da);
			else
				nose_status = DOOR_CLOSED;
		} else { // door opening
			if (nose_proc < 1.0)
				nose_proc = min (1.0, nose_proc+da);
			else
				nose_status = DOOR_OPEN;
		}
		SetAnimation (anim_nose, nose_proc);
		oapiTriggerRedrawArea (0, 0, AID_NOSECONEINDICATOR);
		UpdateStatusIndicators();
	}
	if (noselever_status >= DOOR_CLOSING) {
		double da = simdt * 4.0;
		if (noselever_status == DOOR_CLOSING) {
			if (noselever_proc > 0.0)
				noselever_proc = max (0.0, noselever_proc-da);
			else
				noselever_status = DOOR_CLOSED;
		} else  { // door opening
			if (noselever_proc < 1.0)
				noselever_proc = min (1.0, noselever_proc+da);
			else
				noselever_status = DOOR_OPEN;
		}
		SetAnimation (anim_noselever, noselever_proc);
	}

	// animate undock lever
	if (undock_status >= DOOR_CLOSING) {
		if (undock_status == DOOR_CLOSING) {
			double da = simdt * 10.0;
			if (undock_proc > 0.0)
				undock_proc = max (0.0, undock_proc-da);
			else
				undock_status = DOOR_CLOSED;
		} else { // door opening
			double da = simdt * 5.0;
			if (undock_proc < 1.0)
				undock_proc = min (1.0, undock_proc+da);
			else
				undock_status = DOOR_OPEN;
		}
		SetAnimation (anim_undocklever, undock_proc);
	}

	// animate escape ladder
	if (ladder_status >= DOOR_CLOSING) {
		double da = simdt * LADDER_OPERATING_SPEED;
		if (ladder_status == DOOR_CLOSING) {
			if (ladder_proc > 0.0)
				ladder_proc = max (0.0, ladder_proc-da);
			else {
				ladder_status = DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (2, AID_NOSECONEINDICATOR);
			}
		} else {
			if (ladder_proc < 1.0)
				ladder_proc = min (1.0, ladder_proc+da);
			else {
				ladder_status = DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (2, AID_NOSECONEINDICATOR);
			}
		}
		SetAnimation (anim_ladder, ladder_proc);
	}

	// animate top hatch
	if (hatch_status >= DOOR_CLOSING) {
		double da = simdt * HATCH_OPERATING_SPEED;
		if (hatch_status == DOOR_CLOSING) {
			if (hatch_proc > 0.0)
				hatch_proc = max (0.0, hatch_proc-da);
			else {
				hatch_status = DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (2, AID_NOSECONEINDICATOR);
			}
		} else {
			if (hatch_proc < 1.0)
				hatch_proc = min (1.0, hatch_proc+da);
			else {
				hatch_status = DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (2, AID_NOSECONEINDICATOR);
			}
		}
		SetAnimation (anim_hatch, hatch_proc);
		UpdateStatusIndicators();
	}

	// animate outer airlock
	if (olock_status >= DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (olock_status == DOOR_CLOSING) {
			if (olock_proc > 0.0)
				olock_proc = max (0.0, olock_proc-da);
			else {
				olock_status = DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		} else { // door opening
			if (olock_proc < 1.0)
				olock_proc = min (1.0, olock_proc+da);
			else {
				olock_status = DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		}
		SetAnimation (anim_olock, olock_proc);
		UpdateStatusIndicators();
	}

	// animate inner airlock
	if (ilock_status >= DOOR_CLOSING) {
		double da = simdt * AIRLOCK_OPERATING_SPEED;
		if (ilock_status == DOOR_CLOSING) {
			if (ilock_proc > 0.0)
				ilock_proc = max (0.0, ilock_proc-da);
			else {
				ilock_status = DOOR_CLOSED;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		} else { // door opening
			if (ilock_proc < 1.0)
				ilock_proc = min (1.0, ilock_proc+da);
			else {
				ilock_status = DOOR_OPEN;
				//oapiTriggerPanelRedrawArea (1, AID_AIRLOCKINDICATOR);
			}
		}
		SetAnimation (anim_ilock, ilock_proc);
		UpdateStatusIndicators();
	}

	// animate radiator
	if (radiator_status >= DOOR_CLOSING) {
		double da = simdt * RADIATOR_OPERATING_SPEED;
		if (radiator_status == DOOR_CLOSING) { // retract radiator
			if (radiator_proc > 0.0) radiator_proc = max (0.0, radiator_proc-da);
			else                     radiator_status = DOOR_CLOSED;
		} else {                               // deploy radiator
			if (radiator_proc < 1.0) radiator_proc = min (1.0, radiator_proc+da);
			else                     radiator_status = DOOR_OPEN;
		}
		SetAnimation (anim_radiator, radiator_proc);
		UpdateStatusIndicators();
	}

	// animate airbrake
	if (brake_status >= DOOR_CLOSING) {
		double tgt, da = simdt * AIRBRAKE_OPERATING_SPEED;
		if (brake_status == DOOR_CLOSING) { // retract brake
			tgt = (airbrake_tgt == 1 ? 0.5:0.0);
			if (brake_proc > tgt) brake_proc = max (tgt, brake_proc-da);
			else                  brake_status = DOOR_CLOSED;
		} else {                            // deploy brake
			tgt = (airbrake_tgt == 1 ? 0.5:1.0);
			if (brake_proc < tgt) brake_proc = min (tgt, brake_proc+da);
			else                  brake_status = DOOR_OPEN;
		}
		SetAnimation (anim_brake, brake_proc);
		UpdateStatusIndicators();
	}
	if (airbrakelever_status >= DOOR_CLOSING) {
		double tgt, da = simdt * 4.0;
		if (airbrakelever_status == DOOR_CLOSING) {
			tgt = (airbrake_tgt == 1 ? 0.5:0.0);
			if (airbrakelever_proc > tgt)
				airbrakelever_proc = max (tgt, airbrakelever_proc-da);
			else {
				airbrakelever_status = DOOR_CLOSED;
			}
		} else  { // door opening
			tgt = (airbrake_tgt == 1 ? 0.5:1.0);
			if (airbrakelever_proc < tgt)
				airbrakelever_proc = min (tgt, airbrakelever_proc+da);
			else {
				airbrakelever_status = DOOR_OPEN;
			}
		}
		SetAnimation (anim_airbrakelever, airbrakelever_proc);
	}

	if (hatch_vent && simt > hatch_vent_t + 1.0) {
		DelExhaustStream (hatch_vent);
		hatch_vent = NULL;
	}

	// damage/failure system
	if (bDamageEnabled) TestDamage ();
	if (bMWSActive) {
		double di;
		bool mwson = (modf (simt, &di) < 0.5);
		if (mwson != bMWSOn) {
			bMWSOn = mwson;
			oapiTriggerRedrawArea (0, 0, AID_MWS);
		}
	}

	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkPostStep (simt, simdt, mjd);
}

bool DeltaGlider::clbkLoadGenericCockpit ()
{
	SetCameraOffset (_V(0,1.467,6.782));
	oapiSetDefNavDisplay (1);
	oapiSetDefRCSDisplay (1);
	campos = CAM_GENERIC;
	return true;
}

// --------------------------------------------------------------
// Load 2-D instrument panel mode
// --------------------------------------------------------------

bool DeltaGlider::clbkLoadPanel2D (int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	// set up subsystem panel elements
	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkLoadPanel2D (id, hPanel, viewW, viewH);

	switch (id) {
	case 0:
		DefinePanelMain (hPanel);
		SetPanelScale (hPanel, viewW, viewH);
		oapiSetPanelNeighbours (-1,-1,1,-1);
		SetCameraDefaultDirection (_V(0,0,1)); // forward
		oapiCameraSetCockpitDir (0,0);         // look forward
		return true;
	case 1:
		DefinePanelOverhead (hPanel);
		SetPanelScale (hPanel, viewW, viewH);
		oapiSetPanelNeighbours (-1,-1,-1,0);
		SetCameraDefaultDirection (_V(0,0,1)); // forward
		oapiCameraSetCockpitDir (0,20*RAD);    // look up
		return true;
	default:
		return false;
	}
}

void DeltaGlider::SetPanelScale (PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	double defscale = (double)viewW/PANEL2D_WIDTH;
	double extscale = max (defscale, 1.0);
	SetPanelScaling (hPanel, defscale, extscale);
}

void DeltaGlider::DefinePanelMain (PANELHANDLE hPanel)
{
	ReleaseSurfaces();
	hPanelMesh = panelmesh0;
	SURFHANDLE panel2dtex = oapiGetTextureHandle(hPanelMesh,1);
	SURFHANDLE instr2dtex = oapiGetTextureHandle(hPanelMesh,2);

	const DWORD panelw = PANEL2D_WIDTH, panelh = 572;
	int xofs;

	SetPanelBackground (hPanel, 0, 0, hPanelMesh, panelw, panelh, 190,
		PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM);

	// Define MFD layout (display and buttons)
	RegisterPanelMFDGeometry (hPanel, MFD_LEFT, 0, GRP_LMFD_DISPLAY_P0);
	RegisterPanelMFDGeometry (hPanel, MFD_RIGHT, 0, GRP_RMFD_DISPLAY_P0);

	xofs = 173; // left MFD
	RegisterPanelArea (hPanel, AID_MFD1_BBUTTONS, _R( 51+xofs,359,321+xofs,377), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[28]); // bottom button row
	RegisterPanelArea (hPanel, AID_MFD1_LBUTTONS, _R(    xofs,100, 25+xofs,323), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[29]); // left button column
	RegisterPanelArea (hPanel, AID_MFD1_RBUTTONS, _R(348+xofs,100,373+xofs,323), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[30]); // right button column

	xofs = 736; // right MFD
	RegisterPanelArea (hPanel, AID_MFD2_BBUTTONS, _R( 51+xofs,359,321+xofs,377), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[31]); // bottom button row
	RegisterPanelArea (hPanel, AID_MFD2_LBUTTONS, _R(    xofs,100, 25+xofs,323), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[32]); // left button column
	RegisterPanelArea (hPanel, AID_MFD2_RBUTTONS, _R(348+xofs,100,373+xofs,323), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY, panel2dtex, instr[33]); // right button column

	RegisterPanelArea (hPanel, AID_HORIZON,      _R(0,0,0,0),           PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, 0, instr[0]);
	RegisterPanelArea (hPanel, AID_HSIINSTR,     _R(0,0,0,0),           PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, 0, instr[1]);
	RegisterPanelArea (hPanel, AID_AOAINSTR,     _R(0,0,0,0),           PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[2]);
	RegisterPanelArea (hPanel, AID_VSINSTR,      _R(0,0,0,0),           PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[3]);
	RegisterPanelArea (hPanel, AID_MAINPROP,     _R(0,0,0,0),           PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, instr2dtex, instr[4]);
	RegisterPanelArea (hPanel, AID_NAVMODE,      _R(1121,119,1197,273), PANEL_REDRAW_USER,   PANEL_MOUSE_LBDOWN, 0, instr[5]);
	RegisterPanelArea (hPanel, AID_ELEVATORTRIM, _R(1242,135,1262,195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED, panel2dtex, instr[6]);
	RegisterPanelArea (hPanel, AID_AIRBRAKE,     _R(1242,215,1262,275), PANEL_REDRAW_USER,   PANEL_MOUSE_LBDOWN, panel2dtex, instr[7]);
	RegisterPanelArea (hPanel, AID_ADCTRLMODE,   _R(1217, 69,1257,113), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN, panel2dtex, instr[11]);
	RegisterPanelArea (hPanel, AID_DOCKRELEASE,  _R(1141,474,1172,504), PANEL_REDRAW_MOUSE,  PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP, panel2dtex, instr[12]);
	RegisterPanelArea (hPanel, AID_GEARLEVER,    _R(1230,286,1262,511), PANEL_REDRAW_USER,   PANEL_MOUSE_LBDOWN, panel2dtex, instr[14]);
	RegisterPanelArea (hPanel, AID_GEARINDICATOR, _R(0,0,0,0),          PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, instr[37]);
	RegisterPanelArea (hPanel, AID_NOSECONELEVER, _R(1141,327,1180,421), PANEL_REDRAW_USER,  PANEL_MOUSE_LBDOWN, panel2dtex, instr[23]);
	RegisterPanelArea (hPanel, AID_NOSECONEINDICATOR, _R(0,0,0,0),      PANEL_REDRAW_USER,   PANEL_MOUSE_IGNORE, panel2dtex, instr[24]);
	RegisterPanelArea (hPanel, AID_WBRAKE_BOTH,  _R(1221,494,1273,557), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP, panel2dtex, instr[26]);
	RegisterPanelArea (hPanel, AID_MWS,          _R(1071,  6,1098, 32), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN, panel2dtex, instr[27]);

	if (ScramVersion()) {
		RegisterPanelArea (hPanel, AID_ENGINESCRAM, _R(108,386,161,488), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED, panel2dtex, instr[instr_scram0+0]);
	} else {
		MESHGROUP *grp = oapiMeshGroup (hPanelMesh, GRP_SCRAM_INSTRUMENTS_P0);
		grp->UsrFlag = 3;
	}

	aap->RegisterPanel (hPanel);
	InitPanel (0);
}

void DeltaGlider::DefinePanelOverhead (PANELHANDLE hPanel)
{
	DWORD i;

	MESHGROUP grp;
	memset (&grp, 0, sizeof(MESHGROUP));
	ReleaseSurfaces();
	SURFHANDLE panel2dtex = oapiGetTextureHandle(panelmesh0,1);

	const DWORD NVTX = 8, NIDX = 12;
	const DWORD texw = PANEL2D_TEXW, texh = PANEL2D_TEXH;
	const DWORD panelw = PANEL2D_WIDTH, panelh = 283, panelh2 = 136, panely0 = PANEL2D_TEXH-855;

	// panel billboard definition
	static NTVERTEX VTX[NVTX] = {
		{         0,      0,0,  0,0,0,  0,                               (float)panely0/(float)texh},
		{    panelw,      0,0,  0,0,0,  (float)panelw/(float)texw,       (float)panely0/(float)texh},
		{         0,panelh2,0,  0,0,0,  0,                               (float)(panely0+panelh2)/(float)texh},
		{    panelw,panelh2,0,  0,0,0,  (float)panelw/(float)texw,       (float)(panely0+panelh2)/(float)texh},
		{       255,panelh2,0,  0,0,0,  255.0f/(float)texw,              (float)(panely0+panelh2)/(float)texh},
		{panelw-255,panelh2,0,  0,0,0,  (float)(panelw-255)/(float)texw, (float)(panely0+panelh2)/(float)texh},
		{       425, panelh,0,  0,0,0,  425.0f/(float)texw,              (float)(panely0+panelh)/(float)texh},
		{panelw-425, panelh,0,  0,0,0,  (float)(panelw-425)/(float)texw, (float)(panely0+panelh)/(float)texh}
	};
	static WORD IDX[NIDX] = {
		0,1,2, 3,2,1, 4,5,6, 7,6,5
	};

	// Create the mesh for defining the panel geometry
	if (!panelmesh1) {
		panelmesh1 = oapiCreateMesh (0, 0);
		for (i = 0; i < 2; i++)
			oapiAddMeshGroup (panelmesh1, &grp);

		// Define overhead panel background
		oapiAddMeshGroupBlock (panelmesh1, 0, VTX, NVTX, IDX, NIDX);

		// Define panel elements on top of background
		for (i = 0; i < ninstr_ovhd; i++)
			instr[instr_ovhd0+i]->AddMeshData2D (panelmesh1, 1);
	}
	hPanelMesh = panelmesh1;
	SetPanelBackground (hPanel, &panel2dtex, 1, hPanelMesh, panelw, panelh, 0,
		PANEL_ATTACH_TOP | PANEL_MOVEOUT_TOP);

	RegisterPanelArea (hPanel, AID_AIRLOCKSWITCH, _R(240,30, 390,68), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN, panel2dtex, instr[instr_ovhd0+0]);
	RegisterPanelArea (hPanel, AID_SWITCHARRAY,   _R(797,38,1048,76), PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN, panel2dtex, instr[25]);
	RegisterPanelArea (hPanel, AID_VPITCH, _R(625,191,665,240), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+1]);
	RegisterPanelArea (hPanel, AID_VBANK,  _R(558,191,608,231), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+2]);
	RegisterPanelArea (hPanel, AID_VYAW,   _R(492,191,542,231), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+3]);
	RegisterPanelArea (hPanel, AID_APITCH, _R(625,264,665,313), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+4]);
	RegisterPanelArea (hPanel, AID_ABANK,  _R(558,264,608,304), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+5]);
	RegisterPanelArea (hPanel, AID_AYAW,   _R(492,264,542,304), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+6]);
	RegisterPanelArea (hPanel, AID_MPITCH, _R(625,337,665,386), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+7]);
	RegisterPanelArea (hPanel, AID_MBANK,  _R(558,337,608,377), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+8]);
	RegisterPanelArea (hPanel, AID_MYAW,   _R(492,337,542,377), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, panel2dtex, instr[instr_ovhd0+9]);
	InitPanel (1);
}

// --------------------------------------------------------------
// Respond to panel mouse event
// --------------------------------------------------------------
bool DeltaGlider::clbkPanelMouseEvent (int id, int event, int mx, int my, void *context)
{
	if (context) {
		PanelElement *pe = (PanelElement*)context;
		return pe->ProcessMouse2D (event, mx, my);
	}
	return false;
}

// --------------------------------------------------------------
// Respond to panel redraw event
// --------------------------------------------------------------
bool DeltaGlider::clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf, void *context)
{
	if (context) {
		PanelElement *pe = (PanelElement*)context;
		return pe->Redraw2D (surf);
	}

	return false;
}

// --------------------------------------------------------------
// Load virtual cockpit mode
// --------------------------------------------------------------
bool DeltaGlider::clbkLoadVC (int id)
{
	static VCMFDSPEC mfds_left  = {1, GRP_LMFD_DISPLAY_VC};
	static VCMFDSPEC mfds_right = {1, GRP_RMFD_DISPLAY_VC};
	static VCHUDSPEC huds = {1, GRP_HUDDISP_VC, {0,1.462,7.09}, 0.15};
	SURFHANDLE tex1 = oapiGetTextureHandle (vcmesh_tpl, 16);
	SURFHANDLE tex2 = oapiGetTextureHandle (vcmesh_tpl, 18);
	SURFHANDLE tex3 = oapiGetTextureHandle (vcmesh_tpl, 14);
	intex = oapiGetTextureHandle (vcmesh_tpl, 19);
	vctex = oapiGetTextureHandle (vcmesh_tpl, 20);

	ReleaseSurfaces();
	InitVC (id);

	SetCameraDefaultDirection (_V(0,0,1)); // forward
	oapiVCRegisterHUD (&huds); // HUD parameters
	oapiVCRegisterMFD (MFD_LEFT, &mfds_left);   // left MFD
	oapiVCRegisterMFD (MFD_RIGHT, &mfds_right); // right MFD

	switch (id) {
	case 0: // pilot
		SetCameraOffset (_V(0,1.467,6.782));
		SetCameraShiftRange (_V(0,0,0.1), _V(-0.2,0,0), _V(0.2,0,0));
		oapiVCSetNeighbours (1, 2, -1, -1);

		// left MFD controls on the front panel
		oapiVCRegisterArea (AID_MFD1_BBUTTONS, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD1_BBUTTONS, _V(-0.1844, 1.0745, 7.2238), _V(-0.1456, 1.0745, 7.2238), _V(-0.1844, 1.0587, 7.2180), _V(-0.1456, 1.0587, 7.2180));
		oapiVCRegisterArea (AID_MFD1_LBUTTONS, PANEL_REDRAW_MOUSE|PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD1_LBUTTONS, _V(-0.2684, 1.2155, 7.2751), _V(-0.2516, 1.2155, 7.2751), _V(-0.2684, 1.0963, 7.2317), _V(-0.2516, 1.0963, 7.2317));
		oapiVCRegisterArea (AID_MFD1_RBUTTONS, PANEL_REDRAW_MOUSE|PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD1_RBUTTONS, _V(-0.0784, 1.2155, 7.2751), _V(-0.0616, 1.2155, 7.2751), _V(-0.0784, 1.0963, 7.2317), _V(-0.0616, 1.0963, 7.2317));

		// right MFD controls on the front panel
		oapiVCRegisterArea (AID_MFD2_BBUTTONS, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD2_BBUTTONS, _V(0.1456, 1.0745, 7.2238), _V(0.1844, 1.0745, 7.2238), _V(0.1456, 1.0587, 7.2180), _V(0.1844, 1.0587, 7.2180));
		oapiVCRegisterArea (AID_MFD2_LBUTTONS, PANEL_REDRAW_MOUSE|PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD2_LBUTTONS, _V(0.0616, 1.2155, 7.2751), _V(0.0784, 1.2155, 7.2751), _V(0.0616, 1.0963, 7.2317), _V(0.0784, 1.0963, 7.2317));
		oapiVCRegisterArea (AID_MFD2_RBUTTONS, PANEL_REDRAW_MOUSE|PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_ONREPLAY);
		oapiVCSetAreaClickmode_Quadrilateral (AID_MFD2_RBUTTONS, _V(0.2516, 1.2155, 7.2751), _V(0.2684, 1.2155, 7.2751), _V(0.2516, 1.0963, 7.2317), _V(0.2684, 1.0963, 7.2317));

		//oapiVCRegisterArea (AID_MFD1_PWR, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_ONREPLAY);
		//oapiVCSetAreaClickmode_Spherical (AID_MFD1_PWR, _V(-0.1914,1.009,7.2775), 0.01);

		//oapiVCRegisterArea (AID_MFD2_PWR, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_ONREPLAY);
		//oapiVCSetAreaClickmode_Spherical (AID_MFD2_PWR, _V(0.0483,1.009,7.2775), 0.01);

		// artificial horizon
		oapiVCRegisterArea (AID_HORIZON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// HSI indicator
		oapiVCRegisterArea (AID_HSIINSTR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// Propellant status display
		oapiVCRegisterArea (AID_MAINPROP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// main/retro/hover engine indicators
		oapiVCRegisterArea (AID_MAINDISP1, _R( 50,16, 63,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		oapiVCRegisterArea (AID_MAINDISP2, _R( 85,16, 98,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		oapiVCRegisterArea (AID_MAINDISP3, _R(120,16,133,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		oapiVCRegisterArea (AID_MAINDISP4, _R( 23,16, 29,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		//oapiVCRegisterArea (AID_MAINPROP, _R(122,102,135,197), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		//oapiVCRegisterArea (AID_MAINPROPMASS, _R(110, 199, 140, 208), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_NONE, tex1);
		oapiVCRegisterArea (AID_RCSPROP, _R(162,102,175,197), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
		oapiVCRegisterArea (AID_RCSPROPMASS, _R(156, 199, 174, 208), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_NONE, tex1);

		// AOA/slip/slope+range indicators
		oapiVCRegisterArea (AID_AOAINSTR,   _R( 17,181, 73,237), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex2);
		oapiVCRegisterArea (AID_SLIPINSTR,  _R(109,181,165,237), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex2);
		oapiVCRegisterArea (AID_LOADINSTR,  _R(111, 17,167, 73), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex2);

		// angular velocity/acceleration/torque indicators
		oapiVCRegisterArea (AID_ANGRATEINDICATOR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

		// scram engine indicators
		if (ScramVersion()) {
			oapiVCRegisterArea (AID_SCRAMDISP2, _R(195,16,208,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
			oapiVCRegisterArea (AID_SCRAMDISP3, _R(158,16,171,89), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
			oapiVCRegisterArea (AID_SCRAMTEMPDISP, _R(6,10,87,140), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, tex2);
			oapiVCRegisterArea (AID_SCRAMPROP, _R(200,102,213,197), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BGONREQUEST, tex1);
			oapiVCRegisterArea (AID_SCRAMPROPMASS, _R(188, 199, 218, 208), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_NONE, tex1);
			oapiVCRegisterArea (AID_ENGINESCRAM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED);
			oapiVCSetAreaClickmode_Quadrilateral (AID_ENGINESCRAM, _V(-0.45,0.98,6.94), _V(-0.39,0.98,6.94), _V(-0.45,0.95,7.07), _V(-0.39,0.95,7.07));
		}

		// Landing/docking light switch
		oapiVCRegisterArea (AID_LANDDOCKLIGHT, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_LANDDOCKLIGHT, VC_LANDINGLIGHT_SWITCH_mousearea[0], VC_LANDINGLIGHT_SWITCH_mousearea[1], VC_LANDINGLIGHT_SWITCH_mousearea[2], VC_LANDINGLIGHT_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[44])->DefineAnimationVC (VC_LANDINGLIGHT_SWITCH_ref, VC_LANDINGLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_LANDINGLIGHT_SWITCH_vofs);

		// Strobe light switch
		oapiVCRegisterArea (AID_STROBE_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_STROBE_SWITCH, VC_STROBELIGHT_SWITCH_mousearea[0], VC_STROBELIGHT_SWITCH_mousearea[1], VC_STROBELIGHT_SWITCH_mousearea[2], VC_STROBELIGHT_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[45])->DefineAnimationVC (VC_STROBELIGHT_SWITCH_ref, VC_STROBELIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_STROBELIGHT_SWITCH_vofs);

		// Nav light switch
		oapiVCRegisterArea (AID_NAVLIGHT_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_NAVLIGHT_SWITCH, VC_NAVLIGHT_SWITCH_mousearea[0], VC_NAVLIGHT_SWITCH_mousearea[1], VC_NAVLIGHT_SWITCH_mousearea[2], VC_NAVLIGHT_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[46])->DefineAnimationVC (VC_NAVLIGHT_SWITCH_ref, VC_NAVLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_NAVLIGHT_SWITCH_vofs);

		// Hatch open/close switch
		oapiVCRegisterArea (AID_HATCH_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_HATCH_SWITCH, VC_HATCH_SWITCH_mousearea[0], VC_HATCH_SWITCH_mousearea[1], VC_HATCH_SWITCH_mousearea[2], VC_HATCH_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[47])->DefineAnimationVC (VC_HATCH_SWITCH_ref, VC_HATCH_SWITCH_axis, GRP_SWITCH1_VC, VC_HATCH_SWITCH_vofs);

		// Inner airlock open/close switch
		oapiVCRegisterArea (AID_ILOCK_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_ILOCK_SWITCH, VC_ILOCK_SWITCH_mousearea[0], VC_ILOCK_SWITCH_mousearea[1], VC_ILOCK_SWITCH_mousearea[2], VC_ILOCK_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[48])->DefineAnimationVC (VC_ILOCK_SWITCH_ref, VC_ILOCK_SWITCH_axis, GRP_SWITCH1_VC, VC_ILOCK_SWITCH_vofs);

		// Outer airlock open/close switch
		oapiVCRegisterArea (AID_OLOCK_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_OLOCK_SWITCH, VC_OLOCK_SWITCH_mousearea[0], VC_OLOCK_SWITCH_mousearea[1], VC_OLOCK_SWITCH_mousearea[2], VC_OLOCK_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[49])->DefineAnimationVC (VC_OLOCK_SWITCH_ref, VC_OLOCK_SWITCH_axis, GRP_SWITCH1_VC, VC_OLOCK_SWITCH_vofs);

		//oapiVCRegisterArea (AID_MWS, PANEL_REDRAW_USER | PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		//oapiVCSetAreaClickmode_Spherical (AID_MWS, _V(0.0755,1.2185,7.3576), 0.013);

		// Navmode indicator/selector on the top right of the front panel
		oapiVCRegisterArea (AID_NAVMODE, PANEL_REDRAW_USER | PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (AID_NAVMODE, VC_NAV_BUTTONS_mousearea[0], VC_NAV_BUTTONS_mousearea[1], VC_NAV_BUTTONS_mousearea[2], VC_NAV_BUTTONS_mousearea[3]);
		{
			static DWORD navbtn_vofs[6] = {VC_BTN_NAVMODE_1_vofs, VC_BTN_NAVMODE_2_vofs, VC_BTN_NAVMODE_3_vofs,
				                           VC_BTN_NAVMODE_4_vofs, VC_BTN_NAVMODE_5_vofs, VC_BTN_NAVMODE_6_vofs}; 
			static DWORD navlbl_vofs[6] = {VC_BTN_NAVMODE_1_LABEL_vofs, VC_BTN_NAVMODE_2_LABEL_vofs, VC_BTN_NAVMODE_3_LABEL_vofs,
				                           VC_BTN_NAVMODE_4_LABEL_vofs, VC_BTN_NAVMODE_5_LABEL_vofs, VC_BTN_NAVMODE_6_LABEL_vofs};
			((NavButtons*)instr[5])->DefineAnimationsVC (VC_BTN_NAVMODE_1_axis, GRP_BUTTON3_VC, GRP_LIT_SURF_VC, navbtn_vofs, navlbl_vofs);
		}

		// Button row 1
		oapiVCRegisterArea (AID_BUTTONROW1, PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_BUTTONROW1, _V(0.3014,0.8519,6.9562), _V(0.3679,0.8519,6.9562), _V(0.3014,0.8520,7.0163), _V(0.3679,0.8520,7.0163));

		// Instrument light switch
		oapiVCRegisterArea (AID_INSTRLIGHT_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_INSTRLIGHT_SWITCH, VC_INSTRLIGHT_SWITCH_mousearea[0], VC_INSTRLIGHT_SWITCH_mousearea[1], VC_INSTRLIGHT_SWITCH_mousearea[2], VC_INSTRLIGHT_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[38])->DefineAnimationVC (VC_INSTRLIGHT_SWITCH_ref, VC_INSTRLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_INSTRLIGHT_SWITCH_vofs);

		// Floodlight switch
		oapiVCRegisterArea (AID_FLOODLIGHT_SWITCH, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_FLOODLIGHT_SWITCH, VC_FLOODLIGHT_SWITCH_mousearea[0], VC_FLOODLIGHT_SWITCH_mousearea[1], VC_FLOODLIGHT_SWITCH_mousearea[2], VC_FLOODLIGHT_SWITCH_mousearea[3]);
		((DGSwitch1*)instr[40])->DefineAnimationVC (VC_FLOODLIGHT_SWITCH_ref, VC_FLOODLIGHT_SWITCH_axis, GRP_SWITCH1_VC, VC_FLOODLIGHT_SWITCH_vofs);

		// Instrument brightness dial
		oapiVCRegisterArea (AID_INSTRBRIGHT_DIAL, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (AID_INSTRBRIGHT_DIAL, VC_INSTR_BRIGHTNESS_mousearea[0], VC_INSTR_BRIGHTNESS_mousearea[1], VC_INSTR_BRIGHTNESS_mousearea[2], VC_INSTR_BRIGHTNESS_mousearea[3]);

		// Floodlight brightness dial
		oapiVCRegisterArea (AID_FLOODBRIGHT_DIAL, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (AID_FLOODBRIGHT_DIAL, VC_FLOOD_BRIGHTNESS_mousearea[0], VC_FLOOD_BRIGHTNESS_mousearea[1], VC_FLOOD_BRIGHTNESS_mousearea[2], VC_FLOOD_BRIGHTNESS_mousearea[3]);

		// AF control dial
		oapiVCRegisterArea (AID_ADCTRLMODE, PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_ADCTRLMODE, VC_AF_DIAL_mousearea[0], VC_AF_DIAL_mousearea[1], VC_AF_DIAL_mousearea[2], VC_AF_DIAL_mousearea[3]);
		((DGDial1*)instr[11])->DefineAnimationVC (VC_AF_DIAL_ref, VC_AF_DIAL_axis, GRP_DIAL1_VC, VC_AF_DIAL_vofs);

		// Elevator trim wheel
		oapiVCRegisterArea (AID_ELEVATORTRIM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED);
		oapiVCSetAreaClickmode_Quadrilateral (AID_ELEVATORTRIM, vc_etrimwheel_mousearea[0], vc_etrimwheel_mousearea[1], vc_etrimwheel_mousearea[2], vc_etrimwheel_mousearea[3]);

		// Gear lever
		oapiVCRegisterArea (AID_GEARLEVER, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_GEARLEVER, vc_gearlever_mousearea[0], vc_gearlever_mousearea[1], vc_gearlever_mousearea[2], vc_gearlever_mousearea[3]);

		// Airbrake lever
		oapiVCRegisterArea (AID_AIRBRAKE, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_AIRBRAKE, vc_abrakelever_mousearea[0], vc_abrakelever_mousearea[1], vc_abrakelever_mousearea[2], vc_abrakelever_mousearea[3]);

		// Nosecone lever
		oapiVCRegisterArea (AID_NOSECONELEVER, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Quadrilateral (AID_NOSECONELEVER, vc_nconelever_mousearea[0], vc_nconelever_mousearea[1], vc_nconelever_mousearea[2], vc_nconelever_mousearea[3]);

		// Undock lever
		oapiVCRegisterArea (AID_DOCKRELEASE, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBUP);
		oapiVCSetAreaClickmode_Quadrilateral (AID_DOCKRELEASE, vc_undocklever_mousearea[0], vc_undocklever_mousearea[1], vc_undocklever_mousearea[2], vc_undocklever_mousearea[3]);

		oapiVCRegisterArea (AID_RADIATOREX, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical (AID_RADIATOREX, _V(0.2582,0.9448,7.22),0.01);
		oapiVCRegisterArea (AID_RADIATORIN, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical (AID_RADIATORIN, _V(0.2582,0.9618,7.22),0.01);

		oapiVCRegisterArea (AID_LADDEREX, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical (AID_LADDEREX, _V(0.2889,1.0537,7.2388), 0.01);
		oapiVCRegisterArea (AID_LADDERIN, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical (AID_LADDERIN, _V(0.2889,1.0707,7.2388), 0.01);

		oapiVCRegisterArea (AID_GEARINDICATOR, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
		oapiVCRegisterArea (AID_NOSECONEINDICATOR, _R(32,127,61,158), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, tex1);

		campos = CAM_VCPILOT;
		break;

	case 1: // front left passenger
		SetCameraOffset (_V(-0.7, 1.15, 5.55));
		SetCameraMovement (_V(0.2,-0.05,0.3), -10*RAD, 10*RAD, _V(-0.3,0,0), 80*RAD, 0, _V(0.4,0,0), -90*RAD, 0);
		oapiVCSetNeighbours (-1, 2, 0, 3);
		campos = CAM_VCPSNGR1;
		break;

	case 2: // front right passenger
		SetCameraOffset (_V(0.7, 1.15, 5.55));
		SetCameraMovement (_V(-0.2,-0.05,0.3), 10*RAD, 10*RAD, _V(-0.4,0,0), 90*RAD, 0, _V(0.3,0,0), -80*RAD, 0);
		oapiVCSetNeighbours (1, -1, 0, 4);
		campos = CAM_VCPSNGR2;
		break;

	case 3: // rear left passenger
		SetCameraOffset (_V(-0.8, 1.2, 4.4));
		SetCameraMovement (_V(0.4,0,0), 0, 0, _V(-0.3,0,0), 70*RAD, 0, _V(0.4,0,0), -90*RAD, 0);
		oapiVCSetNeighbours (-1, 4, 1, -1);
		campos = CAM_VCPSNGR3;
		break;

	case 4: // rear right passenger
		SetCameraOffset (_V(0.8, 1.2, 4.4));
		SetCameraMovement (_V(-0.4,0,0), 0, 0, _V(-0.4,0,0), 90*RAD, 0, _V(0.3,0,0), -70*RAD, 0);
		oapiVCSetNeighbours (3, -1, 2, -1);
		campos = CAM_VCPSNGR4;
		return true;

	default:
		return false;
	}

	for (std::vector<DGSubsystem*>::iterator it = ssys.begin(); it != ssys.end(); ++it)
		(*it)->clbkLoadVC (id);

	InitVCMesh();

	return true;
}

// --------------------------------------------------------------
// Respond to virtual cockpit mouse event
// --------------------------------------------------------------
bool DeltaGlider::clbkVCMouseEvent (int id, int event, VECTOR3 &p)
{
	static int ctrl = 0, mode = 0;
	static double py = 0;

	// distribute to subsystem
	int subsys, localid;
	localid = Subsystem::LocalElId (id, subsys);
	if (subsys >= 0) {
		if (subsys >= ssys.size()) return false; // subsystem id out of range
		return ssys[subsys]->clbkVCMouseEvent (localid, event, p);
	}

	// standalone id
	switch (id) {
	case AID_MFD1_LBUTTONS:
	case AID_MFD1_RBUTTONS:
	case AID_MFD1_BBUTTONS:
	case AID_MFD2_LBUTTONS:
	case AID_MFD2_RBUTTONS:
	case AID_MFD2_BBUTTONS:
		return instr[13+id]->ProcessMouseVC (event, p);
	case AID_MFD1_PWR:
		oapiToggleMFD_on (MFD_LEFT);
		return true;
	case AID_MFD2_PWR:
		oapiToggleMFD_on (MFD_RIGHT);
		return true;
	case AID_BUTTONROW1:
		return instr[25]->ProcessMouseVC (event, p);
	case AID_INSTRLIGHT_SWITCH:
		return instr[38]->ProcessMouseVC (event, p);
	case AID_FLOODLIGHT_SWITCH:
		return instr[40]->ProcessMouseVC (event, p);
	case AID_INSTRBRIGHT_DIAL:
		return instr[39]->ProcessMouseVC (event, p);
	case AID_FLOODBRIGHT_DIAL:
		return instr[41]->ProcessMouseVC (event, p);
	case AID_ENGINESCRAM:
		if (event & PANEL_MOUSE_LBDOWN) { // record which slider to operate
			if      (p.x < 0.3) ctrl = 0; // left engine
			else if (p.x > 0.7) ctrl = 1; // right engine
			else                ctrl = 2; // both
			py = p.y;
		} else {
			for (int i = 0; i < 2; i++) {
				if (ctrl == i || ctrl == 2) {
					double lvl = max (0.0, min (1.0, GetThrusterLevel (th_scram[i]) + (p.y-py)));
					if (lvl < 0.01) lvl = 0.0;
					SetThrusterLevel (th_scram[i], lvl);
				}
			}
			py = p.y;
		}
		return true;
	case AID_LANDDOCKLIGHT:
		return instr[44]->ProcessMouseVC (event, p);
	case AID_STROBE_SWITCH:
		return instr[45]->ProcessMouseVC (event, p);
	case AID_NAVLIGHT_SWITCH:
		return instr[46]->ProcessMouseVC (event, p);
	case AID_HATCH_SWITCH:
		return instr[47]->ProcessMouseVC (event, p);
	case AID_ILOCK_SWITCH:
		return instr[48]->ProcessMouseVC (event, p);
	case AID_OLOCK_SWITCH:
		return instr[49]->ProcessMouseVC (event, p);
	case AID_NAVMODE:
		return instr[5]->ProcessMouseVC (event, p);
	case AID_ADCTRLMODE:
		return instr[11]->ProcessMouseVC (event, p);
	case AID_ELEVATORTRIM:
		return instr[6]->ProcessMouseVC (event, p);
	case AID_GEARLEVER:
		return instr[14]->ProcessMouseVC (event, p);
	case AID_AIRBRAKE:
		return instr[7]->ProcessMouseVC (event, p);
	case AID_NOSECONELEVER:
		return instr[23]->ProcessMouseVC (event, p);
	case AID_DOCKRELEASE:
		return instr[12]->ProcessMouseVC (event, p);
	case AID_RADIATOREX:
		ActivateRadiator (DOOR_OPENING);
		return true;
	case AID_RADIATORIN:
		ActivateRadiator (DOOR_CLOSING);
		return true;
	case AID_LADDEREX:
		ActivateLadder (DOOR_OPENING);
		return true;
	case AID_LADDERIN:
		ActivateLadder (DOOR_CLOSING);
		return true;
	case AID_MWS:
		bMWSActive = bMWSOn = false;
		return true;
	}

	return false;
}

// --------------------------------------------------------------
// Respond to virtual cockpit area redraw request
// --------------------------------------------------------------
bool DeltaGlider::clbkVCRedrawEvent (int id, int event, SURFHANDLE surf)
{
	if (!vcmesh) return false;

	// distribute to subsystem
	int subsys, localid;
	localid = Subsystem::LocalElId (id, subsys);
	if (subsys >= 0) {
		if (subsys >= ssys.size()) return false; // subsystem id out of range
		return ssys[subsys]->clbkVCRedrawEvent (localid, event, vcmesh, surf);
	}

	// standalone id
	switch (id) {
	case AID_MFD1_LBUTTONS:
	case AID_MFD1_RBUTTONS:
	case AID_MFD1_BBUTTONS:
	case AID_MFD2_LBUTTONS:
	case AID_MFD2_RBUTTONS:
	case AID_MFD2_BBUTTONS:
		return (vcmesh ? instr[13+id]->RedrawVC(vcmesh, vctex) : false);
	case AID_ENGINESCRAM:
		RedrawVC_ThScram();
		return false;
	case AID_ADCTRLMODE:
		return instr[11]->RedrawVC (vcmesh, 0);
	case AID_MAINPROP:
		return instr[4]->RedrawVC (vcmesh, intex);
	case AID_RCSPROP:
		return RedrawPanel_RCSProp (surf);
	case AID_RCSPROPMASS:
		return RedrawPanel_RCSPropMass (surf);
	case AID_SCRAMPROP:
		return RedrawPanel_ScramProp (surf);
	case AID_SCRAMPROPMASS:
		return RedrawPanel_ScramPropMass (surf);
	case AID_MAINDISP1:
		return RedrawPanel_MainFlow (surf);
	case AID_MAINDISP2:
		return RedrawPanel_RetroFlow (surf);
	case AID_MAINDISP3:
		return RedrawPanel_HoverFlow (surf);
	case AID_MAINDISP4:
		return RedrawPanel_MainTSFC (surf);
	//case AID_AOAINSTR:
	//	return RedrawPanel_AOA (surf, event == PANEL_REDRAW_INIT);
	case AID_SLIPINSTR:
		return RedrawPanel_Slip (surf, event == PANEL_REDRAW_INIT);
	case AID_LOADINSTR:
		return RedrawPanel_Wingload (surf, event == PANEL_REDRAW_INIT);
	case AID_SCRAMDISP2:
		return RedrawPanel_ScramFlow (surf);
	case AID_SCRAMDISP3:
		return RedrawPanel_ScramTSFC (surf);
	case AID_SCRAMTEMPDISP:
		return RedrawPanel_ScramTempDisp (surf);
	case AID_ELEVATORTRIM:
		return instr[6]->RedrawVC (vcmesh, surf);
	case AID_HORIZON:
		return (vcmesh ? instr[0]->RedrawVC (vcmesh, surf) : false);
	case AID_HSIINSTR:
		return instr[1]->RedrawVC (vcmesh, surf);
	case AID_LANDDOCKLIGHT:
		return (vcmesh ? instr[44]->RedrawVC (vcmesh, surf) : false);
	case AID_STROBE_SWITCH:
		return (vcmesh ? instr[45]->RedrawVC (vcmesh, surf) : false);
	case AID_NAVLIGHT_SWITCH:
		return (vcmesh ? instr[46]->RedrawVC (vcmesh, surf) : false);
	case AID_HATCH_SWITCH:
		return (vcmesh ? instr[47]->RedrawVC (vcmesh, surf) : false);
	case AID_ILOCK_SWITCH:
		return (vcmesh ? instr[48]->RedrawVC (vcmesh, surf) : false);
	case AID_OLOCK_SWITCH:
		return (vcmesh ? instr[49]->RedrawVC (vcmesh, surf) : false);
	case AID_NAVMODE:
		return (vcmesh ? instr[5]->RedrawVC (vcmesh, surf) : false);
	case AID_GEARINDICATOR:
		return instr[37]->RedrawVC (vcmesh, surf);
	case AID_NOSECONEINDICATOR:
		return instr[24]->RedrawVC (vcmesh, surf);
	case AID_FLOODLIGHT_SWITCH:
		return (vcmesh ? instr[40]->RedrawVC (vcmesh, surf) : false);
	case AID_INSTRLIGHT_SWITCH:
		return (vcmesh ? instr[38]->RedrawVC (vcmesh, surf) : false);
	case AID_ANGRATEINDICATOR:
		return (vcmesh ? instr[42]->RedrawVC (vcmesh, oapiGetTextureHandle (vcmesh_tpl, 14)) : false);
	case AID_MWS:
		return (vcmesh ? instr[27]->RedrawVC (vcmesh, 0) : false);
	}

	return false;
}

// --------------------------------------------------------------
// Process direct key events
// --------------------------------------------------------------
int DeltaGlider::clbkConsumeDirectKey (char *kstate)
{
	if (KEYMOD_ALT (kstate)) {
		if (KEYDOWN (kstate, OAPI_KEY_ADD)) { // increment scram thrust
			if (scramjet)
				for (int i = 0; i < 2; i++) {
					IncThrusterLevel (th_scram[i], oapiGetSimStep() * 0.3);
					scram_intensity[i] = GetThrusterLevel (th_scram[i]) * scram_max[i];
				}
			RESETKEY (kstate, OAPI_KEY_ADD);
		}
		if (KEYDOWN (kstate, OAPI_KEY_SUBTRACT)) { // decrement scram thrust
			if (scramjet)
				for (int i = 0; i < 2; i++) {
					IncThrusterLevel (th_scram[i], oapiGetSimStep() * -0.3);
					scram_intensity[i] = GetThrusterLevel (th_scram[i]) * scram_max[i];
				}
			RESETKEY (kstate, OAPI_KEY_SUBTRACT);
		}
	}
	return 0;
}

// --------------------------------------------------------------
// Process buffered key events
// --------------------------------------------------------------
int DeltaGlider::clbkConsumeBufferedKey (DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (KEYMOD_ALT (kstate)) {
		switch (key) {
		case OAPI_KEY_B:
			ActivateAirbrake (DOOR_CLOSING);
			return 1;
		}
	} else if (KEYMOD_SHIFT (kstate)) {
	} else if (KEYMOD_CONTROL (kstate)) {
		switch (key) {
		case OAPI_KEY_SPACE: // open control dialog
			oapiOpenDialogEx (g_Param.hDLL, IDD_CTRL, Ctrl_DlgProc, DLG_CAPTIONCLOSE, this);
			return 1;
		case OAPI_KEY_B:
			ActivateAirbrake (DOOR_OPENING);
			return 1;
		case OAPI_KEY_H:
			ssys_hud->RevertHud ();
			return 1;
		}
	} else {
		switch (key) {
		case OAPI_KEY_D:  // "operate radiator"
			RevertRadiator ();
			return 1;
		case OAPI_KEY_G:  // "operate landing gear"
			RevertLandingGear ();
			return 1;
		case OAPI_KEY_H:  // trap HUD change
			ssys_hud->ToggleHUDMode ();
			return true;
		case OAPI_KEY_K:  // "operate docking port"
			RevertDockingPort ();
			return 1;
		case OAPI_KEY_O:  // "operate outer airlock"
			RevertOuterAirlock ();
			return 1;
		}
	}
	return 0;
}

// --------------------------------------------------------------
// Respond to generic messages
// --------------------------------------------------------------
int DeltaGlider::clbkGeneric (int msgid, int prm, void *context)
{
	switch (msgid) {
	case VMSG_LUAINTERPRETER:
		return Lua_InitInterpreter (context);
	case VMSG_LUAINSTANCE:
		return Lua_InitInstance (context);
	}
	return 0;
}


// ==============================================================
// API callback interface
// ==============================================================

// --------------------------------------------------------------
// Module initialisation
// --------------------------------------------------------------
DLLCLBK void InitModule (HINSTANCE hModule)
{
	g_Param.hDLL = hModule;
	oapiRegisterCustomControls (hModule);

	// allocate GDI resources
	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(80,80,224));  // blue
	g_Param.brush[3] = CreateSolidBrush (RGB(160,120,64)); // brown
	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224,224,224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164,164,164));
	g_Param.surf = oapiLoadTexture ("DG\\blitsrc1.dds", true);
}

// --------------------------------------------------------------
// Module cleanup
// --------------------------------------------------------------
DLLCLBK void ExitModule (HINSTANCE hModule)
{
	oapiUnregisterCustomControls (hModule);

	int i;

	// deallocate GDI resources
	for (i = 0; i < 2; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 2; i++) DeleteObject (g_Param.pen[i]);
}

// --------------------------------------------------------------
// Vessel initialisation
// --------------------------------------------------------------
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	// need to init device-dependent resources here in case the screen mode has changed
	g_Param.col[2] = oapiGetColour(80,80,224);
	g_Param.col[3] = oapiGetColour(160,120,64);

	return new DeltaGlider (hvessel, flightmodel);
}

// --------------------------------------------------------------
// Vessel cleanup
// --------------------------------------------------------------
DLLCLBK void ovcExit (VESSEL *vessel)
{
	if (vessel) delete (DeltaGlider*)vessel;
}


// ==============================================================
// Scenario editor interface
// ==============================================================

DeltaGlider *GetDG (HWND hDlg)
{
	// retrieve DG interface from scenario editor
	OBJHANDLE hVessel;
	SendMessage (hDlg, WM_SCNEDITOR, SE_GETVESSEL, (LPARAM)&hVessel);
	return (DeltaGlider*)oapiGetVesselInterface (hVessel);
}

void UpdateDamage (HWND hTab, DeltaGlider *dg)
{
	int i;
	char cbuf[256];

	i = (int)(dg->lwingstatus*100.0+0.5);
	sprintf (cbuf, "%d %%", i);
	SetWindowText (GetDlgItem (hTab, IDC_LEFTWING_STATUS), cbuf);
	oapiSetGaugePos (GetDlgItem (hTab, IDC_LEFTWING_SLIDER), i);
	i = (int)(dg->rwingstatus*100.0+0.5);
	sprintf (cbuf, "%d %%", i);
	SetWindowText (GetDlgItem (hTab, IDC_RIGHTWING_STATUS), cbuf);
	oapiSetGaugePos (GetDlgItem (hTab, IDC_RIGHTWING_SLIDER), i);
}

// --------------------------------------------------------------
// Message procedure for editor page 1 (animation settings)
// --------------------------------------------------------------
BOOL CALLBACK EdPg1Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDHELP:
			g_hc.topic = "/SE_Anim.htm";
			oapiOpenHelp (&g_hc);
			return TRUE;
		case IDC_GEAR_UP:
			GetDG(hTab)->ActivateLandingGear (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_GEAR_DOWN:
			GetDG(hTab)->ActivateLandingGear (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_RETRO_CLOSE:
			GetDG(hTab)->SubsysMainRetro()->ActivateRCover (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_RETRO_OPEN:
			GetDG(hTab)->SubsysMainRetro()->ActivateRCover (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_OLOCK_CLOSE:
			GetDG(hTab)->ActivateOuterAirlock (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_OLOCK_OPEN:
			GetDG(hTab)->ActivateOuterAirlock (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_ILOCK_CLOSE:
			GetDG(hTab)->ActivateInnerAirlock (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_ILOCK_OPEN:
			GetDG(hTab)->ActivateInnerAirlock (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_NCONE_CLOSE:
			GetDG(hTab)->ActivateDockingPort (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_NCONE_OPEN:
			GetDG(hTab)->ActivateDockingPort (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_LADDER_RETRACT:
			GetDG(hTab)->ActivateLadder (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_LADDER_EXTEND:
			GetDG(hTab)->ActivateLadder (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_HATCH_CLOSE:
			GetDG(hTab)->ActivateHatch (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_HATCH_OPEN:
			GetDG(hTab)->ActivateHatch (DeltaGlider::DOOR_OPEN);
			return TRUE;
		case IDC_RADIATOR_RETRACT:
			GetDG(hTab)->ActivateRadiator (DeltaGlider::DOOR_CLOSED);
			return TRUE;
		case IDC_RADIATOR_EXTEND:
			GetDG(hTab)->ActivateRadiator (DeltaGlider::DOOR_OPEN);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// --------------------------------------------------------------
// Message procedure for editor page 2 (passengers)
// --------------------------------------------------------------
BOOL CALLBACK EdPg2Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DeltaGlider *dg;
	int i;

	switch (uMsg) {
	case WM_INITDIALOG: {
		char cbuf[256];
		dg = (DeltaGlider*)oapiGetVesselInterface ((OBJHANDLE)lParam);
		for (i = 0; i < 4; i++)
			SendDlgItemMessage (hTab, IDC_CHECK1+i, BM_SETCHECK, dg->psngr[i] ? BST_CHECKED : BST_UNCHECKED, 0);
		sprintf (cbuf, "%0.2f kg", dg->GetMass());
		SetWindowText (GetDlgItem (hTab, IDC_MASS), cbuf);
		} break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDC_CHECK1:
		case IDC_CHECK2:
		case IDC_CHECK3:
		case IDC_CHECK4: {
			char cbuf[256];
			i = SendDlgItemMessage (hTab, LOWORD(wParam), BM_GETCHECK, 0, 0);
			dg = GetDG(hTab);
			dg->psngr[LOWORD(wParam)-IDC_CHECK1] = (i ? true:false);
			dg->SetPassengerVisuals();
			dg->SetEmptyMass();
			sprintf (cbuf, "%0.2f kg", dg->GetMass());
			SetWindowText (GetDlgItem (hTab, IDC_MASS), cbuf);
			} break;
		}
		break;
	}
	return FALSE;
}

// --------------------------------------------------------------
// Message procedure for editor page 3 (damage)
// --------------------------------------------------------------
BOOL CALLBACK EdPg3Proc (HWND hTab, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DeltaGlider *dg;

	switch (uMsg) {
	case WM_INITDIALOG: {
		dg = (DeltaGlider*)oapiGetVesselInterface ((OBJHANDLE)lParam);
		GAUGEPARAM gp = { 0, 100, GAUGEPARAM::LEFT, GAUGEPARAM::BLACK };
		oapiSetGaugeParams (GetDlgItem (hTab, IDC_LEFTWING_SLIDER), &gp);
		oapiSetGaugeParams (GetDlgItem (hTab, IDC_RIGHTWING_SLIDER), &gp);
		UpdateDamage (hTab, dg);
		} break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDC_REPAIR:
			dg = GetDG(hTab);
			dg->RepairDamage ();
			UpdateDamage (hTab, dg);
			return TRUE;
		}
		break;
	case WM_HSCROLL:
		dg = GetDG(hTab);
		int id = GetDlgCtrlID ((HWND)lParam);
		switch (id) {
		case IDC_LEFTWING_SLIDER:
		case IDC_RIGHTWING_SLIDER:
			switch (LOWORD (wParam)) {
			case SB_THUMBTRACK:
			case SB_LINELEFT:
			case SB_LINERIGHT:
				if (id == IDC_LEFTWING_SLIDER)
					dg->lwingstatus = HIWORD(wParam)*0.01;
				else
					dg->rwingstatus = HIWORD(wParam)*0.01;
				dg->ApplyDamage ();
				UpdateDamage (hTab, dg);
				return TRUE;
			}
			break;
		}
		break;
	}
	return FALSE;
}

// --------------------------------------------------------------
// Add vessel-specific pages into scenario editor
// --------------------------------------------------------------
DLLCLBK void secInit (HWND hEditor, OBJHANDLE hVessel)
{
	DeltaGlider *dg = (DeltaGlider*)oapiGetVesselInterface (hVessel);

	EditorPageSpec eps1 = {"Animations", g_Param.hDLL, IDD_EDITOR_PG1, EdPg1Proc};
	SendMessage (hEditor, WM_SCNEDITOR, SE_ADDPAGEBUTTON, (LPARAM)&eps1);
	EditorPageSpec eps2 = {"Passengers", g_Param.hDLL, IDD_EDITOR_PG2, EdPg2Proc};
	SendMessage (hEditor, WM_SCNEDITOR, SE_ADDPAGEBUTTON, (LPARAM)&eps2);
	if (dg->bDamageEnabled) {
		EditorPageSpec eps3 = {"Damage", g_Param.hDLL, IDD_EDITOR_PG3, EdPg3Proc};
		SendMessage (hEditor, WM_SCNEDITOR, SE_ADDPAGEBUTTON, (LPARAM)&eps3);
	}
}

// ==============================================================
// Message callback function for control dialog box
// ==============================================================

BOOL CALLBACK Ctrl_DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DeltaGlider *dg = (uMsg == WM_INITDIALOG ? (DeltaGlider*)lParam : (DeltaGlider*)oapiGetDialogContext (hWnd));
	// pointer to vessel instance was passed as dialog context

	switch (uMsg) {
	case WM_INITDIALOG:
		UpdateCtrlDialog (dg, hWnd);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			oapiCloseDialog (hWnd);
			return TRUE;
		case IDC_GEAR_UP:
			dg->ActivateLandingGear (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_GEAR_DOWN:
			dg->ActivateLandingGear (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_RETRO_CLOSE:
			dg->SubsysMainRetro()->ActivateRCover (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_RETRO_OPEN:
			dg->SubsysMainRetro()->ActivateRCover (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_NCONE_CLOSE:
			dg->ActivateDockingPort (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_NCONE_OPEN:
			dg->ActivateDockingPort (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_OLOCK_CLOSE:
			dg->ActivateOuterAirlock (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_OLOCK_OPEN:
			dg->ActivateOuterAirlock (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_ILOCK_CLOSE:
			dg->ActivateInnerAirlock (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_ILOCK_OPEN:
			dg->ActivateInnerAirlock (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_LADDER_RETRACT:
			dg->ActivateLadder (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_LADDER_EXTEND:
			dg->ActivateLadder (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_HATCH_CLOSE:
			dg->ActivateHatch (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_HATCH_OPEN:
			dg->ActivateHatch (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_RADIATOR_RETRACT:
			dg->ActivateRadiator (DeltaGlider::DOOR_CLOSING);
			return 0;
		case IDC_RADIATOR_EXTEND:
			dg->ActivateRadiator (DeltaGlider::DOOR_OPENING);
			return 0;
		case IDC_NAVLIGHT:
			dg->SetNavLight (SendDlgItemMessage (hWnd, IDC_NAVLIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
			return 0;
		//case IDC_BEACONLIGHT:
		//	dg->SetBeacon (SendDlgItemMessage (hWnd, IDC_BEACONLIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
		//	return 0;
		case IDC_STROBELIGHT:
			dg->SetStrobeLight (SendDlgItemMessage (hWnd, IDC_STROBELIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
			return 0;
		case IDC_DOCKINGLIGHT:
			//dg->SetDockingLight (SendDlgItemMessage (hWnd, IDC_DOCKINGLIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
			return 0;
		//case IDC_DAMAGE:
		//	oapiOpenDialog (g_Param.hDLL, IDD_DAMAGE, Damage_DlgProc, dg);

		}
		break;
	}
	return oapiDefDialogProc (hWnd, uMsg, wParam, lParam);
}

void UpdateCtrlDialog (DeltaGlider *dg, HWND hWnd)
{
	static int bstatus[2] = {BST_UNCHECKED, BST_CHECKED};

	if (!hWnd) hWnd = oapiFindDialog (g_Param.hDLL, IDD_CTRL);
	if (!hWnd) return;

	int op;

	op = dg->gear_status & 1;
	SendDlgItemMessage (hWnd, IDC_GEAR_DOWN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_GEAR_UP, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->SubsysMainRetro()->RCoverStatus() & 1;
	SendDlgItemMessage (hWnd, IDC_RETRO_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_RETRO_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->nose_status & 1;
	SendDlgItemMessage (hWnd, IDC_NCONE_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_NCONE_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->olock_status & 1;
	SendDlgItemMessage (hWnd, IDC_OLOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_OLOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->ilock_status & 1;
	SendDlgItemMessage (hWnd, IDC_ILOCK_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_ILOCK_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->ladder_status & 1;
	SendDlgItemMessage (hWnd, IDC_LADDER_EXTEND, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_LADDER_RETRACT, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->hatch_status & 1;
	SendDlgItemMessage (hWnd, IDC_HATCH_OPEN, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_HATCH_CLOSE, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->radiator_status & 1;
	SendDlgItemMessage (hWnd, IDC_RADIATOR_EXTEND, BM_SETCHECK, bstatus[op], 0);
	SendDlgItemMessage (hWnd, IDC_RADIATOR_RETRACT, BM_SETCHECK, bstatus[1-op], 0);

	op = dg->beacon[0].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_NAVLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[3].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_BEACONLIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[5].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_STROBELIGHT, BM_SETCHECK, bstatus[op], 0);
	op = dg->beacon[7].active ? 1:0;
	SendDlgItemMessage (hWnd, IDC_DOCKINGLIGHT, BM_SETCHECK, bstatus[op], 0);
}

// ==============================================================
// Message callback function for damage dialog box
// ==============================================================

#ifdef UNDEF
BOOL CALLBACK Damage_DlgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DeltaGlider *dg = (uMsg == WM_INITDIALOG ? (DeltaGlider*)lParam : (DeltaGlider*)oapiGetDialogContext (hWnd));
	// pointer to vessel instance was passed as dialog context

	switch (uMsg) {
	case WM_INITDIALOG:
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			oapiCloseDialog (hWnd);
			return TRUE;
		case IDC_REPAIR:
			dg->RepairDamage();
			return 0;
		}
		break;
	}
	return oapiDefDialogProc (hWnd, uMsg, wParam, lParam);
}

void UpdateDamageDialog (DeltaGlider *dg, HWND hWnd)
{
	if (!hWnd) hWnd = oapiFindDialog (g_Param.hDLL, IDD_DAMAGE);
	if (!hWnd) return;

	char cbuf[16];
	sprintf (cbuf, "%0.0f %%", dg->lwingstatus*100.0);
	SetWindowText (GetDlgItem (hWnd, IDC_LEFTWING_STATUS), cbuf);
	sprintf (cbuf, "%0.0f %%", dg->rwingstatus*100.0);
	SetWindowText (GetDlgItem (hWnd, IDC_RIGHTWING_STATUS), cbuf);
}
#endif
