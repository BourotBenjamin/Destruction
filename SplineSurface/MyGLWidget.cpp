#define GLEW_STATIC 1
#include <GL\glew.h>
#include "MyGLWidget.h"
#include <QtGui\qevent.h>

float POSX = 80.0f, POSY = -80.0f, POSZ = -80.0f;
//float POSX = -90.0f, POSY = -90.0f, POSZ = -90.0f;
float MAX_DIST = 100.0;
int NB_POINTS = 100;


void MyGLWidget::initializeGL()
{
	GLenum err = glewInit();

	mpEngine.initPhysics(true);
	destruction.reserve(365);
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	setMinimumSize(800, 800);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	/*glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);*/
	cube = std::shared_ptr<Objet>(new Objet());

	setMouseTracking(true);
	projection.Perspective(45.f, width(), height(), 0.1f, 1000.f);
	cam = Camera(Point(0, 0, -7.0f), Point(0,0,0), Point(0, 1, 0));

	for (int i = 0; i < 50; ++i)
		buffKey[1] = 0;

	xRel = 0;
	yRel = 0;

	shad = std::unique_ptr<Shader>(new Shader("courbeVertex.vs", "courbeFragment.fs", "", ""));
	shadEdge = std::unique_ptr<Shader>(new Shader("courbeVertexEdge.vs", "courbeFragment.fs", "", ""));
	shadFace = std::unique_ptr<Shader>(new Shader("courbeVertexFace.vs", "courbeFragment.fs", "", ""));
	skyboxShader = std::unique_ptr<Shader>(new Shader("skybox.vs", "skybox.fs", "", ""));
	normalMap = std::unique_ptr<Shader>(new Shader("normalMap.vs", "normalMap.fs", "", ""));
	shadowDepth = std::unique_ptr<Shader>(new Shader("depth.vs", "depth.fs", "", ""));
	lampshad = std::unique_ptr<Shader>(new Shader("lamp.vs", "lamp.fs", "", ""));
	updateParticules = std::unique_ptr<EsgiShader>(new EsgiShader());//"ps_update.vs", "ps_update.fs", "ps_update.gs", ""));
	billboard = std::unique_ptr<Shader>(new Shader("billboard.vs", "billboard.fs", "billboard.gs", ""));
	simpleShaderWithNormals = std::unique_ptr<Shader>(new Shader("normals.vs", "normals.fs", "", ""));

	rightClick = false;
	modelView.identity();

	cam.setPosition(Point(0, 20.f, -10));
	cam.setPointcible(Point());
	cam.lookAt(modelView);

	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 2 + sizeof(float)* 3, nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 3 + sizeof(float)* 3, nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUBO);

	auto program = shad->getProgramID();
	glUseProgram(program);
	auto blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);

	program = normalMap->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = skyboxShader->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);

	program = shadowDepth->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = lampshad->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = billboard->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);

	// Configure depth map FBO
    glGenFramebuffers(1, &shadowMapFBO);
    // - Create depth texture
    glGenTextures(1, &shadowMapTex);
    glBindTexture(GL_TEXTURE_2D, shadowMapTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	cat = std::unique_ptr<Objet>(new Objet("cat.obj"));
	cyborg = std::unique_ptr<Objet>(new Objet("cyborg.obj"));

	floor = std::unique_ptr<Objet>(new Objet("cube.obj"));

	wall1 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall2 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall3 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall4 = std::unique_ptr<Objet>(new Objet("wall.obj"));

	float wallY = 1;
	float floorDepth = 2.f;

	wall1->position = Point(15, wallY, 0);
	wall1->scale = Point(floorDepth, 15.f, 30.f);

	wall2->position = Point(-300, wallY, 0);
	wall2->scale = Point(30.f, 30, 30);

	wall3->position = Point(0, 0, 0);
	wall3->scale = Point(30.f, 30, 30);

	wall4->position = Point(0, wallY, -15);
	wall4->scale = Point(30.f, 15.f, floorDepth);

	floor->setPost(Point(-100.f, -floorDepth/2-0.15, 0.f));
	floor->setScale(Point(30.f, 30, 30.f));
	cat->setPost(Point(-2.5f, 0, 0));
	cat->setScale(Point(4.f, 4.f, 4.f));
	cyborg->setPost(Point(0, 0, 3));



	std::vector<std::string> v = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/back.jpg",
		"skybox/front.jpg",
	};
	 
	skybox = std::unique_ptr<Objet>(new Objet(v));

	lamp = std::unique_ptr<Objet>(new Objet("lamp.obj"));

	lightCoord = Mat4x4(0);
	lightCoord.identity();
	lightCoord *= Mat4x4::translate(0.5f, 0.5f, 0.5f);
	lightCoord *= Mat4x4::scale(0.5f, 0.5f, 0.5f);

	lightPos = Point(0.f, 15.f, -25.f);
	lamp->position = lightPos;

	/*path = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	path->pushPoint(Point(0, 0, 0));
	path->pushPoint(Point(0.f, -2.0f, 3.f));
	path->pushPoint(Point(0.f, 1.0f, 6.f));
	path->pushPoint(Point(0, 2.0f, 10));
	path->pushPoint(Point(0, 4, 14));

	path2 = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	path2->pushPoint(Point(0, 0, 0));
	path2->pushPoint(Point(3, 2.0f, 0));
	path2->pushPoint(Point(5.f, 1.0f, 0.f));
	path2->pushPoint(Point(7.f, -2.0f, 0.f));
	path2->pushPoint(Point(9, 4, 0));

	path3 = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	path3->pushPoint(Point(9, 4, 0));
	path3->pushPoint(Point(9, 2.0f, -1));
	path3->pushPoint(Point(9.f, 1.0f, -2.f));
	path3->pushPoint(Point(9.f, -2.0f, -3.f));
	path3->pushPoint(Point(9, 2.0f, 14));

	path4 = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	path4->pushPoint(Point(0, 4, 14));
	path4->pushPoint(Point(2, 0, 14));
	path4->pushPoint(Point(6.f, -2.0f, 14));
	path4->pushPoint(Point(7.f, 1.0f, 14));
	path4->pushPoint(Point(9, 2.0f, 14));*/

	path = std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs"));
	path->pushPoint(Point(0, 0, 0));
	path->pushPoint(Point(0.f, -2.0f, 3.f));
	path->pushPoint(Point(0.f, 1.0f, 6.f));
	path->pushPoint(Point(0, 2.0f, 10));
	path->pushPoint(Point(0, 4, 14));

	path2 = std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs"));
	path2->pushPoint(Point(0, 0, 0));
	path2->pushPoint(Point(3, 2.0f, 0));
	path2->pushPoint(Point(5.f, 1.0f, 0.f));
	path2->pushPoint(Point(7.f, -2.0f, 0.f));
	path2->pushPoint(Point(9, 4, 0));

	path3 = std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs"));
	path3->pushPoint(Point(9, 4, 0));
	path3->pushPoint(Point(9, 2.0f, 2));
	path3->pushPoint(Point(9.f, 1.0f, 7.f));
	path3->pushPoint(Point(9.f, -2.0f, 10.f));
	path3->pushPoint(Point(9, 2.0f, 14));

	path4 = std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs"));
	path4->pushPoint(Point(0, 4, 14));
	path4->pushPoint(Point(2, 0, 14));
	path4->pushPoint(Point(6.f, -2.0f, 14));
	path4->pushPoint(Point(7.f, 1.0f, 14));
	path4->pushPoint(Point(9, 2.0f, 14));

	

	
	
	coons = std::shared_ptr<PatchCoons>(new PatchCoons("lol", "lol"));

	face = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	edge = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	vertex = std::unique_ptr<Spline>(new Spline("courbeVertex.vs", "courbeFragment.fs"));
	

	path->computePointInCourbe();
	path2->computePointInCourbe();
	path3->computePointInCourbe();
	path4->computePointInCourbe();

	//vecCoons.push_back(std::shared_ptr<PatchCoons>(new PatchCoons("", "")));

	//vecCoons.back()->addControl(path->getPointInCourbe(), Point());
	//vecCoons.back()->addControl(path2->getPointInCourbe(), Point());// tmpCourbe[0]->getPointInCourbe().front());
	//vecCoons.back()->addControl(path3->getPointInCourbe(), Point());// tmpCourbe[1]->getPointInCourbe().back());
	//vecCoons.back()->addControl(path4->getPointInCourbe(), Point());// tmpCourbe[0]->getPointInCourbe().back());

	//vecCoons.back()->computePointInCourbe();

	coons->addControl(path->getPointInCourbe(), Point());
	coons->addControl(path2->getPointInCourbe(), Point());
	coons->addControl(path3->getPointInCourbe(), Point());
	coons->addControl(path4->getPointInCourbe(), Point());

	coons->computePointInCourbe();

	auto listeSpline = path->getPointInCourbe();
	indexInSpline = 1;
	down = false;


	particuleEmitter = std::unique_ptr<Objet>(new Objet("wall.obj", 100, *updateParticules, "ps_update.vs", "ps_update.fs", "ps_update.gs"));
	ortho = 15;

	timer.Begin();

	floor->setupStruct();
	floor->setFacePoint();
	floor->setEdgePoint();
	floor->setVertexPoint();

	wall3->setupStruct();
	wall3->setFacePoint();
	wall3->setEdgePoint();
	wall3->setVertexPoint();

	wall2->setupStruct();
	wall2->setFacePoint();
	wall2->setEdgePoint();
	wall2->setVertexPoint();
	
	/*for (auto it = lamp->facePoint.begin(); it != lamp->facePoint.end(); ++it)
	{
		face->pushPoint(it->position);
	}

	for (auto it = lamp->vertexPoint.begin(); it != lamp->vertexPoint.end(); ++it)
	{
		vertex->pushPoint(it->position);
	}

	for (auto it = lamp->edgePoint.begin(); it != lamp->edgePoint.end(); ++it)
	{
		edge->pushPoint(it->position);
	}
	face->computePointInCourbe();
	edge->computePointInCourbe();
	vertex->computePointInCourbe();*/
	createPyramid();

}

float mAccumulator = 0.0f;
float mStepSize = 1.0f / 60.0f;

bool  MyGLWidget::advance(PxReal dt)
{
	mAccumulator += dt;
	if (mAccumulator < mStepSize)
		return false;

	mAccumulator -= mStepSize;

	mpEngine.stepPhysics(true);
	return true;
}

void MyGLWidget::updateWidget(float deltaTime)
{

	cam.deplacer(buffKey, xRel, yRel, deltaTime);
	xRel = 0;
	yRel = 0;

	auto listeSpline = path->getPointInCourbe();
	Point p;
	if (!down)
	{
		p = (listeSpline[indexInSpline] - listeSpline[indexInSpline - 1]);
		p.normalize();
		cat->position += p * deltaTime* 5.f;
		++indexInSpline;
	}
	else
	{
		p = (listeSpline[indexInSpline - 1] - listeSpline[indexInSpline]);
		p.normalize();
		cat->position += p *deltaTime * 5.f;
		--indexInSpline;
	}
	auto q = Quaternion::RotationBetweenVectors(cat->lookat, p);
	cat->lookat = p;
	cat->rotation += q.toEulerAngle();
	if (indexInSpline == listeSpline.size() - 1 && down == false)
		down = true;
	else if (indexInSpline == 1 && down == true)
		down = false;
	if (canMove)
	{
		for each (std::shared_ptr<Objet> tetra in destruction)
		{
			tetra->position += Point((((rand() % 200) - 100) * 1.f), (((rand() % 200) - 100) * 1.f), (((rand() % 200) - 100) * 1.f)) * deltaTime;
		}
	}

	auto t = timer.GetElapsedTimeSince(timer.GetStartTime())/2;
	//auto lampMove = Point(std::cos(t), 0, std::sin(t));
	auto lampMove = Point(std::cos(t), 0, 0);
	lampMove.normalize();
	//lightPos += lampMove * deltaTime * 10.f;
	//lamp->position = lightPos;

	//cyborg->rotation = Point(0, std::cosf(t), 0);

	//lamp->setLookAtPoint(lampMove);
	
	//auto program = updateParticules->GetProgram();
	//glUseProgram(program);
	//particuleEmitter->UpdateParticules(program, deltaTime * 1000.f);

	if (tmpCourbe.size() == 4)
	{
		if (tmpCourbe[3]->getLength() == tmpCourbe[1]->getLength())
		{
			vecCoons.push_back(std::shared_ptr<PatchCoons>(new PatchCoons("", "")));
			vecCoons.back()->addControl(tmpCourbe[0]->getPointInCourbe(), Point());
			vecCoons.back()->addControl(tmpCourbe[1]->getPointInCourbe(), Point());// tmpCourbe[0]->getPointInCourbe().front());
			vecCoons.back()->addControl(tmpCourbe[2]->getPointInCourbe(), Point());// tmpCourbe[1]->getPointInCourbe().back());
			vecCoons.back()->addControl(tmpCourbe[3]->getPointInCourbe(), Point());// tmpCourbe[0]->getPointInCourbe().back());


			vecCoons.back()->computePointInCourbe();
			tmpCourbe.clear();
		}
	}
	advance(deltaTime);

	PxScene* scene;
	PxGetPhysics().getScenes(&scene, 1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC, (PxActor**)&actors[0], nbActors);
		//Snippets::renderActors(&actors[0], (PxU32)actors.size(), true);
		//void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows, const PxVec3 & color)
		PxShape* shapes[128];
		for (PxU32 i = 0; i<actors.size()-1; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes, nbShapes);
			bool sleeping = actors[i]->isRigidDynamic() ? actors[i]->isRigidDynamic()->isSleeping() : false;

			for (PxU32 j = 0; j<nbShapes; j++)
			{
				//const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				PxTransform newPose = PxShapeExt::getGlobalPose(*shapes[j], *actors[i]);
				auto t = PxTransform(newPose.p, newPose.q * PxQuat(PxIdentity));
				
				destruction[i]->position.x = t.p.x;
				destruction[i]->position.y = t.p.y;
				destruction[i]->position.z = t.p.z;
				
				Quaternion q;
				q.u_.x = t.q.x;
				q.u_.y = t.q.y;
				q.u_.z = t.q.z;
				q.a_ = t.q.w;

				//destruction[i]->rotation = q.toEulerAngle();

				/*if (destruction.size() > 0)
					destruction[i]->setWorldMatrix((float *)&shapePose);*/
			}
		}
	}

	cam.lookAt(modelView);
}



void MyGLWidget::paintGL()
{	
	
	glViewport(0, 0, width(), height());
	glClearColor(1.0f, 1.0f, 1.0f,  1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (persp)
	{
		projection.Perspective(45.f, width(), height(), 0.1f, 1000.f);
	}
	else
	{
		// Orthographic camera
		ortho = 15.0;
		//modelView.identity();
		projection.Orthographic(-ortho, ortho, ortho, -ortho, 0.1, 10000);
	}
	
	GLfloat near_plane = 0.1f, far_plane = 70.f;
	float spotDim = 50.f;
	lightProjection.Orthographic(-spotDim, spotDim, -spotDim, spotDim, near_plane, far_plane);
	//lightProjection.Perspective(45.0f, (GLfloat)SHADOW_WIDTH, (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
	lightView.lookAt(lightPos,floor->position - lightPos, Point(0.f, 1.f, 0.f));
	auto p = cam.getPos();
	float pos[3];
	pos[0] = p.x;
	pos[1] = p.y;
	pos[2] = p.z;

	float lpos[3];
	lpos[0] = lightPos.x;
	lpos[1] = lightPos.y;
	lpos[2] = lightPos.z;

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, modelView.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16, sizeof(float)* 16, projection.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 2, sizeof(float)* 3, pos);

	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, lightView.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16, sizeof(float)* 16, lightProjection.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16*2, sizeof(float)* 16, lightCoord.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 3, sizeof(float)* 3, lpos);
	
    // - now render scene from light's point of view
	auto program = shadowDepth->getProgramID();
	//glUseProgram(program);
 //   glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
 //   glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
 //       glClear(GL_DEPTH_BUFFER_BIT);
	//	glEnable(GL_CULL_FACE);
	//	glFrontFace(GL_CW);
	//	//particuleEmitter->renderBillBoardParticules(program);
 //       renderScene(program);
	//	glDisable(GL_CULL_FACE);
 //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	// 2. Render scene as normal
    glViewport(0, 0, width(), height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 2.0);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	/*glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	//program = billboard->getProgramID();
	//glUseProgram(program);
	//particuleEmitter->renderBillBoardParticules(program);

	//particuleEmitter->currVB = particuleEmitter->currTFB;
	//particuleEmitter->currTFB = (particuleEmitter->currTFB + 1) & 0x1;
	//std::swap(particuleEmitter->currVB, particuleEmitter->currTFB);
	//particuleEmitter->currSource ^= 1;
	program = simpleShaderWithNormals->getProgramID();
	glUseProgram(program);
	renderScene(program, 0);

	/*
	program = shad->getProgramID();
	vertex->drawCourbe2(projection, modelView, program);
	//path->drawCourbe2(projection, modelView, program);
	/*path2->drawCourbe2(projection, modelView, program);
	path3->drawCourbe2(projection, modelView, program);
	path4->drawCourbe2(projection, modelView, program);*/
	/*
	coons->drawCourbe(projection, modelView, program);

	program = shadFace->getProgramID();
	glUseProgram(program);
	face->drawCourbe2(projection, modelView, program);
	program = shadEdge->getProgramID();
	glUseProgram(program);
	edge->drawCourbe2(projection, modelView, program);
	

	if (persp)
	{
		/*for (auto it = vecCoons.begin(); it != vecCoons.end(); ++it)
			(*it)->drawCourbe(projection, modelView, program);*/
		//cam.lookAt(modelView);
		//std::cout << "PAINT";
		/*if (vecCoons.size())
			vecCoons[0]->drawCourbe(projection, modelView, program);
		if (tmpCourbe.size() != 0)
			tmpCourbe.back()->drawCourbe2(projection, modelView, program);*/
		//for (auto i = 0; i < vecCoons.size(); ++i)
		/*if (vecCoons.size())
			vecCoons[0]->drawCourbe(projection, modelView, program);
	}
	else
	{
		if (tmpCourbe.size() != 0)
			tmpCourbe.back()->drawCourbe2(projection, modelView, program);
		if (vecCoons.size())
			vecCoons[0]->drawCourbe(projection, modelView, program);

		switch (tmpCourbe.size()) {
		case 2:
			cam.setPosition(Point(10, 20.f, 0));
			cam.setPointcible(Point());
			cam.lookAt(modelView);
			break;
		case 3:
			cam.setPosition(Point(0, 20.f, -10));
			cam.setPointcible(Point());
			cam.lookAt(modelView);
			break;
		case 4:
			cam.setPosition(Point(10, 20.f, 0));
			cam.setPointcible(Point());
			cam.lookAt(modelView);
			break;
		}
	}
	*/

	program = lampshad->getProgramID();
	glUseProgram(program);
	lamp->render(program);
	
	//program = skyboxShader->getProgramID();
	//glUseProgram(program);
	//skybox->renderCubeMap(program);


	//timer.End();
	//std::cout << timer.GetElapsedTime() << "\n";
}


void MyGLWidget::RenderQuad()
{
	GLuint quadVAO = 0;
	GLuint quadVBO;
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void MyGLWidget::renderScene(GLuint& program, GLuint shadowTex)
{

	//cat->render(program, shadowTex);
	//cyborg->render(program, shadowTex);

	floor->render2(program, shadowTex, wireframe);
	wall3->render2(program, shadowTex, wireframe);
	wall2->render2(program, shadowTex, wireframe);
	if (pyramid->alive)
		pyramid->render2(program, shadowTex, wireframe);
	for each (auto &piece in destruction)
	{
		if (piece->alive)
			piece->render2(program, shadowTex, wireframe);
	}
}

void MyGLWidget::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == Qt::MouseButton::RightButton)
		rightClick = true;

	if (e->button() == Qt::MouseButton::LeftButton) {
		// && tmpCourbe.size() != 0) {
		/*Point p;
		
		if (tmpCourbe.size() == 1 || tmpCourbe.size() == 3) {
			p.x = (e->x() / (width() / 2.0) - 1)* -ortho;
			p.y = (e->y() / (height() / 2.0) - 1)* -ortho * 2;
		}
		else {
			p.y = (e->y() / (height() / 2.0) - 1)* -ortho * 2;
			p.z = (e->x() / (width() / 2.0) - 1)* -ortho;
		}

		

		switch (tmpCourbe.size()) {
		case 1:
			tmpCourbe.back()->pushPoint(p);
			tmpCourbe.back()->computePointInCourbe();
			break;
		case 2:
			if (tmpCourbe.back()->getLength() == 0)
			{
				//cam.orienter(0, 90);
				// on prend le premier point de la première courbe comme premier point de la deuxième.
				tmpCourbe.back()->pushPoint(tmpCourbe[0]->getPointInCourbe().front());
				tmpCourbe.back()->computePointInCourbe();
			}
			p.x = tmpCourbe.back()->getPointAt(0).x;
			tmpCourbe.back()->pushPoint(/*tmpCourbe.back()->getPointAt(0) +*//* p);
			tmpCourbe.back()->computePointInCourbe();
			break;

		case 3:
			// on prend le dernier point de la deuxième courbe comme premier point de la 3éme.
			if (tmpCourbe.back()->getLength() == 0) 
			{
				tmpCourbe.back()->pushPoint(tmpCourbe[1]->getPointInCourbe().back());
				tmpCourbe.back()->computePointInCourbe();
			}
			p.z = tmpCourbe.back()->getPointAt(0).z;
			tmpCourbe.back()->pushPoint(/*tmpCourbe.back()->getPointAt(0) +*//* p);
			tmpCourbe.back()->computePointInCourbe();
			if (tmpCourbe[0]->getLength() == tmpCourbe.back()->getLength())
			{
				tmpCourbe.push_back(std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs")));
			}
			
			break;

		case 4:

			if (tmpCourbe.back()->getLength() == 0)
			{
				tmpCourbe.back()->pushPoint(tmpCourbe[0]->getPointInCourbe().back());
				tmpCourbe.back()->computePointInCourbe();
			}
			p.x = tmpCourbe.back()->getPointAt(0).x;
			tmpCourbe.back()->pushPoint(/*tmpCourbe.back()->getPointAt(0) +*//* p);
			tmpCourbe.back()->computePointInCourbe();
			if (tmpCourbe[1]->getLength() == tmpCourbe[1]->getLength() - 1)
			{
				tmpCourbe.back()->pushPoint(tmpCourbe[2]->getPointInCourbe().back());
				tmpCourbe.back()->computePointInCourbe();
				persp = true;
			}
			break;

		}*/

		destructor.generatePoints(POSX, POSY, POSZ, MAX_DIST, NB_POINTS);

		//std::shared_ptr<Polyhedron_3> baseObject = floor->generatePolyhedron();
		//auto currentDestruction = destructor.generateTriangulation3D(cube, *baseObject);

		destruction.clear();
		destructor.generateTriangulation3D(destruction, cube, *pyramidPoly);
		pyramid->alive = false;

		//mpEngine.createStack(PxTransform(PxVec3(0, 0, 10.0f)), 10, 2.0f);
		mpEngine.createDebris(destruction, 2.5f);
	}
}

void MyGLWidget::createPyramid()
{
	pyramid = new Objet();
	pyramidPoly = new Polyhedron_3();
	std::vector<float> vboPos;
	std::vector<unsigned int> eboIndices;
	std::vector<float> texcoords;
	std::vector<float> normals;
	std::vector<tinyobj::material_t> materials;
	pyramid->alive = true;
	int indice = 0;
	K::Point_3 p1(-100.0f, -100.0f, -100.0f),
		p2(100.0f, -100.0f, -100.0f),
		p4(100.0f, 100.0f, -100.0f),
		p6(100.0f, -100.0f, 100.0f);
	pyramidPoly->make_tetrahedron(p1, p2, p4, p6);
	auto facet = pyramidPoly->facets_begin();
	while (facet != pyramidPoly->facets_end())
	{
		K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_face_normal(facet, *pyramidPoly);
		pyramid->alive = true;
		auto vertice = facet->facet_begin();
		for (int i = 0; i < 3; i++, ++vertice)
		{
			//K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_vertex_normal(vertice->vertex(), triangulationPoly);
			vboPos.push_back(CGAL::to_double(vertice->vertex()->point().x()));
			vboPos.push_back(CGAL::to_double(vertice->vertex()->point().y()));
			vboPos.push_back(CGAL::to_double(vertice->vertex()->point().z()));
			eboIndices.push_back(indice);
			normals.push_back(CGAL::to_double(normal.x()));
			normals.push_back(CGAL::to_double(normal.y()));
			normals.push_back(CGAL::to_double(normal.z()));
			++indice;
		}
		++facet;
	}
	pyramid->loadVerticesAndIndices(eboIndices, vboPos);
	pyramid->reload();
	pyramid->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent * e)
{
	if (rightClick)
	{
		xRel = e->x() - m_oldMousePos.Getx();
		yRel = e->y() - m_oldMousePos.Gety();
	}
	m_oldMousePos = Point(e->x(), e->y(), 0);
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() == Qt::MouseButton::RightButton)
		rightClick = false;
}

void MyGLWidget::resizeEvent(QResizeEvent *e)
{
	projection.Perspective(45.0f, width(),height(), 0.1f,1000.0f );
}

bool MyGLWidget::event(QEvent *e)
{
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (ke->key() == Qt::Key_Left) 
			buffKey[2] = 1;
		
		if (ke->key() == Qt::Key_Right) 
			buffKey[3] = 1;
		
		if (ke->key() == Qt::Key_Up) 
			buffKey[0] = 1;
		
		if (ke->key() == Qt::Key_Down) 
			buffKey[1] = 1;
		
		if (ke->key() == Qt::Key_Space)
			buffKey[4] = 1;

		if (ke->key() == Qt::Key_Q)
			buffKey[2] = 1;
		
		if (ke->key() == Qt::Key_D) 
			buffKey[3] = 1;
		
		if (ke->key() == Qt::Key_Z) 
			buffKey[0] = 1;
		
		if (ke->key() == Qt::Key_S) 
			buffKey[1] = 1;
		
		if (ke->key() == Qt::Key_Shift)
			buffKey[5] = 1;

		if (ke->key() == Qt::Key_1) {
			persp = true;
			repaint();
		}

		if (ke->key() == Qt::Key_0) {
			persp = false;
			topView = false;
		}
		if (ke->key() == Qt::Key_Plus)
		{
			//topView = (topView == true) ? false: true;
			if (tmpCourbe.size() == 0)
				tmpCourbe.push_back(std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs")));
			if (tmpCourbe.back()->getLength() > 4)
				tmpCourbe.push_back(std::unique_ptr<Bezier>(new Bezier("courbeVertex.vs", "courbeFragment.fs")));
		}
		if (ke->key() == Qt::Key_7) {
			
			if (subdiv)
			{
				subdiv = false;
				floor->subdivisionCatmullClark();
				//floor->setupStruct();
				//floor->setFacePoint();
				//floor->setEdgePoint();
				//floor->setVertexPoint();
			}
		}

		if (ke->key() == Qt::Key_9) {

			if (subdiv)
			{
				subdiv = false;
				wall3->subdivisionLoop();
				//floor->setupStruct();
				//floor->setFacePoint();
				//floor->setEdgePoint();
				//floor->setVertexPoint();
			}
		}
		if (ke->key() == Qt::Key_7) {

			if (subdiv)
			{
				subdiv = false;
				floor->subdivisionCatmullClark();
				//floor->setupStruct();
				//floor->setFacePoint();
				//floor->setEdgePoint();
				//floor->setVertexPoint();
			}
		}

		if (ke->key() == Qt::Key_8) {

			if (subdiv)
			{
				subdiv = false;
				wall2->CalculKobbelt();
				//floor->setupStruct();
				//floor->setFacePoint();
				//floor->setEdgePoint();
				//floor->setVertexPoint();
			}
		}
		if (ke->key() == Qt::Key_5)
			wireframe = true;
		if (ke->key() == Qt::Key_2)
			wireframe = false;
		if (ke->key() == Qt::Key_Z)
			canMove = !canMove;
		if (ke->key() == Qt::Key_P)
			mpEngine.stepPhysics(true);

	}
	if (e->type() == QEvent::KeyRelease) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (ke->key() == Qt::Key_Left)
			buffKey[2] = 0;
		
		if (ke->key() == Qt::Key_Right)
			buffKey[3] = 0;
		
		if (ke->key() == Qt::Key_Up)
			buffKey[0] = 0;

		if (ke->key() == Qt::Key_Down)
			buffKey[1] = 0;

		if (ke->key() == Qt::Key_Space)
			buffKey[4] = 0;

		if (ke->key() == Qt::Key_Q)
			buffKey[2] = 0;

		if (ke->key() == Qt::Key_D)
			buffKey[3] = 0;

		if (ke->key() == Qt::Key_Z)
			buffKey[0] = 0;

		if (ke->key() == Qt::Key_S)
			buffKey[1] = 0;

		if (ke->key() == Qt::Key_Shift)
			buffKey[5] = 0;
		if (ke->key() == Qt::Key_7)
			subdiv = true; 
		if (ke->key() == Qt::Key_9)
			subdiv = true;
		if (ke->key() == Qt::Key_8)
			subdiv = true;
		if (ke->key() == Qt::Key_E)
			canMove = !canMove;
	}

	return QWidget::event(e);
}
