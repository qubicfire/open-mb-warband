#ifndef _PARTIES_TEMPLATES_LOADER_H
#define _PARTIES_TEMPLATES_LOADER_H
#include "core/mb.h"
#include "core/io/file_stream_reader.h"

class PartiesTemplatesLoader final
{
public:
	bool load();
private:
	int load_descriptor(FileStreamReader& stream) const;
};

#endif // !_PARTIES_TEMPLATES_LOADER_H
