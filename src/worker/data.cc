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
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "src/data/app_data/app_data_defs.h"
#include "src/data/app_data/app_object.h"
#include "src/worker/data.h"

using namespace nimbus; // NOLINT

Data::Data() {
  dirty_app_object_ = NULL;
  pending_flag_ = 0;
  destroyed_ = false;
}

Data* Data::Clone() {
  std::cout << "cloning the base data\n";
  Data* d = new Data();
  return d;
}

logical_data_id_t Data::logical_id() {
  return logical_id_;
}

physical_data_id_t Data::physical_id() {
  return physical_id_;
}

std::string Data::name() {
    return name_;
}

GeometricRegion Data::region() {
    return region_;
}

data_version_t Data::version() {
  return version_;
}

void Data::set_logical_id(logical_data_id_t logical_id) {
  logical_id_ = logical_id;
}

void Data::set_physical_id(physical_data_id_t physical_id) {
  physical_id_ = physical_id;
}
int Data::get_debug_info() {
    return -1;
}

void Data::set_name(std::string name) {
    name_ = name;
}

void Data::set_region(const GeometricRegion& region) {
    region_ = region;
}

void Data::set_version(data_version_t version) {
  version_ = version;
}

bool Data::destroyed() {
  return destroyed_;
}

void Data::set_destroyed(bool destroyed) {
  destroyed_ = destroyed;
}

/**
 *
 */
void Data::ClearDirtyMappings() {
  assert(dirty_app_object_);
  if (dirty_app_object_) {
    dirty_app_object_->UnsetDirtyData(this);
    dirty_app_object_ = NULL;
  }
}

/**
 * \details
 */
void Data::InvalidateMappings() {
  std::set<AppObject *>::iterator iter = app_objects_.begin();
  for (; iter != app_objects_.end(); ++iter) {
    AppObject *c = *iter;
    c->UnsetData(this);
  }
  app_objects_.clear();
  if (dirty_app_object_)
    dirty_app_object_->UnsetDirtyData(this);
  dirty_app_object_ = NULL;
}

/**
 * \details
 */
void Data::SetUpAppObject(AppObject *co) {
  app_objects_.insert(co);
}

/**
 * \details
 */
void Data::UnsetAppObject(AppObject *co) {
  app_objects_.erase(co);
}

/**
 * \detials
 */
AppObject *Data::dirty_app_object() {
  return dirty_app_object_;
}

/**
 * \details
 */
void Data::SetUpDirtyAppObject(AppObject *co) {
  assert(dirty_app_object_ == NULL || dirty_app_object_ == co);
  dirty_app_object_ = co;
}

/**
 * \details
 */
void Data::UnsetDirtyAppObject(AppObject *co) {
  if (dirty_app_object_ != co)
    dbg(DBG_ERROR, "Data dirty co %p, passed co %p\n", dirty_app_object_, co);
  assert(dirty_app_object_ == co);
  dirty_app_object_ = NULL;
}

/**
 * \detials
 */
app_data::type_id_t Data::app_data_type() {
    return app_data_type_;
}

/**
 * \details
 */
void Data::set_app_data_type(app_data::type_id_t t) {
    app_data_type_ = t;
}
