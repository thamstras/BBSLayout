//#include "Framebuffer.hpp"
#include "Common.hpp"
#include <atomic>

class Framebuffer
{
public:
	Framebuffer(GLuint width, GLuint height);
	void Begin();
	void End();

	GLuint GetWidth();
	GLuint GetHeight();

private:
	static bool fbInUse;
	GLuint width;
	GLuint height;
	GLuint FBO;
	GLuint texColorBuffer;
	GLuint RBO;

};

Framebuffer::Framebuffer(GLuint width, GLuint height)
{
	this->width = width;
	this->height = height;

	GLint old_fbo;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);

}

void Framebuffer::Begin()
{

}