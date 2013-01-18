#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <sstream>

namespace BWPack
{
	namespace IO
	{
		class StreamReader
		{
			std::ifstream mInput;

		public:
			StreamReader(const std::string& fname);
			~StreamReader();

			template<typename T>
			T get()
			{
				T buf;
				mInput.read(reinterpret_cast<char*>(&buf), sizeof(buf));
				return buf;
			}

			std::string getString(int len);
			std::string getNullTerminatedString();
		};

		class StreamBufWriter
		{
			std::ostream mOutput;

		public:
			StreamBufWriter(std::stringbuf* buf) : mOutput(buf) {};

			template<typename T>
			void put(const T& buf)
			{
				mOutput.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
			}

			void putString(const std::string& str, bool nullTerminate=true);
		};
	}
}
