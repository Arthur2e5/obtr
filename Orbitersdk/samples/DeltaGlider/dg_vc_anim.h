// Created by deltaglider_vc.m 26-Sep-2015

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

// GIMBAL_DIAL (VC): mouse catch area
const VECTOR3 VC_GIMBAL_DIAL_mousearea[4] = {{-0.27440,1.00130,7.19359},{-0.24440,1.00130,7.19359},{-0.27440,1.02568,7.21107},{-0.24440,1.02568,7.21107}};

// GIMBAL_DIAL (VC): rotation reference
const VECTOR3 VC_GIMBAL_DIAL_ref = {-0.25940,1.01349,7.20233};

// GIMBAL_DIAL (VC): rotation axis
const VECTOR3 VC_GIMBAL_DIAL_axis = {0.00000,-0.58280,0.81262};

// HOVER_DIAL (VC): mouse catch area
const VECTOR3 VC_HOVER_DIAL_mousearea[4] = {{-0.17793,1.00130,7.19359},{-0.14793,1.00130,7.19359},{-0.17793,1.02568,7.21107},{-0.14793,1.02568,7.21107}};

// HOVER_DIAL (VC): rotation reference
const VECTOR3 VC_HOVER_DIAL_ref = {-0.16293,1.01349,7.20233};

// HOVER_DIAL (VC): rotation axis
const VECTOR3 VC_HOVER_DIAL_axis = {0.00000,-0.58280,0.81262};

// GIMBAL_PSWITCH (VC): mouse catch area
const VECTOR3 VC_GIMBAL_PSWITCH_mousearea[4] = {{-0.22334,1.00616,7.19708},{-0.20350,1.00616,7.19708},{-0.22334,1.02521,7.21074},{-0.20350,1.02521,7.21074}};

// GIMBAL_PSWITCH (VC): rotation reference
const VECTOR3 VC_GIMBAL_PSWITCH_ref = {-0.21342,1.01432,7.20581};

// GIMBAL_PSWITCH (VC): rotation axis
const VECTOR3 VC_GIMBAL_PSWITCH_axis = {1.00000,0.00000,0.00000};

// GIMBAL_YSWITCH (VC): mouse catch area
const VECTOR3 VC_GIMBAL_YSWITCH_mousearea[4] = {{-0.22514,0.99298,7.18762},{-0.22514,0.97686,7.17606},{-0.20170,0.99298,7.18762},{-0.20170,0.97686,7.17606}};

// GIMBAL_YSWITCH (VC): rotation reference
const VECTOR3 VC_GIMBAL_YSWITCH_ref = {-0.21342,0.98355,7.18374};

// GIMBAL_YSWITCH (VC): rotation axis
const VECTOR3 VC_GIMBAL_YSWITCH_axis = {0.00000,-0.81262,-0.58280};

// HOVER_PSWITCH (VC): mouse catch area
const VECTOR3 VC_HOVER_PSWITCH_mousearea[4] = {{-0.12101,1.00616,7.19708},{-0.11289,1.00616,7.19708},{-0.12101,1.02521,7.21074},{-0.11289,1.02521,7.21074}};

// HOVER_PSWITCH (VC): rotation reference
const VECTOR3 VC_HOVER_PSWITCH_ref = {-0.11695,1.01432,7.20581};

// HOVER_PSWITCH (VC): rotation axis
const VECTOR3 VC_HOVER_PSWITCH_axis = {1.00000,0.00000,0.00000};

// HOVER_RSWITCH (VC): mouse catch area
const VECTOR3 VC_HOVER_RSWITCH_mousearea[4] = {{-0.12867,0.99298,7.18762},{-0.12867,0.98638,7.18289},{-0.10523,0.99298,7.18762},{-0.10523,0.98638,7.18289}};

// HOVER_RSWITCH (VC): rotation reference
const VECTOR3 VC_HOVER_RSWITCH_ref = {-0.11695,0.98831,7.18716};

// HOVER_RSWITCH (VC): rotation axis
const VECTOR3 VC_HOVER_RSWITCH_axis = {0.00000,-0.81262,-0.58280};

// HOVER_HOLDALT_SWITCH (VC): mouse catch area
const VECTOR3 VC_HOVER_HOLDALT_SWITCH_mousearea[4] = {{-0.09396,1.00140,7.19366},{-0.09396,0.99481,7.18893},{-0.07052,1.00140,7.19366},{-0.07052,0.99481,7.18893}};

// HOVER_HOLDALT_SWITCH (VC): rotation reference
const VECTOR3 VC_HOVER_HOLDALT_SWITCH_ref = {-0.08224,0.99674,7.19320};

// HOVER_HOLDALT_SWITCH (VC): rotation axis
const VECTOR3 VC_HOVER_HOLDALT_SWITCH_axis = {0.00000,-0.81262,-0.58280};

// GIMBAL_INDICATOR (VC): rotation reference
const VECTOR3 VC_GIMBAL_INDICATOR_ref[2] = {{-0.27012,0.98457,7.18405},{-0.24598,0.98457,7.18405}};

// HOVER_INDICATOR (VC): rotation reference
const VECTOR3 VC_HOVER_INDICATOR_ref = {-0.16158,0.98457,7.18405};

// BTN_HOVER_HOLDALT (VC): mouse catch radius
const double VC_BTN_HOVER_HOLDALT_mouserad = 0.015778;

// BTN_HOVER_HOLDALT (VC): rotation reference
const VECTOR3 VC_BTN_HOVER_HOLDALT_ref = {-0.07638,1.02668,7.21179};

// BTN_HOVER_HOLDALT (VC): rotation axis
const VECTOR3 VC_BTN_HOVER_HOLDALT_axis = {0.00000,-0.58280,0.81262};

// BTN_HOVER_HOLDALT_CUR (VC): mouse catch radius
const double VC_BTN_HOVER_HOLDALT_CUR_mouserad = 0.009000;

// BTN_HOVER_HOLDALT_CUR (VC): rotation reference
const VECTOR3 VC_BTN_HOVER_HOLDALT_CUR_ref = {-0.06330,0.99811,7.19130};

// BTN_HOVER_HOLDALT_CUR (VC): rotation axis
const VECTOR3 VC_BTN_HOVER_HOLDALT_CUR_axis = {0.00000,-0.58280,0.81262};

// HOVERMODE_BUTTONS (VC): mouse catch area
const VECTOR3 VC_HOVERMODE_BUTTONS_mousearea[4] = {{-0.09080,0.97649,7.17580},{-0.06195,0.97649,7.17580},{-0.09080,0.98821,7.18420},{-0.06195,0.98821,7.18420}};

// BTN_HOVERMODE_1 (VC): mouse catch radius
const double VC_BTN_HOVERMODE_1_mouserad = 0.013524;

// BTN_HOVERMODE_1 (VC): rotation reference
const VECTOR3 VC_BTN_HOVERMODE_1_ref = {-0.08382,0.98272,7.18026};

// BTN_HOVERMODE_1 (VC): rotation axis
const VECTOR3 VC_BTN_HOVERMODE_1_axis = {0.00000,-0.58280,0.81262};

// BTN_HOVERMODE_2 (VC): mouse catch radius
const double VC_BTN_HOVERMODE_2_mouserad = 0.013524;

// BTN_HOVERMODE_2 (VC): rotation reference
const VECTOR3 VC_BTN_HOVERMODE_2_ref = {-0.06894,0.98272,7.18026};

// BTN_HOVERMODE_2 (VC): rotation axis
const VECTOR3 VC_BTN_HOVERMODE_2_axis = {0.00000,-0.58280,0.81262};

// RCS_DIAL (VC): mouse catch area
const VECTOR3 VC_RCS_DIAL_mousearea[4] = {{0.05000,1.26200,7.26800},{0.08000,1.26200,7.26800},{0.05000,1.29200,7.26800},{0.08000,1.29200,7.26800}};

// RCS_DIAL (VC): rotation reference
const VECTOR3 VC_RCS_DIAL_ref = {0.06500,1.27700,7.26800};

// RCS_DIAL (VC): rotation axis
const VECTOR3 VC_RCS_DIAL_axis = {0.00000,0.00000,1.00000};

// HUD_BUTTONS (VC): mouse catch area
const VECTOR3 VC_HUD_BUTTONS_mousearea[4] = {{-0.02840,1.28605,7.26800},{0.02840,1.28605,7.26800},{-0.02840,1.30047,7.26800},{0.02840,1.30047,7.26800}};

// BTN_HUDMODE_1 (VC): mouse catch radius
const double VC_BTN_HUDMODE_1_mouserad = 0.015778;

// BTN_HUDMODE_1 (VC): rotation reference
const VECTOR3 VC_BTN_HUDMODE_1_ref = {-0.01938,1.29371,7.26800};

// BTN_HUDMODE_1 (VC): rotation axis
const VECTOR3 VC_BTN_HUDMODE_1_axis = {0.00000,0.00000,1.00000};

// BTN_HUDMODE_2 (VC): mouse catch radius
const double VC_BTN_HUDMODE_2_mouserad = 0.015778;

// BTN_HUDMODE_2 (VC): rotation reference
const VECTOR3 VC_BTN_HUDMODE_2_ref = {0.00000,1.29371,7.26800};

// BTN_HUDMODE_2 (VC): rotation axis
const VECTOR3 VC_BTN_HUDMODE_2_axis = {0.00000,0.00000,1.00000};

// BTN_HUDMODE_3 (VC): mouse catch radius
const double VC_BTN_HUDMODE_3_mouserad = 0.015778;

// BTN_HUDMODE_3 (VC): rotation reference
const VECTOR3 VC_BTN_HUDMODE_3_ref = {0.01938,1.29371,7.26800};

// BTN_HUDMODE_3 (VC): rotation axis
const VECTOR3 VC_BTN_HUDMODE_3_axis = {0.00000,0.00000,1.00000};

// HUD_BRIGHTNESS (VC): mouse catch area
const VECTOR3 VC_HUD_BRIGHTNESS_mousearea[4] = {{-0.02020,1.26191,7.26800},{-0.00060,1.26191,7.26800},{-0.02020,1.28151,7.26800},{-0.00060,1.28151,7.26800}};

// HUD_BRIGHTNESS (VC): rotation reference
const VECTOR3 VC_HUD_BRIGHTNESS_ref = {-0.01040,1.27171,7.26800};

// HUD_BRIGHTNESS (VC): rotation axis
const VECTOR3 VC_HUD_BRIGHTNESS_axis = {0.00000,0.00000,1.00000};

// HUD_COLBUTTON (VC): mouse catch radius
const double VC_HUD_COLBUTTON_mouserad = 0.009000;

// HUD_COLBUTTON (VC): rotation reference
const VECTOR3 VC_HUD_COLBUTTON_ref = {0.01650,1.27171,7.26800};

// HUD_COLBUTTON (VC): rotation axis
const VECTOR3 VC_HUD_COLBUTTON_axis = {0.00000,0.00000,1.00000};

// NAV_BUTTONS (VC): mouse catch area
const VECTOR3 VC_NAV_BUTTONS_mousearea[4] = {{0.10191,1.25710,7.26800},{0.17809,1.25710,7.26800},{0.10191,1.28640,7.26800},{0.17809,1.28640,7.26800}};

// BTN_NAVMODE_1 (VC): mouse catch radius
const double VC_BTN_NAVMODE_1_mouserad = 0.015778;

// BTN_NAVMODE_1 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_1_ref = {0.11092,1.26476,7.26800};

// BTN_NAVMODE_1 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_1_axis = {0.00000,0.00000,1.00000};

// BTN_NAVMODE_2 (VC): mouse catch radius
const double VC_BTN_NAVMODE_2_mouserad = 0.015778;

// BTN_NAVMODE_2 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_2_ref = {0.13031,1.26476,7.26800};

// BTN_NAVMODE_2 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_2_axis = {0.00000,0.00000,1.00000};

// BTN_NAVMODE_3 (VC): mouse catch radius
const double VC_BTN_NAVMODE_3_mouserad = 0.015778;

// BTN_NAVMODE_3 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_3_ref = {0.13031,1.27964,7.26800};

// BTN_NAVMODE_3 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_3_axis = {0.00000,0.00000,1.00000};

// BTN_NAVMODE_4 (VC): mouse catch radius
const double VC_BTN_NAVMODE_4_mouserad = 0.015778;

// BTN_NAVMODE_4 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_4_ref = {0.14969,1.26476,7.26800};

// BTN_NAVMODE_4 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_4_axis = {0.00000,0.00000,1.00000};

// BTN_NAVMODE_5 (VC): mouse catch radius
const double VC_BTN_NAVMODE_5_mouserad = 0.015778;

// BTN_NAVMODE_5 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_5_ref = {0.14969,1.27964,7.26800};

// BTN_NAVMODE_5 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_5_axis = {0.00000,0.00000,1.00000};

// BTN_NAVMODE_6 (VC): mouse catch radius
const double VC_BTN_NAVMODE_6_mouserad = 0.015778;

// BTN_NAVMODE_6 (VC): rotation reference
const VECTOR3 VC_BTN_NAVMODE_6_ref = {0.16908,1.26476,7.26800};

// BTN_NAVMODE_6 (VC): rotation axis
const VECTOR3 VC_BTN_NAVMODE_6_axis = {0.00000,0.00000,1.00000};

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

// Elevator trim indicator (VC): neutral position y
const double vc_etrim_needle0_y[3] = {0.920665,0.923103,0.925541};

// Elevator trim indicator (VC): neutral position z
const double vc_etrim_needle0_z[3] = {7.135757,7.137506,7.139254};

// Elevator trim indicator (VC): tilt angle
const double vc_etrim_tilt = 0.622165;

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

const int VC_AF_DIAL_vofs = 0;

const int VC_GIMBAL_DIAL_vofs = 76;

const int VC_HOVER_DIAL_vofs = 152;

const int VC_GIMBAL_PSWITCH_vofs = 0;

const int VC_GIMBAL_YSWITCH_vofs = 56;

const int VC_HOVER_PSWITCH_vofs = 112;

const int VC_HOVER_RSWITCH_vofs = 140;

const int VC_HOVER_HOLDALT_SWITCH_vofs = 168;

const int VC_GIMBAL_INDICATOR_LEFT_vofs = 0;

const int VC_GIMBAL_INDICATOR_RIGHT_vofs = 8;

const int VC_HOVER_INDICATOR_vofs = 16;

const int VC_BTN_HOVER_HOLDALT_vofs = 0;

const int VC_BTN_HOVER_HOLDALT_LABEL_vofs = 0;

const int VC_BTN_HOVER_HOLDALT_CUR_vofs = 0;

const int VC_BTN_HOVERMODE_1_vofs = 20;

const int VC_BTN_HOVERMODE_1_LABEL_vofs = 8;

const int VC_BTN_HOVERMODE_2_vofs = 40;

const int VC_BTN_HOVERMODE_2_LABEL_vofs = 16;

const int VC_RCS_DIAL_vofs = 228;

const int VC_BTN_HUDMODE_1_vofs = 60;

const int VC_BTN_HUDMODE_1_LABEL_vofs = 24;

const int VC_BTN_HUDMODE_2_vofs = 80;

const int VC_BTN_HUDMODE_2_LABEL_vofs = 32;

const int VC_BTN_HUDMODE_3_vofs = 100;

const int VC_BTN_HUDMODE_3_LABEL_vofs = 40;

const int VC_HUD_COLBUTTON_vofs = 20;

const int VC_BTN_NAVMODE_1_vofs = 120;

const int VC_BTN_NAVMODE_1_LABEL_vofs = 48;

const int VC_BTN_NAVMODE_2_vofs = 140;

const int VC_BTN_NAVMODE_2_LABEL_vofs = 56;

const int VC_BTN_NAVMODE_3_vofs = 160;

const int VC_BTN_NAVMODE_3_LABEL_vofs = 64;

const int VC_BTN_NAVMODE_4_vofs = 180;

const int VC_BTN_NAVMODE_4_LABEL_vofs = 72;

const int VC_BTN_NAVMODE_5_vofs = 200;

const int VC_BTN_NAVMODE_5_LABEL_vofs = 80;

const int VC_BTN_NAVMODE_6_vofs = 220;

const int VC_BTN_NAVMODE_6_LABEL_vofs = 88;

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
