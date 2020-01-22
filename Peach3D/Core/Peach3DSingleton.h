//
//  Singleton.h
//  TestPeach3D
//
//  Created by singoon he on 12-4-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_SINGLETON_H
#define PEACH3D_SINGLETON_H

namespace Peach3D
{
    /**
     * 单件模式类，所有具有单件功能的都继承于此类.
     */
    template <typename T>
    class Singleton
    {
    protected:
        /**
         * 保存单件对象的指针.
         */
        static T* mSingleton;
    public:
        /**
         * 构造函数，子类构造时自动调用，保存单件对象的指针.
         */
        Singleton(void)
        {
            mSingleton = static_cast<T*>(this);
        }
        /**
         * 析构函数.
         */
        virtual ~Singleton(void)
        {
            mSingleton = nullptr;
        }
        /**
         * 获取单件对象.
         * @return 返回单件对象的引用.
         */
        static T& getSingleton(void)
        {
            return (*mSingleton);
        }
        /**
         * 获取单件对象指针.
         * @return 返回单件对象的指针.
         */
        static T* getSingletonPtr(void)
        {
            return mSingleton;
        }
    };

/** Not need instance T in *.cpp file no longer. */
template <typename T> T* Singleton<T>::mSingleton = nullptr;
}

#endif // PEACH3D_SINGLETON_H
