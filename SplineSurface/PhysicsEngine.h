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

	PxFoundation*	gFoundation;
	PxPhysics*		gPhysics;

	PxDefaultCpuDispatcher*	gDispatcher;
	PxScene*			gScene;

	PxMaterial*		gMaterial;

	PxVisualDebuggerConnection*
		gConnection;

	PhysXEng() :gAllocator(), gErrorCallback()
	{
		gFoundation = NULL;
		gPhysics = NULL;
		gDispatcher = NULL;
		gScene = NULL;
		gMaterial = NULL;
		gConnection = NULL;
	}

	PxReal stackZ = 10.0f;

	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
	{
		PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
		dynamic->setAngularDamping(0.5f);
		dynamic->setLinearVelocity(velocity);
		gScene->addActor(*dynamic);
		return dynamic;
	}

	void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
	{
		PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
		for (PxU32 i = 0; i<size; i++)
		{
			for (PxU32 j = 0; j<size - i; j++)
			{
				PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
				PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				gScene->addActor(*body);
			}
		}
		shape->release();
	}

	void createDebris(const std::vector<std::shared_ptr<Objet>>& debris, PxReal halfExtent)
	{
		PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
		
		for(auto it = debris.begin(); it != debris.end()-1; ++it)//for (PxU32 j = 0; j<size - i; j++)
		{
			PxTransform t(PxVec3((*it)->position.x, (*it)->position.y, (*it)->position.z));
			
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t);
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
		
		shape->release();
	}

	void initPhysics(bool interactive)
	{
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
		PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, profileZoneManager);

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

		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f);

		PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(PxVec3(0, -50, 0), PxVec3(0, 1, 0)), *gMaterial);
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

	void stepPhysics(float deltaTime)
	{
		//PX_UNUSED(interactive);
		gScene->simulate(deltaTime);
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