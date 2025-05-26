#pragma once
#include <fstream>
#include <rapidjson/prettywriter.h>

namespace VORTEK_FILESYSTEM
{
	class JSONSerializer
	{
	private:
		std::fstream m_Filestream;
		rapidjson::StringBuffer m_StringBuffer;
		std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> m_pWriter;
		int m_NumObjectsStarted, m_NumArraysStarted;

	public:
		JSONSerializer(const std::string& sFilename);
		~JSONSerializer();

		bool StartDocument();
		bool EndDocument();

		bool Reset(const std::string& sFilename);

		JSONSerializer& StartNewObject(const std::string& key = "");
		JSONSerializer& EndObject();
		JSONSerializer& StartNewArray(const std::string& key);
		JSONSerializer& EndArray();

		template <typename TValue>
		JSONSerializer& AddKeyValuePair(const std::string& key, const TValue& value);

		template <>
		JSONSerializer& AddKeyValuePair(const std::string& key, const bool& value);

	};

} // namespace VORTEK_FILESYSTEM

#include "JSONSerializer.inl"