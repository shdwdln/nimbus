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

#include "applications/physbam/water//physbam_include.h"
#include "src/data/static_config/static_config_variable.h"

#ifndef NIMBUS_APPLICATION_WATER_MULTIPLE_STATIC_CONFIG_COLLISION_BODY_
#define NIMBUS_APPLICATION_WATER_MULTIPLE_STATIC_CONFIG_COLLISION_BODY_
namespace application {

using nimbus::StaticConfigVariable;
using nimbus::GeometricRegion;

class StaticConfigCollisionBody : public StaticConfigVariable {
 public:
  typedef PhysBAM::VECTOR<float, 3> TV;
  typedef typename PhysBAM::COLLISION_GEOMETRY_COLLECTION_POLICY<PhysBAM::GRID<TV> >::
      GRID_BASED_COLLISION_GEOMETRY DataType;
  StaticConfigCollisionBody(const GeometricRegion& global_region);
  ~StaticConfigCollisionBody();
  virtual StaticConfigVariable* CreateNew(const GeometricRegion& local_region)
      const;
  virtual void Destroy();
  DataType* GetData() const;
 private:
  GeometricRegion global_region_;
  GeometricRegion local_region_;
  DataType* physbam_data_structure_;
  PhysBAM::GRID<TV> mac_grid_;
};

}  // namespace application
#endif  // NIMBUS_APPLICATION_WATER_MULTIPLE_STATIC_CONFIG_COLLISION_BODY_
