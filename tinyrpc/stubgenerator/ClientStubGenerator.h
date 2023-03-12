//
// Created by frank on 18-1-24.
//

#ifndef TINYRPC_CLIENTSTUBGENERATOR_H_
#define TINYRPC_CLIENTSTUBGENERATOR_H_

#include <tinyrpc/stubgenerator/StubGenerator.h>

namespace tinyrpc
{

class ClientStubGenerator: public StubGenerator
{
public:
    explicit
    ClientStubGenerator(Value& proto):
            StubGenerator(proto)
    {}

    std::string genStub() override;
    std::string genStubClassName() override;

private:
    std::string genMacroName();

    std::string genProcedureDefinitions();
    std::string genNotifyDefinitions();

    template <typename Rpc>
    std::string genGenericArgs(const Rpc& r, bool appendComma);
    template <typename Rpc>
    std::string genGenericParamMembers(const Rpc& r);
};

}


#endif //TINYRPC_CLIENTSTUBGENERATOR_H_
