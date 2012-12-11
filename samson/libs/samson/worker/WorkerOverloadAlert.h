

#include "au/containers/map.h"
#include "au/statistics/Cronometer.h"

namespace samson {
/**
 * \brief Information about an alert received from a worker
 */

class WorkerOverloadAlert {
public:

  WorkerOverloadAlert(double time) {
    time_ = time;
  }

  bool timeout() {
    return cronometer_.seconds() > time_;
  }

  double seconds() {
    double time = cronometer_.seconds();

    if (time > time_) {
      return 0;
    } else {
      return time_ - time;
    }
  }

private:

  au::Cronometer cronometer_;
  double time_;
};

/**
 * \brief Class to accumulate overload - alerts received from workers
 * If an alert is received from a worker, no more BloqueRequests are sent to this worker during some time
 */

class WorkerOverloadAlerts {
public:

  /**
   * \brief Add an alert received from a worker
   */
  void AddAlert(size_t worker_id, double seconds);

  /**
   * \brief Check if I can send a request to this worker
   */
  bool CheckWorker(size_t worker_id);

private:
  au::map<size_t, WorkerOverloadAlert> alerts_;
};
}