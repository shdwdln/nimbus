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
 *
 * This file specifies the options used to specify initialization hehaviors.
 * "InitConfig" is used to specify the options of WATER_DRIVER/WATER_EXAMPLE
 * initialization.
 * "DataConfig" is used to specify the data to be loaded.
 *
 */

#ifndef NIMBUS_APPLICATION_WATER_MULTIPLE_OPTIONS_H_
#define NIMBUS_APPLICATION_WATER_MULTIPLE_OPTIONS_H_

#include "applications/physbam/water//parameters.h"

namespace application {

// Data structure to specify how to initialize WATER_EXAMPLE and WATER_DRIVER.
struct InitConfig {
  int frame;
  T time;
  T dt;
  int rank;
  bool init_phase;
  int init_part;
  bool set_boundary_condition;
  nimbus::GeometricRegion global_region;
  nimbus::GeometricRegion local_region;
  int projection_iteration;

  // TODO(quhang), global region and local region should be passed as parameters
  // in the future.
  InitConfig() : global_region(1, 1, 1, 0, 0, 0),
                 local_region(1, 1, 1, 0, 0, 0) {
    frame = 0;
    time = 0;
    dt = 0;
    rank = 0;
    init_phase = false;
    init_part = 0;
    set_boundary_condition = true;
    projection_iteration = 0;
  }
};

// Data structure to describe which variables should be initialized in
// initialization stage. Might be moved to another file later. Not completed
// now.
struct DataConfig {
  enum DataType{
    VELOCITY = 0,
    VELOCITY_GHOST,
    DT,
    LEVELSET,  // Default is bandwidth 3.
    LEVELSET_READ,
    LEVELSET_WRITE,
    LEVELSET_BW_SEVEN_READ,
    LEVELSET_BW_SEVEN_WRITE,
    LEVELSET_BW_EIGHT_READ,
    LEVELSET_BW_EIGHT_WRITE,
    POSITIVE_PARTICLE,
    NEGATIVE_PARTICLE,
    REMOVED_POSITIVE_PARTICLE,
    REMOVED_NEGATIVE_PARTICLE,
    SHARED_PARTICLES_FLUSH,
    VALID_MASK,
    DIVERGENCE,
    PSI_N,
    PSI_D,
    REGION_COLORS,
    PRESSURE,
    VECTOR_PRESSURE,
    PRESSURE_SAVE,
    VELOCITY_SAVE,
    U_INTERFACE,
    // The following for projeciton.
    MATRIX_A,
    VECTOR_B,
    INDEX_M2C,
    INDEX_C2M,
    PROJECTION_LOCAL_N,
    PROJECTION_INTERIOR_N,
    // The following only for projection internal loop.
    PROJECTION_LOCAL_TOLERANCE,
    PROJECTION_GLOBAL_TOLERANCE,
    PROJECTION_GLOBAL_N,
    PROJECTION_DESIRED_ITERATIONS,
    PROJECTION_LOCAL_RESIDUAL,
    PROJECTION_LOCAL_RHO,
    PROJECTION_GLOBAL_RHO,
    PROJECTION_GLOBAL_RHO_OLD,
    PROJECTION_LOCAL_DOT_PRODUCT_FOR_ALPHA,
    PROJECTION_ALPHA,
    PROJECTION_BETA,
    MATRIX_C,
    VECTOR_Z,
    VECTOR_P_META_FORMAT,
    VECTOR_TEMP,
    NUM_VARIABLE
  };
  bool flag_[NUM_VARIABLE];
  DataConfig() {
    SetHelper(false);
  }
  void Clear() {
    SetHelper(false);
  }
  void Set(const DataConfig& data_config) {
    for (int i = 0; i < NUM_VARIABLE; ++i)
      flag_[i] = data_config.flag_[i];
  }
  void SetAll() {
    SetHelper(true);
    UnsetFlag(LEVELSET_READ);
    UnsetFlag(LEVELSET_WRITE);
    UnsetFlag(LEVELSET_BW_SEVEN_READ);
    UnsetFlag(LEVELSET_BW_SEVEN_WRITE);
    UnsetFlag(LEVELSET_BW_EIGHT_READ);
    UnsetFlag(LEVELSET_BW_EIGHT_WRITE);
    UnsetFlag(SHARED_PARTICLES_FLUSH);
  }
  void SetHelper(bool value) {
    for (int i = 0; i < NUM_VARIABLE; ++i)
      flag_[i] = value;
  }
  void SetFlag(const DataType data_type) {
    assert(data_type != NUM_VARIABLE);
    flag_[(int)data_type] = true;
  }
  void UnsetFlag(const DataType data_type) {
    assert(data_type != NUM_VARIABLE);
    flag_[(int)data_type] = false;
  }
  bool GetFlag(const DataType data_type) const {
    assert(data_type != NUM_VARIABLE);
    return flag_[(int)data_type];
  }
};

}  // namespace application

#endif  // NIMBUS_APPLICATION_WATER_MULTIPLE_OPTIONS_H_
