// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "validationinterface.h"

static CMainSignals g_signals;

CMainSignals &GetMainSignals() {
    return g_signals;
}

void RegisterValidationInterface(CValidationInterface *pwalletIn) {
    using namespace boost::placeholders;
    g_signals.UpdatedBlockTip.connect(boost::bind( &CValidationInterface::UpdatedBlockTip, pwalletIn, _1, _2, _3));
    g_signals.TransactionAddedToMempool.connect(boost::bind( &CValidationInterface::TransactionAddedToMempool, pwalletIn, _1));
    g_signals.TransactionRemovedFromMempool.connect(boost::bind(&CValidationInterface::TransactionRemovedFromMempool, pwalletIn, _1, _2, _3));
    g_signals.TransactionRemovedFromMempoolBlock.connect(boost::bind(&CValidationInterface::TransactionRemovedFromMempoolBlock, pwalletIn, _1, _2));
    g_signals.TransactionAddedToMempool.connect(boost::bind( &CValidationInterface::TransactionAdded, pwalletIn, _1));
    g_signals.BlockConnected.connect(boost::bind( &CValidationInterface::BlockConnected, pwalletIn, _1, _2, _3));
    g_signals.BlockConnected2.connect(boost::bind( &CValidationInterface::BlockConnected2, pwalletIn, _1, _2));
    g_signals.BlockDisconnected.connect( boost::bind(&CValidationInterface::BlockDisconnected, pwalletIn, _1));
    g_signals.SetBestChain.connect( boost::bind(&CValidationInterface::SetBestChain, pwalletIn, _1));
    g_signals.Inventory.connect( boost::bind(&CValidationInterface::Inventory, pwalletIn, _1));
    g_signals.Broadcast.connect(boost::bind( &CValidationInterface::ResendWalletTransactions, pwalletIn, _1, _2));
    g_signals.BlockChecked.connect( boost::bind(&CValidationInterface::BlockChecked, pwalletIn, _1, _2));
    g_signals.ScriptForMining.connect(boost::bind(&CValidationInterface::GetScriptForMining, pwalletIn, _1));
    g_signals.NewPoWValidBlock.connect(boost::bind( &CValidationInterface::NewPoWValidBlock, pwalletIn, _1, _2));
    g_signals.InvalidTxMessageZMQ.connect(boost::bind( &CValidationInterface::InvalidTxMessageZMQ, pwalletIn, _1));
}

void UnregisterValidationInterface(CValidationInterface *pwalletIn) {
    using namespace boost::placeholders;
    g_signals.ScriptForMining.disconnect(boost::bind(&CValidationInterface::GetScriptForMining, pwalletIn, _1));
    g_signals.BlockChecked.disconnect( boost::bind(&CValidationInterface::BlockChecked, pwalletIn, _1, _2));
    g_signals.Broadcast.disconnect(boost::bind( &CValidationInterface::ResendWalletTransactions, pwalletIn, _1, _2));
    g_signals.Inventory.disconnect( boost::bind(&CValidationInterface::Inventory, pwalletIn, _1));
    g_signals.SetBestChain.disconnect( boost::bind(&CValidationInterface::SetBestChain, pwalletIn, _1));
    g_signals.TransactionAddedToMempool.disconnect(boost::bind( &CValidationInterface::TransactionAddedToMempool, pwalletIn, _1));
    g_signals.TransactionRemovedFromMempool.disconnect(boost::bind(&CValidationInterface::TransactionRemovedFromMempool, pwalletIn, _1, _2, _3));
    g_signals.TransactionRemovedFromMempoolBlock.disconnect(boost::bind(&CValidationInterface::TransactionRemovedFromMempoolBlock, pwalletIn, _1, _2));
    g_signals.TransactionAddedToMempool.disconnect(boost::bind( &CValidationInterface::TransactionAdded, pwalletIn, _1));
    g_signals.BlockConnected.disconnect(boost::bind( &CValidationInterface::BlockConnected, pwalletIn, _1, _2, _3));
    g_signals.BlockConnected2.disconnect(boost::bind(&CValidationInterface::BlockConnected2, pwalletIn, _1, _2));
    g_signals.BlockDisconnected.disconnect( boost::bind(&CValidationInterface::BlockDisconnected, pwalletIn, _1));
    g_signals.UpdatedBlockTip.disconnect(boost::bind( &CValidationInterface::UpdatedBlockTip, pwalletIn, _1, _2, _3));
    g_signals.NewPoWValidBlock.disconnect(boost::bind( &CValidationInterface::NewPoWValidBlock, pwalletIn, _1, _2));
    g_signals.InvalidTxMessageZMQ.disconnect(boost::bind( &CValidationInterface::InvalidTxMessageZMQ, pwalletIn, _1));
}

void UnregisterAllValidationInterfaces() {
    g_signals.BlockChecked.disconnect_all_slots();
    g_signals.Broadcast.disconnect_all_slots();
    g_signals.Inventory.disconnect_all_slots();
    g_signals.SetBestChain.disconnect_all_slots();
    g_signals.TransactionAddedToMempool.disconnect_all_slots();
    g_signals.TransactionRemovedFromMempool.disconnect_all_slots();
    g_signals.TransactionRemovedFromMempoolBlock.disconnect_all_slots();
    g_signals.BlockConnected.disconnect_all_slots();
    g_signals.BlockConnected2.disconnect_all_slots();
    g_signals.ScriptForMining.disconnect_all_slots();
    g_signals.BlockDisconnected.disconnect_all_slots();
    g_signals.UpdatedBlockTip.disconnect_all_slots();
    g_signals.NewPoWValidBlock.disconnect_all_slots();
    g_signals.InvalidTxMessageZMQ.disconnect_all_slots();
}
