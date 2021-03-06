// Copyright (c) 2013 The Bitcoin Core developers
// Copyright (c) 2016 Tom Zander <tomz@freedommail.ch>
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "transaction_utils.h"
#include <primitives/transaction.h>
#include <random.h>

#include <boost/atomic.hpp>

void TxUtils::RandomScript(CScript &script) {
    static const opcodetype oplist[] = {OP_FALSE, OP_1, OP_2, OP_3, OP_CHECKSIG, OP_IF, OP_VERIF, OP_RETURN, OP_CODESEPARATOR};
    script = CScript();
    int ops = (insecure_rand() % 10);
    for (int i=-5; i<ops; i++)
        script << oplist[insecure_rand() % (sizeof(oplist)/sizeof(oplist[0]))];
}

void TxUtils::RandomInScript(CScript &script) {
    script = CScript();
    int ops = 1 + (insecure_rand() % 5);
    for (int i = 0; i < ops; ++i) {
        int type = (insecure_rand() % 10);
        if (type == 0) {
            script << OP_FALSE;
        } else {
            std::vector<unsigned char> signature;
            const int length = (type == 1 ? 50 : 1) + insecure_rand() % 100;
            signature.reserve(length);
            GetRandBytes(&signature[0], length);
            script << signature;
        }
    }
}

void TxUtils::RandomTransaction(CMutableTransaction &tx, RandomTransactionType single) {
    tx.nVersion = 1;
    tx.vin.clear();
    tx.vout.clear();
    tx.nLockTime = (insecure_rand() % 2) ? insecure_rand() : 0;
    int ins = (insecure_rand() % 4) + 1;
    int outs = single == SingleOutput ? 1 : (insecure_rand() % 4) + 1;
    for (int in = 0; in < ins; in++) {
        tx.vin.push_back(CTxIn());
        CTxIn &txin = tx.vin.back();
        txin.prevout.hash = GetRandHash();
        txin.prevout.n = insecure_rand() % 4;
        RandomInScript(txin.scriptSig);
        txin.nSequence = (insecure_rand() % 2) ? (insecure_rand() & 0x40FFFF) : (unsigned int)-1;
    }
    for (int out = 0; out < outs; out++) {
        tx.vout.push_back(CTxOut());
        CTxOut &txout = tx.vout.back();
        txout.nValue = insecure_rand() % 100000000;
        RandomScript(txout.scriptPubKey);
    }
}

extern boost::atomic<bool> flexTransActive;
void TxUtils::allowNewTransactions()
{
    flexTransActive.store(true);
}

void TxUtils::disallowNewTransactions()
{
    flexTransActive.store(false);
}
