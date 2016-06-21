#include "Bezier.h"


Bezier::Bezier(std::string vertexShader, std::string fragmentShader) :Courbe(vertexShader, fragmentShader)
{
	step = 15.0f;
	ratio = 0.1f;
}

Bezier::Bezier(float timeMs) :Courbe(timeMs)
{
	step = 300;
	ratio = 0.1f;
}


Bezier::~Bezier()
{
}

void Bezier::computePointInCourbe()
{
	if (!m_pointInCourbe.empty())
	{
		m_pointInCourbe.clear();
	}
	if (list.size() < 2)
	{
		setVBO();
		load();
		return;
	}
		
	Point A = list[0];
	m_pointInCourbe.push_back(A);
	for (int k = 1; k < step; k++)
	{
	Point bari = CastelJau(k / step);
	m_pointInCourbe.push_back(bari);
	A = bari;
	}
	m_pointInCourbe.push_back(list.back());
	deltaTime = m_pointInCourbe.size() / timeToProceed;
	setVBO();
	setVBOCourbe();
	load();
}

void Bezier::load()
{
	// Destruction d'un éventuel ancien VBO
	if (glIsBuffer(VBO) == GL_TRUE)
		glDeleteBuffers(1, &VBO);

	glGenBuffers(1, &VBO);

	// Verrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocation de la mémoire vidéo
	glBufferData(GL_ARRAY_BUFFER, m_sizeOfList + m_sizeOfCourbe, 0, GL_STATIC_DRAW);

	// Transfert des données
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_sizeOfList, m_toVBO.data());
	glBufferSubData(GL_ARRAY_BUFFER, m_sizeOfList, m_sizeOfCourbe, m_toVBOCourbe.data());

	// Déverrouillage de l'objet
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	// Destruction d'un éventuel ancien VAO
	if (glIsVertexArray(VAO) == GL_TRUE)
		glDeleteVertexArrays(1, &VAO);

	// Génération de l'identifiant du VAO
	glGenVertexArrays(1, &VAO);

	// Verrouillage du VAO
	glBindVertexArray(VAO);

	// Verrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Accès aux vertices dans la mémoire vidéo
	GLint positionLocation = glGetAttribLocation(m_shader.getProgramID(), "a_position");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, m_sizeOfList, 0);
	//glEnableVertexAttribArray(positionLocation);

	// Déverrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);
}

void Bezier::drawCourbe(Mat4x4 projection, Mat4x4 modelView)
{
	// Activation du shader
	glUseProgram(m_shader.getProgramID());

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_modelView"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	glDrawArrays(GL_POINTS, 0, m_toVBO.size() / 3);
	glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}

void Bezier::drawCourbeSurface(Mat4x4 projection, Mat4x4 modelView, bool points, bool poly)
{
	// Activation du shader
	glUseProgram(m_shader.getProgramID());

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_modelView"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	if (points)
		glDrawArrays(GL_POINTS, 0, m_toVBO.size() / 3);
	if (poly)
		glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}

void Bezier::drawCourbe2(Mat4x4 projection, Mat4x4 modelView, GLuint program)
{
	// Activation du shader
	glUseProgram(program);

	// Verrouillage du VAO
	glBindVertexArray(VAO);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "u_modelView"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	glDrawArrays(GL_POINTS, 0, m_toVBO.size() / 3);
	glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}



Point Bezier::CastelJau(double t)
{
	int n = list.size();
	std::vector<ListPts> point;
	point.reserve(n);
	for (int i = 0; i < n; i++)
	{
		ListPts l;
		point.push_back(l);
	}
	for (int i = 0; i < n; i++)
	{
		point[0].push_back(list[i]);
	}
	for (int j = 1; j < n; j++)
	{
		for (int i = 0; i < n - j; i++)
		{
			Point p = Point(
				(1 - t) * point[j - 1][i].Getx() + t * point[j - 1][i + 1].Getx(),
				(1 - t) * point[j - 1][i].Gety() + t * point[j - 1][i + 1].Gety(),
				(1 - t) * point[j - 1][i].Getz() + t * point[j - 1][i + 1].Getz());
			point[j].push_back(p);
		}
	}
	return point[n - 1][0];
}
