/*
 * Copyright (c) 2018 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <switch.h>
#include <stratosphere.hpp>

#include "settings_service.hpp"
#include "settings_task.hpp"

TmaTask *SettingsService::NewTask(TmaPacket *packet) {
    TmaTask *new_task = nullptr;
    switch (packet->GetCommand()) {
        case SettingsServiceCmd_GetSetting:
            {
                new_task = new GetSettingTask(this->manager);
            }
            break;
        default:
            new_task = nullptr;
            break;
    }
    if (new_task != nullptr) {
        new_task->SetServiceId(this->GetServiceId());
        new_task->SetTaskId(packet->GetTaskId());
        new_task->OnStart(packet);
        new_task->SetNeedsPackets(true);
    }
    
    return new_task;
}
