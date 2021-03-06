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
  * Template job entry is a version of job entry that contanis only required
  * information to assign the job properly. It includes version maps and
  * read/write set. This data id=s read from the main job entry and shared
  * among multiple shadoes. This way the creation of shadow jobs is very cost
  * effective, cause only a couple od pointers are passed to each instance.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "src/scheduler/template_job_entry.h"

using namespace nimbus; // NOLINT


TemplateJobEntry::TemplateJobEntry() {
  job_type_ = JOB_TMPL;

  vmap_read_diff_ = boost::shared_ptr<VersionMap>(new VersionMap());
  vlist_write_diff_ = boost::shared_ptr<VersionList>(new VersionList());
}

TemplateJobEntry::TemplateJobEntry(const std::string& job_name,
                                   const job_id_t& job_id,
                                   const size_t& index,
                                   const IDSet<logical_data_id_t> read_set,
                                   const IDSet<logical_data_id_t> write_set,
                                   const IDSet<logical_data_id_t> scratch_set,
                                   const IDSet<logical_data_id_t> reduce_set,
                                   const IDSet<job_id_t>& before_set,
                                   const IDSet<job_id_t>& after_set,
                                   const bool& sterile,
                                   const GeometricRegion& region,
                                   const CombinerMap& combiner_map,
                                   TemplateEntry* template_entry) {
  job_type_ = JOB_TMPL;
  job_name_ = job_name;
  job_id_ = job_id;
  index_ = index;
  read_set_ = read_set;
  write_set_ = write_set;
  scratch_set_ = scratch_set;
  reduce_set_ = reduce_set;
  before_set_ = before_set;
  after_set_ = after_set;
  sterile_ = sterile;
  region_ = region;
  combiner_map_ = combiner_map;
  template_entry_ = template_entry;

  union_set_.insert(read_set_);
  union_set_.insert(write_set_);
  union_set_.insert(scratch_set_);
  union_set_.insert(reduce_set_);

  vmap_read_diff_ = boost::shared_ptr<VersionMap>(new VersionMap());
  vlist_write_diff_ = boost::shared_ptr<VersionList>(new VersionList());
}

TemplateJobEntry::~TemplateJobEntry() {
}

size_t TemplateJobEntry::index() {
  return index_;
}

TemplateEntry* TemplateJobEntry::template_entry() {
  return template_entry_;
}

boost::shared_ptr<VersionMap> TemplateJobEntry::vmap_read_diff() const {
  return vmap_read_diff_;
}

boost::shared_ptr<VersionList> TemplateJobEntry::vlist_write_diff() const {
  return vlist_write_diff_;
}

void TemplateJobEntry::set_index(size_t index) {
  index_ = index;
}

void TemplateJobEntry::set_template_entry(TemplateEntry* template_entry) {
  template_entry_ = template_entry;
}

void TemplateJobEntry::set_vmap_read_diff(boost::shared_ptr<VersionMap> vmap_read_diff) {
  vmap_read_diff_ = vmap_read_diff;
}

void TemplateJobEntry::set_vlist_write_diff(boost::shared_ptr<VersionList> vlist_write_diff) {
  vlist_write_diff_ = vlist_write_diff;
}

