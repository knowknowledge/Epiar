/**\file			calendar.h
 * \author		Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 24, 2012
 * \date			Modified: Sunday, June 24, 2012
 * \brief
 * \details
 *            This represents "space time". The idea is
 *            simply days and years. A period is a day and
 *            an epoch is a year. There are 1000 periods to an epoch.
 *            The game starts on Period 1 of Epoch 3002 (arbitrary lore).
 */

#ifndef __h_calendar__
#define __h_calendar__

#include "includes.h"
#include "Utilities/quadtree.h"
#include "Utilities/timer.h"

#define SECONDS_PER_PERIOD      45
#define LOGIC_FRAMES_PER_PERIOD (LOGIC_FPS * SECONDS_PER_PERIOD)
#define PERIODS_PER_EPOCH       1000

// Abstraction of a single calendar
class Calendar {
	public:
		Calendar();
    
    void Update(void);    
    void AdvanceFromJump(float distance);
    void AdvanceFromLand();
    
    string Now(void);
    
    int GetPeriod(void) { return period; }
    int GetEpoch(void) { return epoch; }
    
  private:
    int period, epoch;
    int ticker;
    
    void AdjustEpoch();
};

#endif // __h_calendar__
