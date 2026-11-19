// SPDX-License-Identifier: AGPL-3.0-only

#include "cloudpb.hpp"

namespace
{
	constexpr size_t kMaxFields = 1024;	// A malformed request must not make us allocate wildly.
}

namespace CloudPB
{
	bool decodeVarint(const uint8_t* buf, const size_t len, size_t& offset, uint64_t& out)
	{
		out = 0;

		for (unsigned i = 0; i < 10; ++i)
		{
			if (offset >= len)
			{
				return false;
			}

			const uint8_t byte = buf[offset++];
			out |= static_cast<uint64_t>(byte & 0x7f) << (i * 7);

			if ((byte & 0x80) == 0)
			{
				return true;
			}
		}

		return false;	// More than 10 bytes: not a valid varint.
	}

	std::vector<Field> parse(const uint8_t* buf, const size_t len)
	{
		std::vector<Field> fields;

		if (buf == nullptr)
		{
			return fields;
		}

		size_t offset = 0;

		while (offset < len && fields.size() < kMaxFields)
		{
			uint64_t tag = 0;
			if (!decodeVarint(buf, len, offset, tag))
			{
				break;
			}

			const uint32_t number = static_cast<uint32_t>(tag >> 3);
			const uint32_t wire = static_cast<uint32_t>(tag & 0x7);

			if (number == 0)
			{
				break;	// Field 0 is invalid; treat as end.
			}

			Field field;
			field.number = number;

			switch (wire)
			{
				case Varint:
					field.wireType = Varint;
					if (!decodeVarint(buf, len, offset, field.varint))
					{
						return fields;
					}
					break;

				case Fixed64:
					field.wireType = Fixed64;
					if (len - offset < 8)
					{
						return fields;
					}
					field.varint = 0;
					for (int i = 0; i < 8; ++i)
					{
						field.varint |= static_cast<uint64_t>(buf[offset + i]) << (i * 8);
					}
					offset += 8;
					break;

				case LengthDelimited:
				{
					field.wireType = LengthDelimited;
					uint64_t size = 0;
					if (!decodeVarint(buf, len, offset, size))
					{
						return fields;
					}
					if (size > len - offset)
					{
						return fields;	// Truncated: stop here, keep what parsed.
					}
					field.bytes.assign(reinterpret_cast<const char*>(buf + offset), static_cast<size_t>(size));
					offset += static_cast<size_t>(size);
					break;
				}

				case Fixed32:
					field.wireType = Fixed32;
					if (len - offset < 4)
					{
						return fields;
					}
					field.varint = 0;
					for (int i = 0; i < 4; ++i)
					{
						field.varint |= static_cast<uint64_t>(buf[offset + i]) << (i * 8);
					}
					offset += 4;
					break;

				default:
					// Groups (3, 4) are not used by any Cloud message. Bail.
					return fields;
			}

			fields.push_back(field);
		}

		return fields;
	}

	std::vector<Field> parse(const std::string& buf)
	{
		return parse(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());
	}

	const Field* find(const std::vector<Field>& fields, const uint32_t number)
	{
		for (const auto& field : fields)
		{
			if (field.number == number)
			{
				return &field;
			}
		}

		return nullptr;
	}

	std::string string(const std::vector<Field>& fields, const uint32_t number)
	{
		const auto* field = find(fields, number);
		if (!field || field->wireType != LengthDelimited)
		{
			return {};
		}

		return field->bytes;
	}

	uint64_t varint(const std::vector<Field>& fields, const uint32_t number, const uint64_t fallback)
	{
		const auto* field = find(fields, number);
		if (!field)
		{
			return fallback;
		}

		if (field->wireType == Varint || field->wireType == Fixed32 || field->wireType == Fixed64)
		{
			return field->varint;
		}

		return fallback;
	}

	std::vector<uint8_t> bytes(const std::vector<Field>& fields, const uint32_t number)
	{
		const auto* field = find(fields, number);
		if (!field || field->wireType != LengthDelimited)
		{
			return {};
		}

		const auto* p = reinterpret_cast<const uint8_t*>(field->bytes.data());
		return std::vector<uint8_t>(p, p + field->bytes.size());
	}

	void Writer::writeTag(const uint32_t number, const WireType wireType)
	{
		writeRawVarint((static_cast<uint64_t>(number) << 3) | static_cast<uint64_t>(wireType));
	}

	void Writer::writeRawVarint(const uint64_t value)
	{
		uint64_t remaining = value;

		while (remaining >= 0x80)
		{
			buffer.push_back(static_cast<uint8_t>(remaining) | 0x80);
			remaining >>= 7;
		}

		buffer.push_back(static_cast<uint8_t>(remaining));
	}

	void Writer::writeVarint(const uint32_t number, const uint64_t value)
	{
		writeTag(number, Varint);
		writeRawVarint(value);
	}

	void Writer::writeFixed32(const uint32_t number, const uint32_t value)
	{
		writeTag(number, Fixed32);

		for (int i = 0; i < 4; ++i)
		{
			buffer.push_back(static_cast<uint8_t>(value >> (i * 8)));
		}
	}

	void Writer::writeString(const uint32_t number, const std::string& value)
	{
		writeTag(number, LengthDelimited);
		writeRawVarint(value.size());
		buffer.insert(buffer.end(), value.begin(), value.end());
	}

	void Writer::writeBytes(const uint32_t number, const uint8_t* data, const size_t len)
	{
		writeTag(number, LengthDelimited);
		writeRawVarint(len);
		buffer.insert(buffer.end(), data, data + len);
	}

	void Writer::writeMessage(const uint32_t number, const Writer& nested)
	{
		writeTag(number, LengthDelimited);
		writeRawVarint(nested.size());
		buffer.insert(buffer.end(), nested.data().begin(), nested.data().end());
	}
}