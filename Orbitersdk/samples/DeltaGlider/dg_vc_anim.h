// Created by deltaglider_vc.m 12-Feb-2015

#ifndef __DG_VC_ANIM_H
#define __DG_VC_ANIM_H

// Lower front panel: tilt from vertical
const double vc_lpanel_tilt = 0.622165;

// AF_DIAL (VC): mouse catch area
const VECTOR3 VC_AF_DIAL_mousearea[4] = {{-0.32300,1.07786,7.23673},{-0.29300,1.07786,7.23673},{-0.32300,1.10605,7.24699},{-0.29300,1.10605,7.24699}};

// AF_DIAL (VC): rotation reference
const VECTOR3 VC_AF_DIAL_ref = {-0.30800,1.09196,7.24186};

// AF_DIAL (VC): rotation axis
const VECTOR3 VC_AF_DIAL_axis = {0.00000,-0.34202,0.93969};

// RCS_DIAL (VC): mouse catch area
const VECTOR3 VC_RCS_DIAL_mousearea[4] = {{0.05000,1.26200,7.26800},{0.08000,1.26200,7.26800},{0.05000,1.29200,7.26800},{0.08000,1.29200,7.26800}};

// RCS_DIAL (VC): rotation reference
const VECTOR3 VC_RCS_DIAL_ref = {0.06500,1.27700,7.26800};

// RCS_DIAL (VC): rotation axis
const VECTOR3 VC_RCS_DIAL_axis = {0.00000,0.00000,1.00000};

// HUD_BUTTONS (VC): mouse catch area
const VECTOR3 VC_HUD_BUTTONS_mousearea[4] = {{-0.02856,1.28531,7.26800},{0.02856,1.28531,7.26800},{-0.02856,1.30211,7.26800},{0.02856,1.30211,7.26800}};

// HUD_BRIGHTNESS (VC): mouse catch area
const VECTOR3 VC_HUD_BRIGHTNESS_mousearea[4] = {{-0.02020,1.26191,7.26800},{-0.00060,1.26191,7.26800},{-0.02020,1.28151,7.26800},{-0.00060,1.28151,7.26800}};

// HUD_BRIGHTNESS (VC): rotation reference
const VECTOR3 VC_HUD_BRIGHTNESS_ref = {-0.01040,1.27171,7.26800};

// HUD_BRIGHTNESS (VC): rotation axis
const VECTOR3 VC_HUD_BRIGHTNESS_axis = {0.00000,0.00000,1.00000};

// HUD_COLBUTTON (VC): mouse catch point
const VECTOR3 VC_HUD_COLBUTTON_mousepoint = {0.01650,1.27171,7.26800};

// HUD_COLBUTTON (VC): mouse catch radius
const double VC_HUD_COLBUTTON_mouserad = 0.018000;

// NAV_BUTTONS (VC): mouse catch area
const VECTOR3 VC_NAV_BUTTONS_mousearea[4] = {{0.10136,1.25700,7.26800},{0.17864,1.25700,7.26800},{0.10136,1.29144,7.26800},{0.17864,1.29144,7.26800}};

// INSTRLIGHT_SWITCH (VC): mouse catch area
const VECTOR3 VC_INSTRLIGHT_SWITCH_mousearea[4] = {{-0.14510,1.65220,6.93405},{-0.12510,1.65220,6.93405},{-0.14510,1.66256,6.89541},{-0.12510,1.66256,6.89541}};

// INSTRLIGHT_SWITCH (VC): rotation reference
const VECTOR3 VC_INSTRLIGHT_SWITCH_ref = {-0.13510,1.66028,6.91550};

// INSTRLIGHT_SWITCH (VC): rotation axis
const VECTOR3 VC_INSTRLIGHT_SWITCH_axis = {1.00000,0.00000,0.00000};

// FLOODLIGHT_SWITCH (VC): mouse catch area
const VECTOR3 VC_FLOODLIGHT_SWITCH_mousearea[4] = {{-0.12031,1.65220,6.93405},{-0.10031,1.65220,6.93405},{-0.12031,1.66256,6.89541},{-0.10031,1.66256,6.89541}};

// FLOODLIGHT_SWITCH (VC): rotation reference
const VECTOR3 VC_FLOODLIGHT_SWITCH_ref = {-0.11031,1.66028,6.91550};

// FLOODLIGHT_SWITCH (VC): rotation axis
const VECTOR3 VC_FLOODLIGHT_SWITCH_axis = {1.00000,0.00000,0.00000};

// INSTR_BRIGHTNESS (VC): mouse catch area
const VECTOR3 VC_INSTR_BRIGHTNESS_mousearea[4] = {{-0.17376,1.65484,6.92419},{-0.15416,1.65484,6.92419},{-0.17376,1.65992,6.90526},{-0.15416,1.65992,6.90526}};

// INSTR_BRIGHTNESS (VC): rotation reference
const VECTOR3 VC_INSTR_BRIGHTNESS_ref = {-0.16396,1.65738,6.91473};

// INSTR_BRIGHTNESS (VC): rotation axis
const VECTOR3 VC_INSTR_BRIGHTNESS_axis = {0.00000,0.96593,0.25882};

// FLOOD_BRIGHTNESS (VC): mouse catch area
const VECTOR3 VC_FLOOD_BRIGHTNESS_mousearea[4] = {{-0.09081,1.65484,6.92419},{-0.07121,1.65484,6.92419},{-0.09081,1.65992,6.90526},{-0.07121,1.65992,6.90526}};

// FLOOD_BRIGHTNESS (VC): rotation reference
const VECTOR3 VC_FLOOD_BRIGHTNESS_ref = {-0.08101,1.65738,6.91473};

// FLOOD_BRIGHTNESS (VC): rotation axis
const VECTOR3 VC_FLOOD_BRIGHTNESS_axis = {0.00000,0.96593,0.25882};

// HUDRETRACT_SWITCH (VC): mouse catch area
const VECTOR3 VC_HUDRETRACT_SWITCH_mousearea[4] = {{0.05585,1.65220,6.93405},{0.07585,1.65220,6.93405},{0.05585,1.66256,6.89541},{0.07585,1.66256,6.89541}};

// HUDRETRACT_SWITCH (VC): rotation reference
const VECTOR3 VC_HUDRETRACT_SWITCH_ref = {0.06585,1.66028,6.91550};

// HUDRETRACT_SWITCH (VC): rotation axis
const VECTOR3 VC_HUDRETRACT_SWITCH_axis = {1.00000,0.00000,0.00000};

// LANDINGLIGHT_SWITCH (VC): mouse catch area
const VECTOR3 VC_LANDINGLIGHT_SWITCH_mousearea[4] = {{0.09011,1.65220,6.93405},{0.11011,1.65220,6.93405},{0.09011,1.66256,6.89541},{0.11011,1.66256,6.89541}};

// LANDINGLIGHT_SWITCH (VC): rotation reference
const VECTOR3 VC_LANDINGLIGHT_SWITCH_ref = {0.10011,1.66028,6.91550};

// LANDINGLIGHT_SWITCH (VC): rotation axis
const VECTOR3 VC_LANDINGLIGHT_SWITCH_axis = {1.00000,0.00000,0.00000};

// STROBELIGHT_SWITCH (VC): mouse catch area
const VECTOR3 VC_STROBELIGHT_SWITCH_mousearea[4] = {{0.11265,1.65220,6.93405},{0.13265,1.65220,6.93405},{0.11265,1.66256,6.89541},{0.13265,1.66256,6.89541}};

// STROBELIGHT_SWITCH (VC): rotation reference
const VECTOR3 VC_STROBELIGHT_SWITCH_ref = {0.12265,1.66028,6.91550};

// STROBELIGHT_SWITCH (VC): rotation axis
const VECTOR3 VC_STROBELIGHT_SWITCH_axis = {1.00000,0.00000,0.00000};

// NAVLIGHT_SWITCH (VC): mouse catch area
const VECTOR3 VC_NAVLIGHT_SWITCH_mousearea[4] = {{0.13519,1.65220,6.93405},{0.15519,1.65220,6.93405},{0.13519,1.66256,6.89541},{0.15519,1.66256,6.89541}};

// NAVLIGHT_SWITCH (VC): rotation reference
const VECTOR3 VC_NAVLIGHT_SWITCH_ref = {0.14519,1.66028,6.91550};

// NAVLIGHT_SWITCH (VC): rotation axis
const VECTOR3 VC_NAVLIGHT_SWITCH_axis = {1.00000,0.00000,0.00000};

// HATCH_SWITCH (VC): mouse catch area
const VECTOR3 VC_HATCH_SWITCH_mousearea[4] = {{-0.15855,1.66582,6.88325},{-0.13855,1.66582,6.88325},{-0.15855,1.67617,6.84461},{-0.13855,1.67617,6.84461}};

// HATCH_SWITCH (VC): rotation reference
const VECTOR3 VC_HATCH_SWITCH_ref = {-0.14855,1.67389,6.86471};

// HATCH_SWITCH (VC): rotation axis
const VECTOR3 VC_HATCH_SWITCH_axis = {1.00000,0.00000,0.00000};

// ILOCK_SWITCH (VC): mouse catch area
const VECTOR3 VC_ILOCK_SWITCH_mousearea[4] = {{-0.13150,1.66582,6.88325},{-0.11150,1.66582,6.88325},{-0.13150,1.67617,6.84461},{-0.11150,1.67617,6.84461}};

// ILOCK_SWITCH (VC): rotation reference
const VECTOR3 VC_ILOCK_SWITCH_ref = {-0.12150,1.67389,6.86471};

// ILOCK_SWITCH (VC): rotation axis
const VECTOR3 VC_ILOCK_SWITCH_axis = {1.00000,0.00000,0.00000};

// OLOCK_SWITCH (VC): mouse catch area
const VECTOR3 VC_OLOCK_SWITCH_mousearea[4] = {{-0.10445,1.66582,6.88325},{-0.08445,1.66582,6.88325},{-0.10445,1.67617,6.84461},{-0.08445,1.67617,6.84461}};

// OLOCK_SWITCH (VC): rotation reference
const VECTOR3 VC_OLOCK_SWITCH_ref = {-0.09445,1.67389,6.86471};

// OLOCK_SWITCH (VC): rotation axis
const VECTOR3 VC_OLOCK_SWITCH_axis = {1.00000,0.00000,0.00000};

// CABIN_O2_SWITCH (VC): mouse catch area
const VECTOR3 VC_CABIN_O2_SWITCH_mousearea[4] = {{-0.18560,1.68723,6.80334},{-0.16560,1.68723,6.80334},{-0.18560,1.69758,6.76470},{-0.16560,1.69758,6.76470}};

// CABIN_O2_SWITCH (VC): rotation reference
const VECTOR3 VC_CABIN_O2_SWITCH_ref = {-0.17560,1.69530,6.78479};

// CABIN_O2_SWITCH (VC): rotation axis
const VECTOR3 VC_CABIN_O2_SWITCH_axis = {1.00000,0.00000,0.00000};

// VALVE1_SWITCH (VC): mouse catch area
const VECTOR3 VC_VALVE1_SWITCH_mousearea[4] = {{-0.15855,1.68723,6.80334},{-0.13855,1.68723,6.80334},{-0.15855,1.69758,6.76470},{-0.13855,1.69758,6.76470}};

// VALVE1_SWITCH (VC): rotation reference
const VECTOR3 VC_VALVE1_SWITCH_ref = {-0.14855,1.69530,6.78479};

// VALVE1_SWITCH (VC): rotation axis
const VECTOR3 VC_VALVE1_SWITCH_axis = {1.00000,0.00000,0.00000};

// VALVE2_SWITCH (VC): mouse catch area
const VECTOR3 VC_VALVE2_SWITCH_mousearea[4] = {{-0.13150,1.68723,6.80334},{-0.11150,1.68723,6.80334},{-0.13150,1.69758,6.76470},{-0.11150,1.69758,6.76470}};

// VALVE2_SWITCH (VC): rotation reference
const VECTOR3 VC_VALVE2_SWITCH_ref = {-0.12150,1.69530,6.78479};

// VALVE2_SWITCH (VC): rotation axis
const VECTOR3 VC_VALVE2_SWITCH_axis = {1.00000,0.00000,0.00000};

// VALVE3_SWITCH (VC): mouse catch area
const VECTOR3 VC_VALVE3_SWITCH_mousearea[4] = {{-0.10445,1.68723,6.80334},{-0.08445,1.68723,6.80334},{-0.10445,1.69758,6.76470},{-0.08445,1.69758,6.76470}};

// VALVE3_SWITCH (VC): rotation reference
const VECTOR3 VC_VALVE3_SWITCH_ref = {-0.09445,1.69530,6.78479};

// VALVE3_SWITCH (VC): rotation axis
const VECTOR3 VC_VALVE3_SWITCH_axis = {1.00000,0.00000,0.00000};

// LOCK_O2_SWITCH (VC): mouse catch area
const VECTOR3 VC_LOCK_O2_SWITCH_mousearea[4] = {{-0.07740,1.68723,6.80334},{-0.05740,1.68723,6.80334},{-0.07740,1.69758,6.76470},{-0.05740,1.69758,6.76470}};

// LOCK_O2_SWITCH (VC): rotation reference
const VECTOR3 VC_LOCK_O2_SWITCH_ref = {-0.06740,1.69530,6.78479};

// LOCK_O2_SWITCH (VC): rotation axis
const VECTOR3 VC_LOCK_O2_SWITCH_axis = {1.00000,0.00000,0.00000};

// Gear lever (VC): mouse catch area
const VECTOR3 vc_gearlever_mousearea[4] = {{-0.38900,0.90014,7.12473},{-0.35900,0.90014,7.12473},{-0.38900,0.99132,7.19012},{-0.35900,0.99132,7.19012}};

// Nosecone lever (VC): mouse catch area
const VECTOR3 vc_nconelever_mousearea[4] = {{0.35900,0.90014,7.12473},{0.38900,0.90014,7.12473},{0.35900,0.99132,7.19012},{0.38900,0.99132,7.19012}};

// Undock lever (VC): mouse catch area
const VECTOR3 vc_undocklever_mousearea[4] = {{0.30450,0.88247,7.10837},{0.33950,0.88247,7.10837},{0.30450,0.97999,7.17830},{0.33950,0.97999,7.17830}};

// Airbrake lever (VC): mouse catch area
const VECTOR3 vc_abrakelever_mousearea[4] = {{-0.33700,0.97755,7.17655},{-0.29300,0.97755,7.17655},{-0.33700,1.04256,7.22318},{-0.29300,1.04256,7.22318}};

// Elevator trim wheel (VC): mouse catch area
const VECTOR3 vc_etrimwheel_mousearea[4] = {{-0.34600,0.87841,7.10545},{-0.31600,0.87841,7.10545},{-0.34600,0.96780,7.16956},{-0.31600,0.96780,7.16956}};

// Gimbal pitch switches (VC): mouse catch area
const VECTOR3 vc_gpswitch_mousearea[4] = {{-0.22189,1.00616,7.19708},{-0.20205,1.00616,7.19708},{-0.22189,1.02521,7.21074},{-0.20205,1.02521,7.21074}};

// Gimbal pitch switches (VC): rotation axis
const VECTOR3 vc_gpswitch_axis = {1.00000,0.00000,0.00000};

// Gimbal pitch switches (VC): rotation reference
const VECTOR3 vc_gpswitch_ref = {-0.21197,1.01432,7.20581};

// Gimbal yaw switches (VC): mouse catch area
const VECTOR3 vc_gyswitch_mousearea[4] = {{-0.22369,0.99298,7.18762},{-0.22369,0.97686,7.17606},{-0.20025,0.99298,7.18762},{-0.20025,0.97686,7.17606}};

// Gimbal yaw switches (VC): rotation axis
const VECTOR3 vc_gyswitch_axis = {0.00000,-0.81262,-0.58280};

// Gimbal yaw switches (VC): rotation reference
const VECTOR3 vc_gyswitch_ref = {-0.21197,0.98355,7.18374};

// Hover pitch switch (VC): mouse catch area
const VECTOR3 vc_hpswitch_mousearea[4] = {{-0.11820,1.00616,7.19708},{-0.11009,1.00616,7.19708},{-0.11820,1.02521,7.21074},{-0.11009,1.02521,7.21074}};

// Hover pitch switch (VC): rotation axis
const VECTOR3 vc_hpswitch_axis = {1.00000,0.00000,0.00000};

// Hover pitch switch (VC): rotation reference
const VECTOR3 vc_hpswitch_ref = {-0.11415,1.01432,7.20581};

// Hover roll switch (VC): mouse catch area
const VECTOR3 vc_hrswitch_mousearea[4] = {{-0.12587,0.99298,7.18762},{-0.12587,0.98638,7.18289},{-0.10243,0.99298,7.18762},{-0.10243,0.98638,7.18289}};

// Hover roll switch (VC): rotation axis
const VECTOR3 vc_hrswitch_axis = {0.00000,-0.81262,-0.58280};

// Hover roll switch (VC): rotation reference
const VECTOR3 vc_hrswitch_ref = {-0.11415,0.98831,7.18716};

// Elevator trim indicator (VC): neutral position y
const double vc_etrim_needle0_y[3] = {0.920665,0.923103,0.925541};

// Elevator trim indicator (VC): neutral position z
const double vc_etrim_needle0_z[3] = {7.135757,7.137506,7.139254};

// Elevator trim indicator (VC): tilt angle
const double vc_etrim_tilt = 0.622165;

// Gimbal position display (VC): centre positions
const VECTOR3 vc_gimind_cnt[2] = {{-0.26912,0.98457,7.18405},{-0.24498,0.98457,7.18405}};

// Hover att position display (VC): centre position
const VECTOR3 vc_hvrind_cnt = {-0.15923,0.98457,7.18405};

// Gimbal mode dial (VC): mouse catch area
const VECTOR3 vc_gimdial_mousearea[4] = {{-0.27340,1.00130,7.19359},{-0.24340,1.00130,7.19359},{-0.27340,1.02568,7.21107},{-0.24340,1.02568,7.21107}};

// Gimbal mode dial (VC): rotation axis
const VECTOR3 vc_gimdial_axis = {0.00000,0.58280,-0.81262};

// Gimbal mode dial (VC): rotation reference
const VECTOR3 vc_gimdial_ref = {-0.25840,1.01349,7.20233};

// Hover att mode dial (VC): mouse catch area
const VECTOR3 vc_hvrdial_mousearea[4] = {{-0.17558,1.00130,7.19359},{-0.14558,1.00130,7.19359},{-0.17558,1.02568,7.21107},{-0.14558,1.02568,7.21107}};

// Hover att mode dial (VC): rotation axis
const VECTOR3 vc_hrvdial_axis = {0.00000,0.58280,-0.81262};

// Hover att mode dial (VC): rotation reference
const VECTOR3 vc_hvrdial_ref = {-0.16058,1.01349,7.20233};

// Elevator trim wheel (VC): rotation axis
const VECTOR3 etrim_wheel_axis = {1.00000,0.00000,0.00000};

// Elevator trim wheel (VC): rotation reference
const VECTOR3 etrim_wheel_ref = {-0.33500,0.89425,7.17773};

// Gear lever (VC): rotation axis
const VECTOR3 vc_gearlever_axis = {1.00000,0.00000,0.00000};

// Gear lever (VC): rotation reference
const VECTOR3 vc_gearlever_ref = {-0.37400,0.91659,7.19806};

// Airbrake lever (VC): rotation axis
const VECTOR3 vc_abrakelever_axis = {1.00000,0.00000,0.00000};

// Airbrake lever (VC): rotation reference
const VECTOR3 vc_abrakelever_ref = {-0.31700,0.94718,7.25322};

// Nosecone lever (VC): rotation axis
const VECTOR3 vc_nconelever_axis = {1.00000,0.00000,0.00000};

// Nosecone lever (VC): rotation reference
const VECTOR3 vc_nconelever_ref = {0.37400,0.91659,7.19806};

// Undock lever (VC): rotation axis
const VECTOR3 vc_undocklever_axis = {1.00000,0.00000,0.00000};

// Undock lever (VC): rotation reference
const VECTOR3 vc_undocklever_ref = {0.32200,0.88697,7.12697};

// Retro cover switch (VC): rotation axis
const VECTOR3 vc_rcoverswitch_axis = {1.00000,0.00000,0.00000};

// Retro cover switch (VC): rotation reference
const VECTOR3 vc_rcoverswitch_ref = {0.00000,1.00378,7.19660};

// Retro cover switch (VC): mouse catch area
const VECTOR3 vc_rcoverswitch_mousearea[4] = {{0.29700,0.98115,7.17668},{0.33700,0.98115,7.17668},{0.29700,1.02991,7.21164},{0.33700,1.02991,7.21164}};

const int VC_INSTRLIGHT_SWITCH_vofs = 0;

const int VC_FLOODLIGHT_SWITCH_vofs = 33;

const int VC_HUDRETRACT_SWITCH_vofs = 66;

const int VC_LANDINGLIGHT_SWITCH_vofs = 99;

const int VC_STROBELIGHT_SWITCH_vofs = 132;

const int VC_NAVLIGHT_SWITCH_vofs = 165;

const int VC_HATCH_SWITCH_vofs = 198;

const int VC_ILOCK_SWITCH_vofs = 231;

const int VC_OLOCK_SWITCH_vofs = 264;

const int VC_CABIN_O2_SWITCH_vofs = 297;

const int VC_VALVE1_SWITCH_vofs = 330;

const int VC_VALVE2_SWITCH_vofs = 363;

const int VC_VALVE3_SWITCH_vofs = 396;

const int VC_LOCK_O2_SWITCH_vofs = 429;

#endif // !__DG_VC_ANIM_H
