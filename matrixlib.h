#pragma once

#include <iostream>

#include <list>
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

        Iterator(const IndexNodeIterator& iterator_)
            : iterator(iterator_)
            , iteratorChildren((*iterator)->children.begin())
    	{    		
        }

        Iterator& operator ++ ()
    	{
            auto c = (*iterator)->children;
            if (++iteratorChildren == c.end())
                ++iterator;
    		return *this;
    	}

    	bool operator != (Iterator it)
    	{
            return iterator != it.iterator;
    	}

        size_t operator * ()
    	{
            return (*iterator)->index;
    	}

    private:
        IndexNodeIterator iterator;
        IndexNodeIterator iteratorChildren;
    };

    Iterator begin()
    {
        return Iterator(root->begin());
    }

    Iterator end()
    {
        return Iterator(root->end());
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
