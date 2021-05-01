#pragma once
#include "Plane.h"
#include "Object.h"
#include "RotationAnimator.h"
#include "TPFollowCamera.h"
class Su33 : public Plane
{
public:
	std::shared_ptr<Object> fuselage;
	std::shared_ptr<Object> navball;
	std::shared_ptr<Object> flapLeft;
	std::shared_ptr<Object> flapRight;
	std::shared_ptr<Object> rearFlap;
	RotationAnimator animator;
	RotationAnimator leftFalpAnimator;
	RotationAnimator rightFlapAnimator;
	RotationAnimator rearFlapAnimator;
	QPoint lastPos;
	std::shared_ptr<GLCamera> fpsCamera;
	std::shared_ptr<GLCamera> chaseCamera;

	QVector3D navBallPos{ -0.105508, 0.082774, -2.34698 };
	QVector3D rearFlapPos{ 0, -0.246162 , 3.45535 };
	
	float maxDeltaMouse = 400;
	void controlPlane(std::shared_ptr<RigidBody> rb, std::shared_ptr<Transform> transform)
	{
		QPoint delta = MouseInput::getPosition() - lastPos;
		float deltaX = std::clamp(delta.x()*1.0f, -maxDeltaMouse, maxDeltaMouse);
		float deltaY = std::clamp(delta.y()*1.0f, -maxDeltaMouse, maxDeltaMouse);
		
		rb->addTorgue(transform->getRight() * deltaY * -0.03f);
		rb->addTorgue(transform->getForward() * deltaX * 0.02f);
		rightFlapAnimator.target = deltaX/maxDeltaMouse*45;
		leftFalpAnimator.target = deltaX / maxDeltaMouse *-45;
		rearFlapAnimator.target = deltaY / maxDeltaMouse *45;
	}

	void onUpdate() override
	{
		{
			auto rb = ComponentManager::getComponent<RigidBody>(fuselage);
			auto transform = ComponentManager::getComponent<Transform>(fuselage);
			if (Input::keyPressed(Qt::Key_Shift))
			{
				rb->addForce(transform->getForward() * 20);
			}
			if (Input::keyPressed(Qt::Key_Control))
			{
				rb->addForce(-transform->getForward() * 20);
			}
			if (Input::keyPressed(Qt::Key_G))
			{
				fpsCamera->position += MouseInput::delta().x() * QVector3D(1,0,0) * 0.002f;
				fpsCamera->position += MouseInput::delta().y() * QVector3D(0,1,0) * 0.002f;
			}
			if(MouseInput::keyJustPressed(Qt::MiddleButton))
			{
				lastPos = MouseInput::getPosition();
			}
			if (MouseInput::keyPressed(Qt::MiddleButton))
			{
				controlPlane(rb, transform);
			}
			else
			{
				rightFlapAnimator.target = 0;
				leftFalpAnimator.target = 0;
			}
			
		}

		auto fuselageQuat = ComponentManager::getComponent<Transform>(fuselage)->getRotationTransform();
		auto navBallTransform = ComponentManager::getComponent<Transform>(navball);
		QMatrix4x4 navTransform;
		navTransform.translate(navBallPos);
		navBallTransform->transform = navTransform;
		navBallTransform->rotate(fuselageQuat.inverted());
	}

	void init(Scene* scene) override
	{
		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/body.obj"), {1.5f, 3, 0},
		                ShaderCollection::shaders["normals"]);
		fuselage = scene->objects.back();
		ComponentManager::addComponent(fuselage, std::make_shared<RigidBody>())->init(
			CollisionShapeGenerator::getCompoundShape(
				{
					CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/body_c.obj"),
					CollisionShapeGenerator::getMeshCollider("Assets/Models/plane/wings_c.obj")
				}), 1);

		auto fuselageRB = ComponentManager::getComponent<RigidBody>(fuselage);
		fuselageRB->setGravity(QVector3D(0, -3.0f, 0));
		fuselageRB->setAngularDamping(0.4f);
		//--

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/wings.obj"), {0, 0, 0},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		animator.init(ComponentManager::getComponent<Transform>(scene->objects.back()), QVector3D(1, 0, 0));

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/engine.obj"), {0, 0, 0},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/panel.obj"), {0, 0, 0},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/seat.obj"), {0, 0, 0},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/visor_frame.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/tail.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/tail2.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());
		
		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/navball.obj"), navBallPos,
		                ShaderCollection::shaders["plain"]);
		navball = scene->objects.back();
		fuselage->addChild(navball);

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/flap_l.obj"), {-2.11702, -0.155708, 1.33746},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());
		flapLeft = scene->objects.back();
		leftFalpAnimator.init(ComponentManager::getComponent<Transform>(flapLeft), QVector3D(1, 0, 0), 30);

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/flap_r.obj"), {2.11702, -0.155708, 1.33746},
		                ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());
		flapRight = scene->objects.back();
		rightFlapAnimator.init(ComponentManager::getComponent<Transform>(flapRight), QVector3D(1, 0, 0), 30);

		scene->addModel(MeshLoader().loadModel("Assets/Models/plane/rearControl.obj"), rearFlapPos,
			ShaderCollection::shaders["normals"]);
		fuselage->addChild(scene->objects.back());
		rearFlap = scene->objects.back();
		rearFlapAnimator.init(ComponentManager::getComponent<Transform>(rearFlap), QVector3D(1, 0, 0), 30);

		//===================
		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/cockpit.obj"), {0, 0, 0},
		                      ShaderCollection::shaders["plain"]);
		fuselage->addChild(scene->transparentObjects.back());
		ComponentManager::getComponent<Material>(scene->transparentObjects.back())->alpha = 0.23f;

		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/visor.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["visor"]);
		fuselage->addChild(scene->transparentObjects.back());


		chaseCamera = std::make_shared<TPFollowCamera>();
		chaseCamera->enabled = false;
		std::dynamic_pointer_cast<TPFollowCamera>(chaseCamera)->target = ComponentManager::getComponent<Transform>(fuselage);

		fpsCamera = std::make_shared<FPSFollowCamera>();
		fpsCamera->enabled = false;
		fpsCamera->position = QVector3D(0, 0.29, -1.9);
		std::dynamic_pointer_cast<FPSFollowCamera>(fpsCamera)->target = ComponentManager::getComponent<Transform>(fuselage);

	}
};
