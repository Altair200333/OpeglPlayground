#pragma once
#include <memory>
#include <qmath.h>



#include "Input.h"
#include "Material.h"
#include "Module.h"
#include "NumberRenderer.h"
#include "RigidBody.h"
#include "RotationAnimator.h"
#include "simpleAerodynamics.h"

class EngineModule: public Module
{
public:
	std::shared_ptr<RigidBody> body;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Object> parent;

	//--
	std::shared_ptr<Object> exhaust1;
	std::shared_ptr<Object> exhaust2;
	std::shared_ptr<Object> thrHandle;
	RotationAnimator thrHandleAnimator;

	//
	std::shared_ptr<NumberRenderer> ias;
	std::shared_ptr<NumberRenderer> alt;
	std::shared_ptr<NumberRenderer> thr;
	
	float angleOfAttack;
	float fLocalSpeed;
	float thrust = 0;
	float thrustRate = 25;
	void init(Scene* scene, std::shared_ptr<RigidBody> _body, std::shared_ptr<Transform> _transform, std::shared_ptr<Object> _parent)
	{
		body = _body;
		transform = _transform;
		parent = _parent;

		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/exhaust1.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["plain"]);
		parent->addChild(scene->transparentObjects.back());
		ComponentManager::getComponent<Material>(scene->transparentObjects.back())->alpha = 0.2f;
		exhaust1 = scene->transparentObjects.back();

		scene->addTransparent(MeshLoader().loadModel("Assets/Models/plane/exhaust2.obj"), { 0, 0, 0 },
			ShaderCollection::shaders["plain"]);
		parent->addChild(scene->transparentObjects.back());
		ComponentManager::getComponent<Material>(scene->transparentObjects.back())->alpha = 0.75f;
		exhaust2 = scene->transparentObjects.back();

		addPanelText(scene, ias, { 0.065182, 0.112829, -2.35856 });
		addPanelText(scene, alt, { 0.063522, 0.103151, -2.35368 });
		addPanelText(scene, thr, { 0.064752, 0.09316, -2.34865 });

		loadPlanePart(scene, "Assets/Models/plane/thrHandle.obj", ShaderCollection::shaders["normals"], { -0.136533,-0.099797 , -2.15335 });
		thrHandle = scene->objects.back();
		thrHandleAnimator.init(ComponentManager::getComponent<Transform>(thrHandle), QVector3D(1, 0, 0), 30);
	}
	void update() override
	{
		
		//std::cout << body->qvelocity().x()<<"\n";
		if (Input::keyPressed(Qt::Key_Shift))
		{
			thrust += FPSCounter::getFrameTime() * thrustRate;
		}
		if (Input::keyPressed(Qt::Key_Control))
		{
			thrust -= FPSCounter::getFrameTime() * thrustRate;
		}

		thrust = std::clamp<float>(thrust, 0, 100);
		thrHandleAnimator.target = 30 - 60 * thrust / 100.0f;
		//---
		ias->number = std::round(body->qvelocity().length());
		alt->number = std::max<int>(0, transform->position.y());
		thr->number = thrust;
		//--

		QVector3D drag = -body->qvelocity();
		fLocalSpeed = drag.length();
		drag.normalize();

		auto vLift = QVector3D::crossProduct(QVector3D::crossProduct(drag, transform->getUp()), drag);
		vLift.normalize();

		float tmp = QVector3D::dotProduct(drag, transform->getUp());
		tmp = std::clamp(tmp, -1.0f, 1.0f);

		angleOfAttack = qRadiansToDegrees(asin(tmp));

		float liftScale = 1;
		float rho = 1;
		float area = 2;
		tmp = 0.5 * rho * fLocalSpeed * fLocalSpeed * area;
		auto result = (vLift * SimpleAerodynamics::LiftCoefficient(angleOfAttack) + drag * SimpleAerodynamics::DragCoefficient(angleOfAttack)) * tmp * liftScale;
		body->addForce(result * FPSCounter::getFrameTime());

		if (!enabled)
		{
			thr->number = 0;
			ComponentManager::getComponent<Material>(exhaust1)->alpha = 0;
			ComponentManager::getComponent<Material>(exhaust2)->alpha = 0;
			return;
		}
		body->addForce(transform->getForward() * thrust * 0.01f * 20);

		//--
		ComponentManager::getComponent<Material>(exhaust1)->alpha = 0.3f * thrust * 0.01f;
		ComponentManager::getComponent<Material>(exhaust2)->alpha = 0.75f * thrust * 0.01f;
	}
	
	void loadPlanePart(Scene* scene, const std::string& path, ShaderData data = ShaderCollection::shaders["normals"], const QVector3D& pos = QVector3D(0, 0, 0))
	{
		scene->addModel(MeshLoader().loadModel(path), pos, data);
		parent->addChild(scene->objects.back());
	}
	void addPanelText(Scene* scene, std::shared_ptr<NumberRenderer>& text, const QVector3D& pos)
	{
		text = std::make_shared<NumberRenderer>(scene->functions);
		text->position = pos;
		text->setParent(parent);
	}
};
