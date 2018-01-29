#pragma once

#include <iostream>

#include <list>
#include <utility>

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
        for (auto node : *root)
            if (node->index == index)
                return Matrix<T, N - 1, Default>(&node->children);
        auto node = new IndexNode<T>{index, {}, new T(Default)};
        root->push_back(node);
        return Matrix<T, N - 1, Default>(&node->children);
    }

    class Iterator
    {
    public:
        using IndexNodeIterator = typename std::list<IndexNodePtr<T>>::iterator;

        Iterator(const IndexNodeIterator& iterator_, bool isBegin)
        {
            iterators[0] = iterator_;

            if (isBegin)
            {
                for (size_t i = 1; i < N; ++i)
                {
                    auto container = (*iterators[i - 1])->children;
                    iterators[i] = container.begin();
                }
            }
        }

        Iterator& operator ++ ()
    	{
            for (size_t i = N; i--; )
            {
                auto container = (*iterators[i - 1])->children;
                if (++iterators[i] == container.end())
                    ++iterators[i - 1];
            }
    		return *this;
    	}

        bool operator != (Iterator it) const
    	{
            for (size_t i = N; i--; )
                if (iterators[i] == it.iterators[i])
                    return false;
            return true;
    	}

        std::pair<std::list<size_t>, T> operator * () const
    	{           
            std::list<size_t> indices;
            for (size_t i = 0; i < N - 1; ++i)
                indices.push_back((*iterators[i])->index);
            return std::pair<std::list<size_t>, T>{indices, *(*iterators[N - 1])->value};
    	}

    private:
        IndexNodeIterator iterators[N];
    };

    Iterator begin()
    {
        return Iterator(root->begin(), true);
    }

    Iterator end()
    {
        return Iterator(root->end(), false);
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
        return root->size();
    }

    T& operator [] (size_t index)
    {
        for (auto node : *root)
            if (node->index == index)
                return *node->value;

        auto node = new IndexNode<T>{index, {}, new T(Default)};
        root->push_back(node);
        return *node->value;
    }

private:
    std::list<IndexNodePtr<T>>* root;

    const static T DefaultValue = Default;
};

int version();
