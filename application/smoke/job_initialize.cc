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
 * Author: Chinmayee Shah <chinmayee.shah@stanford.edu>
 */

#include "application/smoke/app_utils.h"
#include "application/smoke/job_initialize.h"
#include "application/smoke/physbam_utils.h"
#include "application/smoke/smoke_driver.h"
#include "application/smoke/smoke_example.h"
#include "application/smoke/job_names.h"
#include "shared/dbg.h"
#include "shared/nimbus.h"

namespace application {

    JobInitialize::JobInitialize(nimbus::Application *app) {
        set_application(app);
    };

    nimbus::Job* JobInitialize::Clone() {
        return new JobInitialize(application());
    }

    void JobInitialize::Execute(nimbus::Parameter params, const nimbus::DataArray& da) {
        dbg(APP_LOG, "Executing initialize job\n");

        InitConfig init_config;
        init_config.init_phase = true;
        init_config.set_boundary_condition = true;
        init_config.global_region = kDefaultRegion;
        init_config.local_region = kDefaultRegion;
        // Threading settings.
        init_config.use_threading = use_threading();
        init_config.core_quota = core_quota();
        T dt;
        std::string params_str(params.ser_data().data_ptr_raw(),
            params.ser_data().size());
        LoadParameter(params_str, &init_config.frame, &init_config.time, &dt,
            &init_config.global_region, &init_config.local_region);

        PhysBAM::SMOKE_EXAMPLE<TV> *example;
        PhysBAM::SMOKE_DRIVER<TV> *driver;

        DataConfig data_config;
        data_config.SetAll();
        InitializeExampleAndDriver(init_config, data_config,
                                   this, da, example, driver);
        *thread_queue_hook() = example->nimbus_thread_queue;

        *thread_queue_hook() = NULL;
        // Free resources.
        DestroyExampleAndDriver(example, driver);




        dbg(APP_LOG, "Completed executing initialize job\n");
    }

} // namespace application