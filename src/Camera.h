namespace Camera
{
	using glm::vec3, glm::vec4, glm::mat3, glm::mat4,
	      glm::lookAt, glm::perspective,                // gtc/type_ptr.hpp
	      glm::cross, glm::normalize, glm::radians;

	constexpr vec3 world_up_vector = vec3(0.0f, 1.0f, 0.0f);
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;

	float window_w, window_h;
	void setWindowDimmensions(int w, int h) { window_w = static_cast<float>(w), window_h = static_cast<float>(h); }

	struct mouse_ui {
		bool firstMouse = true;
		float yaw = -90.0f;	// yaw is initialized to -90.0 degrees to face negative z direction (a yaw of 0 results in a direction vector pointing in the positive x)
		float pitch = 0.0f;
		float midScreenX = window_w / 2.0f;
		float midScreenY = window_h / 2.0f;
		float lastX = midScreenX;
		float lastY = midScreenY;

		void reset_xy_offset() {
			lastX = window_w / 2;
			lastY = window_h / 2;
		}
	};

	class Viewport
	{
		vec3 position;         // world pos
		mat4 worldToView;      // view matrix
		mat4 viewToProjection; // projection matrix

		mouse_ui m;
	
		float fov_deg = 70.0f; // fov in Degrees

	public:

		// orthogonal camera vectors
		vec3 FORWARD;
		vec3 RIGHT;
		vec3 UP;

		Viewport() : Viewport(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, -1.0f), 1.25f) {}
		Viewport(vec3 pos, vec3 viewDir, float aspect)
			: position{ pos },
			FORWARD{ viewDir },
			worldToView{ lookAt(pos, glm::normalize(pos - viewDir), world_up_vector) },
			viewToProjection{ perspective(glm::radians(70.0f), aspect, 0.1f, 200.0f) },
			RIGHT{ -glm::normalize(cross(world_up_vector, FORWARD)) },
			UP{ glm::normalize(cross(FORWARD, RIGHT)) } {}
		
		void move(float incr, vec3& dir) {
			position += incr * dir;
			worldToView = lookAt(position, normalize(position - FORWARD), world_up_vector);
		}

		void mouse_motion(double xpos, double ypos)
		{
			if (m.firstMouse) {
				m.lastX = (float)xpos;
				m.lastY = (float)ypos;
				m.firstMouse = false;
			}

			float xoffset = (float)xpos - m.lastX;
			float yoffset = m.lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

			m.lastX = (float)xpos;
			m.lastY = (float)ypos;

			process_mouse_movement(xoffset, yoffset);
		}

		void mouse_scroll(int scroll) {
			fov_deg -= (float)(scroll * .1); // decrease fov for scroll up
			constrain_fov();
			update_projection_matrix();
		}

		mat4 get_WorldToView_Matrix() { return lookAt(position, position + FORWARD, world_up_vector); }
		mat4 get_ViewToProjection_Matrix() { return viewToProjection; }
		vec3 get_position() { return position; }

		void set_Position(vec3 pos) { position = pos; }
		void set_ViewDirection(vec3 dir) { FORWARD = dir; }
		void set_firstMouse(bool b) { m.firstMouse = b; }

	private:

		void process_mouse_movement(float xoffset, float yoffset) {
			xoffset *= SENSITIVITY;
			yoffset *= SENSITIVITY;

			m.yaw += xoffset;
			m.pitch += yoffset;

			constrain_pitch();
			update_camera_vectors_using_updated_euler_angles();
		}

		void constrain_pitch() {
			if (m.pitch > 89.0f) m.pitch = 89.0f;
			if (m.pitch < -89.0f) m.pitch = -89.0f;
		}

		void constrain_fov() {
			if (fov_deg < 1.0f) fov_deg = 1.0f;
			else if (fov_deg > 90.0f) fov_deg = 90.0f;
		}

		void update_camera_vectors_using_updated_euler_angles() {
			// calculate the new Front vector
			FORWARD.x = cos(radians(m.yaw)) * cos(radians(m.pitch));
			FORWARD.y = sin(radians(m.pitch));
			FORWARD.z = sin(radians(m.yaw)) * cos(radians(m.pitch));
			FORWARD = normalize(FORWARD);
			// also re-calculate the Right and Up vector
			RIGHT = normalize(cross(FORWARD, world_up_vector));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			UP = normalize(cross(RIGHT, FORWARD));
		}

		void update_projection_matrix() {
			viewToProjection = perspective(radians(fov_deg), 1.25f, .1f, 100.0f);
		}
	};

	class Controller {
	//TODO: partition viewport class which is getting cumbersome 
	};

}