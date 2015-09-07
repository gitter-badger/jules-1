#ifndef JULES_DATAFRAME_DATAFRAME_H
#define JULES_DATAFRAME_DATAFRAME_H

#include "dataframe/dataframe_decl.hpp"
#include "range/range.hpp"

#include <algorithm>
#include <iterator>
#include <regex>

namespace jules
{
template <typename Coercion> base_dataframe<Coercion>::base_dataframe(std::initializer_list<column_t> columns)
{
    auto size = columns.begin()->size();
    if (!std::all_of(columns.begin() + 1, columns.end(), [size](auto& col) { return col.size() == size; }))
        throw std::runtime_error{"columns size mismatch"};

    nrow_ = size;
    columns_.assign(columns.begin(), columns.end());

    std::size_t i = 0;
    for (auto& column : columns_) {
        auto& colname = column.name();
        if (colname.empty())
            colname = std::to_string(i);
        if (colindexes_.find(colname) != colindexes_.end())
            throw std::runtime_error{"repeated column name"};
        colindexes_[colname] = i++;
    }
}

template <typename Coercion> base_dataframe<Coercion>& base_dataframe<Coercion>::cbind(const column_t& column)
{
    auto tmp = column;

    auto& name = tmp.name();
    if (name.empty())
        name = std::to_string(columns_.size());

    auto it = colindexes_.find(name);
    if (it != colindexes_.end())
        throw std::runtime_error{"column already exists"};

    if (!empty() && nrow() != tmp.size())
        throw std::runtime_error{"invalid column size"};

    if (empty())
        nrow_ = tmp.size();

    colindexes_[name] = columns_.size();
    columns_.push_back(std::move(tmp));

    return *this;
}

template <typename Coercion> base_dataframe<Coercion>& base_dataframe<Coercion>::cbind(column_t&& column)
{
    auto& name = column.name();
    if (name.empty())
        name = std::to_string(columns_.size());

    auto it = colindexes_.find(name);
    if (it != colindexes_.end())
        throw std::runtime_error{"column already exists"};

    if (!empty() && nrow() != column.size())
        throw std::runtime_error{"invalid column size"};

    if (empty())
        nrow_ = column.size();

    colindexes_[name] = columns_.size();
    columns_.push_back(std::move(column));

    return *this;
}

template <typename Coercion>
auto base_dataframe<Coercion>::select(const std::string& name) const -> const column_t &
{
    auto it = colindexes_.find(name);
    if (it == colindexes_.end())
        throw std::out_of_range{"column does not exists"};
    return columns_.at(it->second);
}

template <typename Coercion> auto base_dataframe<Coercion>::select(const expr_t& expression) const -> column_t
{
    return expression.extract_from(*this);
}

template <typename Coercion>
auto base_dataframe<Coercion>::select(const expr_list_t& expression_list) const -> base_dataframe
{
    return expression_list.extract_from(*this);
}

template <typename Coercion>
base_dataframe<Coercion> base_dataframe<Coercion>::read(std::istream& is,
                                                        const dataframe_storage_options& opt)
{
    const auto as_string = [](std::istream& is) {
        std::string buffer;
        is.seekg(0, std::ios::end);
        buffer.reserve(is.tellg());
        is.seekg(0);
        buffer.assign(std::istreambuf_iterator<char>(is),
                      std::istreambuf_iterator<char>());
        return buffer;
    };

    const auto as_range = [](auto&& match) {
        return make_iterator_range<std::string::iterator>(match.first, match.second);
    };

    if (!is)
        return {};

    std::string raw_data = as_string(is);

    std::regex eol{opt.eol};
    std::regex sep{opt.separator};

    using namespace adaptors;
    auto line_range = raw_data | tokenized(eol, -1);

    std::vector<std::sub_match<std::string::iterator>> data;
    std::size_t ncol = 0, nrow = 0;

    std::size_t size = 0;
    for (auto&& line: line_range) {
        copy(as_range(line) | tokenized(sep, -1), std::back_inserter(data));

        if (ncol == 0)
            ncol = data.size() - size;

        if (ncol != 0 && data.size() - size != ncol)
            throw std::runtime_error{"number of columns differ"};

        size = data.size();
    }
    nrow = data.size() / ncol;

    base_dataframe<Coercion> df;

    for (std::size_t j = 0; j < ncol; ++j) {
        auto column_data = make_iterator_range(data.begin() + j + (opt.header ? ncol : 0), data.end()) |
                           strided(ncol) |
                           transformed([](auto&& match) { return std::move(std::string{match.first, match.second}); });
        base_column<Coercion> col(opt.header ? std::string{data[j].first, data[j].second} : std::string{},
                                  column_data);
        df.cbind(std::move(col));
    }

    return df;
}

template <typename Coercion>
void base_dataframe<Coercion>::write(const base_dataframe<Coercion>& df, std::ostream& os)
{
    if (df.nrow() == 0 || df.ncol() == 0)
        return;

    std::vector<base_column<Coercion>> coerced;
    std::vector<const_column_view<std::string>> data;

    for (std::size_t j = 0; j < df.ncol(); ++j) {
        const auto& col = df.select(j);

        if (col.elements_type() == typeid(std::string)) {
            data.push_back(make_view<std::string>(col));
        } else {
            coerced.push_back(std::move(coerce_to<std::string>(col)));
            data.push_back(make_view<std::string>(coerced.back()));
        }
    }

    os << df.select(0).name();
    for (std::size_t j = 1; j < df.ncol(); ++j)
        os << "\t" << df.select(j).name();
    os << "\n";

    for (std::size_t i = 0; i < df.nrow(); ++i) {
        os << data[0][i];
        for (std::size_t j = 1; j < df.ncol(); ++j)
            os << "\t" << data[j][i];
        os << "\n";
    }
}

} // namespace jules

#endif // JULES_DATAFRAME_DATAFRAME_H
