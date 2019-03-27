#ifndef STRINGPIECE_H
#define STRINGPIECE_H

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <iosfwd>
#include <limits>
#include <string>
#include "logging.h"

namespace internal
{
    using std::string;

    class StringPiece 
    {
        private:
            const char* ptr_;
            size_t length_;
            static void LogFatalSizeTooBig(size_t size, const char* details)
            {
                SLOG(FATAL) << "size too big: " << size << " details: " << details;
            }
            // must be smaller than sizeof(size_t).
            static size_t CheckedSsizeTFromSizeT(size_t size) 
            {
                if (size > static_cast<size_t>(std::numeric_limits<size_t>::max())) {
                    // Some people grep for this message in logs
                    // so take care if you ever change it.
                    LogFatalSizeTooBig(size, "size_t to int conversion");
                }
                return (size);
            }
            // Out-of-line error path.
            //static void LogFatalSizeTooBig(size_t size, const char* details);
        public:
            // We provide non-explicit singleton constructors so users can pass
            // in a "const char*" or a "string" wherever a "StringPiece" is
            // expected.
            //
            StringPiece() : ptr_(NULL), length_(0) {}

            StringPiece(const char* str)  : ptr_(str), length_(0) {
                if (str != NULL) {
                    length_ = CheckedSsizeTFromSizeT(strlen(str));
                }
            }
            // swallow copy 
            StringPiece(const std::string& str) : ptr_(str.data()), length_(0) {
                length_ = CheckedSsizeTFromSizeT(str.size());
            }

            StringPiece(const char* offset, size_t len) : ptr_(offset), length_(len) {
                assert(len >= 0);
            }
            // Substring of another StringPiece.
            // pos must be non-negative and <= x.length().
            StringPiece(StringPiece x, size_t pos);
            // Substring of another StringPiece.
            // pos must be non-negative and <= x.length().
            // len must be non-negative and will be pinned to at most x.length() - pos.
            StringPiece(StringPiece x, size_t pos, size_t len);
            // data() may return a pointer to a buffer with embedded NULLs, and the
            // returned buffer may or may not be null terminated.  Therefore it is
            // typically a mistake to pass data() to a routine that expects a NUL
            // terminated string.
            const char* data() const { return ptr_; }
            size_t size() const { return length_; }
            size_t length() const { return length_; }
            bool empty() const { return length_ == 0; }
            void clear() 
            {
                ptr_ = NULL;
                length_ = 0;
            }
            void set(const char* data, size_t len) 
            {
                assert(len >= 0);
                ptr_ = data;
                length_ = len;
            }
            void set(const char* str) 
            {
                ptr_ = str;
                if (str != NULL)
                    length_ = CheckedSsizeTFromSizeT(strlen(str));
                else
                    length_ = 0;
            }
            void set(const void* data, size_t len) 
            {
                ptr_ = reinterpret_cast<const char*>(data);
                length_ = len;
            }
            char operator[](size_t i) const 
            {
                assert(0 <= i);
                assert(i < length_);
                return ptr_[i];
            }
            void remove_prefix(size_t n) 
            {
                assert(length_ >= n);
                ptr_ += n;
                length_ -= n;
            }
            void remove_suffix(size_t n) {
                assert(length_ >= n);
                length_ -= n;
            }
            // returns {-1, 0, 1}
            int compare(StringPiece x) const 
            {
                const size_t min_size = length_ < x.length_ ? length_ : x.length_;
                int r = memcmp(ptr_, x.ptr_, static_cast<size_t>(min_size));
                if (r < 0) return -1;
                if (r > 0) return 1;
                if (length_ < x.length_) return -1;
                if (length_ > x.length_) return 1;
                return 0;
            }
            string as_string() const 
            {
                return toString();
            }
            // We also define ToString() here, since many other string-like
            // interfaces name the routine that converts to a C++ string
            // "toString", and it's confusing to have the method that does that
            // for a StringPiece be called "as_string()".  We also leave the
            // "as_string()" method defined here for existing code.
            string toString() const 
            {
                if (ptr_ == NULL) return string();
                return string(data(), static_cast<size_type>(size()));
            }
            operator string() const 
            {
                return toString();
            }

            void CopyToString(string* target) const;
            void AppendToString(string* target) const;
            bool starts_with(StringPiece x) const 
            {
                return (length_ >= x.length_) && (memcmp(ptr_, x.ptr_, static_cast<size_t>(x.length_)) == 0);
            }
            bool ends_with(StringPiece x) const 
            {
                return ((length_ >= x.length_) && (memcmp(ptr_ + (length_-x.length_), x.ptr_, static_cast<size_t>(x.length_)) == 0));
            }
            // Checks whether StringPiece starts with x and if so advances the beginning
            // of it to past the match.  It's basically a shortcut for starts_with
            // followed by remove_prefix.
            bool Consume(StringPiece x);
            // Like above but for the end of the string.
            bool ConsumeFromEnd(StringPiece x);
            // standard STL container boilerplate
            typedef char value_type;
            typedef const char* pointer;
            typedef const char& reference;
            typedef const char& const_reference;
            typedef size_t size_type;
            typedef ptrdiff_t difference_type;
            static const size_type npos;
            typedef const char* const_iterator;
            typedef const char* iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            iterator begin() const { return ptr_; }
            iterator end() const { return ptr_ + length_; }
            const_reverse_iterator rbegin() const {
                return const_reverse_iterator(ptr_ + length_);
            }
            const_reverse_iterator rend() const 
            {
                return const_reverse_iterator(ptr_);
            }
            size_t max_size() const { return length_; }
            size_t capacity() const { return length_; }
            size_t copy(char* buf, size_type n, size_type pos = 0) const;  // NOLINT
            bool contains(StringPiece s) const;
            size_t find(StringPiece s, size_type pos = 0) const;
            size_t find(char c, size_type pos = 0) const;
            size_t rfind(StringPiece s, size_type pos = npos) const;
            size_t rfind(char c, size_type pos = npos) const;
            size_t find_first_of(StringPiece s, size_type pos = 0) const;
            size_t find_first_of(char c, size_type pos = 0) const 
            {
                return find(c, pos);
            }
            size_t find_first_not_of(StringPiece s, size_type pos = 0) const;
            size_t find_first_not_of(char c, size_type pos = 0) const;
            size_t find_last_of(StringPiece s, size_type pos = npos) const;
            size_t find_last_of(char c, size_type pos = npos) const {
                return rfind(c, pos);
            }
            size_t find_last_not_of(StringPiece s, size_type pos = npos) const;
            size_t find_last_not_of(char c, size_type pos = npos) const;

            StringPiece substr(size_type pos, size_type n = npos) const;
    };

    // This large function is defined inline so that in a fairly common case where
    // one of the arguments is a literal, the compiler can elide a lot of the
    // following comparisons.
    inline bool operator==(StringPiece x, StringPiece y) 
    {
        size_t len = x.size();
        if (len != y.size()) {
            return false;
    }

    return x.data() == y.data() || len <= 0 || memcmp(x.data(), y.data(), static_cast<size_t>(len)) == 0;
    }

    inline bool operator!=(StringPiece x, StringPiece y) 
    {
        return !(x == y);
    }

    inline bool operator<(StringPiece x, StringPiece y) 
    {
        const size_t min_size = x.size() < y.size() ? x.size() : y.size();
        const int r = memcmp(x.data(), y.data(), static_cast<size_t>(min_size));
            return (r < 0) || (r == 0 && x.size() < y.size());
    }

    inline bool operator>(StringPiece x, StringPiece y) 
    {
        return y < x;
    }

    inline bool operator<=(StringPiece x, StringPiece y) 
    {
        return !(x > y);
    }

    inline bool operator>=(StringPiece x, StringPiece y) 
    {
        return !(x < y);
    }

    // allow StringPiece to be logged
    extern std::ostream& operator<<(std::ostream& o, StringPiece piece);

}  // namespace internal

#endif // STRINGPIECE_H
