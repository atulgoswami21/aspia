//
// PROJECT:         Aspia
// FILE:            client/ui/client_dialog.cc
// LICENSE:         GNU Lesser General Public License 2.1
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "client/ui/client_dialog.h"

#include "client/ui/desktop_config_dialog.h"
#include "client/client.h"
#include "codec/video_helpers.h"
#include "proto/computer.pb.h"

namespace aspia {

ClientDialog::ClientDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setFixedSize(size());
    SetDefaultConfig();

    ui.edit_address->setText(QString::fromUtf8(computer_.address().c_str(),
                                               computer_.address().size()));
    ui.spin_port->setValue(computer_.port());

    ui.combo_session_type->addItem(QIcon(":/icon/monitor-keyboard.png"),
                                   tr("Desktop Manage"),
                                   QVariant(proto::auth::SESSION_TYPE_DESKTOP_MANAGE));

    ui.combo_session_type->addItem(QIcon(":/icon/monitor.png"),
                                   tr("Desktop View"),
                                   QVariant(proto::auth::SESSION_TYPE_DESKTOP_VIEW));

    ui.combo_session_type->addItem(QIcon(":/icon/folder-stand.png"),
                                   tr("File Transfer"),
                                   QVariant(proto::auth::SESSION_TYPE_FILE_TRANSFER));

    ui.combo_session_type->addItem(QIcon(":/icon/system-monitor.png"),
                                   tr("System Information"),
                                   QVariant(proto::auth::SESSION_TYPE_SYSTEM_INFO));

    int current_session_type = ui.combo_session_type->findData(QVariant(computer_.session_type()));
    if (current_session_type != -1)
    {
        ui.combo_session_type->setCurrentIndex(current_session_type);
        OnSessionTypeChanged(current_session_type);
    }

    connect(ui.combo_session_type, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnSessionTypeChanged(int)));

    connect(ui.button_session_config, SIGNAL(pressed()),
            this, SLOT(OnSessionConfigButtonPressed()));

    connect(ui.button_connect, SIGNAL(pressed()),
            this, SLOT(OnConnectButtonPressed()));
}

void ClientDialog::OnSessionTypeChanged(int item_index)
{
    proto::auth::SessionType session_type = static_cast<proto::auth::SessionType>(
        ui.combo_session_type->itemData(item_index).toInt());

    switch (session_type)
    {
        case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
        case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
            ui.button_session_config->setEnabled(true);
            break;

        default:
            ui.button_session_config->setEnabled(false);
            break;
    }
}

void ClientDialog::OnSessionConfigButtonPressed()
{
    proto::auth::SessionType session_type = static_cast<proto::auth::SessionType>(
        ui.combo_session_type->currentData().toInt());

    switch (session_type)
    {
        case proto::auth::SESSION_TYPE_DESKTOP_MANAGE:
            DesktopConfigDialog(session_type,
                                computer_.mutable_desktop_manage_session(),
                                this).exec();
            break;

        case proto::auth::SESSION_TYPE_DESKTOP_VIEW:
            DesktopConfigDialog(session_type,
                                computer_.mutable_desktop_view_session(),
                                this).exec();
            break;

        default:
            break;
    }
}

void ClientDialog::OnConnectButtonPressed()
{
    proto::auth::SessionType session_type = static_cast<proto::auth::SessionType>(
        ui.combo_session_type->currentData().toInt());

    computer_.set_address(ui.edit_address->text().toUtf8());
    computer_.set_port(ui.spin_port->value());
    computer_.set_session_type(session_type);

    Client* client = new Client(computer_);

    connect(client, SIGNAL(clientTerminated()), this, SLOT(show()));
    connect(client, SIGNAL(clientTerminated()), client, SLOT(deleteLater()));

    hide();
}

void ClientDialog::SetDefaultConfig()
{
    computer_.set_port(kDefaultHostTcpPort);

    proto::desktop::Config* desktop_manage = computer_.mutable_desktop_manage_session();
    desktop_manage->set_flags(proto::desktop::Config::ENABLE_CLIPBOARD |
                              proto::desktop::Config::ENABLE_CURSOR_SHAPE);
    desktop_manage->set_video_encoding(proto::desktop::VideoEncoding::VIDEO_ENCODING_ZLIB);
    desktop_manage->set_update_interval(30);
    desktop_manage->set_compress_ratio(6);
    ConvertToVideoPixelFormat(PixelFormat::RGB565(), desktop_manage->mutable_pixel_format());

    proto::desktop::Config* desktop_view = computer_.mutable_desktop_view_session();
    desktop_view->set_flags(0);
    desktop_view->set_video_encoding(proto::desktop::VideoEncoding::VIDEO_ENCODING_ZLIB);
    desktop_view->set_update_interval(30);
    desktop_view->set_compress_ratio(6);
    ConvertToVideoPixelFormat(PixelFormat::RGB565(), desktop_view->mutable_pixel_format());

    computer_.set_session_type(proto::auth::SESSION_TYPE_DESKTOP_MANAGE);
}

} // namespace aspia
