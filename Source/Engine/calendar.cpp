/**\file			calendar.cpp
 * \author		Christopher Thielen (chris@epiar.net)
 * \date			Created: Sunday, June 24, 2012
 * \date			Modified: Sunday, June 24, 2012
 * \brief
 * \details
 */

#include "includes.h"
#include "Engine/calendar.h"
#include "Engine/hud.h"

/**\class Calendar
 * \brief A stardate system.
 */

/**\brief Initializes a new Calendar to default values.
 *
 */
Calendar::Calendar() :
	period(1),
	epoch(3002)
{
  ticker = 0;
}

string Calendar::Now(void) {
  stringstream ret;
  
  ret << "Period " << period << " of Epoch " << epoch;
    
  return ret.str();
}

/**\brief To be called once per logic frame update. Advances the calendar after a certain number of ticks.
 *
 */
void Calendar::Update(void) {
  int old_period = period;
  int old_epoch = epoch;
  
  ticker++;
  
  if(ticker > LOGIC_FRAMES_PER_PERIOD) {
    ticker = 0;
    period++;
  }
  
  AdjustEpoch();
  
  if((old_period != period) || (old_epoch != epoch)) {
    Hud::Alert("Day changed to %s", Now().c_str());
  }
}

/**\brief Advances the date based on how far a jump was (distance).
 *
 */
void Calendar::AdvanceFromJump(float distance) {
  int old_period = period;
  int old_epoch = epoch;

  int periods = (int)(((float)(distance / QUADRANTSIZE) * (float)(distance / QUADRANTSIZE)) / 2.);
  
  period += periods;
  
  AdjustEpoch();

  if((old_period != period) || (old_epoch != epoch)) {  
    Hud::Alert("Day changed to %s", Now().c_str());
  }
}

void Calendar::AdvanceFromLand() {
  period++;
  
  AdjustEpoch();
  
  Hud::Alert("Day changed to %s", Now().c_str());
}

void Calendar::AdjustEpoch() {
  while(period > PERIODS_PER_EPOCH) {
    period -= PERIODS_PER_EPOCH;
    epoch++;
  }
}
