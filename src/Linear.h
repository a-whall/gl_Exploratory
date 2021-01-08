class Line : public Scene::Object {

	glm::vec3 a, b;

public:

	Line(float a1, float a2, float a3, float b1, float b2, float b3, Camera::Viewport& cam) 
	: a(a1, a2, a3), b(b1, b2, b3), Scene::Object(0, 0, 0, cam) {}


};

