// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "abort_node.h"
#include "ui_interface.h"
#include "warnings.h"
#include "init.h"
#include "logging.h"
#include "util.h"

/** Abort with a message */
bool AbortNode(const std::string &strMessage,
               const std::string &userMessage) {
    SetMiscWarning(strMessage);
    LogPrintf("*** %s\n", strMessage);
    uiInterface.ThreadSafeMessageBox(
        userMessage.empty() ? _("Error: A fatal internal error occurred, see "
                                "mvcd.log for details")
                            : userMessage,
        "", CClientUIInterface::MSG_ERROR);
    StartShutdown();
    return false;
}

bool AbortNode(CValidationState &state, const std::string &strMessage,
               const std::string &userMessage) {
    AbortNode(strMessage, userMessage);
    return state.Error(strMessage);
}
