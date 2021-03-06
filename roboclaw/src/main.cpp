#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

#include <boost/asio/serial_port.hpp>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

#include <roboclaw/io/io.hpp>
#include <roboclaw/io/read_commands.hpp>
#include <roboclaw/io/write_commands.hpp>

namespace po = boost::program_options;
namespace read_commands = roboclaw::io::read_commands;
namespace write_commands = roboclaw::io::write_commands;
using roboclaw::io::serial_controller;

volatile std::atomic<bool> interruption_requested(false);

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        interruption_requested = true;
    }
}

void interruption_point(serial_controller& controller)
{
    if (interruption_requested)
    {
        controller.write(write_commands::m1_m2_drive_duty{0});
        exit(EXIT_SUCCESS);
    }
}

void read_info(roboclaw::io::serial_controller& controller)
{
    std::cout << "Firmware version: "
              << controller.read<read_commands::firmware_version>();

    std::cout << "Main battery voltage: "
              << controller.read<read_commands::main_battery_voltage>() << std::endl;
    std::cout << "Logic battery voltage: "
              << controller.read<read_commands::logic_battery_voltage>() << std::endl;

    // auto motor_pwm = controller.read<read_commands::motor_pwm_values>();
    // std::cout << "Motor PWM values: " << motor_pwm.m1 << ", " << motor_pwm.m2 <<
    // std::endl;

    // auto motor_currents = controller.read<read_commands::motor_currents>();
    // std::cout << "Motor currents: " << motor_currents.m1 << ", " << motor_currents.m2
    // << std::endl;

    // auto main_battery_v_settings =
    // controller.read<read_commands::main_battery_voltage_settings>(); std::cout << "Main
    // battery voltage settings: " << main_battery_v_settings.min << ", " <<
    // main_battery_v_settings.max << std::endl;

    // auto logic_battery_v_settings =
    // controller.read<read_commands::logic_battery_voltage_settings>(); std::cout <<
    // "Logic battery voltage settings: " << logic_battery_v_settings.min << ", " <<
    // logic_battery_v_settings.max << std::endl;

    // auto temp_1 = controller.read<read_commands::board_temperature_1>();
    // std::cout << "Board temperature 1: " << temp_1 << std::endl;

    // auto temp_2 = controller.read<read_commands::board_temperature_2>();
    // std::cout << "Board temperature 2: " << temp_2 << std::endl;

    // auto status = controller.read<read_commands::status>();
    // std::cout << "Status" << std::endl << get_string(status);

    // std::cout << "Encoder mode: " <<
    // get_string(controller.read<read_commands::encoder_mode>()) << std::endl;

    // auto m1_current_limit = controller.read<read_commands::m1_current_limit>();
    // auto m2_current_limit = controller.read<read_commands::m1_current_limit>();
    // std::cout << "M1 current limit: min: " << m1_current_limit.min << ", max: " <<
    // m1_current_limit.max << std::endl; std::cout << "M2 current limit: min: " <<
    // m2_current_limit.min << ", max: " << m2_current_limit.max << std::endl;

    // std::cout << "M1 encoder " <<
    // get_string(controller.read<read_commands::m1_encoder_count>()) << std::endl;
    // std::cout << "M2 encoder " <<
    // get_string(controller.read<read_commands::m2_encoder_count>()) << std::endl;

    // std::cout << "M1 encoder speed: " <<
    // get_string(controller.read<read_commands::m1_encoder_speed>()) << std::endl;
    // std::cout << "M2 encoder speed: " <<
    // get_string(controller.read<read_commands::m2_encoder_speed>()) << std::endl;
    // std::cout << "M1 raw speed: " <<
    // get_string(controller.read<read_commands::m1_raw_speed>()) << std::endl; std::cout
    // << "M2 raw speed: " << get_string(controller.read<read_commands::m2_raw_speed>())
    // << std::endl; std::cout << "Instantaneous speed: " <<
    // get_string(controller.read<read_commands::motor_instantaneous_speed>()) <<
    // std::endl;

    // Writing
    controller.write(write_commands::m1_encoder_mode{true, false});
    controller.write(write_commands::m2_encoder_mode{true, false});

    // while (true)
    std::cout << "Encoder mode: "
              << get_string(controller.read<read_commands::encoder_mode>()) << std::endl;

    controller.write(write_commands::m1_velocity_pid{1.f, 0.5f, 0.25f, 42000});
    controller.write(write_commands::m2_velocity_pid{1.f, 0.5f, 0.25f, 42000});

    std::cout << "M1 velocity PID: "
              << get_string(controller.read<read_commands::m1_velocity_pid>())
              << std::endl;
    std::cout << "M2 velocity PID: "
              << get_string(controller.read<read_commands::m2_velocity_pid>())
              << std::endl;
}

bool setup_verbosity(const po::variables_map& vm)
{
    auto lg_roboclaw = spdlog::stdout_color_mt("roboclaw");
    spdlog::set_async_mode(8192);

    if (vm.count("verbosity"))
    {
        auto verbosity = vm["verbosity"].as<std::string>();
        if (verbosity == "disabled")
        {
            spdlog::set_level(spdlog::level::off);
        }
        else if (verbosity == "debug")
        {
            spdlog::set_level(spdlog::level::debug);
        }
        else if (verbosity == "info")
        {
            spdlog::set_level(spdlog::level::info);
        }
        else if (verbosity == "warning")
        {
            spdlog::set_level(spdlog::level::warn);
        }
        else if (verbosity == "error")
        {
            spdlog::set_level(spdlog::level::err);
        }
        else if (verbosity == "critical")
        {
            spdlog::set_level(spdlog::level::critical);
        }
        else
        {
            std::cout << "invalid verbosity value: " << verbosity << std::endl;
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    std::string port_name;
    int speed;
    float duty;
    std::string verbosity;

    // clang-format off
    po::options_description desc("Allowed options:");
    desc.add_options()
        ("help", "Print help message")
        ("verbosity,v", po::value<std::string>(&verbosity)->default_value("error"), "Verbosity level [trace, debug, info, warning, error, fatal]")
        ("port,p", po::value<std::string>(&port_name)->default_value("/dev/ttyACM0"),
            "Serial port to use to connect to roboclaw")
        ("speed,s", po::value<int>(&speed), "Drive motors with qpps.")
        ("duty,d", po::value<float>(&duty), "Drive motors with duty cycle [-1, 1]");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return EXIT_SUCCESS;
    }
    if (!setup_verbosity(vm))
    {
        std::cout << desc;
        return EXIT_FAILURE;
    }
    if (vm.count("speed") && vm.count("duty"))
    {
        std::cerr << "Select either 'speed (-s)', or 'duty (-d)', but not both"
                  << std::endl;
        return EXIT_FAILURE;
    }

    roboclaw::io::serial_controller controller(port_name, 0x80);
    read_info(controller);

    if (vm.count("duty"))
    {
        // controller.write(write_commands::m1_drive_duty{duty});
        // controller.write(write_commands::m2_drive_duty{duty});
        controller.write(write_commands::m1_m2_drive_duty(duty));
    }
    if (vm.count("speed"))
    {
        std::signal(SIGINT, signal_handler);
        while (true)
        {
            // controller.write(write_commands::m1_drive_qpps{speed});
            // controller.write(write_commands::m2_drive_qpps{-speed});
            controller.write(write_commands::m1_m2_drive_qpps{speed, speed});

            std::this_thread::sleep_for(std::chrono::seconds(2));
            // speed = -speed;
            interruption_point(controller);
        }
    }

    return EXIT_SUCCESS;
}
