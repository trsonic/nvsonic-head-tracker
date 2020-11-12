#pragma once

#if JUCE_MAC
#import <OpenGL/gl.h>
#endif

#include "../JuceLibraryCode/JuceHeader.h"
#include "WavefrontObjParser.h"

inline String loadEntireAssetIntoString(const char* assetName)
{
	int size;
	const char* headModel = BinaryData::getNamedResource("male_head_obj", size);

	MemoryInputStream mem(headModel, size, false);

	return mem.readString();
}

class BinauralHeadView
	: public Component
	, public OpenGLRenderer
{
public:
	BinauralHeadView();
	void init();
	void deinit();

	void setHeadOrientation(float roll, float pitch, float yaw);

private:
	void newOpenGLContextCreated() override;
	void paint(Graphics& g) override;
	void renderOpenGL() override;
	void openGLContextClosing() override;

	Matrix3D<float> getProjectionMatrix() const
	{
		auto w = 1.0f;
		auto h = w * getLocalBounds().toFloat().getAspectRatio(false);

		return Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.5f, 30.0f);
	}

	Matrix3D<float> getViewMatrix() const
	{
		Matrix3D<float> viewMatrix({ 0.0f, 0.0f, -5.0f });
		return viewMatrix;
	}

	Matrix3D<float> getModelMatrix() const
	{
		return Matrix3D<float>::rotation(Vector3D<float>(degreesToRadians(m_pitch), degreesToRadians(m_yaw), degreesToRadians(m_roll)));
	}

	void createShaders()
	{
		m_vertexShader =
			"attribute vec4 position;\n"
			"attribute vec4 normal;\n"
			"\n"
			"uniform mat4 projectionMatrix;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 modelMatrix;\n"
			"uniform vec4 lightPosition;\n"
			"\n"
			"varying float lightIntensity;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    vec4 light =  (viewMatrix * lightPosition) * modelMatrix;\n"
			"    lightIntensity = dot(light, normal);\n"
			"\n"
			"    gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;\n"
			"}\n";

		m_fragmentShader =
			"varying float lightIntensity;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	float l = lightIntensity * 0.1;\n"
			"	vec4 colour = vec4(l, l, l, 1.0);\n"
			"\n"
			"	gl_FragColor = colour;\n"
			"}\n";

		std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(m_renderingContext));
		String statusText;

		if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(m_vertexShader))
			&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(m_fragmentShader))
			&& newShader->link())
		{
			m_shape.reset();
			m_attributes.reset();
			m_uniforms.reset();

			m_shader.reset(newShader.release());
			m_shader->use();

			m_shape.reset(new Shape(m_renderingContext));
			m_attributes.reset(new Attributes(m_renderingContext, *m_shader));
			m_uniforms.reset(new Uniforms(m_renderingContext, *m_shader));

			statusText = "GLSL: v" + String(OpenGLShaderProgram::getLanguageVersion(), 2);
		}
		else
		{
			statusText = newShader->getLastError();
		}
	}

	struct Vertex
	{
		float position[3];
		float normal[3];
		float colour[4];
		float texCoord[2];
	};

	// This class just manages the attributes that the shaders use.
	struct Attributes
	{
		Attributes(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
		{
			position.reset(createAttribute(openGLContext, shaderProgram, "position"));
			normal.reset(createAttribute(openGLContext, shaderProgram, "normal"));
			sourceColour.reset(createAttribute(openGLContext, shaderProgram, "sourceColour"));
			textureCoordIn.reset(createAttribute(openGLContext, shaderProgram, "textureCoordIn"));
		}

		void enable(OpenGLContext& glContext)
		{
			if (position.get() != nullptr)
			{
				glContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
				glContext.extensions.glEnableVertexAttribArray(position->attributeID);
			}

			if (normal.get() != nullptr)
			{
				glContext.extensions.glVertexAttribPointer(normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
				glContext.extensions.glEnableVertexAttribArray(normal->attributeID);
			}

			if (sourceColour.get() != nullptr)
			{
				glContext.extensions.glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
				glContext.extensions.glEnableVertexAttribArray(sourceColour->attributeID);
			}

			if (textureCoordIn.get() != nullptr)
			{
				glContext.extensions.glVertexAttribPointer(textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 10));
				glContext.extensions.glEnableVertexAttribArray(textureCoordIn->attributeID);
			}
		}

		void disable(OpenGLContext& glContext)
		{
			if (position.get() != nullptr)       glContext.extensions.glDisableVertexAttribArray(position->attributeID);
			if (normal.get() != nullptr)         glContext.extensions.glDisableVertexAttribArray(normal->attributeID);
			if (sourceColour.get() != nullptr)   glContext.extensions.glDisableVertexAttribArray(sourceColour->attributeID);
			if (textureCoordIn.get() != nullptr) glContext.extensions.glDisableVertexAttribArray(textureCoordIn->attributeID);
		}

		std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;

	private:
		static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& openGLContext,
			OpenGLShaderProgram& m_shader,
			const char* attributeName)
		{
			if (openGLContext.extensions.glGetAttribLocation(m_shader.getProgramID(), attributeName) < 0)
				return nullptr;

			return new OpenGLShaderProgram::Attribute(m_shader, attributeName);
		}
	};

	struct Uniforms
	{
		Uniforms(OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
		{
			projectionMatrix.reset(createUniform(openGLContext, shaderProgram, "projectionMatrix"));
			viewMatrix.reset(createUniform(openGLContext, shaderProgram, "viewMatrix"));
			modelMatrix.reset(createUniform(openGLContext, shaderProgram, "modelMatrix"));
			lightPosition.reset(createUniform(openGLContext, shaderProgram, "lightPosition"));
		}

		std::unique_ptr<OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix, modelMatrix, lightPosition;

	private:
		static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& openGLContext,
			OpenGLShaderProgram& shaderProgram,
			const char* uniformName)
		{
			if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
				return nullptr;

			return new OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
		}
	};

	struct Shape
	{
		Shape(OpenGLContext& glContext)
		{
			if (shapeFile.load(loadEntireAssetIntoString("male_head.obj")).wasOk())
				for (auto* shapeVertices : shapeFile.shapes)
					vertexBuffers.add(new VertexBuffer(glContext, *shapeVertices));
		}

		void draw(OpenGLContext& glContext, Attributes& glAttributes)
		{
			for (auto* vertexBuffer : vertexBuffers)
			{
				vertexBuffer->bind();

				glAttributes.enable(glContext);
				glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, nullptr);
				glAttributes.disable(glContext);
			}
		}

	private:
		struct VertexBuffer
		{
			VertexBuffer(OpenGLContext& context, WavefrontObjFile::Shape& aShape)
				: openGLContext(context)
			{
				numIndices = aShape.mesh.indices.size();

				openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

				Array<Vertex> vertices;
				createVertexListFromMesh(aShape.mesh, vertices, Colours::green);

				openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER,
					static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof(Vertex)),
					vertices.getRawDataPointer(), GL_STATIC_DRAW);

				openGLContext.extensions.glGenBuffers(1, &indexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof(juce::uint32)),
					aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
			}

			~VertexBuffer()
			{
				openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
				openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
			}

			void bind()
			{
				openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			}

			GLuint vertexBuffer, indexBuffer;
			int numIndices;
			OpenGLContext& openGLContext;

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
		};

		WavefrontObjFile shapeFile;
		OwnedArray<VertexBuffer> vertexBuffers;

		static void createVertexListFromMesh(const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
		{
			auto scale = 0.2f;
			WavefrontObjFile::TextureCoord defaultTexCoord{ 0.5f, 0.5f };
			WavefrontObjFile::Vertex defaultNormal{ 0.5f, 0.5f, 0.5f };

			for (auto i = 0; i < mesh.vertices.size(); ++i)
			{
				const auto& v = mesh.vertices.getReference(i);
				const auto& n = i < mesh.normals.size() ? mesh.normals.getReference(i) : defaultNormal;
				const auto& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference(i) : defaultTexCoord;

				list.add({ { scale * v.x, scale * v.y, scale * v.z, },
							{ scale * n.x, scale * n.y, scale * n.z, },
							{ colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
							{ tc.x, tc.y } });
			}
		}
	};
	
	const char* m_vertexShader;
	const char* m_fragmentShader;

	std::unique_ptr<OpenGLShaderProgram> m_shader;
	std::unique_ptr<Shape> m_shape;
	std::unique_ptr<Attributes> m_attributes;
	std::unique_ptr<Uniforms> m_uniforms;

	String m_newVertexShader;
	String m_newFragmentShader;

	OpenGLContext m_renderingContext;
	int m_frameCounter;

	float m_roll;
	float m_pitch;
	float m_yaw;
};
