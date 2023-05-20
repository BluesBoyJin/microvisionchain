// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Copyright (c) 2021-2023 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "noui.h"

#include "ui_interface.h"
#include "util.h"
#include <string>

static bool noui_ThreadSafeMessageBox(const std::string &message,
                                      const std::string &caption,
                                      unsigned int style) {
    bool fSecure = style & CClientUIInterface::SECURE;
    style &= ~CClientUIInterface::SECURE;

    std::string strCaption;
    // Check for usage of predefined caption
    switch (style) {
        case CClientUIInterface::MSG_ERROR:
            strCaption += _("Error");
            break;
        case CClientUIInterface::MSG_WARNING:
            strCaption += _("Warning");
            break;
        case CClientUIInterface::MSG_INFORMATION:
            strCaption += _("Information");
            break;
        default:
            // Use supplied caption (can be empty)
            strCaption += caption;
    }

    if (!fSecure) LogPrintf("%s: %s\n", strCaption, message);
    fprintf(stderr, "%s: %s\n", strCaption.c_str(), message.c_str());
    return false;
}

static bool
noui_ThreadSafeQuestion(const std::string & /* ignored interactive message */,
                        const std::string &message, const std::string &caption,
                        unsigned int style) {
    return noui_ThreadSafeMessageBox(message, caption, style);
}

static void noui_InitMessage(const std::string &message) {
    LogPrintf("init message: %s\n", message);
}

void noui_connect() {
    // Connect mvcd signal handlers
    uiInterface.ThreadSafeMessageBox.connect(noui_ThreadSafeMessageBox);
    uiInterface.ThreadSafeQuestion.connect(noui_ThreadSafeQuestion);
    uiInterface.InitMessage.connect(noui_InitMessage);
}