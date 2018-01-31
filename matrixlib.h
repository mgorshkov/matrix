#pragma once

#include <list>
#include <utility>
#include <algorithm>
#include <assert.h>
#include <memory>
#include <iostream>

template <typename T, T Default>
struct IndexNode
{
    IndexNode(size_t index_)
        : index(index_)
        , children(std::make_shared<std::list<std::shared_ptr<IndexNode>>>())
        , value(std::make_unique<T>(Default))
    {
    }

    bool IsDefault() const
    {
        return value && *value == Default;
    }

    size_t index;
    std::shared_ptr<std::list<std::shared_ptr<IndexNode>>> children;
    std::unique_ptr<T> value;
};

template <typename T, T Default>
using IndexNodePtr = std::shared_ptr<IndexNode<T, Default>>;

template <typename T, size_t N, T Default>
class Matrix
{
public:
    Matrix()
        : root(std::make_shared<std::list<IndexNodePtr<T, Default>>>())
    {
    }

    Matrix(std::shared_ptr<std::list<IndexNodePtr<T, Default>>> root_)
        : root(root_)
    {
    }

    size_t size() const
    {
        size_t s = 0;
        for (auto index : *root)
        {
            s += Matrix<T, N - 1, Default>(index->children).size();
        }
        return s;
    }

    Matrix<T, N - 1, Default> operator [] (size_t index)
    {
        auto it = std::find_if(root->begin(), root->end(),
            [index](auto node)
            {
                return node->index == index;
            });

        if (it != root->end())
            return Matrix<T, N - 1, Default>((*it)->children);

        auto node = std::make_shared<IndexNode<T, Default>>(index);
        root->emplace_back(node);
        return Matrix<T, N - 1, Default>(node->children);
    }

    class Iterator
    {
    public:
        using IndexNodeIterator = typename std::list<IndexNodePtr<T, Default>>::iterator;

        Iterator(std::shared_ptr<std::list<IndexNodePtr<T, Default>>> rootContainer_, bool isBegin)
            : rootContainer(rootContainer_)
        {
            if (isBegin)
            {
                iterators[0] = rootContainer->begin();

                for (size_t i = 1; i < N; ++i)
                {
                    auto& node = *iterators[i - 1];
                    auto container = node->children;
                    iterators[i] = container->begin();
                    CheckIterator(i);
                }
            }
            else
            {
                iterators[0] = rootContainer->end();
                auto node = *std::prev(iterators[0]);

                for (size_t i = 1; i < N; ++i)
                {
                    auto container = node->children;
                    iterators[i] = container->end();
                    node = *std::prev(iterators[i]);
                }
            }
        }

        void CheckIterator(size_t i)
        {
            std::shared_ptr<std::list<IndexNodePtr<T, Default>>> container;
            if (i == 0)
                container = rootContainer;
            else
            {
                auto& node = *iterators[i - 1];
                container = node->children;
            }
            while (iterators[i] != container->end() && (*iterators[i])->IsDefault())
                ++iterators[i];

            if (iterators[i] == container->end())
            {
                if (i > 0 && ++iterators[i - 1] != rootContainer->end())
                {
                    auto& node = *iterators[i - 1];
                    auto container = node->children;
                    iterators[i] = container->begin();
                    while (iterators[i] != container->end() && (*iterators[i])->IsDefault())
                    {
                        ++iterators[i];
                    }
                }
            }
        }

        Iterator& operator ++ ()
        {
            assert (iterators[0] != rootContainer->end());

            ++iterators[N - 1];
            CheckIterator(N - 1);

            return *this;
        }

        bool operator != (Iterator it) const
        {
            if (rootContainer != it.rootContainer)
                return true;
            for (size_t i = 0; i < N; ++i)
               if (iterators[i] == it.iterators[i])
                    return false;
            return true;
    	}

        std::pair<std::list<size_t>, T> operator * () const
        {
            assert (iterators[0] != rootContainer->end());

            std::list<size_t> indices;
            IndexNodePtr<T, Default> node;
            for (size_t i = 0; i < N; ++i)
            {
                node = *iterators[i];
                indices.emplace_back(node->index);
            }

            return std::pair<std::list<size_t>, T>{indices, *(node->value)};
        }

    private:
        IndexNodeIterator iterators[N];
        std::shared_ptr<std::list<IndexNodePtr<T, Default>>> rootContainer;
    };

    Iterator begin()
    {
        return Iterator(root, true);
    }

    Iterator end()
    {
        return Iterator(root, false);
    }

private:
    std::shared_ptr<std::list<IndexNodePtr<T, Default>>> root;
};

template <typename T, T Default>
class Matrix<T, 1, Default>
{
public:
    Matrix()
        : root(std::make_shared<std::list<IndexNodePtr<T, Default>>>())
    {
    }

    Matrix(std::shared_ptr<std::list<IndexNodePtr<T, Default>>> root_)
        : root(root_)
    {
    }

    size_t size() const
    {
        return std::count_if(root->begin(), root->end(),
            [](auto node)
            {
                return *node->value != Default;
            });
    }

    T& operator [] (size_t index)
    {
        auto it = std::find_if(root->begin(), root->end(),
            [index](auto node)
            {
                return node->index == index;
            });
        if (it != root->end())
            return *(*it)->value;
        auto node = std::make_shared<IndexNode<T, Default>>(index);
        root->emplace_back(node);
        return *node->value;
    }

    using IndexNodeIterator = typename std::list<IndexNodePtr<T, Default>>::iterator;
    IndexNodeIterator begin()
    {
        return root->begin();
    }

    IndexNodeIterator end()
    {
        return root->end();
    }

private:
    std::shared_ptr<std::list<IndexNodePtr<T, Default>>> root;
};

int version();
