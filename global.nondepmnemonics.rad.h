
// radiation equations of motion
#define EOMRADNONE 0 // no source term
#define EOMRADEDD 1 // true Edd
#define EOMRADEDDWITHFLUX 2 // fake Edd with extra flux (KORALTODO, using Fragile 2012 paper inversion -- simple to do)
#define EOMRADM1CLOSURE 3 // M1 Closure

// KORAL
#define BOOSTGRIDPOS (NDIM) // CENT,FACE1,FACE2,FACE3 == assumes they are 0,1,2,3
#define BOOSTDIRS (2)
#define ORTHO2LAB (0) // elo
#define LAB2ORTHO (1) // eup

#define ZAMO2FF (0)
#define FF2ZAMO (1)

#define LAB2FF (0)
#define FF2LAB (1)

#define TYPEUCON (0)
#define TYPEUCOV (1)

//relele.c
// same as dot(a,b) in global.variousmacros.h
//#define dot(A,B) (A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+A[3]*B[3])
#define dot3(A,B) (A[0]*B[0]+A[1]*B[1]+A[2]*B[2])
#define kron(i,j) (i == j ? 1. : 0.)
 

#include "koral.mdefs.h"

/*********************/
//passive definitions
/*********************/

#define RADSOURCEMETHODNONE 0
#define RADSOURCEMETHODEXPLICIT 1
#define RADSOURCEMETHODEXPLICITSUBCYCLE 2
#define RADSOURCEMETHODIMPLICIT 3
#define RADSOURCEMETHODIMPLICITEXPLICITCHECK 4 // KORALTODO


///////////////////
//
// Mathematical or Methods constants (not physical constants)
//
///////////////////
#define Pi (M_PI)     

// KORALTODO: The below need to be chosen intelligently
#define RADEPS (1.e-6)
#define RADCONV (1.e-7)
#define PRADEPS (1.e-6)
#define PRADCONV (1.e-8)
#define IMPEPS (1.e-6)
#define IMPCONV (1.e-6)
#define IMPMAXITER (50)
#define GAMMASMALLLIMIT (1.0-1E-10) // at what point above which assume gamma^2=1.0
#define COUREXPLICIT (0.25) // Effective Courant-like factor for stiff explicit radiation source term
#define RADSHOCKFLAT 1 // 0 or 1.  Whether to include radiation in shock flatener
// RADSHOCKFLAT 1 causes excessive oscillations in RADBEAMFLAT at injection point

// whether to choose Jon or Olek way of handling u2p_rad inversion failures
#define JONCHOICE 0
#define OLEKCHOICE 1

#define CASECHOICE JONCHOICE // choose

#define DORADFIXUPS 1 // whether to fixup inversion failures using harm fixups

// whether to revert to sub-cycle explicit if implicit fails.  Only alternative is die.
#define IMPLICITREVERTEXPLICIT 1

#define SPACETIMESUBSPLITNONE 0
#define TAUSUPPRESS 1
#define SPACETIMESUBSPLITTIME 2
#define SPACETIMESUBSPLITALL 3
#define SPACETIMESUBSPLITSUPERALL 4

// 0 : space-time merged
// 1 : all space merged but separate from time
// 2 : full split
#define WHICHSPACETIMESUBSPLIT SPACETIMESUBSPLITNONE



///////////////
//
// Some physical constants
//
///////////////
#define cTILDA (1.0) // like koral
//#define cTILDA (1E-5)
//#define gTILDA (1E-10) // like koral
#define gTILDA (1.0)
#define GGG0 (6.674e-8)
#define GGG (GGG0/gTILDA) // cgs in cm^3/(kg s^2)
#define CCCTRUE0 (2.99792458e10) // cgs in cm/s
#define CCCTRUE (CCCTRUE0/cTILDA) // cgs in cm/s
#define MSUN (1.989E33) //cgs in grams
#define ARAD (7.56593E-15) // cgs in erg/(K^4 cm^3)
#define K_BOLTZ (1.3806488e-16) // cgs in erg/K
#define M_PROTON (1.67262158e-24) // proton mass in cgs in grams
#define MB (1.66054E-24) // = 1/N_A = 1/(Avogadro's number) = baryon mass in cgs in grams (as often used in general EOSs)
#define SIGMA_RAD (5.67e-5) // cgs in erg/(cm^2 s K^4)

/////////////////////
//
// derived constants
//
/////////////////////
#define MSUNCM (GGG*MSUN/(CCCTRUE*CCCTRUE)) // Msun in cm
