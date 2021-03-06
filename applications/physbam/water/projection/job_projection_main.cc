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
 * Author: Hang Qu <quhang@stanford.edu>
 */

#include <sstream>
#include <string>
#include <vector>

#include "applications/physbam/water//app_utils.h"
#include "applications/physbam/water//data_names.h"
#include "applications/physbam/water//job_names.h"
#include "applications/physbam/water//physbam_utils.h"
#include "applications/physbam/water//reg_def.h"
#include "src/shared/dbg.h"
#include "src/shared/nimbus.h"
#include "src/worker/worker_thread.h"

#include "applications/physbam/water//projection/job_projection_main.h"

namespace application {

JobProjectionMain::JobProjectionMain(nimbus::Application *app) {
  set_application(app);
};

nimbus::Job* JobProjectionMain::Clone() {
  return new JobProjectionMain(application());
}

void JobProjectionMain::Execute(
    nimbus::Parameter params,
    const nimbus::DataArray& da) {
  dbg(APP_LOG, "Executing PROJECTION_MAIN job\n");

  // Get parameters: frame, time
  InitConfig init_config;
  std::string params_str(params.ser_data().data_ptr_raw(),
                         params.ser_data().size());
  LoadParameter(params_str, &init_config);
  T dt = init_config.dt;
  const int& frame = init_config.frame;
  const T& time = init_config.time;

  dbg(APP_LOG, "Frame %i and time %f in PROJECTION_MAIN job\n",
      frame, time);
  SpawnJobs(frame, time, dt, da, init_config.global_region);
}

void JobProjectionMain::SpawnJobs(
    int frame, T time, T dt, const nimbus::DataArray& da,
    const nimbus::GeometricRegion& global_region) {
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  // nimbus::JobQuery job_query(this);
  int projection_job_num = 5;
  std::vector<nimbus::job_id_t> projection_job_ids;
  GetNewJobID(&projection_job_ids, projection_job_num);
  nimbus::IDSet<nimbus::logical_data_id_t> read, write;
  nimbus::IDSet<nimbus::job_id_t> before, after;

  nimbus::Parameter default_params;
  std::string default_params_str;
  SerializeParameter(
      frame, time, dt, kPNAInt,
      global_region, global_region,
      kPNAInt, &default_params_str);
  default_params.set_ser_data(SerializedData(default_params_str));

  std::vector<nimbus::Parameter> default_part_params;
  default_part_params.resize(kProjAppPartNum);

  StartTemplate("projection_main");

  for (uint64_t i = 0; i < kProjAppPartNum; ++i) {
    std::string default_params_str;
    SerializeParameter(
        frame, time, dt, kPNAInt,
        global_region, ph.map()["kProjRegY2W0Central"][i],
        kPNAInt, &default_params_str);
    default_part_params[i].set_ser_data(SerializedData(default_params_str));
  }

  int construct_matrix_job_num = kProjAppPartNum;
  std::vector<nimbus::job_id_t> construct_matrix_job_ids;
  GetNewJobID(&construct_matrix_job_ids, construct_matrix_job_num);

  int local_initialize_job_num = kProjAppPartNum;
  std::vector<nimbus::job_id_t> local_initialize_job_ids;
  GetNewJobID(&local_initialize_job_ids, local_initialize_job_num);

  int calculate_boundary_condition_part_one_job_num = kProjAppPartNum;
  std::vector<nimbus::job_id_t> calculate_boundary_condition_part_one_job_ids;
  GetNewJobID(&calculate_boundary_condition_part_one_job_ids,
              calculate_boundary_condition_part_one_job_num);

  int calculate_boundary_condition_part_two_job_num = kProjAppPartNum;
  std::vector<nimbus::job_id_t> calculate_boundary_condition_part_two_job_ids;
  GetNewJobID(&calculate_boundary_condition_part_two_job_ids,
              calculate_boundary_condition_part_two_job_num);

  for (int index = 0;
       index < calculate_boundary_condition_part_one_job_num;
       ++index) {
    read.clear();
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W3Outer"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W1Outer"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W0Central"][index],
                        APP_U_INTERFACE, NULL);
    write.clear();
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W3CentralWGB"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W1CentralWGB"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W0Central"][index], APP_U_INTERFACE, NULL);

    before.clear();
    StageJobAndLoadBeforeSet(&before, PROJECTION_CALCULATE_BOUNDARY_CONDITION_PART_ONE,
                       calculate_boundary_condition_part_one_job_ids[index],
                       read, write);

    SpawnComputeJob(PROJECTION_CALCULATE_BOUNDARY_CONDITION_PART_ONE,
                       calculate_boundary_condition_part_one_job_ids[index],
                       read, write, before, after,
                       default_part_params[index], true,
                       ph.map()["kProjRegY2W3Central"][index]);
  }
  MarkEndOfStage();

  for (int index = 0;
       index < calculate_boundary_condition_part_two_job_num;
       ++index) {
    read.clear();
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W3Outer"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W1Outer"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W0Central"][index], APP_U_INTERFACE, NULL);
    write.clear();
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W3CentralWGB"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W1CentralWGB"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W0Central"][index],
                        APP_U_INTERFACE, NULL);

    before.clear();
    StageJobAndLoadBeforeSet(&before, PROJECTION_CALCULATE_BOUNDARY_CONDITION_PART_TWO,
                       calculate_boundary_condition_part_two_job_ids[index],
                       read, write);

    SpawnComputeJob(PROJECTION_CALCULATE_BOUNDARY_CONDITION_PART_TWO,
                       calculate_boundary_condition_part_two_job_ids[index],
                       read, write, before, after,
                       default_part_params[index], true,
                       ph.map()["kProjRegY2W3Central"][index]);
  }
  MarkEndOfStage();

  // Construct matrix.
  for (int index = 0; index < construct_matrix_job_num; ++index) {
    read.clear();
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W3Outer"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W1Outer"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W0Central"][index], APP_U_INTERFACE, NULL);
    write.clear();
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W3CentralWGB"][index], APP_FACE_VEL, APP_PHI, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W1CentralWGB"][index],
                        APP_DIVERGENCE, APP_PSI_D, APP_PSI_N,
                        APP_FILLED_REGION_COLORS, APP_PRESSURE, NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W0Central"][index],
                        APP_U_INTERFACE, APP_MATRIX_A,
                        APP_VECTOR_B, APP_PROJECTION_LOCAL_TOLERANCE,
                        APP_INDEX_M2C, APP_INDEX_C2M,
                        APP_PROJECTION_LOCAL_N, APP_PROJECTION_INTERIOR_N,
                        NULL);

    before.clear();
    StageJobAndLoadBeforeSet(&before, PROJECTION_CONSTRUCT_MATRIX,
                       construct_matrix_job_ids[index],
                       read, write);

    SpawnComputeJob(PROJECTION_CONSTRUCT_MATRIX,
                       construct_matrix_job_ids[index],
                       read, write, before, after,
                       default_part_params[index], true,
                       ph.map()["kProjRegY2W3Central"][index]);
  }
  MarkEndOfStage();

  // Global initialize.
  read.clear();
  LoadLdoIdsInSet(&read, ph.map()["kRegW0Central"][0],
                      APP_PROJECTION_INTERIOR_N, APP_PROJECTION_LOCAL_TOLERANCE,
                      NULL);
  write.clear();
  LoadLdoIdsInSet(&write, ph.map()["kRegW0Central"][0],
                      APP_PROJECTION_GLOBAL_N,
                      APP_PROJECTION_GLOBAL_TOLERANCE,
                      APP_PROJECTION_DESIRED_ITERATIONS, NULL);
  before.clear();
  StageJobAndLoadBeforeSet(&before, PROJECTION_GLOBAL_INITIALIZE,
                     projection_job_ids[3],
                     read, write);

  SpawnComputeJob(PROJECTION_GLOBAL_INITIALIZE,
                     projection_job_ids[3],
                     read, write, before, after,
                     default_params, true,
                     ph.map()["kRegW3Central"][0]);
  // Global initialize is a job that serves as a bottleneck.
  MarkEndOfStage();

  // Local initialize.
  for (int index = 0; index < local_initialize_job_num; ++index) {
    read.clear();
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W0Central"][index],
                        APP_PROJECTION_LOCAL_N, APP_PROJECTION_INTERIOR_N,
                        APP_INDEX_M2C,
                        APP_INDEX_C2M,
                        APP_VECTOR_B,
                        APP_MATRIX_A, NULL);
    LoadLdoIdsInSet(&read, ph.map()["kProjRegY2W1Outer"][index], APP_PRESSURE,
                        NULL);
    write.clear();
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W0Central"][index],
                        APP_VECTOR_B, APP_PROJECTION_LOCAL_RESIDUAL, APP_MATRIX_C,
                        APP_VECTOR_TEMP, APP_VECTOR_Z,
                        APP_VECTOR_PRESSURE,
                        NULL);
    LoadLdoIdsInSet(&write, ph.map()["kProjRegY2W1CentralWGB"][index],
                        APP_VECTOR_P_META_FORMAT, NULL);
    before.clear();
    StageJobAndLoadBeforeSet(&before, PROJECTION_LOCAL_INITIALIZE,
                       local_initialize_job_ids[index],
                       read, write);

    SpawnComputeJob(PROJECTION_LOCAL_INITIALIZE,
                       local_initialize_job_ids[index],
                       read, write, before, after,
                       default_part_params[index], true,
                       ph.map()["kProjRegY2W3Central"][index]);
  }
  MarkEndOfStage();

  // Projection loop.
  read.clear();
  LoadLdoIdsInSet(&read, ph.map()["kRegW0Central"][0],
                      APP_PROJECTION_INTERIOR_N,
                      APP_PROJECTION_LOCAL_RESIDUAL,
                      APP_PROJECTION_GLOBAL_TOLERANCE,
                      APP_PROJECTION_DESIRED_ITERATIONS,
                      NULL);
  write.clear();
  nimbus::Parameter projection_loop_iteration_params;
  std::string projection_loop_iteration_str;
  SerializeParameter(
      frame, time, dt, kPNAInt,
      global_region, global_region,
      1, &projection_loop_iteration_str);
  projection_loop_iteration_params.set_ser_data(
      SerializedData(projection_loop_iteration_str));
  before.clear();
  StageJobAndLoadBeforeSet(&before, PROJECTION_LOOP_ITERATION,
                     projection_job_ids[4],
                     read, write,
                     true);

  SpawnComputeJob(PROJECTION_LOOP_ITERATION,
                     projection_job_ids[4],
                     read, write, before, after,
                     projection_loop_iteration_params, false,
                     ph.map()["kRegW3Central"][0]);
  MarkEndOfStage();

  EndTemplate("projection_main");

  // job_query.PrintTimeProfile();

  // if (time == 0) {
  //   dbg(APP_LOG, "Print job dependency figure.\n");
  //   job_query.GenerateDotFigure("projection_main.dot");
  // }
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    double time  = (static_cast<double>(t.tv_sec - start_time.tv_sec)) +
        .000001 * (static_cast<double>(t.tv_usec - start_time.tv_usec));
    dbg(APP_LOG, "\nThe query time spent in job PROJECTION_LOOP_ITERATION_MAIN is %f seconds.\n",
        time);
  }
}


}  // namespace application
