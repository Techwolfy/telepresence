/** 
 * @file  nano_time.h
 *
 * $Id: $
 *
 * @author  Team 691
 * @date    March 01, 2010
 *
 * A set of nanosecond time utilities
 *
 */ 
#ifndef  NANO_TIME_H
#define  NANO_TIME_H

/*
 * Includes
 */
//#include <string.h>
#include <iostream>
#include <limits.h>
#include <time.h>

/*
 * Defines
 */
#define MAX_64_BIT (LLONG_MAX)
/*
 *        CLOCK_REALTIME
 *               Systemwide realtime clock.
 *
 *        CLOCK_MONOTONIC
 *               Represents monotonic time. Cannot be set.
 *
 *        CLOCK_PROCESS_CPUTIME_ID
 *               High resolution per-process timer.
 *
 *        CLOCK_THREAD_CPUTIME_ID
 *               Thread-specific timer.
 *
 *        CLOCK_REALTIME_HR
 *               High resolution version of CLOCK_REALTIME.
 *
 *        CLOCK_MONOTONIC_HR
 *               High resolution version of CLOCK_MONOTONIC.
 */
#define LOOP_CLOCK (CLOCK_REALTIME)



/*
 * Typedefs
 */

typedef long long int nano_time;


class NanoTime
{

public:

  /** @{ @name Constructors/destructors */
  NanoTime();
  bool init();

  /** @} */

  /** @{ @name Time functions */
  nano_time get_time_nsec();  
  bool nano_sleep(nano_time nsec);          /* Sleep for the specified time */
  bool nano_sleep_until(nano_time wakeup);  /* Sleep until the specified time */


  /** @} */

  /** @{ @name Mutators */


  /** @} */

protected:
  timespec ts;                 /* A timespec structure */
  nano_time sleep_time;        /* How long is this sleep */

private:

};

#endif // NANO_TIME_H
