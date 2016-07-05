#ifndef PHYSX_ENGINE_PERSO
#define PHYSX_ENGINE_PERSO

#include <ctype.h>

#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"

using namespace physx;

struct PhysXEng
{
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics = NULL;

	PxDefaultCpuDispatcher*	gDispatcher = NULL;
	PxScene*				gScene = NULL;

	PxMaterial*				gMaterial = NULL;

	PxVisualDebuggerConnection*
		gConnection = NULL;

	PhysXEng() :gAllocator(), gErrorCallback(){}

	void initPhysics(bool interactive)
	{
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
		if (!gFoundation)
			std::cout << "shiiit";
		PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());// , true, profileZoneManager);
		//gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, gAllocator, gErrorCallback, PxTolerancesScale(), true);

		if (gPhysics->getPvdConnectionManager())
		{
			gPhysics->getVisualDebugger()->setVisualizeConstraints(true);
			gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
			gPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
			gConnection = PxVisualDebuggerExt::createConnection(gPhysics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
		}

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		gScene = gPhysics->createScene(sceneDesc);

		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

		PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
		gScene->addActor(*groundPlane);

		/*for (PxU32 i = 0; i<5; i++)
			createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

		if (!interactive)
			createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));*/
	}

	void stepPhysics(bool interactive)
	{
		PX_UNUSED(interactive);
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
	}

	void cleanupPhysics(bool interactive)
	{
		PX_UNUSED(interactive);
		gScene->release();
		gDispatcher->release();
		PxProfileZoneManager* profileZoneManager = gPhysics->getProfileZoneManager();
		if (gConnection != NULL)
			gConnection->release();
		gPhysics->release();
		profileZoneManager->release();
		gFoundation->release();

		printf("SnippetHelloWorld done.\n");
	}
};





#endif