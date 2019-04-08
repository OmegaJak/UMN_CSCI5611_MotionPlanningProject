#pragma once
#include <gtc/matrix_transform.hpp>

class Camera {
   public:
    Camera();

    void ProcessMouseInput(float deltaX, float deltaY, bool constrainPitch = true);
    void ProcessKeyboardInput();

    glm::vec3 GetPosition();
    glm::vec3 GetMousePosition(float normalizedMouseX, float normalizedMouseY, const glm::mat4& proj, float distanceFromCamera);
    glm::vec3 GetForward();
	void setPosition(glm::vec3 pos) {
        _position = pos;
	}
    glm::mat4 GetView() {
      return _view;
	};
    void Update();

	glm::vec3 getForward() {
        return _forward;
	}
	glm::vec3 getRight() {
        return _right;
	}
	glm::vec3 getUp() {
        return _up;
	}






   private:
    void UpdateCameraVectors();

    glm::vec3 _position, _forward, _up, _right, _worldUp;
    float _yaw, _pitch;
    glm::mat4 _view;
};
