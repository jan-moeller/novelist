/**********************************************************
 * @file   ModelPath.cpp
 * @author jan
 * @date   9/14/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "model/ModelPath.h"

namespace novelist {
    ModelPath::ModelPath(std::initializer_list<int> rows) noexcept
    {
        for (int r : rows)
            m_path.emplace_back(r, 0);
    }

    ModelPath::ModelPath(std::initializer_list<RowColumnIdx> l) noexcept
            :m_path{l}
    {
    }

    ModelPath::ModelPath(ModelPath::const_iterator begin, ModelPath::const_iterator end) noexcept
            :m_path(begin, end)
    {
    }

    ModelPath::ModelPath(QModelIndex const& idx) noexcept
    {
        QModelIndex iter = idx;
        while (iter.isValid()) {
            m_path.insert(m_path.begin(), {iter.row(), iter.column()});
            iter = iter.parent();
        }
    }

    QModelIndex ModelPath::toModelIndex(QAbstractItemModel* model) const noexcept
    {
        if (model == nullptr || m_path.empty())
            return QModelIndex{};

        QModelIndex iter;
        for (auto const& p : m_path)
            iter = model->index(p.first, p.second, iter);
        return iter;
    }

    bool ModelPath::isValid(QAbstractItemModel* model) const noexcept
    {
        return toModelIndex(model).isValid();
    }

    size_t ModelPath::depth() const noexcept
    {
        return m_path.size();
    }

    ModelPath::iterator ModelPath::begin() noexcept
    {
        return m_path.begin();
    }

    ModelPath::const_iterator ModelPath::begin() const noexcept
    {
        return m_path.cbegin();
    }

    ModelPath::iterator ModelPath::end() noexcept
    {
        return m_path.end();
    }

    ModelPath::const_iterator ModelPath::end() const noexcept
    {
        return m_path.cend();
    }

    ModelPath ModelPath::parentPath() const noexcept
    {
        if(m_path.empty())
            return ModelPath{};
        return ModelPath(m_path.begin(), m_path.end() - 1);
    }

    void ModelPath::emplace_back(RowColumnIdx const& rowColumnIdx) noexcept
    {
        m_path.emplace_back(rowColumnIdx);
    }

    void ModelPath::emplace_back(int row) noexcept
    {
        m_path.emplace_back(row, 0);
    }

    RowColumnIdx& ModelPath::operator[](size_t pos)
    {
        return m_path[pos];
    }

    RowColumnIdx const& ModelPath::operator[](size_t pos) const
    {
        return m_path[pos];
    }

    RowColumnIdx& ModelPath::leaf()
    {
        return m_path.back();
    }

    RowColumnIdx const& ModelPath::leaf() const
    {
        return m_path.back();
    }

    size_t ModelPath::compare(ModelPath const& other) const noexcept
    {
        for(size_t i = 0; i < std::max(depth(), other.depth()); ++i) {
            if(depth() <= i || other.depth() <= i || m_path[i] != other.m_path[i])
                return i;
        }

        return std::numeric_limits<size_t>::max();
    }

    bool ModelPath::operator==(ModelPath const& other) const noexcept
    {
        return m_path == other.m_path;
    }

    bool ModelPath::operator!=(ModelPath const& other) const noexcept
    {
        return !(*this == other);
    }

    bool ModelPath::operator<(ModelPath const& other) const noexcept
    {
        return m_path < other.m_path;
    }

    bool ModelPath::operator<=(ModelPath const& other) const noexcept
    {
        return m_path <= other.m_path;
    }

    bool ModelPath::operator>(ModelPath const& other) const noexcept
    {
        return m_path > other.m_path;
    }

    bool ModelPath::operator>=(ModelPath const& other) const noexcept
    {
        return m_path >= other.m_path;
    }

    std::ostream& operator<<(std::ostream& stream, ModelPath const& path)
    {
        stream << "(root)";
        for(auto [r,c] : path.m_path)
            stream << "->(" << r << "," << c << ")";
        return stream;
    }
}