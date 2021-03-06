//#####################################################################
// Copyright 2009, Jon Gretarsson, Nipun Kwatra.
// This file is part of PhysBAM whose distribution is governed by the license contained in the accompanying file PHYSBAM_COPYRIGHT.txt.
//#####################################################################
// Class OPENGL_COMPONENT_RIGID_BODIES_1D
//#####################################################################
#ifndef __OPENGL_COMPONENT_RIGID_BODIES_1D__
#define __OPENGL_COMPONENT_RIGID_BODIES_1D__

#include <PhysBAM_Tools/Arrays/ARRAY.h>
#include <PhysBAM_Tools/Data_Structures/PAIR.h>
#include <PhysBAM_Rendering/PhysBAM_OpenGL/OpenGL/OPENGL_SELECTION.h>
#include <PhysBAM_Rendering/PhysBAM_OpenGL/OpenGL_Components/OPENGL_COMPONENT.h>
namespace PhysBAM{

template<class T> class OPENGL_POINT_SIMPLICES_1D;
template<class T> class OPENGL_AXES;
template<class TV> class RIGID_BODY_COLLECTION;

template<class T,class RW=T>
class OPENGL_COMPONENT_RIGID_BODIES_1D : public OPENGL_COMPONENT
{
private:
    typedef VECTOR<T,1> TV;

    std::string basedir;
    int frame_loaded;
    bool valid;
    bool show_object_names;
    bool output_positions;
    bool draw_velocity_vectors;
    bool draw_node_velocity_vectors;
    bool draw_point_simplices;
public:
    RIGID_BODY_COLLECTION<TV>& rigid_body_collection;
private:
    ARRAY<OPENGL_POINT_SIMPLICES_1D<T>*,int> opengl_point_simplices;
    ARRAY<OPENGL_AXES<T>*,int> opengl_axes;
    ARRAY<bool,int> draw_object;
    ARRAY<bool,int> use_object_bounding_box;
    ARRAY<VECTOR<T,1> > positions;
    ARRAY<VECTOR<T,1> > node_positions;
    OPENGL_SELECTION* current_selection;
    bool need_destroy_rigid_body_collection;

public:
    OPENGL_COMPONENT_RIGID_BODIES_1D(const std::string& basedir);
    OPENGL_COMPONENT_RIGID_BODIES_1D(RIGID_BODY_COLLECTION<TV>& rigid_body_collection,const std::string& basedir);
    ~OPENGL_COMPONENT_RIGID_BODIES_1D();
    
//#####################################################################
    bool Valid_Frame(int frame_input) const PHYSBAM_OVERRIDE;
    void Set_Frame(int frame_input) PHYSBAM_OVERRIDE;
    void Set_Draw(bool draw_input = true) PHYSBAM_OVERRIDE;

    void Display(const int in_color=1) const PHYSBAM_OVERRIDE;
    bool Use_Bounding_Box() const PHYSBAM_OVERRIDE;
    virtual RANGE<VECTOR<float,3> > Bounding_Box() const PHYSBAM_OVERRIDE;

    void Read_Hints(const std::string& filename);

    void Set_Draw_Object(int i, bool draw_it);  // Need to call Reinitialize after changing draw objects
    bool Get_Draw_Object(int i) const;
    void Set_Object_Color(int i, const OPENGL_COLOR &color);
    void Set_Use_Object_Bounding_Box(int i, bool use_it);

    void Toggle_Output_Positions();
    void Toggle_Show_Object_Names();
    void Toggle_Draw_Mode();

    DEFINE_COMPONENT_CALLBACK(OPENGL_COMPONENT_RIGID_BODIES_1D, Toggle_Output_Positions, "Toggle output positions");
    DEFINE_COMPONENT_CALLBACK(OPENGL_COMPONENT_RIGID_BODIES_1D, Toggle_Show_Object_Names, "Toggle show object names");
    DEFINE_COMPONENT_CALLBACK(OPENGL_COMPONENT_RIGID_BODIES_1D, Toggle_Draw_Mode, "Toggle draw mode");

public:
    void Reinitialize(const bool force=false);    // Needs to be called after some state changes
private:
    void Create_Geometry(const int id);
    void Update_Geometry(const int id);
    void Destroy_Geometry(const int id);
    void Update_Object_Labels();
//#####################################################################
};

}

#endif
