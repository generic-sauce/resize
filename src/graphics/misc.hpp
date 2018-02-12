#ifndef MISC_HPP
#define MISC_HPP

#include <GL/glew.h>
#include <iostream>

#define PRINT_GL_ERROR() \
if (GLenum err = glGetError(); err != GL_NO_ERROR) \
	std::cout << __FILE__ << " | line " << __LINE__ << " | error " << err << " | " << glewGetErrorString(err) << std::endl; \
else \
	std::cout << __FILE__ << " | line " << __LINE__ << " | no error" << std::endl

#endif
