#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;
	void StartFrom(uint32 secs);

private:
	uint32	started_at;
};

#endif //__TIMER_H__