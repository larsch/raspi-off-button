#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#define PIN RPI_GPIO_P1_12
static const int DEFAULT_TIME = 2000;
static const int DEFAULT_FREQ = 250;
static const char* DEFAULT_COMMAND = "/sbin/poweroff";
int main(int argc, char **argv)
{
    int count = 0;
    int wait_time = DEFAULT_TIME;
    int poll_freq = DEFAULT_FREQ;
    int pin = PIN;
    int high = 0;
    const char* command = DEFAULT_COMMAND;

    while (1) {
        int option_index = 0;
        int c;
        static struct option long_options[] = {
            { "time", required_argument, 0, 't' },
            { "freq", required_argument, 0, 'f' },
            { "command", required_argument, 0, 'c' },
            { "gpio", required_argument, 0, 'g' },
            { "high", no_argument, 0, 'i' },
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 }
        };
        c = getopt_long(argc, argv, "t:f:c:g:ih?", long_options, &option_index);
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
                high = 1;
                break;
            case '?':
            case 'h':
                printf("Usage: raspi-off-button [OPTIONS]\n");
                printf("  -t, --time     Time the input must be active (milliseconds, default %d)\n", DEFAULT_TIME);
                printf("  -f, --freq     Polling frequency (milliseconds, default %d)\n", DEFAULT_FREQ);
                printf("  -c, --command  Command to run (default \"%s\")\n", DEFAULT_COMMAND);
                printf("  -g, --gpio     GPIO number (default %d)\n", PIN);
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Unhandled option: %c\n", c);
                return EXIT_FAILURE;
        }
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
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
    while (1)
    {
        uint8_t value = bcm2835_gpio_lev(pin);
        value ^= high;
	if (!value) count += poll_freq; else count = 0;
        if (count >= wait_time) {
		system(command);
		break;
	}
        delay(poll_freq);
    }
    bcm2835_close();
    return 0;
}
