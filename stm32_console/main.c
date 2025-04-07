#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#define SERIAL_PORT "/dev/ttyUSB0" //à adapter

int setup_serial(const char *port){
    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror("Unable to open serie port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_iflag = IGNBRK;

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int write_serial_port(int fd, const char *data, size_t size){
    return write(fd, data, size);
}

void print_interface(){
    printf("\nCommandes disponibles : \n");
    printf(" LED1 ON / OFF\n");
    printf(" LED2 ON / OFF\n");
    printf(" LED3 ON / OFF\n");
    printf(" CHENILLARD1 ON / OFF\n");
    printf(" CHENILLARD2 ON / OFF\n");
    printf(" CHENILLARD3 ON / OFF\n");
    printf(" CHENILLARD FREQUENCE1 / FREQUENCE2 / FREQUENCE3\n");
    printf("Q / q / quit : Quitter l'application\n");
    printf("H / h / help : Afficher la liste des commandes\n");
    printf("C / c / clear : efface le terminal\n");
}

bool check_command(){

}

int main(int argc, char **argv){
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <serial_port>\n", argv[0]);
        return 1;
    }

    int fd = setup_serial(argv[1]);
    if (fd < 0) return 1;

    char buffer[256];
    print_interface();
    
    while(1){
        printf("> ");
        
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) continue;

        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0 || strcmp(buffer, "quit") == 0) break;
        if (strcmp(buffer, "h") == 0 || strcmp(buffer, "H") == 0 || strcmp(buffer, "help") == 0){
            print_interface();
            continue;
        }
        if (strcmp(buffer, "c") == 0 || strcmp(buffer, "C") == 0 || strcmp(buffer, "clear") == 0){
            system("clear");
            continue;
        }

        write_serial_port(fd, buffer, strlen(buffer));
        close(fd);
        return 0;
    }
}