/* Copyright 2013 Stanford University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 vd* are met:
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

#include <PhysBAM_Tools/Krylov_Solvers/PCG_SPARSE.h>

#include "applications/physbam/water//app_utils.h"
#include "applications/physbam/water//physbam_utils.h"
#include "applications/physbam/water//projection/projection_driver.h"
#include "applications/physbam/water//water_driver.h"
#include "applications/physbam/water//water_example.h"
#include "src/shared/dbg.h"
#include "src/shared/nimbus.h"

#include "src/data/scalar_data.h"
#include "applications/physbam/water//data_include.h"
#include "src/worker/worker_thread.h"
#include "applications/physbam/water//projection/job_projection_local_initialize.h"

namespace application {

JobProjectionLocalInitialize::JobProjectionLocalInitialize(
    nimbus::Application *app) {
  set_application(app);
};

nimbus::Job* JobProjectionLocalInitialize::Clone() {
  return new JobProjectionLocalInitialize(application());
}

void JobProjectionLocalInitialize::Execute(
    nimbus::Parameter params,
    const nimbus::DataArray& da) {
  dbg(APP_LOG, "Executing PROJECTION_LOCAL_INITIALIZE job.\n");

  InitConfig init_config;

  std::string params_str(params.ser_data().data_ptr_raw(),
                         params.ser_data().size());
  LoadParameter(params_str, &init_config);
  T dt = init_config.dt;

  DataConfig data_config;
  data_config.SetFlag(DataConfig::PROJECTION_LOCAL_N);
  data_config.SetFlag(DataConfig::PROJECTION_INTERIOR_N);

  data_config.SetFlag(DataConfig::VECTOR_PRESSURE);
  data_config.SetFlag(DataConfig::VECTOR_P_META_FORMAT);
  data_config.SetFlag(DataConfig::PRESSURE);
  data_config.SetFlag(DataConfig::INDEX_M2C);
  data_config.SetFlag(DataConfig::INDEX_C2M);

  data_config.SetFlag(DataConfig::VECTOR_TEMP);
  data_config.SetFlag(DataConfig::MATRIX_A);
  data_config.SetFlag(DataConfig::VECTOR_B);
  data_config.SetFlag(DataConfig::PROJECTION_LOCAL_RESIDUAL);
  data_config.SetFlag(DataConfig::MATRIX_C);

  data_config.SetFlag(DataConfig::VECTOR_Z);

  PhysBAM::PCG_SPARSE<float> pcg_temp;
  pcg_temp.Set_Maximum_Iterations(application::kMaxIterations);
  pcg_temp.evolution_solver_type = PhysBAM::krylov_solver_cg;
  pcg_temp.cg_restart_iterations = 0;
  pcg_temp.Show_Results();

  PhysBAM::ProjectionDriver projection_driver(
      pcg_temp, init_config, data_config, &worker_thread()->allocated_threads);
  dbg(APP_LOG, "Job PROJECTION_LOCAL_INITIALIZE starts (dt=%f).\n", dt);

  projection_driver.LoadFromNimbus(this, da);

  {
    application::ScopeTimer scope_timer(name());
    projection_driver.LocalInitialize();
  }

  projection_driver.SaveToNimbus(this, da);

  dbg(APP_LOG, "Completed executing PROJECTION_LOCAL_INITIALIZE job\n");
}

}  // namespace application
