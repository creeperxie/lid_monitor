// lid_monitor - a program to monitor lid switch event
// Copyright (C) 2024  Snientals
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>

#include <libudev.h>
#include <libinput.h>

static int open_restriced(const char *path, int flags, void *) {
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *) {
    close(fd);
}

static const struct libinput_interface interface = {
    .open_restricted = open_restriced,
    .close_restricted = close_restricted,
};

static void handle_switch_event(struct libinput_event *ev) {
    struct libinput_event_switch *sw = libinput_event_get_switch_event(ev);
    switch (libinput_event_switch_get_switch(sw)) {
        case LIBINPUT_SWITCH_LID:
            printf("switch lid state: %d\n",
                libinput_event_switch_get_switch_state(sw));
            break;
        default:
            break;
    }
}

static void handle_events(struct libinput *li) {
    struct libinput_event *ev;
    libinput_dispatch(li);
    while ((ev = libinput_get_event(li))) {
        switch (libinput_event_get_type(ev)) {
            case LIBINPUT_EVENT_SWITCH_TOGGLE:
                handle_switch_event(ev);
                break;
            default:
                break;
        }
        libinput_event_destroy(ev);
    }
}

int main() {
    struct udev *udev = udev_new();

    struct libinput *li = libinput_udev_create_context(&interface, NULL, udev);
    libinput_udev_assign_seat(li, "seat0");

    struct pollfd fds;
    fds.fd = libinput_get_fd(li);
    fds.events = POLLIN;
    fds.revents = 0;

    for (;;) {
        if (poll(&fds, 1, -1) > -1) {
            handle_events(li);
        }
    }
}
