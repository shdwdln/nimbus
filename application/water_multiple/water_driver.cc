//#####################################################################
// Copyright 2009, Michael Lentine.
// This file is part of PhysBAM whose distribution is governed by the license contained in the accompanying file PHYSBAM_COPYRIGHT.txt.
//#####################################################################
#include "stdio.h"
#include "string.h"

#include <PhysBAM_Tools/Grids_Uniform/UNIFORM_GRID_ITERATOR_FACE.h>
#include <PhysBAM_Tools/Log/DEBUG_SUBSTEPS.h>
#include <PhysBAM_Tools/Log/LOG.h>
#include <PhysBAM_Tools/Parallel_Computation/DOMAIN_ITERATOR_THREADED.h>
#include <PhysBAM_Tools/Parallel_Computation/PCG_SPARSE_THREADED.h>
#include <PhysBAM_Tools/Vectors/VECTOR_UTILITIES.h>
#include <PhysBAM_Geometry/Grids_Uniform_Interpolation_Collidable/LINEAR_INTERPOLATION_COLLIDABLE_CELL_UNIFORM.h>
#include <PhysBAM_Geometry/Grids_Uniform_Interpolation_Collidable/LINEAR_INTERPOLATION_COLLIDABLE_FACE_UNIFORM.h>
#include <PhysBAM_Fluids/PhysBAM_Incompressible/Incompressible_Flows/PROJECTION_FREE_SURFACE_REFINEMENT_UNIFORM.h>
#include <PhysBAM_Dynamics/Boundaries/BOUNDARY_PHI_WATER.h>

#include "application/water_multiple/app_utils.h"
#include "application/water_multiple/data_names.h"
#include "application/water_multiple/job_names.h"
#include "application/water_multiple/parameters.h"
#include "application/water_multiple/projection/laplace_solver_wrapper.h"
#include "application/water_multiple/projection/projection_helper.h"
#include "application/water_multiple/water_driver.h"
#include "application/water_multiple/water_example.h"
#include "data/physbam/translator_physbam.h"
#include "shared/dbg_modes.h"
#include "shared/dbg.h"
#include "shared/geometric_region.h"
#include "shared/nimbus.h"

using namespace PhysBAM;
namespace{
    template<class TV> void Write_Substep_Helper(void* writer,const std::string& title,int substep,int level)
    {
        ((WATER_DRIVER<TV>*)writer)->Write_Substep(title,substep,level);
    }
};
//#####################################################################
// Initialize
//#####################################################################
template<class TV> WATER_DRIVER<TV>::
    WATER_DRIVER(WATER_EXAMPLE<TV>& example)
:example(example)
{
    DEBUG_SUBSTEPS::Set_Substep_Writer((void*)this,&Write_Substep_Helper<TV>);
}
//#####################################################################
// Initialize
//#####################################################################
template<class TV> WATER_DRIVER<TV>::
~WATER_DRIVER()
{
    DEBUG_SUBSTEPS::Clear_Substep_Writer((void*)this);
}
//#####################################################################
// Run
//#####################################################################
template<class TV> void WATER_DRIVER<TV>::
Run(RANGE<TV_INT>& domain,const T dt,const T time)
{
    T_FACE_ARRAYS_SCALAR face_velocities_ghost = example.face_velocities_ghost;
    LINEAR_INTERPOLATION_UNIFORM<GRID<TV>,TV> interpolation;
    PARTICLE_LEVELSET_UNIFORM<GRID<TV> >& pls = example.particle_levelset_evolution.particle_levelset;
    if (pls.use_removed_positive_particles) {
        for (typename GRID<TV>::NODE_ITERATOR iterator(example.mac_grid,domain);
            iterator.Valid();
            iterator.Next()) {
            if(pls.removed_positive_particles(iterator.Node_Index())) {
                PARTICLE_LEVELSET_REMOVED_PARTICLES<TV>& particles =
                    *pls.removed_positive_particles(iterator.Node_Index());
                for (int p = 1; p <= particles.array_collection->Size(); p++) {
                    TV X = particles.X(p);
                    TV V = interpolation.Clamped_To_Array_Face(example.mac_grid,
                                                               face_velocities_ghost,
                                                               X);
                if (-pls.levelset.Phi(X) > 1.5*particles.radius(p))
                    V-=-TV::Axis_Vector(2)*.3; // buoyancy
                particles.V(p)=V;
                }
            }
        }
    }
    if (pls.use_removed_negative_particles) {
        for (typename GRID<TV>::NODE_ITERATOR iterator(example.mac_grid,domain);
             iterator.Valid();
             iterator.Next()) {
            if (pls.removed_negative_particles(iterator.Node_Index())) {
                PARTICLE_LEVELSET_REMOVED_PARTICLES<TV>& particles =
                    *pls.removed_negative_particles(iterator.Node_Index());
                for (int p = 1; p <= particles.array_collection->Size(); p++)
                    particles.V(p) += -TV::Axis_Vector(2)*dt*9.8; // ballistic
                for (int p = 1; p <= particles.array_collection->Size(); p++) {
                    particles.V(p) += dt*interpolation.
                        Clamped_To_Array_Face(example.mac_grid,
                                              example.incompressible.force,
                                              particles.X(p)); // external forces
                }
            }
        }
    }
}



// Substep without reseeding and writing to frame.
// Operation on time should be solved carefully. --quhang
template<class TV> void WATER_DRIVER<TV>::
CalculateFrameImpl(const nimbus::Job *job,
                   const nimbus::DataArray &da,
                   const bool set_boundary_conditions,
                   const T dt) {
  // LOG::Time("Calculate Dt");
  // example.particle_levelset_evolution.Set_Number_Particles_Per_Cell(16);
  // T dt=example.cfl*example.incompressible.CFL(example.face_velocities);dt=min(dt,example.particle_levelset_evolution.CFL(false,false));
  // if(time+dt>=target_time){dt=target_time-time;done=true;}
  // else if(time+2*dt>=target_time){dt=.5*(target_time-time);}

  //LOG::Time("Compute Occupied Blocks");
  // T maximum_fluid_speed=example.face_velocities.Maxabs().Max();
  // T max_particle_collision_distance=example.particle_levelset_evolution.particle_levelset.max_collision_distance_factor*example.mac_grid.dX.Max();
  // example.collision_bodies_affecting_fluid.Compute_Occupied_Blocks(true,dt*maximum_fluid_speed+2*max_particle_collision_distance+(T).5*example.mac_grid.dX.Max(),10);
  //example.collision_bodies_affecting_fluid.Compute_Occupied_Blocks(true,dt*maximum_fluid_speed+2*max_particle_collision_distance+(T).5*example.mac_grid.dX.Max(),10);

  LOG::Time("Adjust Phi With Objects");
  T_FACE_ARRAYS_SCALAR face_velocities_ghost;face_velocities_ghost.Resize(example.incompressible.grid,example.number_of_ghost_cells,false);
  example.incompressible.boundary->Fill_Ghost_Cells_Face(example.mac_grid,example.face_velocities,face_velocities_ghost,time+dt,example.number_of_ghost_cells);

  //Advect Phi 3.6% (Parallelized)
  LOG::Time("Advect Phi");
  example.phi_boundary_water.Use_Extrapolation_Mode(false);
  assert(example.particle_levelset_evolution.runge_kutta_order_levelset == 1);
  example.particle_levelset_evolution.levelset_advection.Euler_Step(
      example.face_velocities,
      dt, time,
      example.particle_levelset_evolution.particle_levelset.
      number_of_ghost_cells);

  example.particle_levelset_evolution.time += dt;
  example.phi_boundary_water.Use_Extrapolation_Mode(true);

  //Advect Particles 12.1% (Parallelized)
  LOG::Time("Step Particles");
  example.particle_levelset_evolution.particle_levelset.Euler_Step_Particles(face_velocities_ghost,dt,time,true,true,false,false);

  //Advect removed particles (Parallelized)
  LOG::Time("Advect Removed Particles");
  RANGE<TV_INT> domain(example.mac_grid.Domain_Indices());domain.max_corner+=TV_INT::All_Ones_Vector();
  DOMAIN_ITERATOR_THREADED_ALPHA<WATER_DRIVER<TV>,TV>(domain,0).template Run<T,T>(*this,&WATER_DRIVER<TV>::Run,dt,time);

  //Advect Velocities 26% (Parallelized)
  LOG::Time("Advect V");
  example.incompressible.advection->Update_Advection_Equation_Face(example.mac_grid,example.face_velocities,face_velocities_ghost,face_velocities_ghost,*example.incompressible.boundary,dt,time);

  //Add Forces 0%
  LOG::Time("Forces");
  example.incompressible.Advance_One_Time_Step_Forces(example.face_velocities,dt,time,true,0,example.number_of_ghost_cells);

  //Modify Levelset with Particles 15% (Parallelizedish)
  LOG::Time("Modify Levelset");
  example.particle_levelset_evolution.particle_levelset.Exchange_Overlap_Particles();
  example.particle_levelset_evolution.Modify_Levelset_And_Particles(&face_velocities_ghost);

  //Adjust Phi 0%
  LOG::Time("Adjust Phi");
  example.Adjust_Phi_With_Sources(time+dt);

  //Delete Particles 12.5 (Parallelized)
  LOG::Time("Delete Particles");
  example.particle_levelset_evolution.Delete_Particles_Outside_Grid();                                                            //0.1%
  example.particle_levelset_evolution.particle_levelset.Delete_Particles_In_Local_Maximum_Phi_Cells(1);                           //4.9%
  example.particle_levelset_evolution.particle_levelset.Delete_Particles_Far_From_Interface(); // uses visibility                 //7.6%
  example.particle_levelset_evolution.particle_levelset.Identify_And_Remove_Escaped_Particles(face_velocities_ghost,1.5,time+dt); //2.4%

  //Reincorporate Particles 0% (Parallelized)
  LOG::Time("Reincorporate Particles");
  if(example.particle_levelset_evolution.particle_levelset.use_removed_positive_particles || example.particle_levelset_evolution.particle_levelset.use_removed_negative_particles)
    example.particle_levelset_evolution.particle_levelset.Reincorporate_Removed_Particles(1,1,0,true);

  //Project 7% (Parallelizedish)
  LOG::SCOPE *scope=0;
  scope=new LOG::SCOPE("Project");
  if (set_boundary_conditions)
    example.Set_Boundary_Conditions(time);
  example.incompressible.Set_Dirichlet_Boundary_Conditions(&example.particle_levelset_evolution.phi,0);
  example.projection.p*=dt;
  // example.projection.collidable_solver->Set_Up_Second_Order_Cut_Cell_Method();
  example.incompressible.Advance_One_Time_Step_Implicit_Part(example.face_velocities,dt,time,true);
  example.projection.p*=(1/dt);
  example.incompressible.boundary->Apply_Boundary_Condition_Face(example.incompressible.grid,example.face_velocities,time+dt);
  // example.projection.collidable_solver->Set_Up_Second_Order_Cut_Cell_Method(false);
  delete scope;

  //Extrapolate Velocity 7%
  LOG::Time("Extrapolate Velocity");
  T_ARRAYS_SCALAR exchanged_phi_ghost(example.mac_grid.Domain_Indices(8));
  example.particle_levelset_evolution.particle_levelset.levelset.boundary->Fill_Ghost_Cells(example.mac_grid,example.particle_levelset_evolution.phi,exchanged_phi_ghost,0,time+dt,8);
  example.incompressible.Extrapolate_Velocity_Across_Interface(example.face_velocities,exchanged_phi_ghost,false,3,0,TV());

  // TODO(quhang) Take care of this!
  // time+=dt;

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame+1);
}

// Substep with reseeding and writing to frame.
// Operation on time should be solved carefully. --quhang
template<class TV> void WATER_DRIVER<TV>::
WriteFrameImpl(const nimbus::Job *job,

               const nimbus::DataArray &da,
               const bool set_boundary_conditions,
               const T dt) {
  // Comments(quhang): Notice time has already been increased here.
  // Not sure if the Set_Number_Particles_Per_Cell function should go to
  // initalization.
  // example.particle_levelset_evolution.Set_Number_Particles_Per_Cell(16);

  //Reseed
  LOG::Time("Reseed");
  example.particle_levelset_evolution.Reseed_Particles(time);
  example.particle_levelset_evolution.Delete_Particles_Outside_Grid();

  // I changed the order. --quhang
  Write_Output_Files(++output_number);

  //Save State
  // example.Save_To_Nimbus(job, da, current_frame+1);
}

template<class TV> bool WATER_DRIVER<TV>::
ProjectionCalculateBoundaryConditionPartOneImpl (
    const nimbus::Job *job,
    const nimbus::DataArray &da,
    T dt) {
  INCOMPRESSIBLE_UNIFORM<GRID<TV> >& incompressible = example.incompressible;

  // Sets boundary conditions.
  // Local.
  // Read velocity and pressure. Write velocity, pressure, psi_D, and psi_N.
  example.Set_Boundary_Conditions(time);

  // Sets dirichlet boundary conditions in the air.
  // Remote: exchange psi_D and pressure afterwards.
  // Read levelset. Write psi_D and pressure.
  incompressible.Set_Dirichlet_Boundary_Conditions(
      &example.particle_levelset_evolution.phi, 0);
  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ProjectionCalculateBoundaryConditionPartTwoImpl (
    const nimbus::Job *job,
    const nimbus::DataArray &da,
    T dt) {
  PROJECTION_DYNAMICS_UNIFORM<GRID<TV> >& projection = example.projection;
  LAPLACE_COLLIDABLE_UNIFORM<T_GRID>& laplace_solver =
      *dynamic_cast<LAPLACE_COLLIDABLE_UNIFORM<T_GRID>* >(
          projection.elliptic_solver);
  // Scales pressure.
  // Read/Write pressure.
  projection.p *= dt;

  typedef typename INTERPOLATION_POLICY<GRID<TV> >::FACE_LOOKUP
      T_FACE_LOOKUP;

  // Computes divergence.
  // Local.
  // Read velocity. Write divergence(solver->f).
  projection.Compute_Divergence(
      T_FACE_LOOKUP(example.face_velocities),
      &laplace_solver);

  // Coloring.
  // Local.
  // Read psi_D, psi_N.
  // Write filled_region_colors.
  FillUniformRegionColor(
      laplace_solver.grid,
      laplace_solver.psi_D, laplace_solver.psi_N,
      false, &laplace_solver.filled_region_colors);

  return true;
}

// TAG_PROJECTION
template<class TV> bool WATER_DRIVER<TV>::
ProjectionConstructMatrixImpl (
    const nimbus::Job *job,
    const nimbus::DataArray &da,
    T dt) {
  // Read psi_N, psi_D, filled_region_colors, divergence, pressure.
  // Write A, b, x, tolerance, indexing.
  example.laplace_solver_wrapper.PrepareProjectionInput();
  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ProjectionWrapupImpl(
    const nimbus::Job *job,
    const nimbus::DataArray &da,
    T dt) {
  // Applies pressure.
  // Local.
  // Read pressure(u/p), levelset, psi_D, psi_N, u_interface, velocity.
  // Write velocity.
  example.projection.Apply_Pressure(example.face_velocities, dt, time);

  // Scales pressure.
  // Read/Write pressure.
  example.projection.p *= (1/dt);
  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ExtrapolationImpl (const nimbus::Job *job,
                 const nimbus::DataArray &da,
                 T dt) {
  example.incompressible.Extrapolate_Velocity_Across_Interface(
      example.face_velocities,
      example.phi_ghost_bandwidth_eight,
      false, 3, 0, TV());

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
UpdateGhostVelocitiesImpl (const nimbus::Job *job,
                          const nimbus::DataArray &da,
                          T dt) {
  LOG::Time("Adjust Phi With Objects");
  example.incompressible.boundary->Fill_Ghost_Cells_Face(
      example.mac_grid, example.face_velocities, example.face_velocities_ghost,
      time + dt, example.number_of_ghost_cells);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ExtrapolatePhiImpl(const nimbus::Job *job,
                   const nimbus::DataArray &da,
                   T dt) {
  // example.phi_boundary_water.Use_Extrapolation_Mode(false);
  assert(example.particle_levelset_evolution.runge_kutta_order_levelset == 1);
  {
    example.particle_levelset_evolution.particle_levelset.levelset.boundary->Fill_Ghost_Cells(
        example.mac_grid,
        example.particle_levelset_evolution.phi,
        example.phi_ghost_bandwidth_eight,
        0, time+dt, 8);
    std::cout << "OMID: called extrapolate phi.\n";
  }
  // example.phi_boundary_water.Use_Extrapolation_Mode(true);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
AdvectPhiImpl(const nimbus::Job *job,
              const nimbus::DataArray &da,
              T dt) {
  //Advect Phi 3.6% (Parallelized)
  LOG::Time("Advect Phi");
  example.phi_boundary_water.Use_Extrapolation_Mode(false);
  assert(example.particle_levelset_evolution.runge_kutta_order_levelset == 1);
  // I wrote and tested the following code, which broke levelset advection
  // into function calls. Because extrapolation and MPI calls are used
  // implicitly in this function call, I think we cannot get rid of it.
  {
    typedef typename LEVELSET_ADVECTION_POLICY<GRID<TV> >
        ::FAST_LEVELSET_ADVECTION_T T_FAST_LEVELSET_ADVECTION;
    typedef typename LEVELSET_POLICY<GRID<TV> >
        ::FAST_LEVELSET_T T_FAST_LEVELSET;
    T_FAST_LEVELSET_ADVECTION& levelset_advection =
        example.particle_levelset_evolution.levelset_advection;
    GRID<TV>& grid = ((T_FAST_LEVELSET*)levelset_advection.levelset)->grid;
    T_ARRAYS_SCALAR& phi = ((T_FAST_LEVELSET*)levelset_advection.levelset)->phi;
    assert(grid.Is_MAC_Grid() && levelset_advection.advection);
    T_ARRAYS_SCALAR phi_ghost(grid.Domain_Indices(
            example.particle_levelset_evolution.particle_levelset.
            number_of_ghost_cells));
    T_ARRAYS_SCALAR::Copy(phi, phi_ghost);
    levelset_advection.advection->Update_Advection_Equation_Cell(
        grid, phi, phi_ghost, example.face_velocities,
        *((T_FAST_LEVELSET*)levelset_advection.levelset)->boundary, dt, time);
    ((T_FAST_LEVELSET*)levelset_advection.levelset)->boundary->Apply_Boundary_Condition(
        grid, phi, time + dt);
  }

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
StepParticlesImpl(const nimbus::Job *job,
                  const nimbus::DataArray &da,
                  T dt) {
  //Advect Particles 12.1% (Parallelized)
  LOG::Time("Step Particles");
  example.particle_levelset_evolution.particle_levelset.Euler_Step_Particles(
      example.face_velocities_ghost, dt, time, true, true, false, false);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
AdvectRemovedParticlesImpl(const nimbus::Job *job,
                           const nimbus::DataArray &da,
                           T dt) {
  //Advect removed particles (Parallelized)
  LOG::Time("Advect Removed Particles");
  RANGE<TV_INT> domain(example.mac_grid.Domain_Indices());
  domain.max_corner += TV_INT::All_Ones_Vector();
  DOMAIN_ITERATOR_THREADED_ALPHA<WATER_DRIVER<TV>,TV>(domain,0).template Run<T,T>(
      *this, &WATER_DRIVER<TV>::Run, dt, time);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
AdvectVImpl(const nimbus::Job *job,
            const nimbus::DataArray &da,
            T dt) {
  //Advect Velocities 26% (Parallelized)
  LOG::Time("Advect V");
  example.incompressible.advection->Update_Advection_Equation_Face(
      example.mac_grid, example.face_velocities, example.face_velocities_ghost,
      example.face_velocities_ghost, *example.incompressible.boundary, dt, time);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ApplyForcesImpl(const nimbus::Job *job,
           const nimbus::DataArray &da,
           T dt) {
  //Add Forces 0%
  LOG::Time("Forces");
  example.incompressible.Advance_One_Time_Step_Forces(
      example.face_velocities, example.face_velocities_ghost, dt, time, true, 0, example.number_of_ghost_cells);

  // Save State.
  // example.Save_To_Nimbus(job, da, current_frame + 1);

  return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ModifyLevelSetPartOneImpl(const nimbus::Job *job,
                          const nimbus::DataArray &da,
                          const nimbus::GeometricRegion &local_region,
                          T dt) {
    LOG::Time("Modify Levelset Part one ...\n");

    example.particle_levelset_evolution.
        Modify_Levelset_And_Particles_Nimbus_One(&example.
                                                 face_velocities_ghost);
    example.particle_levelset_evolution.particle_levelset.levelset.boundary->Fill_Ghost_Cells(
        example.mac_grid,
        example.particle_levelset_evolution.phi,
        example.phi_ghost_bandwidth_seven,
        0, time+dt, 7);

    // save state
    // example.Save_To_Nimbus(job, da, current_frame+1);

    return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ModifyLevelSetPartTwoImpl(const nimbus::Job *job,
                          const nimbus::DataArray &da,
                          const nimbus::GeometricRegion &local_region,
                          T dt) {
    LOG::Time("Modify Levelset Part two ...\n");

    // TODO: this involves redundant copy operation. can get rid of some/ all
    // of this after merging with Hang's updates.
    int ghost_cells = 7;
    example.particle_levelset_evolution.
        Modify_Levelset_And_Particles_Nimbus_Two(&example.face_velocities_ghost,
                                                 &example.phi_ghost_bandwidth_seven,
                                                 ghost_cells);

    // save state
    // example.Save_To_Nimbus(job, da, current_frame+1);

    return true;
}

template<class TV> bool WATER_DRIVER<TV>::
AdjustPhiImpl(const nimbus::Job *job,
        const nimbus::DataArray &da,
        T dt) {
    LOG::Time("Adjust Phi ...\n");

    // adjust phi with sources
    example.Adjust_Phi_With_Sources(time+dt);

    // Save State.
    // example.Save_To_Nimbus(job, da, current_frame + 1);

    return true;
}

template<class TV> bool WATER_DRIVER<TV>::
DeleteParticlesImpl(const nimbus::Job *job,
                    const nimbus::DataArray &da,
                    T dt) {
    LOG::Time("Delete Particles ...\n");

    // delete particles
    int lid1 = example.particle_levelset_evolution.particle_levelset.last_unique_particle_id;
    example.particle_levelset_evolution.Delete_Particles_Outside_Grid();
    example.particle_levelset_evolution.particle_levelset.
        Delete_Particles_In_Local_Maximum_Phi_Cells(1);
    example.particle_levelset_evolution.particle_levelset.
        Delete_Particles_Far_From_Interface(); // uses visibility
    example.particle_levelset_evolution.particle_levelset.
        Identify_And_Remove_Escaped_Particles(example.face_velocities_ghost,
                1.5,
                time + dt);


    // save state
    // example.Save_To_Nimbus(job, da, current_frame+1);
    int lid2 = example.particle_levelset_evolution.particle_levelset.last_unique_particle_id;
    if (lid1 != lid2) {
      dbg(APP_LOG, "***** last unique particle id are different, %i and %i\n", lid1, lid2);
      exit(-1);
    }

    return true;
}

template<class TV> bool WATER_DRIVER<TV>::
ReincorporateParticlesImpl(const nimbus::Job *job,
                           const nimbus::DataArray &da,
                           T dt) {
    LOG::Time("Reincorporate Removed Particles ...\n");

    // reincorporate removed particles
    if (example.particle_levelset_evolution.particle_levelset.
            use_removed_positive_particles ||
            example.particle_levelset_evolution.particle_levelset.
            use_removed_negative_particles)
        example.particle_levelset_evolution.particle_levelset.
            Reincorporate_Removed_Particles(1, 1, 0, true);

    // save state
    // example.Save_To_Nimbus(job, da, current_frame+1);

    return true;
}

//#####################################################################
// Function Write_Substep
//#####################################################################
template<class TV> void WATER_DRIVER<TV>::
Write_Substep(const std::string& title,const int substep,const int level)
{
    if(level<=example.write_substeps_level){
        example.frame_title=title;
        std::stringstream ss;ss<<"Writing substep ["<<example.frame_title<<"]: output_number="<<output_number+1<<", time="<<time<<", frame="<<current_frame<<", substep="<<substep<<std::endl;LOG::filecout(ss.str());
        Write_Output_Files(++output_number);example.frame_title="";}
}
//#####################################################################
// Write_Output_Files
//#####################################################################
template<class TV> void WATER_DRIVER<TV>::
Write_Output_Files(const int frame)
{
    FILE_UTILITIES::Create_Directory(example.output_directory);
    FILE_UTILITIES::Create_Directory(example.output_directory+STRING_UTILITIES::string_sprintf("/%d",frame));
    FILE_UTILITIES::Create_Directory(example.output_directory+"/common");
    FILE_UTILITIES::Write_To_Text_File(example.output_directory+STRING_UTILITIES::string_sprintf("/%d/frame_title",frame),example.frame_title);
    if(frame==example.first_frame) 
        FILE_UTILITIES::Write_To_Text_File(example.output_directory+"/common/first_frame",frame,"\n");
    example.Write_Output_Files(frame);
    FILE_UTILITIES::Write_To_Text_File(example.output_directory+"/common/last_frame",frame,"\n");
}
//#####################################################################
template class WATER_DRIVER<VECTOR<float,3> >;
