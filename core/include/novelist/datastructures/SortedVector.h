/**********************************************************
 * @file   SortedVector.h
 * @author jan
 * @date   10/26/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SORTEDVECTOR_H
#define NOVELIST_SORTEDVECTOR_H

#include <functional>

namespace novelist {

    template<typename T, typename Pred, typename Alloc>
    class SortedVector;

    /**
     * Move a range of elements to another position within the same collection.
     * @tparam Iter Iterator type
     * @param src First element to move
     * @param length Amount of elements after \p src to move
     * @param dest Destination. The source element will be inserted before this.
     * @return Iterator to the moved element after the operation
     */
    template<typename Iter>
    Iter move_range(Iter src, size_t length, Iter dest)
    {
        if (src == dest || src + 1 == dest)
            return src;
        if (src < dest) {
            auto first = src;
            auto middle = first + length;
            auto last = dest;
            return std::rotate(first, middle, last);
        }
        else {
            auto first = std::make_reverse_iterator(src + 1);
            auto middle = first + length;
            auto last = std::make_reverse_iterator(dest);
            return std::rotate(first, middle, last).base() - 1;
        }
    }

    /**
     * Find the location where a new element would be placed if it was inserted into a SortedVector
     * @tparam Iter Iterator type
     * @param begin Begin iterator of a sorted vector
     * @param end End iterator of a sorted vector
     * @param element Element to find insert location for
     * @return Iterator to the position where \p element would end up if inserted into the range
     */
    template<typename Iter>
    Iter findInsertIdx(Iter begin, Iter end, typename std::iterator_traits<Iter>::value_type const& element)
    {
        typename Iter::predicate_type comp;
        return std::lower_bound(begin, end, element, comp);
    }

    /**
     * Find the location where a new element would be placed if it was inserted into a SortedVector
     * @tparam T Element type
     * @tparam Pred Order predicate
     * @tparam Alloc Allocator
     * @param vec Sorted vector instance
     * @param element Element to find insert location for
     * @return Iterator to the position where \p element would end up if inserted into \p vec
     */
    template<typename T, typename Pred, typename Alloc>
    typename SortedVector<T, Pred, Alloc>::const_iterator findInsertIdx(SortedVector<T, Pred, Alloc> const& vec,
            T const& element)
    {
        Pred comp;
        return std::lower_bound(vec.begin(), vec.end(), element, comp);
    }

    /**
     * A vector that is sorted at all times.
     * @tparam T Base type
     * @tparam Pred Comparison predicate, defaults to std::less
     * @tparam Alloc Allocator, defaults to std::allocator
     */
    template<typename T, typename Pred = std::less<T>, typename Alloc = std::allocator<T>>
    class SortedVector : private std::vector<T, Alloc> {
    private:
        using vector_t = std::vector<T, Alloc>;

        struct iterator_t : public vector_t::iterator {
            using predicate_type = Pred;
            using vector_t::iterator::iterator;

        private:
            iterator_t(typename vector_t::iterator iter)
                    :vector_t::iterator(iter) { }

            friend SortedVector;
        };

        struct const_iterator_t : public vector_t::const_iterator {
            using predicate_type = Pred;
            using vector_t::const_iterator::const_iterator;

            const_iterator_t(typename vector_t::const_iterator iter)
                    :vector_t::const_iterator(iter) { }

            const_iterator_t(typename vector_t::iterator iter)
                    :vector_t::const_iterator(iter) { }

            friend SortedVector;
        };

        struct reverse_iterator_t : public vector_t::reverse_iterator {
            using predicate_type = Pred;
            using vector_t::reverse_iterator::reverse_iterator;

        private:
            reverse_iterator_t(typename vector_t::reverse_iterator iter)
                    :vector_t::reverse_iterator(iter) { }

            friend SortedVector;
        };

        struct const_reverse_iterator_t : public vector_t::const_reverse_iterator {
            using predicate_type = Pred;
            using vector_t::const_reverse_iterator::const_reverse_iterator;

        private:
            const_reverse_iterator_t(typename vector_t::const_reverse_iterator iter)
                    :vector_t::const_reverse_iterator(iter) { }

            const_reverse_iterator_t(typename vector_t::reverse_iterator iter)
                    :vector_t::const_reverse_iterator(iter) { }

            friend SortedVector;
        };

    public:

        using iterator = iterator_t;
        using const_iterator = const_iterator_t;
        using reverse_iterator = reverse_iterator_t;
        using const_reverse_iterator = const_reverse_iterator_t;
        using const_reference = typename vector_t::const_reference;

    private:
        const_iterator relocate(const_iterator iter)
        {
            if (size() <= 1)
                return iter;

            typename vector_t::iterator mutIter = vector_t::begin() + std::distance(begin(), iter);

            Pred comp;
            if (iter != begin() && comp(*iter, *(iter - 1))) {
                auto destIter = findInsertIdx(begin(), iter, *iter);
                typename vector_t::iterator mutDestIter = vector_t::begin() + std::distance(begin(), destIter);
                return move_range(mutIter, 1, mutDestIter);
            }
            else if (iter != (end() - 1) && comp(*(iter + 1), *iter)) {
                auto destIter = findInsertIdx(const_iterator(iter + 1), end(), *iter);
                typename vector_t::iterator mutDestIter = vector_t::end() - std::distance(destIter, end());
                return move_range(mutIter, 1, mutDestIter);
            }

            return iter;
        }

    public:
        using vector_t::vector;
        using vector_t::empty;
        using vector_t::size;
        using vector_t::max_size;
        using vector_t::reserve;
        using vector_t::capacity;
        using vector_t::shrink_to_fit;
        using vector_t::clear;
        using vector_t::erase;
        using vector_t::pop_back;
        using vector_t::swap;

        explicit SortedVector(size_t count, Alloc const& alloc = Alloc())
                :vector_t(count, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        template<class InputIt>
        SortedVector(InputIt first, InputIt last, Alloc const& alloc = Alloc())
                : vector_t(first, last, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        explicit SortedVector(vector_t const& other)
                :vector_t(other,
                std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator()))
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(vector_t const& other, Alloc const& alloc)
                :vector_t(other, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(SortedVector const& other)
                :vector_t(other.begin(), other.end())
        {
        }

        explicit SortedVector(vector_t&& other) noexcept
                :vector_t(std::move(other))
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(vector_t&& other, Alloc const& alloc)
                :vector_t(std::move(other), alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(SortedVector&& other) noexcept
                :vector_t(std::move(other))
        {
        }

        SortedVector(std::initializer_list<T> init, Alloc const& alloc = Alloc())
                :vector_t(init, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end());
        }

        SortedVector& operator=(SortedVector const& other)
        {
            vector_t::operator=(other);
            return *this;
        }

        SortedVector& operator=(SortedVector&& other) noexcept
        {
            vector_t::operator=(std::move(other));
            return *this;
        }

        SortedVector& operator=(std::initializer_list<T> iList)
        {
            vector_t::operator=(iList);
            std::sort(vector_t::begin(), vector_t::end(), Pred());
            return *this;
        }

        const_reference at(size_t pos) const
        {
            return vector_t::at(pos);
        }

        const_reference operator[](size_t pos) const
        {
            return vector_t::operator[](pos);
        }

        const_reference front() const
        {
            return vector_t::front();
        }

        const_reference back() const
        {
            return vector_t::back();
        }

        const_iterator begin() const
        {
            return vector_t::begin();
        }

        const_iterator end() const
        {
            return vector_t::end();
        }

        const_reverse_iterator rbegin() const
        {
            return vector_t::rbegin();
        }

        const_reverse_iterator rend() const
        {
            return vector_t::rend();
        }

        /**
         * Insert an element into the vector
         * @param value  Value to insert
         * @return Iterator to the inserted element
         */
        const_iterator insert(T const& value)
        {
            return vector_t::insert(findInsertIdx(*this, value), value);
        }

        /**
         * Insert an element into the vector, giving a hint to its destination. The better the hint, the less time this
         * method takes to finish.
         * @param value Value to insert
         * @param hint Hint to the position to insert at
         * @return Iterator to the inserted element
         */
        const_iterator insert(T const& value, const_iterator hint)
        {
            auto iter = vector_t::insert(hint, value);
            return relocate(iter);
        }

        /**
         * Insert an element into the vector
         * @param value  Value to insert
         * @return Iterator to the inserted element
         */
        const_iterator insert(T&& value)
        {
            return vector_t::insert(findInsertIdx(*this, value), std::move(value));
        }

        /**
         * Insert an element into the vector, giving a hint to its destination. The better the hint, the less time this
         * method takes to finish.
         * @param value Value to insert
         * @param hint Hint to the position to insert at
         * @return Iterator to the inserted element
         */
        const_iterator insert(T&& value, const_iterator hint)
        {
            auto iter = vector_t::insert(hint, std::move(value));
            return relocate(iter);
        }

        /**
         * Insert \p count elements into the vector
         * @param count Amount of copies to insert
         * @param value  Value to insert
         * @return Iterator to the first inserted element
         */
        const_iterator insert(size_t count, T const& value)
        {
            return vector_t::insert(findInsertIdx(*this, value), count, value);
        }

        /**
         * Insert elements from a range of input iterators
         * @tparam InputIt Input iterator type
         * @param first First element
         * @param last Last element
         */
        template<
                typename InputIt,
                typename = std::enable_if<std::is_base_of_v<
                        typename std::iterator_traits<InputIt>::iterator_category, std::input_iterator_tag>>>
        void insert(InputIt first, InputIt last)
        {
            for (auto it = first; it != last; ++it)
                vector_t::insert(findInsertIdx(*this, *it), *it);
        }

        /**
         * Insert from an initializer list
         * @param iList Initializer list to insert
         */
        void insert(std::initializer_list<T> iList)
        {
            insert(iList.begin(), iList.end());
        }

        /**
         * Modify a range of elements. If the sort criterion is affected, the class invariant is restored automatically.
         * @tparam C Function type
         * @param first First element
         * @param last Last element
         * @param functor Functor to modify the elements
         */
        template<typename C, typename = std::enable_if<std::is_invocable_v<void(T&)>>>
        void modify(const_iterator first, const_iterator last, C const& functor)
        {
            for (auto iter = first; iter != last; ++iter) {
                modify(iter, functor);
            }
        }

        /**
         * Modify an element. If the sort criterion is affected, the class invariant is restored automatically.
         * @tparam C Function type
         * @param iter Element to modify
         * @param functor Functor to modify the elements
         * @return Iterator to the modified element
         */
        template<typename C, typename = std::enable_if<std::is_invocable_v<void(T&)>>>
        const_iterator modify(const_iterator iter, C const& functor)
        {
            functor(vector_t::operator[](std::distance(begin(), iter)));
            return relocate(iter);
        }

        friend std::ostream& operator<<(std::ostream& stream, SortedVector const& vec)
        {
            stream << "{ ";
            if (!vec.empty())
                stream << vec.front();
            for (size_t i = 1; i < vec.size(); ++i)
                stream << ", " << vec[i];
            stream << " }";
            return stream;
        }
    };
}

#endif //NOVELIST_SORTEDVECTOR_H
