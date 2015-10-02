// ==============================================================
//             ORBITER MODULE: Common vessel tools
//                  Part of the ORBITER SDK
//          Copyright (C) 2001-2015 Martin Schweiger
//                   All rights reserved
//
// Instrument.h
// Interface for class PanelElement:
//   Base class for panel and VC instrument visualisations
// Interface for class Subsystem:
//   Base class for a vessel subsystem: acts as a container for
//   a group of panel elements and underlying system logic
// ==============================================================

#ifndef __INSTRUMENT_H
#define __INSTRUMENT_H

#include "Orbitersdk.h"

class VESSEL3;

// ==============================================================

class PanelElement {
public:
	PanelElement (VESSEL3 *v);
	virtual ~PanelElement ();

	virtual void AddMeshData2D (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void AddMeshDataVC (MESHHANDLE hMesh, DWORD grpidx) {}
	virtual void Reset2D ();
	virtual void Reset2D (MESHHANDLE hMesh);
	virtual void ResetVC (DEVMESHHANDLE hMesh);
	virtual bool Redraw2D (SURFHANDLE surf);
	virtual bool RedrawVC (DEVMESHHANDLE hMesh, SURFHANDLE surf);
	virtual bool ProcessMouse2D (int event, int mx, int my);
	virtual bool ProcessMouseVC (int event, VECTOR3 &p);

protected:
	void AddGeometry (MESHHANDLE hMesh, DWORD grpidx, const NTVERTEX *vtx, DWORD nvtx, const WORD *idx, DWORD nidx);

	char *DispStr (double dist, int precision=4);

	VESSEL3 *vessel;
	MESHHANDLE mesh;
	DWORD gidx;
	MESHGROUP *grp; // panel mesh group representing the instrument
	DWORD vtxofs;   // vertex offset in mesh group
};

// ==============================================================

/**
 * \brief Base class for a vessel subsystem
 *
 * This class can be used to represent a logical "subsystem" for a vessel (engine
 * management, autopilots, electrical, thermal, pressure, payload management, etc.
 * Defining subsystems in separate classes rather than directly in the vessel class
 * helps stucturing the code logically and de-clutters the vessel class.
 * Subsystem also acts as a container for the PanelElement objects that provide the
 * user interface for that subsystem. It provides mouse and redraw callback functions
 * that can be called from the corresponding vessel class callback function, and
 * passes the call on to the appropriate subsystem panel element.
 */
class Subsystem {
public:
	/**
	 * \brief Create a new subsystem.
	 * \param v Vessel instance
	 * \param ident subsystem identifier
	 * \note Usually called from the vessel constructor
	 */
	Subsystem (VESSEL3 *v, int ident);

	/**
	 * \brief Subsystem destructor
	 * \note Usually called from the vessel destructor
	 */
	virtual ~Subsystem ();

	/**
	 * \brief Returns pointer to the associated vessel instance
	 */
	inline VESSEL3 *Vessel() const { return vessel; }

	/**
	 * \brief Returns panel identifier
	 */
	inline int Id() const { return id; }

	/**
	 * \brief Add a PanelElement instance to the subsystem.
	 * \param el Pointer to panel element instance
	 * \return Local panel element identifier
	 * \note The panel element instance is managed by the subsystem and destroyed
	 *   together with the subsystem. The calling function must not delete the instance.
	 */
	int AddElement (PanelElement *el);


	PanelElement *Element (DWORD i) const { return (i < nelement ? element[i]:0); }

	/**
	 * \brief Return the number of panel elements associated with the subsystem
	 */
	inline int NumElements() const { return nelement; }

	/**
	 * \brief Maps a subsystem-local panel element identifier to a global identfier.
	 * \param elementid Local element identifier
	 * \return Global identifier
	 * \note Mapping between local and global identifiers allows the subsystem to define
	 *   its own IDs for panel elements, and associating it with the global ID used by
	 *   Orbiter for vessel-wide communication.
	 */
	inline int GlobalElId (int elementid) const { return elementid + (id+1)*1000; }

	/**
	 * \brief Maps a global panel element identifier to a subsystem-local ID
	 * \param [in] globalid Global panel element identifier
	 * \param [out] subsysid Identifier of the subsystem responsible for the element
	 * \return Local element ID
	 * \note Splits a global element ID into a subsystem ID and a local element ID.
	 *   This method can be used by the vessel class to distribute incoming element
	 *   events to the appropriate subsystem.
	 */
	static int LocalElId (int globalid, int &subsysid);

	/**
	 * \brief Per-frame notification
	 * \param simt Session logical runtime [s]
	 * \param simdt last step interval [s]
	 * \param mjd absolute time in MJD format [days]
	 * \note This method should be called by VESSEL2::clbkPostStep for all defined
	 *   subsystems.
	 */
	virtual void clbkPostStep (double simt, double simdt, double mjd) {}

	virtual void clbkPostCreation () {}

	virtual void clbkSaveState (FILEHANDLE scn) {}

	virtual bool clbkParseScenarioLine (const char *line) { return false; }

	/**
	 * \brief Set up the 2D instrument panel for the subsystem
	 * \param panelid Panel ID, as passed to VESSEL3::clbkLoadPanel2D
	 * \param hPanel Panel handle, as passed to VESSEL3::clbkLoadPanel2D
	 * \param viewW viewport width, as passed to VESSEL3::clbkLoadPanel2D
	 * \param viewH viewport height, as passed to VESSEL3::clbkLoadPanel2D
	 * \return true if the subsystem supports 2D panel cockpit mode
     */
	virtual bool clbkLoadPanel2D (int panelid, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) { return false; }

	/**
	 * \brief Set up the virtual panel elements for the subsystem
	 * \param vcid Virtual cockpit position ID (>= 0)
	 * \return true if the subsystem supports a virtual cockpit mode
	 * \note This method should be called by the vessel clbkLoadVC method
	 *   for all defined subsystems.
	 */
	virtual bool clbkLoadVC (int vcid) { return false; }

	virtual void clbkReset2D (int panelid, MESHHANDLE hMesh);

	virtual void clbkResetVC (int vcid, DEVMESHHANDLE hMesh);

	/**
	 * \brief Redraw event for a subsystem panel element
	 * \param id Subsystem-local panel element ID
	 * \param event event type, as passed to VESSEL2::clbkVCRedrawEvent
	 * \param hMesh VC mesh handle, as passed to VESSEL2::clbkVCRedrawEvent
	 * \param hSurf texture surface mesh handle, as passed to VESSEL2::clbkVCRedrawEvent
	 * \return The function should return true if it processes the event,
	 *   false otherwise.
	 * \note This method should be called by the vessel clbkVCRedrawEvent method
	 *   after splitting the global element ID into subsystem and local element IDs
	 */
	virtual bool clbkVCRedrawEvent (int id, int event, DEVMESHHANDLE hMesh, SURFHANDLE hSurf);

	virtual bool clbkVCMouseEvent (int id, int event, VECTOR3 &p);

private:
	VESSEL3 *vessel;         ///< associated vessel object
	int id;                  ///< subsystem ID
	PanelElement **element;  ///< list of panel elements
	DWORD nelement;          ///< number of panel elements
};

#endif // !__INSTRUMENT_H