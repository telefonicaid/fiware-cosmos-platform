
#include <iomanip>
#include <iostream>


#include "ThreadManager.h"  // Own interface
#include "au/S.h"


namespace au {
void ThreadManager::wait_all_threads(std::string title) {
  au::Singleton<au::ThreadManager>::shared()->wait(title);
}

ThreadManager::ThreadManager() : token_("ThreadManager") {
  for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
    threads_[i] = NULL;
  }
}

int ThreadManager::addThread(std::string thread_name, pthread_t *__restrict t, const pthread_attr_t *__restrict attr_t,
                             thread_function f,
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
    AddThreads(thread_info);
  } else {
    LM_W(("Not possible to create thread %s %d ", thread_name.c_str(), s));
    delete thread_info;
  }

  return s;
}

int ThreadManager::addNonDetachedThread(std::string thread_name, pthread_t *__restrict t,
                                        const pthread_attr_t *__restrict attr_t,
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
    AddThreads(thread_info);
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
  RemoveThreads(thread_info);
}

au::StringVector ThreadManager::getThreadNames() {
  // Mutex protection
  au::TokenTaker tt(&token_);

  au::StringVector names;

  for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
    if (threads_[i] != NULL) {
      names.push_back(threads_[i]->name_);
    }
  }
  return names;
}

std::string ThreadManager::str() {
  // Mutex protection
  au::TokenTaker tt(&token_);


  std::ostringstream o;

  o << "Running threads\n";
  o << "------------------------------------\n";
  std::set< ThreadInfo * >::iterator it_threads;
  for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
    if (threads_[i] != NULL) {
      o << threads_[i]->name_ << " " << threads_[i]->cronometer_ << "\n";
    }
  }

  return o.str();
}

void ThreadManager::wait(std::string title) {
  // std::cerr << "Waiting all threads to finish\n";

  au::Cronometer cronometer;

  while (true) {
    {
      au::TokenTaker tt(&token_);

      if (num_threads() == 0) {
        // std::cerr << "All threads finished\n";
        return;
      }

      if (cronometer.seconds() > 5) {
        LM_W(("%s: Waiting all threads to finish", title.c_str()));
        LM_W(("Stll running %d threads", num_threads()));
        LM_W((str().c_str()));
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
  // Detach myself.... no one is waiting for me...
  pthread_detach(pthread_self());

  ThreadInfo *thread_info = (ThreadInfo *)p;

  LM_VV(("Running thread %s", thread_info->name_.c_str()));

  // Execute the real function
  void *ans = thread_info->f_(thread_info->p_);

  // Notify my ThreadRunner
  au::Singleton<au::ThreadManager>::shared()->notify_finish_thread(thread_info);

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
  au::Singleton<au::ThreadManager>::shared()->notify_finish_thread(thread_info);

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
