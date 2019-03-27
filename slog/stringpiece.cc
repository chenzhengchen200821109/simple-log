#include "stringpiece.h"
#include <string.h>
#include <algorithm>
#include <climits>
#include <string>
#include <ostream>

namespace internal 
{
    std::ostream& operator<<(std::ostream& o, StringPiece piece) 
    {
        o.write(piece.data(), piece.size());
        return o;
    }

    StringPiece::StringPiece(StringPiece x, size_t pos) : ptr_(x.ptr_ + pos), length_(x.length_ - pos) 
    {
        SLOG_CHECK_LE(0, pos);
        SLOG_CHECK_LE(pos, x.length_);
    }

    StringPiece::StringPiece(StringPiece x, size_t pos, size_t len) : ptr_(x.ptr_ + pos), length_(std::min(len, x.length_ - pos)) 
    {
        SLOG_CHECK_LE(0, pos);
        SLOG_CHECK_LE(pos, x.length_);
        SLOG_CHECK_GE(len, 0);
    }

    void StringPiece::CopyToString(string* target) const 
    {
        target->assign(ptr_, length_);
    }

    void StringPiece::AppendToString(string* target) const {
        target->append(ptr_, length_);
    }

    bool StringPiece::Consume(StringPiece x) 
    {
        if (starts_with(x)) {
            ptr_ += x.length_;
            length_ -= x.length_;
            return true;
        }
        return false;
    }

    bool StringPiece::ConsumeFromEnd(StringPiece x) 
    {
        if (ends_with(x)) {
            length_ -= x.length_;
            return true;
        }
        return false;
    }

    size_t StringPiece::copy(char* buf, size_type n, size_type pos) const 
    {
        size_t ret = std::min(length_ - pos, n);
        memcpy(buf, ptr_ + pos, ret);
        return ret;
    }

    bool StringPiece::contains(StringPiece s) const 
    {
        return find(s, 0) != npos;
    }

    size_t StringPiece::find(StringPiece s, size_type pos) const 
    {
        if (length_ <= 0 || pos > static_cast<size_type>(length_)) {
            if (length_ == 0 && pos == 0 && s.length_ == 0) return 0;
                return npos;
        }
        const char *result = std::search(ptr_ + pos, ptr_ + length_, s.ptr_, s.ptr_ + s.length_);
        return result == ptr_ + length_ ? npos : result - ptr_;
    }

    size_t StringPiece::find(char c, size_type pos) const 
    {
        if (length_ <= 0 || pos >= static_cast<size_type>(length_)) {
            return npos;
        }
        const char* result = static_cast<const char*>(memchr(ptr_ + pos, c, length_ - pos));
        return result != NULL ? result - ptr_ : npos;
    }

    size_t StringPiece::rfind(StringPiece s, size_type pos) const 
    {
        if (length_ < s.length_) return npos;
        const size_t ulen = length_;
        if (s.length_ == 0) return std::min(ulen, pos);

        const char* last = ptr_ + std::min(ulen - s.length_, pos) + s.length_;
        const char* result = std::find_end(ptr_, last, s.ptr_, s.ptr_ + s.length_);
        return result != last ? result - ptr_ : npos;
    }

    // Search range is [0..pos] inclusive.  If pos == npos, search everything.
    size_t StringPiece::rfind(char c, size_type pos) const 
    {
        // Note: memrchr() is not available on Windows.
        if (length_ <= 0) return npos;
        for (size_t i = std::min(pos, static_cast<size_type>(length_ - 1)); i >= 0; --i) {
            if (ptr_[i] == c) {
                return i;
            }
        }
        return npos;
    }

    // For each character in characters_wanted, sets the index corresponding
    // to the ASCII code of that character to 1 in table.  This is used by
    // the find_.*_of methods below to tell whether or not a character is in
    // the lookup table in constant time.
    // The argument `table' must be an array that is large enough to hold all
    // the possible values of an unsigned char.  Thus it should be be declared
    // as follows:
    //   bool table[UCHAR_MAX + 1]
    static inline void BuildLookupTable(StringPiece characters_wanted, bool* table) 
    {
        const size_t length = characters_wanted.length();
        const char* const data = characters_wanted.data();
        for (size_t i = 0; i < length; ++i) {
            table[static_cast<unsigned char>(data[i])] = true;
        }
    }

    size_t StringPiece::find_first_of(StringPiece s, size_type pos) const 
    {
        if (length_ <= 0 || s.length_ <= 0) {
            return npos;
        }
        // Avoid the cost of BuildLookupTable() for a single-character search.
        // invoke size_t find_first_of(char c, size_type pos = 0) const.
        if (s.length_ == 1) return find_first_of(s.ptr_[0], pos);

        bool lookup[UCHAR_MAX + 1] = { false };
        BuildLookupTable(s, lookup);
        for (size_t i = pos; i < length_; ++i) {
            if (lookup[static_cast<unsigned char>(ptr_[i])]) {
                return i;
            }
        }
        return npos;
    }

    size_t StringPiece::find_first_not_of(StringPiece s, size_type pos) const 
    {
        if (length_ <= 0) return npos;
        if (s.length_ <= 0) return 0;
        // Avoid the cost of BuildLookupTable() for a single-character search.
        if (s.length_ == 1) return find_first_not_of(s.ptr_[0], pos);

        bool lookup[UCHAR_MAX + 1] = { false };
        BuildLookupTable(s, lookup);
        for (size_t i = pos; i < length_; ++i) {
            if (!lookup[static_cast<unsigned char>(ptr_[i])]) {
                return i;
            }
        }
        return npos;
    }

    size_t StringPiece::find_first_not_of(char c, size_type pos) const 
    {
        if (length_ <= 0) return npos;

        for (; pos < static_cast<size_type>(length_); ++pos) {
            if (ptr_[pos] != c) {
                return pos;
            }
        }
        return npos;
    }

    size_t StringPiece::find_last_of(StringPiece s, size_type pos) const 
    {
        if (length_ <= 0 || s.length_ <= 0) return npos;
        // Avoid the cost of BuildLookupTable() for a single-character search.
        if (s.length_ == 1) return find_last_of(s.ptr_[0], pos);

        bool lookup[UCHAR_MAX + 1] = { false };
        BuildLookupTable(s, lookup);
        for (size_t i = std::min(pos, static_cast<size_type>(length_ - 1)); i >= 0; --i) {
            if (lookup[static_cast<unsigned char>(ptr_[i])]) {
                return i;
            }
        }
        return npos;
    }

    size_t StringPiece::find_last_not_of(StringPiece s, size_type pos) const 
    {
        if (length_ <= 0) return npos;

        size_t i = std::min(pos, static_cast<size_type>(length_ - 1));
        if (s.length_ <= 0) return i;

        // Avoid the cost of BuildLookupTable() for a single-character search.
        if (s.length_ == 1) return find_last_not_of(s.ptr_[0], pos);

        bool lookup[UCHAR_MAX + 1] = { false };
        BuildLookupTable(s, lookup);
        for (; i >= 0; --i) {
            if (!lookup[static_cast<unsigned char>(ptr_[i])]) {
                return i;
            }
        }
        return npos;
    }

    size_t StringPiece::find_last_not_of(char c, size_type pos) const 
    {
        if (length_ <= 0) return npos;

        for (size_t i = std::min(pos, static_cast<size_type>(length_ - 1)); i >= 0; --i) {
            if (ptr_[i] != c) {
                return i;
            }
        }
        return npos;
    }

    StringPiece StringPiece::substr(size_type pos, size_type n) const 
    {
        if (pos > length_) pos = length_;
        if (n > length_ - pos) n = length_ - pos;
        return StringPiece(ptr_ + pos, n);
    }

    const StringPiece::size_type StringPiece::npos = size_type(-1);

} // namespace internal
