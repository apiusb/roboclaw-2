#include <roboclaw/io/read_commands/encoder_speed.hpp>

namespace roboclaw
{
namespace io
{
namespace read_commands
{
template<uint8_t command_id>
encoder_speed_values encoder_speed_base<command_id>::read_response(
        boost::asio::serial_port& port, crc_calculator_16& crc,
        std::string& log_str)
{
    return_type r;

    r.speed = read_value<int32_t>(port, crc, log_str);
    r.forward = !(read_value<uint8_t>(port, crc, log_str) & 0x01);

    return r;
}

std::string get_string(const encoder_speed_values& m)
{
    std::stringstream ss;
    ss << "speed: " << m.speed << " " << (m.forward ? "[forward]" : "[backward]");
    return ss.str();
}

template struct encoder_speed_base<M1_ENCODER_SPEED_CMD>;
template struct encoder_speed_base<M2_ENCODER_SPEED_CMD>;
template struct encoder_speed_base<M1_RAW_SPEED_CMD>;
template struct encoder_speed_base<M2_RAW_SPEED_CMD>;

}  // namespace read_commands
}  // namespace io
}  // namespace roboclaw
