// Copyright (c) 2021-2023 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <config.h>
#include <mining/factory.h>
#include <mining/journaling_block_assembler.h>
#include <stdexcept>

namespace mining
{

// Constructor
CMiningFactory::CMiningFactory(const Config& config)
: mConfig{config}
{
    if(config.GetMiningCandidateBuilder() == CMiningFactory::BlockAssemblerType::JOURNALING)
    {
        // Create a journaling block assembler
        mJournalingAssembler = std::make_shared<JournalingBlockAssembler>(config);
    }
}

// Get an appropriate block assembler
BlockAssemblerRef CMiningFactory::GetAssembler() const
{
    switch(mConfig.GetMiningCandidateBuilder())
    {
        case(CMiningFactory::BlockAssemblerType::JOURNALING):
			return mJournalingAssembler;
        default:
            break;
    }

    throw std::runtime_error("Unsupported BlockAssemblerType");
}

// Get a reference to the mining candidate manager
CMiningCandidateManager& CMiningFactory::GetCandidateManager()
{
    static CMiningCandidateManager manager {};
    return manager;
}

// Enable enum_cast for BlockAssemblerType
const enumTableT<CMiningFactory::BlockAssemblerType>& enumTable(CMiningFactory::BlockAssemblerType)
{   
    static enumTableT<CMiningFactory::BlockAssemblerType> table
    {   
        { CMiningFactory::BlockAssemblerType::UNKNOWN,    "UNKNOWN" },
        { CMiningFactory::BlockAssemblerType::JOURNALING, "JOURNALING" }
    };
    return table;
}

}
