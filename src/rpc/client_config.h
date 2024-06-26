// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <rpc/client_utils.h>

#include <string>

class Config;

namespace rpc::client
{

/**
 * Wrapper for RPC client config.
 */
class RPCClientConfig
{
  public:

    // Some default config values (here so they can be checked from unit tests)
    static constexpr int DEFAULT_DS_ENDPOINT_PORT { 80 };
    static constexpr int DEFAULT_DS_ENDPOINT_FAST_TIMEOUT { 5 };
    static constexpr int DEFAULT_DS_ENDPOINT_SLOW_TIMEOUT { 60 };

    // Factory methods
    static RPCClientConfig CreateForMVCd();
    static RPCClientConfig CreateForDoubleSpendEndpoint(const Config& config,
                                                        const std::string& addr,
                                                        int timeout,
                                                        unsigned protocolVersion);
    static RPCClientConfig CreateForSafeModeWebhook(const Config& config);
    static RPCClientConfig CreateForDoubleSpendDetectedWebhook(const Config& config);

    // Accessors
    const std::string& GetServerIP() const { return mServerIP; }
    int GetServerPort() const { return mServerPort; }
    int GetConnectionTimeout() const { return mConnectionTimeout; }
    const std::string& GetCredentials() const { return mUsernamePassword; }
    const std::string& GetWallet() const { return mWallet; }
    const std::string& GetEndpoint() const { return mEndpoint; }
    bool GetValidEmptyResponse() const { return mValidEmptyResponse; }

    bool UsesAuth() const { return ! mUsernamePassword.empty(); }

  private:

    // Server address details
    std::string mServerIP {};
    int mServerPort {-1};

    // Connection timeout
    int mConnectionTimeout { DEFAULT_HTTP_CLIENT_TIMEOUT };

    // Server username:password, or auth cookie
    std::string mUsernamePassword {};

    // Special wallet endpoint
    std::string mWallet {};

    // The configured endpoint (may be extended elsewhere)
    std::string mEndpoint {};

    // Are empty responses to be expected?
    bool mValidEmptyResponse {false};

};

} // namespace rpc::client

