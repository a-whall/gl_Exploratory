namespace Texture
{
    using std::string;
    using std::cout;
    using Debug::abort_MyGL_App;

	GLuint load(const char* filePath) {
		GLuint textureHandle;
		SDL_Surface* image = IMG_Load(filePath);
		if (image == nullptr) {
			cout << SDL_GetError();
			return 0;
		}
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image->w, image->h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->w, image->h, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		SDL_FreeSurface(image);
		return textureHandle;
	}


    GLuint loadCubeMap(const string& baseName)
    {
        GLuint texID;
        const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };

        // Load the first one to get width/height
        string texName = baseName + "_" + suffixes[0] + ".png";
        SDL_Surface* image = IMG_Load(texName.c_str());
        if (image == nullptr)
            abort_MyGL_App("SDL_image failed to load " + texName + ": ", IMG_GetError());

        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

        int mode = GL_RGB;

        if (image->format->BytesPerPixel == 4) {
            mode = GL_RGBA;
        }

        // Allocate immutable storage for the whole cube map texture
        glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB8, image->w, image->h);
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, image->w, image->h, mode, GL_UNSIGNED_BYTE, image->pixels);
        SDL_FreeSurface(image);

        // Load the other 5 cube-map faces
        for (int i = 1; i < 6; i++) {
            std::string texName = baseName + "_" + suffixes[i] + ".png";
            image = IMG_Load(texName.c_str());
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, image->w, image->h, mode, GL_UNSIGNED_BYTE, image->pixels);
            SDL_FreeSurface(image);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texID;
    }
}