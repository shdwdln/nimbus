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
 * Data classes are implemented here.
 *
 */

#include <stdlib.h>
#include "./data.h"

using vector_msg::VectorMsg;

Vec::Vec() {
  data_ = NULL;
  size_ = 0;
};

Vec::~Vec() {
};

Data* Vec::Clone() {
  return new Vec();
};

void Vec::Create() {
  size_ = abs(region().dx() * region().dy() * region().dz());
  data_ = new double[size_];
};

void Vec::Destroy() {
  if (data_ != NULL) {
    delete data_;
  }
  size_ = 0;
};

void Vec::Copy(Data* from) {
  Vec *d = static_cast<Vec*>(from);
  assert(d);
  assert(size_ == d->size_);
  for (size_t i = 0; i < size_; ++i) {
    data_[i] = d->data_[i];
  }
}

bool Vec::Serialize(SerializedData* ser_data) {
  VectorMsg vec_msg;
  for (size_t i = 0; i < size_; i++) {
    vec_msg.add_elem(data_[i]);
  }
  std::string str;
  vec_msg.SerializeToString(&str);
  char* ptr = new char[str.length()];
  memcpy(ptr, str.c_str(), str.length());
  ser_data->set_data_ptr(ptr);
  ser_data->set_size(str.length());
  return true;
}

bool Vec::DeSerialize(const SerializedData& ser_data, Data** result) {
  VectorMsg vec_msg;
  std::string str(ser_data.data_ptr_raw(), ser_data.size());
  vec_msg.ParseFromString(str);
  Vec* vec = new Vec();
  vec->data_ = new double[size_];
  vec->size_ = size_;
  for (size_t i = 0; (i < size_) && (i < (size_t)vec_msg.elem_size()); ++i)
     vec->data_[i] = vec_msg.elem(i);

  *result = vec;
  return true;
}

size_t Vec::size() {
  return size_;
}

double* Vec::data() {
  return data_;
}

