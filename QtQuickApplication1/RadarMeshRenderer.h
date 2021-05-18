#pragma once
#include "MeshRenderer.h"

class RadarMeshRenderer : public MeshRenderer
{
public:
	QVector3D planePos;
	
	void render(const RenderContext& context) override
	{
		if (!enabled)
			return;

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader->bind();
		uploadCameraDetails(context.camera, shader);
		shader->setUniformValue(shader->uniformLocation("model"), transform->getGlobalTransform());
		shader->setUniformValue("planePos", planePos);
		
		vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		vao->release();
	}
};
