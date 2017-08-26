#pragma once
#include"SimpleKey.h"

namespace DistributedAssignment
{

template<typename TCommunicatorType>
class DistributedKeyIssuer
{
public:
    typedef typename TCommunicatorType::Location Location;
    typedef SimpleKey<Location> Key;

    DistributedKeyIssuer() = delete;

    DistributedKeyIssuer( TCommunicatorType & communicator )
    :   mNumOfKeyIssued{0},
        mpCommunicator{& communicator}
    {}

    ~DistributedKeyIssuer()
    {}

    void Clear()
    {
        mNumOfKeyIssued = 0;
    }

    Key IssueNewKey()
    {
        Key key = { mpCommunicator->Here(), mNumOfKeyIssued };
        mNumberOfKeyIssued++;
        return key;
    }

private:
    std::size_t mNumOfKeyIssued;
    TCommunicatorType * mpCommunicator;
};

}