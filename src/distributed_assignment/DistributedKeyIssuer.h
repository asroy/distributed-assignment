#pragma once
#include"DistributedKey.h"

namespace DistributedAssignment
{

template<typename TCommunicatorType>
class DistributedKeyIssuer
{
public:
    typedef DistributedKey<TCommunicatorType> Key;

    DistributedKeyIssuer() = delete;

    DistributedKeyIssuer( const TCommunicatorType & communicator )
    :   mNumKeyIssued{0},
        mpCommunicator{& communicator}
    {}

    ~DistributedKeyIssuer()
    {}

    void Clear()
    {
        mNumKeyIssued = 0;
    }

    Key IssueNewKey()
    {
        Key key = { mpCommunicator->Here(), mNumKeyIssued };
        mNumKeyIssued++;
        return key;
    }

private:
    std::size_t mNumKeyIssued;
    const TCommunicatorType * const mpCommunicator;
};

}