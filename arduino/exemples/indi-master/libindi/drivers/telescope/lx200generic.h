/*
    LX200 Generic
    Copyright (C) 2003-2015 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#pragma once

#include "indiguiderinterface.h"
#include "inditelescope.h"

class LX200Generic : public INDI::Telescope, public INDI::GuiderInterface
{
  public:
    LX200Generic();
    virtual ~LX200Generic();

    /**
     * \struct LX200Capability
     * \brief Holds properties of LX200 Generic that might be used by child classes
     */
    enum
    {
        LX200_HAS_FOCUS             = 1 << 0, /** Define focus properties */
        LX200_HAS_TRACKING_FREQ     = 1 << 1, /** Define Tracking Frequency */
        LX200_HAS_ALIGNMENT_TYPE    = 1 << 2, /** Define Alignment Type */
        LX200_HAS_SITES             = 1 << 3, /** Define Sites */
        LX200_HAS_PULSE_GUIDING     = 1 << 4, /** Define Pulse Guiding */
        LX200_HAS_TRACK_MODE        = 1 << 5  /** Define Track Mode */
    } LX200Capability;

    uint32_t getLX200Capability() const { return genericCapability; }
    void setLX200Capability(uint32_t cap) { genericCapability = cap; }

    virtual const char *getDefaultName();
    virtual const char *getDriverName();
    virtual bool Handshake();
    virtual bool ReadScopeStatus();
    virtual void ISGetProperties(const char *dev);
    virtual bool initProperties();
    virtual bool updateProperties();
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n);
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n);
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);

    void updateFocusTimer();
    void guideTimeout();

  protected:
    // Slew Rate
    virtual bool SetSlewRate(int index);
    // Track Mode (Sidereal, Solar..etc)
    virtual bool SetTrackMode(int mode);

    // NSWE Motion Commands
    virtual bool MoveNS(INDI_DIR_NS dir, TelescopeMotionCommand command);
    virtual bool MoveWE(INDI_DIR_WE dir, TelescopeMotionCommand command);

    // Abort ALL motion
    virtual bool Abort();

    // Time and Location
    virtual bool updateTime(ln_date *utc, double utc_offset);
    virtual bool updateLocation(double latitude, double longitude, double elevation);

    // Guide Commands
    virtual IPState GuideNorth(float ms);
    virtual IPState GuideSouth(float ms);
    virtual IPState GuideEast(float ms);
    virtual IPState GuideWest(float ms);

    // Guide Pulse Commands
    virtual int SendPulseCmd(int direction, int duration_msec);

    // Goto
    virtual bool Goto(double ra, double dec);

    // Is slew over?
    virtual bool isSlewComplete();

    // Park Mount
    virtual bool Park();

    // Sync coordinates
    virtual bool Sync(double ra, double dec);

    // Check if mount is responsive
    virtual bool checkConnection();

    // Save properties in config file
    virtual bool saveConfigItems(FILE *fp);

    // Action to perform when Debug is turned on or off
    virtual void debugTriggered(bool enable);

    // Initial function to get data after connection is successful
    virtual void getBasicData();

    // Send slew error message to client
    void slewError(int slewCode);

    // Get mount alignment type (AltAz..etc)
    void getAlignment();

    // Send Mount time and location settings to client
    void sendScopeTime();
    void sendScopeLocation();

    // Simulate Mount in simulation mode
    void mountSim();

    static void updateFocusHelper(void *p);
    static void guideTimeoutHelper(void *p);

    int GuideNSTID;
    int GuideWETID;

    int timeFormat;
    int currentSiteNum;
    int trackingMode;
    long guide_direction;

    unsigned int DBG_SCOPE;

    double JD;
    double targetRA, targetDEC;
    double currentRA, currentDEC;
    int MaxReticleFlashRate;

    /* Telescope Alignment Mode */
    ISwitchVectorProperty AlignmentSP;
    ISwitch AlignmentS[3];

    /* Tracking Mode */
    ISwitchVectorProperty TrackModeSP;
    ISwitch TrackModeS[4];

    /* Tracking Frequency */
    INumberVectorProperty TrackingFreqNP;
    INumber TrackFreqN[1];

    /* Use pulse-guide commands */
    ISwitchVectorProperty UsePulseCmdSP;
    ISwitch UsePulseCmdS[2];

    /* Site Management */
    ISwitchVectorProperty SiteSP;
    ISwitch SiteS[4];

    /* Site Name */
    ITextVectorProperty SiteNameTP;
    IText SiteNameT[1];

    /* Focus motion */
    ISwitchVectorProperty FocusMotionSP;
    ISwitch FocusMotionS[2];

    /* Focus Timer */
    INumberVectorProperty FocusTimerNP;
    INumber FocusTimerN[1];

    /* Focus Mode */
    ISwitchVectorProperty FocusModeSP;
    ISwitch FocusModeS[3];

    uint32_t genericCapability;

};
