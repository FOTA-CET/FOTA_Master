
enum class ECU {
    STM32,
    ATMEGA328P,
    ESP32
};

class fotaClient {
    public:
        bool flashECU(ECU ecuType, const std::string& file);
    private:
};