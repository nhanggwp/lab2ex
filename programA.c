// program_a.c - Gửi lệnh qua FIFO, kiểm tra hợp lệ trước khi gửi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define FIFO_PATH "/tmp/thread_mgmt_pipe"

int is_number(const char *str)
{
    if (*str == '\0')
        return 0;
    for (int i = 0; str[i]; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int is_valid_command(const char *cmd)
{
    if (strcmp(cmd, "CREATE_THREAD") == 0)
        return 1;
    if (strcmp(cmd, "LIST_THREADS") == 0)
        return 1;
    if (strncmp(cmd, "KILL_THREAD ", 12) == 0)
    {
        const char *id_str = cmd + 12;
        return is_number(id_str);
    }
    return 0;
}

int main()
{
    // Mở FIFO ở chế độ ghi
    int fd = open("/tmp/thread_mgmt_pipe", O_RDWR);
    if (fd < 0)
    {
        perror("open FIFO");
        exit(EXIT_FAILURE);
    }

    char command[128];

    while (1)
    {
        printf("Enter command (CREATE_THREAD, LIST_THREADS, KILL_THREAD <id>): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        if (!is_valid_command(command))
        {
            printf("❌ Invalid command format. Try again.\n");
            continue;
        }

        if (write(fd, command, strlen(command)) < 0)
        {
            perror("write");
        }
    }

    close(fd);
    return 0;
}
