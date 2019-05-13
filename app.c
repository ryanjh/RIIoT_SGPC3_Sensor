#include "spr_app.h"
#include "example_sgpc3_tvoc_sensor.h"

/********* Constants ***********/
#define MONITOR_TVOC_INTERVAL   (1) // minute
#define TRANSMITE_TVOC_INTERVAL (60) // minute
#define ALARM_TVOC              (20) // ppm

/********* Private Variables ****************/
// NOTE: Do NOT initialize a global variable at declaration:
// NOTE: Do NOT initialize an array (global or within a function) at declaration
static TimerId resetNetworkTimer;
static TimerId measureSensorTimer;

/********* Private Function Declarations ****************/
// These are event handlers that will be implemented later in the file, but
// are declared here so Setup() can reference them when registering events.
static void networkStateChangedHandler(NetworkState state);
static void resetNetwork(void);
static void measureSensorTVOC(void);
static void transmitSensorTVOC(uint16_t tvoc);

/********* Public Functions ****************/

/**
* Setup() is called by the framework on startup
*/
void Setup()
{
    Debug.printline("App v%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    Debug.printline("Hello World!");

    // Configure I2C (SGPC3 supports I2C fast mode)
    I2C.init(I2C_400KHZ);

    // Configure the network
    Network.setFreqBand(FREQ_868_MHZ);
    Network.setDataRate(DATA_RATE_50_KBPS);
    Network.enableChannel(16);
    Network.setNetworkStateChangeHandler(networkStateChangedHandler);
    Network.setAutoJoin(true);
    Node.setBatteryPowered(false);
    Network.setPollRate(0 * SECONDS);

    // timer to reset the network if rejoin is not successful within the duration
    resetNetworkTimer = Timer.create(ONE_SHOT, 10 * SECOND, resetNetwork);

    // The sensor shall be monitored every 1 minute. 
    measureSensorTimer = Timer.create(PERIODIC, MONITOR_TVOC_INTERVAL * MINUTE, measureSensorTVOC);

    // Sampling interval: low-power mode 2s, ultra-low power mode 30s
    if (SPR_OK == startSensor())
        Timer.start(measureSensorTimer);
}

/********* Private Functions ****************/
static void networkStateChangedHandler(NetworkState state)
{
    Debug.printf("Network State: %s\r\n", Network.getNetworkStateString());

    if (ONLINE == state || REJOINING == state)
        Debug.printline("PanId=%2x Ch=%d ShortAddr=%2x", Network.getPanId(), Network.getChannel(), Network.getShortAddress());
    
    if (REJOINING == state)
        Timer.start(resetNetworkTimer); // reset network after a time if rejoining doesn't work
    else
        Timer.stop(resetNetworkTimer);
}

static void resetNetwork(void)
{
    Debug.printline("Reset network");
    Network.leave();
}

static void measureSensorTVOC(void)
{
    static uint8_t measureCount = 0;
    uint16_t tvoc = 0;

    SPR_Status status = readSensor(SGPC3_MEASURE_AIR_QUALITY, &tvoc);
    Debug.printline("measureSensorTVOC: status=%x, tvoc=%d", status, tvoc);
    
    measureCount += MONITOR_TVOC_INTERVAL;
    if (measureCount >= TRANSMITE_TVOC_INTERVAL) {
        // Make periodic transmissions every 1 hours under normal operation.
        transmitSensorTVOC(tvoc);
        measureCount = 0; // reset counter
    } else if (SPR_OK == status && tvoc > ALARM_TVOC * 1000) {
        // If the VOC reading is out of the normal range, make an immediate “alarm” transmission.
        // The normal range is up to 20 ppm (20000 ppb).
        transmitSensorTVOC(tvoc);
    }
}

static void transmitSensorTVOC(uint16_t tvoc)
{
    Debug.printline("transmitSensorTVOC");
    // send sensor reading to concentrator if online
    if (Network.getNetworkState() == ONLINE) {
        uint8_t message[2];
        Util.pack_uint16_msb(message, 0, tvoc);
        SPR_Status status = Network.send(sizeof(message), message);
        Debug.printline("transmitSensorTVOC: Send status=%x", status);
    }
}
