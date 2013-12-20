#if !defined ARV_ARRAY_VIEW_HPP_INCLUDED
#define      ARV_ARRAY_VIEW_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <array>
#include <vector>
#include <stdexcept>
#include <memory>
#include <type_traits>

namespace boost {
template<class T, std::size_t N>
class array;
} // namespace boost

namespace arv {

using std::size_t;

// array_view {{{
template<class T>
class array_view {
public:
    //
    // types
    //
    typedef T value_type;
    typedef value_type const* pointer;
    typedef value_type const* const_pointer;
    typedef value_type const& reference;
    typedef value_type const& const_reference;
    typedef value_type const* iterator;
    typedef value_type const* const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    //
    // ctors and assign operators
    //
    constexpr array_view() noexcept
        : length_(0), data_(nullptr)
    {}

    constexpr array_view(array_view const&) noexcept = default;

    // Note:
    // This constructor can't be constexpr because & operator can't be constexpr.
    template<class U, size_type N>
    /*implicit*/ array_view(std::array<U, N> const& a) noexcept
        : length_(N), data_(N > 0 ? a.data() : nullptr)
    {}

    // Note:
    // This constructor can't be constexpr because & operator can't be constexpr.
    template<class U, size_type N>
    /*implicit*/ array_view(U const (& a)[N]) noexcept
        : length_(N), data_(N > 0 ? std::addressof(a[0]) : nullptr)
    {}

    template<class U, size_type N>
    /*implicit*/ array_view(boost::array<U, N> const& a) noexcept
        : length_(N), data_(a.data())
    {}

    template<class U>
    /*implicit*/ array_view(std::vector<U> const& v) noexcept
        : length_(v.size()), data_(v.empty() ? nullptr : v.data())
    {}

    template<class U>
    explicit constexpr array_view(U const* a, size_type const n) noexcept
        : length_(n), data_(a)
    {}

    array_view& operator=(array_view const&) noexcept = default;

    //
    // iterator interfaces
    //
    constexpr const_iterator begin() const noexcept
    {
        return data_;
    }
    constexpr const_iterator end() const noexcept
    {
        return data_ + length_;
    }
    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }
    constexpr const_iterator cend() const noexcept
    {
        return end();
    }
    const_reverse_iterator rbegin() const
    {
        return {end()};
    }
    const_reverse_iterator rend() const
    {
        return {begin()};
    }
    const_reverse_iterator crbegin() const
    {
        return rbegin();
    }
    const_reverse_iterator crend() const
    {
        return rend();
    }

    //
    // access
    //
    constexpr size_type size() const noexcept
    {
        return length_;
    }
    constexpr size_type length() const noexcept
    {
        return size();
    }
    constexpr size_type max_size() const noexcept
    {
        return size();
    }
    constexpr bool empty() const noexcept
    {
        return length_ == 0;
    }
    constexpr const_reference operator[](size_type const n) const noexcept
    {
        return *(data_ + n);
    }
    constexpr const_reference at(size_type const n) const
    {
        if (n >= length_) throw std::out_of_range("array_view::at");
        return *(data_ + n);
    }
    constexpr const_pointer data() const noexcept
    {
        return data_;
    }
    constexpr const_reference front() const noexcept
    {
        return *data_;
    }
    constexpr const_reference back() const noexcept
    {
        return *(data_ + length_ - 1);
    }

private:
    size_type const length_;
    const_pointer const data_;
};
// }}}

namespace detail { // {{{

    template<class Array>
    struct is_array_class {
        static bool const value = false;
    };
    template<class T, size_t N>
    struct is_array_class<std::array<T, N>> {
        static bool const value = true;
    };
    template<class T, size_t N>
    struct is_array_class<boost::array<T, N>> {
        static bool const value = true;
    };
    template<class T>
    struct is_array_class<std::vector<T>> {
        static bool const value = true;
    };

    template< class Array >
    struct is_array : is_array_class<Array>
    {};
    template< class T, size_t N>
    struct is_array<T [N]> {
        static bool const value = true;
    };

} // namespace detail
// }}}

// compare {{{
namespace detail {
    template< class ArrayL, class ArrayR >
    inline constexpr
    bool operator_equal_impl(ArrayL const& lhs, size_t const lhs_size, ArrayR const& rhs, size_t const rhs_size)
    {
        if (lhs_size != rhs_size) {
            return false;
        }

        for (auto litr = std::begin(lhs), ritr = std::begin(rhs);
            litr != std::end(lhs);
            ++litr, ++ritr) {
            if (!(*litr == *ritr)) {
                return false;
            }
        }

        return true;
    }
} // namespace detail

template<class T1, class T2>
inline constexpr
bool operator==(array_view<T1> const& lhs, array_view<T2> const& rhs)
{
    return detail::operator_equal_impl(lhs, lhs.length(), rhs, rhs.length());
}

template<
    class T,
    class Array,
    class = typename std::enable_if<
        detail::is_array_class<Array>::value
    >::type
>
inline constexpr
bool operator==(array_view<T> const& lhs, Array const& rhs)
{
    return detail::operator_equal_impl(lhs, lhs.length(), rhs, rhs.size());
}

template<class T1, class T2, size_t N>
inline constexpr
bool operator==(array_view<T1> const& lhs, T2 const (& rhs)[N])
{
    return detail::operator_equal_impl(lhs, lhs.length(), rhs, N);
}

template<
    class T,
    class Array,
    class = typename std::enable_if<
        detail::is_array<Array>::value
    >::type
>
inline constexpr
bool operator!=(array_view<T> const& lhs, Array const& rhs)
{
    return !(lhs == rhs);
}

template<
    class Array,
    class T,
    class = typename std::enable_if<
        detail::is_array<Array>::value
    >::type
>
inline constexpr
bool operator==(Array const& lhs, array_view<T> const& rhs)
{
    return rhs == lhs;
}

template<
    class Array,
    class T,
    class = typename std::enable_if<
        detail::is_array<Array>::value,
        Array
    >::type
>
inline constexpr
bool operator!=(Array const& lhs, array_view<T> const& rhs)
{
    return !(rhs == lhs);
}
// }}}

// helpers to construct view {{{
template<
    class Array,
    class = typename std::enable_if<
        detail::is_array_class<Array>::value
    >::type
>
inline constexpr
auto make_view(Array const& a)
    -> array_view<typename Array::value_type>
{
    return {a};
}

template< class T, size_t N>
inline constexpr
array_view<T> make_view(T const (&a)[N])
{
    return {a};
}

template<class T>
inline
constexpr array_view<T> make_view(T const* p, typename array_view<T>::size_type const n)
{
    return array_view<T>{p, n};
}

// }}}

} // namespace arv

#endif    // ARV_ARRAY_VIEW_HPP_INCLUDED
