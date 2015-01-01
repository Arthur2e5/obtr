// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2009 Martin Schweiger
//                   All rights reserved
//
// WheelBrake.cpp
// User interface for Wheel brake levers
// ==============================================================

#define STRICT 1
#include "WheelBrake.h"
#include "meshres_p0.h"

// ==============================================================

WheelBrakeLever::WheelBrakeLever (VESSEL3 *v): PanelElement (v)
{
	int i;
	for (i = 0; i < 2; i++)
		isdown[i] = false;
}

// ==============================================================

void WheelBrakeLever::Reset2D (MESHHANDLE hMesh)
{
	grp = oapiMeshGroup (hMesh, GRP_INSTRUMENTS_ABOVE_P0);
	vtxofs = 104;
}

// ==============================================================

bool WheelBrakeLever::Redraw2D (SURFHANDLE surf)
{
	static const float texh = (float)PANEL2D_TEXH; // texture height
	static const float tx_y0 = texh-650.0f;
	static const float tx_dy = 77.0f;
	int i, j;
	for (i = 0; i < 2; i++) {
		double lvl = vessel->GetWheelbrakeLevel (i+1);
		bool down = (lvl > 0.5);
		if (down != isdown[i]) {
			float tv = (down ? tx_y0+tx_dy : tx_y0)/texh;
			for (j = 2; j < 4; j++)
				grp->Vtx[vtxofs+i*4+j].tv = tv;
			isdown[i] = down;
		}
	}
	return false;
}

// ==============================================================

bool WheelBrakeLever::ProcessMouse2D (int event, int mx, int my)
{
	int which = (mx < 15 ? 1 : mx > 37 ? 2 : 0);
	bool press = (event == PANEL_MOUSE_LBDOWN);
	vessel->SetWheelbrakeLevel (press ? 1.0:0.0, which);
	return false;
}
