#include "Surface.h"

Surface::Surface(std::string vertexShader, std::string fragmentShader)
{
	m_shader = Shader(vertexShader, fragmentShader, "", "");
	m_shader.charger();
	m_sizeOfList = 0;
	m_sizeOfCourbe = 0;
	step = 20;
	ratio = 1.0f;
}

Surface::Surface()
{
	//m_shader = Shader(vertexShader, fragmentShader, "", "");
	//m_shader.charger();
	m_sizeOfList = 0;
	m_sizeOfCourbe = 0;
	step = 20;
	ratio = 1.0f;
}

Surface::~Surface()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_triangleEBO);
	glDeleteVertexArrays(1, &m_vaoID);
}

void Surface::genericSetVBO(std::vector<float>& coord, std::vector<ListPts> listOfPts, int& size)
{
	if (!coord.empty())
	{
		coord.clear();
	}
	for (auto it2 = listOfPts.begin(); it2 != listOfPts.end(); ++it2)
	{
		for (auto it = (*it2).begin(); it != (*it2).end(); ++it)
		{
			coord.push_back((*it).Getx());
			coord.push_back((*it).Gety());
			coord.push_back((*it).Getz());
		}
	}
	size = coord.size() * sizeof(float);
}

void Surface::setVBO()
{
	//genericSetVBO(m_toVBO, list, m_sizeOfList);
}

void Surface::setVBOCourbe()
{
	genericSetVBO(m_toVBOCourbe, m_listCourbes, m_sizeOfCourbe);
}

void Surface::setEBO()
{
	if (!m_indicesTriangle.empty())
		m_indicesTriangle.clear();
	GLushort m = m_listCourbes.size() - 1;
	GLushort p = m_listCourbes[0].size() - 1;
	for (GLushort j = 0; j < m; j++)
	{
		for (GLushort i = 0; i < p; i++)
		{
			m_indicesTriangle.push_back(j*(p + 1) + i);
			m_indicesTriangle.push_back((j + 1)*(p + 1) + i);
			m_indicesTriangle.push_back((j + 1)*(p + 1) + i + 1);

			m_indicesTriangle.push_back(j * (p + 1) + i);
			m_indicesTriangle.push_back((j + 1)*(p + 1) + i + 1);
			m_indicesTriangle.push_back(j * (p + 1) + i + 1);

		}
	}
	m_sizeIndiceTriangle = m_indicesTriangle.size() * sizeof(GLushort);
}

void Surface::computePointInCourbe()
{
	if (!m_listCourbes.empty())
		m_listCourbes.clear();

	for (int j = 0; j < step; j++)
	{
		ListPts L;
		for (int i = 0; i < step; i++)
		{
			L.push_back(Point(i-(step/2), 0, j - (step/2)));
		}
		m_listCourbes.push_back(L);
	}
	setVBOCourbe();
	setEBO();
	load();
}

void Surface::load()
{
	// Destruction d'un éventuel ancien VAO
	if (glIsVertexArray(m_vaoID) == GL_TRUE)
		glDeleteVertexArrays(1, &m_vaoID);
	// Destruction d'un éventuel ancien VBO
	if (glIsBuffer(m_vboID) == GL_TRUE)
		glDeleteBuffers(1, &m_vboID);
	if (glIsBuffer(m_triangleEBO) == GL_TRUE)
		glDeleteBuffers(1, &m_triangleEBO);


	// Génération de l'identifiant des VAO, VBO et EBO
	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(1, &m_vboID);
	glGenBuffers(1, &m_triangleEBO);

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Verrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

	// Allocation de la mémoire vidéo
	glBufferData(GL_ARRAY_BUFFER, m_sizeOfCourbe, 0, GL_STATIC_DRAW);

	// Transfert des données
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_sizeOfCourbe, m_toVBOCourbe.data());

	// Déverrouillage de l'objet
	//glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_triangleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sizeIndiceTriangle, m_indicesTriangle.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Accès aux vertices dans la mémoire vidéo
	GLint positionLocation = glGetAttribLocation(m_shader.getProgramID(), "a_position");
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionLocation);

	//glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, m_sizeOfList, 0);
	//glEnableVertexAttribArray(positionLocation);

	// Déverrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);
}

void Surface::drawCourbe(Mat4x4 projection, Mat4x4 modelView)
{
	// Activation du shader
	glUseProgram(m_shader.getProgramID());

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "modelview"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	//glDrawArrays(GL_POINTS, 0, m_toVBOCourbe.size() / 3);
	//glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	//glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_indicesTriangle.size(), GL_UNSIGNED_SHORT, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}

