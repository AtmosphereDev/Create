#pragma once
#include "CServer.hpp"
#include "CCommon.hpp"
#include "CClient.hpp"

class AllConfigs {
private:
    static CServer _server;
    static CCommon _common;
    static CClient _client;

public:
    static CServer& server() {
        return _server;
    }

    static CCommon& common() {
        return _common;
    }

    static CClient& client() {
        return _client;
    }
};