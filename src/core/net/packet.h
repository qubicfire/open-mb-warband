#ifndef _PACKET_H
#define _PACKET_H
#include "core/mb_type_traits.h"

#include <enet/enet.h>

struct Packet
{
	int8_t m_id;

	template <class _Tx, 
		std::enable_if_t<std::is_enum_v<_Tx>, int> = 0>
	inline void set_id(const _Tx type)
	{
		m_id = static_cast<int8_t>(type);
	}
};

enum class ServerPackets : int
{
	Ping,
	Message,
	Object, // object update
	Scene, // change scene
	Kicked,
};

enum class ClientPackets : int
{
	None,
};

template <class _Tx, class _Dx,
	std::enable_if_t<!std::is_same_v<_Dx, uint8_t*>, int> = 0>
inline _Tx cast_packet(_Dx& message) noexcept
{
	return static_cast<_Tx&>(message);
}

template <class _Tx, class _Dx,
	std::enable_if_t<!std::is_same_v<_Dx, uint8_t*>, int> = 0>
inline _Tx cast_packet(const _Dx& message) noexcept
{
	return static_cast<const _Tx&>(message);
}

template <class _Tx>
inline _Tx cast_packet(uint8_t* packet_info)
{
	_Tx packet {};

	std::memcpy(&packet, packet_info, sizeof(_Tx));

	return packet;
}

enum class NetworkObjectState : int8_t
{
	None = (1 << 0),
	Changed = (1 << 1),
};

struct NetworkListener
{
	virtual void server_send_packet() { }
	virtual void client_send_packet() { }

	virtual void server_receive_packet(uint8_t*) {}
	virtual void client_receive_packet(uint8_t*) { }

	bool has_network_state_changed() const
	{ 
		return m_network_state > 0;
	}

	int m_network_state;
};

template <class _Tx, auto _Ky>
class NetworkField
{
public:
	inline NetworkField() = default;

	inline NetworkField(NetworkListener* listener) noexcept
		: m_listener(listener)
	{ }

	inline void set(NetworkListener* listener) noexcept
	{ 
		m_listener = listener; 
	}

	inline NetworkField& operator=(const _Tx& other)
	{
		m_field = other;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator+=(const _Dx& other)
	{
		m_field += other;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator-=(const _Dx& other)
	{
		m_field -= other;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator*=(const _Dx& other)
	{
		m_field *= other;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator/=(const _Dx& other)
	{
		m_field /= other;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	inline NetworkField& operator++()
	{
		m_field++;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	inline NetworkField& operator--()
	{
		m_field--;
		m_listener->m_network_state = static_cast<int>(_Ky);
		return *this;
	}

	template <class _Dx>
	inline bool operator==(const _Dx& other) const
	{
		return m_field == other;
	}

	template <class _Dx>
	inline bool operator!=(const _Dx& other) const
	{
		return m_field != other;
	}
private:
	NetworkListener* m_listener = nullptr;
	_Tx m_field;
};

#endif // !_PACKET_H
