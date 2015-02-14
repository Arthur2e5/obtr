// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// DGSwitches.cpp
// Prototypes for DG-specific cockpit switches and dials
// ==============================================================

#include "DGSwitches.h"

// ==============================================================

double DGSwitch1::travel = 28.0*RAD;
const int DGSwitch1::nvtx = 33;

DGSwitch1::DGSwitch1 (VESSEL3 *v, Mode m): PanelElement(v), mode(m)
{
	state = vstate = CENTER; // we always initiate as centered, even for 2state switches
}

void DGSwitch1::DefineAnimationVC (const VECTOR3 &ref, const VECTOR3 &axis,
	int meshgrp, int vtxofs)
{
	rf = ref;
	ax = axis;
	grp = meshgrp;
	vofs = vtxofs;
}

bool DGSwitch1::ProcessMouseVC (int event, VECTOR3 &p)
{
	if (event & PANEL_MOUSE_LBDOWN) {
		if (p.y < 0.5) Down();
		else           Up();
	} else if (event & PANEL_MOUSE_LBUP) {
		if (mode == SPRING)
			SetState (CENTER);
	}
	return (state != vstate);
}

bool DGSwitch1::RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf)
{
	static double phi[3] = {0.0, travel, -travel};
	if (state != vstate) {
		int i;
		double phi0 = phi[vstate];
		double phi1 = phi[state];
		double dphi = phi1-phi0;
		VECTOR3 p, pt;
		MATRIX3 R = rotm(ax,dphi); // rotation matrix from current to new state

		NTVERTEX vtx[nvtx];
		WORD vperm[nvtx];
		for (i = 0; i < nvtx; i++) vperm[i] = vofs + i;
		GROUPREQUESTSPEC grs = {vtx, nvtx, vperm, 0, 0, 0, 0, 0};
		oapiGetMeshGroup (hMesh, grp, &grs);
		for (i = 0; i < nvtx; i++) {
			p.x = vtx[i].x - rf.x;
			p.y = vtx[i].y - rf.y;
			p.z = vtx[i].z - rf.z;
			pt = mul(R,p);
			vtx[i].x = (float)(pt.x + rf.x);
			vtx[i].y = (float)(pt.y + rf.y);
			vtx[i].z = (float)(pt.z + rf.z);
			p.x = vtx[i].nx;
			p.y = vtx[i].ny;
			p.z = vtx[i].nz;
			pt = mul(R,p);
			vtx[i].nx = (float)pt.x;
			vtx[i].ny = (float)pt.y;
			vtx[i].nz = (float)pt.z;
		}
		GROUPEDITSPEC ges = {GRPEDIT_VTXCRD|GRPEDIT_VTXNML, 0, vtx, nvtx, vperm};
		oapiEditMeshGroup (hMesh, grp, &ges);
		vstate = state;
	}
	return false;
}

bool DGSwitch1::SetState (State s)
{
	// note: it is admissable to force a 2-state switch to center position
	if (state != s) {
		state = s;
		return true;
	}
	return false;
}

DGSwitch1::State DGSwitch1::Up ()
{
	if (state != UP)
		SetState (state == DOWN && mode != TWOSTATE ? CENTER : UP);
	return state;
}

DGSwitch1::State DGSwitch1::Down ()
{
	if (state != DOWN)
		SetState (state == UP && mode != TWOSTATE ? CENTER : DOWN);
	return state;
}
