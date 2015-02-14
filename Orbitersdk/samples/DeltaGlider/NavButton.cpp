// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2008 Martin Schweiger
//                   All rights reserved
//
// NavButton.cpp
// Navigation mode button user interface
// ==============================================================

#define STRICT 1
#include "NavButton.h"
#include "DeltaGlider.h"
#include "meshres_p0.h"
#include "meshres_vc.h"

// ==============================================================

NavButton::NavButton (VESSEL3 *v): PanelElement (v)
{
}

// --------------------------------------------------------------

void NavButton::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 20;
}

// --------------------------------------------------------------

bool NavButton::Redraw2D (SURFHANDLE)
{
	// constants for texture coordinates
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_y0 = texh-655.0f;        // top edge of texture block
	static const float tx_dy = 37.0f;              // texture block height
	static const float tv0_active = (tx_y0)/texh, tv1_active = (tx_y0+tx_dy)/texh;
	static const float tv0_idle = (tx_y0+tx_dy+0.5f)/texh, tv1_idle = (tx_y0+tx_dy+0.5f)/texh;
	float tv0, tv1;
	int vofs;

	for (DWORD i = NAVMODE_KILLROT; i <= NAVMODE_HOLDALT; i++) {
		if (vessel->GetNavmodeState (i)) tv0 = tv0_active, tv1 = tv1_active;
		else                             tv0 = tv0_idle,   tv1 = tv1_idle;
		vofs = vtxofs+(i-NAVMODE_KILLROT)*4;
		grp->Vtx[vofs+0].tv = grp->Vtx[vofs+1].tv = tv0;
		grp->Vtx[vofs+2].tv = grp->Vtx[vofs+3].tv = tv1;
	}
		
	return false;
}

// --------------------------------------------------------------

bool NavButton::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE)
{
	if (!hMesh) return false;

	static const int nbutton = 6;
	static const int nvtx_per_button = 16;
	static const int nvtx = nbutton * nvtx_per_button;
	int i, j;
	NTVERTEX vtx[nvtx];
	GROUPEDITSPEC ges;
	ges.flags = GRPEDIT_VTXCRDZ | GRPEDIT_VTXTEXU;
	ges.Vtx = vtx;
	ges.nVtx = nvtx;
	ges.vIdx = NULL;
	static float z0_base[nvtx_per_button] = {
		7.2630f,7.2630f,7.2630f,7.2630f,7.2630f,7.2630f,7.2680f,7.2680f,
		7.2630f,7.2630f,7.2680f,7.2680f,7.2630f,7.2630f,7.2680f,7.2680f
	};
	//static float z0_base[nvtx_per_button] = {
	//	7.2650f,7.2650f,7.2650f,7.2650f,7.2650f,7.2650f,7.2700f,7.2700f,
	//	7.2650f,7.2650f,7.2700f,7.2700f,7.2650f,7.2650f,7.2700f,7.2700f
	//};
	static float z0_shift = 0.004f;
	static float u0_base[nvtx_per_button] = {
		0.3594f,0.3984f,0.3594f,0.3984f,0.3594f,0.3984f,0.3594f,0.3984f,
		0.3594f,0.3594f,0.3594f,0.3594f,0.3594f,0.3594f,0.3594f,0.3594f
	};
	static float u0_shift = (float)(40.0/1024.0);
	for (i = 0; i < nbutton; i++) {
		int vofs = i*nvtx_per_button;
		bool hilight = vessel->GetNavmodeState (i + NAVMODE_KILLROT);
		for (j = 0; j < nvtx_per_button; j++) {
			vtx[vofs+j].z = z0_base[j] + (hilight ? z0_shift : 0);
			vtx[vofs+j].tu = u0_base[j] + (hilight ? u0_shift : 0);
		}
	}
	oapiEditMeshGroup (hMesh, GRP_NAV_BUTTONS_VC, &ges);
	return false;
}

// --------------------------------------------------------------

bool NavButton::ProcessMouse2D (int event, int mx, int my)
{
	int mode = 0;
	if (my < 39) {
		if (mx >= 20 && mx < 59) mode = NAVMODE_KILLROT;
	} else {
		static int navmode[6] = {
			NAVMODE_PROGRADE, NAVMODE_RETROGRADE,
			NAVMODE_NORMAL, NAVMODE_ANTINORMAL,
			NAVMODE_HLEVEL, NAVMODE_HOLDALT
		};
		mode = navmode[mx/39 + ((my-39)/39)*2];
	}
	if (mode) vessel->ToggleNavmode (mode);
	return (mode != 0);
}

// --------------------------------------------------------------

bool NavButton::ProcessMouseVC (int event, VECTOR3 &p)
{
	double dp;
	static int modemap[2][4] = {{2,6,4,1},{0,5,3,0}};
	if (modf (p.x*20.0/5.0, &dp) < 0.8) {
		int col = (int)dp;
		if (modf (p.y*40.0/20.0, &dp) < 0.95) {
			int row = (int)dp;
			int mode = modemap[row][col];
			if (mode > 0) {
				vessel->ToggleNavmode (mode);
				return true;
			}
		}
	}
	return false;
	
}