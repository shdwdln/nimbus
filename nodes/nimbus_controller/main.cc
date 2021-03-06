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
  * This file has the main function that launches Nimbus controller.
  *
  * Author: Omid Mashayekhi <omidm@stanford.edu>
  */

#include <boost/program_options.hpp>
#include <iostream> // NOLINT
#include "src/shared/nimbus.h"
#include "src/shared/nimbus_types.h"
#include "src/shared/scheduler_command.h"
#include "src/shared/parser.h"
#include "src/scheduler/scheduler.h"

using namespace nimbus; // NOLINT

int main(int argc, char *argv[]) {
  namespace po = boost::program_options;

  port_t listening_port;
  size_t worker_num;
  size_t assign_batch_size;
  size_t assign_thread_num;
  size_t command_batch_size;
  int64_t lb_period;
  int64_t ft_period;
  std::vector<size_t> split;
  std::vector<size_t> sub_split;
  std::vector<int_dimension_t> region;


  po::options_description desc("Nimbus Controller Options");
  desc.add_options()
    ("help,h", "produce help message")
    // Required arguments
    ("port,p", po::value<port_t>(&listening_port)->required(), "listening port")

    // Optinal arguments
    ("worker_num,w", po::value<size_t>(&worker_num), "number of initial workers")
    ("assign_batch_size,a", po::value<size_t>(&assign_batch_size), "maximum number of jobs in a batch assignment") //NOLINT
    ("assign_thread_num,t", po::value<size_t>(&assign_thread_num), "job assigner thread nums")
    ("command_batch_size,c", po::value<size_t>(&command_batch_size), "maximum number of commands in a batch processing") //NOLINT
    ("lb_period", po::value<int64_t>(&lb_period), "query period for load balancing (seconds)") //NOLINT
    ("ft_period", po::value<int64_t>(&ft_period), "checkpoint creation period for fault tolerance (seconds)") //NOLINT
    ("split", po::value<std::vector<size_t> >(&split)->multitoken(), "three space separated positive integres to sepcify the number of partitions per dimension: nx ny nz") // NOLINT
    ("sub_split", po::value<std::vector<size_t> >(&sub_split)->multitoken(), "three space separated positive integres to sepcify the number of sub-partitions per dimension: nx ny nz") // NOLINT
    ("region", po::value<std::vector<int_dimension_t> >(&region)->multitoken(), "six space separated integres to sepcify the global region: x y z dx dy dz") // NOLINT

    ("dct", "deactivate controller template")
    ("dcm", "deactivate complex memoization")
    ("dbm", "deactivate binding memoization")
    ("dwt", "deactivate worker template")
    ("dmr", "deactivate mega rcr job")
    ("dcb", "deactivate cascaded binding")

    ("alb", "activate load balancing")
    ("aft", "activate fault tolerance");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
  } catch (std::exception& e) { // NOLINT
    std::cerr << "ERROR: " << e.what() << "\n";
    return -1;
  }

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  try {
    po::notify(vm);
  } catch (std::exception& e) { // NOLINT
    std::cerr << "ERROR: " << e.what() << "\n";
    return -1;
  }


  nimbus::nimbus_initialize();

  Scheduler * s = new Scheduler(listening_port);

  if (vm.count("worker_num")) {
    s->set_init_worker_num(worker_num);
  }

  if (vm.count("assign_batch_size")) {
    s->set_assign_batch_size(assign_batch_size);
  }

  if (vm.count("assign_thread_num")) {
    s->set_job_assigner_thread_num(assign_thread_num);
  }

  if (vm.count("command_batch_size")) {
    s->set_command_batch_size(command_batch_size);
  }

  if (vm.count("dct")) {
    s->set_controller_template_active(false);
  }

  if (vm.count("dcm")) {
    s->set_complex_memoization_active(false);
  }

  if (vm.count("dbm")) {
    s->set_binding_memoization_active(false);
  }

  if (vm.count("dwt")) {
    s->set_worker_template_active(false);
  }

  if (vm.count("dmr")) {
    s->set_mega_rcr_job_active(false);
  }

  if (vm.count("dcb")) {
    s->set_cascaded_binding_active(false);
  }

  if (vm.count("lb_period")) {
    s->set_load_balancing_period(lb_period);
  }

  if (vm.count("ft_period")) {
    s->set_checkpoint_creation_period(ft_period);
  }

  if (vm.count("alb")) {
    s->set_load_balancing_active(true);
  }

  if (vm.count("split")) {
    if (split.size() != 3) {
      std::cerr << "ERROR: split need exactly 3 integers\n";
      return -1;
    }
    if (!(split[0] && split[1] && split[2])) {
        std::cerr << "ERROR: split integers should be positive\n";
        return -1;
    }
    s->set_split_dimensions(split);
  }

  if (vm.count("sub_split")) {
    if (sub_split.size() != 3) {
      std::cerr << "ERROR: sub_split need exactly 3 integers\n";
      return -1;
    }
    if (!(sub_split[0] && sub_split[1] && sub_split[2])) {
        std::cerr << "ERROR: sub_split integers should be positive\n";
        return -1;
    }
    s->set_sub_split_dimensions(sub_split);
  }

  if (vm.count("region")) {
    if (region.size() != 6) {
      std::cerr << "ERROR: global region needs exactly 6 integers\n";
      return -1;
    }
    s->set_global_region(
        GeometricRegion(region[0],
                        region[1],
                        region[2],
                        region[3],
                        region[4],
                        region[5]));
  }

  if (vm.count("aft")) {
    s->set_fault_tolerance_active(true);
  }

  s->Run();
}

