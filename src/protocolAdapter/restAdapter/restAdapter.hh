#include "../protocolAdapter.hh"

class restAdapter : public protocolAdapter {
    public:
        virtual void config() override = 0;
        virtual void checkConnect() override = 0;
        virtual void sendData() override = 0;
};