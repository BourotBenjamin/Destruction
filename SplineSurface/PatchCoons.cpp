#include "PatchCoons.h"

PatchCoons::PatchCoons(std::string vertexShader, std::string fragmentShader)// :Surface(vertexShader, fragmentShader)
{
	m_sizeOfList = 0;
	m_sizeOfCourbe = 0;
	step = 10;
	ratio = 1.0f;
}

void PatchCoons::computePointInCourbe()
{

	//L_c(s, t) = (1 - t) c_0(s) + t c_1(s) \,
	//	and between d0, d1

	//	L_d(s, t) = (1 - s) d_0(t) + s d_1(t) \,
	//	producing two ruled surfaces defined on the unit square.

	//	The bilinear interpolation on the four corner points is another surface

	//	B(s, t) = c_0(0) (1 - s)(1 - t) + c_0(1) s(1 - t) + c_1(0) (1 - s)t + c_1(1) s t. \,
	//	A bilinearly blended Coons patch is the surface

	//	C(s, t) = L_c(s, t) + L_d(s, t) - B(s, t). \,
	 

	if (!m_listCourbes.empty())
		m_listCourbes.clear();

	int smax = control[0].size();
	int tmax = control[1].size();

	ListPts& c0 = control[0];
	ListPts& d0 = control[1];
	ListPts& c1 = control[2];
	ListPts& d1 = control[3];

	for (int s = 0; s < smax; s++)
	{
		ListPts res;
		float ss = (s*1.0 / smax);
		for (int t = 0; t < tmax; t++)
		{
			float tt = (t*1.0 / tmax);
			Point c = c0[s] * (1.f - tt) + c1[s] * tt;
			Point d = d0[t] * (1.f - ss) + d1[t] * ss;

			Point b = c0.front() * (1.f - ss) *(1.f - tt) + c0.back() * ss * (1 - tt)  + c1.front() * (1 - ss) * tt + c1.back() * ss * tt;

			res.push_back(c+d-b);
		}
		m_listCourbes.push_back(res);
	}
	setVBOCourbe();
	setEBO();
	load();
}

void PatchCoons::load()
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, m_sizeOfList, 0);
	//glEnableVertexAttribArray(positionLocation);

	// Déverrouillage du VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);
}

void PatchCoons::drawCourbe(Mat4x4 projection, Mat4x4 modelView, GLuint program)
{
	// Activation du shader
	glUseProgram(program);

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, modelView.getMatrix());

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