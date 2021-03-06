#ifndef NIMBUS_APPLICATION_WATER_MULTIPLE_WATER_SOURCES_H_
#define NIMBUS_APPLICATION_WATER_MULTIPLE_WATER_SOURCES_H_

#include "applications/physbam/water//app_utils.h"
#include "applications/physbam/water//water_example.h"
#include <PhysBAM_Tools/Vectors/VECTOR.h>

namespace PhysBAM {

    class WaterSources {
        public:
            WaterSources();
            static void Add_Source(WATER_EXAMPLE<VECTOR<application::T,1> > *example);
            static void Add_Source(WATER_EXAMPLE<VECTOR<application::T,2> > *example);
            static void Add_Source(WATER_EXAMPLE<VECTOR<application::T,3> > *example);
    };

} // namespace PhysBAM

#endif  // NIMBUS_APPLICATION_WATER_MULTIPLE_WATER_SOURCES_H_
