#include "roboclawmain.h"
#include "ui_roboclawmain.h"

#include <roboclaw/io/read_commands.hpp>

namespace read_commands = roboclaw::io::read_commands;

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_t)
{
    namespace expr = boost::log::expressions;
    logger_t lg;
    boost::log::add_common_attributes();
    boost::log::add_file_log(
                boost::log::keywords::file_name = "log.txt",
                boost::log::keywords::format = (
                        expr::stream << expr::format_date_time<     boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                        << " [" << expr::attr<     boost::log::trivial::severity_level >("Severity") << "] "
                        << expr::smessage
            )
    );
    boost::log::add_console_log(
                std::cout,
                boost::log::keywords::format = (
                        expr::stream << expr::format_date_time<     boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                        << " [" << expr::attr<     boost::log::trivial::severity_level >("Severity") << "] "
                        << expr::smessage
            )
    );
    return lg;
}

RoboclawMain::RoboclawMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RoboclawMain),
    controller("/dev/ttyACM0", 0x80)
{
    ui->setupUi(this);
    setFixedSize(minimumSizeHint().width(), minimumSizeHint().height());

    QTimer* controllerInfoTimer = new QTimer(this);
    QObject::connect(controllerInfoTimer, &QTimer::timeout,
                     this, &RoboclawMain::updateSerialData);
    controllerInfoTimer->start(500);
}

RoboclawMain::~RoboclawMain()
{
    delete ui;
}

void RoboclawMain::updateSerialData()
{
    ui->firmwareVersionEdit->setText(QString::fromStdString(
                    controller.read<read_commands::firmware_version>()));

    ui->t1Edit->setText(QString::fromStdString(
                read_commands::get_string(
                    controller.read<read_commands::board_temperature_1>())));
    ui->t2Edit->setText(QString::fromStdString(
                read_commands::get_string(
                    controller.read<read_commands::board_temperature_2>())));
}
