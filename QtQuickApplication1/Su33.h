#pragma once
#include "EngineModule.h"
#include "NumberRenderer.h"
#include "Plane.h"
#include "Object.h"
#include "RotationAnimator.h"
#include "TPFollowCamera.h"
#include "simpleAerodynamics.h"
#include "RocketLauncherModule.h"

class Su33 : public Plane
{
public:
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> navball;
	std::shared_ptr<Object> flapLeft;
	std::shared_ptr<Object> flapRight;
	std::shared_ptr<Object> rearFlap;
	

	std::shared_ptr<Object> powerBtn;
	std::shared_ptr<Object> engineBtn;
	
	std::shared_ptr<Object> radar;

	RotationAnimator leftFalpAnimator;
	RotationAnimator rightFlapAnimator;
	RotationAnimator rearFlapAnimator;
	QPoint lastPos;
	QPoint currentPos;
	std::shared_ptr<GLCamera> fpsCamera;
	std::shared_ptr<GLCamera> chaseCamera;
	
	QVector3D navBallPos{-0.105508, 0.082774, -2.34698};
	QVector3D rearFlapPos{0, -0.246162, 3.45535};
	//--
	std::shared_ptr<Sprite> mouseDeltaPos;
	float maxDeltaMouse = 200;
	float deltaX = 0;
	float deltaY = 0;

	EngineModule engine;
	RocketLauncherModule rockets;
	void animateSurfaces(std::shared_ptr<RigidBody> rb, std::shared_ptr<Transform> transform)
	{
		const QPoint delta = currentPos - lastPos;
		deltaX = std::clamp(delta.x() * 1.0f, -maxDeltaMouse, maxDeltaMouse);
		deltaY = std::clamp(delta.y() * 1.0f, -maxDeltaMouse, maxDeltaMouse);

		rb->addTorgue(transform->getRight() * deltaY * 0.00001f * SimpleAerodynamics::getControlCurve(engine.angleOfAttack, engine.fLocalSpeed));
		rb->addTorgue(transform->getForward() * deltaX * 0.00001f * SimpleAerodynamics::getControlCurve(engine.angleOfAttack, engine.fLocalSpeed));

		rightFlapAnimator.target = deltaX / maxDeltaMouse * 45;
		leftFalpAnimator.target = deltaX / maxDeltaMouse * -45;
		rearFlapAnimator.target = deltaY / maxDeltaMouse * -45;
		mouseDeltaPos->x = mouseDeltaPos->viewportW / 2 + deltaX;
		mouseDeltaPos->y = mouseDeltaPos->viewportH / 2 + deltaY;
	}
	
	void printQv(const QVector3D& v)
	{
		std::cout << v.x() << " " << v.y() << " " << v.z() << "\n";
	}
	
	void positionNavball()
	{
		auto fuselageQuat = ComponentManager::getComponent<Transform>(fuselage)->getRotationTransform();
		auto navBallTransform = ComponentManager::getComponent<Transform>(navball);
		QMatrix4x4 navTransform;
		navTransform.translate(navBallPos);
		navBallTransform->transform = navTransform;
		navBallTransform->rotate(fuselageQuat.inverted());
	}

	void onUpdate() override
	{
		deltaX = 0;
		deltaY = 0;

		auto rb = ComponentManager::getComponent<RigidBody>(fuselage);
		auto transform = ComponentManager::getComponent<Transform>(fuselage);

		if (Input::keyPressed(Qt::Key_G))
		{
			fpsCamera->position += MouseInput::delta().x() * QVector3D(1, 0, 0) * 0.002f;
			fpsCamera->position += MouseInput::delta().y() * QVector3D(0, 1, 0) * 0.002f;
		}
		if (MouseInput::keyJustPressed(Qt::MiddleButton))
		{
			lastPos = MouseInput::getPosition();
			currentPos = lastPos;
		}
		if (MouseInput::keyPressed(Qt::MiddleButton))
		{
			currentPos += MouseInput::delta();
			mouseDeltaPos->visible = true;
			animateSurfaces(rb, transform);
		}
		else
		{
			mouseDeltaPos->visible = false;

			rightFlapAnimator.target = 0;
			leftFalpAnimator.target = 0;
		}
		engine.update();
		rockets.update();
		
		positionNavball();
		
		if(Input::keyJustPressed(Qt::Key_F) && scene->pickedObjectId != -1 && scene->pickedObjectId < scene->objects.size())
		{
			auto selected = scene->objects[scene->pickedObjectId];
			if(selected == engineBtn)
			{
				engine.enabled = !engine.enabled;
				ComponentManager::getComponent<Material>(engineBtn)->diffuse = engine.enabled ? QColor(0, 220, 0) : QColor(220, 0, 0);
				std::cout << "Eng\n";
			}
		}
		std::shared_ptr<RadarMeshRenderer> rm = std::dynamic_pointer_cast<RadarMeshRenderer>(ComponentManager::getComponent<MeshRenderer>(radar));
		rm->planePos = transform->position;
		std::cout << transform->position.z()<<'\n';
	}
	
	Scene* scene;
	void init(Scene* scene) override
	{
		this->scene = scene;
		//
		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/body.obj"), {-18.0f, 10, 40},
		                ShaderCollection::shaders["normals"]);
		fuselage = scene->objects.back();
		ComponentManager::addComponent(fuselage, std::make_shared<RigidBody>())->init(
			CollisionShapeGenerator::getCompoundShape(
				{
					CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/body_c.obj"),
					CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/wings_c.obj")
				}), 1);

		auto fuselageRB = ComponentManager::getComponent<RigidBody>(fuselage);
		//fuselageRB->setGravity(QVector3D(0, -3.0f, 0));
		fuselageRB->setAngularDamping(0.6f);
		fuselageRB->setLinearDamping(0.07f);
		//--

		loadPlanePart(scene, "Assets/Models/plane/wings.obj");
		loadPlanePart(scene, "Assets/Models/plane/engine.obj");
		loadPlanePart(scene, "Assets/Models/plane/panel.obj");
		loadPlanePart(scene, "Assets/Models/plane/seat.obj");
		loadPlanePart(scene, "Assets/Models/plane/visor_frame.obj");
		loadPlanePart(scene, "Assets/Models/plane/tail.obj");
		loadPlanePart(scene, "Assets/Models/plane/tail2.obj");

		loadPlanePart(scene, "Assets/Models/plane/navball.obj", ShaderCollection::shaders["plain"], navBallPos);
		navball = scene->objects.back();

		loadPlanePart(scene, "Assets/Models/plane/flap_l.obj", ShaderCollection::shaders["normals"], { -2.11702, -0.155708, 1.33746 });
		flapLeft = scene->objects.back();
		leftFalpAnimator.init(ComponentManager::getComponent<Transform>(flapLeft), QVector3D(1, 0, 0), 120);

		loadPlanePart(scene, "Assets/Models/plane/flap_r.obj", ShaderCollection::shaders["normals"], { 2.11702, -0.155708, 1.33746 });
		flapRight = scene->objects.back();
		rightFlapAnimator.init(ComponentManager::getComponent<Transform>(flapRight), QVector3D(1, 0, 0), 120);

		loadPlanePart(scene, "Assets/Models/plane/rearControl.obj", ShaderCollection::shaders["normals"], rearFlapPos);
		
		rearFlap = scene->objects.back();
		rearFlapAnimator.init(ComponentManager::getComponent<Transform>(rearFlap), QVector3D(1, 0, 0), 120);

		//===================
		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/cockpit.obj"), {0, 0, 0},
		                      ShaderCollection::shaders["plain"]);
		fuselage->addChild(scene->transparentObjects.back());
		ComponentManager::getComponent<Material>(scene->transparentObjects.back())->alpha = 0.23f;

		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/visor.obj"), {0, 0, 0},
		                      ShaderCollection::shaders["visor"]);
		fuselage->addChild(scene->transparentObjects.back());
		///----
		//exhaust2.obj
		
		chaseCamera = std::make_shared<TPFollowCamera>();
		chaseCamera->enabled = false;
		std::dynamic_pointer_cast<TPFollowCamera>(chaseCamera)->target = ComponentManager::getComponent<Transform>(fuselage);

		fpsCamera = std::make_shared<FPSFollowCamera>();
		fpsCamera->enabled = false;
		fpsCamera->position = QVector3D(0, 0.29, -1.9);
		std::dynamic_pointer_cast<FPSFollowCamera>(fpsCamera)->target = ComponentManager::getComponent<Transform>(fuselage);

		//-----
		loadPlanePart(scene, "Assets/Models/plane/text/ias.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/text/alt.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/text/thr.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/text/power.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/text/eng.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/text/gear.obj", ShaderCollection::shaders["plain"]);

		//--------

		loadPlanePart(scene, "Assets/Models/plane/btns/engBtn.obj", ShaderCollection::shaders["plain"]);
		engineBtn = scene->objects.back();
		loadPlanePart(scene, "Assets/Models/plane/btns/powBtn.obj", ShaderCollection::shaders["plain"]);
		loadPlanePart(scene, "Assets/Models/plane/btns/gearBtn.obj", ShaderCollection::shaders["plain"]);
		
		loadPlanePart(scene, "Assets/Models/plane/radar.obj", ShaderCollection::shaders["radar"]);
		radar = scene->objects.back();
		//---
		auto cross = std::make_shared<Sprite>("Assets\\Sprites\\cross.png", 40, 40);
		cross->setRelPos(0, 0);
		scene->sprites.push_back(cross);

		mouseDeltaPos = std::make_shared<Sprite>("Assets\\Sprites\\cross2.png", 30, 30);
		scene->sprites.push_back(mouseDeltaPos);
		//
		auto rb = ComponentManager::getComponent<RigidBody>(fuselage);
		auto transform = ComponentManager::getComponent<Transform>(fuselage);
		
		engine.init(scene, rb, transform, fuselage);
		rockets.init(scene, rb, transform, fuselage);
	}
	
	void loadPlanePart(Scene* scene, const std::string& path, ShaderData data = ShaderCollection::shaders["normals"], const QVector3D& pos = QVector3D(0,0,0))
	{
		scene->addModel(MeshLoader().loadModel(path), pos, data);
		fuselage->addChild(scene->objects.back());
	}
};
