#include "SurfaceBezier.h"


SurfaceBezier::SurfaceBezier(std::string vertexShader, std::string fragmentShader) :Surface(vertexShader, fragmentShader)
{
	step = 6;
	ratio = 0.1f;

	for(int i = 0; i < step; i++)
	{
		ListPts L;
		for (int j = 0; j < step; j++)
		{
			L.push_back(Point(i-(step/2.f), std::rand()%3, j-(step/2.f)));
		}
		listOfControl.push_back(L);
	}
}


SurfaceBezier::~SurfaceBezier()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_triangleEBO);
	glDeleteVertexArrays(1, &m_vaoID);
}

Point SurfaceBezier::CastelJau(ListPts entryList, double t)
{
	int n = entryList.size();
	std::vector<ListPts> point;
	point.reserve(n);
	for (int i = 0; i < n; i++)
	{
		ListPts l;
		point.push_back(l);
	}
	for (int i = 0; i < n; i++)
	{
		point[0].push_back(entryList[i]);
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

Bezier SurfaceBezier::computeBezier(ListPts l)
{
	Bezier ret("courbeVertex.vs", "courbeFragment.fs");
	if (l.size() < 2)
		return ret;

	for (auto it = l.begin(); it != l.end(); ++it)
		ret.pushPoint((*it));
	ret.computePointInCourbe();
	return ret;
}

void SurfaceBezier::SurfaceCastelJau()
{
	listOfBezier.clear();
	listOfVertical.clear();
	std::vector<ListPts> vlp;
	for(int i  = 0; i < listOfControl[0].size(); i++)
	{
		ListPts l;
		for (int j = 0; j < listOfControl.size(); j++)
			l.push_back(listOfControl[j][i]);

		vlp.push_back(l);
	}
	for(int i = 0; i < listOfControl[0].size(); i++)
		listOfBezier.push_back(computeBezier(vlp[i]));

	for (int i = 0; i < listOfBezier[0].getPointInCourbe().size(); i++)
	{
		ListPts curve;
		for(int j  = 0; j < listOfBezier.size(); j++)
		{
			curve.push_back(listOfBezier[j].getPtsInCourbeAt(i));
		}
		listOfVertical.push_back(computeBezier(curve));
	}
}

void SurfaceBezier::computePointInCourbe()
{
	SurfaceCastelJau();
	for (auto it = listOfVertical.begin(); it != listOfVertical.end(); ++it)
	{
		m_listCourbes.push_back((*it).getPointInCourbe());
	}
	setVBOCourbe();
	setEBO();
	load();
			
}

void SurfaceBezier::load()
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

void SurfaceBezier::drawCourbe(Mat4x4 projection, Mat4x4 modelView)
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
	glDrawElements(GL_TRIANGLES, m_indicesTriangle.size(), GL_UNSIGNED_SHORT, 0);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}

/*void SurfaceBezier::load()
{
	
}*/

/*void SurfaceBezier::drawCourbe(Mat4x4 projection, Mat4x4 modelView)
{
	for (auto it = listOfBezier.begin(); it != listOfBezier.end(); ++it)
		(*it).drawCourbeSurface(projection, modelView, false,false);
	for (auto it = listOfVertical.begin(); it != listOfVertical.end(); ++it)
		(*it).drawCourbeSurface(projection, modelView, false, false);
}*/
