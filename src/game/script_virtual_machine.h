#ifndef _SCRIPT_VIRTUAL_MACHINE_H
#define _SCRIPT_VIRTUAL_MACHINE_H
#include "core/mb.h"
#include "core/mb_type_traits.h"
#include <unordered_map>

using ScriptMethod = void(*)(const int64_t*);

class ScriptMachine
{
public:
	enum class Status : uint8_t
	{
		None,
		Loop,
		End,
		Begin,
		Interrupt,
		Conditional,
		Call
	};

	enum Attributes : uint8_t
	{
		None = (1 << 0),
		Or = (1 << 1),
		Negative = (1 << 2),
	};
private:
	struct Command
	{
		_inline_ void invoke() const
		{
			if (m_invoke != nullptr)
				m_invoke(m_args.data());
		}

		Attributes m_attributes;
		int m_next_pc;
		ScriptMethod m_invoke;
		std::vector<int64_t> m_args; // Is it a good way to do that? Idk
	};

	struct CallFrame
	{
		_inline_ void push_command(Command&& command)
		{
			m_commands.push_back(std::move(command));
		}

		_inline_ uint64_t increment() { return m_pc++; }
		_inline_ uint64_t decrement() { return ++m_pc; }
		_inline_ uint64_t get_commands_count() const { return m_commands.size(); }

		const int64_t* m_args;
		bool m_should_increase;
		uint64_t m_pc;
		Attributes m_attributes;
		Status m_status;
		std::vector<Command> m_commands;
		std::vector<int> m_locals;
	};
public:
	bool compile();

	void call(const std::string& method, const int64_t* args = nullptr);
	void call(const int id, const int64_t* args = nullptr);
	void call_command(const Command& command);
	void range(int64_t tag, 
		const int64_t lower_bound,
		const int64_t upper_bound);
	void process_condition();

	_inline_ void try_skip_or_block(const bool condition)
	{
		// if this_or_next will return true
		if (condition && has_attribute(Attributes::Or))
			jump();
	}

	_inline_ void set_status(const Status status)
	{
		m_frame->m_status = status;
	}

	_inline_ void set_global(const int id, const int value)
	{
#ifdef _DEBUG
		if (id >= m_globals.size())
			return log_warning("\'%d\' - id variable is INVALID", id);
#endif // _DEBUG
		m_globals[id] = value;
	}

	_inline_ void set_local(const int id, const int value)
	{
#ifdef _DEBUG
		if (id >= m_frame->m_locals.size())
			return log_warning("\'%d\' - id variable is INVALID", id);
#endif // _DEBUG
		m_frame->m_locals[id] = value;
	}

	_inline_ void set_register(const int id, const int value)
	{
#ifdef _DEBUG
		if (id >= m_regs.size())
			return log_warning("\'%d\' - id register is INVALID", id);
#endif // _DEBUG
		m_regs[id] = value;
	}

	_inline_ void skip_pc_increase()
	{
		m_frame->m_should_increase = false;
	}

	_inline_ void jump()
	{ 
		m_frame->m_pc = m_frame->m_commands[m_frame->m_pc].m_next_pc;

		skip_pc_increase();
	}

	_inline_ int get_global(const int id) const 
	{ 
		return m_globals[id];
	}

	_inline_ int get_local(const int id) const 
	{ 
		return m_frame->m_locals[id];
	}

	_inline_ int get_register(const int id) const
	{
		return m_regs[id];
	}

	_inline_ Status get_status() const 
	{ 
		return m_frame->m_status; 
	}

	_inline_ bool has_attribute(const Attributes attribute) const 
	{ 
		return m_frame->m_attributes & attribute;
	}
private:
	_no_discard_ int load_descriptor(FileStreamReader& stream) const;

	void prepare_globals_objects();

	void call_frame(const CallFrame* frame, const int64_t* args = nullptr);

	int& get_reference(int64_t tag);

	static constexpr int DEFAULT_COMMANDS_CAPACITY = 8;
	static constexpr int DEFAULT_REGS_CAPACITY = 8;
private:
	CallFrame* m_frame;

	HashMap<std::string, CallFrame> m_frames;
	std::vector<int> m_regs;
	std::vector<int> m_globals;
};

// TODO: declare_global_unique_class instead of create_global_class
// because if the client connects to the server, he doesn't need to compile
// scripts. 
// THIS CLASS MUST BE CREATED ONLY IN SINGLEPLAYER, PTP, OR IF SERVER IS DEDICATED
create_global_class(ScriptMachine, scripts)

#endif // !_SCRIPT_VIRTUAL_MACHINE_H
