#pragma once

#include "arrayalgebra.h"

float constexpr camera_speed{0.2};

class Camera
{
    aa::mat4 view;
    aa::mat4 proj;
    aa::vec3 camera_position;
    aa::vec3 camera_front;
    aa::vec3 world_up;
    aa::vec3 camera_up;

    bool perspective_projection;

    float yaw;
    float pitch;

    float near_plane;
    float far_plane;
    float aspect_ratio;
    float fov_angle_degrees;

public:
    Camera(
        aa::vec3 camera_position,
        float yaw,
        float pitch,
        aa::vec3 world_up,
        float aspect_ratio,
        float near_plane,
        float far_plane,
        float fov_angle_degrees
    );
    aa::mat4 get_proj_matrix();
    aa::mat4 get_view_matrix();
    aa::vec3 get_camera_position() const;
    aa::vec3& get_camera_position_ref();
    aa::vec3 get_camera_front() const;
    aa::vec3& get_camera_front_ref();
    aa::vec3 get_camera_up() const;
    aa::vec3& get_camera_up_ref();
    float& get_yaw_ref();
    float get_yaw() const;
    float& get_pitch_ref();
    float get_pitch() const;

    void set_aspect_ratio(float aspect_ratio);
    float get_aspect_ratio() const;
    void set_perspective_mode();
    void set_orthogonal_mode();
};
