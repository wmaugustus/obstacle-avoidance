#include "Lepton.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

Lepton::Lepton() {
}

Lepton::~Lepton() {
}

void Lepton::run() {

    cv::Mat frame(80, 60, CV_8UC1);

    //open spi port
    SpiOpenPort(0);

    while (true) {
        //read data packets from lepton over SPI
        int resets = 0;
        for (int j = 0; j < PACKETS_PER_FRAME; j++) {
            //if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
            read(spi_cs0_fd, result + sizeof(uint8_t) * PACKET_SIZE * j, sizeof(uint8_t) * PACKET_SIZE);
            int packetNumber = result[j * PACKET_SIZE + 1];
            if (packetNumber != j) {
                j = -1;
                resets += 1;
                usleep(1000);
                if (resets == 750) {
                    SpiClosePort(0);
                    usleep(750000);
                    SpiOpenPort(0);
                }
            }
        }

        frameBuffer = (uint16_t *) result;
        int row, column;
        uint16_t value;
        uint16_t minValue = 65535;
        uint16_t maxValue = 0;


        for (int i = 0; i < FRAME_SIZE_UINT16; i++) {
            //skip the first 2 uint16_t's of every packet, they're 4 header bytes
            if (i % PACKET_SIZE_UINT16 < 2) {
                continue;
            }

            //flip the MSB and LSB at the last second
            int temp = result[i * 2];
            result[i * 2] = result[i * 2 + 1];
            result[i * 2 + 1] = temp;

            value = frameBuffer[i];
            if (value > maxValue) {
                maxValue = value;
            }
            if (value < minValue) {
                minValue = value;
            }
            column = i % PACKET_SIZE_UINT16 - 2;
            row = i / PACKET_SIZE_UINT16;
        }

        float diff = maxValue - minValue;
        float scale = 255 / diff;
        for (int i = 0; i < FRAME_SIZE_UINT16; i++) {
            if (i % PACKET_SIZE_UINT16 < 2) {
                continue;
            }

            value = (frameBuffer[i] - minValue) * scale;
//            const int *colormap = colormap_ironblack;
            column = (i % PACKET_SIZE_UINT16) - 2;
            row = i / PACKET_SIZE_UINT16;
            frame.at<uchar>(row, column, 0) = value;

        }

        imshow("Capture", frame);
    }

    //finally, close SPI port just bcuz
    SpiClosePort(0);
}