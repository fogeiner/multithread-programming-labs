#pragma once

#include "ClientRetranslator.h"
#include "DownloaderRetranslator.h"

class Retranslator: public ClientRetranslator, public DownloadRetranslator{
public:
};