/// @file       comm_string_util.h
/// @date       2013/12/05 21:07
///
/// @author
///
/// @brief      字符串处理
///

#ifndef COMM_STRING_UTIL_H_
#define COMM_STRING_UTIL_H_

#include <stdlib.h>
#include <wchar.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <functional>
#include <iterator>
#include <iosfwd>
#include <assert.h>

#ifdef WIN32
#pragma warning ( disable : 4702)
#endif

template <typename Ch>
struct zen_char_traits : public std::char_traits < Ch > {};

template <>
struct zen_char_traits < char > : public std::char_traits < char >
{
    typedef std::string StringT;

    static int_type ttolower(int_type c)
    {
        return ::tolower(c);
    }
    static int_type ttoupper(int_type c)
    {
        return ::toupper(c);
    }

    static int tisspace(int_type c)
    {
        return (isascii(c) && ::isspace(c));
    }

    static char_type *tstrend(char_type *s)
    {
        return ::strchr(s, 0);
    }
    static const char_type *tstrend(const char_type *s)
    {
        return ::strchr(s, 0);
    }

};

template <>
struct zen_char_traits < wchar_t > : public std::char_traits < wchar_t >
{
    typedef std::wstring    StringT;

    static int_type ttolower(int_type c)
    {
        return ::towlower(c);
    }

    static int_type ttoupper(int_type c)
    {
        return ::towupper(c);
    }

    static int tisspace(int_type c)
    {
        return ::iswspace(c);
    }

    static char_type *tstrend(char_type *s)
    {
        return ::wcschr(s, 0);
    }

    static const char_type *tstrend(const char_type *s)
    {
        return ::wcschr(s, 0);
    }

};

template < typename Ch >
struct ch_equal_to_nocase : public std::binary_function < Ch, Ch, bool >
{
    bool operator()(Ch c1, Ch c2) const
    {
        return zen_char_traits<Ch>::ttoupper(c1) == zen_char_traits<Ch>::ttoupper(c2);
    }
};

template <typename Ch>
inline void
trim_left(Ch *str)
{
    assert(str != NULL);

    // find first non-space position
    Ch *p = str;

    while (zen_char_traits<Ch>::tisspace(*p))
    {
        ++ p;
    }

    // move to front
    while (*p != 0)
    {
        *str ++ = *p ++;
    }

    *str = 0;
    return;
}

template <typename Ch>
inline void
trim_right(Ch *str)
{
    assert(str != NULL);

    // find last non-space position
    Ch *end = NULL;

    while (*str != 0)
    {
        if (!zen_char_traits<Ch>::tisspace(*str))
        {
            end = ++ str;
        }
        else
        {
            ++ str;
        }
    }

    // cut down
    if (end != NULL)
    {
        *end = 0;
    }

    return;
}

template < typename Ch >
inline void
trim(Ch *str)
{
    trim_right(str);
    trim_left(str);
    return;
}

template < typename Ch, typename Tr, typename A >
inline void
trim_left(std::basic_string<Ch, Tr, A> &str)
{
    typename std::basic_string<Ch, Tr, A>::iterator it = str.begin();

    while (it != str.end() && zen_char_traits<Ch>::tisspace(*it))
    {
        ++it;
    }

    str.erase(str.begin(), it);
    return;
}

template < typename Ch, typename Tr, typename A >
inline void
trim_right(std::basic_string<Ch, Tr, A> &str)
{
    typename std::basic_string<Ch, Tr, A>::iterator it = str.end();

    if (it == str.begin())
    {
        return;
    }

    while (it != str.begin() && zen_char_traits<Ch>::tisspace(*--it));

    str.erase(it + 1, str.end());

    return;
}

template < typename Ch, typename Tr, typename A >
inline void
trim(std::basic_string<Ch, Tr, A> &str)
{
    trim_right(str);
    trim_left(str);

    return;
}

// 以一串字符来trim,例如 str_trim="ab",则左边是a或者b的字符都会被去除
template < typename Ch >
inline void
trim_left(Ch *str, const Ch *str_trim)
{
    assert(str != NULL && str_trim != NULL);

    // find character not in str_trim
    const Ch *trim_end = zen_char_traits<Ch>::tstrend(str_trim);
    Ch *p = str;

    while (std::find(str_trim, trim_end, *p) != trim_end)
    {
        ++ p;
    }

    // move to front
    while (*p != 0)
    {
        *str ++ = *p ++;
    }

    // cut down
    *str = 0;
}

template < typename Ch >
inline void
trim_right(Ch *str, const Ch *str_trim)
{
    assert(str != NULL && str_trim != NULL);

    // find last character not in str_trim
    const Ch *trim_end = zen_char_traits<Ch>::tstrend(str_trim);
    Ch *end = NULL;

    while (*str != 0)
    {
        if (std::find(str_trim, trim_end, *str) == trim_end)
        {
            end = ++ str;    // record the last character not in str_trim
        }
        else
        {
            ++ str;
        }
    }

    // cut down
    if (end != NULL)
    {
        *end = 0;
    }

    return;
}

template < typename Ch >
inline void
trim(Ch *str, const Ch *str_trim)
{
    trim_right(str, str_trim);
    trim_left(str, str_trim);

    return;
}

template < typename Ch, typename Tr, typename A, typename TrimStrT >
inline void
trim_left(std::basic_string<Ch, Tr, A> &str, const TrimStrT &str_trim)
{
    str.erase(0, str.find_first_not_of(str_trim));
    return;
}

template < typename Ch, typename Tr, typename A, typename TrimStrT >
inline void
trim_right(std::basic_string<Ch, Tr, A> &str, const TrimStrT &str_trim)
{
    str.erase(str.find_last_not_of(str_trim) + 1);
    return;
}

template < typename StringT, typename TrimStrT >
inline void
trim(StringT &str, const TrimStrT &str_trim)
{
    trim_right(str, str_trim);
    trim_left(str, str_trim);
    return;
}

/// trim end

/// split start

///
/// @fn
/// @brief  split string
///
/// @param fs   first position of source sequence
/// @param ls   last position of source sequence
/// @param fo   first position of separator sequence
/// @param lo   last position of separator sequence
/// @param pr   equals compare functor
/// @param o    output iterator
/// @param x    this parameter will ignore, it use to indicates type StringT only
///
/// @return     true always
///
template < typename II_S, typename II_O, typename Pr, typename OI, typename StringT >
void
split(II_S fs,
      II_S ls,
      II_O fo,
      II_O lo,
      Pr pr,
      OI o,
      const StringT *x)
{
    // 避免编译告警，参数未被引用
    //ZEN_UNUSED_ARG(x);

    if (fo == lo)
    {
        *o = StringT(fs, ls);
        return ;
    }

    // current position old_str in str
    II_S pos = std::search(fs, ls, fo, lo, pr);

    if (pos == ls)
    {
        if (fs < ls)
        {
            *o = StringT(fs, ls);
        }

        return ;
    }

    size_t old_size = std::distance(fo, lo);

    for (; pos != ls; )
    {
        // append src string
        *o = StringT(fs, pos);
        // ignore old_str
        fs = pos + old_size;
        // find next
        pos = std::search(fs, ls, fo, lo, pr);
    }

    if (fs != ls)
    {
        *o = StringT(fs, ls);
    }

    return ;
}

///
/// @fn
/// @brief  split string
///
/// @param str          source string
/// @param separator    separator
/// @param v            result
///
/// @return
///
template < typename Ch, typename ContainerT >
inline void
split(const Ch *str, const Ch *separator, ContainerT &v)
{
    split(str,
          zen_char_traits<Ch>::tstrend(str),
          separator,
          zen_char_traits<Ch>::tstrend(separator),
          std::equal_to<Ch>(),
          std::back_inserter(v),
          static_cast<std::basic_string<Ch>*>(NULL));

    return ;
}

template < typename Ch, typename ContainerT >
inline void
split(const Ch *str, ContainerT &v)
{
    const Ch *start = str;
    const Ch *p = start;
    const Ch *end = zen_char_traits<Ch>::tstrend(str);
    typedef std::basic_string<Ch> StringT;
    std::back_insert_iterator<ContainerT> o = std::back_inserter(v);

    for (; p < end; p ++)
    {
        if (zen_char_traits<Ch>::tisspace(*p))
        {
            if (start < p)
            {
                *o = StringT(start, p - start);
            }

            start = p + 1;
        }
    }

    if (start < p)
    {
        *o = StringT(start, p - start);
    }
}

/// end of split

inline int
str_to_hex(const char *str, unsigned char *buff, unsigned int *buf_len)
{
    if (str == NULL
        || buff == NULL
        || buf_len == NULL)
    {
        // 参数不对
        return -1;
    }

    short idx = 0;
    unsigned short i = 0;
    char temp_char = 0;
    int temp = 0;
    unsigned short str_len = (unsigned short)strlen(str);

    if (str_len > (unsigned short)2 * (*buf_len))
    {
        return -2;
    }

    for (i = 0; i < str_len; ++ i)
    {
        temp_char = str[i];

        if (temp_char <= '9' && temp_char >= '0')
        {
            temp_char -= 0x30;
        }
        else
        {
            if (temp_char >= 'A' && temp_char <= 'F')
            {
                temp_char -= 0x37;
            }
            else
            {
                if (temp_char >= 'a' && temp_char <= 'f')
                {
                    temp_char -= 0x57;
                }
                else
                {
                    temp_char = 0;
                }
            }
        }

        temp += temp_char;

        if ((i % 2) != 0)
        {
            buff[idx] = (unsigned char)temp;
            temp = 0;
            idx ++;
        }
        else
        {
            temp *= 0x10;
        }
    }

    *buf_len = idx;
    return 0;
}

// 将一段二进制数据以16进制打印转成字符串
inline int
hex_to_str(const unsigned char *buf, size_t buf_len,
           char *dest, size_t dest_len)
{
    if (dest_len < buf_len * 2 + 1)
    {
        // 输出长度不够
        return -1;
    }

    char *p = dest;
    // 16进制对应的字符
    static const char *tag = "0123456789ABCDEF";
    unsigned char c;

    for (unsigned int i = 0; i < buf_len; ++ i)
    {
        c = buf[i];
        // 128 -> "F0"
        p[0] = tag[c >> 4]; // 高4位
        p[1] = tag[c & 0xF]; // 低4位
        p += 2;
    }

    // 字符串结尾
    *p = 0;

    return 0;
}

//////// replace ///////
// replace all sequence [fo, lo) to sequence [fn, ln] in string s
//  return value means the count of [fo, lo) replaced.
//
template < typename Ch, typename Tr, typename A, typename II_O, typename II_N, typename Pr >
int replace_all(std::basic_string<Ch, Tr, A> &s,
                II_O fo,
                II_O lo,
                II_N fn,
                II_N ln,
                Pr pr)
{
    if (fo == lo)
    {
        return 0;
    }

    typedef std::basic_string<Ch, Tr, A> StrT;
    // current position old_str in s
    typename StrT::iterator pos = std::search(s.begin(), s.end(),
                                              fo, lo, pr);

    if (pos == s.end())
    {
        return 0;
    }

    // position writed in s
    typename StrT::iterator write_it = s.begin();
    // temp string to save result
    StrT str;
    // count of replacement
    int count = 0;
    int old_size = (int)std::distance(fo, lo);
    int new_size = (int)std::distance(fn, ln);
    str.reserve(new_size < old_size ? s.size() : (s.size() * 2));

    for (; pos != s.end(); ++ count)
    {
        // append src string
        str.append(write_it, pos);
        // append new_str
        str.append(fn, ln);
        // ignore old_str
        write_it = pos + old_size;
        // find next
        pos = std::search(write_it, s.end(),
                          fo, lo, pr);
    }

    // append last sub-string
    str.append(write_it, s.end());
    s.swap(str);
    return count;
}

// replace all substring old_str to new_str in string s
// \param s     string to replace
// \param old_str   the string to be replaced
// \param new_str   the replacement string
// \return          count of old_str replaced
//
template < typename Ch, typename Tr, typename A >
inline int replace_all(std::basic_string<Ch, Tr, A> &s,
                       const Ch *old_str,
                       const Ch *new_str)
{
    assert(old_str != NULL && new_str != NULL);
    return replace_all(s, old_str, zen_char_traits<Ch>::tstrend(old_str),
                       new_str, zen_char_traits<Ch>::tstrend(new_str),
                       std::equal_to<Ch>());
}

// replace all substring old_str to new_str in string s, ignore case
// \param s     string to replace
// \param old_str   the string to be replaced
// \param new_str   the replacement string
// \return          count of old_str replaced
template < typename Ch, typename Tr, typename A >
inline int replace_all_no_case(std::basic_string<Ch, Tr, A> &s,
                               const Ch *old_str,
                               const Ch *new_str)
{
    assert(old_str != NULL && new_str != NULL);
    return replace_all(s, old_str, zen_char_traits<Ch>::tstrend(old_str),
                       new_str, zen_char_traits<Ch>::tstrend(new_str),
                       ch_equal_to_nocase<Ch>());
}

// s中的字符在old_chars中出现的，都替换成new_char
inline int replace_all(char *s, const char *old_chars, const char new_char)
{
    if (s == NULL || old_chars == NULL)
    {
        // 参数错误
        return -1;
    }

    char *p = s;

    while (*p != 0)
    {
        if (::strchr(old_chars, *p) != NULL)
        {
            *p = new_char;
        }

        p ++;
    }

    return 0;
}

#ifdef WIN32
#pragma warning ( default : 4702)
#endif

#endif // COMM_STRING_UTIL_H_
