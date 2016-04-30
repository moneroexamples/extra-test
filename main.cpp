#include "src/MicroCore.h"
#include "src/CmdLineOptions.h"
#include "src/tools.h"

#include "ext/minicsv.h"
#include "ext/format.h"

#include <iostream>
#include <string>
#include <vector>

using boost::filesystem::path;

using namespace fmt;
using namespace std;


// without this it wont work. I'm not sure what it does.
// it has something to do with locking the blockchain and tx pool
// during certain operations to avoid deadlocks.

namespace epee {
    unsigned int g_test_dbg_lock_sleep = 0;
}


int main(int ac, const char* av[]) {

    // get command line options
    xmreg::CmdLineOptions opts {ac, av};

    auto help_opt = opts.get_option<bool>("help");

    // if help was chosen, display help text and finish
    if (*help_opt)
    {
        return 0;
    }

    // get other options
    auto bc_path_opt      = opts.get_option<string>("bc-path");
    auto testnet_opt      = opts.get_option<bool>("testnet");

    bool testnet        = *testnet_opt ;


    path blockchain_path;

    if (!xmreg::get_blockchain_path(bc_path_opt, blockchain_path))
    {
        // if problem obtaining blockchain path, finish.
        return 1;
    }


    print("Blockchain path: {:s}\n", blockchain_path);


    // enable basic monero log output
    xmreg::enable_monero_log();

    // create instance of our MicroCore
    xmreg::MicroCore mcore;

    // initialize the core using the blockchain path
    if (!mcore.init(blockchain_path.string()))
    {
        cerr << "Error accessing blockchain." << endl;
        return 1;
    }

    // get the high level cryptonote::Blockchain object to interact
    // with the blockchain lmdb database
    cryptonote::Blockchain& core_storage = mcore.get_core();

    // get the current blockchain height. Just to check
    // if it reads ok.
    uint64_t height = core_storage.get_current_blockchain_height();

    uint64_t start_height {1036765};

    if (start_height > height)
    {
        cerr << "Given height is greater than blockchain height" << endl;
        return 1;
    }

    for (uint64_t i = start_height; i < height; ++i)
    {
        cryptonote::block blk;

        try
        {
            blk = core_storage.get_db().get_block_from_height(i);
        }
        catch (std::exception& e)
        {
            cerr << e.what() << endl;
            continue;
        }

        string blk_time = xmreg::timestamp_to_str(blk.timestamp);


        // get all transactions in the block found
        // initialize the first list with transaction for solving
        // the block i.e. coinbase.
        list<cryptonote::transaction> txs {blk.miner_tx};
        list<crypto::hash> missed_txs;

        if (!mcore.get_core().get_transactions(blk.tx_hashes, txs, missed_txs))
        {
            cerr << "Cant find transactions in block: " << height << endl;
            return 1;
        }

        for (const cryptonote::transaction& tx : txs)
        {

            crypto::hash tx_hash = cryptonote::get_transaction_hash(tx);

            cout << "\nblk no: " << i << ", tx: " << tx_hash << endl;

            crypto::hash  payment_id;
            crypto::hash8 payment_id8;

           // cryptonote::tx_extra_field txe = tx.

            string extra_str {reinterpret_cast<const char*>(tx.extra.data()), tx.extra.size()};

            cout << " - extra_str: " << epee::string_tools::buff_to_hex_nodelimer(extra_str) << endl;

            xmreg::get_payment_id(tx, payment_id, payment_id8);

           // cout << " - paymet id:  " << payment_id << endl;
            //cout << " - paymet id8: " << payment_id8 << endl;


        } // for (const cryptonote::transaction& tx : txs)

    } // for (uint64_t i = 0; i < height; ++i)


    cout << "\nEnd of program." << endl;

    return 0;
}
