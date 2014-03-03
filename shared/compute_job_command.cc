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
  * Compute job command used to send compute jobs from scheduler to workers.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "shared/compute_job_command.h"

using namespace nimbus;  // NOLINT
using boost::tokenizer;
using boost::char_separator;

ComputeJobCommand::ComputeJobCommand() {
  name_ = COMPUTE_JOB_NAME;
  type_ = COMPUTE_JOB;
}

ComputeJobCommand::ComputeJobCommand(const std::string& job_name,
    const ID<job_id_t>& job_id,
    const IDSet<physical_data_id_t>& read, const IDSet<physical_data_id_t>& write,
    const IDSet<job_id_t>& before, const IDSet<job_id_t>& after,
    const Parameter& params,
    const bool& sterile)
: job_name_(job_name), job_id_(job_id),
  read_set_(read), write_set_(write),
  before_set_(before), after_set_(after),
  params_(params), sterile_(sterile) {
  name_ = COMPUTE_JOB_NAME;
  type_ = COMPUTE_JOB;
}

ComputeJobCommand::~ComputeJobCommand() {
}

SchedulerCommand* ComputeJobCommand::Clone() {
  return new ComputeJobCommand();
}

bool ComputeJobCommand::Parse(const std::string& params) {
  int num = 8;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(params, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: ComputeJobCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: ComputeJobCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  job_name_ = *iter;

  iter++;
  if (!job_id_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (!read_set_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid read set." << std::endl;
    return false;
  }

  iter++;
  if (!write_set_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid write set." << std::endl;
    return false;
  }

  iter++;
  if (!before_set_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (!after_set_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (!params_.Parse(*iter)) {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  iter++;
  if (*iter == "sterile") {
    sterile_ = true;
  } else if (*iter == "not_sterile") {
    sterile_ = false;
  } else {
    std::cout << "ERROR: Could not detect valid is parent flag." << std::endl;
    return false;
  }

  return true;
}

std::string ComputeJobCommand::toString() {
  std::string str;
  str += (name_ + " ");
  str += (job_name_ + " ");
  str += (job_id_.toString() + " ");
  str += (read_set_.toString() + " ");
  str += (write_set_.toString() + " ");
  str += (before_set_.toString() + " ");
  str += (after_set_.toString() + " ");
  str += (params_.toString() + " ");
  if (sterile_) {
    str += "sterile";
  } else {
    str += "not_sterile";
  }

  return str;
}

std::string ComputeJobCommand::toStringWTags() {
  std::string str;
  str += (name_ + " ");
  str += ("name:" + job_name_ + " ");
  str += ("id:" + job_id_.toString() + " ");
  str += ("read:" + read_set_.toString() + " ");
  str += ("write:" + write_set_.toString() + " ");
  str += ("before:" + before_set_.toString() + " ");
  str += ("after:" + after_set_.toString() + " ");
  str += ("params:" + params_.toString() + " ");
  if (sterile_) {
    str += "sterile";
  } else {
    str += "not_sterile";
  }
  return str;
}

std::string ComputeJobCommand::job_name() {
  return job_name_;
}

ID<job_id_t> ComputeJobCommand::job_id() {
  return job_id_;
}

IDSet<physical_data_id_t> ComputeJobCommand::read_set() {
  return read_set_;
}

IDSet<physical_data_id_t> ComputeJobCommand::write_set() {
  return write_set_;
}

IDSet<job_id_t> ComputeJobCommand::after_set() {
  return after_set_;
}
IDSet<job_id_t> ComputeJobCommand::before_set() {
  return before_set_;
}

Parameter ComputeJobCommand::params() {
  return params_;
}

bool ComputeJobCommand::sterile() {
  return sterile_;
}

