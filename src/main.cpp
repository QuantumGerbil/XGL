#include <dlfcn.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>

// If you use something like GLEW or glad, include those here.
// For example:
// #include <GL/glew.h>

// On some systems you may need <GL/gl.h> or <OpenGL/gl.h> etc.
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#if defined(linux)
#include <GL/glx.h>
#endif

#define DEC_GL_FUNC(name, returnType, ...)		\
   typedef returnType (*PFN##name)(__VA_ARGS__);	\
   static PFN##name p##name = nullptr;

#define LOAD_GL_FUNC(name)				\
   p##name = (PFN##name)loadFunc(#name);		\
   if(!p##name) {					\
	std::cerr << "Failed to load " #name << "\n";   \
	dlclose(libGL);					\
	return false;					\
   }

// Simple vertex and fragment shaders
static const char* vertexShaderSrc = R"(
#version 130
in vec3 inPosition;
in vec3 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragColor;

void main()
{
    gl_Position = proj * view * model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
)";

static const char* fragmentShaderSrc = R"(
#version 130
in vec3 fragColor;
out vec4 outColor;
void main()
{
    outColor = vec4(fragColor, 1.0);
}
)";

DEC_GL_FUNC(glCreateShader, GLuint, GLenum);
DEC_GL_FUNC(glShaderSource, void, GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
DEC_GL_FUNC(glCompileShader, void, GLuint shader);
DEC_GL_FUNC(glGetShaderiv, void, GLuint shader, GLenum pname, GLint *params);
DEC_GL_FUNC(glGetShaderInfoLog, void, GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
DEC_GL_FUNC(glCreateProgram, GLuint, void);
DEC_GL_FUNC(glAttachShader, void, GLuint program, GLuint shader);
DEC_GL_FUNC(glLinkProgram, void, GLuint program);
DEC_GL_FUNC(glDeleteShader, void, GLuint shader);
DEC_GL_FUNC(glGetProgramiv, void, GLuint program, GLenum pname, GLint *params);
DEC_GL_FUNC(glGetProgramInfoLog,void, GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
DEC_GL_FUNC(glGenVertexArrays, void, GLsizei n, GLuint *arrays);
DEC_GL_FUNC(glGenBuffers, void, GLsizei n, GLuint * buffers);
DEC_GL_FUNC(glBindVertexArray, void, GLuint array);
DEC_GL_FUNC(glBindBuffer, void, GLenum target, GLuint buffer);
DEC_GL_FUNC(glBufferData, void, GLenum target, GLsizeiptr size, const void * data, GLenum usage);
DEC_GL_FUNC(glEnableVertexAttribArray, void, GLuint index);
DEC_GL_FUNC(glVertexAttribPointer, void, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
DEC_GL_FUNC(glGetUniformLocation, GLint, GLuint program, const GLchar *name);
DEC_GL_FUNC(glUseProgram, void, GLuint program);
DEC_GL_FUNC(glUniformMatrix4fv, void, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
DEC_GL_FUNC(glDeleteProgram, void, GLuint program);
DEC_GL_FUNC(glDeleteVertexArrays, void, GLsizei n, const GLuint *arrays);
DEC_GL_FUNC(glDeleteBuffers, void, GLsizei n, const GLuint *buffers);

bool loadOpenGLFunctions()
{
    // Open the system's GL library
    void* libGL = dlopen("libGL.so", RTLD_LAZY);
    if (!libGL)
    {
        std::cerr << "Failed to open libGL.so" << std::endl;
        return false;
    }

    // On Linux, glXGetProcAddress is the platform-specific function loader
    // Cast it appropriately
    auto glXGetProcAddress_ =
        (void* (*)(const GLubyte*))dlsym(libGL, "glXGetProcAddress");
    if (!glXGetProcAddress_)
    {
        std::cerr << "Failed to get glXGetProcAddress" << std::endl;
        dlclose(libGL);
        return false;
    }

    // Helper to load functions
    auto loadFunc = [&](const char* name) -> void*
    {
        // First try glXGetProcAddress
        auto addr = glXGetProcAddress_((const GLubyte*)name);
        if (!addr)
        {
            // If glXGetProcAddress fails, try dlsym on libGL
            addr = dlsym(libGL, name);
        }
        return addr;
    };

    // Load glCreateShader, do similarly for all other modern GL functions
    LOAD_GL_FUNC(glCreateShader);
    LOAD_GL_FUNC(glShaderSource);
    LOAD_GL_FUNC(glCompileShader);
    LOAD_GL_FUNC(glGetShaderiv);
    LOAD_GL_FUNC(glGetShaderInfoLog);
    LOAD_GL_FUNC(glCreateProgram);
    LOAD_GL_FUNC(glAttachShader);
    LOAD_GL_FUNC(glLinkProgram);
    LOAD_GL_FUNC(glDeleteShader);
    LOAD_GL_FUNC(glGetProgramiv);
    LOAD_GL_FUNC(glGetProgramInfoLog);
    LOAD_GL_FUNC(glGenVertexArrays);
    LOAD_GL_FUNC(glGenBuffers);
    LOAD_GL_FUNC(glBindBuffer);
    LOAD_GL_FUNC(glBufferData);
    LOAD_GL_FUNC(glEnableVertexAttribArray);
    LOAD_GL_FUNC(glVertexAttribPointer);
    LOAD_GL_FUNC(glGetUniformLocation);
    LOAD_GL_FUNC(glUseProgram);
    LOAD_GL_FUNC(glUniformMatrix4fv);
    LOAD_GL_FUNC(glDeleteProgram);
    LOAD_GL_FUNC(glDeleteVertexArrays);
    LOAD_GL_FUNC(glDeleteBuffers);

    // Keep libGL handle around if necessary...
    return true;
}

// Helper function for compiling a single shader
GLuint compileShader(GLenum type, const char* src){

    if (!loadOpenGLFunctions())
    {
        std::cerr << "Could not load some OpenGL functions" << std::endl;
        return 1;
    }

    GLuint shader = pglCreateShader(GL_VERTEX_SHADER);
    std::cout << "Created shader with ID: " << shader << std::endl;


    pglShaderSource(shader, 1, &src, nullptr);
    pglCompileShader(shader);

    GLint success = 0;
    pglGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        pglGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
        return 0;
    }
    return shader;
}

// Create a program from the two shaders
GLuint createProgram(const char* vertSrc, const char* fragSrc)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = pglCreateProgram();
    pglAttachShader(program, vs);
    pglAttachShader(program, fs);
    pglLinkProgram(program);

    pglDeleteShader(vs);
    pglDeleteShader(fs);

    GLint linked;
    pglGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        char log[512];
        pglGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program link error:\n" << log << std::endl;
        return 0;
    }
    return program;
}

int main(int argc, char* argv[])
{
    // 1) Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Use double buffering and an OpenGL 3.2 Core Profile if desired
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow(
        "Spinning Cube",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Optionally load OpenGL function pointers here (e.g. glewInit())

    // 2) Define cube data
    // Each vertex has 3D position and an RGB color
    GLfloat vertices[] = {
        //  Position         Color
        -0.5f, -0.5f, -0.5f,   1.f, 0.f, 0.f,
         0.5f, -0.5f, -0.5f,   0.f, 1.f, 0.f,
         0.5f,  0.5f, -0.5f,   0.f, 0.f, 1.f,
        -0.5f,  0.5f, -0.5f,   1.f, 1.f, 0.f,

        -0.5f, -0.5f,  0.5f,   0.f, 1.f, 1.f,
         0.5f, -0.5f,  0.5f,   1.f, 0.f, 1.f,
         0.5f,  0.5f,  0.5f,   1.f, 1.f, 1.f,
        -0.5f,  0.5f,  0.5f,   0.f, 0.f, 0.f
    };

    GLuint indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        0, 3, 7, 7, 4, 0,
        // Right face
        1, 2, 6, 6, 5, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        0, 1, 5, 5, 4, 0
    };

    // 3) Create buffers and arrays
    GLuint vbo, ebo, vao;
    pglGenVertexArrays(1, &vao);
    pglGenBuffers(1, &vbo);
    pglGenBuffers(1, &ebo);

    pglBindVertexArray(vao);

    // Upload vertex data
    pglBindBuffer(GL_ARRAY_BUFFER, vbo);
    pglBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Upload index data
    pglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    pglBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    GLint posAttribLoc = 0; // We'll bind it manually
    GLint colorAttribLoc = 1;
    pglEnableVertexAttribArray(posAttribLoc);
    pglVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    // Color attribute
    pglEnableVertexAttribArray(colorAttribLoc);
    pglVertexAttribPointer(colorAttribLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(GLfloat)));

    pglBindVertexArray(0);

    // 4) Compile shaders and create program
    GLuint program = createProgram(vertexShaderSrc, fragmentShaderSrc);
    if (!program)
    {
        std::cerr << "Failed to create program." << std::endl;
        return 1;
    }

    // Get uniform locations for model, view, and projection
    GLint modelLoc = pglGetUniformLocation(program, "model");
    GLint viewLoc  = pglGetUniformLocation(program, "view");
    GLint projLoc  = pglGetUniformLocation(program, "proj");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main loop
    bool running = true;
    float angle = 0.0f;
    Uint32 lastTime = SDL_GetTicks();
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = false;
        }

        // Calculate delta time
        Uint32 current = SDL_GetTicks();
        float deltaTime = (current - lastTime) * 0.001f;
        lastTime = current;

        // Update rotation angle
        angle += 50.0f * deltaTime; // rotate 50 degrees per second

        // Set viewport and clear the screen
        glViewport(0, 0, 800, 600);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 5) Render
        pglUseProgram(program);
        pglBindVertexArray(vao);

        // Construct simple perspective (hard-coded) and camera transformations
        // Here we do a very simple approach with a rough perspective and view
        float nearZ = 0.1f, farZ = 100.0f;
        float fov = 1.0f; // in radians (~57 degrees)
        float aspect = 800.0f / 600.0f;
        float top = nearZ * tanf(fov / 2);
        float right = top * aspect;

        // Very minimal matrix building
        float proj[16] = {
            nearZ/right, 0,          0,                              0,
            0,           nearZ/top,  0,                              0,
            0,           0,          -(farZ+nearZ)/(farZ-nearZ),     -1,
            0,           0,          -(2*farZ*nearZ)/(farZ-nearZ),    0
        };

        // Simple look-at from (0,0,3) to (0,0,0)
        float view[16] = {
            1, 0,  0, 0,
            0, 1,  0, 0,
            0, 0,  1, 0,
            0, 0, -3, 1
        };

        // Basic rotation around Y-axis
        float c = cosf(angle * 3.14159f / 180.0f);
        float s = sinf(angle * 3.14159f / 180.0f);

        float model[16] = {
            c,  0, -s, 0,
            0,  1,  0, 0,
            s,  0,  c, 0,
            0,  0,  0, 1
        };

        pglUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        pglUniformMatrix4fv(viewLoc,  1, GL_FALSE, view);
        pglUniformMatrix4fv(projLoc,  1, GL_FALSE, proj);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // 6) Cleanup
    pglDeleteProgram(program);
    pglDeleteVertexArrays(1, &vao);
    pglDeleteBuffers(1, &vbo);
    pglDeleteBuffers(1, &ebo);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

