#ifndef DEF_SHADER
#define DEF_SHADER


// Include Windows

#ifdef WIN32
#include <GL/glew.h>


// Include Mac

#elif __APPLE__
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>


// Include UNIX/Linux

#else
#define GL3_PROTOTYPES 1
#include <GL3/gl3.h>

#endif


// Includes communs

#include <iostream>
#include <string>
#include <fstream>


// Classe Shader

class Shader
{
    public:

    Shader();
    Shader(Shader const &shaderACopier);
    Shader(std::string vertexSource, std::string fragmentSource,
		std::string geometrySource, std::string tesselationSource);
    ~Shader();

    Shader& operator=(Shader const &shaderACopier);

    bool charger();
	bool link();
    bool compilerShader(GLuint &shader, GLenum type, std::string const &fichierSource);
    GLuint getProgramID() const;


    private:

    GLuint m_vertexID;
    GLuint m_fragmentID;
	GLuint m_geometryID;
	GLuint m_tesselationID;

    GLuint m_programID;

    std::string m_vertexSource;
    std::string m_fragmentSource;
	std::string m_geometrySource;
	std::string m_tesselationSource;
};

#endif
