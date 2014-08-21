#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cutils/properties.h>
#include <cutils/android_reboot.h>
#include <unistd.h>

int reboot_main(int argc, char *argv[])
{
    int ret;
    size_t prop_len;
    char property_val[PROPERTY_VALUE_MAX];
    const char *cmd = "reboot";
    char *optarg = "";

    opterr = 0;
    do {
        int c;

        c = getopt(argc, argv, "p");

        if (c == EOF) {
            break;
        }

        switch (c) {
        case 'p':
            cmd = "shutdown";
            break;
        case '?':
            fprintf(stderr, "usage: %s [-p] [rebootcommand]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } while (1);

    if(argc > optind + 1) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc > optind)
        optarg = argv[optind];

    prop_len = snprintf(property_val, sizeof(property_val), "%s,%s", cmd, optarg);
    if (prop_len >= sizeof(property_val)) {
        fprintf(stderr, "reboot command too long: %s\n", optarg);
        exit(EXIT_FAILURE);
    }

    ret = property_set(ANDROID_RB_PROPERTY, property_val);
    if(ret < 0) {
        perror("reboot");
        exit(EXIT_FAILURE);
    }

    // Don't return early. Give the reboot command time to take effect
    // to avoid messing up scripts which do "adb shell reboot && adb wait-for-device"
    while(1) { pause(); }

    fprintf(stderr, "Done\n");
    return 0;
}
