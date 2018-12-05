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
#include "tma_service.hpp"
#include "tma_service_manager.hpp"

u32 TmaService::GetNextTaskId() {
    return this->manager->GetNextTaskId();
}

void TmaService::OnSleep() {
    /* Default service does nothing here. */
}

void TmaService::OnWake() {
    /* Default service does nothing here. */
}