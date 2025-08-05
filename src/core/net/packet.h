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
	virtual Packet& server_build_packet() { }
	virtual Packet& client_build_message() { }

	virtual void server_receive_message(const Packet&) { }
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

#define begin_server_message(type)								\
public:															\
	struct Message_##type : Message								\
	{															\
		using MessageProvider = type;							\
		void build_message(MessageStorage& storage) {			\

#define store_info(field)										\
	storage.append(cast_offset_field(MessageProvider, field),	\
		cast_size_field(MessageProvider, field));				\

#define end_server_message()									\
		}														\
	};															\

#endif // !_PACKET_H
