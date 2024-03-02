#pragma once
#include <juce_core/juce_core.h>
#include <juce_opengl/juce_opengl.h>

#include "BinaryData.h"
#include "WavefrontObjParser.h"
#if JUCE_MAC
#include <OpenGL/gl.h>
#endif

inline juce::String loadEntireAssetIntoString(const char *assetName) {
  int size;
  const char *headModel = BinaryData::getNamedResource(assetName, size);

  juce::MemoryInputStream mem(headModel, size, false);

  return mem.readString();
}

class BinauralHeadView : public juce::Component, public juce::OpenGLRenderer {
 public:
  BinauralHeadView()
      : m_frameCounter(0), m_roll(0.0f), m_pitch(0.0f), m_yaw(0.0f) {
    setOpaque(true);

    m_renderingContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    m_renderingContext.setRenderer(this);
    m_renderingContext.attachTo(*this);
    m_renderingContext.setContinuousRepainting(true);
  }

  ~BinauralHeadView() override { m_renderingContext.detach(); }

  void setHeadOrientation(float roll, float pitch, float yaw) {
    m_roll = roll;
    m_pitch = -pitch;
    m_yaw = -yaw + 180.0f;
  }

 private:
  void newOpenGLContextCreated() override {
    using namespace juce::gl;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    createShaders();
  }

  void paint(juce::Graphics &g) override {}

  void renderOpenGL() override {
    using namespace juce::gl;
    jassert(juce::OpenGLHelpers::isContextActive());

    auto desktopScale = (float)m_renderingContext.getRenderingScale();

    juce::OpenGLHelpers::clear(juce::Colour::fromRGBA(209, 219, 244, 255));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, juce::roundToInt(desktopScale * getWidth()),
               juce::roundToInt(desktopScale * getHeight()));

    m_shader->use();

    if (m_uniforms->projectionMatrix.get() != nullptr)
      m_uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1,
                                               false);

    if (m_uniforms->viewMatrix.get() != nullptr)
      m_uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

    if (m_uniforms->modelMatrix.get() != nullptr)
      m_uniforms->modelMatrix->setMatrix4(getModelMatrix().mat, 1, false);

    if (m_uniforms->lightPosition.get() != nullptr)
      m_uniforms->lightPosition->set(0.0f, 10.0f, 15.0f, 0.0f);

    m_shape->draw(m_renderingContext, *m_attributes);

    // Reset the element buffers so child Components draw correctly
    m_renderingContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_renderingContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_frameCounter++;
  }

  void openGLContextClosing() override {
    m_shader.reset();
    m_shape.reset();
    m_attributes.reset();
    m_uniforms.reset();
  }

  juce::Matrix3D<float> getProjectionMatrix() const {
    auto w = 1.0f;
    auto h = w * getLocalBounds().toFloat().getAspectRatio(false);

    return juce::Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.5f, 30.0f);
  }

  juce::Matrix3D<float> getViewMatrix() const {
    return juce::Matrix3D<float>::fromTranslation({0.0f, 0.0f, -5.0f});
  }

  juce::Matrix3D<float> getModelMatrix() const {
    return juce::Matrix3D<float>::rotation(juce::Vector3D<float>(
        juce::degreesToRadians(m_pitch), juce::degreesToRadians(m_yaw),
        juce::degreesToRadians(m_roll)));
  }

  void createShaders() {
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
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * "
        "position;\n"
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

    std::unique_ptr<juce::OpenGLShaderProgram> newShader(
        new juce::OpenGLShaderProgram(m_renderingContext));
    juce::String statusText;

    if (newShader->addVertexShader(
            juce::OpenGLHelpers::translateVertexShaderToV3(m_vertexShader)) &&
        newShader->addFragmentShader(
            juce::OpenGLHelpers::translateFragmentShaderToV3(
                m_fragmentShader)) &&
        newShader->link()) {
      m_shape.reset();
      m_attributes.reset();
      m_uniforms.reset();

      m_shader.reset(newShader.release());
      m_shader->use();

      m_shape.reset(new Shape(m_renderingContext));
      m_attributes.reset(new Attributes(m_renderingContext, *m_shader));
      m_uniforms.reset(new Uniforms(m_renderingContext, *m_shader));

      statusText =
          "GLSL: v" +
          juce::String(juce::OpenGLShaderProgram::getLanguageVersion(), 2);
    } else {
      statusText = newShader->getLastError();
    }
  }

  struct Vertex {
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
  };

  // This class just manages the attributes that the shaders use.
  struct Attributes {
    Attributes(juce::OpenGLContext &openGLContext,
               juce::OpenGLShaderProgram &shaderProgram) {
      position.reset(createAttribute(openGLContext, shaderProgram, "position"));
      normal.reset(createAttribute(openGLContext, shaderProgram, "normal"));
      sourceColour.reset(
          createAttribute(openGLContext, shaderProgram, "sourceColour"));
      textureCoordIn.reset(
          createAttribute(openGLContext, shaderProgram, "textureCoordIn"));
    }

    void enable(juce::OpenGLContext &glContext) {
      using namespace juce::gl;
      if (position.get() != nullptr) {
        glContext.extensions.glVertexAttribPointer(position->attributeID, 3,
                                                   GL_FLOAT, GL_FALSE,
                                                   sizeof(Vertex), nullptr);
        glContext.extensions.glEnableVertexAttribArray(position->attributeID);
      }

      if (normal.get() != nullptr) {
        glContext.extensions.glVertexAttribPointer(
            normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid *)(sizeof(float) * 3));
        glContext.extensions.glEnableVertexAttribArray(normal->attributeID);
      }

      if (sourceColour.get() != nullptr) {
        glContext.extensions.glVertexAttribPointer(
            sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid *)(sizeof(float) * 6));
        glContext.extensions.glEnableVertexAttribArray(
            sourceColour->attributeID);
      }

      if (textureCoordIn.get() != nullptr) {
        glContext.extensions.glVertexAttribPointer(
            textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid *)(sizeof(float) * 10));
        glContext.extensions.glEnableVertexAttribArray(
            textureCoordIn->attributeID);
      }
    }

    void disable(juce::OpenGLContext &glContext) {
      if (position.get() != nullptr)
        glContext.extensions.glDisableVertexAttribArray(position->attributeID);
      if (normal.get() != nullptr)
        glContext.extensions.glDisableVertexAttribArray(normal->attributeID);
      if (sourceColour.get() != nullptr)
        glContext.extensions.glDisableVertexAttribArray(
            sourceColour->attributeID);
      if (textureCoordIn.get() != nullptr)
        glContext.extensions.glDisableVertexAttribArray(
            textureCoordIn->attributeID);
    }

    std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position, normal,
        sourceColour, textureCoordIn;

   private:
    static juce::OpenGLShaderProgram::Attribute *createAttribute(
        juce::OpenGLContext &openGLContext, juce::OpenGLShaderProgram &m_shader,
        const char *attributeName) {
      if (openGLContext.extensions.glGetAttribLocation(m_shader.getProgramID(),
                                                       attributeName) < 0)
        return nullptr;

      return new juce::OpenGLShaderProgram::Attribute(m_shader, attributeName);
    }
  };

  struct Uniforms {
    Uniforms(juce::OpenGLContext &openGLContext,
             juce::OpenGLShaderProgram &shaderProgram) {
      projectionMatrix.reset(
          createUniform(openGLContext, shaderProgram, "projectionMatrix"));
      viewMatrix.reset(
          createUniform(openGLContext, shaderProgram, "viewMatrix"));
      modelMatrix.reset(
          createUniform(openGLContext, shaderProgram, "modelMatrix"));
      lightPosition.reset(
          createUniform(openGLContext, shaderProgram, "lightPosition"));
    }

    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> projectionMatrix,
        viewMatrix, modelMatrix, lightPosition;

   private:
    static juce::OpenGLShaderProgram::Uniform *createUniform(
        juce::OpenGLContext &openGLContext,
        juce::OpenGLShaderProgram &shaderProgram, const char *uniformName) {
      if (openGLContext.extensions.glGetUniformLocation(
              shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

      return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
    }
  };

  struct Shape {
    Shape(juce::OpenGLContext &glContext) {
      if (shapeFile.load(loadEntireAssetIntoString("male_head_obj")).wasOk())
        for (auto *shapeVertices : shapeFile.shapes)
          vertexBuffers.add(new VertexBuffer(glContext, *shapeVertices));
    }

    void draw(juce::OpenGLContext &glContext, Attributes &glAttributes) {
      using namespace juce::gl;
      for (auto *vertexBuffer : vertexBuffers) {
        vertexBuffer->bind();

        glAttributes.enable(glContext);
        glDrawElements(GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT,
                       nullptr);
        glAttributes.disable(glContext);
      }
    }

   private:
    struct VertexBuffer {
      VertexBuffer(juce::OpenGLContext &context,
                   WavefrontObjFile::Shape &aShape)
          : openGLContext(context) {
        using namespace juce::gl;
        numIndices = aShape.mesh.indices.size();

        openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
        openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        juce::Array<Vertex> vertices;
        createVertexListFromMesh(aShape.mesh, vertices, juce::Colours::green);

        openGLContext.extensions.glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(vertices.size()) *
                                    sizeof(Vertex)),
            vertices.getRawDataPointer(), GL_STATIC_DRAW);

        openGLContext.extensions.glGenBuffers(1, &indexBuffer);
        openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                              indexBuffer);
        openGLContext.extensions.glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(static_cast<size_t>(numIndices) *
                                    sizeof(juce::uint32)),
            aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
      }

      ~VertexBuffer() {
        openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
        openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
      }

      void bind() {
        using namespace juce::gl;
        openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                              indexBuffer);
      }

      GLuint vertexBuffer, indexBuffer;
      int numIndices;
      juce::OpenGLContext &openGLContext;

      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VertexBuffer)
    };

    WavefrontObjFile shapeFile;
    juce::OwnedArray<VertexBuffer> vertexBuffers;

    static void createVertexListFromMesh(const WavefrontObjFile::Mesh &mesh,
                                         juce::Array<Vertex> &list,
                                         juce::Colour colour) {
      auto scale = 0.2f;
      WavefrontObjFile::TextureCoord defaultTexCoord{0.5f, 0.5f};
      WavefrontObjFile::Vertex defaultNormal{0.5f, 0.5f, 0.5f};

      for (auto i = 0; i < mesh.vertices.size(); ++i) {
        const auto &v = mesh.vertices.getReference(i);
        const auto &n = i < mesh.normals.size() ? mesh.normals.getReference(i)
                                                : defaultNormal;
        const auto &tc = i < mesh.textureCoords.size()
                             ? mesh.textureCoords.getReference(i)
                             : defaultTexCoord;

        list.add({{
                      scale * v.x,
                      scale * v.y,
                      scale * v.z,
                  },
                  {
                      scale * n.x,
                      scale * n.y,
                      scale * n.z,
                  },
                  {colour.getFloatRed(), colour.getFloatGreen(),
                   colour.getFloatBlue(), colour.getFloatAlpha()},
                  {tc.x, tc.y}});
      }
    }
  };

  const char *m_vertexShader;
  const char *m_fragmentShader;

  std::unique_ptr<juce::OpenGLShaderProgram> m_shader;
  std::unique_ptr<Shape> m_shape;
  std::unique_ptr<Attributes> m_attributes;
  std::unique_ptr<Uniforms> m_uniforms;

  juce::String m_newVertexShader;
  juce::String m_newFragmentShader;

  juce::OpenGLContext m_renderingContext;
  int m_frameCounter;

  float m_roll;
  float m_pitch;
  float m_yaw;
};
