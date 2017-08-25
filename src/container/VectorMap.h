#pragma once
#include<iostream>
#include<vector>
#include<map>

namespace Container
{

template<typename TKeyType,
         typename TDataType>
class VectorMap
{
public:
    typedef typename std::map<TKeyType, std::vector<TDataType>> MapIterator;
    typedef typename std::vector<TDataType>::iterator VectorIterator;

    struct Iterator
    {
        MapItertor mMapIterator;
        VectorIterator mVectorIterator;
    }

    VectorMap()
    {}

    ~VectorMap()
    {}

    void Insert( const TKeyType key, const TDataType data )
    {
        mVectorMap[key].push_back(data);
    }

    Iterator Begin()
    {
        MapIterator it_map = mVectorMap.begin();
        VectorIterator it_vector = it_map->second.begin();
        Iterator iterator = { it_map, it_vector };
        return Iterator
    }

    Iterator End()
    {
        MapIterator it_map = mVectorMap.end();
        VectorIterator it_vector = mVectorMap.begin()->second.end();//hack, it_map, it_vector are not consistent
        Iterator iterator = { it_map, it_vector };
        return Iterator
    }

    Iterator Next( const Iterator iterator )
    {
        MapIterator it_map = iterator.mMapIterator;
        VectorIterator it_vector = iterator.mVectorIterator;
        
        //check
        if( it_map== mVectorMap.end() )
        {
            std::cout<<__func__<<": map iterator is already at the end! exit"<<std::endl;
            exit(EXIT_FAILURE);
        }

        it_vector = std::next(it_vector);
        
        if ( it_vector == it_map->second.end() )
        {
            it_map = std::next(it_map);

            if( it_map == mVectorMap.end() )
                return End();
            else
            {
                it_vector = it_map->second.begin();
                Iterator it = { it_map, it_vector };
                return it;
            }
        }
        else
        {
            Iterator it = { it_map, it_vector };
            return it;
        }
    }

    void Clear()
    {
        mVectorMap.clear();
    }

private:
    std::map<TKeyType, std::vector<TDataType>> mVectorMap;
}

}