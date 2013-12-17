#if !defined ARV_ARRAY_VIEW_HPP_INCLUDED
#define      ARV_ARRAY_VIEW_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <array>
#include <vector>
#include <stdexcept>
#include <memory>

namespace boost {
    template<class T, std::size_t N>
    class array;
} // namespace boost

namespace arv {

using std::size_t;

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
    // ctors and assing operators
    //
    constexpr array_view() noexcept
        : length_(0), data_(nullptr)
    {}

    constexpr array_view(array_view const&) noexcept = default;

    // Note:
    // This constructor can't be constexpr because & operator can't be constexpr.
    template<size_t N>
    array_view(std::array<T, N> const& a) noexcept
        : length_(N), data_(a.data())
    {}

    // Note:
    // This constructor can't be constexpr because & operator can't be constexpr.
    template<size_t N>
    array_view(T const (& a)[N]) noexcept
        : length_(N), data_(std::addressof(a[0]))
    {}

    template<size_t N>
    array_view(boost::array<T, N> const& a) noexcept
        : length_(N), data_(a.data())
    {}

    array_view(std::vector<T> const& v) noexcept
        : length_(v.size()), data_(v.data())
    {}

    explicit constexpr array_view(T const* a, size_type const n) noexcept
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

//
// compare
// TODO: use template and SFINAE not to repeat yourself for arv::array_view, std::array, int [], 
//
template<class T1, class T2>
inline
constexpr bool operator==(array_view<T1> const& lhs, array_view<T2> const& rhs)
{
    auto litr = lhs.begin();
    auto ritr = rhs.begin();
    auto const llast = lhs.end();
    auto const rlast = rhs.end();

    if (llast - litr != rlast - ritr) {
        return false;
    }

    for (; litr != llast; ++litr, ++ritr) {
        if (!(*litr == *ritr)) {
            return false;
        }
    }

    return true;
}

template<class T1, class T2>
inline
constexpr bool operator!=(array_view<T1> const& lhs, array_view<T2> const& rhs)
{
    return !(lhs == rhs);
}

//
// helpers to construct view
//
template<class Array>
constexpr auto make_view(Array const& a)
    -> array_view<typename Array::value_type>
{
    return {a};
}

template<class T>
array_view<T> make_view(T const* p, typename array_view<T>::size_type const n)
{
    return {p, n};
}

} // namespace arv

#endif    // ARV_ARRAY_VIEW_HPP_INCLUDED
