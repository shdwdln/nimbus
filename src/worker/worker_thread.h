/*
 * Copyright 2013 Stanford University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of the copyright holders nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /*
  * The abstraction of a worker thread. A worker thread keeps pulling job from
  * the worker manager and executes them.
  * Now, they are implemented as busy-waiting.
  *
  * Author: Hang Qu <quhang@stanford.edu>
  */

#ifndef NIMBUS_SRC_WORKER_WORKER_THREAD_H_
#define NIMBUS_SRC_WORKER_WORKER_THREAD_H_

#include <pthread.h>

#include <list>

#include "src/shared/nimbus.h"
#include "src/shared/high_resolution_timer.h"
#include "src/worker/task_thread_pool.h"

namespace nimbus {
class WorkerManager;
class WorkerThread {
 public:
  explicit WorkerThread(WorkerManager* worker_manager);
  virtual ~WorkerThread();
  virtual void Run() = 0;

  pthread_t thread_id;
  TaskThreadPool::TaskThreadList allocated_threads;
#ifndef __MACH__
  virtual void SetThreadAffinity(const cpu_set_t* cpuset);
#endif

 protected:
#ifndef __MACH__
  cpu_set_t* used_cpu_set_;
#endif
  WorkerManager* worker_manager_;
};
}  // namespace nimbus

#endif  // NIMBUS_SRC_WORKER_WORKER_THREAD_H_
