#include "core/io/file_stream_reader.h"
#include "utils/profiler.h"

#include "script_virtual_machine.h"

namespace operators
{
	constexpr inline int try_end(3);
	constexpr inline int else_try(5);
	constexpr inline int try_begin(4);
	constexpr inline int try_for_range(6);
	constexpr inline int try_for_range_backwards(7);
	constexpr inline int try_for_parties(11);
	constexpr inline int try_for_agents(12);
	constexpr inline int try_for_prop_instances(16);
	constexpr inline int try_for_players(17);
}

namespace scripts
{
	enum : int
	{
		reg = 1,
		global, string, item, troop,
		faction, quest,
		party_template, party,
		scene, mission_template, menu, procedure,
		particle, scene_prop, sound,
		var, icon, skill, mesh,
		presentation, quick_string,
		track, tableau, animation
	};

	_inline_ int extract_tag(int64_t& tag)
	{
		int result = (tag & 0xFF00000000000000) >> 56;
		tag = static_cast<int>(tag);
		return result;
	}

	_inline_ bool is_argument_common(const int64_t tag)
	{
		return tag > 0x00FFFFFFFFFFFFFF;
	}

	_inline_ void get_argument_value(int64_t& source)
	{
		if (is_argument_common(source))
		{
			switch (extract_tag(source))
			{
			case scripts::global:
				source = g_scripts->get_global(source); break;
			case scripts::var:
				source = g_scripts->get_local(source); break;
			case scripts::reg:
				source = g_scripts->get_register(source); break;
			}
		}
	}

	_inline_ void set_destination(const int tag,
		const int64_t destination, 
		const int64_t source)
	{
		switch (tag)
		{
		case scripts::global:
			g_scripts->set_global(destination, source); break;
		case scripts::var:
			g_scripts->set_local(destination, source); break;
		case scripts::reg:
			g_scripts->set_register(destination, source); break;
		}
	}
}

namespace attributes
{
	enum : uint64_t
	{
		this_or_next = 0x40000000,
		neg = 0x80000000
	};
}

_inline_ static void check_condition_with_attributes(const bool condition)
{
	bool negative = g_scripts->has_attribute(ScriptMachine::Attributes::Negative);
	bool this_or_next = g_scripts->has_attribute(ScriptMachine::Attributes::Or);

	if ((!condition && negative) || (!this_or_next && !condition))
		g_scripts->set_status(ScriptMachine::Status::Interrupt);
	else
		g_scripts->try_skip_or_block(condition);
}

_implemented_ static void call_script(const int64_t*);

_implemented_ static void try_begin(const int64_t*);
_implemented_ static void else_try(const int64_t*);
_implemented_ static void try_end(const int64_t*);
_implemented_ static void try_for_range(const int64_t*);

_implemented_ static void gt(const int64_t*);
_implemented_ static void ge(const int64_t*);
_implemented_ static void eq(const int64_t*);
_implemented_ static void is_between(const int64_t*);

_implemented_ static void faction_set_slot(const int64_t*);

_implemented_ static void assign(const int64_t*);

_implemented_ static void store_add(const int64_t*);
_implemented_ static void store_sub(const int64_t*);
_implemented_ static void store_mul(const int64_t*);
_implemented_ static void store_div(const int64_t*);
_implemented_ static void store_mod(const int64_t*);

_implemented_ static void val_add(const int64_t*);
_implemented_ static void val_sub(const int64_t*);
_implemented_ static void val_mul(const int64_t*);
_implemented_ static void val_div(const int64_t*);
_implemented_ static void val_mod(const int64_t*);
_implemented_ static void val_min(const int64_t*);
_implemented_ static void val_max(const int64_t*);
_implemented_ static void val_clamp(const int64_t*);
_implemented_ static void val_abs(const int64_t*);

static const HashMap<int, ScriptMethod> script_methods =
{
	{ 1, call_script },
	{ 4, try_begin },
	{ 5, else_try },
	{ 3, try_end },
	{ 6, try_for_range },
	{ 32, gt },
	{ 30, ge },
	{ 31, eq },
	{ 33, is_between },
	{ 2133, assign },
	{ 2120, store_add },
	{ 2121, store_sub },
	{ 2122, store_mul },
	{ 2123, store_div },
	{ 2119, store_mod },
	{ 2105, val_add },
	{ 2106, val_sub },
	{ 2107, val_mul },
	{ 2108, val_div },
	{ 2109, val_mod },
	{ 2110, val_min },
	{ 2111, val_max },
	{ 2112, val_clamp },
	{ 2113, val_abs },
	{ 502, faction_set_slot }
};

bool ScriptMachine::compile()
{
	// I tried to make compiling scripts with multithreading,
	// but the file structure of scripts.txt is so fucked up,
	// that I have no idea how it supposed to work.
	// Previously, I rewrote FileStreamReader a bit
	// to make it work like an async reader, buuuuuuuuuuuut
	// HERE'S the main problem. FILE STRUCTURE.
	// ...
	// [method name] -1 <---- Always '-1' after method name. What does it mean?
	// : [total lines] [command id] [args] [command id] [args]
	// [some method name again] -1
	// : [total lines] [command id] [args] [command id] [args]
	// etc.
	// Did you see the problem here? How am I supposed to jump over those lines?
	// Like, why couldn't the method name and all additional stuff
	// be in a single line (kinda like a file header)
	// Is this even possible to implement with threads?
	prepare_globals_objects();

	FileStreamReader stream {};
	if (!stream.open("test/scripts.txt"))
		return false;

	const int methods = load_descriptor(stream);

	std::vector<int> next_commands {};
	next_commands.reserve(DEFAULT_COMMANDS_CAPACITY);
	m_frames.reserve(methods);

	log_print("Start scripts compiling");
	profiler_start();

	for (int i = 0; i < methods; i++)
	{
		const auto& name = stream.read_until(' ', '\n');
		const auto unused = stream.read<std::string_view>();
		const int length = stream.number_from_chars<int>();

		Attributes previous_attributes = Attributes::None;
		int optimized_attributes = 0;
		bool should_optimize_attributes = false;
		int used_locals = 0;
		bool used_locals_once = false;
		CallFrame frame {}; 

		for (int _ = 0; _ < length; _++)
		{
			int id = stream.number_from_chars<int>(); // operator id

			std::vector<int64_t> args {};
			args.resize(
				stream.number_from_chars<int>()
			);

			for (auto& argument : args)
			{
				argument = stream.number_from_chars<int64_t>();
				int type = (argument & 0xFF00000000000000) >> 56;

				if (type == scripts::var)
				{
					used_locals = std::max(static_cast<int>(argument), used_locals);
					used_locals_once = true;
				}
			}

			// Fucking hardcode
			const auto is_id_start_loop = [](const int id) -> bool {
				return id == operators::try_begin 
					|| id == operators::try_for_range 
					|| id == operators::try_for_range_backwards 
					|| id == operators::try_for_parties
					|| id == operators::try_for_agents 
					|| id == operators::try_for_prop_instances
					|| id == operators::try_for_players;
			};

			const int current_command_id = static_cast<int>(
				frame.get_commands_count()
			);

			if (should_optimize_attributes)
			{
				for (int i = optimized_attributes - 1; i >= 0; i--)
				{
					const int& back = next_commands.back();

					frame.m_commands[back].m_next_pc = current_command_id;

					next_commands.pop_back();
				}

				should_optimize_attributes = false;
				optimized_attributes = 0;
			}

			// TODO: Optimize this_or_next attribute
			// this_or_next condition should also use m_next_pc
			Attributes attributes = Attributes::None;
			if (id & attributes::neg)
			{
				attributes = Attributes::Negative;
				id ^= attributes::neg;
			}
			else if (id & attributes::this_or_next)
			{
				attributes = Attributes::Or;
				id ^= attributes::this_or_next;
				previous_attributes = attributes;
				optimized_attributes++;

				next_commands.push_back(current_command_id);
			}
			else if (previous_attributes & Attributes::Or)
			{
				previous_attributes = Attributes::None;
				should_optimize_attributes = true;
			}

			if (is_id_start_loop(id))
			{
				next_commands.push_back(current_command_id);
			}
			else if (id == operators::try_end)
			{
				const int& back = next_commands.back();

				frame.m_commands[back].m_next_pc = current_command_id;

				next_commands.pop_back();
			}
			else if (id == operators::else_try)
			{
				int& back = next_commands.back();

				frame.m_commands[back].m_next_pc = current_command_id;
				back = current_command_id;
			}

			const auto& callable = script_methods.find(id);
			ScriptMethod invoke = nullptr;

			/*if (callable == script_methods.end())
				log_warning("%d - operator command is not implemented", id);
			else
				invoke = callable->second;*/

			invoke = callable->second;

			Command command { attributes, 0, invoke, std::move(args) };
			frame.push_command(std::move(command));
		}

		if (used_locals_once && used_locals == 0)
			frame.m_locals.push_back(0);
		else
			frame.m_locals.resize(used_locals);

		next_commands.clear();
		m_frames.emplace(name, std::move(frame));
	}

	return true;
}

void ScriptMachine::call(const std::string& method, const int64_t* args)
{
	const auto& it = m_frames.find(method);

	if (it == m_frames.end())
		return log_alert("Failed to call \'%s\'. Method doesn't exist", method.c_str());

	call_frame(&it->second, args);
}

void ScriptMachine::call(const int id, const int64_t* args)
{
	const auto& it = std::next(m_frames.begin(), id);

	if (it == m_frames.end())
		return log_alert("Failed to call \'%d\'. Method doesn't exist", id);

	call_frame(&it->second, args);
}

void ScriptMachine::call_command(const Command& command)
{
	m_frame->m_attributes = command.m_attributes;

	command.invoke();

	if (m_frame->m_should_increase)
		m_frame->increment();
	else
		m_frame->m_should_increase = true;
}

void ScriptMachine::range(int64_t tag,
	const int64_t lower_bound,
	const int64_t upper_bound)
{
	set_status(ScriptMachine::Status::Loop);

	uint64_t start = m_frame->decrement();
	int& destination = get_reference(tag);
	
	for (destination = lower_bound; destination < upper_bound; destination++)
	{
		while (m_frame->m_status != Status::End &&
			m_frame->m_status != Status::Interrupt)
		{
			const Command& command = m_frame->m_commands[m_frame->m_pc];

			call_command(command);
		}

		if (destination + 1 < upper_bound)
		{
			m_frame->m_status = Status::Loop;
			m_frame->m_pc = start;
		}
	}

	// Fucking hack.
	// try_end command calls pointer to increment
	// and that's okay, but it happens inside of try_for_range command
	// so it's also increment the pointer and we skips next
	// command after try_end because of that

	// It seems that I'm supposed to add a save check
	// after command call, but it would be a good solution?
	// Maybe there's a normal way of doing that
	//m_pointer--; 

	avoid_increase_once();
}

void ScriptMachine::process_condition()
{
	uint64_t next_pc = m_frame->m_commands[m_frame->m_pc++].m_next_pc;
	m_frame->m_status = Status::Begin;

	while (m_frame->m_status != Status::End)
	{
		const Command& command = m_frame->m_commands[m_frame->m_pc];

		call_command(command);

		if (m_frame->m_status == Status::Interrupt)
		{
			m_frame->m_pc = next_pc;
			next_pc = m_frame->m_commands[next_pc].m_next_pc;
		}
	}

	avoid_increase_once();
}

int ScriptMachine::load_descriptor(FileStreamReader& stream) const
{
	const auto file_id = stream.read<std::string_view>();
	const auto version_word = stream.read<std::string_view>();
	const auto version = stream.read<std::string_view>();

	if (file_id != "scriptsfile" || version_word != "version" || version != "1")
		return 0;

	return stream.number_from_chars<int>();
}

void ScriptMachine::prepare_globals_objects()
{
	// TODO: The engine shouldn't have known that file even exist
	// So remove that mess up later
	std::ifstream stream("test/variables.txt");
	const auto count = std::count_if(std::istreambuf_iterator<char>{stream}, {},
		[](const char c) -> bool
		{
			return c == '\n';
		}
	);

	m_globals.resize(count);
	m_regs.resize(DEFAULT_REGS_CAPACITY);
}

void ScriptMachine::call_frame(const CallFrame* frame, const int64_t* args)
{
	CallFrame* backup_frame = m_frame;

	m_frame = const_cast<CallFrame*>(frame);
	m_frame->m_pc = 0;
	m_frame->m_status = Status::Call;
	m_frame->m_should_increase = true;
	m_frame->m_args = args;

	while (m_frame->m_pc < m_frame->m_commands.size())
		call_command(m_frame->m_commands[m_frame->m_pc]);

	m_frame = backup_frame;
}

int& ScriptMachine::get_reference(int64_t tag)
{
	int type = scripts::extract_tag(tag);

	switch (type)
	{
		case scripts::global: return m_globals[tag];
		case scripts::reg: return m_regs[tag];
		case scripts::var: return m_frame->m_locals[tag];
		default: return m_globals[tag];
	}
}

static void call_script(const int64_t* args)
{
	int64_t id = args[0];

	g_scripts->call(static_cast<int>(id), args + 1);
}

static void try_begin(const int64_t* args)
{
	g_scripts->process_condition();
}

static void else_try(const int64_t* args)
{
	// If we get to the else_try command without being
	// interrupted by conditional operators, then
	// we should jump to the next command and avoid pointer increasing.

	// process_condition jumps over conditional block when gets interrupt
	// so no need additional checks

	if (g_scripts->get_status() != ScriptMachine::Status::Interrupt)
		g_scripts->jump();
	else
		g_scripts->set_status(ScriptMachine::Status::Begin);
}

static void try_end(const int64_t* args)
{
	g_scripts->set_status(ScriptMachine::Status::End);
}

static void try_for_range(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t lower_bound = args[1];
	int64_t upper_bound = args[2];

	scripts::get_argument_value(lower_bound);
	scripts::get_argument_value(upper_bound);

	g_scripts->range(destination, lower_bound, upper_bound);
}

static void gt(const int64_t* args)
{
	int64_t source_a = args[0];
	int64_t source_b = args[1];

	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	check_condition_with_attributes(source_a > source_b);
}

static void ge(const int64_t* args)
{
	int64_t source_a = args[0];
	int64_t source_b = args[1];

	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	check_condition_with_attributes(source_a >= source_b);
}

static void eq(const int64_t* args)
{
	int64_t source_a = args[0];
	int64_t source_b = args[1];

	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	check_condition_with_attributes(source_a == source_b);
}

static void is_between(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t lower_bound = args[1];
	int64_t upper_bound = args[2];

	scripts::get_argument_value(destination);
	scripts::get_argument_value(lower_bound);
	scripts::get_argument_value(upper_bound);

	check_condition_with_attributes(lower_bound <= destination
		&& destination < upper_bound);
}

static void faction_set_slot(const int64_t* args)
{
	int64_t tag = args[0];
	int64_t slot_no = args[1];
	int64_t value = args[2];
}

static void assign(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);

	scripts::set_destination(tag, destination, source);
}

static void store_add(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	scripts::set_destination(tag, destination, source_a + source_b);
}

static void store_sub(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	scripts::set_destination(tag, destination, source_a - source_b);
}

static void store_mul(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	scripts::set_destination(tag, destination, source_a * source_b);
}

static void store_div(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	scripts::set_destination(tag, destination, source_a / source_b);
}

static void store_mod(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);

	scripts::set_destination(tag, destination, source_a % source_b);
}

static void val_add(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, value + source);
}

static void val_sub(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, value - source);
}

static void val_mul(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, value * source);
}

static void val_div(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, value / source);
}

static void val_mod(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, value % source);
}

static void val_min(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, std::min(value, source));
}

static void val_max(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source = args[1];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, std::max(value, source));
}

static void val_clamp(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;
	int64_t source_a = args[1];
	int64_t source_b = args[2];

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(source_a);
	scripts::get_argument_value(source_b);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, std::clamp(value, source_a, source_b));
}

static void val_abs(const int64_t* args)
{
	int64_t destination = args[0];
	int64_t value = destination;

	int tag = scripts::extract_tag(destination);
	scripts::get_argument_value(value);

	scripts::set_destination(tag, destination, std::abs(value));
}