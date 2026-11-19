// SPDX-License-Identifier: AGPL-3.0-only
//
// Protobuf wire codec for the Cloud.* RPCs. Steam's generated message classes are
// C++ objects we cannot construct on the client's behalf, so requests are read
// and responses written as raw wire bytes — the same shape CCloud_* messages use.
// Field numbers are taken from SteamDatabase/Protobufs, not guessed.
//
// LengthDelimited payloads are copied into the Field, not aliased into the input.
// An earlier version aliased them and returned fields pointing into a temporary,
// which dangled the moment the caller read a string; the copy costs a few hundred
// bytes per request and removes the whole class of lifetime bugs.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace CloudPB
{
	enum WireType : uint8_t
	{
		Varint = 0,
		Fixed64 = 1,
		LengthDelimited = 2,
		Fixed32 = 5
	};

	struct Field
	{
		uint32_t number = 0;
		WireType wireType = Varint;
		uint64_t varint = 0;		// Varint / Fixed32 / Fixed64
		std::string bytes;			// LengthDelimited

		const uint8_t* data() const
		{
			return reinterpret_cast<const uint8_t*>(bytes.data());
		}

		size_t dataLen() const
		{
			return bytes.size();
		}
	};

	// Both return false on a truncated buffer rather than reading past it: the
	// input is another process's protobuf, so it is untrusted. Parsing stops at
	// the first malformed field and returns what it had.
	bool decodeVarint(const uint8_t* buf, const size_t len, size_t& offset, uint64_t& out);

	std::vector<Field> parse(const uint8_t* buf, const size_t len);
	std::vector<Field> parse(const std::string& buf);

	// First field with this number, or nullptr.
	const Field* find(const std::vector<Field>& fields, const uint32_t number);
	std::string string(const std::vector<Field>& fields, const uint32_t number);
	uint64_t varint(const std::vector<Field>& fields, const uint32_t number, const uint64_t fallback = 0);
	std::vector<uint8_t> bytes(const std::vector<Field>& fields, const uint32_t number);

	class Writer
	{
	public:
		void writeVarint(const uint32_t number, const uint64_t value);
		void writeFixed32(const uint32_t number, const uint32_t value);
		void writeString(const uint32_t number, const std::string& value);
		void writeBytes(const uint32_t number, const uint8_t* data, const size_t len);
		void writeMessage(const uint32_t number, const Writer& nested);

		const std::vector<uint8_t>& data() const { return buffer; }
		const uint8_t* bytes() const { return buffer.data(); }
		size_t size() const { return buffer.size(); }
		void clear() { buffer.clear(); }

	private:
		void writeTag(const uint32_t number, const WireType wireType);
		void writeRawVarint(const uint64_t value);

		std::vector<uint8_t> buffer;
	};
}