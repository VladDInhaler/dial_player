#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <cctype>
#include <filesystem>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "structures/paths.hpp"
#include "structures/data_structures.hpp"
#include "functions/start_programs.hpp"
#include "functions/load_functs.hpp"
#include "functions/play_functs.hpp"
#include "functions/config_functs.hpp"

static constexpr uint8_t START_BYTE = 0x02;
static constexpr uint8_t END_BYTE   = 0x03;

//static const char* SERIAL_PORT = "/dev/ttyACM0"; // <-- change me
static constexpr int BAUD = 9600;                 // <-- change me


int open_serial(const char* device, int baud) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open_serial");
        return -1;
    }

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    // baud rate
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    // timeout-ish behavior
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1; // 0.1s

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

bool try_read_framed_number(int serial_fd, std::string& outAsciiNumber) {
    static bool inFrame = false;
    static std::string buf;

    uint8_t b;
    while (read(serial_fd, &b, 1) == 1) {
        if (!inFrame) {
            if (b == START_BYTE) {
                inFrame = true;
                buf.clear();
            }
        } else {
            if (b == END_BYTE) {
                inFrame = false;
                outAsciiNumber = buf;
                return true; // got one full frame
            } else {
                buf.push_back(static_cast<char>(b)); // ASCII bytes expected
            }
        }
    }
    return false;
}

inline bool play_from_serial_number(
    std::string const& serialNumber,
    std::vector<std::string> const& video_file_paths,
    std::string const& default_video_file_path
) {
    namespace fs = std::filesystem;

    // exact match by filename: "<serialNumber>.mp4"
    std::string wantedFile = serialNumber + ".mp4";

    for (auto const& fullPath : video_file_paths) {
        fs::path p(fullPath);
        if (p.filename() == wantedFile) {
            play_from_path(fullPath, default_video_file_path);
            return true;
        }
    }

    // no match -> play default
    play_from_path(default_video_file_path, default_video_file_path);
    return false;
}

int main() {
    bool program_running = true;
    std::cout << "  program running" << std::endl;

    load_cfg(folder_path, input_pipe_path);

    std::string default_video_file_path = load_default_video_file_path(folder_path);
    load_videos_from_folder(folder_path, video_file_paths);
    play_default(default_video_file_path);

    int serial_fd = open_serial(input_pipe_path.c_str(), BAUD);
    if (serial_fd < 0) {
        std::cerr << "Failed to open serial port.\n";
        return 1;
    }

    do {
        std::string input;
        if (try_read_framed_number(serial_fd, input)) {
            std::cout << input << std::endl;

            if (input == "") {
                // nothing
            }
            // playing video when input is all digits (any length)
            else if (std::all_of(input.begin(), input.end(),
                                  [](unsigned char c) { return std::isdigit(c) != 0; })) {
                play_from_serial_number(input, video_file_paths, default_video_file_path);
            }
            // easter egg: frame contains "N..." (same as your original logic)
            else if (input.size() > 1 && std::isdigit(static_cast<unsigned char>(input[0]))) {
                try_easter_Eggs(easter_egg_Names, easter_egg_Videos, input);
            }
            else {
                std::cout << "  invalid input, please try again" << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    } while (program_running);

    close(serial_fd);
    std::system("./kill_mpv.sh");
    std::cout << "   quitting program" << std::endl;
    return 0;
}


das projekt ist video player cpp
das arduino programm ist untitled.

der quellcode muss noch auf github gespeichert werden.



