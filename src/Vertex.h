namespace Vertex
{
	using glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4;
	using std::vector, std::initializer_list, std::size_t;

	struct Attribute
	{
		unsigned count;
		unsigned type;
		bool normalized;
		unsigned offset;
		int index;
		bool instanced; // implicit bool to int used as attrib divisor. false -> per vertex, true -> per instance
	};

	struct DataLayout
	{
		vector<Attribute> attrib_vec;
		unsigned nBytes = 0;
		
		template <typename T>
		void push(unsigned, int, bool = false) { static_assert(false); }

		template<> inline
		void push<float>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_FLOAT, false, nBytes, attribIndex, isInstanced });//nBytes given as offset
			nBytes += count * 4;// sizeof(float)                                                 before getting updated
		}
		template<> inline
		void push<unsigned>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_UNSIGNED_INT, false, nBytes, attribIndex, isInstanced });
			nBytes += count * 4;// sizeof(unsigned)
		}
		template<> inline
		void push<unsigned char>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_UNSIGNED_BYTE, false, nBytes++, attribIndex, isInstanced });
		}
	};

	template<typename T>
	class Buffer
	{
	protected:
		GLenum binding_target = GL_ARRAY_BUFFER;
		DataLayout layout;
		vector<T> data_vec;
		bool generated = false;

	public:

		unsigned handle;

		Buffer(int nElems = 0)        : data_vec(nElems), layout()          { gen_buffer(); }
		Buffer(int nElems, T initVal) : data_vec(nElems, initVal), layout() { gen_buffer(); }
		Buffer(initializer_list<T> data) : data_vec(0), layout()            { gen_buffer(); data_vec.insert(data_vec.end(), data); }
		Buffer(unsigned size, T data[], GLenum glMemUseHint) : data_vec(0)
		{ gen_buffer(); bind(); fill_data_vec(data); buffer_data(glMemUseHint); }
		Buffer(GLenum bt, unsigned size, T data[], GLenum glMemUseHint)
		{ gen_buffer(); bind(bt); data_vec.insert(data_vec.end(), &data[0], &data[sizeof(data) / sizeof(T)]); buffer_data(glMemUseHint); }

		~Buffer() { glDeleteBuffers(1, &handle); }

		virtual void bind() const { glBindBuffer(binding_target, handle); }
		virtual void bind     (GLenum bindingTarget) { glBindBuffer(binding_target = bindingTarget, handle); }
		virtual void bind_base(GLenum bt, int b_idx) { glBindBufferBase(binding_target = bt, b_idx, handle); }

		template<typename T> void add_attribute(int index, bool = false) { static_assert(false); }
		template<> inline void add_attribute<float>(int i, bool perInst) { layout.push<float>(1, i, perInst); }
		template<> inline void add_attribute<vec2>(int i, bool perInst)  { layout.push<float>(2, i, perInst); }
		template<> inline void add_attribute<vec3>(int i, bool perInst)  { layout.push<float>(3, i, perInst); }
		template<> inline void add_attribute<vec4>(int i, bool perInst)  { layout.push<float>(4, i, perInst); }
		//template<> inline void add_attribute<mat3>(int i, bool perInst) { layout.push<float>(4, i, perInst); } //TODO
		//template<> inline void add_attribute<mat4>(int i, bool perInst) { layout.push<float>(4, i, perInst); } //TODO

		void buffer_data(GLenum usage_hint = GL_STATIC_DRAW)
		{
			glBufferData(binding_target, data_vec.size() * sizeof(T), data_vec.data(), usage_hint);
		}
		void buffer_sub_data() {
			glBufferSubData(binding_target, 0, data_vec.size() * sizeof(T), data_vec.data());
		}
		void buffer_sub_data(unsigned size, float data[])
		{
			data_vec.insert(data_vec.begin(), &data[0], &data[sizeof(data) / sizeof(T)]);
			glBufferSubData(binding_target, 0, size, data);
		}
		void enable_vertex_attribute_pointers()
		{
			bind();
			for (const Attribute& a : layout.attrib_vec) {
				glEnableVertexAttribArray(a.index);
				glVertexAttribPointer(a.index, a.count, a.type, a.normalized, layout.nBytes, (void*)a.offset);
				glVertexAttribDivisor(a.index, a.instanced);
			}
		}

		int getNumElements() { return data_vec.size(); }
		void operator=(initializer_list<T> data) { data_vec.insert(data_vec.end(), data); buffer_data(); }
		T&   operator[](size_t i)                { return data_vec[i]; }

	private:

		void gen_buffer() { glGenBuffers(1, &handle); }

		void fill_data_vec(const T data[]) { data_vec.insert(data_vec.end(), data, data + sizeof(*data) / sizeof(T)); }
	};


	struct Index : public Buffer<unsigned>
	{
		using Buffer::operator=;
		Index(int size = 0) : Buffer(size) {}
		Index(unsigned size, GLuint data[]) : Buffer(size, data, GL_STATIC_DRAW) {}
	};


	class Array
	{
	protected:
		
		GLuint handle;

	public:

		Array() { glGenVertexArrays(1, &handle); }
		~Array() { glDeleteVertexArrays(1, &handle); }

		void bind()   const { glBindVertexArray(handle); }
		void unBind() const { glBindVertexArray(0); }
		
		template<typename T> void enable_attributes(Buffer<T>& vbo)
		{
			bind();
			vbo.enable_vertex_attribute_pointers();
			unBind();
		}
		template<typename T> void bind_buffers(Index& ebo, Buffer<T>& vbo, bool hasNorm, bool hasTC, bool hasTan)
		{
			bind();
			ebo.bind();
			vbo.bind();
			vbo.add_attribute<vec3>(0);
			if (hasNorm) vbo.add_attribute<vec3>(1);
			if ( hasTC ) vbo.add_attribute<vec2>(2);
			if (hasTan ) vbo.add_attribute<vec3>(3);
			vbo.enable_vertex_attribute_pointers();
			unBind();
		}
	};

	template<typename T>
	void bind_buffer(GLenum targetBufferBindingPoint, const Buffer<T>& bufferObject)
	{
		glBindBuffer(targetBufferBindingPoint, bufferObject.handle);
	};

	template<typename T>
	void bind_buffer_base(GLenum targetBufferBindingPoint, int bindingIndex, const Buffer<T>& bufferObject)
	{
		glBindBufferBase(targetBufferBindingPoint, bindingIndex, bufferObject.handle);
	};
}