#pragma once
#include <glm.hpp>
#include <gtc/type_ptr.hpp> // for glm::make_mat4()

constexpr glm::vec3 world_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float BOUND_MULTIPLIER = .25f;

namespace Camera
{
	float window_w, window_h;
	void setWindowDimmensions(int w, int h) { window_w = static_cast<float>(w), window_h = static_cast<float>(h); }

	using glm::vec3, glm::vec4, glm::mat3, glm::mat4;
	using glm::lookAt, glm::perspective, glm::cross, glm::normalize, glm::radians;

	enum Movement
	{
		FRONT,
		BACKWARD,
		RIGHTWARD,
		LEFTWARD
	};

	enum Type
	{
		PERSPECTIVE,
		ORTHO
	};

	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	float ZOOM = 70.0f;

	class Viewport
	{
		vec3 position;
		mat4 worldToView;
		mat4 viewToProjection;
		mat4 rotation = mat4(1.0f);

		bool firstMouse = true;

		float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		float pitch = 0.0f;
		float midScreenX = window_w / 2.0f;
		float midScreenY = window_h / 2.0f;
		float thetaDegrees;
		float lastX = midScreenX;
		float lastY = midScreenY;

		float ZOOM = 70.0f; // fov in Degrees
	public:
		int wBound = (int)(BOUND_MULTIPLIER * window_w);
		int hBound = (int)(BOUND_MULTIPLIER * window_h);

		vec3 FORWARD;
		vec3 RIGHT;
		vec3 UP;

		Viewport(vec3 pos, vec3 viewDir, float aspect, float zNear, float zFar)
			: position(pos), FORWARD(viewDir)
		{
			reset_xy_offset();

			worldToView = lookAt(pos, glm::normalize(pos - viewDir), world_up_vector);
			viewToProjection = perspective(glm::radians(70.0f), aspect, zNear, zFar);

			RIGHT = -glm::normalize(cross(world_up_vector, FORWARD));
			UP = glm::normalize(cross(FORWARD, RIGHT));
		}
		Viewport() : Viewport(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, -1.0f), 1.25f, 0.1f, 100.0f) {}


		void updateRotationMatrices() {
			worldToView = worldToView * yaw * pitch;
		}
		
		void view_MouseMotion(int x, int y) {

			if (firstMouse)
			{
				lastX = (float)x;
				lastY = (float)y;
				firstMouse = false;
			}
			
			float xoffset = (float)x - lastX;
			float yoffset = lastY - (float)y;
			lastX = (float)x;
			lastY = (float)y;

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			worldToView = look(pitch, yaw);
			updateRotationMatrices();
		}
		
		void reset_xy_offset() {
			lastX = window_w / 2;
			lastY = window_h / 2;
		}
		
		mat4 look(float pitch, float yaw) { // broken af

			if (pitch > 89.0f)  pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;

			FORWARD.x = cos(radians(yaw)) * cos(radians(pitch));
			FORWARD.y = sin(radians(pitch));
			FORWARD.z = sin(radians(yaw)) * cos(radians(pitch));

			FORWARD = normalize(FORWARD); // unit length forward direction
			RIGHT = normalize( cross(UP, FORWARD));
			UP = cross(FORWARD, RIGHT); // no need to normalize

			float matrix[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0f };
			// set rotation part, inverse rotation matrix: M^-1 = M^T for Euclidean transform
			matrix[0] = RIGHT.x;
			matrix[4] = RIGHT.y;
			matrix[8] = RIGHT.z;
			matrix[1] = UP.x;
			matrix[5] = UP.y;
			matrix[9] = UP.z;
			matrix[2] = FORWARD.x;
			matrix[6] = FORWARD.y;
			matrix[10] = FORWARD.z;

			// set translation part
			matrix[12] = -RIGHT.x * position.x - RIGHT.y * position.y - RIGHT.z * position.z;
			matrix[13] = -UP.x * position.x - UP.y * position.y - UP.z * position.z;
			matrix[14] = -FORWARD.x * position.x - FORWARD.y * position.y - FORWARD.z * position.z;


			return glm::make_mat4((float*)&rotation);
		}
		void move(float incr, vec3& dir) {
			position += incr * dir;
			worldToView = lookAt(position, normalize(position - FORWARD), world_up_vector);
		}



		void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= SENSITIVITY;
			yoffset *= SENSITIVITY;

			yaw += xoffset;
			pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		void processKeyboard(Movement direction, float dt) {
			float velocity = SPEED * dt;
			if (direction == FRONT)
				position += FORWARD * velocity;
			if (direction == BACKWARD)
				position -= FORWARD * velocity;
			if (direction == LEFTWARD)
				position -= RIGHT * velocity;
			if (direction == RIGHTWARD)
				position += RIGHT * velocity;
		}

		void updateCameraVectors() {
			// calculate the new Front vector
			FORWARD.x = cos(radians(yaw)) * cos(radians(pitch));
			FORWARD.y = sin(radians(pitch));
			FORWARD.z = sin(radians(yaw)) * cos(radians(pitch));
			FORWARD = normalize(FORWARD);
			// also re-calculate the Right and Up vector
			RIGHT = normalize(cross(FORWARD, world_up_vector));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			UP = normalize(cross(RIGHT, FORWARD));
		}

		void updateProjection() {
			viewToProjection = perspective(radians(ZOOM), 1.25f, .1f, 100.0f);
		}


		mat4 get_WorldToView_Matrix() { return lookAt(position, position + FORWARD, world_up_vector); }
		mat4 get_ViewToProjection_Matrix() { return viewToProjection; }
		vec3 get_position() { return position; }

		void set_Position(vec3 pos) { position = pos; }
		void set_ViewDirection(vec3 dir) { FORWARD = dir; }
		void set_firstMouse(bool b) { firstMouse = b; }





		void mouse_motion(double xpos, double ypos)
		{
			if (firstMouse)
			{
				lastX = (float)xpos;
				lastY = (float)ypos;
				firstMouse = false;
			}

			float xoffset = (float)xpos - lastX;
			float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

			lastX = (float)xpos;
			lastY = (float)ypos;

			processMouseMovement(xoffset, yoffset);
		}


		void mouse_scroll(int scroll) {
			// decrease fov for scroll up
			ZOOM -= (float)(scroll * .1);

			if (ZOOM < 1.0f) ZOOM = 1.0f;
			else if (ZOOM > 90.0f) ZOOM = 90.0f;

			updateProjection();
		}
	};


	class Controller {


	};

}