#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class orbit_cam {
public:
    glm::vec3 center;
    glm::vec3 world_up;
    float radius;
    float x_angle;
    float y_angle;
    const float full_circle = 2.0f * glm::pi<float>();
    const float y_cap = glm::pi<float>() / 2.0f - 0.001f;
    void createCam(const glm::vec3& center, const glm::vec3& up, float radius, float x_angle, float y_angle);

    void rotate_x(const float radians);

    void rotate_y(const float radians);

    void changeRadius(const float offset);

    glm::vec3 get_eye();

    glm::mat4 get_view_matrix();
};