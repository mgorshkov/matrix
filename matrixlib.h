#pragma once

#include <list>
#include <utility>
#include <algorithm>
#include <assert.h>

template <typename T>
struct IndexNode
{
    size_t index;
    std::list<IndexNode*> children;
    T* value;
};

template <typename T>
using IndexNodePtr = IndexNode<T>*;

template <typename T, size_t N, T Default>
class Matrix
{
public:
    Matrix()
        : root(new std::list<IndexNodePtr<T>>())
    {
    }

    Matrix(std::list<IndexNodePtr<T>>* root_)
        : root(root_)
    {
    }

    size_t size() const
    {
        size_t s = 0;
        for (auto index : *root)
        {
            s += Matrix<T, N - 1, Default>(&index->children).size();
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
            return Matrix<T, N - 1, Default>(&(*it)->children);

        auto node = new IndexNode<T>{index, {}, new T(Default)};
        root->push_back(node);
        return Matrix<T, N - 1, Default>(&node->children);
    }

    class Iterator
    {
    public:
        using IndexNodeIterator = typename std::list<IndexNodePtr<T>>::iterator;

        Iterator(std::list<IndexNodePtr<T>>* rootContainer_, bool isBegin)
            : rootContainer(rootContainer_)
        {
            if (isBegin)
            {
                iterators[0] = rootContainer->begin();

                for (size_t i = 1; i < N; ++i)
                {
                    auto& node = *iterators[i - 1];
                    auto& container = node->children;
                    iterators[i] = container.begin();
                }
            }
            else
            {
                iterators[0] = rootContainer->end();
                auto& node = *std::prev(iterators[0]);

                for (size_t i = 1; i < N; ++i)
                {
                    auto& container = node->children;
                    iterators[i] = container.end();
                    node = *std::prev(iterators[i]);
                }
            }
        }

        Iterator& operator ++ ()
        {
            assert (iterators[0] != rootContainer->end());

            for (size_t i = N; i--; )
            {
                std::list<IndexNodePtr<T>>* container;
                if (i == 0)
                    container = rootContainer;
                else
                {
                    auto& node = *iterators[i - 1];
                    container = &node->children;
                }
                if (++iterators[i] == container->end())
                    ++iterators[i - 1];
            }
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
            IndexNodePtr<T> node;
            for (size_t i = 0; i < N; ++i)
            {
                node = *iterators[i];
                indices.push_back(node->index);
            }

            return std::pair<std::list<size_t>, T>{indices, *(node->value)};
        }

    private:
        IndexNodeIterator iterators[N];
        std::list<IndexNodePtr<T>>* rootContainer;
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
    std::list<IndexNodePtr<T>>* root;

    const static T DefaultValue = Default;
};

template <typename T, T Default>
class Matrix<T, 1, Default>
{
public:
    Matrix()
        : root(new std::list<IndexNodePtr<T>>)
    {
    }

    Matrix(std::list<IndexNodePtr<T>>* root_)
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
        auto node = new IndexNode<T>{index, {}, new T(Default)};
        root->push_back(node);
        return *node->value;
    }

    using IndexNodeIterator = typename std::list<IndexNodePtr<T>>::iterator;
    IndexNodeIterator begin()
    {
        return root->begin();
    }

    IndexNodeIterator end()
    {
        return root->end();
    }

private:
    std::list<IndexNodePtr<T>>* root;

    const static T DefaultValue = Default;
};

int version();
