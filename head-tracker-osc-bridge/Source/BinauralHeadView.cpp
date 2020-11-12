#include "BinauralHeadView.h"

BinauralHeadView::BinauralHeadView()
	: m_frameCounter(0)
	, m_roll(0.0f)
	, m_pitch(0.0f)
	, m_yaw(0.0f)
{
	setOpaque(true);

	m_renderingContext.setRenderer(this);
	m_renderingContext.attachTo(*this);
	m_renderingContext.setContinuousRepainting(true);
}

void BinauralHeadView::init()
{

}

void BinauralHeadView::deinit()
{

}

void BinauralHeadView::setHeadOrientation(float roll, float pitch, float yaw)
{
	//m_roll = -roll;
	//m_pitch = -pitch;
	//m_yaw = yaw;
	m_roll = roll;
	m_pitch = -pitch;
	m_yaw = -yaw + 180.0f;
}

void BinauralHeadView::newOpenGLContextCreated()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	createShaders();
}

void BinauralHeadView::paint(Graphics& g)
{
	
}

void BinauralHeadView::renderOpenGL()
{
	jassert(OpenGLHelpers::isContextActive());

	auto desktopScale = (float)m_renderingContext.getRenderingScale();

	OpenGLHelpers::clear(Colour::fromRGBA(209, 219, 244, 255));
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, roundToInt(desktopScale * getWidth()), roundToInt(desktopScale * getHeight()));

	m_shader->use();

	if (m_uniforms->projectionMatrix.get() != nullptr)
		m_uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);

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

void BinauralHeadView::openGLContextClosing()
{
	m_shader.reset();
	m_shape.reset();
	m_attributes.reset();
	m_uniforms.reset();
}
