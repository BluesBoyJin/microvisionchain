// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2021-2023 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MVC_CHAINPARAMSBASE_H
#define MVC_CHAINPARAMSBASE_H

#include <memory>
#include <string>

/**
 * CBaseChainParams defines the base parameters
 * (shared between mvc-cli and mvcd)
 * of a given instance of the MVC system.
 */
class CBaseChainParams
{
public:
    /** BIP70 chain name strings (main, test or regtest) */
    static const std::string MAIN;
    static const std::string TESTNET;

    CBaseChainParams(int port, const std::string& data_dir);

    const std::string& DataDir() const { return strDataDir; }
    int RPCPort() const { return nRPCPort; }

private:
    int nRPCPort;
    std::string strDataDir;
};

/**
 * Creates and returns a std::unique_ptr<CBaseChainParams> of the chosen chain.
 * @returns a CBaseChainParams* of the chosen chain.
 * @throws a std::runtime_error if the chain is not supported.
 */
std::unique_ptr<CBaseChainParams>
CreateBaseChainParams(const std::string &chain);

/**
 * Append the help messages for the chainparams options to the
 * parameter string.
 */
void AppendParamsHelpMessages(std::string &strUsage, bool debugHelp = true);

/**
 * Return the currently selected parameters. This won't change after app
 * startup, except for unit tests.
 */
const CBaseChainParams &BaseParams();

/** Sets the params returned by Params() to those for the given network. */
void SelectBaseParams(const std::string &chain);

/**
 * Looks for -testnet and returns the appropriate BIP70 chain name.
 * @return CBaseChainParams::MAX_NETWORK_TYPES if an invalid combination is
 * given. CBaseChainParams::MAIN by default.
 */
std::string ChainNameFromCommandLine();

#endif // MVC_CHAINPARAMSBASE_H
