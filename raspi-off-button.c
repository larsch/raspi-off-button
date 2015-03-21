#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#define DEFAULT_PIN RPI_GPIO_P1_07

static const int DEFAULT_TIME = 1000;
static const int DEFAULT_FREQ = 25;
static const char* DEFAULT_COMMAND = "/sbin/poweroff";
int main(int argc, char **argv)
{
    int count = 0;
    int wait_time = DEFAULT_TIME;
    int poll_freq = DEFAULT_FREQ;
    int pin = DEFAULT_PIN;
    int invert = 0;
    int pud_mode = -1;
    int verbose = 0;
    const char* command = DEFAULT_COMMAND;

    while (1) {
        int option_index = 0;
        int c;
        static struct option long_options[] = {
            { "time", required_argument, 0, 't' },
            { "freq", required_argument, 0, 'f' },
            { "command", required_argument, 0, 'c' },
            { "pull", required_argument, 0, 'p' },
            { "gpio", required_argument, 0, 'g' },
            { "invert", no_argument, 0, 'i' },
            { "verbose", no_argument, 0, 'v' },
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 }
        };
        c = getopt_long(argc, argv, "t:f:c:g:ivh?", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 't':
                wait_time = atoi(optarg);
                break;
            case 'f':
                poll_freq = atoi(optarg);
                break;
            case 'c':
                command = strdup(optarg);
                break;
            case 'g':
                pin = atoi(optarg);
                break;
            case 'i':
                invert = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case '?':
            case 'h':
                printf("Usage: raspi-off-button [OPTIONS]\n");
                printf("  -t, --time     Time the input must be active (milliseconds, default %d)\n", DEFAULT_TIME);
                printf("  -f, --freq     Polling frequency (milliseconds, default %d)\n", DEFAULT_FREQ);
                printf("  -c, --command  Command to run (default \"%s\")\n", DEFAULT_COMMAND);
                printf("  -g, --gpio     GPIO number (default %d)\n", DEFAULT_PIN);
                printf("  -i, --invert   Invert input, i.e. pull up and wait for it to go low\n");
                printf("  -p, --pull     Pull up/down mode ('up', 'down', 'off', default is 'down' or 'up' if invert is enabled)\n");
                printf("  -v, --verbose  Verbose output\n");
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Unhandled option: %c\n", c);
                return EXIT_FAILURE;
        }
    }

    if (pud_mode == -1) {
        if (invert)
            pud_mode = BCM2835_GPIO_PUD_UP;
        else
        pud_mode = BCM2835_GPIO_PUD_DOWN;
        if (verbose) printf("Defaulting pull mode to %u (%s)\n", pud_mode, (pud_mode == BCM2835_GPIO_PUD_UP) ? "up" : "down");
    }

    if (wait_time < 0) {
        fprintf(stderr, "Invalid value for time option: %d\n", wait_time);
        return EXIT_FAILURE;
    }
    if (poll_freq < 0) {
        fprintf(stderr, "Invalid value for freq option: %d\n", poll_freq);
        return EXIT_FAILURE;
    }
    if (pin < 0 || pin > 255) {
        fprintf(stderr, "Invalid value for GPIO pin: %d\n", pin);
        return EXIT_FAILURE;
    }

    if (!bcm2835_init())
        return 1;
    if (verbose) printf("Setting pin %d to input mode\n", pin);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    if (verbose) printf("Setting pin %d pull mode to %d\n", pin, pud_mode);
    bcm2835_gpio_set_pud(pin, pud_mode);
    while (1)
    {
        uint8_t value = bcm2835_gpio_lev(pin);
        value ^= invert;
        if (value) count += poll_freq; else count = 0;
        if (value & verbose) printf("Pin %d is active\n", pin);
        if (count >= wait_time) {
                if (verbose) printf("Pin was active %d, running %s\n", count, command);
                system(command);
                break;
        }
        delay(poll_freq);
    }
    bcm2835_close();
    return 0;
}
