#include "SurfaceSimpleExtrude.h"


SurfaceSimpleExtrude::SurfaceSimpleExtrude(std::string vertexShader, std::string fragmentShader) :Surface(vertexShader, fragmentShader)
{
	p = Point(0.f,0.f,-4.f);
	step = 20;
	ratio = 0.1f;
}


SurfaceSimpleExtrude::~SurfaceSimpleExtrude()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_triangleEBO);
	glDeleteVertexArrays(1, &m_vaoID);
}

void SurfaceSimpleExtrude::SimpleExtrude()
{
	if (m_listCourbes.empty() || m_listCourbes[0].size() < 2)
		return;

	ListPts begin = m_listCourbes[0];
	int beginSize = begin.size();
	//this will be our translation vector
	Point vector = Point();

	//this will be the center of the spline
	Point base = Point();

	//only if nb pts is not odd
	Point pMid = (begin[beginSize / 2 + 1] - begin[beginSize / 2]) / 2;

	//Get the center of the spline
	///////////////////////////////////////////////////////////////////////////////
	if (beginSize % 2 == 0)
		base = begin[beginSize / 2] + pMid;
	else
		base = begin[beginSize / 2 + 1];
	///////////////////////////////////////////////////////////////////////////////

	//Get translation vector
	vector = p - base;

	//We initialise the last spline with empty points
	ListPts end;
	for (int i = 0; i < beginSize; i++)
		end.push_back(Point());

	//We set last Points of spline according to ratio and begin at the center
	//if nb of point is odd we take the middle point
	//if not, we take the base point (center of spline) and we add or retrive
	//pMid to get 2 points
	///////////////////////////////////////////////////////////////////////////////
	int odd = 0;
	if (end.size() % 2 == 0)
	{
		end[beginSize / 2] = base + vector + (pMid*ratio);
		end[beginSize / 2 - 1] = base + vector - (pMid*ratio);
		odd = 1;
	}
	else
		end[beginSize / 2] = base + vector;

	for (int i = beginSize / 2 + 1; i < beginSize; i++)
		end[i] = end[i - 1] + ((begin[i] - begin[i - 1])*ratio);

	for (int i = beginSize / 2 - (1+odd); i >= 0; i--)
		end[i] = end[i + 1] + ((begin[i] - begin[i + 1])*ratio);
	///////////////////////////////////////////////////////////////////////////////


	//we add step list in ou list of ListPts according to:
	// (1-t)begin + t end
	//begin is m_listCourbes[0]
	//end is end
	///////////////////////////////////////////////////////////////////////////////
	for (int k = 0; k < step; k++)
	{
		ListPts L;
		for (auto i = 0; i < end.size(); i++)
		{
			vector = end[i] - begin[i];
			Point tmp = begin[i]*(1 - (1.f*k / step)) + (end[i] * (1.f*k / step) + (vector / step));
			L.push_back(tmp);
		}
		m_listCourbes.push_back(L);
	}
	//we add our end list as the final points
	m_listCourbes.push_back(end);
	///////////////////////////////////////////////////////////////////////////////

	setVBOCourbe();
	setEBO();
	load();
}

void SurfaceSimpleExtrude::ExtrudeFromAxis()
{
	if (m_listCourbes.empty() || m_listCourbes[0].size() < 2)
		return;

	ListPts begin = m_listCourbes[0];
	int beginSize = begin.size();

	
	float stepAngle = 2 * M_PI / step;
	float actual = 0;
	for (int k = 0; k < step+1; k++)
	{
		actual += stepAngle;
		ListPts L;
		for (auto i = 0; i < beginSize; i++)
		{
			float xabs = std::sqrtf(begin[i].Getx()*begin[i].Getx());
			float yabs = std::sqrtf(begin[i].Gety()*begin[i].Gety());
			float zabs = std::sqrtf(begin[i].Getz()*begin[i].Getz());


			float x = xabs * std::cosf(actual);
			float y = begin[i].Gety();
			float z = xabs * std::sinf(actual);
			Point tmp(x, y, z);
			L.push_back(tmp);
		}
		m_listCourbes.push_back(L);
	}
	setVBOCourbe();
	setEBO();
	load();
}

void SurfaceSimpleExtrude::ExtrudeFromSoul(std::shared_ptr<Courbe> soul)
{
	/*if (m_listCourbes.empty() || m_listCourbes[0].size() < 2)
		return;

	ListPts form = m_listCourbes[0];
	int formSize = form.size();
	Point vector = (*soul)[1] - (*soul)[0];
	for(int j = 1; j < soul->size()-1; j++)
	{
		ListPts L;
		for (auto i = 0; i < formSize; i++)
		{
			Point tmp;
			L.push_back(tmp);
		}
		m_listCourbes.push_back(L);
		vector = (*soul)[j+1] - (*soul)[j-1];
	}
	*/
}

void SurfaceSimpleExtrude::computePointInCourbe()
{
	SimpleExtrude();
	//ExtrudeFromAxis();
}

void SurfaceSimpleExtrude::load()
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

void SurfaceSimpleExtrude::drawCourbe(Mat4x4 projection, Mat4x4 modelView)
{
	// Activation du shader
	glUseProgram(m_shader.getProgramID());

	// Verrouillage du VAO
	glBindVertexArray(m_vaoID);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramID(), "u_modelView"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	//glDrawArrays(GL_POINTS, 0, m_toVBOCourbe.size() / 3);
	//glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	//glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, m_indicesTriangle.size() , GL_UNSIGNED_SHORT, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}
