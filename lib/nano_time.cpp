/** 
 * @file  nano_time.cc
 *
 * $Id: $
 *
 * @author  Team 691
 * @date    March 01, 2010
 *
 * A set of nanosecond time utilities
 *
 */ 


#include "nano_time.h"

using namespace std;

NanoTime::NanoTime()
{

}

bool NanoTime::init()
{

  nano_time nsec = 0;

  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  clock_getres(LOOP_CLOCK, &ts);

  nsec = ts.tv_sec * 1000000000ll + ts.tv_nsec;

  cout << "Clock resolution = " << nsec << endl;

  return true;
}

/**
 * Get the epoch time in nanoseconds.  Convert a timespec to
 * a single 64 bit int to simplify math.
 *
 * @return      The current time with the best resolution available
 */
nano_time NanoTime::get_time_nsec()
{
  nano_time nsec = 0;

  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  int rc = clock_gettime(LOOP_CLOCK, &ts);

  nsec = ts.tv_sec * 1000000000ll + ts.tv_nsec;

  if(rc)
    {
      cout << "Error getting clocktime, rc = " << rc 
      << ", ts.tv_sec = " << ts.tv_sec << endl;
    }
  
  return nsec;
}

/**
 * Sleep for the specified time.  Use a non-blocking sleep
 * implementation
 *
 * @param[in]   nsec - number of nanoseconds to sleep
 * @return      true if OK, otherwise false
 */
bool NanoTime::nano_sleep(nano_time nsec)
{
  /*
   * Set up the ts struct
   */

  sleep_time = nsec;
  ts.tv_sec = nsec/1000000000ll;
  ts.tv_nsec = nsec % 1000000000ll;
  //  clock_nanosleep(LOOP_CLOCK, 0, &ts, NULL);
  nanosleep(&ts, NULL);

  return true;
}

/**
 * Sleep until the specified time.  Use a non-blocking sleep
 * implementation
 *
 * @param[in]   wakeup - wakeup time
 * @return      true if OK, otherwise false
 */
bool NanoTime::nano_sleep_until(nano_time wakeup)
{
  /*
   * Set up the ts struct
   */

  sleep_time = wakeup - get_time_nsec();
  ts.tv_sec = wakeup/1000000000ll;
  ts.tv_nsec = wakeup % 1000000000ll;
  clock_nanosleep(LOOP_CLOCK, TIMER_ABSTIME, &ts, NULL);

  return true;
}
