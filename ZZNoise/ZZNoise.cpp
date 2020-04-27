#include "SC_PlugIn.h"


#include <algorithm>
#include "braids/macro_oscillator.h"
#include "braids/envelope.h"
#include "braids/vco_jitter_source.h"


// for signature_waveshaper, need abs
inline int16_t abs(int16_t x) { return x <0.0f ? -x : x;}
#include "braids/signature_waveshaper.h"


// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ZZNoise : public Unit
{
    double mPhase; // phase of the oscillator, from -1 to 1.
    float mFreqMul; // a constant for multiplying frequency
    braids::MacroOscillator osc;

};

// declare unit generator functions
static void ZZNoise_next_a(ZZNoise *unit, int inNumSamples);
static void ZZNoise_Ctor(ZZNoise* unit);

static uint8_t* sync = new uint8_t[128];
static int16_t* outint = new int16_t[128];


//////////////////////////////////////////////////////////////////

// Ctor is called to initialize the unit generator.
// It only executes once.

// A Ctor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
void ZZNoise_Ctor(ZZNoise* unit)
{
    // 1. set the calculation function.
        // if the frequency argument is audio rate
    SETCALC(ZZNoise_next_a);

    // 2. initialize the unit generator state variables.
    // initialize a constant for multiplying the frequency
    unit->mFreqMul = 2.0 * SAMPLEDUR;
    // get initial phase of oscillator
    unit->mPhase = IN0(1);
    unit->osc.Init();
    unit->osc.set_shape(braids::MACRO_OSC_SHAPE_WAVETABLES);

    // 3. calculate one sample of output.
    ZZNoise_next_a(unit, 1);
}


//////////////////////////////////////////////////////////////////

// The calculation function executes once per control period
// which is typically 64 samples.

// calculation function for an audio rate frequency argument
void ZZNoise_next_a(ZZNoise *unit, int inNumSamples)
{
    // get the pointer to the output buffer
    float *out = OUT(0);

    // get the pointer to the input buffer
    float *freq = IN(0);

    // get phase and freqmul constant from struct and store it in a
    // local variable.
    // The optimizer will cause them to be loaded it into a register.
    float freqmul = unit->mFreqMul;
    double phase = unit->mPhase;

    unit->osc.set_pitch(freqmul);


    // perform a loop for the number of samples in the control period.
    // If this unit is audio rate then inNumSamples will be 64 or whatever
    // the block size is. If this unit is control rate then inNumSamples will
    // be 1.
    //bool sync_zero = x->f_ad_mod_vca!=0  || x->f_ad_mod_timbre !=0 || x->f_ad_mod_colour !=0 || x->f_ad_mod_fm !=0;;
    bool sync_zero = 1;

    // Generete the blocks needed for given "n" size.
    for (int i = 0; i < inNumSamples; i++) {
        sync[i] = 0;
    }

    unit->osc.Render(sync, outint, inNumSamples);

    for (int i = 0; i < inNumSamples; i++) {
      //out[i] = outint[i] / 65536.0f ;
      out[i] = unit->mParent->mRGen->frand2();
    }

    // store the phase back to the struct
    unit->mPhase = phase;
}

// the entry point is called by the host when the plug-in is loaded
PluginLoad(ZZNoise)
{
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable

    DefineSimpleUnit(ZZNoise);
}
