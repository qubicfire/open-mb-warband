#include "file_stream_reader.h"

FileStreamReader::FileStreamReader() noexcept
	: m_offset(0)
{ }

FileStreamReader::FileStreamReader(const FileStreamReader& stream, bool is_async) noexcept
	: m_data(stream.m_data)
	, m_offset(stream.m_offset)
	, m_is_async(is_async)
{ }

FileStreamReader::~FileStreamReader()
{
	if (!m_is_async)
		delete[] m_data;
}

template <class _Tx>
struct FileMappingDelete {};

template <class _Tx>
struct FileMappingDelete<_Tx[]>
{
	constexpr FileMappingDelete() noexcept = default;

	template <class _Uty, 
		std::enable_if_t<std::is_convertible_v<_Uty(*)[], _Tx(*)[]>, int> = 0>
	inline FileMappingDelete(const FileMappingDelete<_Uty[]>&) noexcept {}

	template <class _Uty, 
		std::enable_if_t<std::is_convertible_v<_Uty(*)[], _Tx(*)[]>, int> = 0>
	inline void operator()(_Uty* pointer) const noexcept
	{
		static_assert(0 < sizeof(_Uty), "can't delete an incomplete type");
#ifdef _WIN32
		UnmapViewOfFile(pointer);
#endif
	}
};

size_t FileStreamReader::open(const std::string& path)
{
	std::ifstream file_stream(path, std::ios::binary | std::ios::in | std::ios::ate);

	if (!file_stream)
		return 0;

	/*HANDLE file = CreateFileA(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	HANDLE mapping = CreateFileMappingA(
		file,
		nullptr,
		PAGE_READONLY,
		0,
		0,
		nullptr
	);
	void* data = MapViewOfFile(
		mapping,
		FILE_MAP_READ,
		0,
		0,
		0
	);
	m_data.reset(
		static_cast<uint8_t*>(data)
	);*/

	const size_t length = file_stream.tellg();
	file_stream.seekg(0, std::ios::beg);

	//m_data = create_shared<uint8_t[]>(length);
	//file_stream.read(reinterpret_cast<char*>(m_data.get()), length);

	m_data = new uint8_t[length];
	file_stream.read(reinterpret_cast<char*>(m_data), length);

    return length;
}
