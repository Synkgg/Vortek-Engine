#include "JSONSerializer.h"

namespace VORTEK_FILESYSTEM
{
	template <typename TValue>
	inline JSONSerializer& JSONSerializer::AddKeyValuePair(const std::string& key, const TValue& value)
	{
		m_pWriter->Key(key.c_str());

		if constexpr (std::is_same_v<TValue, std::string>)
		{
			m_pWriter->String(value.c_str());
		}
		else if constexpr (std::is_same_v<TValue, const char*>)
		{
			m_pWriter->String(value);
		}
		else if constexpr (std::is_same_v<TValue, const char>)
		{
			m_pWriter->String(value);
		}
		else if constexpr (std::is_integral_v<TValue>)
		{
			m_pWriter->Int64(value);
		}
		else if constexpr (std::is_unsigned_v<TValue>)
		{
			m_pWriter->Uint64(value);
		}
		else if constexpr (std::is_floating_point_v<TValue>)
		{
			m_pWriter->Double(value);
		}
		else
		{
			static_assert(false, "Type is not supported!");
		}

		return *this;
	}

	template <>
	inline JSONSerializer& JSONSerializer::AddKeyValuePair(const std::string& key, const bool& value)
	{
		m_pWriter->Key(key.c_str());
		m_pWriter->Bool(value);

		return *this;
	}
} // namespace VORTEK_FILESYSTEM