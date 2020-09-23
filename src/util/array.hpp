#ifndef CPP_UTIL_ARRAY_HPP
#define CPP_UTIL_ARRAY_HPP

namespace util {
    class identity_array {
    public:
        template <typename T>
        T operator[](T t) { return (t); }
    };
    template <typename T>
    class constant_array {
    public:
        T _t;
        constant_array(T t) : _t(t) {}
        template <typename I>
        T operator[](I) { return (_t); }
    };
} // namespace util

#endif // CPP_UTIL_ARRAY_HPP
