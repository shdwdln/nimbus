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
  * Parser for Nimbus scheduler protocol. 
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include "shared/parser.h"

namespace nimbus {

// TODO(omidm): remove these obsolete parse functions.

void parseCommand(const std::string& str, const CmSet& cms,
                  std::string& cm, std::vector<int>& args) {
  cm.clear();
  args.clear();
  std::set<std::string>::iterator it;
  for (it = cms.begin(); it != cms.end(); ++it) {
    if (str.find(*it) == 0) {
      cm = *it;
      int arg;
      std::stringstream ss;
      ss << str.substr(it->length(), std::string::npos);
      while (true) {
        ss >> arg;
        if (ss.fail()) {
          break;
        }
        args.push_back(arg);
      }
      break;
    }
  }
  if (cm == "") {
    std::cout << "wrong command! try again." << std::endl;
  }
}


int parseCommandFile(const std::string& fname, CmSet& cs) {
  return 0;
}

using boost::tokenizer;
using boost::char_separator;


bool parseCommandFromString(const std::string& input,
                            std::string& command,
                            std::vector<std::string>& parameters) {
  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  if (iter == tokens.end()) {
    command = "";
    return false;
  }

  command = *iter;
  ++iter;
  for (; iter != tokens.end(); ++iter) {
    parameters.push_back(*iter);
  }
  return true;
}

void parseParameterFromString(const std::string& input, std::string& tag,
    std::string& args, std::string& string_set) {
  char_separator<char> separator(":");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  if (iter == tokens.end()) {
    tag = "";
    return;
  }

  tag = *iter;
  iter++;
  if (isSet(*iter))
    string_set = *iter;
  else
    args = *iter;
}

void parseIDSetFromString(const std::string& input, std::set<uint64_t>& set) {
  int num;
  std::string str = input.substr(1, input.length() - 2);
  char_separator<char> separator(",");
  tokenizer<char_separator<char> > tokens(str, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (; iter != tokens.end(); ++iter) {
    std::stringstream ss(*iter);
    ss >> num;
    set.insert(num);
  }
}

void parseIDSetFromString(const std::string& input, std::set<uint32_t>& set) {
  int num;
  std::string str = input.substr(1, input.length() - 2);
  char_separator<char> separator(",");
  tokenizer<char_separator<char> > tokens(str, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (; iter != tokens.end(); ++iter) {
    std::stringstream ss(*iter);
    ss >> num;
    set.insert(num);
  }
}

int countOccurence(std::string str, std::string substr) {
  int count = 0;
  std::size_t pos = -1;

  while (true) {
    pos = str.find(substr, pos + 1);
    if (pos != std::string::npos)
      count++;
    else
      break;
  }
  return count;
}


bool isSet(const std::string& str) {
  if (str.find("{") == std::string::npos)
    return false;
  return true;
}

// ********************************************************

bool ParseSchedulerCommand(const std::string& input,
    SchedulerCommand::TypeSet* command_set,
    std::string& name, std::string& param_segment,
    SchedulerCommand::Type& command_type) {
  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  if (iter == tokens.end()) {
    std::cout << "ERROR: Command is empty." << std::endl;
    return false;
  }
  name = *iter;
  bool name_is_valid = false;
  SchedulerCommand::TypeSet::iterator itr = command_set->begin();
  for (; itr != command_set->end(); itr++) {
    if (name == SchedulerCommand::GetNameFromType(*itr)) {
      name_is_valid = true;
      command_type = *itr;
      break;
    }
  }
  if (!name_is_valid) {
    std::cout << "ERROR: Command name is unknown: " << name << std::endl;
    return false;
  }

  param_segment = input.substr(name.length());
  return true;
}



bool ParseHandshakeCommand(const std::string& input,
    ID<worker_id_t>& worker_id,
    std::string& ip, ID<port_t>& port) {
  int num = 3;
  worker_id_t worker_id_elem;
  port_t port_elem;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: HandshakeCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: HandshakeCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, worker_id_elem)) {
    ID<worker_id_t> temp(worker_id_elem);
    worker_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid worker id." << std::endl;
    return false;
  }

  iter++;
  ip = *iter;

  iter++;
  if (ParseID(*iter, port_elem)) {
    ID<port_t> temp(port_elem);
    port = temp;
  } else {
    std::cout << "ERROR: Could not detect valid port." << std::endl;
    return false;
  }

  return true;
}

bool ParseJobDoneCommand(const std::string& input,
    ID<job_id_t>& job_id,
    IDSet<job_id_t>& after_set,
    Parameter& params) {
  int num = 3;
  job_id_t job_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: JobDoneCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: JobDone has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after_set = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}

bool ParseSpawnJobCommand(const std::string& input,
    std::string& job_name,
    IDSet<job_id_t>& job_id,
    IDSet<data_id_t>& read, IDSet<data_id_t>& write,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after,
    JobType& job_type, Parameter& params) {
  int num = 8;
  IDSet<job_id_t>::IDSetContainer job_id_set;
  IDSet<data_id_t>::IDSetContainer data_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: SpawnJobCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: SpawnJobCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  job_name = *iter;

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    read = temp;
  } else {
    std::cout << "ERROR: Could not detect valid read set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    write = temp;
  } else {
    std::cout << "ERROR: Could not detect valid write set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (*iter == "COMP") {
    job_type = JOB_COMP;
  } else if (*iter == "SYNC") {
    job_type = JOB_SYNC;
  } else {
    std::cout << "ERROR: Unknown job type." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}

bool ParseSpawnComputeJobCommand(const std::string& input,
    std::string& job_name,
    ID<job_id_t>& job_id,
    IDSet<data_id_t>& read, IDSet<data_id_t>& write,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after,
    Parameter& params) {
  int num = 7;
  job_id_t job_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;
  IDSet<data_id_t>::IDSetContainer data_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: SpawnComputeJobCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: SpawnComputeJobCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  job_name = *iter;

  iter++;
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    read = temp;
  } else {
    std::cout << "ERROR: Could not detect valid read set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    write = temp;
  } else {
    std::cout << "ERROR: Could not detect valid write set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}

bool ParseSpawnCopyJobCommand(const std::string& input,
    ID<job_id_t>& job_id,
    ID<data_id_t>& from_id, ID<data_id_t>& to_id,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after,
    Parameter& params) {
  int num = 6;
  job_id_t job_id_elem;
  data_id_t data_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: SpawnCopyJobCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: SpawnCopyJobCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    from_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid from id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    to_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid to id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}

bool ParseComputeJobCommand(const std::string& input,
    std::string& job_name,
    ID<job_id_t>& job_id,
    IDSet<data_id_t>& read, IDSet<data_id_t>& write,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after,
    Parameter& params) {
  int num = 7;
  job_id_t job_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;
  IDSet<data_id_t>::IDSetContainer data_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: ComputeJobCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: ComputeJobCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  job_name = *iter;

  iter++;
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    read = temp;
  } else {
    std::cout << "ERROR: Could not detect valid read set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, data_id_set)) {
    IDSet<data_id_t> temp(data_id_set);
    write = temp;
  } else {
    std::cout << "ERROR: Could not detect valid write set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}


bool ParseCreateDataCommand(const std::string& input,
    ID<job_id_t>& job_id,
    std::string& data_name, ID<data_id_t>& data_id,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after) {
  int num = 5;
  job_id_t job_id_elem;
  data_id_t data_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: CreateDataCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: CreateDataCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  data_name = *iter;

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  return true;
}


bool ParseRemoteCopySendCommand(const std::string& input,
    ID<job_id_t>& job_id,
    ID<job_id_t>& receive_job_id,
    ID<data_id_t>& from_data_id,
    ID<worker_id_t>& to_worker_id,
    std::string& to_ip,
    ID<port_t>& to_port,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after) {
  int num = 8;
  job_id_t job_id_elem;
  data_id_t data_id_elem;
  worker_id_t worker_id_elem;
  port_t port_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: RemoteCopySendCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: RemoteCopySendCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    receive_job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid receive job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    from_data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid from data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, worker_id_elem)) {
    ID<worker_id_t> temp(worker_id_elem);
    to_worker_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid to worker id." << std::endl;
    return false;
  }

  iter++;
  to_ip = *iter;

  iter++;
  if (ParseID(*iter, port_elem)) {
    ID<port_t> temp(port_elem);
    to_port = temp;
  } else {
    std::cout << "ERROR: Could not detect valid to port." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  return true;
}

bool ParseRemoteCopyReceiveCommand(const std::string& input,
    ID<job_id_t>& job_id,
    ID<data_id_t>& to_data_id,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after) {
  int num = 4;
  job_id_t job_id_elem;
  data_id_t data_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: RemoteCopyReceiveCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: RemoteCopyReceiveCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    to_data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid to data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  return true;
}






bool ParseLocalCopyCommand(const std::string& input,
    ID<job_id_t>& job_id,
    ID<data_id_t>& from_data_id,
    ID<data_id_t>& to_data_id,
    IDSet<job_id_t>& before, IDSet<job_id_t>& after) {
  int num = 5;
  job_id_t job_id_elem;
  data_id_t data_id_elem;
  IDSet<job_id_t>::IDSetContainer job_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: LocalCopyCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: LocalCopyCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseID(*iter, job_id_elem)) {
    ID<job_id_t> temp(job_id_elem);
    job_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid job id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    from_data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid from data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    to_data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid to data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    before = temp;
  } else {
    std::cout << "ERROR: Could not detect valid before set." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, job_id_set)) {
    IDSet<job_id_t> temp(job_id_set);
    after = temp;
  } else {
    std::cout << "ERROR: Could not detect valid after set." << std::endl;
    return false;
  }

  return true;
}








bool ParseDefineDataCommand(const std::string& input,
    std::string& data_name,
    ID<data_id_t>& data_id,
    ID<partition_id_t>& partition_id,
    IDSet<partition_id_t>& neighbor_partitions,
    Parameter& params) {
  int num = 5;
  data_id_t data_id_elem;
  partition_id_t partition_id_elem;
  IDSet<partition_id_t>::IDSetContainer partition_id_set;
  SerializedData ser_data;
  IDSet<param_id_t> param_id_set;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: DefineDataCommand has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: DefineDataCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  data_name = *iter;

  iter++;
  if (ParseID(*iter, data_id_elem)) {
    ID<data_id_t> temp(data_id_elem);
    data_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid data id." << std::endl;
    return false;
  }

  iter++;
  if (ParseID(*iter, partition_id_elem)) {
    ID<data_id_t> temp(partition_id_elem);
    partition_id = temp;
  } else {
    std::cout << "ERROR: Could not detect valid partiiton id." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, partition_id_set)) {
    IDSet<data_id_t> temp(partition_id_set);
    neighbor_partitions = temp;
  } else {
    std::cout << "ERROR: Could not detect valid partition neighbor set." << std::endl;
    return false;
  }

  iter++;
  if (ParseParameter(*iter, ser_data, param_id_set)) {
    Parameter temp(ser_data, param_id_set);
    params = temp;
  } else {
    std::cout << "ERROR: Could not detect valid parameter." << std::endl;
    return false;
  }

  return true;
}

bool ParseIDSet(const std::string& input, IDSet<uint64_t>::IDSetContainer& list) {
  uint64_t num;
  list.clear();
  if (input[0] != '{' || input[input.length() - 1] != '}') {
    std::cout << "ERROR: wrong format for IDSet." << std::endl;
    return false;
  }
  std::string str = input.substr(1, input.length() - 2);
  char_separator<char> separator(",");
  tokenizer<char_separator<char> > tokens(str, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (; iter != tokens.end(); ++iter) {
    std::stringstream ss(*iter);
    ss >> num;
    if (ss.fail()) {
      std::cout << "ERROR: wrong element in IDSet." << std::endl;
      return false;
    }
    list.push_back(num);
  }
  return true;
}

bool ParseIDSet(const std::string& input, IDSet<uint32_t>::IDSetContainer& list) {
  uint32_t num;
  list.clear();
  if (input[0] != '{' || input[input.length() - 1] != '}') {
    std::cout << "ERROR: wrong format for IDSet." << std::endl;
    return false;
  }
  std::string str = input.substr(1, input.length() - 2);
  char_separator<char> separator(",");
  tokenizer<char_separator<char> > tokens(str, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (; iter != tokens.end(); ++iter) {
    std::stringstream ss(*iter);
    ss >> num;
    if (ss.fail()) {
      std::cout << "ERROR: wrong element in IDSet." << std::endl;
      return false;
    }
    list.push_back(num);
  }
  return true;
}

bool ParseID(const std::string& input, uint64_t& elem) {
  uint64_t num;
  std::stringstream ss(input);
  ss >> num;
  if (ss.fail()) {
    std::cout << "ERROR: wrong element as ID." << std::endl;
    return false;
  }
  elem = num;
  return true;
}

bool ParseID(const std::string& input, uint32_t& elem) {
  uint32_t num;
  std::stringstream ss(input);
  ss >> num;
  if (ss.fail()) {
    std::cout << "ERROR: wrong element as ID." << std::endl;
    return false;
  }
  elem = num;
  return true;
}

bool ParseWorkerDataHeader(const std::string& input,
    job_id_t& job_id, size_t& data_length) {
  int num = 2;
  job_id_t temp_j;
  size_t temp_d;

  char_separator<char> separator(" \n\t\r");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: Data header has only " << i <<
        " parameters (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: DefineDataCommand has more than "<<
      num << " parameters." << std::endl;
    return false;
  }

  iter = tokens.begin();
  std::stringstream ss_j(*iter);
  ss_j >> temp_j;
  if (ss_j.fail()) {
    std::cout << "ERROR: wrong element for job id." << std::endl;
    return false;
  } else {
    job_id = temp_j;
  }

  iter++;
  std::stringstream ss_d(*iter);
  ss_d >> temp_d;
  if (ss_d.fail()) {
    std::cout << "ERROR: wrong element for data length." << std::endl;
    return false;
  } else {
    data_length = temp_d;
  }
  return true;
}

bool ParseSerializedData(const std::string& input,
    boost::shared_ptr<char>& data_ptr, size_t& size) {
  std::string str = input;
  UnescapeString(&str);
  size = str.length();
  data_ptr = boost::shared_ptr<char>(new char[size]);
  memcpy(get_pointer(data_ptr), str.c_str(), size);
  return true;
}

bool ParseParameter(const std::string& input,
    SerializedData& ser_data, IDSet<param_id_t>& idset) {
  int num = 2;
  IDSet<param_id_t>::IDSetContainer list;
  boost::shared_ptr<char> data_ptr;
  size_t size;

  char_separator<char> separator(":");
  tokenizer<char_separator<char> > tokens(input, separator);
  tokenizer<char_separator<char> >::iterator iter = tokens.begin();
  for (int i = 0; i < num; i++) {
    if (iter == tokens.end()) {
      std::cout << "ERROR: Parameter has only " << i <<
        " fields (expected " << num << ")." << std::endl;
      return false;
    }
    iter++;
  }
  if (iter != tokens.end()) {
    std::cout << "ERROR: Parameter has more than "<<
      num << " fields." << std::endl;
    return false;
  }

  iter = tokens.begin();
  if (ParseSerializedData(*iter, data_ptr, size)) {
    SerializedData temp(data_ptr, size);
    ser_data = temp;
  } else {
    std::cout << "ERROR: Could not detect valid serialized data." << std::endl;
    return false;
  }

  iter++;
  if (ParseIDSet(*iter, list)) {
    IDSet<param_id_t> temp(list);
    idset = temp;
  } else {
    std::cout << "ERROR: Could not detect valid idset." << std::endl;
    return false;
  }

  return true;
}



}  // namespace nimbus

