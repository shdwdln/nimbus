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
 * This file contains a job corresponding to one iteration consisting of all
 * different simulation stages (advection, projection, extrapolation etc).
 * (NOTE TODO: Right now, it contains the entire water simulation job. We'll
 * edit it as we progress.)
 *
 * Author: Chinmayee Shah <chinmayee.shah@stanford.edu>
 */

#include "application/water_alternate_coarse/app_utils.h"
#include "application/water_alternate_coarse/job_iteration.h"
#include "application/water_alternate_coarse/water_driver.h"
#include "application/water_alternate_coarse/water_example.h"
#include "application/water_alternate_coarse/water_sources.h"
#include "shared/nimbus.h"

namespace application {

    JobIteration::JobIteration(Application *app) {
        set_application(app);
    };

    nimbus::Job* JobIteration::Clone() {
        return new JobIteration(application());
    }

    void JobIteration::Execute(Parameter params, const DataArray& da) {
        dbg(APP_LOG, "Executing iteration job\n");

        // initialize example
        PhysBAM::WATER_EXAMPLE<TV> *example =
            new PhysBAM::WATER_EXAMPLE<TV>(PhysBAM::STREAM_TYPE((RW())));
        example->Initialize_Grid(TV_INT::All_Ones_Vector()*kScale,
                                 PhysBAM::RANGE<TV>(TV(),
                                                    TV::All_Ones_Vector())
                                 );
        PhysBAM::WaterSources::Add_Source(example);

        // initialize driver
        PhysBAM::WATER_DRIVER<TV> driver(*example);

        // TODO: main program - separate into initialization and iterations
        driver.Execute_Main_Program();

        dbg(APP_LOG, "Completed executing iteration job\n");
    }

} // namespace application
