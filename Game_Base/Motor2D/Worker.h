#ifndef _WORKER_H_
#define _WORKER_H_

#include "Actions.h"
#include <queue>

/// Worker --------------------------------------
class Worker
{
public:

	Worker();
	~Worker();

private:

	std::priority_queue<Action*> actions;

public:

	//Actions Factory(multiple methods)
};
/// ---------------------------------------------
#endif // !_WORKER_H_
