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
    float full_circle;
    float y_cap;
    void createCam(const glm::vec3& center, const glm::vec3& up, float radius, float x_angle, float y_angle);

    void rotate_x(const float radians);

    void rotate_y(const float radians);

    void changeRadius(const float offset);

    glm::vec3 get_eye();

    glm::mat4 get_view_matrix();
};