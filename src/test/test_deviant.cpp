// Copyright (c) 2011-2013 The Bitcoin Core developers
// Copyright (c) 2017-2019 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_TEST_MODULE dev Test Suite

#include "test_deviant.h"

#include "main.h"
#include "random.h"
#include "txdb.h"
#include "guiinterface.h"

#include <boost/test/unit_test.hpp>

CClientUIInterface uiInterface;

uint256 insecure_rand_seed = GetRandHash();
FastRandomContext insecure_rand_ctx(insecure_rand_seed);

extern bool fPrintToConsole;
extern void noui_connect();

BasicTestingSetup::BasicTestingSetup()
{
        RandomInit();
        ECC_Start();
        SetupEnvironment();
        fCheckBlockIndex = true;
        SelectParams(CBaseChainParams::MAIN);
}
BasicTestingSetup::~BasicTestingSetup()
{
        ECC_Stop();
}

TestingSetup::TestingSetup()
{
        ClearDatadirCache();
        pathTemp = GetTempPath() / strprintf("test_deviant_%lu_%i", (unsigned long)GetTime(), (int)(InsecureRandRange(100000)));
        boost::filesystem::create_directories(pathTemp);
        mapArgs["-datadir"] = pathTemp.string();
        pblocktree = new CBlockTreeDB(1 << 20, true);
        pcoinsdbview = new CCoinsViewDB(1 << 23, true);
        pcoinsTip = new CCoinsViewCache(pcoinsdbview);
        InitBlockIndex();
        {
            CValidationState state;
            bool ok = ActivateBestChain(state);
            BOOST_CHECK(ok);
        }
        nScriptCheckThreads = 3;
        for (int i=0; i < nScriptCheckThreads-1; i++)
            threadGroup.create_thread(&ThreadScriptCheck);
        RegisterNodeSignals(GetNodeSignals());
}

TestingSetup::~TestingSetup()
{
        UnregisterNodeSignals(GetNodeSignals());
        threadGroup.interrupt_all();
        threadGroup.join_all();
        UnloadBlockIndex();
        delete pcoinsTip;
        delete pcoinsdbview;
        delete pblocktree;
        boost::filesystem::remove_all(pathTemp);
}

[[noreturn]] void Shutdown(void* parg)
{
    std::exit(0);
}

[[noreturn]] void StartShutdown()
{
    std::exit(0);
}

bool ShutdownRequested()
{
  return false;
}