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
  * Job entry in the job table of the job manager. Each entry holds the
  * meta data of the compute and copy jobs received by the scheduler.   
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "src/scheduler/job_entry.h"

using namespace nimbus; // NOLINT

JobEntry::JobEntry() {
  Initialize();
  job_depth_ = NIMBUS_INIT_JOB_DEPTH;
  sterile_ = false;
  memoize_ = false;
  memoize_binding_ = false;
  to_finalize_binding_template_ = false;
  template_job_ = NULL;
  binding_template_ = NULL;
  region_ = GeometricRegion();
  partial_versioned_ = false;
  versioned_ = false;
  versioned_for_pattern_ = false;
  versioned_entire_context_ = false;
  assigned_ = false;
  done_ = false;
  future_ = false;
  checkpoint_id_ = NIMBUS_INIT_CHECKPOINT_ID;
  read_region_valid_ = false;
  write_region_valid_ = false;
  scratch_region_valid_ = false;
  reduce_region_valid_ = false;
  union_region_valid_ = false;
  assigned_worker_ = NULL;
  parent_job_name_ = "";  // will be set for non-sterile and complex jobs
  grand_parent_job_name_ = "";  // will be set for complex jobs
  load_balancing_id_ = NIMBUS_INIT_LOAD_BALANCING_ID;  // will be set by load_balancer/job_manager
  parent_load_balancing_id_ = NIMBUS_INIT_LOAD_BALANCING_ID;  // will be set for complex jobs
}

void JobEntry::Initialize() {
    vmap_read_ = boost::shared_ptr<VersionMap>(new VersionMap());
    vmap_write_ = boost::shared_ptr<VersionMap>(new VersionMap());
    vmap_partial_ = boost::shared_ptr<VersionMap>(new VersionMap());
    meta_before_set_ =  boost::shared_ptr<MetaBeforeSet>(new MetaBeforeSet());
/*
  static boost::shared_ptr<VersionMap> empty_vmap =
    boost::shared_ptr<VersionMap>(new VersionMap());
  vmap_read_ = empty_vmap;
  vmap_write_ = empty_vmap;


  static boost::shared_ptr<MetaBeforeSet> empty_meta_before_set =
    boost::shared_ptr<MetaBeforeSet> (new MetaBeforeSet());
  meta_before_set_ = empty_meta_before_set;
*/
}

JobEntry::~JobEntry() {
}

JobType JobEntry::job_type() const {
  return job_type_;
}

std::string JobEntry::job_name() const {
  return job_name_;
}

std::string JobEntry::parent_job_name() const {
  return parent_job_name_;
}

std::string JobEntry::grand_parent_job_name() const {
  return grand_parent_job_name_;
}

job_id_t JobEntry::job_id() const {
  return job_id_;
}

IDSet<logical_data_id_t> JobEntry::read_set() const {
  return read_set_;
}

IDSet<logical_data_id_t> JobEntry::write_set() const {
  return write_set_;
}

IDSet<logical_data_id_t> JobEntry::scratch_set() const {
  return scratch_set_;
}

IDSet<logical_data_id_t> JobEntry::reduce_set() const {
  return reduce_set_;
}

IDSet<logical_data_id_t> JobEntry::union_set() const {
  return union_set_;
}

IDSet<job_id_t> JobEntry::before_set() const {
  return before_set_;
}

IDSet<job_id_t> JobEntry::after_set() const {
  return after_set_;
}

job_id_t JobEntry::parent_job_id() const {
  return parent_job_id_;
}

job_id_t JobEntry::future_job_id() const {
  return future_job_id_;
}

Parameter JobEntry::params() const {
  return params_;
}

CombinerMap JobEntry::combiner_map() const {
  return combiner_map_;
}

boost::shared_ptr<VersionMap> JobEntry::vmap_read() const {
  return vmap_read_;
}

boost::shared_ptr<VersionMap> JobEntry::vmap_write() const {
  return vmap_write_;
}

boost::shared_ptr<VersionMap> JobEntry::vmap_partial() const {
  return vmap_partial_;
}

boost::shared_ptr<MetaBeforeSet> JobEntry::meta_before_set() const {
  return meta_before_set_;
}

job_depth_t JobEntry::job_depth() const {
  return job_depth_;
}

JobEntry::PhysicalTable JobEntry::physical_table() const {
  return physical_table_;
}

IDSet<job_id_t> JobEntry::jobs_passed_versions() const {
  return jobs_passed_versions_;
}

IDSet<job_id_t> JobEntry::need_set() const {
  IDSet<job_id_t> need = before_set_;
  need.insert(parent_job_id_);
  need.remove(jobs_passed_versions_);
  return need;
}

worker_id_t JobEntry::assigned_worker_id() const {
  if (!assigned_) {
    return NIMBUS_SCHEDULER_ID;
  }
  return assigned_worker_id_;
}

SchedulerWorker* JobEntry::assigned_worker() const {
  return assigned_worker_;
}

bool JobEntry::sterile() const {
  return sterile_;
}

bool JobEntry::memoize() const {
  return memoize_;
}

bool JobEntry::memoize_binding() const {
  return memoize_binding_;
}

bool JobEntry::to_finalize_binding_template() const {
  return to_finalize_binding_template_;
}

TemplateJobEntry* JobEntry::template_job() const {
  return template_job_;
}

BindingTemplate* JobEntry::binding_template() const {
  return binding_template_;
}

GeometricRegion JobEntry::region() const {
  return region_;
}

bool JobEntry::partial_versioned() const {
  return partial_versioned_;
}

bool JobEntry::versioned() const {
  return versioned_;
}

bool JobEntry::versioned_for_pattern() const {
  return versioned_for_pattern_;
}

bool JobEntry::versioned_entire_context() const {
  return versioned_entire_context_;
}

bool JobEntry::assigned() const {
  return assigned_;
}

bool JobEntry::done() const {
  return done_;
}

bool JobEntry::future() const {
  return future_;
}

checkpoint_id_t JobEntry::checkpoint_id() const {
  return checkpoint_id_;
}

load_balancing_id_t JobEntry::load_balancing_id() const {
  return load_balancing_id_;
}

load_balancing_id_t JobEntry::parent_load_balancing_id() const {
  return parent_load_balancing_id_;
}

const IDSet<logical_data_id_t>* JobEntry::read_set_p() const {
  return &read_set_;
}

const IDSet<logical_data_id_t>* JobEntry::write_set_p() const {
  return &write_set_;
}

const IDSet<logical_data_id_t>* JobEntry::scratch_set_p() const {
  return &scratch_set_;
}

const IDSet<logical_data_id_t>* JobEntry::reduce_set_p() const {
  return &reduce_set_;
}

const IDSet<logical_data_id_t>* JobEntry::union_set_p() const {
  return &union_set_;
}

const CombinerMap* JobEntry::combiner_map_p() const {
  return &combiner_map_;
}

const IDSet<job_id_t>* JobEntry::before_set_p() const {
  return &before_set_;
}

IDSet<job_id_t>* JobEntry::before_set_p() {
  return &before_set_;
}

void JobEntry::set_job_type(JobType job_type) {
  job_type_ = job_type;
}

void JobEntry::set_job_name(std::string job_name) {
  job_name_ = job_name;
}

void JobEntry::set_parent_job_name(std::string parent_job_name) {
  parent_job_name_ = parent_job_name;
}

void JobEntry::set_grand_parent_job_name(std::string grand_parent_job_name) {
  grand_parent_job_name_ = grand_parent_job_name;
}

void JobEntry::set_job_id(job_id_t job_id) {
  job_id_ = job_id;
}

void JobEntry::set_combiner_map(const CombinerMap& combiner_map) {
  combiner_map_ = combiner_map;
}

void JobEntry::set_read_set(IDSet<logical_data_id_t> read_set) {
  union_set_.remove(read_set_);
  union_set_.insert(read_set);
  read_region_valid_ = false;
  union_region_valid_ = false;
  read_set_ = read_set;
}

void JobEntry::set_write_set(IDSet<logical_data_id_t> write_set) {
  union_set_.remove(write_set_);
  union_set_.insert(write_set);
  write_region_valid_ = false;
  union_region_valid_ = false;
  write_set_ = write_set;
}

void JobEntry::set_scratch_set(IDSet<logical_data_id_t> scratch_set) {
  union_set_.remove(scratch_set_);
  union_set_.insert(scratch_set);
  scratch_region_valid_ = false;
  union_region_valid_ = false;
  scratch_set_ = scratch_set;
}

void JobEntry::set_reduce_set(IDSet<logical_data_id_t> reduce_set) {
  union_set_.remove(reduce_set_);
  union_set_.insert(reduce_set);
  reduce_region_valid_ = false;
  union_region_valid_ = false;
  reduce_set_ = reduce_set;
}

void JobEntry::set_before_set(IDSet<job_id_t> before_set, bool update_dependencies) {
  if (update_dependencies) {
    assignment_dependencies_.remove(before_set_);
    assignment_dependencies_.insert(before_set);
    versioning_dependencies_ = assignment_dependencies_;
  }
  before_set_ = before_set;
}

void JobEntry::set_after_set(IDSet<job_id_t> after_set) {
  after_set_ = after_set;
}

void JobEntry::set_parent_job_id(job_id_t parent_job_id, bool update_dependencies) {
  if (update_dependencies) {
    assignment_dependencies_.remove(parent_job_id_);
    assignment_dependencies_.insert(parent_job_id);
    versioning_dependencies_ = assignment_dependencies_;
  }
  parent_job_id_ = parent_job_id;
}

void JobEntry::set_future_job_id(job_id_t future_job_id) {
  future_job_id_ = future_job_id;
}


void JobEntry::set_params(Parameter params) {
  params_ = params;
}

void JobEntry::set_vmap_read(boost::shared_ptr<VersionMap> vmap_read) {
  vmap_read_ = vmap_read;
}

void JobEntry::set_vmap_write(boost::shared_ptr<VersionMap> vmap_write) {
  vmap_write_ = vmap_write;
}

void JobEntry::set_vmap_partial(boost::shared_ptr<VersionMap> vmap_partial) {
  vmap_partial_ = vmap_partial;
}

void JobEntry::set_meta_before_set(boost::shared_ptr<MetaBeforeSet> meta_before_set) {
  meta_before_set_ = meta_before_set;
}

void JobEntry::set_job_depth(job_depth_t job_depth) {
  job_depth_ = job_depth;
  meta_before_set_->set_job_depth(job_depth);
}

void JobEntry::set_physical_table(PhysicalTable physical_table) {
  physical_table_ = physical_table;
}

void JobEntry::set_physical_table_entry(logical_data_id_t l_id, physical_data_id_t p_id) {
  physical_table_[l_id] = p_id;
}


void JobEntry::add_physical_reduce_set_entry(physical_data_id_t p_id) {
  physical_reduce_set_.insert(p_id);
}

void JobEntry::set_jobs_passed_versions(IDSet<job_id_t> jobs) {
  jobs_passed_versions_ = jobs;
}

void JobEntry::add_job_passed_versions(job_id_t job_id) {
  jobs_passed_versions_.insert(job_id);
}

void JobEntry::set_assigned_worker_id(worker_id_t assigned_worker_id) {
  assigned_worker_id_ = assigned_worker_id;
}

void JobEntry::set_assigned_worker(SchedulerWorker *assigned_worker) {
  assigned_worker_ = assigned_worker;
}

void JobEntry::set_sterile(bool flag) {
  sterile_ = flag;
}

void JobEntry::set_memoize(bool flag) {
  memoize_ = flag;
}

void JobEntry::set_memoize_binding(bool flag) {
  memoize_binding_ = flag;
}

void JobEntry::set_to_finalize_binding_template(bool flag) {
  to_finalize_binding_template_ = flag;
}

void JobEntry::set_template_job(TemplateJobEntry* template_job) {
  template_job_ = template_job;
}

void JobEntry::set_binding_template(BindingTemplate* binding_template) {
  binding_template_ = binding_template;
}

void JobEntry::set_region(GeometricRegion region) {
  region_ = region;
}

void JobEntry::set_partial_versioned(bool flag) {
  partial_versioned_ = flag;
}

void JobEntry::set_versioned(bool flag) {
  versioned_ = flag;
}

void JobEntry::set_versioned_for_pattern(bool flag) {
  versioned_for_pattern_ = flag;
}

void JobEntry::set_versioned_entire_context(bool flag) {
  versioned_entire_context_ = flag;
}

void JobEntry::set_assigned(bool flag) {
  assigned_ = flag;
}

void JobEntry::set_done(bool flag) {
  done_ = flag;
}

void JobEntry::set_future(bool flag) {
  future_ = flag;
}

void JobEntry::set_checkpoint_id(checkpoint_id_t checkpoint_id) {
  checkpoint_id_ = checkpoint_id;
}

void JobEntry::set_load_balancing_id(load_balancing_id_t lb_id) {
  load_balancing_id_ = lb_id;
}

void JobEntry::set_parent_load_balancing_id(load_balancing_id_t lb_id) {
  parent_load_balancing_id_ = lb_id;
}

bool JobEntry::GetPhysicalReadSet(IDSet<physical_data_id_t>* set) {
  set->clear();
  IDSet<logical_data_id_t>::IDSetIter it;
  for (it = read_set_.begin(); it != read_set_.end(); ++it) {
    PhysicalTable::iterator i = physical_table_.find(*it);
    if (i == physical_table_.end()) {
      dbg(DBG_ERROR, "ERROR: did not find physical element for ldid %lu in the read set!\n", *it);
      exit(-1);
      return false;
    }
    set->insert(i->second);
  }
  return true;
}

bool JobEntry::GetPhysicalWriteSet(IDSet<physical_data_id_t>* set) {
  set->clear();
  IDSet<logical_data_id_t>::IDSetIter it;
  for (it = write_set_.begin(); it != write_set_.end(); ++it) {
    PhysicalTable::iterator i = physical_table_.find(*it);
    if (i == physical_table_.end()) {
      dbg(DBG_ERROR, "ERROR: did not find physical element for ldid %lu in the write set!\n", *it);
      exit(-1);
      return false;
    }
    set->insert(i->second);
  }
  return true;
}

bool JobEntry::GetPhysicalScratchSet(IDSet<physical_data_id_t>* set) {
  set->clear();
  IDSet<logical_data_id_t>::IDSetIter it;
  for (it = scratch_set_.begin(); it != scratch_set_.end(); ++it) {
    PhysicalTable::iterator i = physical_table_.find(*it);
    if (i == physical_table_.end()) {
      dbg(DBG_ERROR, "ERROR: did not find physical element for ldid %lu in the scratch set!\n", *it); // NOLINT
      exit(-1);
      return false;
    }
    set->insert(i->second);
  }
  return true;
}

bool JobEntry::GetPhysicalReduceSet(IDSet<physical_data_id_t>* set) {
  set->clear();
  *set = physical_reduce_set_;
  if (reduce_set_.size() > 0) {
    if (physical_reduce_set_.size() == 0) {
      dbg(DBG_ERROR, "ERROR: physical reduce set was empty!\n");
      exit(-1);
      return false;
    }
  }

  return true;
}

bool JobEntry::IsReadyToAssign() {
  return (assignment_dependencies_.size() == 0);
}

void JobEntry::remove_assignment_dependency(job_id_t job_id) {
  assignment_dependencies_.remove(job_id);
}

bool JobEntry::IsReadyForCompleteVersioning() {
  return (versioning_dependencies_.size() == 0 && !future_);
}

void JobEntry::remove_versioning_dependency(job_id_t job_id) {
  versioning_dependencies_.remove(job_id);
  meta_before_set_->InvalidateNegativeQueryCache();
}

bool JobEntry::GetRegion(GeometricRegion *region) {
  if (region_ != GeometricRegion()) {
    *region = region_;
    return true;
  }

  return false;
}

bool JobEntry::GetUnionSetRegion(DataManager *data_manager, GeometricRegion *region) {
  if (union_region_valid_) {
    *region = union_region_;
    return true;
  } else {
    if (union_set_.size() == 0) {
      return false;
    } else {
      const LogicalDataObject* ldo;
      IDSet<logical_data_id_t>::IDSetIter iter = union_set_.begin();
      ldo = data_manager->FindLogicalObject(*iter);
      union_region_ = *ldo->region();
      ++iter;
      for (; iter != union_set_.end(); ++iter) {
        ldo = data_manager->FindLogicalObject(*iter);
        union_region_ = GeometricRegion::GetBoundingBox(union_region_, *ldo->region());
      }
      *region = union_region_;
      union_region_valid_ = true;
      return true;
    }
  }
}


bool JobEntry::GetReadSetRegion(DataManager *data_manager, GeometricRegion *region) {
  if (read_region_valid_) {
    *region = read_region_;
    return true;
  } else {
    if (read_set_.size() == 0) {
      return false;
    } else {
      const LogicalDataObject* ldo;
      IDSet<logical_data_id_t>::IDSetIter iter = read_set_.begin();
      ldo = data_manager->FindLogicalObject(*iter);
      read_region_ = *ldo->region();
      ++iter;
      for (; iter != read_set_.end(); ++iter) {
        ldo = data_manager->FindLogicalObject(*iter);
        read_region_ = GeometricRegion::GetBoundingBox(read_region_, *ldo->region());
      }
      *region = read_region_;
      read_region_valid_ = true;
      return true;
    }
  }
}

bool JobEntry::GetWriteSetRegion(DataManager *data_manager, GeometricRegion *region) {
  if (write_region_valid_) {
    *region = write_region_;
    return true;
  } else {
    if (write_set_.size() == 0) {
      return false;
    } else {
      const LogicalDataObject* ldo;
      IDSet<logical_data_id_t>::IDSetIter iter = write_set_.begin();
      ldo = data_manager->FindLogicalObject(*iter);
      write_region_ = *ldo->region();
      ++iter;
      for (; iter != write_set_.end(); ++iter) {
        ldo = data_manager->FindLogicalObject(*iter);
        write_region_ = GeometricRegion::GetBoundingBox(write_region_, *ldo->region());
      }
      *region = write_region_;
      write_region_valid_ = true;
      return true;
    }
  }
}

bool JobEntry::GetScratchSetRegion(DataManager *data_manager, GeometricRegion *region) {
  if (scratch_region_valid_) {
    *region = scratch_region_;
    return true;
  } else {
    if (scratch_set_.size() == 0) {
      return false;
    } else {
      const LogicalDataObject* ldo;
      IDSet<logical_data_id_t>::IDSetIter iter = scratch_set_.begin();
      ldo = data_manager->FindLogicalObject(*iter);
      scratch_region_ = *ldo->region();
      ++iter;
      for (; iter != scratch_set_.end(); ++iter) {
        ldo = data_manager->FindLogicalObject(*iter);
        scratch_region_ = GeometricRegion::GetBoundingBox(scratch_region_, *ldo->region());
      }
      *region = scratch_region_;
      scratch_region_valid_ = true;
      return true;
    }
  }
}

bool JobEntry::GetReduceSetRegion(DataManager *data_manager, GeometricRegion *region) {
  if (reduce_region_valid_) {
    *region = reduce_region_;
    return true;
  } else {
    if (reduce_set_.size() == 0) {
      return false;
    } else {
      const LogicalDataObject* ldo;
      IDSet<logical_data_id_t>::IDSetIter iter = reduce_set_.begin();
      ldo = data_manager->FindLogicalObject(*iter);
      reduce_region_ = *ldo->region();
      ++iter;
      for (; iter != reduce_set_.end(); ++iter) {
        ldo = data_manager->FindLogicalObject(*iter);
        reduce_region_ = GeometricRegion::GetBoundingBox(reduce_region_, *ldo->region());
      }
      *region = reduce_region_;
      reduce_region_valid_ = true;
      return true;
    }
  }
}

void JobEntry::MarkJobAsCompletelyResolved() {
    assignment_dependencies_.clear();
    versioning_dependencies_.clear();
    versioned_ = true;
    versioned_entire_context_ = true;
}

bool JobEntry::LookUpMetaBeforeSet(JobEntry* job) {
  return meta_before_set_->LookUpBeforeSetChain(job->job_id(), job->job_depth());
}





ComputeJobEntry::ComputeJobEntry(
    const std::string& job_name,
    const job_id_t& job_id,
    const IDSet<logical_data_id_t>& read_set,
    const IDSet<logical_data_id_t>& write_set,
    const IDSet<logical_data_id_t>& scratch_set,
    const IDSet<logical_data_id_t>& reduce_set,
    const IDSet<job_id_t>& before_set,
    const IDSet<job_id_t>& after_set,
    const job_id_t& parent_job_id,
    const job_id_t& future_job_id,
    const bool& sterile,
    const GeometricRegion& region,
    const Parameter& params,
    const CombinerMap& combiner_map) {
    job_type_ = JOB_COMP;
    job_name_ = job_name;
    job_id_ = job_id;
    read_set_ = read_set;
    write_set_ = write_set;
    scratch_set_ = scratch_set;
    reduce_set_ = reduce_set;
    before_set_ = before_set;
    after_set_ = after_set;
    parent_job_id_ = parent_job_id;
    future_job_id_ = future_job_id;
    sterile_ = sterile;
    region_ = region;
    params_ = params;
    combiner_map_ = combiner_map;

    union_set_.insert(read_set_);
    union_set_.insert(write_set_);
    union_set_.insert(scratch_set_);
    union_set_.insert(reduce_set_);

    // parent should be explicitally in before set - omidm
    before_set_.insert(parent_job_id);

    assignment_dependencies_ = before_set_;
    versioning_dependencies_ = before_set_;
}

ComputeJobEntry::~ComputeJobEntry() {
}

SaveDataJobEntry::SaveDataJobEntry(
    const job_id_t& job_id,
    const IDSet<logical_data_id_t>& read_set) {
    job_type_ = JOB_SAVE;
    job_name_ = NIMBUS_SAVE_DATA_JOB_NAME;
    job_id_ = job_id;
    read_set_ = read_set;
}

SaveDataJobEntry::~SaveDataJobEntry() {
}

KernelJobEntry::KernelJobEntry() {
  job_type_ = JOB_SCHED;
  job_name_ = NIMBUS_KERNEL_JOB_NAME;
  job_id_ = NIMBUS_KERNEL_JOB_ID;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
  sterile_ = true;
  versioned_ = true;
  assigned_ = true;

  meta_before_set_ = boost::shared_ptr<MetaBeforeSet> (new MetaBeforeSet());
  job_depth_ = NIMBUS_INIT_JOB_DEPTH;
}

KernelJobEntry::~KernelJobEntry() {
}


MainJobEntry::MainJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_COMP;
  job_name_ = NIMBUS_MAIN_JOB_NAME;
  job_id_ = job_id;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
}

MainJobEntry::~MainJobEntry() {
}


FutureJobEntry::FutureJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_FUTURE;
  job_id_ = job_id;
  future_ = true;
}

FutureJobEntry::~FutureJobEntry() {
}


LocalCopyJobEntry::LocalCopyJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_COPY;
  job_name_ = NIMBUS_LOCAL_COPY_JOB_NAME;
  job_id_ = job_id;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
  sterile_ = true;
  versioned_ = true;
  assigned_ = true;
}

LocalCopyJobEntry::~LocalCopyJobEntry() {
}


CreateDataJobEntry::CreateDataJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_CREATE;
  job_name_ = NIMBUS_CREATE_DATA_JOB_NAME;
  job_id_ = job_id;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
  sterile_ = true;
  versioned_ = true;
  assigned_ = true;
}

CreateDataJobEntry::~CreateDataJobEntry() {
}


RemoteCopyReceiveJobEntry::RemoteCopyReceiveJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_COPY;
  job_name_ = NIMBUS_REMOTE_COPY_RECEIVE_JOB_NAME;
  job_id_ = job_id;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
  sterile_ = true;
  versioned_ = true;
  assigned_ = true;
}

RemoteCopyReceiveJobEntry::~RemoteCopyReceiveJobEntry() {
}


RemoteCopySendJobEntry::RemoteCopySendJobEntry(const job_id_t& job_id) {
  job_type_ = JOB_COPY;
  job_name_ = NIMBUS_REMOTE_COPY_SEND_JOB_NAME;
  job_id_ = job_id;
  parent_job_id_ = NIMBUS_KERNEL_JOB_ID;
  sterile_ = true;
  versioned_ = true;
  assigned_ = true;
}

RemoteCopySendJobEntry::~RemoteCopySendJobEntry() {
}

ID<job_id_t> RemoteCopySendJobEntry::receive_job_id() {
  return receive_job_id_;
}

ID<worker_id_t> RemoteCopySendJobEntry::to_worker_id() {
  return to_worker_id_;
}

std::string RemoteCopySendJobEntry::to_ip() {
  return to_ip_;
}

ID<port_t> RemoteCopySendJobEntry::to_port() {
  return to_port_;
}

void RemoteCopySendJobEntry::set_receive_job_id(ID<job_id_t> receive_job_id) {
  receive_job_id_ = receive_job_id;
}

void RemoteCopySendJobEntry::set_to_worker_id(ID<worker_id_t> worker_id) {
  to_worker_id_ = worker_id;
}

void RemoteCopySendJobEntry::set_to_ip(std::string ip) {
  to_ip_ = ip;
}

void RemoteCopySendJobEntry::set_to_port(ID<port_t> port) {
  to_port_ = port;
}


