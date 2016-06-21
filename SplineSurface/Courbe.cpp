#include "Courbe.h"


Courbe::Courbe(std::string vertexShader, std::string fragmentShader)
{
	m_shader = Shader(vertexShader, fragmentShader, "", "");
	m_shader.charger();
	m_sizeOfList = 0;
	m_sizeOfCourbe = 0;
}

Courbe::Courbe(float timeMs)
{
	timeToProceed = timeMs;
	m_sizeOfList = 0;
	m_sizeOfCourbe = 0;
}

Courbe::~Courbe()
{
}

void Courbe::genericSetVBO(std::vector<float>& coord,ListPts listOfPts, int& size)
{
	if (!coord.empty())
	{
		coord.clear();
	}
	for (auto it = listOfPts.begin(); it != listOfPts.end(); ++it)
	{
		coord.push_back((*it).Getx());
		coord.push_back((*it).Gety());
		coord.push_back((*it).Getz());
	}
	size = listOfPts.size() * 3 * sizeof(float);
}

void Courbe::setVBO()
{
	genericSetVBO(m_toVBO, list, m_sizeOfList);
}

void Courbe::setVBOCourbe()
{
	genericSetVBO(m_toVBOCourbe, m_pointInCourbe, m_sizeOfCourbe);
}
