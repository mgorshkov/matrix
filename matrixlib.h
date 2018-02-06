#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <assert.h>
#include <memory>
#include <iostream>

/**
 * @brief N-dimensional matrix library.
 */
template <typename T, size_t N, T Default>
struct IndexNode;

template <typename T, size_t N, T Default>
using IndexNodePtr = std::shared_ptr<IndexNode<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodePtrContainer = std::vector<IndexNodePtr<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodePtrContainerPtr = std::shared_ptr<IndexNodePtrContainer<T, N, Default>>;

template <typename T, size_t N, T Default>
using IndexNodeInternalIterator = typename IndexNodePtrContainer<T, N, Default>::iterator;

struct IExtraNodeDeleter
{
    virtual ~IExtraNodeDeleter(){}
    virtual void DeleteNode(void* node) = 0;
};

template <typename T, size_t N, T Default>
struct IndexNode : IExtraNodeDeleter
{
    IndexNode(IExtraNodeDeleter* parentExtraNodeDeleter, size_t index)
        : mParentExtraNodeDeleter(parentExtraNodeDeleter)
        , mIndex(index)
        , mChildren(std::make_shared<IndexNodePtrContainer<T, N - 1, Default>>())
    {
    }

    const IndexNode<T, N - 1, Default>& operator [] (size_t index) const
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [index](auto node)
            {
                return node->mIndex == index;
            });

        if (it != mChildren->end())
            return **it;
        assert(0);
    }

    IndexNode<T, N - 1, Default>& operator [] (size_t index)
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [index](auto node)
            {
                return node->mIndex == index;
            });

        if (it != mChildren->end())
            return **it;

        auto node = std::make_shared<IndexNode<T, N - 1, Default>>(this, index);
        mChildren->emplace_back(node);
        return *node;
    }

    void DeleteNode(void* node) override
    {
        auto it = std::find_if(mChildren->begin(), mChildren->end(),
            [node](auto child)
            {
                return &*child == static_cast<IndexNode<T, N - 1, Default>*>(node);
            });
        assert (it != mChildren->end());
        mChildren->erase(it);
        if (!mParentExtraNodeDeleter)
            return;
        mParentExtraNodeDeleter->DeleteNode(this);
    }

    IExtraNodeDeleter* mParentExtraNodeDeleter;
    size_t mIndex;
    IndexNodePtrContainerPtr<T, N - 1, Default> mChildren;
};

template <typename T, T Default>
struct IndexNode<T, 0, Default> : IExtraNodeDeleter
{
    IndexNode(IExtraNodeDeleter* parentExtraNodeDeleter, size_t index)
        : mParentExtraNodeDeleter(parentExtraNodeDeleter)
        , mIndex(index)
        , mValue({Default})
    {
    }

    IndexNode(IndexNode& node)
        : mParentExtraNodeDeleter(node.mParentExtraNodeDeleter)
        , mIndex(node.mIndex)
        , mValue(node.mValue)
    {
        if (node.IsDefault())
            mParentExtraNodeDeleter->DeleteNode(&node);
    }

    IndexNode& operator = (const IndexNode& node)
    {
        mParentExtraNodeDeleter = node.mParentExtraNodeDeleter;
        mIndex = node.mIndex;
        mValue = node.mValue;
        if (node.IsDefault())
            mParentExtraNodeDeleter->DeleteNode(&node);
        return *this;
    }

    IndexNode& operator = (const T& value)
    {
        mValue[0] = value;
        if (value == Default)
            mParentExtraNodeDeleter->DeleteNode(this);
        return *this;
    }

    operator T() const
    {
#ifdef DEBUG_PRINT
        std::cout << "T()" << std::endl;
#endif
        return mValue[0];
    }

    operator T&()
    {
#ifdef DEBUG_PRINT
        std::cout << "T&()" << std::endl;
#endif
        if (IsDefault())
        {
            mParentExtraNodeDeleter->DeleteNode(this);
            return DefaultValue;
        }
        return mValue[0];
    }

    void DeleteNode(void*) override
    {
    }

    bool IsDefault() const
    {
        return mValue[0] == Default;
    }

    IExtraNodeDeleter* mParentExtraNodeDeleter;
    size_t mIndex;
    std::vector<T> mValue;

    static T DefaultValue;
};

template <typename T, T Default>
T IndexNode<T, 0, Default>::DefaultValue = Default;

template <typename T, size_t N, T Default>
class IndexNodeIterator
{
public:
    IndexNodeIterator(IndexNodePtr<T, N, Default> rootContainer, bool isBegin)
        : mInternalIterator(rootContainer->mChildren->begin())
        , mInternalIteratorEnd(rootContainer->mChildren->end())
        , mNextIterator(*mInternalIterator, true)
        , mNextIteratorEnd(*std::prev(mInternalIteratorEnd), false)
    {
        if (!isBegin)
        {
            mInternalIterator = mInternalIteratorEnd;
            mNextIterator = mNextIteratorEnd;
        }
#ifdef DEBUG_PRINT
        std::cout << "IndexNodeIterator::IndexNodeIterator, N==" << N << ", isBegin=" << isBegin << std::endl;
#endif
    }

    IndexNodeIterator(const IndexNodeIterator& other)
        : mInternalIterator(other.mInternalIterator)
        , mInternalIteratorEnd(other.mInternalIteratorEnd)
        , mNextIterator(other.mNextIterator)
        , mNextIteratorEnd(other.mNextIteratorEnd)
    {
    }

    IndexNodeIterator& operator = (const IndexNodeIterator& other)
    {
        mInternalIterator = other.mInternalIterator;
        mInternalIteratorEnd = other.mInternalIteratorEnd;
        mNextIterator = other.mNextIterator;
        mNextIteratorEnd = other.mNextIteratorEnd;
        return *this;
    }

    IndexNodeIterator& operator ++ ()
    {
        assert (mInternalIterator != mInternalIteratorEnd);

#ifdef DEBUG_PRINT
        std::cout << "operator++" << ", N==" << N << std::endl;
#endif
        ++mNextIterator;

        if (mNextIterator.mInternalIterator == mNextIterator.mInternalIteratorEnd)
        {
#ifdef DEBUG_PRINT
            std::cout << "operator++" << ", N==" << N << " ++mInternalIterator before" << std::endl;
#endif
            ++mInternalIterator;
#ifdef DEBUG_PRINT
            std::cout << "operator++" << ", N==" << N << " ++mInternalIterator after, ended=" << (mInternalIterator == mInternalIteratorEnd) << std::endl;
#endif
            if (mInternalIterator != mInternalIteratorEnd)
            {
#ifdef DEBUG_PRINT
                std::cout << "operator++" << ", N==" << N << " mNextIterator before invalidate" << std::endl;
#endif
                mNextIterator = IndexNodeIterator<T, N - 1, Default>(*mInternalIterator, true);
#ifdef DEBUG_PRINT
                std::cout << "operator++" << ", N==" << N << " mNextIterator after invalidate" << std::endl;
#endif
            }
        }

#ifdef DEBUG_PRINT
        std::cout << "operator++" << ", N==" << N << " success" << std::endl;
#endif
        return *this;
    }

    bool operator == (IndexNodeIterator it) const
    {
#ifdef DEBUG_PRINT
        std::cout << "operator==" << ", N==" << N << std::endl;
#endif
        return mInternalIterator == it.mInternalIterator;
    }

    bool operator != (IndexNodeIterator it) const
    {
#ifdef DEBUG_PRINT
        std::cout << "operator!=" << ", N==" << N << std::endl;
#endif
        return !(*this == it);
    }

    std::pair<std::vector<size_t>, T> operator * () const
    {
#ifdef DEBUG_PRINT
        std::cout << "operator *, N==" << N << std::endl;
#endif
        assert (mInternalIterator != mInternalIteratorEnd);
        assert (mNextIterator != mNextIteratorEnd);

        size_t index = (*mInternalIterator)->mIndex;
#ifdef DEBUG_PRINT
        std::cout << "operator *, index==" << index << ", N==" << N << std::endl;
#endif
        auto indicesNext = *mNextIterator;
        indicesNext.first.emplace(indicesNext.first.begin(), index);
        return indicesNext;
    }

public:
    IndexNodeInternalIterator<T, N - 1, Default> mInternalIterator;
    IndexNodeInternalIterator<T, N - 1, Default> mInternalIteratorEnd;
    IndexNodeIterator<T, N - 1, Default> mNextIterator;
    IndexNodeIterator<T, N - 1, Default> mNextIteratorEnd;
};

template <typename T, T Default>
class IndexNodeIterator<T, 0, Default>
{
public:
    IndexNodeIterator(IndexNodePtr<T, 0, Default>& rootContainer, bool isBegin)
        : mInternalIterator(isBegin ? rootContainer->mValue.begin() : rootContainer->mValue.end())
        , mInternalIteratorEnd(rootContainer->mValue.end())
    {
#ifdef DEBUG_PRINT
        std::cout << "IndexNodeIterator::IndexNodeIterator, N==0, isBegin=" << isBegin << std::endl;
#endif
    }

    IndexNodeIterator(const IndexNodeIterator& other)
        : mInternalIterator(other.mInternalIterator)
        , mInternalIteratorEnd(other.mInternalIteratorEnd)
    {
    }

    IndexNodeIterator& operator = (const IndexNodeIterator& other)
    {
        mInternalIterator = other.mInternalIterator;
        mInternalIteratorEnd = other.mInternalIteratorEnd;
        return *this;
    }

    IndexNodeIterator& operator ++ ()
    {
        assert (mInternalIterator != mInternalIteratorEnd);
#ifdef DEBUG_PRINT
        std::cout << "operator++, N==0" << std::endl;
#endif
        ++mInternalIterator;
#ifdef DEBUG_PRINT
        std::cout << "operator++, N==0 success" << std::endl;
#endif
        return *this;
    }

    bool operator == (IndexNodeIterator it) const
    {
        return mInternalIterator == it.mInternalIterator;
    }

    bool operator != (IndexNodeIterator it) const
    {
        return !(*this == it);
    }

    std::pair<std::vector<size_t>, T> operator * () const
    {
        std::vector<size_t> indices = {};
#ifdef DEBUG_PRINT
        std::cout << "operator *, value=" << *mInternalIterator << ", N==0" << std::endl;
#endif
        return std::make_pair(indices, *mInternalIterator);
    }

    typename std::vector<T>::iterator mInternalIterator;
    typename std::vector<T>::iterator mInternalIteratorEnd;
};

template <typename T, size_t N, T Default>
class Matrix
{
public:
    Matrix()
        : mRoot(nullptr)
    {
    }

    Matrix(IndexNodePtr<T, N, Default> root)
        : mRoot(root)
    {
    }

    size_t size() const
    {
        size_t s = 0;
        if (!mRoot)
            return 0;
        for (auto child : *mRoot->mChildren)
        {
            s += Matrix<T, N - 1, Default>(child).size();
        }
        return s;
    }

    const IndexNode<T, N - 1, Default>& operator [] (size_t index) const
    {
        assert (mRoot);
        return (*mRoot)[index];
    }

    IndexNode<T, N - 1, Default>& operator [] (size_t index)
    {
        if (!mRoot)
            mRoot = std::make_shared<IndexNode<T, N, Default>>(nullptr, index);
        return (*mRoot)[index];
    }

    IndexNodeIterator<T, N, Default> begin()
    {
        return IndexNodeIterator<T, N, Default>(mRoot, true);
    }

    IndexNodeIterator<T, N, Default> end()
    {
        return IndexNodeIterator<T, N, Default>(mRoot, false);
    }

private:
    IndexNodePtr<T, N, Default> mRoot;
};

template <typename T, T Default>
class Matrix<T, 0, Default>
{
public:
    Matrix()
        : mRoot(nullptr)
    {
    }

    Matrix(IndexNodePtr<T, 0, Default> root)
        : mRoot(root)
    {
    }

    size_t size() const
    {
         return 1;
    }

    IndexNodeIterator<T, 0, Default> begin()
    {
        return IndexNodeIterator<T, 0, Default>(*mRoot, true);
    }

    IndexNodeIterator<T, 0, Default> end()
    {
        return IndexNodeIterator<T, 0, Default>(*mRoot, false);
    }

private:
    IndexNodePtr<T, 0, Default> mRoot;
};

int version();
