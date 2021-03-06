/* Copyright 2013 Stanford University.
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
 * This file contains the job that modifies levelset and particles. This job
 * should be spawned after calling advance time step forces and before
 * adjusting phi with sources.
 *
 * This job needs all data (levelset, particles and face velocities).
 *
 * Author: Chinmayee Shah <chinmayee.shah@stanford.edu>
 */

#include "applications/physbam/water//app_utils.h"
#include "applications/physbam/water//job_modify_levelset_part_two.h"
#include "applications/physbam/water//physbam_utils.h"
#include "applications/physbam/water//water_driver.h"
#include "applications/physbam/water//water_example.h"
#include "src/data/physbam/physbam_data.h"
#include "src/shared/dbg.h"
#include "src/shared/nimbus.h"
#include <sstream>
#include <string>

namespace application {

JobModifyLevelsetPartTwo::JobModifyLevelsetPartTwo(nimbus::Application *app) {
    set_application(app);
};

nimbus::Job* JobModifyLevelsetPartTwo::Clone() {
    return new JobModifyLevelsetPartTwo(application());
}

void JobModifyLevelsetPartTwo::Execute(nimbus::Parameter params, const nimbus::DataArray& da) {
    dbg(APP_LOG, "--- Executing modify levelset job -- part two\n");

    InitConfig init_config;
    init_config.set_boundary_condition = false;
    std::string params_str(params.ser_data().data_ptr_raw(),
                           params.ser_data().size());
    LoadParameter(params_str, &init_config);
    T dt = init_config.dt;
    dbg(APP_LOG, "Frame %i in modify levelset job\n", init_config.frame);

    // initialize configuration and state
    PhysBAM::WATER_EXAMPLE<TV> *example;
    PhysBAM::WATER_DRIVER<TV> *driver;

    DataConfig data_config;
    data_config.SetFlag(DataConfig::VELOCITY);
    data_config.SetFlag(DataConfig::VELOCITY_GHOST);
    // TODO: test whether it is correct.
    //data_config.SetFlag(DataConfig::LEVELSET_READ);
    data_config.SetFlag(DataConfig::LEVELSET_WRITE);
    data_config.SetFlag(DataConfig::LEVELSET_BW_SEVEN_READ);
    data_config.SetFlag(DataConfig::POSITIVE_PARTICLE);
    data_config.SetFlag(DataConfig::NEGATIVE_PARTICLE);
    data_config.SetFlag(DataConfig::REMOVED_POSITIVE_PARTICLE);
    data_config.SetFlag(DataConfig::REMOVED_NEGATIVE_PARTICLE);
    InitializeExampleAndDriver(init_config, data_config,
                               this, da, example, driver);

    {
      application::ScopeTimer scope_timer(name());
      driver->ModifyLevelSetPartTwoImpl(this, da, init_config.local_region, dt);
    }

    // Write, free resources.
    example->Save_To_Nimbus(this, da, driver->current_frame + 1);
    DestroyExampleAndDriver(example, driver);

    dbg(APP_LOG, "Completed executing modify levelset job -- part two\n");
}

} // namespace application
