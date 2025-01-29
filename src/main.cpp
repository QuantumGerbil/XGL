#include <SDL2/SDL.h>
#include <iostream>
#include <stdexcept>
#include <memory>
#include "defines.hpp"
#include "shader.hpp"

class SDLWindow {
	public:
		SDLWindow(const char* title, int width, int height) {
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				throw std::runtime_error(std::string("SDL could not initialize! SDL Error: ") + SDL_GetError());
			}

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

			window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									  width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
			if (!window) {
				throw std::runtime_error(std::string("Window could not be created! SDL Error: ") + SDL_GetError());
			}

			glContext = SDL_GL_CreateContext(window);
			if (!glContext) {
				throw std::runtime_error(std::string("OpenGL context could not be created! SDL Error: ") + SDL_GetError());
			}

			if (glewInit() != GLEW_OK) {
				throw std::runtime_error("Failed to initialize GLEW");
			}

			glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		}

		~SDLWindow() {
			if (glContext) SDL_GL_DeleteContext(glContext);
			if (window) SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void swapBuffers() {
			SDL_GL_SwapWindow(window);
		}

		SDL_Window* getWindow() { return window; }

	private:
		SDL_Window* window{nullptr};
		SDL_GLContext glContext{nullptr};
};

int main() {
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint programID;
	try {
		SDLWindow window("Spinning Cube", 800, 600);

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		programID = LoadShaders( "SimpleVertexShader.vert", "SimpleFragmentShader.frag" );

		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		bool running = true;
		//float angle = 0.0f;
		//auto lastTime = SDL_GetTicks();

		while (running) {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
					running = false;
				}
			}

			//auto current = SDL_GetTicks();
			//float deltaTime = (current - lastTime) * 0.001f;
			//lastTime = current;

			glClear(GL_COLOR_BUFFER_BIT);

			// Use our shader
			glUseProgram(programID);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

			glDisableVertexAttribArray(0);

			window.swapBuffers();
		}
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	return 0;
}

