
//###################################################
//   gpgate - a gps-driven gate remote controller
//- - - - - - - - - - - - - - - - - - - - - - - - - -
//  CONFIGURATION FILE   (this is an exsample)
//---------------------------------------------------


// ---------------
//   RC settings

#define RC_PULSELEN 420  // pulse length [ms]
#define RC_PROTOCOL  10  // trasmission protocol


// ---------------
//    WAYPOINTS

//  entry waypoint is { latitude, longitude, id, label }
const waypoint wps[] = {
                 { 46.23212, 6.04516, 1, "gate1" },  // main entrance
                 { 46.23176, 6.04611, 2, "gate2" },  // exit
                 { 46.23187, 6.04627, 3, "gate3" }   // secondary entrance
                };



// ---------------
//      GATES

//  entry gate is { label, rc code, color }
const gate gates[] = {
                 { "g1", "001100010010", 0x0000FF }, 
                 { "g2", "011110100001", 0x00FF00 },
                 { "g3", "101101110011", 0xFF0000 }
               };

// sys variables
const size_t nwps = sizeof(wps)/sizeof(wps[0]);
const size_t ngates = sizeof(gates)/sizeof(gates[0]);
