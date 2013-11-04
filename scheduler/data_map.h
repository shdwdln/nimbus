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
  * Data structure for maintaining and querying for the physical instances
  * of each logical data object.
  * 
  */

#ifndef NIMBUS_SCHEDULER_DATA_MAP_H_
#define NIMBUS_SCHEDULER_DATA_MAP_H_

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "data/physical_data.h"
#include "shared/logical_data_object.h"
#include "shared/geometric_region.h"
#include "shared/nimbus_types.h"


namespace nimbus {

  typedef std::map<data_id_t, PhysicalDataVector*> DataMapType;

  class DataMap {
  public:
    DataMap();
    virtual ~DataMap();

    virtual bool AddLogicalObject(LogicalDataObject* object);
    virtual bool RemoveLogicalObject(data_id_t id);
    virtual bool AddPhysicalInstance(LogicalDataObject* object,
                                     PhysicalData instance);
    virtual bool RemovePhysicalInstance(LogicalDataObject* object,
                                        PhysicalData instance);


    virtual const PhysicalDataVector* AllInstances(LogicalDataObject* object);
    virtual int AllInstances(LogicalDataObject* object,
                             PhysicalDataVector* dest);
    virtual int InstancesByWorker(LogicalDataObject* object,
                                  worker_id_t worker,
                                  PhysicalDataVector* dest);
    virtual int InstancesByVersion(LogicalDataObject* object,
                                   data_version_t version,
                                   PhysicalDataVector* dest);
  private:
    DataMapType data_map_;
  };
}  // namespace nimbus

#endif  // NIMBUS_SCHEDULER_DATA_MAP_H_
