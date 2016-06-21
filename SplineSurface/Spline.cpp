#include "Spline.h"
#include <numeric>


Spline::Spline(std::string vertexShader, std::string fragmentShader) :Courbe(vertexShader, fragmentShader)
{
	step = 10;
	p = 3;
}

Spline::Spline(float timeMS) :Courbe(timeMS)
{
	step = 100;
	p = 3;
}

Spline::~Spline()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);
}

void Spline::calculateT()
{
	m_T.clear();
	n = list.size() + p + 1;
	for (int i = 0; i < n; ++i)
	{
		m_T.push_back(i);
	}
}

void Spline::computePointInCourbe()
{
	if (!m_pointInCourbe.empty())
	{
		m_pointInCourbe.clear();
	}
	calculateT();
	for (int t = m_T[p]; t < m_T[n - p - 1]; ++t)
	{
		for (int k = 0; k < step; ++k)
		{
			float x = 0.f, y = 0.f, z = 0.f;
			Point tmp = Point();
			for (int i = 0; i < list.size(); i++)
			{
				float fac = DeBoor(p, i, t + (k / step));
				x += fac * list[i].Getx();
				y += fac * list[i].Gety();
				z += fac * list[i].Getz();
			}
			tmp.x = x;
			tmp.y = y;
			tmp.z = z;
			m_pointInCourbe.push_back(tmp);
		}
	}
	m_pointInCourbe.push_back(list.back());
	deltaTime = m_pointInCourbe.size() / timeToProceed;
	setVBO();
	setVBOCourbe();
	load();
}

//non recursive version too much time spend on this shit
/*float Spline::DeBoor(int p, int i, float t)
{
	std::vector<std::vector<float>> point;
	point.reserve(p + 1);

	for (int i = 0; i < p + 1; i++)
	{
		std::vector<float> l;
		point.push_back(l);
	}
	point[0].push_back(1.f);
	point[1].push_back(alpha1(t, i, p));
	point[1].push_back(alpha2(t, i, p));
	for (int q = 1; q < p; q++)
	{
		if (q == p - 1)
		{
			for (int k = 0; k < point[q].size()/2; k++)
			{

				for (int j = 0; j <= 1; j++)
				{
					if (t >= m_T[i+ k + j] && t < m_T[ i+ k + j + 1])
						point[q + 1].push_back(1.f);
					else
						point[q + 1].push_back(0.f);
				}

				//for (int j = 0; j <= 1; j++)
				//{
				//	if (t >= m_T[i+ k + j] && t < m_T[ i+ k + j + 1])
				//		point[q + 1].push_back(1.f);
				//	else
				//		point[q + 1].push_back(0.f);
				//}
			}
		}
		else
		{
			for (int k = 0; k < point[q].size(); k++)
			{
				float a1 = alpha1(t, i + k, p - q);
				float a2 = alpha2(t, i + k, p - q);
				float a3 = alpha2(3.f, 0, 2);
				point[q + 1].push_back(alpha1(t, i+k , p - q));
				point[q + 1].push_back(alpha2(t, i+k , p - q));
			}
		}
	}
	for(int k = 0; point[p-1].size(); k++)
	{
		point[p-1][k] *= point[p][k];
	}

	for (int m = p - 2; m > 0; m--)
	{
		for (int k = 0; k < point[m].size(); k+=2)
		{
			point[m][k/2] *= point[m + 1][k] + point[m + 1][k + 1];
		}
	}
	point[0].back() *= point[1][0] + point[1][1];
	return point[0].back();
}*/

float Spline::DeBoor(int p, int i, float t)
{
	if (p == 0)
	{
		if (t >= m_T[i] && t < m_T[i + 1])
			return 1.f;
		else
			return 0.f;
	}

	return alpha1(t, i, p) * DeBoor(p - 1, i, t)
		+ alpha2(t, i, p) * DeBoor(p - 1, i + 1, t);

	//return (((t - m_T[i]) / (m_T[i + p] - m_T[i])) * DeBoor(p - 1, i, t))
	//	+ (((m_T[i + 1 + p] - t) / (m_T[i + 1 + p] - m_T[i + 1])) * DeBoor(p - 1, i + 1, t));

}

void Spline::load()
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

void Spline::drawCourbe2(Mat4x4 projection, Mat4x4 modelView, GLuint program)
{
	// Activation du shader
	glUseProgram(program);

	// Verrouillage du VAO
	glBindVertexArray(VAO);

	// Envoi des matrices
	glUniformMatrix4fv(glGetUniformLocation(program, "u_projection"), 1, GL_FALSE, projection.getMatrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "u_modelView"), 1, GL_FALSE, modelView.getMatrix());

	// Rendu
	glDrawArrays(GL_POINTS, 0, m_toVBO.size()/3);
	//glDrawArrays(GL_LINE_STRIP, 0, m_toVBO.size() / 3);
	//glDrawArrays(GL_LINE_STRIP, m_toVBO.size() / 3, m_toVBOCourbe.size() / 3);

	// Déverrouillage du VAO
	glBindVertexArray(0);

	// Désactivation du shader
	glUseProgram(0);
}

void Spline::render(GLuint& program, GLuint shadowText)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());
	/*
	if (isDiffuseMap)
	{
		//UpdateMaterial(program);
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
	*/

	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}