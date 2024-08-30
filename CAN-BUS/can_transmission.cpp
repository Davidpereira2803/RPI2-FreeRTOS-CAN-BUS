#include <pigpio.h>
#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <chrono>

// Global variables
bool nottriggered = true;

std::chrono::high_resolution_clock::time_point start_time;
std::chrono::high_resolution_clock::time_point end_time;


// Function to send predefined 8bit data over CAN Bus
int can_send_transmission()
{
    struct sockaddr_can can_address;
    struct can_frame frame_can;
    struct ifreq ifr_req;
    int socket_addr;

    // create CAN socket
    socket_addr = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // select can0
    std::strcpy(ifr_req.ifr_name, "can0");
    ioctl(socket_addr, SIOCGIFINDEX, &ifr_req);
    can_address.can_family = AF_CAN;
    can_address.can_ifindex = ifr_req.ifr_ifindex;
    
    // bind the socket to CAN
    bind(socket_addr, (struct sockaddr *)&can_address, sizeof(can_address));

    /*
        data to be transmitted
        data id
        data length
    */
    frame_can.can_id = 0x2024;
    frame_can.can_dlc = 8;
    frame_can.data[0] = 0xAA;
    frame_can.data[1] = 0x11;
    frame_can.data[2] = 0xBB;
    frame_can.data[3] = 0x22;
    frame_can.data[4] = 0xCC;
    frame_can.data[5] = 0x33;
    frame_can.data[6] = 0xDD;
    frame_can.data[7] = 0x44;

    // write data to the socket
    write(socket_addr, &frame_can, sizeof(struct can_frame));

    std::cout << "Message: 0x" << std::hex << frame_can.can_id << "sent!" << std::endl;

    // close the socket
    close(socket_addr);

    // take the chrono time, when the transmission has finished.
    end_time = std::chrono::high_resolution_clock::now();

    // compute the transmission time (end_time - start_time)
    auto transmission_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "The CAN transmission took: " << transmission_time.count() << " microseconds" << std::endl;

    return 0;
}

// Function to receive data over CAN Bus
int can_receive_transmission()
{
    struct sockaddr_can can_address;
    struct can_frame frame_can;
    struct ifreq ifr_req;
    int socket_addr;

    // create CAN socket
    socket_addr = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    // select can0
    std::strcpy(ifr_req.ifr_name, "can0");
    ioctl(socket_addr, SIOCGIFINDEX, &ifr_req);
    can_address.can_family = AF_CAN;
    can_address.can_ifindex = ifr_req.ifr_ifindex;
    
    // bind the socket to CAN
    bind(socket_addr, (struct sockaddr *)&can_address, sizeof(can_address));

    // while loop to wait for incoming messages
    while (true)
    {
        // read the incoming data
        read(socket_addr, &frame_can, sizeof(struct can_frame));

        // display the received data
        std::cout << "Data received: " << std::endl;
        std::cout << "ID: 0x" << std::hex << frame_can.can_id << "Bytes: " << std::dec << std::endl;
        for (int i = 0; i < frame_can.can_dlc; i++) {
            std::cout << std::hex << (int)frame_can.data[i] << " ," << std::dec;
        }
        std::cout << std::endl;

        end_time = std::chrono::high_resolution_clock::now();

        // compute the transmission time (end_time - start_time)
        auto transmission_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "The CAN transmission took: " << transmission_time.count() << " microseconds" << std::endl;
    }
    

    // close the socket
    close(socket_addr);

    return 0;
}

// Function to react when the button is called
void press_button()
{
    std::cout << "Button pressed, CAN and timer started!" << std::endl;

    // set the value of the variable to false
    nottriggered = false;
}

// Function to isolate a core
void* isolate_core(void* arg)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    // set the affinity
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    /*
        Loop through each CPU core and check if the current cpu is set(isolated)
        if so the core_iod is that core
    */
    int core_id = -1;
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            core_id = i;
            break;
        }
    }

    // code to be executed on the isolated core

    // start the timer
    start_time = std::chrono::high_resolution_clock::now();
    can_receive_transmission();
    //can_send_transmission();

    // exit the isolated core
    pthread_exit(nullptr);
}

// Function to initiate the core isolation
int init_core()
{
    pthread_t thread;
    cpu_set_t cpuset;

    // Isolate core 1
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);  

    // Create the thread, link the core to the isolate_core function
    pthread_create(&thread, nullptr, isolate_core, nullptr);

    // Set the CPU affinity for the thread
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

    pthread_join(thread, nullptr);
    return 0;
}

/*
    Main function, which initializes the gpio pins,
    and waits for the button to be pressed to call init_core
*/
int main()
{
    // Initialise the GPIO pins
    gpioInitialise();

    // Set the GPIO pin 18 as button input and link the button function to it
    gpioSetMode(18, PI_INPUT);
    gpioSetPullUpDown(18, PI_PUD_UP);
    gpioSetAlertFunc(18, press_button);

    std::cout << "Press the button to start the CAN Communication and the timer!" << std::endl;

    while (nottriggered){}

    // call the init_core() function to isolate the core and send/receive the data
    init_core();

    // Terminate the GPIO pins
    gpioTerminate();
}