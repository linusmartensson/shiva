#include"stdafx.h"
#include"buffers.h"
#include"shaders.h"
#include"clengine.h"
#include"shiva.h"
#include"varray.h"
#include"framebuffer.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

uint BufferBase::copyReadBuf = 0;
uint BufferBase::copyWriteBuf = 0;
uint VArray::current = 0;
uint Program::current = 0;
CLProgram::CLContext *CLProgram::context = 0;
size_t CLProgram::usage = 0;

FrameBuffer *FrameBuffer::current = 0;
std::set<int> shiva::core::downkeys;

uint shiva::core::_width = 0; 
uint shiva::core::_height = 0;
bool shiva::core::dying = false;
HGLRC shiva::core::primaryContext = NULL;
HDC shiva::core::deviceContext = NULL;
int shiva::core::wheelpos = 0;
int shiva::core::lx = 0; 
int shiva::core::ly = 0;
resourcetracker shiva::core::rt;

boost::shared_mutex shiva::behaviour::cmutex;
boost::shared_mutex shiva::state::cmutex;
shiva::threadpool *shiva::core::pool = 0;
std::set<int> shiva::core::mousekeys;
std::set<int> shiva::core::repeatkeys;
int shiva::core::currentChar;
shiva::state* shiva::core::root;
std::map<std::string, shiva::state*> shiva::state::statemap;