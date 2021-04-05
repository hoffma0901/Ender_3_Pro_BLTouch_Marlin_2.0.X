# Ender_3_Pro_BLTouch_Marlin_2.0.X


Working Creality Ender 3 Pro Firmware with BLTouch Sensor. Fixed some issues with the configuration for the Ender 3 Pro provided within the Marlin firmware repo.

I ran into some problems with the z axis missing steps, resulting in wrong layer heights and a poor result of the auto bed leveling. Fixed with the following fixes.

My Ender 3 Pro Configuration:

- Creality Silent Board V4.2.7
- Creality BLTouch connected to the BLTouch Connector
- BLV Linear Guide Kit
- Stock Extruder
- Z-Limit switch removed

Changes done to configuration.h (Verison 020008):

Line 121 - #define SERIAL_PORT_2 3

Line 536 - #define DEFAULT_Kp  28.72
Line 537 - #define DEFAULT_Ki   2.26
Line 538 - #define DEFAULT_Kd  78.81

Line 559 - #define PIDTEMPBED

Line 577 - #define DEFAULT_bedKp 462.1
Line 578 - #define DEFAULT_bedKi 85.47
Line 579 - #define DEFAULT_bedKd 624.59

Line 832 - #define DEFAULT_MAX_FEEDRATE          { 500, 500, 2.5, 25 }        // Fixes problems with z axis missing steps, while printing

Line 845 - #define DEFAULT_MAX_ACCELERATION      { 500, 500, 100, 1000 }      // Fixes problems with z axis missing steps, while printing

Line 872 - #define CLASSIC_JERK
Line 880 - #define LIMITED_JERK_EDITING

Line 882 - #define MAX_JERK_EDIT_VALUES { 10, 10, 0.3, 5 }

Line 925 - //#define Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN                     //the z endstop pin will not be used

Line 928 - #define USE_PROBE_FOR_Z_HOMING                                   //BLTouch as z end stop

Line 945 - #define Z_MIN_PROBE_PIN PB1                                      //BLtouch port on 4.2.7 board

Line 989 - #define BLTOUCH

Line 1086 - #define NOZZLE_TO_PROBE_OFFSET { -44, -16, 0 }                  //adjust to your own setup

Line 1090 - #define PROBING_MARGIN 20

Line 1418 - #define AUTO_BED_LEVELING_BILINEAR                              //using ABL with a 3x3 matrix

Line 1428 - #define RESTORE_LEVELING_AFTER_G28

Line 1611 - #define Z_SAFE_HOMING

Changes done to configuration_adv.h

Line 1772 - #define BABYSTEP_MULTIPLICATOR_Z  10                            //makes babystepping easier

Line 1787 - #define BABYSTEP_ZPROBE_OFFSET                                  //babystepping for the z offset

Line 1790 - #define BABYSTEP_ZPROBE_GFX_OVERLAY                             //graphical overlay for setting z offset

Start G-Code configuration for Cura:

; Ender 3 Custom Start G-code

M140 S{material_bed_temperature_layer_0} ; Set Heat Bed temperature28
M105 ; Report Temps
M190 S{material_bed_temperature_layer_0} ; Wait for Heat Bed temperature

G28 ; Home all axis
G29 ; Bed Leveling

M104 S{material_print_temperature_layer_0} ; Set Extruder temperature
M105 ; Report Temps
M109 S{material_print_temperature_layer_0} ; Wait for Extruder temperature
G92 E0 ; Reset Extruder
G1 Z2.0 F3000 ; Move Z Axis up little to prevent scratching of Heat Bed
G1 X0.1 Y20 Z0.3 F5000.0 ; Move to start position
G1 X0.1 Y200.0 Z0.3 F1500.0 E15 ; Draw the first line
G1 X0.4 Y200.0 Z0.3 F5000.0 ; Move to side a little
G1 X0.4 Y20 Z0.3 F1500.0 E30 ; Draw the second line
G92 E0 ; Reset Extruder
G1 Z2.0 F3000 ; Move Z Axis up little to prevent scratching of Heat Bed
G1 X5 Y20 Z0.3 F5000.0 ; Move over to prevent blob squish
