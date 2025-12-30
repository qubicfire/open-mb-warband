#ifndef _MODULE_CONFIG_H
#define _MODULE_CONFIG_H
#include "core/mb.h"
#include "core/mb_type_traits.h"

class ModuleConfig
{
public:
	void initialize();

	template <class _Tx>
	_Tx get(const std::string& key)
	{
		const auto& it = m_properties.find(key);

		return it != m_properties.end() ? static_cast<_Tx>(it->second) : static_cast<_Tx>(0);
	}
private:
	mtd::hash_map<std::string, float> m_properties;
};

declare_global_unique_class(ModuleConfig, module_config)

#endif // !_MODULE_CONFIG_H
