

#include "WorkerOverloadAlert.h"  // Own interface

#include "au/log/LogMain.h"
#include "samson/common/Logs.h"

namespace samson {
void WorkerOverloadAlerts::AddAlert(size_t worker_id, double seconds) {
  WorkerOverloadAlert *alert = alerts_.findInMap(worker_id);

  if (alert) {
    if (alert->seconds() > seconds) {
      return;
    } else {
      delete alerts_.extractFromMap(worker_id);
    }
  }
  LOG_W(logs.worker, ("Overload-alert received from worker %lu. ( %f seconds )", worker_id, seconds ));
  alerts_.insertInMap(worker_id, new WorkerOverloadAlert(seconds));
}

/**
 * \brief Check if I can send a request to this worker
 */

bool WorkerOverloadAlerts::CheckWorker(size_t worker_id) {
  WorkerOverloadAlert *alert = alerts_.findInMap(worker_id);

  if (!alert) {
    return true;
  }

  if (alert->seconds() == 0) {
    delete alerts_.extractFromMap(worker_id);
    return true;
  }

  // There is an active alert from this worker
  return false;
}
}