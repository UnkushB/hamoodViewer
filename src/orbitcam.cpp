#include "orbitcam.h"

void orbit_cam::createCam(const glm::vec3& center, const glm::vec3& up, float radius, float x_angle, float y_angle) {
    this->center = center; this->world_up = up; this->radius = radius;
    this->x_angle = x_angle; this->y_angle = y_angle;
};

void orbit_cam::rotate_x(const float radians) {
    x_angle += radians;

    x_angle = std::fmod(x_angle, full_circle);

    if (x_angle < 0.0f)
        x_angle = full_circle + x_angle;
};

void orbit_cam::rotate_y(const float radians) {
    y_angle += radians;

    if (y_angle > y_cap)
        y_angle = y_cap;

    if (y_angle < -y_cap)
        y_angle = -y_cap;
};

void orbit_cam::changeRadius(const float offset) {
    radius -= offset;
    if (radius < 0.5f)
        radius = 0.5f;
}

glm::vec3 orbit_cam::get_eye() {
    const float sin_x = sin(x_angle);
    const float cosin_x = cos(x_angle);
    const float sin_y = sin(y_angle);
    const float cosin_y = cos(y_angle);

    // Calculate eye position out of them
    const float x = center.x + radius * cosin_y * cosin_x;
    const float y = center.y + radius * sin_y;
    const float z = center.z + radius * cosin_y * sin_x;

    return glm::vec3(x, y, z);
};

glm::mat4 orbit_cam::get_view_matrix() {
    glm::vec3 eye(get_eye());

    glm::vec3 forward = glm::normalize(glm::vec3(0.0f) - eye);
    glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
    glm::vec3 cam_up = glm::normalize(glm::cross(right, forward));

    return glm::lookAt(eye, glm::vec3(0.0f), cam_up);
};