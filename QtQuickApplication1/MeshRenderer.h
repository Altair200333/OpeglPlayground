#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <utility>

#include "ImageBackground.h"
#include "ComponentManager.h"
#include "Mesh.h"
#include "GLcamera.h"
#include "PointLight.h"
#include "Material.h"
#include "RenderContext.h"
#include "Transform.h"

class MeshRenderer : public Component
{
public:
	QOpenGLBuffer* vbo = nullptr;
	QOpenGLBuffer* ibo = nullptr;
	QOpenGLVertexArrayObject* vao = nullptr;

	std::shared_ptr<QOpenGLShaderProgram> shader = nullptr;

	QOpenGLFunctions* functions;

	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	bool enabled = true;

	MeshRenderer() = default;
	virtual ~MeshRenderer() = default;

	void enableAttributes(std::shared_ptr<QOpenGLShaderProgram> shader) const
	{
		shader->enableAttributeArray("posAttr");
		shader->setAttributeBuffer("posAttr", GL_FLOAT, 0, 3, sizeof(Vertex));

		shader->enableAttributeArray("normalAttr");
		shader->setAttributeBuffer("normalAttr", GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

		shader->enableAttributeArray("colAttr");
		shader->setAttributeBuffer("colAttr", GL_FLOAT, offsetof(Vertex, color), 3, sizeof(Vertex));

		shader->enableAttributeArray("aTexCoords");
		shader->setAttributeBuffer("aTexCoords", GL_FLOAT, offsetof(Vertex, TexCoords), 2, sizeof(Vertex));

		shader->enableAttributeArray("aTangent");
		shader->setAttributeBuffer("aTangent", GL_FLOAT, offsetof(Vertex, Tangent), 3, sizeof(Vertex));

		shader->enableAttributeArray("aBitangent");
		shader->setAttributeBuffer("aBitangent", GL_FLOAT, offsetof(Vertex, Bitangent), 3, sizeof(Vertex));
	}

	void createShader(const std::string& fragment, const std::string& vertex, const std::string& geometry)
	{
		shader = std::make_shared<QOpenGLShaderProgram>();
		shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex.c_str());
		shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment.c_str());

		if (!geometry.empty())
			shader->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry.c_str());
	}

	void createVao()
	{
		vao = new QOpenGLVertexArrayObject();
		vao->create();
		vao->bind();
	}

	void createVbo()
	{
		vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		vbo->create();
		vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
		if (!vbo->bind())
		{
			qWarning() << "Could not bind vertex buffer to the context";
			return;
		}
		vbo->allocate(mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
	}

	void createIbo()
	{
		ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		ibo->create();
		ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
		ibo->bind();
		ibo->allocate(mesh->indices.data(), mesh->indices.size() * sizeof(GLuint));
	}

	void init(QOpenGLFunctions* _functions)
	{
		functions = _functions;

		mesh = ComponentManager::getComponent<Mesh>(owner);
		material = ComponentManager::getComponent<Material>(owner);
		transform = ComponentManager::getComponent<Transform>(owner);

		createVao();
		createVbo();
		createIbo();

		enableAttributes(shader);

		vao->release();
	}

	void initMeshRenderer(QOpenGLFunctions* _functions, std::shared_ptr<QOpenGLShaderProgram> _shader)
	{
		shader = _shader;

		init(_functions);
	}

	
	void uploadCameraDetails(std::shared_ptr<GLCamera> camera, std::shared_ptr<QOpenGLShaderProgram> shader) const
	{
		shader->setUniformValue(shader->uniformLocation("view"), camera->getViewMatrix());
		shader->setUniformValue(shader->uniformLocation("projection"), camera->getProjectionMatrix());
		shader->setUniformValue(shader->uniformLocation("cameraPos"), camera->getPosition());
	}

	void renderWireframe(std::shared_ptr<GLCamera>& camera) const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shader->bind();
		uploadCameraDetails(camera, shader);
		shader->setUniformValue(shader->uniformLocation("model"),transform->getGlobalTransform());
		shader->setUniformValue("wireframe", true);

		vao->bind();
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		vao->release();
	}

	virtual void render(const RenderContext& context) = 0;
};
