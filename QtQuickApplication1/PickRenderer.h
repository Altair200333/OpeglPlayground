#pragma once
#include <QOpenGLContext>
#include <qopenglframebufferobject.h>

#include "GLWindow.h"
#include "RenderLayer.h"

class PickRenderer final: public RenderLayer
{
public:
	std::shared_ptr<Scene> scene;
	std::shared_ptr<GLWindow> viewport;
	
	int pickedObjectId = -1;
protected:

	QOpenGLFramebufferObject* mFBO = nullptr;
	std::unordered_map<std::shared_ptr<Object>, bool> initializedPickShaders;

	void createFBO()
	{
		QOpenGLContext* ctx = QOpenGLContext::currentContext();

		QOpenGLFramebufferObjectFormat format;
		format.setSamples(0);
		format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

		if (!mFBO)
			mFBO = new QOpenGLFramebufferObject(QSize(viewport->width(), viewport->height()), format);


		viewport->glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx->defaultFramebufferObject());
		viewport->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBO->handle());
		ctx->extraFunctions()->glBlitFramebuffer(0, 0, viewport->width(), viewport->height(),
			0, 0, mFBO->width(), mFBO->height(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void onRender() override
	{
		createFBO();

		mFBO->bind();

		renderPickShader();

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned char data[4] = {1, 1, 1, 0};
		viewport->glReadPixels(MouseInput::getPosition().x(), viewport->height() - MouseInput::getPosition().y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		const int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

		mFBO->release();

		if (pickedID != 0x00ffffff)
		{
			pickedObjectId = pickedID;
			std::string col = std::to_string(data[0]) + " " + std::to_string(data[1]) + " " + std::to_string(data[2]) + " "
				+ std::to_string(data[3]) + "; " + std::to_string(pickedID);
		}
		else
		{
			pickedObjectId = -1;
		}
	}
	void renderPickShader()
	{
		auto shader = ShaderCollection::shaders["pick"].getShader();
		shader->bind();

		for (int i = 0; i < scene->objects.size(); ++i)
		{
			int r = (i & 0x000000FF) >> 0;
			int g = (i & 0x0000FF00) >> 8;
			int b = (i & 0x00FF0000) >> 16;

			shader->setUniformValue("color", QColor(r, g, b));
			auto meshRenderer = ComponentManager::getComponent<MeshRenderer>(scene->objects[i]);
			const auto mesh = ComponentManager::getComponent<Mesh>(scene->objects[i]);
			const auto transform = ComponentManager::getComponent<Transform>(scene->objects[i]);

			if (!initializedPickShaders[scene->objects[i]])
			{
				meshRenderer->enableAttributes(shader);
				initializedPickShaders[scene->objects[i]] = true;
			}

			meshRenderer->uploadCameraDetails(scene->camera, shader);
			shader->setUniformValue(shader->uniformLocation("model"), transform->getGlobalTransform());

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			meshRenderer->vao->bind();
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			meshRenderer->vao->release();
		}
	}
};
