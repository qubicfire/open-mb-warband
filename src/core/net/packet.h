#ifndef _PACKET_H
#define _PACKET_H
#include "core/mb_type_traits.h"

struct Packet
{
	uint32_t m_id; // Entity's id
};

template <class _Tx, class _Dx>
static inline _Tx cast_packet(_Dx& message) noexcept
{
	return static_cast<_Tx&>(message);
}

template <class _Tx, class _Dx>
static inline _Tx cast_packet(const _Dx& message) noexcept
{
	return static_cast<const _Tx&>(message);
}

enum class NetworkObjectState : int8_t
{
	None = (1 << 0),
	Changed = (1 << 1),
};

struct NetworkListener
{
	virtual Packet& server_send_packet() { }
	virtual Packet& client_send_packet() { }

	virtual void server_receive_packet(const Packet&) { }
	virtual void client_receive_packet(const Packet&) { }

	virtual bool has_network_state_changed() const
	{ 
		return m_network_state == NetworkObjectState::Changed;
	}

	NetworkObjectState m_network_state = NetworkObjectState::None;
};

template <class _Tx>
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
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator+=(const _Dx& other)
	{
		m_field += other;
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator-=(const _Dx& other)
	{
		m_field -= other;
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator*=(const _Dx& other)
	{
		m_field *= other;
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	template <class _Dx>
	inline NetworkField& operator/=(const _Dx& other)
	{
		m_field /= other;
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	inline NetworkField& operator++()
	{
		m_field++;
		m_listener->m_network_state = NetworkObjectState::Changed;
		return *this;
	}

	inline NetworkField& operator--()
	{
		m_field--;
		m_listener->m_network_state = NetworkObjectState::Changed;
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
