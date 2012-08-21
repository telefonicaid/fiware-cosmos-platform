
#include <iomanip>
#include <iostream>


#include "ThreadManager.h"  // Own interface
#include "au/S.h"


namespace au {
std::ostream& operator<<(std::ostream& o, const ThreadInfo& thread_info) {
  o << std::setw(60) << thread_info.name_ << " " << thread_info.cronometer_;
  return o;
}

ThreadManager *thread_manager;

ThreadManager *ThreadManager::shared() {
  if (!thread_manager)
    thread_manager = new ThreadManager(); return thread_manager;
}

void ThreadManager::wait_all_threads(std::string title) {
  thread_manager->wait(title);
}

ThreadManager::ThreadManager() : token_("ThreadManager") {
}

int ThreadManager::addThread(std::string thread_name, pthread_t *__restrict t, const pthread_attr_t *__restrict attr_t, thread_function f,
                             void *__restrict p) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Create a thread info structure
  ThreadInfo *thread_info = new ThreadInfo(thread_name, f, p);

  // Create the thread with our "run_ThreadInfo" function
  int s = pthread_create(t, attr_t, run_ThreadInfo, thread_info);

  // Record the thread id
  thread_info->t_ = *t;

  if (s == 0) {
    LM_T(LmtThreadManager, ("Thread '%s' created and inserted in map", thread_name.c_str()));
    threads_.insertInMap(thread_info->t_, thread_info);
  } else {
    LM_W(("Not possible to create thread %s %d ", thread_name.c_str(), s));
    delete thread_info;
  }

  return s;
}

int ThreadManager::addNonDetachedThread(std::string thread_name, pthread_t *__restrict t, const pthread_attr_t *__restrict attr_t,
                                        thread_function f,
                                        void *__restrict p) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  // Create a thread info structure
  ThreadInfo *thread_info = new ThreadInfo(thread_name, f, p);

  // Create the thread with our "run_ThreadInfo" function
  int s = pthread_create(t, attr_t, run_NonDetachedThreadInfo, thread_info);

  // Record the thread id
  thread_info->t_ = *t;

  if (s == 0) {
    LM_T(LmtThreadManager, ("Thread '%s' created and inserted in map", thread_name.c_str()));
    threads_.insertInMap(thread_info->t_, thread_info);
  } else {
    LM_W(("Not possible to create thread %s %d ", thread_name.c_str(), s));
    delete thread_info;
  }

  return s;
}

void ThreadManager::notify_finish_thread(ThreadInfo *thread_info) {
  // Mutex protection
  au::TokenTaker tt(&token_);

  LM_T(LmtThreadManager, ("Thread '%s' extracted from map", thread_info->name_.c_str()));

  if (threads_.extractFromMap(thread_info->t_) == NULL)
    LM_X(1, ("Error in ThreadManager"));
}

int ThreadManager::getNumThreads() {
  // Mutex protection
  au::TokenTaker tt(&token_);

  return threads_.size();
}

au::StringVector ThreadManager::getThreadNames() {
  // Mutex protection
  au::TokenTaker tt(&token_);

  au::StringVector names;

  au::map< pthread_t, ThreadInfo >::iterator it_threads;
  for (it_threads = threads_.begin(); it_threads != threads_.end(); it_threads++) {
    names.push_back(it_threads->second->name_);
  }

  return names;
}

std::ostream& operator<<(std::ostream& o, ThreadManager& thread_manager) {
  // Mutex protection
  au::TokenTaker tt(&thread_manager.token_);

  o << "Running threads\n";
  o << "------------------------------------\n";
  au::map< pthread_t, ThreadInfo >::const_iterator it_threads;
  for (it_threads = thread_manager.threads_.begin()
       ; it_threads != thread_manager.threads_.end()
       ; it_threads++)
  {
    o << *it_threads->second << "\n";
  }

  return o;
}

void ThreadManager::wait(std::string title) {
  // std::cerr << "Waiting all threads to finish\n";

  au::Cronometer cronometer;

  while (true) {
    {
      au::TokenTaker tt(&token_);

      if (threads_.size() == 0)
        // std::cerr << "All threads finished\n";
        return;

      if (threads_.size() == 1)
        if (threads_.begin()->second->t_ == pthread_self())
          // std::cerr << "All threads finished\n";
          return;

      if (cronometer.seconds() > 5) {
        LM_W(("%s: Waiting all threads to finish", title.c_str()));
        LM_W(("Stll running %d threads", threads_.size()));
        std::ostringstream str;
        str << *this;
        LM_W((str.str().c_str()));

        std::cerr << title << ": Waiting all threads to finish\n";
        std::cerr << *this << "\n";
        std::cerr << au::str("Still running %d threads ...\n", threads_.size());

        cronometer.Reset();
      }
    }

    usleep(100000);
  }
}

// --------------------------------------------------------------------------------
// Function to run a thread info
// --------------------------------------------------------------------------------

void *run_ThreadInfo(void *p) {
  // Detach myself.... noone is waiting for me...
  pthread_detach(pthread_self());

  ThreadInfo *thread_info = (ThreadInfo *)p;

  LM_VV(("Running thread %s", thread_info->name_.c_str()));

  // Execute the real function
  void *ans = thread_info->f_(thread_info->p_);

  // Notify my ThreadRunner
  ThreadManager::shared()->notify_finish_thread(thread_info);

  LM_VV(("Finished thread %s", thread_info->name_.c_str()));

  // Delete this structure
  delete thread_info;

  return ans;
}

void *run_NonDetachedThreadInfo(void *p) {
  ThreadInfo *thread_info = (ThreadInfo *)p;

  LM_VV(("Running thread %s", thread_info->name_.c_str()));

  // Execute the real function
  void *ans = thread_info->f_(thread_info->p_);

  // Notify my ThreadRunner
  ThreadManager::shared()->notify_finish_thread(thread_info);

  LM_VV(("Finished thread %s", thread_info->name_.c_str()));

  // Delete this structure
  delete thread_info;

  return ans;
}

void *run_Thread(void *p) {
  Thread *t = (Thread *)p;

  t->run();
  t->pthread_running_ = false;
  return NULL;
}
}
