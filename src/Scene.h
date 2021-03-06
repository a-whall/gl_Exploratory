using glm::vec3, glm::vec4, glm::mat3, glm::mat4;

namespace Scene
{ 
	using glm::mat4, glm::vec3, std::vector, std::unique_ptr;
	using glm::translate;

	class Object
	{
	protected:

		int nVerts = 0;
		mat4 model;
		mat4 mv;

		Camera::Viewport& cam;
		Shader::Program* shader;

	public:

		bool isActive = true;

		Object(float x, float y, float z, Camera::Viewport& c, Shader::Program* s = nullptr)
			: model(translate(mat4(1.0f), vec3(x, y, z))), mv(mat4(1.0f)), cam(c)
		{
			if (s != nullptr) shader = s;
			else              shader = nullptr;
		}

		virtual void init_buffers() = 0;
		virtual void update( float) = 0;
		virtual void render() = 0;

		virtual mat4 getModel() final { return model; };
		virtual void setModel(mat4 m) final { model = m; };
	};

	class Manager
	{
		vector<unique_ptr<Object>> objects;
		
	public:

		template <typename T, typename... TArgs>
		T& new_object(TArgs&&... mArgs) {
			T* o(new T(std::forward<TArgs>(mArgs)...));
			unique_ptr<Object> uPtr{ o };
			objects.emplace_back(std::move(uPtr));
			return *o;
		}
		void update(float t) {
			for (auto& o : objects) o->update(t);
		}
		void render() {
			for (auto& o : objects) o->render();
		}
		void refresh() {
			objects.erase(
				std::remove_if(std::begin(objects), std::end(objects),
					[](const unique_ptr<Object>& mEntity) {
					    return !mEntity->isActive;
					}),
				std::end(objects));
		}
	};
}