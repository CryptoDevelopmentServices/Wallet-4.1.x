// Copyright (c) 2019 The DeVault developers
// Copyright (c) 2019 Jon Spock
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
	
#include "upgrade_check.h"
#include <clientversion.h>
#include "util.h"
#include "ui_interface.h"
#include "alert.h"
static bool fWarned = false;
	
	// Keeps a running sum of peer versions and if more that 5 peers exist with
	// more than 25% at a higher version, logs a warning to the GUI/terminal to check for upgrades
	
	// This will work during startup while adding peers, but likely not respond to changing conditions of
	// peers
	
void ShouldUpgrade(const std::string& SubVer) {
	    struct PeerVersions {
	        int higher;
	        int others;
	    };
	
	    static PeerVersions sPeerVersions = {.higher = 0, .others = 0};
	    static int PeersCount = 0;
	    const double requiredRatioForUpgrade = 0.33;
	    const int requiredPeersForUpgrade = 5;
	    int PeerVersion = UnformatSubVersion(SubVer);
	
	    if (PeerVersion) {
	        PeersCount++;
	        if (PeerVersion > CLIENT_VERSION)  sPeerVersions.higher++;
	        else sPeerVersions.others++;
	    }
	    double ratio = (double)sPeerVersions.higher/PeersCount;
	    LogPrintf("%s : peer has version %d : , ratio = %g, count = %d\n",__func__,PeerVersion, ratio, PeersCount);
	    bool shouldupgrade =  (ratio > requiredRatioForUpgrade && PeersCount > requiredPeersForUpgrade);
	    if (shouldupgrade) {
	        std::string strWarning = "More than 33% or 2/5 of your peers are reporting a newer software version. "
	            "Please check for upgrades at http://github.com/deviant/deviant/releases "
	            "and for safety you can confirm release info by checking our twitter/discord/telegram";
                strMiscWarning = strWarning;

	    if (!fWarned) {
	          CAlert::Notify(strMiscWarning, true);
             fWarned = true;
	        }
	    }
}