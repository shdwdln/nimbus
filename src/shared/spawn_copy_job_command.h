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
  * Workers send spawn copy jobs to the controller to tell it to copy
  * one logical object to another logical object. These is needed, for
  * example, when there are explicit ghost regions, such that one job
  * is reading state from a prior iteration while another job writes
  * it on this iteration.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  * Author: Philip Levis <pal@cs.stanford.edu>
  */

#ifndef NIMBUS_SRC_SHARED_SPAWN_COPY_JOB_COMMAND_H_
#define NIMBUS_SRC_SHARED_SPAWN_COPY_JOB_COMMAND_H_


#include <string>
#include "src/shared/scheduler_command.h"
#include "src/shared/protobuf_compiled/commands.pb.h"

namespace nimbus {
class SpawnCopyJobCommand : public SchedulerCommand {
  public:
    SpawnCopyJobCommand();
    SpawnCopyJobCommand(const ID<job_id_t>& job_id,
                        const ID<logical_data_id_t>& from_logical_id,
                        const ID<logical_data_id_t>& to_logical_id,
                        const IDSet<job_id_t>& before,
                        const IDSet<job_id_t>& after,
                        const ID<job_id_t>& parent_job_id);
    ~SpawnCopyJobCommand();

    virtual SchedulerCommand* Clone();
    virtual bool Parse(const std::string& param_segment);
    virtual bool Parse(const SchedulerPBuf& buf);
    virtual std::string ToNetworkData();
    virtual std::string ToString();
    ID<job_id_t> job_id();
    ID<logical_data_id_t> from_logical_id();
    ID<logical_data_id_t> to_logical_id();
    IDSet<job_id_t> before_set();
    IDSet<job_id_t> after_set();
    ID<job_id_t> parent_job_id();

  private:
    ID<job_id_t> job_id_;
    ID<logical_data_id_t> from_logical_id_;
    ID<logical_data_id_t> to_logical_id_;
    IDSet<job_id_t> before_set_;
    IDSet<job_id_t> after_set_;
    ID<job_id_t> parent_job_id_;

    bool ReadFromProtobuf(const SubmitCopyJobPBuf& buf);
    bool WriteToProtobuf(SubmitCopyJobPBuf* buf);
};

}  // namespace nimbus

#endif  // NIMBUS_SRC_SHARED_SPAWN_COPY_JOB_COMMAND_H_
