// https://github.com/PaulStoffregen/Time
#include <Time.h>
#include <TimeLib.h>

// http://github.com/JChristensen/Timezone
#include <Timezone.h>

// https://github.com/dmkishi/Dusk2Dawn
#include <Dusk2Dawn.h>

#define LAT 50.62448840748527
#define LNG 2.6875116810701773
#define TMZ 2

Dusk2Dawn home(LAT, LNG, TMZ);

void time_setup()
{

}
