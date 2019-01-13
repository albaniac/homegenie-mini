/*
 * HomeGenie-Mini (c) 2018-2019 G-Labs
 *
 *
 * This file is part of HomeGenie-Mini (HGM).
 *
 *  HomeGenie-Mini is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HomeGenie-Mini is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HomeGenie-Mini.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Authors:
 * - Generoso Martello <gene@homegenie.it>
 *
 *
 * Releases:
 * - 2019-13-01 Initial release
 *
 */

#include "TaskManager.h"

static Task *taskList = NULL, *currentTask = NULL;

TaskManager::TaskManager() {
    taskList = NULL;
    currentTask = NULL;
}

void TaskManager::loop() {
    IO::Logger::verbose("Tasker loop() >> BEGIN");
    Task *t = taskList;
    uint c = 0;
    while (t != NULL) {
        IO::Logger::verbose("- running task %d", c++);
        t->loop();
        t = t->nextTask;
    }
    IO::Logger::verbose("Tasker loop() << END");
}

void TaskManager::addTask(Task *task) {
    if (taskList == NULL) {
        taskList = currentTask = task;
        taskList->nextTask = NULL;
        taskList->previousTask = NULL;
    } else {
        currentTask->nextTask = task;
        task->previousTask = currentTask;
        task->nextTask = NULL;
        currentTask = task;
    }
}