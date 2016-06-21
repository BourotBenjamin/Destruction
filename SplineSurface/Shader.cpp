#include "Shader.h"

// Constructeurs et Destructeur

Shader::Shader() : m_vertexID(0), m_fragmentID(0), m_programID(0), m_vertexSource(), m_fragmentSource(), m_geometrySource(), m_tesselationSource()
{
}


Shader::Shader(Shader const &shaderACopier)
{
    // Copie des fichiers sources

	m_vertexSource = shaderACopier.m_vertexSource;
	m_fragmentSource = shaderACopier.m_fragmentSource; 
	m_geometrySource = shaderACopier.m_geometrySource;
	m_tesselationSource = shaderACopier.m_tesselationSource;


    // Chargement du nouveau shader

    charger();
}


Shader::Shader(std::string vertexSource, std::string fragmentSource,
	std::string geometrySource, std::string tesselationSource) : m_vertexID(0), m_fragmentID(0), m_programID(0),
                                                                       m_vertexSource(vertexSource), m_fragmentSource(fragmentSource),
																	   m_geometrySource(geometrySource), m_tesselationSource(tesselationSource)
{
	charger();
}


Shader::~Shader()
{
    // Destruction du shader

    glDeleteShader(m_vertexID);
    glDeleteShader(m_fragmentID);
	glDeleteShader(m_geometryID);
	glDeleteShader(m_tesselationID);
    glDeleteProgram(m_programID);
}


// Méthodes

Shader& Shader::operator=(Shader const &shaderACopier)
{
    // Copie des fichiers sources

    m_vertexSource = shaderACopier.m_vertexSource;
	m_fragmentSource = shaderACopier.m_fragmentSource;
	m_geometrySource = shaderACopier.m_geometrySource;
	m_tesselationSource = shaderACopier.m_tesselationSource;


    // Chargement du nouveau shader

    charger();


    // Retour du pointeur this

    return *this;
}


bool Shader::charger()
{
    // Destruction d'un éventuel ancien Shader

    if(glIsShader(m_vertexID) == GL_TRUE)
        glDeleteShader(m_vertexID);

    if(glIsShader(m_fragmentID) == GL_TRUE)
        glDeleteShader(m_fragmentID);

	if (glIsShader(m_geometryID) == GL_TRUE)
		glDeleteShader(m_geometryID);

	if (glIsShader(m_tesselationID) == GL_TRUE)
		glDeleteShader(m_tesselationID);

    if(glIsProgram(m_programID) == GL_TRUE)
        glDeleteProgram(m_programID);


    // Compilation des shaders

    if(!compilerShader(m_vertexID, GL_VERTEX_SHADER, m_vertexSource))
        return false;

    if(!compilerShader(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource))
        return false;

	if (m_geometrySource.size())
	{
		if (!compilerShader(m_geometryID, GL_GEOMETRY_SHADER, m_geometrySource))
			return false;

		/*GLchar ErrorLog2[4096] = { 0 };
		glGetShaderSource(m_geometryID, sizeof(ErrorLog2), NULL, ErrorLog2);
		fprintf(stderr, "Shader Source is: '%s'\n", ErrorLog2);*/
	}

	/*if (m_tesselationSource.size())
	{
		if (!compilerShader(m_tesselationID, GL_TES, m_geometrySource))
			return false;
	}*/


    // Création du programme

    m_programID = glCreateProgram();


    // Association des shaders

    glAttachShader(m_programID, m_vertexID);
    glAttachShader(m_programID, m_fragmentID);

	if (m_geometrySource.size())
	{
		glAttachShader(m_programID, m_geometryID);

		/*const GLchar* Varyings[3];
		Varyings[0] = "Position";
		Varyings[1] = "Velocity";
		Varyings[2] = "Age";

		glTransformFeedbackVaryings(m_programID, 3, Varyings, GL_INTERLEAVED_ATTRIBS);*/
	}

	return link();
}

bool Shader::link()
{
	GLint Success = 0;
	GLchar ErrorLog[2048] = { 0 };

	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(m_programID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program:lol '%s'\n", ErrorLog);

		
		//return false;
	}

	glValidateProgram(m_programID);
	glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(m_programID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		//   return false;
	}
		//glDeleteProgram(m_programID);

	return true;
}


bool Shader::compilerShader(GLuint &shader, GLenum type, std::string const &fichierSource)
{
    // Création du shader

    shader = glCreateShader(type);


    // Vérification du shader

    if(shader == 0)
    {
        std::cout << "Erreur, le type de shader (" << type << ") n'existe pas" << std::endl;
        return false;
    }


    // Flux de lecture

    std::ifstream fichier(fichierSource.c_str());


    // Test d'ouverture

    if(!fichier)
    {
        std::cout << "Erreur le fichier " << fichierSource << " est introuvable" << std::endl;
        glDeleteShader(shader);

        return false;
    }


    // Strings permettant de lire le code source

    std::string ligne;
    std::string codeSource;


    // Lecture

    while(getline(fichier, ligne))
        codeSource += ligne + '\n';


    // Fermeture du fichier

    fichier.close();


    // Récupération de la chaine C du code source

    const GLchar* chaineCodeSource = codeSource.c_str();


    // Envoi du code source au shader

    glShaderSource(shader, 1, &chaineCodeSource, 0);


    // Compilation du shader

    glCompileShader(shader);


    // Vérification de la compilation

    GLint erreurCompilation(0);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);


    // S'il y a eu une erreur

    if(erreurCompilation != GL_TRUE)
    {
        // Récupération de la taille de l'erreur

        GLint tailleErreur(0);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tailleErreur);


        // Allocation de mémoire

        char *erreur = new char[tailleErreur + 1];


        // Récupération de l'erreur

        glGetShaderInfoLog(shader, tailleErreur, &tailleErreur, erreur);
        erreur[tailleErreur] = '\0';


        // Affichage de l'erreur

        std::cout << erreur << std::endl;


        // Libération de la mémoire et retour du booléen false

        delete[] erreur;
        glDeleteShader(shader);

        return false;
    }


    // Sinon c'est que tout s'est bien passé

    else
        return true;
}


// Getter

GLuint Shader::getProgramID() const
{
    return m_programID;
}
