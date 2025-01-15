#include "camera.h"

Camera::Camera(
    aa::vec3 camera_position,
    float yaw,
    float pitch,
    aa::vec3 world_up,
    float aspect_ratio,
    float near_plane,
    float far_plane,
    float fov_angle_degrees
)
    : view{}, proj{}, camera_position{camera_position}, world_up(world_up),
      yaw(yaw), pitch(pitch), near_plane(near_plane), far_plane(far_plane),
      aspect_ratio(aspect_ratio), fov_angle_degrees(fov_angle_degrees)
{
    perspective_projection = true;
    proj =
        aa::perspective(fov_angle_degrees, aspect_ratio, near_plane, far_plane);
    auto const [view_ret, front_ret, up_ret] =
        aa::view(yaw, pitch, world_up, camera_position);
    view = view_ret;
    camera_front = front_ret;
    camera_up = up_ret;
}
aa::mat4 Camera::get_proj_matrix()
{
    if (perspective_projection) {
        proj = aa::perspective(
            fov_angle_degrees,
            aspect_ratio,
            near_plane,
            far_plane
        );
    } else {
        proj = aa::orthogonal(
            fov_angle_degrees,
            aspect_ratio,
            near_plane,
            far_plane
        );
    }
    return proj;
}
aa::mat4 Camera::get_view_matrix()
{
    auto const [view_ret, front_ret, up_ret] =
        aa::view(yaw, pitch, world_up, camera_position);
    view = view_ret;
    camera_front = front_ret;
    camera_up = up_ret;
    return view;
}

aa::vec3 Camera::get_camera_position() const { return camera_position; }
aa::vec3& Camera::get_camera_position_ref() { return camera_position; }
aa::vec3 Camera::get_camera_front() const { return camera_front; }
aa::vec3& Camera::get_camera_front_ref() { return camera_front; }
aa::vec3 Camera::get_camera_up() const { return camera_up; }
aa::vec3& Camera::get_camera_up_ref() { return camera_up; }
float& Camera::get_yaw_ref() { return yaw; }
float Camera::get_yaw() const { return yaw; }
float& Camera::get_pitch_ref() { return pitch; }
float Camera::get_pitch() const { return pitch; }

void Camera::set_aspect_ratio(float aspect_ratio)
{
    this->aspect_ratio = aspect_ratio;
}

float Camera::get_aspect_ratio() const { return aspect_ratio; }

void Camera::set_perspective_mode() { perspective_projection = true; }
void Camera::set_orthogonal_mode() { perspective_projection = false; }
