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
  * Scheduler Job Manager object. This module serves the scheduler by providing
  * facilities about jobs ready to be maped, and their dependencies.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "scheduler/job_manager.h"

using namespace nimbus; // NOLINT

JobManager::JobManager() {
  processed_new_job_done_ = false;
  // Add the KERNEL job which is the parent of main, create and copy jobs that
  // are spawned by the scheduler.
  IDSet<job_id_t> job_id_set;
  IDSet<logical_data_id_t> logical_data_id_set;
  Parameter params;
  JobEntry* job = new JobEntry(JOB_SCHED, "kernel", NIMBUS_KERNEL_JOB_ID,
      NIMBUS_KERNEL_JOB_ID , true, true, true);
  if (!job_graph_.AddVertex(NIMBUS_KERNEL_JOB_ID, job)) {
    delete job;
    dbg(DBG_ERROR, "ERROR: could not add scheduler kernel job in job manager constructor.\n");
  } else {
    job->set_done(true);
  }
}

JobManager::~JobManager() {
  // TODO(omidm): do you need to call remove obsolete?
  JobEntry* job;
  if (JobManager::GetJobEntry(NIMBUS_KERNEL_JOB_ID, job)) {
    delete job;
  }
}

bool JobManager::AddJobEntry(const JobType& job_type,
    const std::string& job_name,
    const job_id_t& job_id,
    const IDSet<logical_data_id_t>& read_set,
    const IDSet<logical_data_id_t>& write_set,
    const IDSet<job_id_t>& before_set,
    const IDSet<job_id_t>& after_set,
    const job_id_t& parent_job_id,
    const Parameter& params,
    const bool& sterile) {
  JobEntry* job = new JobEntry(job_type, job_name, job_id, read_set, write_set,
      before_set, after_set, parent_job_id, params, sterile);

  if (!job_graph_.AddVertex(job_id, job)) {
    dbg(DBG_SCHED, "Filling possible future job (id: %lu) in job manager.\n", job_id);
    delete job;
    GetJobEntry(job_id, job);
    if (job->future()) {
      job->set_job_type(job_type);
      job->set_job_name(job_name);
      job->set_read_set(read_set);
      job->set_write_set(write_set);
      job->set_before_set(before_set);
      job->set_after_set(after_set);
      job->set_parent_job_id(parent_job_id);
      job->set_params(params);
      job->set_sterile(sterile);
      job->set_future(false);
      dbg(DBG_SCHED, "Filled the information for used to be future job (id: %lu).\n", job_id);
    } else {
      dbg(DBG_ERROR, "ERROR: could not add job (id: %lu) in job manager.\n", job_id);
      exit(-1);
      return false;
    }
  }

  bool completed_before_set_edges = true;

  IDSet<job_id_t>::ConstIter it;
  for (it = before_set.begin(); it != before_set.end(); ++it) {
    if (!job_graph_.AddEdge(*it, job_id)) {
      dbg(DBG_SCHED, "Adding possible future job (id: %lu) in job manager.\n", *it);
      AddFutureJobEntry(*it);
      if (!job_graph_.AddEdge(*it, job_id)) {
        completed_before_set_edges = false;
        break;
      }
    }
  }

  if (!completed_before_set_edges) {
    job_graph_.RemoveVertex(job_id);
    delete job;
    dbg(DBG_ERROR, "ERROR: could not add job (id: %lu) in job manager.\n", job_id);
    exit(-1);
    return false;
  }

  return true;
}

bool JobManager::AddJobEntry(const JobType& job_type,
    const std::string& job_name,
    const job_id_t& job_id,
    const job_id_t& parent_job_id,
    const bool& sterile,
    const bool& versioned,
    const bool& assigned) {
  JobEntry* job = new JobEntry(job_type, job_name, job_id, parent_job_id,
      sterile, versioned, assigned);

  if (!job_graph_.AddVertex(job_id, job)) {
    delete job;
    dbg(DBG_ERROR, "ERROR: could not add job (id: %lu) in job manager.\n", job_id);
    exit(-1);
    return false;
  }
  return true;
}

bool JobManager::AddFutureJobEntry(
    const job_id_t& job_id) {
  JobEntry* job = new JobEntry(job_id);
  if (!job_graph_.AddVertex(job_id, job)) {
    delete job;
    dbg(DBG_ERROR, "ERROR: could not add job (id: %lu) in job manager as future job.\n", job_id);
    exit(-1);
    return false;
  }
  return true;
}


bool JobManager::GetJobEntry(job_id_t job_id, JobEntry*& job) {
  Vertex<JobEntry, job_id_t>* vertex;
  if (job_graph_.GetVertex(job_id, &vertex)) {
    job = vertex->entry();
    return true;
  } else {
    job = NULL;
    return false;
  }
}

bool JobManager::RemoveJobEntry(JobEntry* job) {
  if (job_graph_.RemoveVertex(job->job_id())) {
    // version_manager_.RemoveJobVersionTables(job);
    delete job;
    return true;
  } else {
    return false;
  }
}

bool JobManager::RemoveJobEntry(job_id_t job_id) {
  JobEntry* job;
  if (GetJobEntry(job_id, job)) {
    assert(job_id == job->job_id());
    job_graph_.RemoveVertex(job_id);
    // version_manager_.RemoveJobVersionTables(job);
    delete job;
    return true;
  } else {
    return false;
  }
}

size_t JobManager::GetJobsReadyToAssign(JobEntryList* list, size_t max_num) {
  while (ResolveVersions() > 0) {
    continue;
  }

  size_t num = 0;
  list->clear();
  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
  for (; (iter != job_graph_.end()) && (num < max_num); ++iter) {
    Vertex<JobEntry, job_id_t>* vertex = iter->second;
    JobEntry* job = vertex->entry();
    if (job->versioned() && !job->assigned()) {
      // Job is already versioned so it has the information from parent and
      // beforeset already, they may not be in the graph at this point though. -omidm
      JobEntry* j;

      if (GetJobEntry(job->parent_job_id(), j)) {
        if (!(j->done())) {
          continue;
        }
      }

      bool before_set_done_or_sterile = true;

      Edge<JobEntry, job_id_t>::Iter it;
      typename Edge<JobEntry, job_id_t>::Map* incoming_edges = vertex->incoming_edges();
      for (it = incoming_edges->begin(); it != incoming_edges->end(); ++it) {
        j = it->second->start_vertex()->entry();
        /* 
         * Due to the current graph traversal we should only assign the job if
         * before set is done otherwise by leveraging the sterile flag we could
         * end up flooding worker with lots of jobs that depend on a job that
         * has not been assigned yet and so it causes a lot of latency. the
         * graph traversal right now is based on iterator of map (so job ids)
         * we need to traverse based on graph shape. In addition to efficiency
         * issues it could cause problem since the before set may not be
         * assigned yet and so we may not find the data version for the job in
         * the system. -omidm
         */
         // if (!(j->done())) {
        /* 
         * For now and sake of current water multiple since the number of jobs
         * are not too large and we have huge number of data partitions the
         * application would benefit if scheduler can use the sterile flag and
         * scheduler jobs in advance. Note that since still the job ids may not
         * be in order we have to make sure that the jobs in before set are
         * already assigned, otherwise we may not find the data version we want
         * for the job in the system. -omidm
         */
        if (!(j->done()) && !(j->sterile() && j->assigned())) {
        /*
         * The ultimate goal is to turn it to this after we have built the
         * graph traversal, since the job in before set is assigned for sure
         * before the job in after set if we travers properly. -omidm
         */
        // if (!(j->done()) && !(j->sterile())) {
          before_set_done_or_sterile = false;
          break;
        }
      }

      if (before_set_done_or_sterile) {
        // job->set_assigned(true); No, we are not sure yet thet it will be assignd!
        list->push_back(job);
        ++num;
      }
    }
  }
  return num;
}

size_t JobManager::RemoveObsoleteJobEntries() {
  if (!processed_new_job_done_) {
    return 0;
  }

  while (ResolveVersions() > 0) {
    continue;
  }

  size_t num = 0;
  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
  for (; (iter != job_graph_.end());) {
    JobEntry* job = iter->second->entry();
    job_id_t job_id = job->job_id();
    ++iter;
    if (job->done() && (job_id != NIMBUS_KERNEL_JOB_ID)) {
      RemoveJobEntry(job);
      dbg(DBG_SCHED, "removed job with id %lu from job manager.\n", job_id);
      ++num;
    }
  }
  return num;
}

void JobManager::JobDone(job_id_t job_id) {
  JobEntry* job;
  if (GetJobEntry(job_id, job)) {
    job->set_done(true);
    processed_new_job_done_ = true;
  } else {
    dbg(DBG_WARN, "WARNING: done job with id %lu is not in the graph.\n", job_id);
  }
}

void JobManager::DefineData(job_id_t job_id, logical_data_id_t ldid) {
//  JobEntry* job;
//  if (GetJobEntry(job_id, job)) {
//    JobEntry::VersionTable vt;
//    vt = job->version_table_out();
//    JobEntry::VTIter it = vt.begin();
//    bool new_logical_data = true;
//    for (; it != vt.end(); ++it) {
//      if (it->first == ldid) {
//        new_logical_data = false;
//        dbg(DBG_ERROR, "ERROR: defining logical data id %lu, which already exist.\n", ldid);
//        break;
//      }
//    }
//    if (new_logical_data) {
//      vt[ldid] = (data_version_t)(0);
//      job->set_version_table_out(vt);
//      // version_manager_.AddVersionEntry(ldid, NIMBUS_INIT_DATA_VERSION, job, VersionEntry::OUT);
//    }
//  } else {
//    dbg(DBG_WARN, "WARNING: parent of define data with job id %lu is not in the graph.\n", job_id); // NOLINT
//  }


  JobEntry* job;
  if (GetJobEntry(job_id, job)) {
    data_version_t version;
    if (!job->vtable_out()->query_entry(ldid, &version)) {
      job->vtable_out()->set_entry(ldid, NIMBUS_INIT_DATA_VERSION);
    } else {
      dbg(DBG_ERROR, "ERROR: defining logical data id %lu, which already exist.\n", ldid);
      exit(-1);
    }
  } else {
    dbg(DBG_ERROR, "ERROR: parent of define data with job id %lu is not in the graph.\n", job_id);
    exit(-1);
  }
}

bool JobManager::ResolveJobDataVersions(JobEntry* job) {
  if (job->future()) {
    return false;
  }

  if (job->versioned()) {
    return true;
  }

  IDSet<job_id_t> need = job->need_set();
  IDSet<job_id_t>::IDSetIter iter;

  Log log;
  log.StartTimer();
  size_t need_count = need.size();
  size_t remain_count = need_count;

  std::vector<boost::shared_ptr<const VersionTable> > tables;
  if (job->partial_versioned()) {
    tables.push_back(job->vtable_in());
  }
  for (iter = need.begin(); iter != need.end(); ++iter) {
    job_id_t id = (*iter);
    JobEntry* j;
    if (GetJobEntry(id, j)) {
      if (j->versioned()) {
        tables.push_back(j->vtable_out());
        job->add_job_passed_versions(id);
        job->set_partial_versioned(true);
        --remain_count;
      }
    } else {
      dbg(DBG_ERROR, "ERROR: Job in need set (id: %lu) is not in the graph.\n", id);
      exit(-1);
      return false;
    }
  }

  if (tables.size() > 0) {
    boost::shared_ptr<VersionTable> merged;
    version_operator_.MergeVersionTables(tables, &merged);
    job->set_vtable_in(merged);
  }

  if (remain_count == 0) {
    if (!job->sterile()) {
      boost::shared_ptr<VersionTable> merged;
      std::vector<boost::shared_ptr<VersionTable> > table_vec;
      table_vec.push_back(job->vtable_in());
      version_operator_.RecomputeRootForVersionTables(table_vec);
    }
    boost::shared_ptr<VersionTable> table_out;
    version_operator_.MakeVersionTableOut(job->vtable_in(), job->write_set(), &table_out);
    job->set_vtable_out(table_out);
    job->set_versioned(true);
    log.StopTimer();
    dbg(DBG_SCHED, "Versioning for %s took %f.\n", job->job_name().c_str(), log.timer());
    return true;
  }

  return false;


//  IDSet<job_id_t> need = job->need_set();
//  IDSet<job_id_t>::IDSetIter iter;
//
//  Log log;
//  log.StartTimer();
//  size_t need_count = need.size();
//  size_t remain_count = need_count;
//
//  for (iter = need.begin(); iter != need.end(); ++iter) {
//    job_id_t id = (*iter);
//    JobEntry* j;
//    if (GetJobEntry(id, j)) {
//      if (j->versioned()) {
//        const JobEntry::VersionTable *vt = j->version_table_out_p();
//        JobEntry::ConstVTIter it;
//        for (it = vt->begin(); it != vt->end(); ++it) {
//          if (job->version_table_in_p()->count(it->first) == 0) {
//            // version_manager_.AddVersionEntry(it->first, it->second, job, VersionEntry::IN);
//            job->set_version_table_in_entry(it->first, it->second);
//          } else if ((it->second) > (job->version_table_in_query(it->first))) {
//            // version_manager_.RemoveVersionEntry(
//            //     it->first, job->version_table_in_query(it->first), job, VersionEntry::IN);
//            // version_manager_.AddVersionEntry(it->first, it->second, job, VersionEntry::IN);
//            job->set_version_table_in_entry(it->first, it->second);
//          }
//        }
//        job->add_job_passed_versions(id);
//        job->set_partial_versioned(true);
//        remain_count--;
//      } else {
//        dbg(DBG_SCHED, "Job in need set (id: %lu) is not versioned yet.\n", id);
//        return false;
//      }
//    } else {
//      dbg(DBG_ERROR, "ERROR: Job in need set (id: %lu) is not in the graph.\n", id);
//      return false;
//    }
//  }
//
//  if (remain_count == 0) {
//    if (job->build_version_table_out_and_check()) {
//      // version_manager_.AddJobVersionTableOut(job);
//      job->set_versioned(true);
//      log.StopTimer();
//      std::cout << "Old Versioning System: " << job->job_name() << " " << log.timer() << std::endl; // NOLINT
//      return true;
//    }
//  }
//  log.StopTimer();
//  std::cout << "Old Versioning System: " << job->job_name() << " " << log.timer() << std::endl;
//
//  return false;
}

size_t JobManager::ResolveVersions() {
  size_t num_new_versioned = 0;

  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
  for (; iter != job_graph_.end(); ++iter) {
    if ((!iter->second->entry()->versioned())) {
      if (ResolveJobDataVersions(iter->second->entry()))
        ++num_new_versioned;
    }
  }
  return num_new_versioned;
}


size_t JobManager::GetJobsNeedDataVersion(JobEntryList* list,
    VersionedLogicalData vld) {
  /*
   * Version manager approach.
   */
//  return version_manager_.GetJobsNeedDataVersion(list, vld);

  /*
   * Job graph aproach with old version table
   */
//  size_t num = 0;
//  list->clear();
//  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
//  for (; iter != job_graph_.end(); ++iter) {
//    JobEntry* job = iter->second->entry();
//    if ((job->versioned() || job->partial_versioned()) && !job->assigned()) {
//      if (job->version_table_in_p()->count(vld.first) != 0) {
//        if ((job->version_table_in_query(vld.first) == vld.second) &&
//            ((job->read_set_p()->contains(vld.first)) || !(job->sterile()))) {
//          list->push_back(job);
//          ++num;
//        }
//      }
//    }
//  }
//  return num;


  /*
   * Job graph aproach with new version table
   */
  size_t num = 0;
  list->clear();
  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
  for (; iter != job_graph_.end(); ++iter) {
    JobEntry* job = iter->second->entry();
    if ((job->versioned() || job->partial_versioned()) && !job->assigned()) {
      data_version_t version;
      if (job->vtable_in()->query_entry(vld.first, &version)) {
        if ((version == vld.second) &&
            ((job->read_set_p()->contains(vld.first)) || !(job->sterile()))) {
          list->push_back(job);
          ++num;
        }
      }
    }
  }
  return num;

  /*
   * Obsolete slow approach
   *
  size_t num = 0;
  list->clear();
  JobGraph::Iter iter = job_graph_.Begin();
  for (; iter != job_graph_.End(); ++iter) {
    JobEntry* job = iter->second;
    if (job->versioned() && !job->assigned()) {
      JobEntry::VersionTable version_table_in = job->version_table_in();
      if (version_table_in.count(vld.first) != 0) {
        if ((version_table_in[vld.first] == vld.second)) {
            // && (job->union_set().contains(vld.first))) {
            // Since job could be productive it does not need to read or writ it.
          list->push_back(job);
          ++num;
        }
      }
    }
  }
  return num;
  */
}

bool JobManager::AllJobsAreDone() {
  bool all_done = true;

  typename Vertex<JobEntry, job_id_t>::Iter iter = job_graph_.begin();
  for (; iter != job_graph_.end(); ++iter) {
    JobEntry* job = iter->second->entry();
    if (!job->done()) {
      all_done = false;
      break;
    }
  }
  return all_done;
}

void JobManager::UpdateJobBeforeSet(JobEntry* job) {
  IDSet<job_id_t> before_set = job->before_set();
  UpdateBeforeSet(&before_set);
  job->set_before_set(before_set);
}

void JobManager::UpdateBeforeSet(IDSet<job_id_t>* before_set) {
  IDSet<job_id_t>::IDSetIter it;
  for (it = before_set->begin(); it != before_set->end();) {
    JobEntry* j;
    job_id_t id = *it;
    if (GetJobEntry(id, j)) {
      if ((j->done()) || (id == NIMBUS_KERNEL_JOB_ID)) {
        before_set->remove(it++);
      } else {
        ++it;
      }
    } else {
      // if the job is not in the table it is already done and removed.
      before_set->remove(it++);
    }
  }
}


size_t JobManager::ResolveDataVersions() {
  size_t num = 0;
  std::map<job_id_t, JobEntryList> new_pass_version;
  std::map<job_id_t, JobEntryList>::iterator iter;
  for (iter = pass_version_.begin(); iter != pass_version_.end(); ++iter) {
    JobEntry *job;
    job_id_t job_id = iter->first;
    if (GetJobEntry(job_id, job)) {
      PassDataVersionToJob(job, iter->second);
      jobs_need_versioin_[job_id] = job;
      if (JobVersionIsComplete(job)) {
        job->set_versioned(true);
        Vertex<JobEntry, job_id_t>* vertex;
        job_graph_.GetVertex(job_id, &vertex);
        typename Edge<JobEntry, job_id_t>::Iter it;
        for (it = vertex->outgoing_edges()->begin();
            it != vertex->outgoing_edges()->end(); ++it) {
          new_pass_version[it->second->end_vertex()->entry()->job_id()].push_back(job);
        }
        ++num;
      }
    } else {
      dbg(DBG_ERROR, "ERROR: Job (id: %lu) is not in the graph to receive the versions.\n", iter->first); // NOLINT
      exit(-1);
    }
  }

  pass_version_ = new_pass_version;
  return num;
}

void JobManager::PassDataVersionToJob(
    JobEntry *job, const JobEntryList& source_jobs) {
  assert(!job->future() && !job->versioned());
  assert(source_jobs.size() > 0);

  std::vector<boost::shared_ptr<const VersionTable> > tables;
  if (job->partial_versioned()) {
    tables.push_back(job->vtable_in());
  }

  JobEntryList::const_iterator iter;
  for (iter = source_jobs.begin(); iter != source_jobs.end(); ++iter) {
    JobEntry* j = (*iter);
    assert(j->versioned());
    tables.push_back(j->vtable_out());
    job->add_job_passed_versions(j->job_id());
  }

  boost::shared_ptr<VersionTable> merged;
  version_operator_.MergeVersionTables(tables, &merged);
  job->set_vtable_in(merged);
  job->set_partial_versioned(true);
}

bool JobManager::JobVersionIsComplete(JobEntry *job) {
  IDSet<job_id_t> need = job->need_set();
  return (need.size() == 0);
}

size_t JobManager::ExploreToAssignJobs() {
  // TODO(omidm): Implement!
  return 0;
}

void JobManager::RemoveJobAssignmentDependency(
    JobEntry *job, const JobEntryList& from_jobs) {
  // TODO(omidm): Implement!
}

bool JobManager::JobIsReadyToAssign() {
  // TODO(omidm): Implement!
  return false;
}








