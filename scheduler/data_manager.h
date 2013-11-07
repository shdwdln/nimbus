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
  * The Data Manager maintains the controller's metadata on what
  * logical data objects are in the system as well as their physical
  * instances.
  */

#ifndef NIMBUS_SCHEDULER_DATA_MANAGER_H_
#define NIMBUS_SCHEDULER_DATA_MANAGER_H_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "shared/nimbus_types.h"
#include "shared/logical_data_object.h"
#include "data/physical_data.h"
#include "shared/ldo_index.h"
#include "scheduler/data_map.h"

namespace nimbus {

  class DataManager {
  public:
    DataManager();
    virtual ~DataManager();

    bool AddPartition(partition_id_t id, GeometricRegion region);
    bool RemovePartition(partition_id_t id);
    bool HasPartition(partition_id_t id);
    GeometricRegion FindPartition(partition_id_t id);

    bool AddLogicalObject(data_id_t id,
                          std::string variable,
                          GeometricRegion region);
    bool AddLogicalObject(data_id_t id,
                          std::string variable,
                          partition_id_t partition);

    bool RemoveLogicalObject(data_id_t id);

    const LogicalDataObject* FindLogicalObject(data_id_t id);
    int FindLogicalObjects(std::string variable,
                           CLdoVector* dest);
    int FindIntersectingLogicalObjects(std::string variable,
                                       GeometricRegion* region,
                                       CLdoVector* dest);
    int FindCoveredLogicalObjects(std::string variable,
                                  GeometricRegion* region,
                                  CLdoVector* dest);
    int FindAdjacentLogicalObjects(std::string variable,
                                   GeometricRegion* region,
                                   CLdoVector* dest);

    bool AddPhysicalInstance(LogicalDataObject* object,
                             PhysicalData instance);
    bool RemovePhysicalInstance(LogicalDataObject* object,
                                PhysicalData instance);

    const PhysicalDataVector* AllInstances(LogicalDataObject* object);
    int AllInstances(LogicalDataObject* object,
                     PhysicalDataVector* dest);
    int InstancesByWorker(LogicalDataObject* object,
                          worker_id_t worker,
                          PhysicalDataVector* dest);
    int InstancesByVersion(LogicalDataObject* object,
                           data_version_t version,
                           PhysicalDataVector* dest);

  private:
    DataMap data_map_;
    LdoIndex ldo_index_;
    std::map<data_id_t, LogicalDataObject*> ldo_map_;
    std::map<partition_id_t, GeometricRegion> partition_map_;
  };
}  // namespace nimbus

#endif  // NIMBUS_SCHEDULER_DATA_MANAGER_H_
