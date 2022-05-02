#include "rwbase.h"

namespace rw {

struct Material
{
	PLUGINBASE
	Texture *texture;
	RGBA color;
	SurfaceProperties surfaceProps;
	Pipeline *pipeline;
	int32 refCount;

	static int32 numAllocated;

	static Material *create(void);
	void addRef(void) { this->refCount++; }
	Material *clone(void);
	void destroy(void);
	void setTexture(Texture *tex);
	static Material *streamRead(Stream *stream);
	bool streamWrite(Stream *stream);
	uint32 streamGetSize(void);
};

struct InstanceDataHeader
{
	uint32 platform;
};

}