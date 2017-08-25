#pragma once
#include"SimpleKey.h"

namespace DistributedAssignment
{

template<typename TCommunicatorType>
class SimpleKeyIssuer
{
public:
    typedef typename TCommunicatorType::Location Location;
    typedef SimpleKey<Location> Key;

    SimpleKeyIssuer() = delete;

    SimpleKeyIssuer( TCommunicatorType & communicator )
    :   mNumOfKeyIssued{0},
        mpCommunicator{& communicator}
    {}

    ~SimpleKeyIssuer()
    {}

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