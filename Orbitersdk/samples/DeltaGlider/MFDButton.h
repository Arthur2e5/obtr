// ==============================================================
//                ORBITER MODULE: DeltaGlider
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2009 Martin Schweiger
//                   All rights reserved
//
// MFDButton.h
// User interface for MFD buttons
// ==============================================================

#ifndef __MFDBUTTON_H
#define __MFDBUTTON_H

#include "DeltaGlider.h"

class MFDButtonGroup: public PanelElement {
public:
	MFDButtonGroup (VESSEL3 *v, DWORD _mfdid, DWORD _nbtn);
	~MFDButtonGroup ();

protected:
	void PushButtonVC (DEVMESHHANDLE hMesh, int meshgrp, int btn, bool down);
	DWORD mfdid;          ///< MFD identifier (MFD_LEFT, MFD_RIGHT)
	DWORD nbtn;           ///< number of buttons in the group
	DWORD pending_btn;    ///< button waiting to be animated
	DWORD pending_action; ///< 0=none, 1=down, 2=up
	bool *ispushed;
};

// ==============================================================
/**
 * \brief Defines a column of 6 function buttons to the
 *   left or right of an MFD display
 */
class MFDButtonCol: public MFDButtonGroup {
public:
	/**
	 * \brief Creates a button column interface.
	 * \param v vessel instance
	 * \param _mfdid MFD identifier (MFD_LEFT, MFD_RIGHT)
	 * \param _lr 0=left column, 1=right column
	 */
	MFDButtonCol (VESSEL3 *v, DWORD _mfdid, DWORD _lr);

	~MFDButtonCol ();

	bool Redraw2D (SURFHANDLE surf);
	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	DWORD lr;     ///< 0=left, 1=right button column
	DWORD xcnt;   ///< x-offset of button centre line in texture
	int curbtn;   ///< currently pressed button
};

// ==============================================================
/**
 * \brief Defines the three buttons at the bottom edge of an MFD display.
 */
class MFDButtonRow: public MFDButtonGroup {
public:
	/**
	 * \brief Creates a button row interface.
	 * \param v vessel instance
	 * \param _mfdid MFD identifier (MFD_LEFT, MFD_RIGHT)
	 */
	MFDButtonRow (VESSEL3 *v, DWORD _mfdid);

	bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	bool ProcessMouse2D (int event, int mx, int my);
	bool ProcessMouseVC (int event, VECTOR3 &p);

private:
	int curbtn;       ///< currently pressed button
	bool ispushed[2]; ///< button currently down?
};

#endif // !__MFDBUTTON_H
