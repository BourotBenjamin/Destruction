#ifndef MY_GL_WIDGET
#define MY_GL_WIDGET
#include "Spline.h"
#include "Bezier.h"
#include "Surface.h"
#include "SurfaceFromSpline.h"
#include "SurfaceSimpleExtrude.h"
#include "SurfaceBezier.h"
#include "PatchCoons.h"
#include "Objet.h"
#include <QtOpenGL\qglWidget>
#include <QtCore\qtimer.h>
#include "Mat4x4.h"
#include "Camera.h"
#include "Quaternion.h"
#include "EsgiTimer.h"
#include "EsgiShader.h"
#include "Destructor.h"
#include "PhysicsEngine.h"

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class MyGLWidget : public QGLWidget
{
	//Q_OBJECT
protected:


	PhysXEng mpEngine;

	std::shared_ptr<Objet> cube;
	Destructor destructor;
	std::vector<std::shared_ptr<Objet>> destruction;

	std::vector<std::shared_ptr<Courbe>> listCourbes;
	std::vector<std::shared_ptr<Surface>> listSurface;

	std::shared_ptr<Courbe> path;
	std::shared_ptr<Courbe> path2;
	std::shared_ptr<Courbe> path3;
	std::shared_ptr<Courbe> path4;
	std::shared_ptr<PatchCoons> coons;
	std::vector<std::shared_ptr<Courbe>> tmpCourbe;
	std::vector<std::shared_ptr<PatchCoons>> vecCoons;


	std::shared_ptr<Spline> face;
	std::shared_ptr<Spline> edge;
	std::shared_ptr<Spline> vertex;


	int indexInSpline;
	bool down;

	bool persp = false;
	bool topView = false;
	bool subdiv = true;

	bool wireframe = false;
	bool canMove = false;

	int ortho;
	
	EsgiTimer timer;
	bool canDraw = true;

	std::unique_ptr< Shader> shad;
	std::unique_ptr< Shader> shadEdge;
	std::unique_ptr< Shader> shadFace;
	std::unique_ptr< Shader> skyboxShader;
	std::unique_ptr< Shader> normalMap;
	std::unique_ptr< Shader> shadowDepth;
	std::unique_ptr< Shader> debugDepth;
	std::unique_ptr< Shader> lampshad;
	std::unique_ptr< Shader> simpleShaderWithNormals;
	std::unique_ptr< Shader> billboard;
	std::unique_ptr< EsgiShader> updateParticules;

	Mat4x4 projection;
	Mat4x4 modelView;
	Mat4x4 lightProjection;
	Mat4x4 lightView;
	Mat4x4 lightCoord;

	float anglex;
	float angley;

	float xRel;
	float yRel;

	int buffKey[50];

	GLuint UBO;
	GLuint lightUBO;
	GLuint shadowMapFBO;
	GLuint shadowMapTex;

	float m_z;
	Point m_relativeMouse;
	Point m_oldMousePos;

	Point lightPos;

	std::unique_ptr< Objet > rock;
	std::unique_ptr< Objet > cat;
	std::unique_ptr< Objet > cyborg;

	std::unique_ptr< Objet > catnoN;
	std::unique_ptr< Objet > cyborgnoN;

	std::unique_ptr< Objet > floor;

	std::unique_ptr< Objet > wall1;
	std::unique_ptr< Objet > wall2;
	std::unique_ptr< Objet > wall3;
	std::unique_ptr< Objet > wall4;

	std::unique_ptr< Objet > skybox;

	std::unique_ptr< Objet > lamp;

	std::unique_ptr< Objet > particuleEmitter;

	Camera cam;


	bool rightClick;
	void RenderQuad();
	void renderScene(GLuint& program, GLuint shadow = 0);

	void initializeGL();
	void paintGL();
	void mousePressEvent(QMouseEvent * e);
	void mouseMoveEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *e);
	Objet* pyramid;
	Polyhedron_3* pyramidPoly;
	

public:
	~MyGLWidget()
	{
		if (shadowMapTex)
			glDeleteTextures(1, &shadowMapTex);
		if (shadowMapFBO)
			glDeleteFramebuffers(1, &shadowMapFBO);
		if (UBO)
			glDeleteBuffers(1, &UBO);
		if (lightUBO)
			glDeleteBuffers(1, &lightUBO);
		mpEngine.cleanupPhysics(true);
	}
	void eventWrapper(QEvent * e){ event(e); }
	void updateWidget(float deltaTime);
	void MyGLWidget::createPyramid();

//private slots:
//void createBezier() { listCourbes.push_back(std::shared_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs"))); repaint(); }
//void createSpline() { listCourbes.push_back(std::shared_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"))); repaint(); }
//void createsBezier(){ listSurface.push_back(std::shared_ptr<SurfaceBezier>(new SurfaceBezier("courbeVertex.vs", "courbeFragment.fs"))); listSurface.back()->computePointInCourbe(); repaint(); }
//void createSSpline(){ listSurface.push_back(std::shared_ptr<SurfaceFromSpline>(new SurfaceFromSpline("courbeVertex.vs", "courbeFragment.fs"))); repaint(); }
//void createSSESpline(){ listSurface.push_back(std::shared_ptr<SurfaceSimpleExtrude>(new SurfaceSimpleExtrude("courbeVertex.vs", "courbeFragment.fs"))); repaint(); }
//void clearSceneGL() { listCourbes.clear(); listSurface.clear(); repaint(); }
//void goLeft() {
//	anglex += 5.73;
//	cam.deplacer(2, 5.73f, 0);
//	cam.lookAt(modelView);
//	repaint();
//}
//void goRight(){
//	anglex -= 5.73;
//	cam.deplacer(3, -5.73f, 0);
//	cam.lookAt(modelView);
//	repaint();
//}
//void goUp()   {
//	angley -= 5.73;
//	cam.deplacer(0, 0, -5.73f);
//	cam.lookAt(modelView);
//	repaint();
//}
//void goDown() {
//	angley += 5.73;
//	cam.deplacer(1, 0, +5.73f);
//	cam.lookAt(modelView);
//	repaint();
//}

};

#endif