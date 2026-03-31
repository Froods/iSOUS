#include "window_driver.h"


//stages to make the motor spin with the 4 coils
//the motor spins when a coil i grounded
const bool stages_O_Y_P_B[8][4] = {
//step 1:
{false, true, true, true},
//step 2:
{false, false, true, true},
//step 3:
{true, false, true, true},
//step 4:
{true, false, false, true},
//step 5:
{true, true, false, true},
//step 6:
{true, true, false, false},
//step 7:
{true, true, true, false},
//step 8:
{false, true, true, false},
};

