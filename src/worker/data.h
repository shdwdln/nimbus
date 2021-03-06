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
  * Nimbus abstraction of data.
  *
  * Nimbus calls following chain of methods:
  *   Clone -> Create
  *   Clone -> Create -> Serialize
  *   Clone -> Create -> Deserialize -> Copy
  *
  * Before Create gets called logical_id, phisical_id, and region are set for
  * the data. For more information see worker/worker.cc and worker/job.cc files.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#ifndef NIMBUS_SRC_WORKER_DATA_H_
#define NIMBUS_SRC_WORKER_DATA_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include "src/data/app_data/app_data_defs.h"
#include "src/shared/cluster.h"
#include "src/shared/idset.h"
#include "src/shared/serialized_data.h"
#include "src/shared/nimbus_types.h"

namespace nimbus {
class Data;
typedef std::set<Data*> Neighbors;
typedef std::map<logical_data_id_t, Data*> LogicalDataMap;
// typedef std::map<physical_data_id_t, Data*> PhysicalDataMap;
typedef std::map<std::string, Data*> DataTable;

// forward declaration
class AppObject;

class Data {
 public:
  enum Use { NONE, READ, WRITE };

  Data();
  virtual ~Data() {}

  virtual Data* Clone();
  virtual void Create() {}
  virtual void Destroy() {}

  virtual float FloatingHash() {
    return 0;
  }

  virtual uint32_t HashCode() {
    return 0;
  }

  virtual void Copy(Data* from) {}
  virtual bool Serialize(SerializedData* ser_data) {return false;}
  virtual bool DeSerialize(const SerializedData& ser_data, Data** result) {return false;}


  virtual void duplicate(Computer source, Computer destination) {}
  virtual void migrate(Computer sourcer, Computer destination) {}
  virtual void split(Data *, Data *) {}
  virtual void merge(Data *, Data *) {}

  virtual size_t memory_size() {
    return sizeof(*this);
  }

  virtual int get_debug_info();

  logical_data_id_t logical_id();
  physical_data_id_t physical_id();
  std::string name();
  GeometricRegion region();
  data_version_t version();

  void set_logical_id(logical_data_id_t logical_id);
  void set_physical_id(physical_data_id_t physical_id);
  void set_name(std::string name);
  void set_region(const GeometricRegion& region);
  void set_version(data_version_t version);

  bool destroyed();
  void set_destroyed(bool destroyed);

  /**
   * \brief Removes dirty object mappings
   */
  void ClearDirtyMappings();

  /**
   * \brief Removes all mappings between this data instance and all other cache
   * instances (dirty and non-dirty)
   */
  void InvalidateMappings();

  /**
   * \brief Inserts a mapping between this data instance and app object ob
   * \param ob is the app object to map to
   */
  void SetUpAppObject(AppObject *ob);

  /**
   * \brief Removes mapping between this data instance and app object ob
   * \param ob is the cache object to unmap
   */
  void UnsetAppObject(AppObject *ob);

  /**
   * \brief Accessor for dirty data
   * \return App object
   */
  AppObject *dirty_app_object();

  /**
   * \brief Inserts a dirty object mapping between this data instance and app
   * object ob
   * \param ob is the app object to map to
   */
  void SetUpDirtyAppObject(AppObject *ob);

  /**
   * \brief Removes dirty object mapping between this data instance and app
   * object ob
   * \param ob is the app object to unmap
   */
  void UnsetDirtyAppObject(AppObject *ob);

  /**
   * \brief Accessor for application var type
   * \return Cache variable type (used if cache object is cache struct)
   */
  app_data::type_id_t app_data_type();

  /**
   * \brief Setter for application var type
   * \param Cache variable type (used if cache object is cache struct)
   */
  void set_app_data_type(app_data::type_id_t t);

  size_t app_ob_size() { return app_objects_.size(); }

  int pending_flag() {
    return pending_flag_;
  }
  void set_pending_flag(Use use) {
    if (use == READ) {
      pending_flag_++;
    } else if (use == WRITE) {
      assert(pending_flag_ == 0);
      pending_flag_--;
    }
  }
  void unset_pending_flag(Use use) {
    if (use == READ) {
      pending_flag_--;
    } else if (use == WRITE) {
      assert(pending_flag_ == -1);
      pending_flag_++;
    }
  }

 private:
  int pending_flag_;
  logical_data_id_t logical_id_;
  physical_data_id_t physical_id_;
  partition_id_t partition_id_;
  GeometricRegion region_;
  std::string name_;
  data_version_t version_;
  bool advanceData_;

  // destroyed_ is true after Destroy is called (memory is freed) by
  // an app data manager (default at init = false).
  // It should be set to false after Create in app data manager.
  bool destroyed_;

  // Set of data ids that could be involved in SYNC jobs with this data.
  IDSet<logical_data_id_t> neighbors_;

  // Set of partition ids neighbor to this partition.
  IDSet<partition_id_t> neighbor_partitions_;

  // Set of application objects that this data corresponds to
  std::set<AppObject *> app_objects_;
  AppObject *dirty_app_object_;
  app_data::type_id_t app_data_type_;
};

typedef std::vector<Data*> DataArray;

}  // namespace nimbus

#endif  // NIMBUS_SRC_WORKER_DATA_H_
