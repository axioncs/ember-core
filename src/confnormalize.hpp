#pragma once

#include <string>
#include <vector>

namespace ConfNormalize
{
	namespace detail
	{
		inline bool isBlankOrComment(const std::string& line)
		{
			for (char c : line)
			{
				if (c == ' ' || c == '\t') continue;
				return c == '#';
			}
			return true;
		}

		inline bool isSeqItem(const std::string& line, size_t& indent)
		{
			size_t i = 0;
			while (i < line.size() && line[i] == ' ') i++;
			if (i >= line.size() || line[i] != '-') return false;

			const size_t after = i + 1;
			if (after < line.size() && line[after] != ' ' && line[after] != '\t')
				return false;
			indent = i;
			return true;
		}

		inline std::string stripLeadingSpaces(const std::string& line)
		{
			size_t i = 0;
			while (i < line.size() && line[i] == ' ') i++;
			return line.substr(i);
		}

		inline std::vector<std::string> splitLines(const std::string& text,
		                                            bool& trailingNewline)
		{
			std::vector<std::string> lines;
			std::string cur;
			for (char c : text)
			{
				if (c == '\n')
				{
					lines.push_back(cur);
					cur.clear();
				}
				else
				{
					cur.push_back(c);
				}
			}
			trailingNewline = cur.empty() && !text.empty();
			if (!cur.empty()) lines.push_back(cur);
			return lines;
		}
	}

	inline std::string repairSeqIndent(const std::string& text,
	                                   bool* changed = nullptr)
	{
		bool trailingNewline = false;
		std::vector<std::string> lines = detail::splitLines(text, trailingNewline);

		bool didChange = false;
		const size_t n = lines.size();
		size_t i = 0;
		while (i < n)
		{
			size_t firstIndent = 0;
			if (!detail::isSeqItem(lines[i], firstIndent))
			{
				i++;
				continue;
			}

			size_t lastItem = i;
			size_t k = i + 1;
			while (k < n)
			{
				size_t ind = 0;
				if (detail::isSeqItem(lines[k], ind))
				{
					lastItem = k;
					k++;
				}
				else if (detail::isBlankOrComment(lines[k]))
				{
					k++;
				}
				else
				{
					break;
				}
			}

			const std::string pad(firstIndent, ' ');
			for (size_t m = i; m <= lastItem; m++)
			{
				size_t ind = 0;
				if (!detail::isSeqItem(lines[m], ind)) continue;
				if (ind == firstIndent) continue;
				lines[m] = pad + detail::stripLeadingSpaces(lines[m]);
				didChange = true;
			}

			i = lastItem + 1;
		}

		if (changed) *changed = didChange;
		if (!didChange) return text;

		std::string out;
		for (size_t j = 0; j < lines.size(); j++)
		{
			out += lines[j];
			if (j + 1 < lines.size() || trailingNewline) out += '\n';
		}
		return out;
	}
}
