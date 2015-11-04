// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// MomentInd.h
// User interface for moment indicators on overhead panel
// ==============================================================

#ifndef __MOMENTIND_H
#define __MOMENTIND_H

#include "..\Common\Vessel\Instrument.h"

// ==============================================================

class AngRateIndicator: public PanelElement {
public:
	AngRateIndicator (VESSEL3 *v, SURFHANDLE blitsrc);
	~AngRateIndicator ();
	void Reset2D (MESHHANDLE hMesh);
	void ResetVC (DEVMESHHANDLE hMesh);
	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);

protected:
	void UncoverScale (int which, int axis, double phi, NTVERTEX *vtx);
	void ValStr (double v, char *str);
	void BlitReadout (int which, int axis, const char *str, SURFHANDLE tgt);

private:
	double upt;
	NTVERTEX *vtx0;
	int nvtx;
	double w0, tilt, cost, sint;
	double xcnt[3], ycnt[3], zcnt[3];
	static int bmp_w[3];
	static int bmp_h[3];
	static int ofs_x[3];
	static int ofs_y[3];
	static int label_ofs_x[3];
	char label[3][3][8];
	SURFHANDLE bsrc;
};

#endif // !__MOMENTIND_H