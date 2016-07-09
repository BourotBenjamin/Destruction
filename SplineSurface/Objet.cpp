#include "Objet.h"
#include "Quaternion.h"
#include <algorithm>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif


float maxZ = 400.f;


void setupFeedback(GLuint program)
{
	const GLchar* Varyings[3];
	Varyings[0] = "out_Position";
	Varyings[1] = "out_Velocity";
	Varyings[2] = "out_Age";
	glTransformFeedbackVaryings(program, 2, Varyings, GL_INTERLEAVED_ATTRIBS);
}

Objet::Objet() :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1)
{
	worldMatrix.identity();
}

Objet::Objet(std::string objfile, bool noNormal) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1)
{
	LoadFromObj(objfile, noNormal);
	worldMatrix.identity();
}

Objet::Objet(std::string objfile, uint32_t nbParticules, EsgiShader& programUpdate, std::string vertexShader, std::string fragmentShader, std::string geometryShader) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1),
currTFB(1), currVB(0), currSource(0)
{
	LoadFromObj(objfile);
	isParticul = true;
	this->nbParticules = nbParticules;

	std::vector<Particle> Particles(nbParticules);
	
	for (int i = 0; i < nbParticules; ++i)
	{
		Particles[i].Pos = Point();
		Particles[i].Vel = Point(RandomFloat(), RandomFloat(), RandomFloat());
		Particles[i].LifetimeMillis = 0.0f;
	}

	//Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
	Particles[0].Pos = position;
	Particles[0].Vel = Point(0.0f, 0.0001f, 0.0f);
	Particles[0].LifetimeMillis = 0.0f;

	/*glGenTransformFeedbacks(2, transformFeedback);
	glGenBuffers(2, particleBuffer);

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[i]);
	}*/

	glGenBuffers(2, particleBuffer);
	glGenVertexArrays(2, particuleUpdateVAO);
	glGenTransformFeedbacks(2, transformFeedback);
	GLsizei stride = sizeof(float)* 3;

	for (int i = 0; i < 2; i++)
	{
		stride = sizeof(Particle);
		glBindVertexArray(particuleUpdateVAO[i]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, nbParticules * stride, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[i]);
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		// velocity
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)12);
		glEnableVertexAttribArray(1);
		// Age
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)24);
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*const GLchar* Varyings[3];
	Varyings[0] = "Position";
	Varyings[1] = "Velocity";
	Varyings[2] = "Age";

	auto prog = programUpdate.getProgramID();
	bool isNVIDIA;

	glGetString(GL_VENDOR)[0] == 'N' ? isNVIDIA = true : isNVIDIA = false;

	if (isNVIDIA)
	{
		
		glUseProgram(prog);

		GLint* Varyings2 = new int[3];
		Varyings2[0] = glGetVaryingLocationNV(prog, Varyings[0]);
		Varyings2[1] = glGetVaryingLocationNV(prog, Varyings[1]);
		Varyings2[2] = glGetVaryingLocationNV(prog, Varyings[2]);

		glTransformFeedbackVaryingsNV(prog, 3, Varyings2, GL_INTERLEAVED_ATTRIBS);
		glUseProgram(0);

		delete[] Varyings2;
	}

	else
	{
		glTransformFeedbackVaryings(prog, 3, Varyings, GL_INTERLEAVED_ATTRIBS);
	}
	

	
	if (programUpdate.link())
		std::cout << "OOOOWWWW SHIT!";*/

	if (vertexShader.size())
		programUpdate.LoadVertexShader(vertexShader.c_str());
	//if (fragmentShader.size())
	//	programUpdate.LoadFragmentShader(fragmentShader.c_str());
	if (geometryShader.size())
		programUpdate.LoadGeometryShader(geometryShader.c_str());
	programUpdate.SetPreLinkCallback(setupFeedback);
	programUpdate.Create();


	Point* pRandomData = new Point[nbParticules];
	for (unsigned int i = 0; i < nbParticules; i++) {
		pRandomData[i].x = RandomFloat();
		pRandomData[i].y = RandomFloat();
		pRandomData[i].z = RandomFloat();
	}

	glGenTextures(1, &randomTex);
	glBindTexture(GL_TEXTURE_1D, randomTex);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, nbParticules, 0, GL_RGB, GL_FLOAT, pRandomData);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] pRandomData;

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[0]);
	float* particles = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < nbParticules; i++)
	{
		float rndx = (rand() / (float)RAND_MAX)*30.f - 15.f;
		float rndy = (rand() / (float)RAND_MAX)*5.f - 2.5f;
		float rndz = (rand() / (float)RAND_MAX)*5.f - 2.5f;
		//pos
		*particles++ = rndx;
		*particles++ = rndy;
		*particles++ = rndz;
		//velocity
		*particles++ = rndx;//0.f;
		*particles++ = rndx;//0.f;
		*particles++ = rndx;//0.f;
		//age
		*particles++ = 0.f;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[1]);
	//particles = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//for (int i = 0; i < nbParticules; i++)
	//{
	//	float rndx = (rand() / (float)RAND_MAX)*30.f - 15.f;
	//	float rndy = (rand() / (float)RAND_MAX)*5.f - 2.5f;
	//	float rndz = (rand() / (float)RAND_MAX)*5.f - 2.5f;
	//	//pos
	//	*particles++ = rndx;
	//	*particles++ = rndy;
	//	*particles++ = rndz;
	//	//velocity
	//	*particles++ = 0.f;
	//	*particles++ = 0.f;
	//	*particles++ = 0.f;
	//	//age
	//	*particles++ = 0.f;
	//}
	//glUnmapBuffer(GL_ARRAY_BUFFER);
	
}


Objet::Objet(std::vector<std::string>& v) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1)
{
	static const float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	int w, h, comp;

	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

	for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
	{
		const char * filename = v[faceIndex].c_str();
		uint8_t * data = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// RAPPEL: sizeof(skyboxVertices) fonctionne -cad qu'on obtient la taille totale du tableau-
	//			du fait que skyboxVertices est un tableau STATIC et donc que le compilateur peut deduire
	//			sa taille lors de la compilation. Autrement sizeof aurait du renvoyer la taille du pointeur.
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	
	worldMatrix.identity();
}

Objet::Objet(std::string diffuse, std::string normal) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0,0,1)
{
	//m_shader = shader;
	worldMatrix.identity();
	Point pos1(-1.0, 1.0, 0.0);
	Point pos2(-1.0, -1.0, 0.0);
	Point pos3(1.0, -1.0, 0.0);
	Point pos4(1.0, 1.0, 0.0);
	// texture coordinates
	Point uv1(0.0, 1.0,0);
	Point uv2(0.0, 0.0,0);
	Point uv3(1.0, 0.0,0);
	Point uv4(1.0, 1.0,0);
	// normal vector
	Point nm(0.0, 0.0, 1.0);

	// calculate tangent/bitangent vectors of both triangles
	Point tangent1, bitangent1;
	Point tangent2, bitangent2;
	// - triangle 1
	Point edge1 = pos2 - pos1;
	Point edge2 = pos3 - pos1;
	Point deltaUV1 = uv2 - uv1;
	Point deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV2.Getx() * deltaUV1.Gety());

	tangent1.Setx(f * (deltaUV2.Gety() * edge1.Getx() - deltaUV1.Gety() * edge2.Getx()));
	tangent1.Sety(f * (deltaUV2.Gety() * edge1.Gety() - deltaUV1.Gety() * edge2.Gety()));
	tangent1.Setz(f * (deltaUV2.Gety() * edge1.Getz() - deltaUV1.Gety() * edge2.Getz()));
	tangent1.normalize();

	bitangent1.Setx(f * (-deltaUV2.Getx() * edge1.Getx() + deltaUV1.Getx() * edge2.Getx()));
	bitangent1.Sety(f * (-deltaUV2.Getx() * edge1.Gety() + deltaUV1.Getx() * edge2.Gety()));
	bitangent1.Setz(f * (-deltaUV2.Getx() * edge1.Getz() + deltaUV1.Getx() * edge2.Getz()));
	bitangent1.normalize();

	// - triangle 2
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV2.Getx() * deltaUV1.Gety());

	tangent2.Setx(f * (deltaUV2.Gety() * edge1.Getx() - deltaUV1.Gety() * edge2.Getx()));
	tangent2.Sety(f * (deltaUV2.Gety() * edge1.Gety() - deltaUV1.Gety() * edge2.Gety()));
	tangent2.Setz(f * (deltaUV2.Gety() * edge1.Getz() - deltaUV1.Gety() * edge2.Getz()));
	tangent2.normalize();


	bitangent2.Setx(f * (-deltaUV2.Getx() * edge1.Getx() + deltaUV1.Getx() * edge2.Getx()));
	bitangent2.Sety(f * (-deltaUV2.Getx() * edge1.Gety() + deltaUV1.Getx() * edge2.Gety()));
	bitangent2.Setz(f * (-deltaUV2.Getx() * edge1.Getz() + deltaUV1.Getx() * edge2.Getz()));
	bitangent2.normalize();

	std::vector<uint32_t> indices{ 1, 2, 3, 1, 3, 4 };
	ElementCount = 2;
	//ebo
	//123   134
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLfloat quadVertices[] = {
		// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
		pos1.Getx(), pos1.Gety(), pos1.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv1.Getx(), uv1.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos2.Getx(), pos2.Gety(), pos2.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv2.Getx(), uv2.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos3.Getx(), pos3.Gety(), pos3.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv3.Getx(), uv3.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos4.Getx(), pos4.Gety(), pos4.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv4.Getx(), uv4.Gety(), tangent2.Getx(), tangent2.Gety(), tangent2.Getz(), bitangent2.Getx(), bitangent2.Gety(), bitangent2.Getz()
	};
	// Setup plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));

	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(diffuse.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glGenTextures(1, &normalObj);
	glBindTexture(GL_TEXTURE_2D, normalObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//int w, h;
	data = stbi_load(normal.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	worldMatrix.identity();
}

Objet::~Objet()
{
	if (textureObj)
		glDeleteTextures(1, &textureObj);
	if (isSpecularMap)
		glDeleteTextures(1, &specularObj);
	if (isNormalMap)
		glDeleteTextures(1, &normalObj); 
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (EBO)
		glDeleteBuffers(1, &EBO);
}

void Objet::setupStruct()
{
	face.clear();
	edge.clear();
	vertex.clear();

	face.reserve(100000);
	edge.reserve(100000);
	vertex.reserve(100000);

	for (int it = 0; it < vertice.size(); ++it)
	{
		Vertex v;
		v.indice = it;
		v.position = vertice[it];
		if (std::find(vertex.begin(), vertex.end(), v) == vertex.end())
			vertex.push_back(v);
	}
	for (auto it = triangle.begin(); it != triangle.end(); ++it)
	{
		Face f;
		f.indice[0] = it->indice[0];
		f.indice[1] = it->indice[1];
		f.indice[2] = it->indice[2];

		Vertex * v1 = &(*std::find(vertex.begin(), vertex.end(), vertice[f.indice[0]]));
		Vertex * v2 = &(*std::find(vertex.begin(), vertex.end(), vertice[f.indice[1]]));
		Vertex * v3 = &(*std::find(vertex.begin(), vertex.end(), vertice[f.indice[2]]));


		f.vertice[0] = v1;
		f.vertice[1] = v2;
		f.vertice[2] = v3;

		MyEdge e1;
		e1.indice[0] = it->indice[0];
		e1.indice[1] = it->indice[1];

		e1.vertice[0] = v1;
		e1.vertice[1] = v2;

		e1.voisin[0] = nullptr;
		e1.voisin[1] = nullptr;

		e1.verticeVoisin[0] = nullptr;
		e1.verticeVoisin[1] = nullptr;

		if (std::find(edge.begin(), edge.end(), e1) == edge.end())
			edge.push_back(e1);

		MyEdge e2;
		e2.indice[0] = it->indice[1];
		e2.indice[1] = it->indice[2];
		 
		e2.vertice[0] = v2;
		e2.vertice[1] = v3;

		e2.voisin[0] = nullptr;
		e2.voisin[1] = nullptr;

		e2.verticeVoisin[0] = nullptr;
		e2.verticeVoisin[1] = nullptr;

		if (std::find(edge.begin(), edge.end(), e2) == edge.end())
			edge.push_back(e2);

		MyEdge e3;
		e3.indice[0] = it->indice[0];
		e3.indice[1] = it->indice[2];
		 
		e3.vertice[0] = v1;
		e3.vertice[1] = v3;

		e3.voisin[0] = nullptr;
		e3.voisin[1] = nullptr;

		e3.verticeVoisin[0] = nullptr;
		e3.verticeVoisin[1] = nullptr;

		if (std::find(edge.begin(), edge.end(), e3) == edge.end())
			edge.push_back(e3);

		MyEdge * pe1 = &(*std::find(edge.begin(), edge.end(), e1));
		MyEdge * pe2 = &(*std::find(edge.begin(), edge.end(), e2));
		MyEdge * pe3 = &(*std::find(edge.begin(), edge.end(), e3));

		if (std::find(v1->edge.begin(), v1->edge.end(), pe1) == v1->edge.end())
			v1->edge.push_back(pe1);
		if (std::find(v1->edge.begin(), v1->edge.end(), pe3) == v1->edge.end())
			v1->edge.push_back(pe3);

		if (std::find(v2->edge.begin(), v2->edge.end(), pe1) == v2->edge.end())
			v2->edge.push_back(pe1);
		if (std::find(v2->edge.begin(), v2->edge.end(), pe2) == v2->edge.end())
			v2->edge.push_back(pe2);

		if (std::find(v3->edge.begin(), v3->edge.end(), pe2) == v3->edge.end())
			v3->edge.push_back(pe2);
		if (std::find(v3->edge.begin(), v3->edge.end(), pe3) == v3->edge.end())
			v3->edge.push_back(pe3);


		f.edge[0] = pe1;
		f.edge[1] = pe2;
		f.edge[2] = pe3;

		face.push_back(f);

		Face * pf = &face.back();

		v1->face.push_back(pf);
		v2->face.push_back(pf);
		v3->face.push_back(pf);

		if (pe1->voisin[0] == nullptr)
			pe1->voisin[0] = pf;
		else
			pe1->voisin[1] = pf;

		if (pe2->voisin[0] == nullptr)
			pe2->voisin[0] = pf;
		else
			pe2->voisin[1] = pf;

		if (pe3->voisin[0] == nullptr)
			pe3->voisin[0] = pf;
		else
			pe3->voisin[1] = pf;

		if (pe1->verticeVoisin[0] == nullptr)
			pe1->verticeVoisin[0] = v3;
		else
			pe1->verticeVoisin[1] = v3;

		if (pe2->verticeVoisin[0] == nullptr)
			pe2->verticeVoisin[0] = v1;
		else
			pe2->verticeVoisin[1] = v1;

		if (pe3->verticeVoisin[0] == nullptr)
			pe3->verticeVoisin[0] = v2;
		else
			pe3->verticeVoisin[1] = v2;

		
	}


}

void Objet::setFacePoint()
{
	facePoint.clear();
	int i = 0;
	for (auto it = face.begin(); it != face.end(); ++it)
	{
		Vertex v;
		v.indice = i;
		v.position = (it->vertice[0]->position + it->vertice[1]->position + it->vertice[2]->position) / 3.f;
		it->facePoint = (it->vertice[0]->position + it->vertice[1]->position + it->vertice[2]->position) / 3.f;
		facePoint.push_back(v);
	}
}

void Objet::setEdgePoint()
{
	edgePoint.clear();
	int i = facePoint.size();
	for (auto it = edge.begin(); it != edge.end(); ++it)
	{
		Vertex v;
		v.indice = i;
		v.position = (it->vertice[0]->position + it->vertice[1]->position + it->voisin[0]->facePoint + it->voisin[1]->facePoint) / 4.f;
		it->edgePoint = (it->vertice[0]->position + it->vertice[1]->position + it->voisin[0]->facePoint + it->voisin[1]->facePoint) / 4.f;
		
		edgePoint.push_back(v);
	}
}

void Objet::setVertexPoint()
{
	vertexPoint.clear();
	int i = facePoint.size() + edgePoint.size();
	for (auto it = vertex.begin(); it != vertex.end(); ++it)
	{
		Point Q;
		for (auto it2 = it->face.begin(); it2 != it->face.end(); ++it2)
		{
			Q += (*it2)->facePoint;
		}
		Q /= it->face.size();

		Point R;
		for (auto it2 = it->edge.begin(); it2 != it->edge.end(); ++it2)
		{
			R += ((*it2)->vertice[0]->position + (*it2)->vertice[1]->position)/2.f;
		}
		R /= it->edge.size();

		Point P;

		P = it->position;

		int n = it->edge.size();

		Vertex v;
		v.indice = i;
		v.position = (Q/n) + (R * 2)/n + (P * (n - 3))/n;

		it->vertexPoint = (Q / n) + (R * 2) / n + (P * (n - 3)) / n;

		vertexPoint.push_back(v);
	}
}

void Objet::subdivisionCatmullClark()
{
	int i = 0;
	Point O(0.f, -100.f, 0.f);
	ElementCount = 0;
	eboIndices.clear();
	vboPos.clear();
	unsigned int faceIndice = 0, currentVerticeIndice, lastInd1 = -1, lastInd2 = -1;
	float minAngleVer = 0.f, minAngleEdg = 0.f, currentMinAngle = FLT_MAX, currentAngle;
	bool edg[3], ver[3];
	for (auto it = face.begin(); it != face.end(); ++it)
	{
		vboPos.push_back(it->facePoint.x);
		vboPos.push_back(it->facePoint.y);
		vboPos.push_back(it->facePoint.z);
		//std::cout << "F : " << it->facePoint.x << " ; " << it->facePoint.y << " ; " << it->facePoint.z << std::endl;
		minAngleEdg = 0.f;
		minAngleVer = 0.f;
		lastInd1 = -1, lastInd2 = -1;
		edg[0] = edg[1] = edg[2] = ver[0] = ver[1] = ver[2] = false;

		vboPos.push_back(it->edge[1]->edgePoint.x);
		vboPos.push_back(it->edge[1]->edgePoint.y);
		vboPos.push_back(it->edge[1]->edgePoint.z);
		vboPos.push_back(it->vertice[1]->vertexPoint.x);
		vboPos.push_back(it->vertice[1]->vertexPoint.y);
		vboPos.push_back(it->vertice[1]->vertexPoint.z);

		vboPos.push_back(it->edge[0]->edgePoint.x);
		vboPos.push_back(it->edge[0]->edgePoint.y);
		vboPos.push_back(it->edge[0]->edgePoint.z);
		vboPos.push_back(it->vertice[0]->vertexPoint.x);
		vboPos.push_back(it->vertice[0]->vertexPoint.y);
		vboPos.push_back(it->vertice[0]->vertexPoint.z);

		vboPos.push_back(it->edge[2]->edgePoint.x);
		vboPos.push_back(it->edge[2]->edgePoint.y);
		vboPos.push_back(it->edge[2]->edgePoint.z);
		vboPos.push_back(it->vertice[2]->vertexPoint.x);
		vboPos.push_back(it->vertice[2]->vertexPoint.y);
		vboPos.push_back(it->vertice[2]->vertexPoint.z);


		eboIndices.push_back(faceIndice + 1);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 2);
		eboIndices.push_back(faceIndice + 2);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 3);
		eboIndices.push_back(faceIndice + 3);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 4);
		eboIndices.push_back(faceIndice + 4);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 5);
		eboIndices.push_back(faceIndice + 5);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 6);
		eboIndices.push_back(faceIndice + 6);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 1);
		faceIndice += 7;
		ElementCount += 18;
	}

	reload();


	const std::vector<float> normals, texcoords;
	std::vector<tinyobj::material_t> materials;
	LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);

	this->setupStruct();
	this->setFacePoint();
	this->setEdgePoint();
	this->setVertexPoint();
}

void Objet::subdivisionLoop()
{

	eboIndices.clear();
	vboPos.clear();
	ElementCount = 0;
	int faceIndice = 0;
	for each(auto f in face)
	{

		Point eP[3], vP[3];
		for (int i = 0; i < 3; ++i)
		{
			eP[i] = (f.edge[i]->vertice[0]->position + f.edge[i]->vertice[1]->position) * (3.0f / 8.0f) + (f.edge[i]->verticeVoisin[0]->position + f.edge[i]->verticeVoisin[1]->position) * (1.0f / 8.0f);
			for each (auto edge in f.vertice[i]->edge)
			{
				if (f.vertice[i] == edge->vertice[0])
					vP[i] += edge->vertice[1]->position;
				else
					vP[i] += edge->vertice[0]->position;
			}
			float n = f.vertice[i]->edge.size();
			float a;
			if (n == 3)
			{
				a = 3 / 16;
			}
			else
			{
				a = (1.0f / n) * ((5.0f / 8.0f) - ((3.0f / 8.0f) + ((1.0f / 4.0f) * cos(2 * M_PI / n)) * ((1.0f / 4.0f) * cos(2 * M_PI / n))));
			}
			vP[i] = f.vertice[i]->position * (1 - n * a) + vP[i] * a;
		}


		vboPos.push_back(eP[0].x);
		vboPos.push_back(eP[0].y);
		vboPos.push_back(eP[0].z);
		vboPos.push_back(eP[1].x);
		vboPos.push_back(eP[1].y);
		vboPos.push_back(eP[1].z);
		vboPos.push_back(eP[2].x);
		vboPos.push_back(eP[2].y);
		vboPos.push_back(eP[2].z);
		vboPos.push_back(vP[0].x);
		vboPos.push_back(vP[0].y);
		vboPos.push_back(vP[0].z);
		vboPos.push_back(vP[1].x);
		vboPos.push_back(vP[1].y);
		vboPos.push_back(vP[1].z);
		vboPos.push_back(vP[2].x);
		vboPos.push_back(vP[2].y);
		vboPos.push_back(vP[2].z);

		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 1);
		eboIndices.push_back(faceIndice + 2);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 4);
		eboIndices.push_back(faceIndice + 1);
		eboIndices.push_back(faceIndice);
		eboIndices.push_back(faceIndice + 2);
		eboIndices.push_back(faceIndice + 3);
		eboIndices.push_back(faceIndice + 1);
		eboIndices.push_back(faceIndice + 5);
		eboIndices.push_back(faceIndice + 2);
		ElementCount += 12;
		ElementCount += 6;
		faceIndice += 6;


	}
	reload();
	reloadEBO();
	const std::vector<float> normals, texcoords;
	std::vector<tinyobj::material_t> materials;
	LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
	this->setupStruct();
	this->setFacePoint();
	this->setEdgePoint();
	this->setVertexPoint();

}

bool less(Point a, Point b, Point center)
{
	if (a.x - center.x >= 0 && b.x - center.x < 0)
		return true;
	if (a.x - center.x < 0 && b.x - center.x >= 0)
		return false;
	if (a.x - center.x == 0 && b.x - center.x == 0) {
		if (a.y - center.y >= 0 || b.y - center.y >= 0)
			return a.y > b.y;
		return b.y > a.y;
	}

	// compute the cross product of vectors (center -> a) x (center -> b)
	int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
	if (det < 0)
		return true;
	if (det > 0)
		return false;

	// points a and b are on the same line from the center
	// check which point is closer to the center
	int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
	int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
	return d1 > d2;
}

void Objet::CalculKobbelt()
{
	ElementCount = 0;
	vboPos.clear();
	eboIndices.clear();
	// calcul des centres
	std::vector<Vertex> vertexToAdd;
	for (int i = 0; i < face.size(); i++)
	{
		Face& oneFace = face.at(i);
		Vertex center;
		center.indice = vertex.size() + i;
		center.vertexPoint = (oneFace.vertice[0]->vertexPoint + oneFace.vertice[1]->vertexPoint + oneFace.vertice[2]->vertexPoint) / 3.f;
		vertexToAdd.push_back(center);
		oneFace.FP = center;
	}
	std::vector<Vertex> perturbed;
	for (auto& vertice : vertex)
	{
		int n = vertice.edge.size();
		vertice.indice = vboPos.size() / 3;
		Vertex perturbedVertex(vertice);
		Point sumNeighbours;

		float alpha = (4.f - 2.f * cos(2.f * M_PI / n)) / 9.f;
		for (auto& oneEdge : vertice.edge)
		{
			sumNeighbours += (vertice.vertexPoint == oneEdge->vertice[0]->vertexPoint
				? oneEdge->vertice[1]->vertexPoint
				: oneEdge->vertice[0]->vertexPoint);
		}
		perturbedVertex.vertexPoint = (vertice.vertexPoint)*(1.f - alpha) + (sumNeighbours*(alpha / n));

		perturbed.push_back(perturbedVertex);
		vboPos.push_back(perturbedVertex.vertexPoint.x);
		vboPos.push_back(perturbedVertex.vertexPoint.y);
		vboPos.push_back(perturbedVertex.vertexPoint.z);
	}
	for (auto& v : vertexToAdd)
	{
		vertex.push_back(v);
		vboPos.push_back(v.vertexPoint.x);
		vboPos.push_back(v.vertexPoint.y);
		vboPos.push_back(v.vertexPoint.z);
	}
	std::vector<Face> newFaces;
	for (int i = 0; i < edge.size(); i++)
	{
		
		MyEdge& oneEdge = edge.at(i);
		// Test for 2D meshes
		if (oneEdge.voisin[0] != nullptr && oneEdge.voisin[1] != nullptr)
		{
			Face face1, face2;
			face1.indice[0] = face2.indice[0] = (oneEdge.voisin[0])->FP.indice;
			face1.indice[1] = face2.indice[1] = (oneEdge.voisin[1])->FP.indice;
			face1.indice[2] = oneEdge.vertice[0]->indice;
			face2.indice[2] = oneEdge.vertice[1]->indice;

			Point center1 = ((oneEdge.voisin[0])->FP.position + (oneEdge.voisin[1])->FP.position + oneEdge.vertice[0]->position) / 3.f;
			Point center2 = ((oneEdge.voisin[0])->FP.position + (oneEdge.voisin[1])->FP.position + oneEdge.vertice[1]->position) / 3.f;
			
			newFaces.push_back(face1);
			newFaces.push_back(face2);

			if (!less((oneEdge.voisin[1])->FP.position, oneEdge.vertice[0]->position, center1) || less((oneEdge.voisin[0])->FP.position, oneEdge.vertice[0]->position, center1))
			{
				eboIndices.push_back(face1.indice[1]);
				eboIndices.push_back(face1.indice[0]);
				eboIndices.push_back(face1.indice[2]);
			}
			else
			{
				eboIndices.push_back(face1.indice[1]);
				eboIndices.push_back(face1.indice[2]);
				eboIndices.push_back(face1.indice[0]);
			}

			if (!less((oneEdge.voisin[1])->FP.position, oneEdge.vertice[1]->position, center2) || less((oneEdge.voisin[0])->FP.position, oneEdge.vertice[1]->position, center2))
			{
				eboIndices.push_back(face2.indice[1]);
				eboIndices.push_back(face2.indice[0]);
				eboIndices.push_back(face2.indice[2]);
			}
			else
			{
				eboIndices.push_back(face2.indice[1]);
				eboIndices.push_back(face2.indice[2]);
				eboIndices.push_back(face2.indice[0]);
			}

		}
		else
		{
			Face face;
			face.indice[0] = oneEdge.vertice[0]->indice;
			face.indice[1] = oneEdge.vertice[1]->indice;
			if (oneEdge.voisin[0] != nullptr)
				face.indice[2] = oneEdge.voisin[0]->FP.indice;
			else
				face.indice[2] = oneEdge.voisin[1]->FP.indice;
			newFaces.push_back(face);

			eboIndices.push_back(face.indice[0]);
			eboIndices.push_back(face.indice[1]);
			eboIndices.push_back(face.indice[2]);
		}
	}
	ElementCount = eboIndices.size();
	face = newFaces;
	reload();

	const std::vector<float> normals, texcoords;
	std::vector<tinyobj::material_t> materials;
	LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
	this->setupStruct();
	this->setFacePoint();
	this->setEdgePoint();
	this->setVertexPoint();
}

void Objet::reload()
{
	if (glIsBuffer(VAO) == GL_TRUE)
		glDeleteBuffers(1, &VAO);
	if (glIsBuffer(VBO) == GL_TRUE)
		glDeleteBuffers(1, &VBO);
	if (glIsBuffer(EBO) == GL_TRUE)
		glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vboPos.size()* sizeof(float), nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vboPos.size()* sizeof(float), vboPos.data());

	//ElementCount = eboIndices.size() / 3;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboIndices.size() * sizeof(uint32_t), &eboIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Objet::reloadEBO()
{
	

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
}

void Objet::LoadFromObj(std::string file, bool noNormal)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, file.c_str());
	const std::vector<unsigned int>& indices = shapes[0].mesh.indices;
	const std::vector<float>& positions = shapes[0].mesh.positions;
	const std::vector<float>& normals = shapes[0].mesh.normals;
	const std::vector<float>& texcoords = shapes[0].mesh.texcoords;

	LoadByDatas(indices, positions, normals, texcoords, file, materials, noNormal);
}

void Objet::loadVerticesAndIndices(const std::vector<unsigned int>& indices, const std::vector<float>& positions)
{
	for each (float pos in positions)
	{
		vboPos.push_back(pos);
	}
	for each (float indice in indices)
	{
		eboIndices.push_back(indice);
	}
}

void Objet::LoadByDatas(const std::vector<unsigned int>& indices, const std::vector<float>& positions, const std::vector<float>& normals, const std::vector<float>& texcoords, std::string& file, std::vector<tinyobj::material_t>& materials, bool noNormal)
{
	triangle.clear();
	vertice.clear();
	isSpecularMap = isDiffuseMap = isNormalMap = false;
	for (int i = 0; i < indices.size(); i += 3)
	{
		Triangle t;
		t.indice[0] = indices[i];
		t.indice[1] = indices[i + 1];
		t.indice[2] = indices[i + 2];
		triangle.push_back(t);
	}
	for (int i = 0; i < positions.size(); i += 3)
	{
		Point p;
		p.x = positions[i];
		p.y = positions[i + 1];
		p.z = positions[i + 2];
		vertice.push_back(p);
	}

	std::vector<float> tangentes;
	std::vector<float> biTangentes;

	ElementCount = indices.size();
	if (!materials.empty())
	{
		if (file == "cyborg.obj")
			materials[0].bump_texname = "cyborg_normal.png";

		if (materials[0].diffuse_texname.size())
		{
			isDiffuseMap = true;
		}
	}

	uint32_t stride = 0;

	if (positions.size()) {
		stride += 3 * sizeof(float);
	}
	if (normals.size()) {
		stride += 3 * sizeof(float);
	}
	if (isDiffuseMap) {
		stride += 2 * sizeof(float);
	}
	if (!materials.empty())
	{
		if (materials[0].specular_texname.size())
			isSpecularMap = true;
		if (materials[0].bump_texname.size() && noNormal == false)
		{
			isNormalMap = true;
			stride += 6 * sizeof(float);
		}
		if (materials[0].diffuse_texname.size())
		{
			isDiffuseMap = true;
		}

		if (isDiffuseMap)
		{
			ambiant = Point(materials[0].ambient[0], materials[0].ambient[1], materials[0].ambient[2]);
			diffuse = Point(materials[0].diffuse[0], materials[0].diffuse[1], materials[0].diffuse[2]);
			specular = Point(materials[0].specular[0], materials[0].specular[1], materials[0].specular[2]);
			shininess = materials[0].shininess;
		}
	}

	const auto count = positions.size() / 3;
	const auto totalSize = count * stride;

	//calcul des tangentes et bitangente
	if (isNormalMap)
	{
		tangentes.reserve(positions.size());
		biTangentes.reserve(positions.size());
		for (int i = 0; i < positions.size(); ++i)
		{
			tangentes.push_back(0);
			biTangentes.push_back(0);
		}
		for (int i = 0; i < indices.size(); i += 3)
		{
			// Shortcuts for vertices
			Point v0 = Point(positions[(indices[i] * 3) + 0], positions[(indices[i] * 3) + 1], positions[(indices[i] * 3) + 2]);
			Point v1 = Point(positions[((indices[i + 1]) * 3) + 0], positions[((indices[i + 1]) * 3) + 1], positions[((indices[i + 1]) * 3) + 2]);
			Point v2 = Point(positions[((indices[i + 2]) * 3) + 0], positions[((indices[i + 2]) * 3) + 1], positions[((indices[i + 2]) * 3) + 2]);

			// Shortcuts for UVs
			Point uv0 = Point(texcoords[((indices[i]) * 2) + 0], texcoords[((indices[i]) * 2) + 1], 0);
			Point uv1 = Point(texcoords[((indices[i + 1]) * 2) + 0], texcoords[((indices[i + 1]) * 2) + 1], 0);
			Point uv2 = Point(texcoords[((indices[i + 2]) * 2) + 0], texcoords[((indices[i + 2]) * 2) + 1], 0);

			// Edges of the triangle : postion delta
			Point deltaPos1 = v1 - v0;
			Point deltaPos2 = v2 - v0;

			// UV delta
			Point deltaUV1 = uv1 - uv0;
			Point deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV1.Gety() * deltaUV2.Getx());
			Point tangent = (deltaPos1 * deltaUV2.Gety() - deltaPos2 * deltaUV1.Gety())*r;
			Point bitangent = (deltaPos2 * deltaUV1.Getx() - deltaPos1 * deltaUV2.Getx())*r;

			tangentes[(indices[i] * 3)] += tangent.Getx();
			tangentes[(indices[i] * 3) + 1] += tangent.Gety();
			tangentes[(indices[i] * 3) + 2] += tangent.Getz();

			tangentes[((indices[i + 1]) * 3)] += tangent.Getx();
			tangentes[((indices[i + 1]) * 3) + 1] += tangent.Gety();
			tangentes[((indices[i + 1]) * 3) + 2] += tangent.Getz();

			tangentes[((indices[i + 2]) * 3)] += tangent.Getx();
			tangentes[((indices[i + 2]) * 3) + 1] += tangent.Gety();
			tangentes[((indices[i + 2]) * 3) + 2] += tangent.Getz();

			biTangentes[(indices[i] * 3)] += bitangent.Getx();
			biTangentes[(indices[i] * 3) + 1] += bitangent.Gety();
			biTangentes[(indices[i] * 3) + 2] += bitangent.Getz();

			biTangentes[((indices[i + 1]) * 3)] += bitangent.Getx();
			biTangentes[((indices[i + 1]) * 3) + 1] += bitangent.Gety();
			biTangentes[((indices[i + 1]) * 3) + 2] += bitangent.Getz();

			biTangentes[((indices[i + 2]) * 3)] += bitangent.Getx();
			biTangentes[((indices[i + 2]) * 3) + 1] += bitangent.Gety();
			biTangentes[((indices[i + 2]) * 3) + 2] += bitangent.Getz();

		}


	}


	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

	// glMapBuffer retourne un pointeur sur la zone memoire allouee par glBufferData 
	// du Buffer Object qui est actuellement actif - via glBindBuffer(<cible>, <id>)
	// il est imperatif d'appeler glUnmapBuffer() une fois que l'on a termine car le
	// driver peut tres bien etre amener a modifier l'emplacement memoire du BO.
	float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (auto index = 0; index < count; ++index)
	{
		if (positions.size()) {
			memcpy(vertices, &positions[index * 3], 3 * sizeof(float));
			vertices += 3;
		}
		if (normals.size()) {
			memcpy(vertices, &normals[index * 3], 3 * sizeof(float));
			vertices += 3;
		}
		if (isDiffuseMap) {
			memcpy(vertices, &texcoords[index * 2], 2 * sizeof(float));
			vertices += 2;
		}
		if (isNormalMap)
		{
			memcpy(vertices, &tangentes[index * 3], 3 * sizeof(float));
			vertices += 3;
			memcpy(vertices, &biTangentes[index * 3], 3 * sizeof(float));
			vertices += 3;
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	uint32_t offset = 3 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, nullptr);
	glEnableVertexAttribArray(0);
	if (normals.size()) {
		glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
		glEnableVertexAttribArray(1);
		offset += 3 * sizeof(float);
	}
	if (isDiffuseMap) {
		glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (GLvoid *)offset);
		glEnableVertexAttribArray(2);
		offset += 2 * sizeof(float);
	}
	if (isNormalMap)
	{
		glVertexAttribPointer(3, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
		glEnableVertexAttribArray(3);
		offset += 3 * sizeof(float);

		glVertexAttribPointer(4, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
		glEnableVertexAttribArray(4);
		offset += 3 * sizeof(float);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (isDiffuseMap)
	{
		glGenTextures(1, &textureObj);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);

		// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
		// autrement le Texture Object est considere comme invalide
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int w, h;
		uint8_t *data = stbi_load(materials[0].diffuse_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	if (isSpecularMap)
	{
		glGenTextures(1, &specularObj);
		glBindTexture(GL_TEXTURE_2D, specularObj);

		// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
		// autrement le Texture Object est considere comme invalide
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int w, h;
		uint8_t *data = stbi_load(materials[0].specular_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	if (isNormalMap)
	{
		glGenTextures(1, &normalObj);
		glBindTexture(GL_TEXTURE_2D, normalObj);

		// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
		// autrement le Texture Object est considere comme invalide
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int w, h;
		uint8_t *data = stbi_load(materials[0].bump_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}
}

void Objet::render(GLuint& program, GLuint shadowText)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	if (isDiffuseMap)
	{
		UpdateMaterial(program);
		auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
		glUniform1i(diffTexLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	if (isSpecularMap)
	{
		auto specTexLocation = glGetUniformLocation(program, "u_specularMap");
		glUniform1i(specTexLocation, 1);
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularObj);
	}
	else
	{
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 0);
	}

	if (shadowText != 0)
	{
		auto shadowLoc = glGetUniformLocation(program, "u_shadowMap");
		glUniform1i(shadowLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowText);
	}

	if(isNormalMap)
	{
		auto bumpTexLocation = glGetUniformLocation(program, "u_normalMap");
		glUniform1i(bumpTexLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalObj);
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPointSize(10);
	//glDrawElements(GL_POINTS, ElementCount, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Objet::render2(GLuint& program, GLuint shadowText, bool wireframe)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	if (isDiffuseMap)
	{
		UpdateMaterial(program);
		auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
		glUniform1i(diffTexLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	if (isSpecularMap)
	{
		auto specTexLocation = glGetUniformLocation(program, "u_specularMap");
		glUniform1i(specTexLocation, 1);
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularObj);
	}
	else
	{
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 0);
	}

	if (shadowText != 0)
	{
		auto shadowLoc = glGetUniformLocation(program, "u_shadowMap");
		glUniform1i(shadowLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowText);
	}

	if (isNormalMap)
	{
		auto bumpTexLocation = glGetUniformLocation(program, "u_normalMap");
		glUniform1i(bumpTexLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalObj);
	}
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//glPointSize(10);
	//glDrawElements(GL_POINTS, ElementCount, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Objet::render3(GLuint& program, GLuint shadowText, bool wireframe)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	if (isDiffuseMap)
	{
		UpdateMaterial(program);
		auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
		glUniform1i(diffTexLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	if (isSpecularMap)
	{
		auto specTexLocation = glGetUniformLocation(program, "u_specularMap");
		glUniform1i(specTexLocation, 1);
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularObj);
	}
	else
	{
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 0);
	}

	if (shadowText != 0)
	{
		auto shadowLoc = glGetUniformLocation(program, "u_shadowMap");
		glUniform1i(shadowLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowText);
	}

	if (isNormalMap)
	{
		auto bumpTexLocation = glGetUniformLocation(program, "u_normalMap");
		glUniform1i(bumpTexLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalObj);
	}
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glPointSize(10);
	//glDrawElements(GL_POINTS, ElementCount, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Objet::renderNormalMap(GLuint& program)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");
	

	glBindTexture(GL_TEXTURE_2D, textureObj);
	glBindTexture(GL_TEXTURE_2D, normalObj);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
}

void Objet::renderCubeMap(GLuint& program)
{

	auto cubemapIndex = glGetUniformLocation(program, "u_CubeMap");
	glUniform1f(cubemapIndex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(VAO);
	// Tres important ! D'une part, comme la cubemap represente un environnement distant
	// il n'est pas utile d'ecrire dans le depth buffer (on est toujours au plus loin)
	// cependant il faut quand effectuer le test de profondeur (donc on n'a pas glDisable(GL_DEPTH_TEST)).
	// Neamoins il faut legerement changer l'operateur du test dans le cas ou 
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 8 * 2 * 3);
	// on retabli ensuite les render states par defaut
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void Objet::renderBillBoardParticules(GLuint& program)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	if (isDiffuseMap)
	{
		UpdateMaterial(program);
		auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
		glUniform1i(diffTexLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	// on affiche le vbo non transforme
	glBindVertexArray(particuleUpdateVAO[currTFB]);
	glDrawArrays(GL_POINTS, 0, nbParticules);
	//glDrawTransformFeedback(GL_POINTS, transformFeedback[currTFB]);
	glBindVertexArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currTFB]);

	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);  // position

	//glDrawTransformFeedback(GL_POINTS, transformFeedback[currTFB]);

	//glDisableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Objet::UpdateMaterial(GLuint& program)
{
	GLint matAmbientLoc = glGetUniformLocation(program, "u_ambient");
	GLint matDiffuseLoc = glGetUniformLocation(program, "u_diffuse");
	GLint matSpecularLoc = glGetUniformLocation(program, "u_specular");
	GLint matShininessLoc = glGetUniformLocation(program, "u_shininess");

	glUniform3f(matAmbientLoc, ambiant.Getx(), ambiant.Gety(), ambiant.Getz());
	glUniform3f(matDiffuseLoc, diffuse.Getx(), diffuse.Gety(), diffuse.Getz());
	glUniform3f(matSpecularLoc, specular.Getx(), specular.Gety(), specular.Getz());
	glUniform1f(matShininessLoc, shininess);
}

void Objet::UpdateTransform()
{ 
	
	auto t = Mat4x4::translate(position.x, position.y, position.z);
	Quaternion r = Quaternion::eulerAngle(rotation);
	auto s = Mat4x4::scale(scale.x, scale.y, scale.z);
	worldMatrix.identity();
	worldMatrix *= t;
	worldMatrix *= r.to_mat4();
	worldMatrix *= s;
}


void Objet::UpdateParticules(GLuint& program, float deltaTime)
{
	time += deltaTime;

	GLint timeLoc = glGetUniformLocation(program, "u_time");
	GLint deltaTimeLoc = glGetUniformLocation(program, "u_deltaTime");
	glUniform1f(timeLoc, time);
	glUniform1f(deltaTimeLoc, deltaTime);

	auto randTexLocation = glGetUniformLocation(program, "u_random");
	glUniform1i(randTexLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, randomTex);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(particuleUpdateVAO[currVB]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[currTFB]);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[currTFB]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, nbParticules);
	/*if (isFirst) {
		glDrawArrays(GL_POINTS, 0, nbParticules);

		isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, transformFeedback[currVB]);
	}*/
	glEndTransformFeedback();
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glDisable(GL_RASTERIZER_DISCARD);


	//glEnable(GL_RASTERIZER_DISCARD);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currVB]);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[currTFB]);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12);         // position
	//glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24);        // velocity

	//glBeginTransformFeedback(GL_POINTS);

	//if (isFirst) {
	//	glDrawArrays(GL_POINTS, 0, 1);

	//	isFirst = false;
	//}
	//else {
	//	glDrawTransformFeedback(GL_POINTS, transformFeedback[currVB]);
	//}

	//glEndTransformFeedback();

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glDisable(GL_RASTERIZER_DISCARD);
}

void Objet::setLookAtPoint(Point p)
{
	auto q = Quaternion::RotationBetweenVectors(lookat, p);
	rotation += q.toEulerAngle();
	lookat = p;
	isLookat = true;
}

std::shared_ptr<Polyhedron_3> Objet::generatePolyhedron()
{
	this->setupStruct();
	std::shared_ptr<Polyhedron_3> p = std::shared_ptr<Polyhedron_3>(new Polyhedron_3());
	auto f = this->face.begin();
	while (f != this->face.end())
	{
		K::Point_3 p1(f->vertice[0]->position.Getx(), f->vertice[0]->position.Gety(), f->vertice[0]->position.Getz()), p2(f->vertice[1]->position.Getx(), f->vertice[1]->position.Gety(), f->vertice[1]->position.Getz()), p3(f->vertice[2]->position.Getx(), f->vertice[2]->position.Gety(), f->vertice[2]->position.Getz());
		p->make_triangle(p1, p2, p3);
		++f;
	}
	return p;
}