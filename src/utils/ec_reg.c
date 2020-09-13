//
// Created by arnold on 4/24/2018.
//

#include "ec_reg.h"
#include "../sdk/hisi_11/memmap.h"
#include "../log/ec_log.h"


EC_VOID ec_reg_set (EC_UINT phy_addr, EC_UINT value)
{
    EC_VOID *addr = EC_NULL;

    addr = memmap(phy_addr, 4);
    if (!addr)
    {
        dzlog_fatal("memmap failed");
        return;
    }
    *(EC_UINT *)addr = value;


    memunmap(addr);

    return;
}