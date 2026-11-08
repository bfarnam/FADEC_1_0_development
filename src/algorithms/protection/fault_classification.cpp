#include "fault_classification.h"

#include "../../repositories/fault_status.h"

void faultClassificationInit()
{
    g_faultStatus.highestSeverity = FaultSeverity::NONE;
    g_faultStatus.faultPresent = false;
    g_faultStatus.criticalFaultPresent = false;
}

void faultClassificationRun()
{
    g_faultStatus.faultPresent =
        (g_faultStatus.activeFaults != 0U);

    g_faultStatus.highestSeverity =
        FaultSeverity::NONE;

    g_faultStatus.criticalFaultPresent = false;

   if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::RPM_INVALID))
    {
        if (g_faultStatus.highestSeverity < FaultSeverity::CRITICAL)
        {
            g_faultStatus.highestSeverity =
                FaultSeverity::CRITICAL;
        }

        g_faultStatus.criticalFaultPresent = true;
    }
   
    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::EGT_INVALID))
    {
        if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
        {
            g_faultStatus.highestSeverity =
                FaultSeverity::WARNING;
        }
    }
    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::THROTTLE_INVALID))
    {
        if (g_faultStatus.highestSeverity <
            FaultSeverity::WARNING)
        {
            g_faultStatus.highestSeverity =
                FaultSeverity::WARNING;
        }
    }
    
    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::OVERSPEED))
    {
        if (g_faultStatus.highestSeverity < FaultSeverity::CRITICAL)
        {
            g_faultStatus.highestSeverity =
                FaultSeverity::CRITICAL;
        }

        g_faultStatus.criticalFaultPresent = true;
    }

     if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::OVERTEMPERATURE))
    {
        if (g_faultStatus.highestSeverity < FaultSeverity::CRITICAL)
        {
            g_faultStatus.highestSeverity =
                FaultSeverity::CRITICAL;
        }

        g_faultStatus.criticalFaultPresent = true;
    } 
     

    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::FLAME_OUT))
        {
            if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        }

    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::UNDERSPEED))
        {
            if (g_faultStatus.highestSeverity <
                FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        }

    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::START_TIMEOUT))
        {
            if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        }
    
    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::IGNITION_TIMEOUT))
        {
            if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        } 

    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::ACCELERATION_TIMEOUT))
        {
            if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        }

    
    if (faultIsActive(
        g_faultStatus.activeFaults,
        FaultId::SHUTDOWN_TIMEOUT))
        {
            if (g_faultStatus.highestSeverity < FaultSeverity::WARNING)
            {
                g_faultStatus.highestSeverity =
                    FaultSeverity::WARNING;
            }
        }

}