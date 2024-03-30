// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MVC_RPCSERVER_H
#define MVC_RPCSERVER_H

#include "amount.h"
#include "rpc/protocol.h"
#include "uint256.h"
#include "httpserver.h"

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <string>

#include <univalue.h>

class CRPCCommand;

namespace RPCServer {
void OnStarted(std::function<void()> slot);
void OnStopped(std::function<void()> slot);
void OnPreCommand(std::function<void(const CRPCCommand &)> slot);
void OnPostCommand(std::function<void(const CRPCCommand &)> slot);
} // namespace RPCServer

class CBlockIndex;
class Config;
class CNetAddr;

/** Wrapper for UniValue::VType, which includes typeAny:
 * Used to denote don't care type. Only used by RPCTypeCheckObj */
struct UniValueType {
    UniValueType(UniValue::VType _type) : typeAny(false), type(_type) {}
    UniValueType() : typeAny(true) {}
    bool typeAny;
    UniValue::VType type;
};

class JSONRPCRequest {
public:
    UniValue id;
    std::string strMethod;
    UniValue params;
    bool fHelp;
    std::string URI;
    std::string authUser;

    JSONRPCRequest() {
        id = NullUniValue;
        params = NullUniValue;
        fHelp = false;
    }

    void parse(const UniValue &valRequest);
};

/** Query whether RPC is running */
bool IsRPCRunning();

/**
 * Set the RPC warmup status.  When this is done, all RPC calls will error out
 * immediately with RPC_IN_WARMUP.
 */
void SetRPCWarmupStatus(const std::string &newStatus);
/* Mark warmup as done.  RPC calls will be processed from now on.  */
void SetRPCWarmupFinished();

/* returns the current warmup state.  */
bool RPCIsInWarmup(std::string *statusOut);

/**
 * Type-check arguments; throws JSONRPCError if wrong type given. Does not check
 * that the right number of arguments are passed, just that any passed are the
 * correct type.
 */
void RPCTypeCheck(const UniValue &params,
                  const std::list<UniValue::VType> &typesExpected,
                  bool fAllowNull = false);

/**
 * Type-check one argument; throws JSONRPCError if wrong type given.
 */
void RPCTypeCheckArgument(const UniValue &value, UniValue::VType typeExpected);

/*
  Check for expected keys/value types in an Object.
*/
void RPCTypeCheckObj(const UniValue &o,
                     const std::map<std::string, UniValueType> &typesExpected,
                     bool fAllowNull = false, bool fStrict = false);

/** Opaque base class for timers returned by NewTimerFunc.
 * This provides no methods at the moment, but makes sure that delete cleans up
 * the whole state.
 */
class RPCTimerBase {
public:
    virtual ~RPCTimerBase() {}
};

/**
 * RPC timer "driver".
 */
class RPCTimerInterface {
public:
    virtual ~RPCTimerInterface() {}
    /** Implementation name */
    virtual const char *Name() = 0;
    /** Factory function for timers.
     * RPC will call the function to create a timer that will call func in
     * *millis* milliseconds.
     * @note As the RPC mechanism is backend-neutral, it can use different
     * implementations of timers.
     * This is needed to cope with the case in which there is no HTTP server,
     * but only GUI RPC console, and to break the dependency of pcserver on
     * httprpc.
     */
    virtual RPCTimerBase *NewTimer(std::function<void(void)> &func,
                                   int64_t millis) = 0;
};

/** Set the factory function for timers */
void RPCSetTimerInterface(RPCTimerInterface *iface);
/** Set the factory function for timer, but only, if unset */
void RPCSetTimerInterfaceIfUnset(RPCTimerInterface *iface);
/** Unset factory function for timers */
void RPCUnsetTimerInterface(RPCTimerInterface *iface);

/**
 * Run func nSeconds from now.
 * Overrides previous timer <name> (if any).
 */
void RPCRunLater(const std::string &name, std::function<void(void)> func,
                 int64_t nSeconds);

using rpcfn_type = UniValue (*)(Config&, const JSONRPCRequest&);
using const_rpcfn_type = UniValue (*)(const Config&, const JSONRPCRequest&);
using rpcfn_http_type = void (*)(const Config&,
                                 const JSONRPCRequest&,
                                 HTTPRequest*,
                                 bool processedInBatch);

class CRPCCommand {
public:
    std::string category;
    std::string name;
    bool okSafeMode;

private:
    union {
        rpcfn_type fn;
        const_rpcfn_type cfn;
        rpcfn_http_type http_fn;
    } actor;
    bool useConstConfig;
    bool useHTTPRequest;

public:
    std::vector<std::string> argNames;

    /**
     * There are different constructors depending whether Http request is required or
     * Config is const or not, so we can call the command through the proper pointer.
     * Casting constness on parameters of function is undefined behavior.
     */
    CRPCCommand(std::string category,
                std::string name,
                bool okSafeMode,
                bool useConstConfig,
                bool useHTTPRequest,
                std::vector<std::string> argNames)
        : category{std::move(category)},
          name{std::move(name)},
          okSafeMode{okSafeMode},
          useConstConfig{useConstConfig},
          useHTTPRequest{useHTTPRequest},
          argNames{std::move(argNames)}
    {
    }

    CRPCCommand(std::string category,
                std::string name,
                rpcfn_type fn,
                bool okSafeMode,
                std::vector<std::string> argNames)
        : CRPCCommand{std::move(category),
                      std::move(name),
                      okSafeMode,
                      false,
                      false,
                      std::move(argNames)}
    {
        actor.fn = fn;
    }

    CRPCCommand(std::string category,
                std::string name,
                const_rpcfn_type fn,
                bool okSafeMode,
                std::vector<std::string> argNames)
        : CRPCCommand{std::move(category),
                      std::move(name),
                      okSafeMode,
                      true,
                      false,
                      std::move(argNames)}
    {
        actor.cfn = fn;
    }

    CRPCCommand(std::string category,
                std::string name,
                rpcfn_http_type fn,
                bool okSafeMode,
                std::vector<std::string> argNames)
        : CRPCCommand{std::move(category),
                      std::move(name),
                      okSafeMode,
                      true,
                      true,
                      std::move(argNames)}
    {
        actor.http_fn = fn;
    }

    UniValue call(Config&,
                  const JSONRPCRequest&,
                  HTTPRequest* httpReq = nullptr,
                  bool processedInBatch = true) const;
};

/**
 * MVC RPC command dispatcher.
 */
class CRPCTable {
private:
    std::map<std::string, const CRPCCommand *> mapCommands;

public:
    CRPCTable();

    const CRPCCommand *operator[](const std::string &name) const;

    std::string help(Config &config, const std::string &name,
                     const JSONRPCRequest &helpReq) const;

    /**
     * Execute a method.
     * @param request The JSONRPCRequest to execute
     * @param httpReq The httpRequest to handle
     * @param processedInBatch If true, write response in multiple chunks
     * @throws an exception (JSONRPCError) when an error happens.
     */
    void execute(Config &config, const JSONRPCRequest &,
                 HTTPRequest *httpReq = nullptr,
                 bool processedInBatch = true) const;

    /**
     * Returns a list of registered commands
     * @returns List of registered commands.
     */
    std::vector<std::string> listCommands() const;

    /**
     * Appends a CRPCCommand to the dispatch table.
     * Returns false if RPC server is already running (dump concurrency
     * protection).
     * Commands cannot be overwritten (returns false).
     */
    bool appendCommand(const std::string &name, const CRPCCommand *pcmd);
};

extern CRPCTable tableRPC;

/**
 * Utilities: convert hex-encoded Values
 * (throws error if not hex).
 */
extern uint256 ParseHashV(const UniValue &v, std::string strName);
extern uint256 ParseHashO(const UniValue &o, std::string strKey);
extern std::vector<uint8_t> ParseHexV(const UniValue &v, std::string strName);
extern std::vector<uint8_t> ParseHexO(const UniValue &o, std::string strKey);

extern Amount AmountFromValue(const UniValue &value);
extern UniValue ValueFromAmount(const Amount &amount);
extern std::string HelpExampleCli(const std::string &methodname,
                                  const std::string &args);
extern std::string HelpExampleRpc(const std::string &methodname,
                                  const std::string &args);

bool StartRPC();
void InterruptRPC();
void StopRPC();
void JSONRPCExecBatch(Config &config, const JSONRPCRequest &req,
                             const UniValue &vReq, HTTPRequest& httpReq);
void RPCNotifyBlockChange(bool ibd, const CBlockIndex *);

// Retrieves any serialization flags requested in command line argument
int RPCSerializationFlags();

#endif // MVC_RPCSERVER_H
