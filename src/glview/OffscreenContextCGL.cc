#include "OffscreenContextCGL.h"

#include <iostream>

#include "system-gl.h"
#include <OpenGL/OpenGL.h>

class OffscreenContextCGL : public OffscreenContext {

public:
  OffscreenContextCGL(int width, int height) : OffscreenContext(width, height) {}
  ~OffscreenContextCGL() {
    // FIXME: Implement
  }

  // FIXME: What info are we really interested in here?
  std::string getInfo() const override {
    std::ostringstream out;
    out << "GL context creator: CGL (new)\n"
	<< "PNG generator: Core Foundation\n";
    return out.str();
  }

  bool makeCurrent() const override {
    return CGLSetCurrentContext(this->cglContext) == kCGLNoError;
  }

  CGLContextObj cglContext = nullptr;
};

std::shared_ptr<OffscreenContext> CreateOffscreenContextCGL(size_t width, size_t height,
							    size_t majorGLVersion, size_t minorGLVersion)
{
  auto ctx = std::make_shared<OffscreenContextCGL>(width, height);

  CGLOpenGLProfile glVersion = kCGLOGLPVersion_Legacy;
  if (majorGLVersion >= 4) glVersion = kCGLOGLPVersion_GL4_Core;
  else if (majorGLVersion >= 3) glVersion = kCGLOGLPVersion_GL3_Core;

  CGLPixelFormatAttribute attributes[13] = {
    kCGLPFAOpenGLProfile,
    (CGLPixelFormatAttribute)glVersion,
    kCGLPFAAccelerated,
    kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
    kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
    kCGLPFADoubleBuffer,
    kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
    kCGLPFASamples,  (CGLPixelFormatAttribute)4,
    (CGLPixelFormatAttribute) 0
  };
  CGLPixelFormatObj pixelFormat = NULL;
  GLint numPixelFormats = 0;
  if (CGLChoosePixelFormat(attributes, &pixelFormat, &numPixelFormats) != kCGLNoError) {
    std::cerr << "CGLChoosePixelFormat() failed" << std::endl;
    return nullptr;
  }
  CGLCreateContext(pixelFormat, NULL, &ctx->cglContext);
  CGLDestroyPixelFormat(pixelFormat); // or CGLReleasePixelFormat()

  return ctx;
}
